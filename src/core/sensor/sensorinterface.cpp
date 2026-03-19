#include "sensorinterface.h"
#include <QDateTime>
#include <QThread>
#include <QElapsedTimer>
#include <qobject.h>
#include <QTimer>

// 包含加载器头文件
#include "core/fileio/imuserialloader.h"
#include "core/fileio/gnssserialloader.h"

// 包含日志头文件
#include "core/common/logger.h"

// SensorBase基类实现
SensorBase::SensorBase(QObject *parent) : 
    QObject(parent),
    port_(""),
    baudrate_(115200),
    connected_(false),
    running_(false),
    thread_(nullptr),
    watchdogTimer_(nullptr),
    timeSyncDt_(0.0),
    timeSynced_(false)
{
    // 初始化线程
    thread_ = new QThread(this);
    // 注意：watchdogTimer_ 在子类中初始化和连接
}

SensorBase::~SensorBase()
{
    // 注意：watchdogTimer_ 在子类中停止和清理

    // 等待线程结束
    if (thread_ && thread_->isRunning()) {
        thread_->wait(1000); // 等待最多 1 秒
    }
    // 线程会被自动销毁，因为它的父对象是 this
}

/**
 * @brief 启动传感器线程
 * @return 是否启动成功
 */
bool SensorBase::startThread()
{
    if (!thread_ || thread_->isRunning()) {
        return false;
    }
    
    // 移动对象到线程：将传感器对象移动到线程中，确保在线程中运行
    // 这个步骤非常关键，卡了我一周TNND！！！！！！！！！！！！
    this->moveToThread(thread_);
    
    // 启动线程
    thread_->start();
    return true;
}

/**
 * @brief 停止传感器线程
 */
void SensorBase::stopThread()
{
    if (thread_ && thread_->isRunning()) {
        thread_->quit();
        // 等待线程结束，确保线程完全停止
        if (!thread_->wait(1000)) {
            LOG_WARN("Sensor thread did not stop within 1 second, forcing termination");
            thread_->terminate();
            thread_->wait();
        }
        LOG_INFO("Sensor thread stopped successfully");
    }
}

/**
 * @brief 传感器时钟方法，用于记录传感器上的时间
 * @return 传感器上的时间戳
 */
double SensorBase::clock()
{
    // 默认返回系统时间戳
    return static_cast<double>(time(nullptr));
}

void SensorBase::setSerialParams(const QString &port, int baudrate)
{
    port_ = port;
    baudrate_ = baudrate;
}

bool SensorBase::isConnected() const
{
    return connected_;
}

bool SensorBase::isRunning() const
{
    return running_;
}

QString SensorBase::getPort() const
{
    return port_;
}

int SensorBase::getBaudrate() const
{
    return baudrate_;
}

QList<QSerialPortInfo> SensorBase::getAvailableSerialPorts()
{
    LOG_INFO("Getting available serial ports");
    QList<QSerialPortInfo> availablePorts = QSerialPortInfo::availablePorts();
    LOG_INFO("Found %d available serial ports", availablePorts.size());
    for (const QSerialPortInfo& port : availablePorts) {
        LOG_DEBUG("Available port: %s, description: %s, manufacturer: %s", 
                 port.portName().toStdString().c_str(),
                 port.description().toStdString().c_str(),
                 port.manufacturer().toStdString().c_str());
    }
    return availablePorts;
}

/**
 * @brief 时间同步方法，为所有传感器同步时间
 * @param systemTimestamp 系统时间戳
 * @param utcTime UTC时间
 * @return 是否同步成功
 */
bool SensorBase::syncTime(double systemTimestamp, double localTime)
{
    // 计算系统时间与本地时间的差值
    double timeDifference = localTime - systemTimestamp;
    
    LOG_DEBUG("Sensor: Time difference calculated: %.3f seconds", timeDifference);
    
    // 设置时间同步的dt值
    setTimeSyncDt(timeDifference);
    setTimeSynced(true);
    
    LOG_INFO("Sensor: Time sync completed, dt=%.3f seconds", timeSyncDt_);
    
    return true;
}

/**
 * @brief 设置时间同步的dt值
 * @param dt 时间同步的dt值
 */
void SensorBase::setTimeSyncDt(double dt)
{
    timeSyncDt_ = dt;
    LOG_DEBUG("Sensor: Time sync dt updated to %.3f seconds", dt);
}

/**
 * @brief 设置时间同步状态
 * @param synced 时间是否已同步
 */
void SensorBase::setTimeSynced(bool synced)
{
    timeSynced_ = synced;
    LOG_DEBUG("Sensor: Time synced status updated to %d", synced);
}

/**
 * @brief 获取时间同步的dt值
 * @return 时间同步的dt值
 */
double SensorBase::getTimeSyncDt() const
{
    return timeSyncDt_;
}

/**
 * @brief 检查时间是否已同步
 * @return 时间是否已同步
 */
bool SensorBase::isTimeSynced() const
{
    return timeSynced_;
}

// ImuSensor 实现
ImuSensor::ImuSensor(QObject *parent) :
    SensorBase(parent),
    imuLoader_(nullptr),
    dataSendTimer_(nullptr),
    watchdogTimer_(nullptr)
{
    // 初始化数据发送定时器
    dataSendTimer_ = new QTimer(this);
    QObject::connect(dataSendTimer_, &QTimer::timeout, this, &ImuSensor::sendBufferedData);

    // 初始化看门狗定时器（此项目的采集线程为阻塞循环，不依赖 Qt 事件循环；
    // 所以不启动 QTimer 看门狗，而是在采集线程内做超时检测和重连。）
    watchdogTimer_ = new QTimer(this);
    QObject::connect(watchdogTimer_, &QTimer::timeout, this, &ImuSensor::onWatchdogTimeout);

    LOG_INFO("IMU sensor initialized");
}

ImuSensor::~ImuSensor()
{
    // 停止定时器
    if (dataSendTimer_) {
        dataSendTimer_->stop();
    }
    if (watchdogTimer_) {
        watchdogTimer_->stop();
    }
    
    // 清理 loader 资源
    if (imuLoader_) {
        delete imuLoader_;
        imuLoader_ = nullptr;
    }
}

bool ImuSensor::connect()
{
    LOG_INFO("Connecting to IMU: %s, baudrate: %d", port_.toStdString().c_str(), baudrate_);
    
    if (!port_.isEmpty()) {
        try {
            if (imuLoader_) {
                // 删除旧的loader
                delete imuLoader_;
                imuLoader_ = nullptr;
            }
            
            // 创建新的loader，使用当前的串口参数
            imuLoader_ = new ImuSerialLoader(port_.toStdString(), baudrate_, 200);
            if (imuLoader_->isOpen()) {
                connected_ = true;
                LOG_INFO("IMU connected successfully");
                return true;
            } else {
                LOG_Error("Failed to open IMU serial port");
                delete imuLoader_;
                imuLoader_ = nullptr;
                connected_ = false;
                return false;
            }
        } catch (const std::exception& e) {
            LOG_Error("Exception connecting to IMU: %s", e.what());
            delete imuLoader_;
            imuLoader_ = nullptr;
            connected_ = false;
            return false;
        }
    } else {
        LOG_Error("IMU port not specified");
        connected_ = false;
        return false;
    }
}

void ImuSensor::disconnect()
{
    LOG_INFO("Disconnecting IMU");
    
    if (imuLoader_) {
        delete imuLoader_;
        imuLoader_ = nullptr;
    }
    connected_ = false;
    running_ = false;
}

bool ImuSensor::start()
{
    LOG_INFO("Starting IMU data collection");
    
    if (!connected_) {
        LOG_Error("IMU not connected, cannot start");
        return false;
    }
    
    running_ = true;
    
    if (!thread_->isRunning()) {
        // 启动线程（会将对象移动到线程）
        if (startThread()) {
            // 先断开可能存在的旧连接
            QObject::disconnect(thread_, &QThread::started, this, &ImuSensor::imuDataCollectionThread);
            // 连接数据采集槽函数（使用直接连接，因为对象已在新线程中）
            QObject::connect(thread_, &QThread::started, this, &ImuSensor::imuDataCollectionThread, Qt::DirectConnection);
            
            LOG_INFO("ImuSensor thread started");
            return true;
        } else {
            LOG_Error("Failed to start ImuSensor thread");
            return false;
        }
    } else {
        LOG_INFO("ImuSensor thread already running");
        return true;
    }
}

void ImuSensor::stop()
{
    LOG_INFO("Stopping ImuSensor");
    running_ = false;
    
    if (thread_->isRunning()) {
        // 停止线程
        stopThread();
        LOG_INFO("ImuSensor thread stopped");
    }
}

void* ImuSensor::getLatestData() const
{
    if (imuLoader_) {
        try {
            IMU* imuData = new IMU(imuLoader_->next());
            return imuData;
        } catch (...) {
            return nullptr;
        }
    }
    return nullptr;
}

ImuSerialLoader* ImuSensor::getLoader() const
{
    return imuLoader_;
}

void ImuSensor::setSerialParams(const QString &port, int baudrate)
{
    LOG_INFO("Updating IMU serial parameters: port=%s, baud=%d", 
             port.toStdString().c_str(), baudrate);
    
    // 保存旧的IMU连接状态
    bool wasConnected = connected_;
    
    // 更新IMU串口参数
    SensorBase::setSerialParams(port, baudrate);
    
    // 如果IMU已经连接，重新连接IMU
    if (wasConnected) {
        LOG_INFO("ImuSensor was connected, reconnecting with new parameters");
        stop();
        disconnect();
        bool reconnected = connect();
        LOG_INFO("ImuSensor reconnection %s", reconnected ? "successful" : "failed");
        if (reconnected) {
            start();
        }
    } else {
        LOG_INFO("ImuSensor was not connected, parameters updated for future connection");
    }
    
    LOG_INFO("ImuSensor serial parameters updated successfully: port=%s, baud=%d", 
             port.toStdString().c_str(), baudrate);
}

void ImuSensor::imuDataCollectionThread()
{
    LOG_INFO("IMU data collection thread started");

    // 时间同步执行标志，用于控制只输出一次同步日志
    bool timeSyncLogExecuted = false;

    // 错误计数和重连参数
    int consecutiveErrorCount = 0;
    const int MAX_CONSECUTIVE_ERRORS = 50; // 允许的最大连续错误数
    const int RECONNECT_DELAY_MS = 500; // 重连前的延时

    // 缓冲区监控参数
    int noDataCount = 0; // 连续无数据计数
    const int MAX_NO_DATA_COUNT = 200; // 最大连续无数据次数（约2秒）
    qint64 lastBufferSize = 0;
    int bufferCheckCounter = 0;
    const int BUFFER_CHECK_INTERVAL = 100; // 每100次循环检查一次缓冲区

    // 初始化最后有效数据计时器（用于 1s 假死检测）
    lastValidDataTimer_.start();

    while (running_ && connected_ && imuLoader_) {
        try {
            // 检查 IMU 加载器是否打开
            if (!imuLoader_->isOpen()) {
                LOG_Error("IMU serial port closed unexpectedly, attempting to reconnect...");
                consecutiveErrorCount++;

                if (consecutiveErrorCount >= MAX_CONSECUTIVE_ERRORS) {
                    LOG_Error("Too many consecutive errors (%d), stopping IMU data collection", consecutiveErrorCount);
                    running_ = false;
                    connected_ = false;
                    break;
                }

                // 短暂延时后重试
                QThread::msleep(RECONNECT_DELAY_MS);
                continue;
            }

            // 定期检查缓冲区状态
            bufferCheckCounter++;
            if (bufferCheckCounter >= BUFFER_CHECK_INTERVAL) {
                bufferCheckCounter = 0;
                qint64 available = imuLoader_->bytesAvailable();

                // 检测缓冲区是否持续增长（数据积压）
                if (available > 4096) {
                    LOG_WARN("IMU buffer overflow detected (%lld bytes), clearing buffer", available);
                    imuLoader_->clearBuffer();
                    noDataCount = 0;
                    lastBufferSize = 0;
                } else if (available > 2048 && available >= lastBufferSize) {
                    LOG_WARN("IMU buffer growing (%lld bytes), potential data backlog", available);
                }
                lastBufferSize = available;
            }

            // 重置错误计数
            consecutiveErrorCount = 0;

            // 获取下一条 IMU 数据
            IMU imuData = imuLoader_->next();

            // 1s 假死检测：超过阈值无有效数据则执行恢复
            if (lastValidDataTimer_.isValid() && lastValidDataTimer_.elapsed() > 1000) {
                LOG_WARN("IMU watchdog(thread): no valid data for %lld ms, attempting reconnect", (long long)lastValidDataTimer_.elapsed());

                // 清缓冲（如果有积压）
                qint64 available = imuLoader_->bytesAvailable();
                if (available > 0) {
                    LOG_WARN("IMU watchdog(thread): %lld bytes available, clearing buffer", (long long)available);
                    imuLoader_->clearBuffer();
                }

                // 重连（删除旧 loader 再 new）
                const QString port = port_;
                const int baud = baudrate_;

                delete imuLoader_;
                imuLoader_ = nullptr;
                connected_ = false;

                QThread::msleep(100);

                if (!port.isEmpty()) {
                    imuLoader_ = new ImuSerialLoader(port.toStdString(), baud, 200);
                    if (imuLoader_->isOpen()) {
                        connected_ = true;
                        lastValidDataTimer_.restart();
                        LOG_INFO("IMU watchdog(thread): reconnect successful (port=%s, baud=%d)", port.toStdString().c_str(), baud);
                    } else {
                        LOG_Error("IMU watchdog(thread): reconnect failed (port=%s, baud=%d)", port.toStdString().c_str(), baud);
                        delete imuLoader_;
                        imuLoader_ = nullptr;
                        connected_ = false;
                        QThread::msleep(500);
                    }
                }

                continue;
            }

            // 如果 IMU 数据有效（time 不为 0），实时发送
            if (imuData.time > 0) {
                // 刷新“最后有效数据”计时器（看门狗使用）
                lastValidDataTimer_.restart();

                // 如果时间已同步，更新 gpstime、synced 标志和 dt 字段
                if (isTimeSynced()) {
                    imuData.gpstime = imuData.timestamp + getTimeSyncDt();
                    imuData.synced = true;
                    imuData.dt = getTimeSyncDt();

                    // 只输出一次同步日志
                    if (!timeSyncLogExecuted) {
                        LOG_INFO("ImuSensor: Time synchronized - system timestamp: %.3f, GPS time: %.3f, dt: %.3f", imuData.timestamp, imuData.gpstime, imuData.dt);
                        timeSyncLogExecuted = true;
                    }
                }

                // 实时发送数据
                emit imuDataReceived(imuData);
                noDataCount = 0; // 重置无数据计数

                // 有数据时短暂休眠
                QThread::msleep(1);
            } else {
                // 无效数据时增加计数
                noDataCount++;

                // 如果长时间没有有效数据，检查是否需要清理缓冲区
                if (noDataCount >= MAX_NO_DATA_COUNT) {
                    qint64 available = imuLoader_->bytesAvailable();
                    if (available > 1024) {
                        LOG_WARN("IMU: No valid data for %d cycles, but %lld bytes in buffer, clearing",
                                noDataCount, available);
                        imuLoader_->clearBuffer();
                    }
                    noDataCount = 0;
                }

                // 没有数据时休眠更长时间，避免空转
                QThread::msleep(10);
            }
        } catch (const std::exception& e) {
            LOG_Error("Exception in IMU data collection: %s", e.what());
            consecutiveErrorCount++;

            if (consecutiveErrorCount >= MAX_CONSECUTIVE_ERRORS) {
                LOG_Error("Too many consecutive exceptions (%d), stopping IMU data collection", consecutiveErrorCount);
                // 继续循环，但标记为错误状态
                consecutiveErrorCount = 0;
                QThread::msleep(RECONNECT_DELAY_MS);
            }
        } catch (...) {
            LOG_Error("Unknown exception in IMU data collection");
            consecutiveErrorCount++;

            if (consecutiveErrorCount >= MAX_CONSECUTIVE_ERRORS) {
                LOG_Error("Too many consecutive unknown exceptions (%d), stopping IMU data collection", consecutiveErrorCount);
                consecutiveErrorCount = 0;
                QThread::msleep(RECONNECT_DELAY_MS);
            }
        }
    }

    // 发送剩余的数据
    if (!imuDataBuffer_.empty()) {
        sendBufferedData();
    }

    LOG_INFO("IMU data collection thread stopped");
}

void ImuSensor::sendBufferedData()
{
    if (imuDataBuffer_.empty()) {
        return;
    }
    
    // 批量发送缓存的数据
    for (const IMU& imuData : imuDataBuffer_) {
        emit imuDataReceived(imuData);
    }
    
    LOG_DEBUG("IMU: Sent %zu buffered data frames", imuDataBuffer_.size());
    imuDataBuffer_.clear();
}

void ImuSensor::onWatchdogTimeout()
{
    // 看门狗逻辑在 imuDataCollectionThread() 内执行。
}

// GpsSensor 实现
GpsSensor::GpsSensor(QObject *parent) :
    SensorBase(parent),
    gnssLoader_(nullptr),
    validGgaCount_(0),
    dataSendTimer_(nullptr),
    watchdogTimer_(nullptr)
{
    // 初始化数据发送定时器
    dataSendTimer_ = new QTimer(this);
    QObject::connect(dataSendTimer_, &QTimer::timeout, this, &GpsSensor::sendBufferedData);

    // 初始化看门狗定时器（采集线程为阻塞循环；超时检测在采集线程内执行）
    watchdogTimer_ = new QTimer(this);
    QObject::connect(watchdogTimer_, &QTimer::timeout, this, &GpsSensor::onWatchdogTimeout);

    watchdogTimeoutStrikes_ = 0;

    LOG_INFO("GPS sensor initialized");
}

GpsSensor::~GpsSensor()
{
    // 停止定时器
    if (dataSendTimer_) {
        dataSendTimer_->stop();
    }
    if (watchdogTimer_) {
        watchdogTimer_->stop();
    }
    
    // 清理 loader 资源
    if (gnssLoader_) {
        delete gnssLoader_;
        gnssLoader_ = nullptr;
    }
}

bool GpsSensor::connect()
{
    LOG_INFO("Connecting to GPS: %s, baudrate: %d", port_.toStdString().c_str(), baudrate_);
    
    if (!port_.isEmpty()) {
        try {
            if (gnssLoader_) {
                // 删除旧的loader
                delete gnssLoader_;
                gnssLoader_ = nullptr;
            }
            
            // 创建新的loader，使用当前的串口参数
            gnssLoader_ = new GnssSerialLoader(port_.toStdString(), baudrate_);
            if (gnssLoader_->isOpen()) {
                connected_ = true;
                LOG_INFO("GPS connected successfully");
                return true;
            } else {
                LOG_Error("Failed to open GPS serial port");
                delete gnssLoader_;
                gnssLoader_ = nullptr;
                connected_ = false;
                return false;
            }
        } catch (const std::exception& e) {
            LOG_Error("Exception connecting to GPS: %s", e.what());
            delete gnssLoader_;
            gnssLoader_ = nullptr;
            connected_ = false;
            return false;
        }
    } else {
        LOG_Error("GPS port not specified");
        connected_ = false;
        return false;
    }
}

void GpsSensor::disconnect()
{
    LOG_INFO("Disconnecting GPS");
    
    if (gnssLoader_) {
        delete gnssLoader_;
        gnssLoader_ = nullptr;
    }
    connected_ = false;
    running_ = false;
}

bool GpsSensor::start()
{
    LOG_INFO("Starting GpsSensor");
    
    if (!connected_) {
        LOG_Error("GPS not connected, cannot start");
        return false;
    }
    
    running_ = true;
    
    if (!thread_->isRunning()) {
        // 启动线程（会将对象移动到线程）
        if (startThread()) {
            // 先断开可能存在的旧连接
            QObject::disconnect(thread_, &QThread::started, this, &GpsSensor::gpsDataCollectionThread);
            // 连接数据采集槽函数（使用直接连接，因为对象已在新线程中）
            QObject::connect(thread_, &QThread::started, this, &GpsSensor::gpsDataCollectionThread, Qt::DirectConnection);
            
            LOG_INFO("GpsSensor thread started");
            return true;
        } else {
            LOG_Error("Failed to start GpsSensor thread");
            return false;
        }
    } else {
        LOG_INFO("GpsSensor thread already running");
        return true;
    }
}

void GpsSensor::stop()
{
    LOG_INFO("Stopping GpsSensor");   
    running_ = false;
    
    if (thread_->isRunning()) {
        // 停止线程
        stopThread();
        LOG_INFO("GpsSensor thread stopped");
    }
}

void* GpsSensor::getLatestData() const
{
    if (gnssLoader_) {
        try {
            GNSS* gnssData = new GNSS(gnssLoader_->next());
            return gnssData;
        } catch (...) {
            return nullptr;
        }
    }
    return nullptr;
}

GnssSerialLoader* GpsSensor::getLoader() const
{
    return gnssLoader_;
}

void GpsSensor::setSerialParams(const QString &port, int baudrate)
{
    LOG_INFO("Updating GPS serial parameters: port=%s, baud=%d", 
             port.toStdString().c_str(), baudrate);
    
    // 保存旧的GPS连接状态
    bool wasConnected = connected_;
    
    // 更新GPS串口参数
    SensorBase::setSerialParams(port, baudrate);
    
    // 如果GPS已经连接，重新连接GPS
    if (wasConnected) {
        LOG_INFO("GPS was connected, reconnecting with new parameters");
        stop();
        disconnect();
        bool reconnected = connect();
        LOG_INFO("GPS reconnection %s", reconnected ? "successful" : "failed");
        if (reconnected) {
            start();
        }
    } else {
        LOG_INFO("GPS was not connected, parameters updated for future connection");
    }
    
    LOG_INFO("GPS serial parameters updated successfully: port=%s, baud=%d", 
             port.toStdString().c_str(), baudrate);
}

/**
 * @brief 时间同步方法，为所有传感器同步时间
 * @param gnssData GNSS数据
 */
void GpsSensor::syncTime(const GNSS& gnssData)
{
    // 检查数据是否有效
    if (!gnssData.isvalid) {
        return;
    }
    
    // 如果已经同步过，不再进行同步
    if (isTimeSynced()) {
        return;
    }
    
    // 记录卫星时间（UTC时间）与本地时间的差值
    double systemTimestamp = gnssData.timestamp;
    double localtime = gnssData.time;
    double timeDifference = localtime - systemTimestamp;
    
    // 增加有效的GGA消息计数
    validGgaCount_++;
    
    // 记录本地时间差值
    if (validGgaCount_ <= 10) {
        localTimeDifferences_.push_back(timeDifference);
        LOG_DEBUG("GPS: Recording local time difference %d: %.3f", validGgaCount_, timeDifference);
    }
    
    // 当收集到10个有效的GGA消息后，计算平均差值作为同步用的dt值
    if (validGgaCount_ == 10) {
        // 计算平均差值
        double sum = 0.0;
        for (double diff : localTimeDifferences_) {
            sum += diff;
        }
        double avgTimeDifference = sum / localTimeDifferences_.size();
        
        // 计算平均本地时间
        double avgLocalTime = systemTimestamp + avgTimeDifference;
        
        // 使用父类的syncTime方法设置时间同步
        SensorBase::syncTime(systemTimestamp, avgLocalTime);
        
        // 发出时间同步完成信号
        emit timeSyncCompleted(getTimeSyncDt());
        
        LOG_INFO("GPS: Time sync completed, dt=%.3f seconds", getTimeSyncDt());
        LOG_INFO("GPS: Collected %d valid GGA messages for time sync", localTimeDifferences_.size());
    }
}

void GpsSensor::gpsDataCollectionThread()
{
    LOG_INFO("GPS data collection thread started");

    // 错误计数和重连参数
    int consecutiveErrorCount = 0;
    const int MAX_CONSECUTIVE_ERRORS = 30;
    const int RECONNECT_DELAY_MS = 500;

    // 缓冲区监控参数
    int noDataCount = 0;
    const int MAX_NO_DATA_COUNT = 100;
    qint64 lastBufferSize = 0;
    int bufferCheckCounter = 0;
    const int BUFFER_CHECK_INTERVAL = 50;

    // 性能监控参数 - 改为按有效数据计数
    int validDataCount = 0;
    int invalidDataCount = 0;
    int totalLoopCount = 0;
    QElapsedTimer perfTimer;
    QElapsedTimer sessionTimer; // 会话总时间
    perfTimer.start();
    sessionTimer.start();

    // 上次输出性能日志时的有效数据数
    int lastReportedValidCount = 0;
    const int REPORT_INTERVAL = 500; // 每500个有效数据输出一次

    // 初始化最后有效数据计时器（用于 1s 假死检测）
    lastValidDataTimer_.start();

    // GPS 看门狗策略：每次有效数据重置计时；若连续 2 次检查均超过 1s 无有效数据，则重连。
    watchdogTimeoutStrikes_ = 0;

    while (running_ && connected_ && gnssLoader_) {
        try {
            totalLoopCount++;

            // 检查 GPS 加载器是否打开
            if (!gnssLoader_->isOpen()) {
                LOG_Error("GPS serial port closed unexpectedly, attempting to reconnect...");
                consecutiveErrorCount++;

                if (consecutiveErrorCount >= MAX_CONSECUTIVE_ERRORS) {
                    LOG_Error("Too many consecutive errors (%d), stopping GPS data collection", consecutiveErrorCount);
                    running_ = false;
                    connected_ = false;
                    break;
                }

                QThread::msleep(RECONNECT_DELAY_MS);
                continue;
            }

            // 定期检查缓冲区状态
            bufferCheckCounter++;
            if (bufferCheckCounter >= BUFFER_CHECK_INTERVAL) {
                bufferCheckCounter = 0;
                qint64 available = gnssLoader_->bytesAvailable();

                if (available > 4096) {
                    LOG_WARN("GPS buffer overflow detected (%lld bytes), clearing buffer", available);
                    gnssLoader_->clearBuffer();
                    noDataCount = 0;
                    lastBufferSize = 0;
                } else if (available > 2048 && available >= lastBufferSize) {
                    LOG_WARN("GPS buffer growing (%lld bytes), potential data backlog", available);
                }
                lastBufferSize = available;
            }

            consecutiveErrorCount = 0;

            // 获取下一条 GPS 数据
            GNSS gnssData = gnssLoader_->next();

            // 1s 看门狗（你选择：连续 2 次检查都超过 1s 才重连）
            if (lastValidDataTimer_.isValid() && lastValidDataTimer_.elapsed() > 1000) {
                watchdogTimeoutStrikes_++;
                if (watchdogTimeoutStrikes_ >= 2) {
                    LOG_WARN("GPS watchdog(thread): no valid data for %lld ms (strikes=%d), attempting reconnect",  
                             (long long)lastValidDataTimer_.elapsed(), watchdogTimeoutStrikes_);

                    // 先清理缓冲（如果有积压）
                    qint64 available = gnssLoader_->bytesAvailable();
                    if (available > 0) {
                        LOG_WARN("GPS watchdog(thread): %lld bytes available, clearing buffer", (long long)available);
                        gnssLoader_->clearBuffer();
                    }

                    const QString port = port_;
                    const int baud = baudrate_;

                    delete gnssLoader_;
                    gnssLoader_ = nullptr;
                    connected_ = false;

                    QThread::msleep(100);

                    if (!port.isEmpty()) {
                        gnssLoader_ = new GnssSerialLoader(port.toStdString(), baud);
                        if (gnssLoader_->isOpen()) {
                            connected_ = true;
                            lastValidDataTimer_.restart();
                            watchdogTimeoutStrikes_ = 0;
                            LOG_INFO("GPS watchdog(thread): reconnect successful (port=%s, baud=%d)", port.toStdString().c_str(), baud);
                        } else {
                            LOG_Error("GPS watchdog(thread): reconnect failed (port=%s, baud=%d)", port.toStdString().c_str(), baud);
                            delete gnssLoader_;
                            gnssLoader_ = nullptr;
                            connected_ = false;
                            QThread::msleep(500);
                        }
                    }

                    continue;
                }
            } else {
                watchdogTimeoutStrikes_ = 0;
            }

            // 进行时间同步
            syncTime(gnssData);

            // 如果数据有效，实时发送
            if (gnssData.isvalid) {
                // 刷新"最后有效数据"计时器（看门狗使用）
                lastValidDataTimer_.restart();
                watchdogTimeoutStrikes_ = 0;

                // 实时发送数据（不再做节流，确保 UI 侧能按 20Hz 刷新）
                emit gnssDataReceived(gnssData);

                validDataCount++;
                noDataCount = 0;

                // 每500个有效数据输出一次性能统计
                if (validDataCount - lastReportedValidCount >= REPORT_INTERVAL) {
                    qint64 elapsed = perfTimer.elapsed();
                    qint64 sessionElapsed = sessionTimer.elapsed();

                    // 计算统计数据
                    double avgLoopTime = static_cast<double>(elapsed) / totalLoopCount;
                    double dataRate = validDataCount * 1000.0 / sessionElapsed;
                    double validRate = static_cast<double>(validDataCount) / totalLoopCount * 100.0;

                    LOG_INFO("GPS Performance: Valid=%d, Loops=%d, ValidRate=%f, AvgLoop=%f ms, Rate=%f Hz, Buffer=%lld bytes",
                             validDataCount, totalLoopCount, validRate, avgLoopTime, dataRate, (long long)lastBufferSize);

                    // 重置计数器
                    lastReportedValidCount = validDataCount;
                    perfTimer.restart();
                    totalLoopCount = 0;
                }

                QThread::msleep(1);
            } else {
                invalidDataCount++;
                noDataCount++;

                if (noDataCount >= MAX_NO_DATA_COUNT) {
                    qint64 available = gnssLoader_->bytesAvailable();
                    if (available > 1024) {
                        LOG_WARN("GPS: No valid data for %d cycles, but %lld bytes in buffer, clearing",
                                noDataCount, available);
                        gnssLoader_->clearBuffer();
                    }
                    noDataCount = 0;
                }

                QThread::msleep(10);
            }

        } catch (const std::exception& e) {
            LOG_Error("Exception in GPS data collection: %s", e.what());
            consecutiveErrorCount++;

            if (consecutiveErrorCount >= MAX_CONSECUTIVE_ERRORS) {
                LOG_Error("Too many consecutive exceptions (%d), stopping GPS data collection", consecutiveErrorCount);
                consecutiveErrorCount = 0;
                QThread::msleep(RECONNECT_DELAY_MS);
            }
        } catch (...) {
            LOG_Error("Unknown exception in GPS data collection");
            consecutiveErrorCount++;

            if (consecutiveErrorCount >= MAX_CONSECUTIVE_ERRORS) {
                LOG_Error("Too many consecutive unknown exceptions (%d), stopping GPS data collection", consecutiveErrorCount);
                consecutiveErrorCount = 0;
                QThread::msleep(RECONNECT_DELAY_MS);
            }
        }
    }

    // 发送剩余的数据
    if (!gnssDataBuffer_.empty()) {
        sendBufferedData();
    }

    // 输出最终统计
    qint64 totalTime = sessionTimer.elapsed();
    double avgRate = validDataCount * 1000.0 / totalTime;
    LOG_INFO("GPS data collection thread stopped - Total: Valid=%d, Invalid=%d, Time=%.1fs, AvgRate=%.1f Hz",
             validDataCount, invalidDataCount, totalTime / 1000.0, avgRate);
}

void GpsSensor::sendBufferedData()
{
    if (gnssDataBuffer_.empty()) {
        return;
    }
    
    // 批量发送缓存的数据
    for (const GNSS& gnssData : gnssDataBuffer_) {
        emit gnssDataReceived(gnssData);
    }
    
    LOG_DEBUG("GPS: Sent %zu buffered data frames", gnssDataBuffer_.size());
    gnssDataBuffer_.clear();
}

void GpsSensor::onWatchdogTimeout()
{
    // 看门狗逻辑在 gpsDataCollectionThread() 内执行。
}

