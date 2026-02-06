#include "sensorinterface.h"
#include <QDateTime>
#include <QThread>
#include <sstream>
#include <iomanip>

// 包含日志头文件
#include "core/common/logger.h"

/**
 * @brief 构造函数，初始化默认参数
 * @param parent 父对象指针
 */
SensorInterface::SensorInterface(QObject *parent) : QObject(parent)
{


    imuRunning_ = false;
    gnssRunning_ = false;
    imuConnected_ = false;
    gnssConnected_ = false;
    
    // 初始化串口参数
    imuPort_ = "";
    imuBaudrate_ = 115200;
    gnssPort_ = "";
    gnssBaudrate_ = 115200;
    
    // 初始化线程
    imuThread_ = new QThread(this);
    gnssThread_ = new QThread(this);
    
    // 初始化KF-GINS加载器
    imuLoader_ = nullptr;
    gnssLoader_ = nullptr;
    
    // 初始化原始数据缓存
    rawImuData_ = "";
    rawGnssData_ = "";
    
    // 记录初始化日志（日志系统已在MainWindow中初始化）
    LOG_INFO("%s", "SensorInterface initialized");
}

/**
 * @brief 析构函数
 */
SensorInterface::~SensorInterface()
{

    // 断开所有传感器连接
    disconnectIMU();
    disconnectGPS();
    
    // 清理线程
    if (imuThread_) {
        delete imuThread_;
    }
    if (gnssThread_) {
        delete gnssThread_;
    }
}

/**
 * @brief 启动IMU数据采集
 * @return 是否启动成功
 */
bool SensorInterface::startIMU()
{
    LOG_INFO("Starting IMU data collection...");
    
    if (!imuConnected_) {
        LOG_WARN("IMU is not connected, cannot start data collection");
        return false;
    }
    
    // 设置运行标志
    imuRunning_ = true;
    
    if (!imuThread_->isRunning()) {
        // 确保之前的连接已经断开
        disconnect(imuThread_, &QThread::started, this, &SensorInterface::imuDataCollectionThread);
        // 连接IMU数据采集槽函数
        connect(imuThread_, &QThread::started, this, &SensorInterface::imuDataCollectionThread, Qt::DirectConnection);
        imuThread_->start();
        LOG_INFO("IMU data collection thread started");
        return true;
    } else {
        LOG_INFO("IMU data collection thread is already running");
        return true;
    }
}

/**
 * @brief 停止IMU数据采集
 */
void SensorInterface::stopIMU()
{
    LOG_INFO("Stopping IMU data collection...");
    
    // 首先设置运行标志为false，使线程能够退出循环
    imuRunning_ = false;
    
    // 等待线程停止
    if (imuThread_->isRunning()) {
        // 等待线程退出，设置超时时间为2秒
        if (!imuThread_->wait(500)) {
            LOG_WARN("IMU thread did not stop within timeout, forcing termination");
            // 强制终止线程
            imuThread_->terminate();
            imuThread_->wait();
        }
        LOG_INFO("IMU data collection thread stopped");
    }
    
    // 重置IMU加载器
    if (imuLoader_) {
        imuLoader_.reset();
        imuConnected_ = false;
        LOG_INFO("IMU loader reset");
    }
}

/**
 * @brief 启动GPS数据采集
 * @return 是否启动成功
 */
bool SensorInterface::startGPS()
{
    LOG_INFO("Starting GPS data collection...");
    
    if (!gnssConnected_) {
        LOG_WARN("GPS is not connected, cannot start data collection");
        return false;
    }
    
    // 设置运行标志
    gnssRunning_ = true;
    
    if (!gnssThread_->isRunning()) {
        // 确保之前的连接已经断开
        disconnect(gnssThread_, &QThread::started, this, &SensorInterface::gnssDataCollectionThread);
        // 连接GPS数据采集槽函数
        connect(gnssThread_, &QThread::started, this, &SensorInterface::gnssDataCollectionThread, Qt::DirectConnection);
        gnssThread_->start();
        LOG_INFO("GPS data collection thread started");
        return true;
    } else {
        LOG_INFO("GPS data collection thread is already running");
        return true;
    }
}

/**
 * @brief 停止GPS数据采集
 */
void SensorInterface::stopGPS()
{
    LOG_INFO("Stopping GPS data collection...");
    
    // 首先设置运行标志为false，使线程能够退出循环
    gnssRunning_ = false;
    
    // 等待线程停止
    if (gnssThread_->isRunning()) {
        // 等待线程退出，设置超时时间为2秒
        if (!gnssThread_->wait(500)) {
            LOG_WARN("GPS thread did not stop within timeout, forcing termination");
            // 强制终止线程
            gnssThread_->terminate();
            gnssThread_->wait();
        }
        LOG_INFO("GPS data collection thread stopped");
    }
    
    // 重置GPS加载器
    if (gnssLoader_) {
        gnssLoader_.reset();
        gnssConnected_ = false;
        LOG_INFO("GPS loader reset");
    }
}



/**
 * @brief 连接IMU模块
 * @return 是否连接成功
 */
bool SensorInterface::connectIMU()
{
    LOG_INFO("Connecting to IMU module...");
    
    // 保存旧的IMU连接状态
    bool oldImuConnected = imuConnected_;
    
    // 初始化IMU加载器
    if (!imuPort_.isEmpty()) {
        try {
            LOG_INFO("Initializing IMU loader: %s, baudrate: %d", imuPort_.toStdString().c_str(), imuBaudrate_);
            imuLoader_ = std::make_unique<ImuSerialLoader>(imuPort_.toStdString(), imuBaudrate_);
            imuConnected_ = imuLoader_->isOpen();
            if (!imuConnected_) {
                LOG_ERROR("Failed to open IMU serial port: %s", imuPort_.toStdString().c_str());
            } else {
                LOG_INFO("IMU serial port opened successfully: %s, baudrate: %d", imuPort_.toStdString().c_str(), imuBaudrate_);
            }
        } catch (const std::exception& e) {
            LOG_ERROR("Error initializing IMU loader: %s", e.what());
            imuConnected_ = false;
        }
    } else {
        LOG_INFO("No IMU port specified");
        imuConnected_ = false;
    }
    
    
    // 记录连接状态变化
    if (imuConnected_ != oldImuConnected) {
        LOG_INFO("IMU connection status changed: %s", imuConnected_ ? "Connected" : "Disconnected");
    }

    // 如果成功连接，发送IMU数据更新信号
    if (imuConnected_) {
        // 连接成功时不需要发送数据信号，因为还没有实际数据
    }
    
    return imuConnected_;
}

/**
 * @brief 断开IMU模块连接
 */
void SensorInterface::disconnectIMU()
{
    LOG_INFO("Disconnecting from IMU module...");
    
    // 保存旧的IMU连接状态
    bool wasImuConnected = imuConnected_;
    
    // 清理IMU加载器
    if (imuLoader_) {
        imuLoader_.reset();
        imuConnected_ = false;
        LOG_INFO("IMU loader reset");
    }
    
    // 记录连接状态变化
    if (wasImuConnected) {
        LOG_INFO("IMU connection status changed: Disconnected");
        // 断开连接后不需要发送数据信号，因为连接状态变化应该通过其他方式通知UI
    }
    
    LOG_INFO("IMU disconnected");
}

/**
 * @brief 连接GPS模块
 * @return 是否连接成功
 */
bool SensorInterface::connectGPS()
{
    LOG_INFO("Connecting to GPS module...");
    
    // 保存旧的GPS连接状态
    bool oldGnssConnected = gnssConnected_;
    
    // 初始化GNSS加载器
    if (!gnssPort_.isEmpty()) {
        try {
            LOG_INFO("Initializing GNSS loader: %s, baudrate: %d", gnssPort_.toStdString().c_str(), gnssBaudrate_);
            gnssLoader_ = std::make_unique<GnssSerialLoader>(gnssPort_.toStdString(), gnssBaudrate_);
            gnssConnected_ = gnssLoader_->isOpen();
            if (!gnssConnected_) {
                LOG_ERROR("Failed to open GNSS serial port: %s", gnssPort_.toStdString().c_str());
            } else {
                LOG_INFO("GNSS serial port opened successfully: %s, baudrate: %d", gnssPort_.toStdString().c_str(), gnssBaudrate_);
            }
        } catch (const std::exception& e) {
            LOG_ERROR("Error initializing GNSS loader: %s", e.what());
            gnssConnected_ = false;
        }
    } else {
        LOG_INFO("No GNSS port specified");
        gnssConnected_ = false;
    }
    
    
    // 记录连接状态变化
    if (gnssConnected_ != oldGnssConnected) {
        LOG_INFO("GNSS connection status changed: %s", gnssConnected_ ? "Connected" : "Disconnected");
    }

    // 如果成功连接，发送GPS数据更新信号
    if (gnssConnected_) {
        // 连接成功时不需要发送数据信号，因为还没有实际数据
    }

    
    return gnssConnected_;
}

/**
 * @brief 断开GPS模块连接
 */
void SensorInterface::disconnectGPS()
{
    LOG_INFO("Disconnecting from GPS module...");
    
    // 保存旧的GPS连接状态
    bool wasGnssConnected = gnssConnected_;
    
    // 清理GNSS加载器
    if (gnssLoader_) {
        gnssLoader_.reset();
        gnssConnected_ = false;
        LOG_INFO("GNSS loader reset");
    }
    
    // 记录连接状态变化
    if (wasGnssConnected) {
        LOG_INFO("GNSS connection status changed: Disconnected");
        // 断开连接后不需要发送数据信号，因为连接状态变化应该通过其他方式通知UI
    }
    
    LOG_INFO("GPS disconnected");
}



/**
 * @brief 获取IMU原始数据
 * @return IMU原始数据
 */
QString SensorInterface::getRawImuData() const
{
    return rawImuData_;
}

/**
 * @brief 获取GNSS原始数据
 * @return GNSS原始数据
 */
QString SensorInterface::getRawGnssData() const
{
    return rawGnssData_;
}

/**
 * @brief 设置IMU串口参数
 * @param imuPort IMU串口端口
 * @param imuBaudrate IMU串口波特率
 */
void SensorInterface::setImuSerialParams(const QString &imuPort, int imuBaudrate)
{
    LOG_INFO("Updating IMU serial parameters: port=%s, baud=%d", 
             imuPort.toStdString().c_str(), imuBaudrate);
    
    // 保存旧的IMU连接状态
    bool wasImuConnected = imuConnected_;
    
    // 更新IMU串口参数
    imuPort_ = imuPort;
    imuBaudrate_ = imuBaudrate;
    
    // 如果IMU已经连接，重新连接IMU
    if (wasImuConnected) {
        LOG_INFO("IMU was connected, reconnecting with new parameters");
        disconnectIMU();
        bool reconnected = connectIMU();
        LOG_INFO("IMU reconnection %s", reconnected ? "successful" : "failed");
        
        // 重新连接后不需要发送数据信号，因为还没有实际数据
        if (reconnected) {
            // 重新连接成功时不需要发送数据信号，等待实际数据到来时再发送
        }
    } else {
        LOG_INFO("IMU was not connected, parameters updated for future connection");
    }
    
    LOG_INFO("IMU serial parameters updated successfully: port=%s, baud=%d", 
             imuPort.toStdString().c_str(), imuBaudrate);
}

/**
 * @brief 设置GPS串口参数
 * @param gnssPort GNSS串口端口
 * @param gnssBaudrate GNSS串口波特率
 */
void SensorInterface::setGpsSerialParams(const QString &gnssPort, int gnssBaudrate)
{
    LOG_INFO("Updating GPS serial parameters: port=%s, baud=%d", 
             gnssPort.toStdString().c_str(), gnssBaudrate);
    
    // 保存旧的GPS连接状态
    bool wasGnssConnected = gnssConnected_;
    
    // 更新GPS串口参数
    gnssPort_ = gnssPort;
    gnssBaudrate_ = gnssBaudrate;
    
    // 如果GPS已经连接，重新连接GPS
    if (wasGnssConnected) {
        LOG_INFO("GPS was connected, reconnecting with new parameters");
        disconnectGPS();
        bool reconnected = connectGPS();
        LOG_INFO("GPS reconnection %s", reconnected ? "successful" : "failed");
        
        // 重新连接后不需要发送数据信号，因为还没有实际数据
        if (reconnected) {
            // 重新连接成功时不需要发送数据信号，等待实际数据到来时再发送
        }
    } else {
        LOG_INFO("GPS was not connected, parameters updated for future connection");
    }
    
    LOG_INFO("GPS serial parameters updated successfully: port=%s, baud=%d", 
             gnssPort.toStdString().c_str(), gnssBaudrate);
}



/**
 * @brief 获取IMU加载器指针
 * @return IMU加载器指针
 */
ImuSerialLoader* SensorInterface::getImuLoader() const
{
    return imuLoader_.get();
}

/**
 * @brief 获取GNSS加载器指针
 * @return GNSS加载器指针
 */
GnssSerialLoader* SensorInterface::getGnssLoader() const
{
    return gnssLoader_.get();
}

/**
 * @brief 检查IMU是否连接
 * @return IMU是否连接
 */
bool SensorInterface::isImuConnected() const
{
    return imuConnected_;
}

/**
 * @brief 检查GNSS是否连接
 * @return GNSS是否连接
 */
bool SensorInterface::isGnssConnected() const
{
    return gnssConnected_;
}

/**
 * @brief IMU数据采集线程函数
 * @note 在独立线程中运行，实时读取和处理IMU数据
 */
void SensorInterface::imuDataCollectionThread()
{
    LOG_INFO("IMU data collection thread started");
    
    while (imuRunning_ && imuConnected_ && imuLoader_) {
        try {
            // 实时读取IMU数据
            const IMU& imuData = imuLoader_->next();
            
            // 更新最新IMU数据
            latestImuData_ = imuData;
            
            // 构建原始数据字符串
            std::stringstream ss;
            ss << std::fixed << std::setprecision(3);
            ss << "IMU Data: time=" << imuData.time 
               << " acc=\"[" << imuData.dvel[0]/imuData.dt << ", " << imuData.dvel[1]/imuData.dt << ", " << imuData.dvel[2]/imuData.dt << "]\""
               << " gyro=\"[" << imuData.dtheta[0]/imuData.dt * 180.0/M_PI << ", " << imuData.dtheta[1]/imuData.dt * 180.0/M_PI << ", " << imuData.dtheta[2]/imuData.dt * 180.0/M_PI << "]\""
               << " mag=\"[" << imuData.magnetic_field[0] << ", " << imuData.magnetic_field[1] << ", " << imuData.magnetic_field[2] << "]\""
               << " temp=" << imuData.temperature
               << " mag_heading=" << imuData.magnetic_heading
               << " true_heading=" << imuData.true_heading;
            rawImuData_ = QString::fromStdString(ss.str());
            
            // 记录IMU数据日志
            LOG_DEBUG("IMU data received: time=%f", imuData.time);
            
            // 发送IMU数据更新信号
            emit imuDataReceived(imuData);
        } catch (const std::exception& e) {
            LOG_ERROR("Error reading IMU data: %s", e.what());
            // 继续尝试，不中断数据采集
            QThread::msleep(10); // 短暂休眠，避免错误时CPU占用过高
        }
    }
    
    LOG_INFO("IMU data collection thread stopped");
}

/**
 * @brief GPS数据采集线程函数
 * @note 在独立线程中运行，实时读取和处理GPS数据
 */
void SensorInterface::gnssDataCollectionThread()
{
    LOG_INFO("GPS data collection thread started");
    
    while (gnssRunning_ && gnssConnected_ && gnssLoader_) {
        try {
            // 实时读取GPS数据
            const GNSS& gnssData = gnssLoader_->next();
            
            // 更新最新GNSS数据
            latestGnssData_ = gnssData;
            
            // 构建原始数据字符串
            std::stringstream ss;
            ss << std::fixed << std::setprecision(7);
            ss << "GNSS Data: time=" << gnssData.time 
               << " pos=\"[" << gnssData.blh[0] << ", " << gnssData.blh[1] << ", " << gnssData.blh[2] << "]\""
               << " vel=\"[" << gnssData.vel[0] << ", " << gnssData.vel[1] << ", " << gnssData.vel[2] << "]\""
               << " hdop=" << gnssData.hdop 
               << " vdop=" << gnssData.vdop 
               << " pdop=" << gnssData.pdop 
               << " used_sv=" << gnssData.used_sv 
               << " visible_sv=" << gnssData.visible_sv
               << " fix_mode=" << gnssData.fix_mode
               << " quality=" << gnssData.quality
               << " valid=" << (gnssData.isvalid ? "true" : "false");
            rawGnssData_ = QString::fromStdString(ss.str());
            
            // 判断GPS数据是否有效
            bool isValidData = false;
            
            // 检查isvalid字段
            if (gnssData.isvalid) {
                // 检查经纬度是否有效（非零值）
                if (gnssData.blh[0] != 0.0 || gnssData.blh[1] != 0.0) {
                    // 检查卫星数是否合理
                    if (gnssData.used_sv >= 4) {
                        // 检查定位质量
                        if (gnssData.quality >= 1) {
                            isValidData = true;
                        }
                    }
                }
            }
            
            if (isValidData) {
                // 记录GPS数据日志
                LOG_DEBUG("GNSS data received: time=%f, pos=[%.7f, %.7f, %.3f], valid=%s", 
                         gnssData.time, 
                         gnssData.blh[0], gnssData.blh[1], gnssData.blh[2],
                         gnssData.isvalid ? "true" : "false");
                
                // 发送GPS数据更新信号
                emit gnssDataReceived(gnssData);
            } else {
                // 未解析到有效GPS数据
                LOG_INFO("未解析到GPS数据: 有效标志=%s, 经纬度=[%.7f, %.7f], 卫星数=%d, 定位质量=%d", 
                         gnssData.isvalid ? "true" : "false",
                         gnssData.blh[0], gnssData.blh[1],
                         gnssData.used_sv,
                         gnssData.quality);
            }
        } catch (const std::exception& e) {
            LOG_ERROR("Error reading GNSS data: %s", e.what());
            // 继续尝试，不中断数据采集
            QThread::msleep(10); // 短暂休眠，避免错误时CPU占用过高
        }
    }
    
    LOG_INFO("GPS data collection thread stopped");
}



/**
 * @brief 获取系统中所有可用的串口列表
 * @return 可用串口列表
 */
QList<QSerialPortInfo> SensorInterface::getAvailableSerialPorts()
{
    LOG_INFO("Getting available serial ports...");
    
    // 获取系统中所有可用的串口
    QList<QSerialPortInfo> availablePorts = QSerialPortInfo::availablePorts();
    
    // 记录串口信息
    LOG_INFO("Found %d available serial ports:", availablePorts.size());
    for (const QSerialPortInfo& port : availablePorts) {
        LOG_INFO("  Port: %s, Description: %s, Manufacturer: %s", 
                 port.portName().toStdString().c_str(),
                 port.description().toStdString().c_str(),
                 port.manufacturer().toStdString().c_str());
    }
    
    return availablePorts;
}

/**
 * @brief 获取最新的IMU数据
 * @return 最新的IMU数据
 */
IMU SensorInterface::getLatestImuData() const
{
    return latestImuData_;
}

/**
 * @brief 获取最新的GNSS数据
 * @return 最新的GNSS数据
 */
GNSS SensorInterface::getLatestGnssData() const
{
    return latestGnssData_;
}