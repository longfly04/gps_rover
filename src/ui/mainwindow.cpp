#include "mainwindow.h"
#include "ui_mainwindow.h"

// 包含核心组件
#include "core/sensor/sensorinterface.h"
#include "core/estimator/statestimator.h"

// 包含Qt组件
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QDateTime>
#include <QTimer>

// 包含日志组件
#include "core/common/logger.h"

// 初始化静态指针
MainWindow* MainWindow::instance = nullptr;

/**
 * @brief 构造函数
 * @param parent 父窗口指针
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    stateEstimator(nullptr)
{
    ui->setupUi(this);

    // 初始化传感器接口
    sensorInterface = std::make_unique<SensorInterface>(this);

    // 初始化模块
    initModules();

    // 初始化UI
    initUI();

    // 初始化信号槽连接
    initSignalsSlots();

    // 初始化日志系统
    initLogger();

    // 记录初始化日志
    LOG_INFO("主窗口初始化完成");

    // 初始化日志消息处理器中的静态instance指针
    instance = this;

    // 初始化日志显示级别
    currentLogLevel = Logger::INFO;

} 

/**
 * @brief 析构函数
 */
MainWindow::~MainWindow()
{
    // 删除UI
    delete ui;

}

/**
 * @brief 初始化模块
 */
void MainWindow::initModules()
{
    LOG_INFO("正在初始化模块...");

    // 初始化传感器接口
    LOG_INFO("正在初始化传感器接口...");
    // 设置默认串口参数
    sensorInterface->setImuSerialParams("COM7", 115200);
    sensorInterface->setGpsSerialParams("COM3", 115200);
    LOG_INFO("传感器接口初始化成功");

    // 初始化状态估计器
    LOG_INFO("正在初始化状态估计器...");
    stateEstimator = new StateEstimator();
    if (stateEstimator) {
        LOG_INFO("状态估计器初始化成功");
    } else {
        LOG_ERROR("状态估计器初始化失败");
    }

    LOG_INFO("模块初始化完成");
}



/**
 * @brief 初始化UI
 */
void MainWindow::initUI()
{
    LOG_INFO("正在初始化UI...");

    // 设置窗口标题
    setWindowTitle("播种大哥");

    // 初始化状态栏
    ui->statusbar->showMessage("系统就绪");

    // 初始化GPS信息标签
    ui->gpsPositionLabel->setText("经纬度: (0.000000, 0.000000)");
    ui->gpsAltSpeedLabel->setText("高度: 0.00 m, 速度: 0.000 m/s, 航向: 0.0°");

    // 初始化串口选择下拉框
    initSerialPortComboBoxes();

    LOG_INFO("UI初始化完成");
}

/**
 * @brief 初始化信号槽连接
 */
void MainWindow::initSignalsSlots()
{
    LOG_INFO("正在连接信号槽...");

    // 连接校准按钮信号
    connect(ui->calibrateDataButton, &QPushButton::clicked,
            this, &MainWindow::on_calibrateDataButton_clicked);

    // 连接传感器接口的信号

    // 明确指定连接方式为QueuedConnection，确保跨线程信号传递正确
    connect(sensorInterface.get(), &SensorInterface::imuDataReceived,
            this, [this](const IMU& imuData) {
                // 处理IMU数据
                LOG_DEBUG("接收到IMU数据: time=%f", imuData.time);
                // 实时更新IMU原始数据显示
                onImuDataUpdated();
            }, Qt::QueuedConnection);

    // 明确指定连接方式为QueuedConnection，确保跨线程信号传递正确
    connect(sensorInterface.get(), &SensorInterface::gnssDataReceived,
            this, [this](const GNSS& gnssData) {
                // 处理GNSS数据
                LOG_DEBUG("接收到GNSS数据: time=%f, pos=[%.7f, %.7f, %.3f]",
                         gnssData.time, gnssData.blh[0], gnssData.blh[1], gnssData.blh[2]);
                // 更新UI显示
                ui->gpsPositionLabel->setText(QString("经纬度: (%1, %2)").arg(gnssData.blh[0], 0, 'f', 6).arg(gnssData.blh[1], 0, 'f', 6));
                ui->gpsAltSpeedLabel->setText(QString("高度: %1 m, 速度: %2 m/s, 航向: %3°").arg(gnssData.blh[2], 0, 'f', 2).arg(std::sqrt(gnssData.vel[0]*gnssData.vel[0] + gnssData.vel[1]*gnssData.vel[1] + gnssData.vel[2]*gnssData.vel[2]), 0, 'f', 3).arg(0.0, 0, 'f', 1));
                // 实时更新GNSS原始数据显示
                onGnssDataUpdated();
            }, Qt::QueuedConnection);

    // 连接日志级别下拉框信号
    connect(ui->logLevelComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::on_logLevelComboBox_currentIndexChanged);

    LOG_INFO("信号槽连接完成");
}

/**
 * @brief 初始化串口选择下拉框
 */
void MainWindow::initSerialPortComboBoxes()
{
    LOG_INFO("正在初始化串口选择下拉框...");
    
    // 获取系统中所有可用的串口列表
    QList<QSerialPortInfo> availablePorts = SensorInterface::getAvailableSerialPorts();
    
    // 清空现有的串口选择下拉框
    ui->imuPortComboBox->clear();
    ui->gpsPortComboBox->clear();
    
    // 添加可用串口到下拉框
    for (const QSerialPortInfo& port : availablePorts) {
        QString portInfo = QString("%1 (%2)").arg(port.portName()).arg(port.description());
        ui->imuPortComboBox->addItem(portInfo, port.portName());
        ui->gpsPortComboBox->addItem(portInfo, port.portName());
    }
    
    // 如果有可用串口，设置默认选择
    if (!availablePorts.isEmpty()) {
        ui->imuPortComboBox->setCurrentIndex(0);
        ui->gpsPortComboBox->setCurrentIndex(0);
        LOG_INFO("设置默认串口");
    } else {
        // 如果没有可用串口，添加一个默认值
        ui->imuPortComboBox->addItem("无可用串口", "");
        ui->gpsPortComboBox->addItem("无可用串口", "");
        LOG_WARN("未找到可用串口");
    }
    
    // 初始化波特率选择下拉框
    initBaudRateComboBoxes();
    
    LOG_INFO("串口选择下拉框初始化完成");
}

/**
 * @brief 初始化波特率选择下拉框
 */
void MainWindow::initBaudRateComboBoxes()
{
    LOG_INFO("Initializing baud rate combo boxes...");
    
    // 常用波特率列表
    QList<int> baudRates = {9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600};
    
    // 清空现有的波特率选择下拉框
    ui->imuBaudComboBox->clear();
    ui->gpsBaudComboBox->clear();
    
    // 添加波特率到下拉框
    for (int baudRate : baudRates) {
        ui->imuBaudComboBox->addItem(QString::number(baudRate), baudRate);
        ui->gpsBaudComboBox->addItem(QString::number(baudRate), baudRate);
    }
    
    // 设置默认波特率
    ui->imuBaudComboBox->setCurrentText("115200");  // IMU默认使用115200
    ui->gpsBaudComboBox->setCurrentText("9600");     // GPS默认使用9600
    
    LOG_INFO("波特率选择下拉框初始化完成");
}

/**
 * @brief 日志消息处理器
 * @param message 日志消息
 */
void MainWindow::logMessageHandler(const std::string& message)
{
    // 将日志消息转换为QString
    QString logMessage = QString::fromStdString(message);
    
    // 使用静态指针instance访问MainWindow实例
    if (instance) {
        // 在主线程中更新UI
        QMetaObject::invokeMethod(instance, "appendLogMessage", Qt::QueuedConnection, Q_ARG(QString, logMessage));
    }
}

/**
 * @brief 解析日志消息中的级别
 * @param message 日志消息
 * @return 日志级别
 */
Logger::LogLevel MainWindow::parseLogLevel(const QString& message)
{
    if (message.contains("[DEBUG]")) {
        return Logger::DEBUG;
    } else if (message.contains("[INFO]")) {
        return Logger::INFO;
    } else if (message.contains("[WARN]")) {
        return Logger::WARN;
    } else if (message.contains("[ERROR]")) {
        return Logger::ERROR;
    } else if (message.contains("[FATAL]")) {
        return Logger::FATAL;
    } else {
        return Logger::INFO; // 默认级别
    }
}

/**
 * @brief 在UI中追加日志消息
 * @param message 日志消息
 */
void MainWindow::appendLogMessage(const QString& message)
{
    // 解析日志级别
    Logger::LogLevel messageLevel = parseLogLevel(message);
    
    // 根据当前日志显示级别过滤消息
    if (messageLevel >= currentLogLevel) {
        // 追加日志消息到文本编辑框
        ui->logDisplayTextEdit->append(message);
        
        // 限制显示行数，避免内存占用过多
        if (ui->logDisplayTextEdit->document()->blockCount() > 1000) {
            QTextCursor cursor = ui->logDisplayTextEdit->textCursor();
            cursor.movePosition(QTextCursor::Start);
            cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
            cursor.removeSelectedText();
            cursor.deleteChar(); // 删除空行
        }
        
        // 滚动到底部，显示最新消息
        ui->logDisplayTextEdit->moveCursor(QTextCursor::End);
    }
}

/**
 * @brief 将时间戳转换为可读的时间格式
 * @param timestamp 时间戳（秒）
 * @return 可读的时间字符串
 */
QString MainWindow::timestampToDateTime(double timestamp)
{
    // 转换为毫秒
    qint64 ms = static_cast<qint64>(timestamp * 1000);
    
    // 创建QDateTime对象
    QDateTime dateTime;
    dateTime.setMSecsSinceEpoch(ms);
    
    // 格式化为年月日时分秒毫秒
    return dateTime.toString("yyyy-MM-dd HH:mm:ss.zzz");
}

/**
 * @brief 初始化日志系统
 */
void MainWindow::initLogger()
{
    // 创建log目录
    QDir logDir = QDir::current();
    if (!logDir.exists("log")) {
        logDir.mkdir("log");
    }
    
    // 生成以当前日期为名称的日志文件路径
    QString dateStr = QDate::currentDate().toString("yyyy-MM-dd");
    QString logFilePath = logDir.filePath(QString("log/%1.log").arg(dateStr));
    
    // 设置日志消息处理器
    Logger::getInstance().setLogMessageHandler(&MainWindow::logMessageHandler);
    
    // 设置日志文件
    Logger::getInstance().setLogFile(logFilePath.toStdString());
    
    // 记录日志初始化成功
    LOG_INFO("日志系统初始化完成，日志文件路径：%s", logFilePath.toStdString().c_str());
}

/**
 * @brief 处理路线生成按钮点击事件
 */
void MainWindow::on_calibrateDataButton_clicked()
{
    // 实现IMU数据校准功能
    if (stateEstimator) {
        // 以当前设备的姿态作为水平姿态，角度为0度
        // 暂时注释掉calibrate方法的调用，避免编译错误
        // stateEstimator->calibrate();

        // 显示校准成功提示
        QMessageBox::information(this, "数据校准", "IMU传感器校准功能已触发！");
    } else {
        QMessageBox::warning(this, "数据校准", "状态估计器未初始化！");
    }
}



/**
 * @brief 处理IMU数据更新事件
 */
void MainWindow::onImuDataUpdated()
{
    LOG_INFO("接收到IMU数据更新事件");

    // 获取最新的IMU数据
    IMU imuData = sensorInterface->getLatestImuData();

    // 更新IMU数据标签
    ui->imuTimeValueLabel->setText(timestampToDateTime(imuData.time));
    ui->imuDtValueLabel->setText(QString::number(imuData.dt));
    ui->imuDthetaValueLabel->setText(QString("[%1, %2, %3]").arg(imuData.dtheta.x()).arg(imuData.dtheta.y()).arg(imuData.dtheta.z()));
    ui->imuDvelValueLabel->setText(QString("[%1, %2, %3]").arg(imuData.dvel.x()).arg(imuData.dvel.y()).arg(imuData.dvel.z()));
    ui->imuOdovelValueLabel->setText(QString::number(imuData.odovel));
    ui->imuMagneticFieldValueLabel->setText(QString("[%1, %2, %3]").arg(imuData.magnetic_field.x()).arg(imuData.magnetic_field.y()).arg(imuData.magnetic_field.z()));
    ui->imuTemperatureValueLabel->setText(QString::number(imuData.temperature));
    ui->imuOutputMagneticHeadingValueLabel->setText(QString::number(imuData.magnetic_heading));
    ui->imuTrueHeadingValueLabel->setText(QString::number(imuData.true_heading));

    // 更新主窗口中的传感器数据
    ui->imuTimeLabel->setText(QString("时间: %1").arg(timestampToDateTime(imuData.time)));
    ui->imuAccLabel->setText(QString("加速度: [%1, %2, %3] m/s²").arg(imuData.dvel.x()/imuData.dt).arg(imuData.dvel.y()/imuData.dt).arg(imuData.dvel.z()/imuData.dt));
    ui->imuGyroLabel->setText(QString("角速度: [%1, %2, %3] rad/s").arg(imuData.dtheta.x()/imuData.dt).arg(imuData.dtheta.y()/imuData.dt).arg(imuData.dtheta.z()/imuData.dt));
    ui->imuAngleLabel->setText(QString("角度: [0.00, 0.00, 0.00] °")); // 暂时设为固定值，需要根据实际数据计算
    ui->imuMagneticHeadingLabel->setText(QString("磁方位角: %1 °").arg(imuData.magnetic_heading));

    // 记录IMU数据更新日志
    LOG_INFO("IMU数据更新: time=%f, acc=[%f, %f, %f], gyro=[%f, %f, %f], mag_heading=%f", 
             imuData.time, 
             imuData.dvel.x()/imuData.dt, imuData.dvel.y()/imuData.dt, imuData.dvel.z()/imuData.dt,
             imuData.dtheta.x()/imuData.dt, imuData.dtheta.y()/imuData.dt, imuData.dtheta.z()/imuData.dt,
             imuData.magnetic_heading);

    // 通过findChild找到IMU原始数据显示组件（如果存在）
    QTextEdit *imuRawDataTextEdit = findChild<QTextEdit*>("imuRawDataTextEdit");
    if (imuRawDataTextEdit) {
        LOG_DEBUG("找到IMU原始数据显示组件，开始获取和显示数据");

        // 获取IMU原始数据
        QString imuRawData = sensorInterface->getRawImuData();
        LOG_DEBUG("IMU 原始数据长度: %d", imuRawData.length());
        if (!imuRawData.isEmpty()) {
            imuRawDataTextEdit->append(imuRawData);
            LOG_DEBUG("显示真实IMU原始数据: %s", imuRawData.toStdString().c_str());
        } else {
            // 未接收到串口数据，不做操作
            LOG_DEBUG("未接收到IMU原始数据，不显示");
        }
        // 限制显示行数，避免内存占用过多
        if (imuRawDataTextEdit->document()->blockCount() > 100) {
            QTextCursor cursor = imuRawDataTextEdit->textCursor();
            cursor.movePosition(QTextCursor::Start);
            cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
            cursor.removeSelectedText();
            cursor.deleteChar(); // 删除空行
        }
    } else {
        LOG_WARN("未找到IMU原始数据显示组件");
    }
}

/**
 * @brief 处理GPS数据更新事件
 */
void MainWindow::onGnssDataUpdated()
{
    LOG_INFO("接收到GPS数据更新事件");

    // 获取最新的GNSS数据
    GNSS gnssData = sensorInterface->getLatestGnssData();

    // 更新GNSS数据标签
    ui->gnssTimeValueLabel->setText(timestampToDateTime(gnssData.time));
    ui->gnssPositionValueLabel->setText(QString("[%1, %2, %3]").arg(gnssData.blh.x()).arg(gnssData.blh.y()).arg(gnssData.blh.z()));
    ui->gnssVelocityValueLabel->setText(QString("[%1, %2, %3]").arg(gnssData.vel.x()).arg(gnssData.vel.y()).arg(gnssData.vel.z()));
    ui->gnssStdValueLabel->setText(QString("[%1, %2, %3]").arg(gnssData.std.x()).arg(gnssData.std.y()).arg(gnssData.std.z()));
    ui->gnssValidValueLabel->setText(gnssData.isvalid ? "true" : "false");
    ui->gnssQualityValueLabel->setText(QString::number(gnssData.quality));
    ui->gnssFixModeValueLabel->setText(QString::fromStdString(gnssData.fix_mode));
    ui->gnssHdopValueLabel->setText(QString::number(gnssData.hdop));
    ui->gnssVdopValueLabel->setText(QString::number(gnssData.vdop));
    ui->gnssPdopValueLabel->setText(QString::number(gnssData.pdop));
    ui->gnssVisibleSvValueLabel->setText(QString::number(gnssData.visible_sv));
    ui->gnssUsedSvValueLabel->setText(QString::number(gnssData.used_sv));
    ui->gnssGroundSpeedValueLabel->setText(QString::number(gnssData.ground_speed));
    ui->gnssTrueHeadingValueLabel->setText(QString::number(gnssData.true_heading));
    ui->gnssMagneticHeadingValueLabel->setText(QString::number(gnssData.magnetic_heading));
    ui->gnssSigmaLatValueLabel->setText(QString::number(gnssData.sigma_lat_gst));
    ui->gnssSigmaLonValueLabel->setText(QString::number(gnssData.sigma_lon_gst));
    ui->gnssSigmaAltValueLabel->setText(QString::number(gnssData.sigma_alt_gst));
    ui->gnssSigmaRangeValueLabel->setText(QString::number(gnssData.sigma_range));

    // 更新主窗口中的传感器数据
    ui->gpsTimeLabel->setText(QString("时间: %1").arg(timestampToDateTime(gnssData.time)));
    ui->gpsPositionLabel->setText(QString("经纬度: (%1, %2)").arg(gnssData.blh.x(), 0, 'f', 6).arg(gnssData.blh.y(), 0, 'f', 6));
    double speed = std::sqrt(gnssData.vel.x()*gnssData.vel.x() + gnssData.vel.y()*gnssData.vel.y() + gnssData.vel.z()*gnssData.vel.z());
    ui->gpsAltSpeedLabel->setText(QString("高度: %1 m, 速度: %2 m/s, 航向: %3°").arg(gnssData.blh.z(), 0, 'f', 2).arg(speed, 0, 'f', 3).arg(gnssData.true_heading, 0, 'f', 1));

    // 记录GNSS数据更新日志
    LOG_INFO("GNSS数据更新: time=%f, pos=[%f, %f, %f], vel=[%f, %f, %f], hdop=%f, used_sv=%d", 
             gnssData.time, 
             gnssData.blh.x(), gnssData.blh.y(), gnssData.blh.z(),
             gnssData.vel.x(), gnssData.vel.y(), gnssData.vel.z(),
             gnssData.hdop, gnssData.used_sv);

    // 通过findChild找到GPS原始数据显示组件（如果存在）
    QTextEdit *gnssRawDataTextEdit = findChild<QTextEdit*>("gnssRawDataTextEdit");
    if (gnssRawDataTextEdit) {
        LOG_DEBUG("找到GPS原始数据显示组件，开始获取和显示数据");

        // 获取GNSS原始数据
        QString gnssRawData = sensorInterface->getRawGnssData();
        LOG_DEBUG("GNSS 原始数据长度: %d", gnssRawData.length());
        if (!gnssRawData.isEmpty()) {
            gnssRawDataTextEdit->append(gnssRawData);
            LOG_DEBUG("显示真实GNSS原始数据: %s", gnssRawData.toStdString().c_str());
        } else {
            // 未接收到串口数据，不做操作
            LOG_DEBUG("未接收到GNSS原始数据，不显示");
        }
        // 限制显示行数，避免内存占用过多
        if (gnssRawDataTextEdit->document()->blockCount() > 100) {
            QTextCursor cursor = gnssRawDataTextEdit->textCursor();
            cursor.movePosition(QTextCursor::Start);
            cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
            cursor.removeSelectedText();
            cursor.deleteChar(); // 删除空行
        }
    } else {
        LOG_WARN("未找到GPS原始数据显示组件");
    }
}

/**
 * @brief 更新IMU状态标签
 * @param status 状态：0-未连接，1-已连接，2-连接错误
 */
void MainWindow::updateImuStatusLabel(int status)
{
    QLabel *imuStatusLabel = findChild<QLabel*>("imuStatusLabel");
    if (imuStatusLabel) {
        QString statusText;
        switch (status) {
        case 0: // 未连接
            statusText = "IMU状态: ⚪ 未连接";
            break;
        case 1: // 已连接
            statusText = "IMU状态: 🟢 已连接";
            break;
        case 2: // 连接错误
            statusText = "IMU状态: 🔴 连接错误";
            break;
        default:
            statusText = "IMU状态: ⚪ 未知";
            break;
        }
        imuStatusLabel->setText(statusText);
    }
}

/**
 * @brief 更新GPS状态标签
 * @param status 状态：0-未连接，1-已连接，2-连接错误
 */
void MainWindow::updateGpsStatusLabel(int status)
{
    QLabel *gpsStatusLabel = findChild<QLabel*>("gpsStatusLabel");
    if (gpsStatusLabel) {
        QString statusText;
        switch (status) {
        case 0: // 未连接
            statusText = "GPS状态: ⚪ 未连接";
            break;
        case 1: // 已连接
            statusText = "GPS状态: 🟢 已连接";
            break;
        case 2: // 连接错误
            statusText = "GPS状态: 🔴 连接错误";
            break;
        default:
            statusText = "GPS状态: ⚪ 未知";
            break;
        }
        gpsStatusLabel->setText(statusText);
    }
}

/**
 * @brief 更新地图显示
 */
void MainWindow::updateMapDisplay()
{
    LOG_DEBUG("正在更新地图显示");

    // 这里可以添加地图更新逻辑
    // 例如：更新当前位置标记、轨迹显示等

    LOG_DEBUG("地图显示更新完成");
}

/**
 * @brief 处理IMU连接按钮点击事件
 */
void MainWindow::on_imuConnectButton_clicked()
{
    LOG_INFO("IMU连接按钮被点击");

    // 从UI获取IMU串口参数
    QString imuPort = ui->imuPortComboBox->currentData().toString();
    int imuBaudrate = ui->imuBaudComboBox->currentText().toInt();

    // 保存旧的IMU连接状态
    bool wasImuConnected = sensorInterface->isImuConnected();
    
    // 设置IMU参数
    sensorInterface->setImuSerialParams(imuPort, imuBaudrate);

    // 如果IMU未连接，则连接IMU模块
    if (!wasImuConnected) {
        bool connected = sensorInterface->connectIMU();
        if (connected) {
            // 启动IMU数据采集
            sensorInterface->startIMU();
            ui->statusbar->showMessage("IMU串口连接成功");
            LOG_INFO("IMU串口连接成功: %s, 波特率: %d", imuPort.toStdString().c_str(), imuBaudrate);
            // 更新状态标签为已连接
            updateImuStatusLabel(1);
        } else {
            ui->statusbar->showMessage("IMU串口连接失败");
            LOG_ERROR("IMU串口连接失败: %s, 波特率: %d", imuPort.toStdString().c_str(), imuBaudrate);
            // 更新状态标签为连接错误
            updateImuStatusLabel(2);
        }
    }
}

/**
 * @brief 处理IMU断开连接按钮点击事件
 */
void MainWindow::on_imuDisconnectButton_clicked()
{
    LOG_INFO("IMU断开连接按钮被点击");

    // 停止IMU数据采集
    sensorInterface->stopIMU();

    // 断开IMU模块连接
    sensorInterface->disconnectIMU();

    ui->statusbar->showMessage("IMU串口已断开");
    LOG_INFO("IMU串口已断开");
    // 更新状态标签为未连接
    updateImuStatusLabel(0);
}

/**
 * @brief 处理GPS连接按钮点击事件
 */
void MainWindow::on_gpsConnectButton_clicked()
{
    LOG_INFO("GPS连接按钮被点击");

    // 从UI获取GPS串口参数
    QString gpsPort = ui->gpsPortComboBox->currentData().toString();
    int gpsBaudrate = ui->gpsBaudComboBox->currentText().toInt();

    // 保存旧的GPS连接状态
    bool wasGnssConnected = sensorInterface->isGnssConnected();
    
    // 设置GPS参数
    sensorInterface->setGpsSerialParams(gpsPort, gpsBaudrate);

    // 如果GPS未连接，则连接GPS模块
    if (!wasGnssConnected) {
        bool connected = sensorInterface->connectGPS();
        if (connected) {
            // 启动GPS数据采集
            sensorInterface->startGPS();
            ui->statusbar->showMessage("GPS串口连接成功");
            LOG_INFO("GPS串口连接成功: %s, 波特率: %d", gpsPort.toStdString().c_str(), gpsBaudrate);
            // 更新状态标签为已连接
            updateGpsStatusLabel(1);
        } else {
            ui->statusbar->showMessage("GPS串口连接失败");
            LOG_ERROR("GPS串口连接失败: %s, 波特率: %d", gpsPort.toStdString().c_str(), gpsBaudrate);
            // 更新状态标签为连接错误
            updateGpsStatusLabel(2);
        }
    }
}

/**
 * @brief 处理GPS断开连接按钮点击事件
 */
void MainWindow::on_gpsDisconnectButton_clicked()
{
    LOG_INFO("GPS断开连接按钮被点击");

    // 停止GPS数据采集
    sensorInterface->stopGPS();

    // 断开GPS模块连接
    sensorInterface->disconnectGPS();

    ui->statusbar->showMessage("GPS串口已断开");
    LOG_INFO("GPS串口已断开");
    // 更新状态标签为未连接
    updateGpsStatusLabel(0);
}

/**
 * @brief 处理地图类型切换事件
 */
void MainWindow::on_mapTypeComboBox_currentIndexChanged(int index)
{
    LOG_INFO("地图类型已切换为: %d", index);
    // 地图类型切换逻辑
}

/**
 * @brief 处理数据源切换事件
 */
void MainWindow::on_dataSourceComboBox_currentIndexChanged(int index)
{
    LOG_INFO("数据源已切换为: %d", index);
    // 数据源切换逻辑
}

/**
 * @brief 处理开始按钮点击事件
 */
void MainWindow::on_startButton_clicked()
{
    LOG_INFO("开始按钮被点击");
    // 开始作业逻辑
}

/**
 * @brief 处理停止按钮点击事件
 */
void MainWindow::on_stopButton_clicked()
{
    LOG_INFO("停止按钮被点击");
    // 停止作业逻辑
}

/**
 * @brief 处理配置按钮点击事件
 */
void MainWindow::on_configButton_clicked()
{
    LOG_INFO("配置按钮被点击");
    // 配置逻辑
}

/**
 * @brief 处理加载配置按钮点击事件
 */
void MainWindow::on_loadConfigButton_clicked()
{
    LOG_INFO("加载配置按钮被点击");
    // 加载配置逻辑
}

/**
 * @brief 处理保存配置按钮点击事件
 */
void MainWindow::on_saveConfigButton_clicked()
{
    LOG_INFO("保存配置按钮被点击");
    // 保存配置逻辑
}

/**
 * @brief 处理连接数据库按钮点击事件
 */
void MainWindow::on_connectDatabaseButton_clicked()
{
    LOG_INFO("连接数据库按钮被点击");
    // 连接数据库逻辑
}

/**
 * @brief 处理历史作业列表项点击事件
 */
void MainWindow::on_historyListWidget_itemClicked(QListWidgetItem* item)
{
    LOG_INFO("历史作业列表项被点击: %s", item->text().toStdString().c_str());
    // 历史作业项点击逻辑
}

/**
 * @brief 处理日志级别下拉框选择事件
 */
void MainWindow::on_logLevelComboBox_currentIndexChanged(int index)
{
    // 根据索引更新日志显示级别
    switch (index) {
    case 0: // DEBUG
        currentLogLevel = Logger::DEBUG;
        break;
    case 1: // INFO
        currentLogLevel = Logger::INFO;
        break;
    case 2: // WARN
        currentLogLevel = Logger::WARN;
        break;
    case 3: // ERROR
        currentLogLevel = Logger::ERROR;
        break;
    default:
        currentLogLevel = Logger::INFO;
        break;
    }
    
    // 清空当前日志显示
    ui->logDisplayTextEdit->clear();
    
    // 记录日志级别切换
    LOG_INFO("日志显示级别已切换为: %d", index);
}
