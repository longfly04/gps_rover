#include "mainwindow.h"
#include "ui_mainwindow.h"

// 包含核心组件
#include "core/sensor/sensorinterface.h"
#include "map/statestimator.h"

// 包含UI组件
#include "map/mapwidget.h"
#include "map/tablewidget.h"

// 包含Qt组件
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QDateTime>
#include <QTime>
#include <QTimer>
#include <QElapsedTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QTcpSocket>

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
    stateEstimator(nullptr),
    imuDataCount(0),
    gpsDataCount(0),
    dataCountTimer(nullptr),
    localTimeTimer(nullptr),
    mapUpdateTimer(nullptr),
    lastImuUpdateTime(0),
    lastGnssUpdateTime(0),
    hasValidImuData(false),
    hasValidGnssData(false),
    lastValidImuData(), // 调用默认构造函数，初始化所有字段为0
    lastValidGnssData(), // 调用默认构造函数，初始化所有字段为0
    latestGnssData() // 调用默认构造函数，初始化所有字段为0
{


    ui->setupUi(this);

    // 初始化日志系统
    initLogger();

    // 初始化模块
    initModules();

    // 初始化UI
    initUI();

    // 初始化信号槽连接
    initSignalsSlots();
    
    // 记录初始化日志
    LOG_INFO("主窗口初始化完成");
    LOG_INFO("等待用户手动连接IMU和GPS模块");
    // 初始化静态instance指针
    instance = this;
    
    // 初始化数据计数和时间更新相关变量
    dataCountTimer = new QTimer(this);
    localTimeTimer = new QTimer(this);
    QTimer *gpsTimeTimer = new QTimer(this);
    mapUpdateTimer = new QTimer(this);
    
    // 连接定时器信号和槽
    connect(dataCountTimer, &QTimer::timeout, this, &MainWindow::resetDataCounts);
    connect(localTimeTimer, &QTimer::timeout, this, &MainWindow::updateLocalTime);
    connect(gpsTimeTimer, &QTimer::timeout, this, &MainWindow::updateGpsTime);
    connect(mapUpdateTimer, &QTimer::timeout, this, &MainWindow::updateMapFromTimer);
    
    // 启动定时器
    localTimeTimer->start(100); // 每100毫秒更新一次本地时间
    gpsTimeTimer->start(100); // 每100毫秒更新一次卫星时间
    mapUpdateTimer->start(100); // 每100毫秒更新一次地图
    
    // 初始化本地时间显示
    updateLocalTime();
} 

/**
 * @brief 析构函数
 */
MainWindow::~MainWindow()
{
    // 停止并断开IMU传感器
    if (imuThread) {
        imuThread->stop();
        imuThread->disconnect(); // 调用传感器的disconnect方法
    }

    // 停止并断开GPS传感器
    if (gpsThread) {
        gpsThread->stop();
        gpsThread->disconnect(); // 调用传感器的disconnect方法
    }

    // 删除坐标转换模块
    if (coordinate) {
        delete coordinate;
        coordinate = nullptr;
    }

    // 删除小区生成模块
    if (blockGenerator) {
        delete blockGenerator;
        blockGenerator = nullptr;
    }

    // 删除状态估计器
    if (stateEstimator) {
        delete stateEstimator;
        stateEstimator = nullptr;
    }

    // 清理UI组件
    delete ui;
}

/**
 * @brief 初始化模块
 */
void MainWindow::initModules()
{
    LOG_INFO("正在初始化模块...");

    // 初始化IMU传感器线程
    LOG_INFO("正在初始化IMU传感器线程...");
    imuThread = std::make_unique<ImuSensor>(this);
    // 不设置默认串口参数，等待用户手动配置
    LOG_INFO("IMU传感器线程初始化成功");

    // 初始化GPS传感器线程
    LOG_INFO("正在初始化GPS传感器线程...");
    gpsThread = std::make_unique<GpsSensor>(this);
    // 不设置默认串口参数，等待用户手动配置
    LOG_INFO("GPS传感器线程初始化成功");

    // 初始化坐标转换模块
    LOG_INFO("正在初始化坐标转换模块...");
    coordinate = new Coordinate();
    LOG_INFO("坐标转换模块初始化成功");

    // 初始化小区生成模块
    LOG_INFO("正在初始化小区生成模块...");
    blockGenerator = new BlockGenerator();
    LOG_INFO("小区生成模块初始化成功");

    // 初始化状态估计器
    LOG_INFO("正在初始化状态估计器...");
    stateEstimator = new StateEstimator();
    if (stateEstimator) {
        stateEstimator->setCoordinate(coordinate);
        LOG_INFO("状态估计器初始化成功");
    } else {
        LOG_Error("状态估计器初始化失败");
    }

    // 初始化传感器线程回调函数
    // initSensorThreads(); // 移除这个方法，直接在 initSignalsSlots 中连接信号

    // 初始化坐标系统组件
    initCoordinateWidget();



    LOG_INFO("模块初始化完成");
}



/**
 * @brief 初始化UI
 */
void MainWindow::initUI()
{
    LOG_INFO("正在初始化UI...");

    // 设置窗口标题
    setWindowTitle("干就完了 v0.2");

    // 初始化状态栏
    ui->statusbar->showMessage("系统就绪");

    // 初始化GPS信息标签
    ui->gpsPositionLabel->setText("经纬度/高度: (0.000000, 0.000000, 0.00 m)");
    ui->speedLabel->setText("速度: 0.00 m/s, 北向速度: 0.00 m/s, 东向速度: 0.00 m/s");

    // 初始化串口选择下拉框
    initSerialPortComboBoxes();

    LOG_INFO("UI初始化完成");
}



/**
 * @brief 初始化坐标系统组件
 */
void MainWindow::initCoordinateWidget()
{
    LOG_INFO("正在初始化坐标系统组件...");

    auto syncBlockGeneratorParams = [this]() {
        if (!blockGenerator) {
            return;
        }

        blockGenerator->setAreaRange(ui->areaLengthSpinBox->value(), ui->areaWidthSpinBox->value());
        blockGenerator->setRowSpacing(ui->rowSpacingSpinBox->value());
        blockGenerator->setRowsPerBlock(ui->rowsPerBlockSpinBox->value());
        blockGenerator->setFieldLength(ui->fieldLengthSpinBox->value());
        blockGenerator->setHeadlandWidth(ui->headlandWidthSpinBox->value());
    };

    auto invalidateCoordinatePlan = [this, syncBlockGeneratorParams]() {
        syncBlockGeneratorParams();

        const bool wasConfigured = hasLocalFrameConfigured;
        hasLocalFrameConfigured = false;
        blockStartWorldPosition_ = QPointF(0.0, 0.0);

        if (stateEstimator) {
            stateEstimator->reset();
        }

        if (ui->coordinateSystemWidget) {
            if (TableWidget* tableWidget = dynamic_cast<TableWidget*>(ui->coordinateSystemWidget)) {
                tableWidget->clearData();
            }
        }

        if (ui->mapWidget) {
            if (MapWidget* mapWidget = dynamic_cast<MapWidget*>(ui->mapWidget)) {
                mapWidget->setBlockPlanResult(BlockPlanResult());
                mapWidget->setCoordinate(nullptr);
                if (wasConfigured) {
                    mapWidget->clearPath();
                }
            }
        }

        if (ui->statusbar) {
            ui->statusbar->showMessage("本地坐标系参数已更新，请点击“生成”应用", 3000);
        }
    };

    connect(ui->areaLengthSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, invalidateCoordinatePlan);
    connect(ui->areaWidthSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, invalidateCoordinatePlan);
    connect(ui->rowSpacingSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, invalidateCoordinatePlan);
    connect(ui->rowsPerBlockSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, invalidateCoordinatePlan);
    connect(ui->fieldLengthSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, invalidateCoordinatePlan);
    connect(ui->headlandWidthSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, invalidateCoordinatePlan);
    connect(ui->oPointLatitudeSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, invalidateCoordinatePlan);
    connect(ui->oPointLongitudeSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, invalidateCoordinatePlan);
    connect(ui->aPointLatitudeSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, invalidateCoordinatePlan);
    connect(ui->aPointLongitudeSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, invalidateCoordinatePlan);

    syncBlockGeneratorParams();

    if (ui->coordinateSystemWidget) {
        if (TableWidget* tableWidget = dynamic_cast<TableWidget*>(ui->coordinateSystemWidget)) {
            tableWidget->clearData();
        }
    }

    if (ui->mapWidget) {
        if (MapWidget* mapWidget = dynamic_cast<MapWidget*>(ui->mapWidget)) {
            mapWidget->setBlockPlanResult(BlockPlanResult());
            mapWidget->setCoordinate(nullptr);
        }
    }

    LOG_INFO("坐标系统组件初始化完成");
}

/**
 * @brief 初始化信号槽连接
 */
void MainWindow::initSignalsSlots()
{
    LOG_INFO("正在连接信号槽...");

    // 连接 IMU 传感器的信号到处理函数，使用队列连接确保在主线程处理
    // 注意：IMU 频率可能 200Hz+，直接逐帧刷新 UI 会造成事件队列积压。
    // 这里采用 UI 侧合并：每帧只缓存最新数据，并在下一轮事件循环刷新一次 UI。
    connect(imuThread.get(), &ImuSensor::imuDataReceived,
            this,
            [this](const IMU& imuData) {
                this->lastImuUiData_ = imuData;
                if (this->imuUiUpdatePending_) {
                    return;
                }
                this->imuUiUpdatePending_ = true;
                QTimer::singleShot(0, this, [this]() {
                    this->imuUiUpdatePending_ = false;
                    this->onImuDataUpdated(this->lastImuUiData_);
                });
            },
            Qt::QueuedConnection);

    // 连接 GPS 传感器的信号到处理函数，使用队列连接确保在主线程处理
    // GPS 可能 20Hz（模拟）或更高，为确保 UI 实时且不积压，同样采用 UI 侧合并。
    connect(gpsThread.get(), &GpsSensor::gnssDataReceived,
            this,
            [this](const GNSS& gnssData) {
                this->lastGnssUiData_ = gnssData;
                if (this->gpsUiUpdatePending_) {
                    return;
                }
                this->gpsUiUpdatePending_ = true;
                QTimer::singleShot(0, this, [this]() {
                    this->gpsUiUpdatePending_ = false;
                    this->onGnssDataUpdated(this->lastGnssUiData_);
                });
            },
            Qt::QueuedConnection);
    
    // 连接 GPS 传感器的时间同步完成信号
    connect(gpsThread.get(), &GpsSensor::timeSyncCompleted, this, &MainWindow::onTimeSyncCompleted, Qt::QueuedConnection);
    

    
    // 连接PLC连接和断开按钮信号
    connect(ui->plcConnectButton, &QPushButton::clicked, this, &MainWindow::on_plcConnectButton_clicked);
    connect(ui->plcDisconnectButton, &QPushButton::clicked, this, &MainWindow::on_plcDisconnectButton_clicked);


    // 连接TableWidget的blockSelected信号到MapWidget的highlightBlock方法
    if (ui->coordinateSystemWidget && ui->mapWidget) {
        TableWidget* tableWidget = dynamic_cast<TableWidget*>(ui->coordinateSystemWidget);
        MapWidget* mapWidget = dynamic_cast<MapWidget*>(ui->mapWidget);
        if (tableWidget && mapWidget) {
            connect(tableWidget, &TableWidget::blockSelected, mapWidget, &MapWidget::highlightBlock);
        }
    }

    LOG_INFO("信号槽连接完成");
}

/**
 * @brief 初始化串口选择下拉框
 */
void MainWindow::initSerialPortComboBoxes()
{
    LOG_INFO("正在初始化串口选择下拉框...");
    
    // 获取系统中所有可用的串口列表
    QList<QSerialPortInfo> availablePorts = SensorBase::getAvailableSerialPorts();
    
    // 清空现有的串口选择下拉框
    ui->imuPortComboBox->clear();
    ui->gpsPortComboBox->clear();
    
    // 添加空选项作为默认值
    ui->imuPortComboBox->addItem("请选择串口", "");
    ui->gpsPortComboBox->addItem("请选择串口", "");
    
    // 添加可用串口到下拉框
    for (const QSerialPortInfo& port : availablePorts) {
        QString portInfo = QString("%1 (%2)").arg(port.portName()).arg(port.description());
        ui->imuPortComboBox->addItem(portInfo, port.portName());
        ui->gpsPortComboBox->addItem(portInfo, port.portName());
    }
    
    // 如果没有可用串口，添加一个提示项
    if (availablePorts.isEmpty()) {
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
    
    // 添加空选项作为默认值
    ui->imuBaudComboBox->addItem("请选择波特率", 0);
    ui->gpsBaudComboBox->addItem("请选择波特率", 0);
    
    // 添加波特率到下拉框
    for (int baudRate : baudRates) {
        ui->imuBaudComboBox->addItem(QString::number(baudRate), baudRate);
        ui->gpsBaudComboBox->addItem(QString::number(baudRate), baudRate);
    }
    
    LOG_INFO("波特率选择下拉框初始化完成");
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
    
    // 设置日志级别为DEBUG
    Logger::getInstance().setLevel(Logger::LogLevel::INFO);
    
    // 设置日志文件
    Logger::getInstance().setLogFile(logFilePath.toStdString());
    
    // 记录日志初始化成功
    LOG_INFO("日志系统初始化完成，日志文件路径：%s", logFilePath.toStdString().c_str());
}





/**
 * @brief 处理IMU数据更新事件
 * @param imuData IMU数据
 */
void MainWindow::onImuDataUpdated(const IMU& imuData)
{
    try {
        // 增加IMU数据计数（单调递增，从程序启动开始）
        imuDataCount++;

        // 保存最新IMU数据
        lastValidImuData = imuData;
        hasValidImuData = true;

        // IMU UI 更新：采用 UI 侧合并后，这里不再做额外的 10Hz 节流。
        // 每次合并触发时（通常 0-1 个事件循环延迟）都刷新关键标签，保证“实时显示”。

        // 只更新概要信息区 + 关键 IMU 数值
        ui->imuDataCountLabel->setText(QString("有效IMU数据: %1").arg(imuDataCount));

        // IMU角度显示（概要信息区）
        ui->imuAngleDisplayLabel->setText(
            QString("当前角度: (%1, %2, %3) °")
                .arg(imuData.angle.x() * 180.0 / M_PI, 0, 'f', 2)
                .arg(imuData.angle.y() * 180.0 / M_PI, 0, 'f', 2)
                .arg(imuData.angle.z() * 180.0 / M_PI, 0, 'f', 2));

        // 配置页的计数标签（轻量，保留）
        ui->imuConfigLabel->setText(QString("IMU有效数据计数: %1").arg(imuDataCount));

        // 保留IMU磁航向、加速度和角速度增量的刷新逻辑
        if (ui->imuOutputMagneticHeadingLabel) {
            ui->imuOutputMagneticHeadingLabel->setText(QString("IMU磁航向: %1 °").arg(imuData.magnetic_heading, 0, 'f', 1));
        }
        if (ui->imuDthetaLabel) {
            ui->imuDthetaLabel->setText(QString("角速度增量: [%1, %2, %3]")
                .arg(imuData.dtheta.x(), 0, 'f', 6)
                .arg(imuData.dtheta.y(), 0, 'f', 6)
                .arg(imuData.dtheta.z(), 0, 'f', 6));
        }
        if (ui->imuAccelerationLabel) {
            ui->imuAccelerationLabel->setText(QString("加速度: [%1, %2, %3] m/s^2")
                .arg(imuData.acceleration.x(), 0, 'f', 3)
                .arg(imuData.acceleration.y(), 0, 'f', 3)
                .arg(imuData.acceleration.z(), 0, 'f', 3));
        }

        // 每 500 个数据点记录一次日志（IMU 约200Hz，即约 2.5 秒一次）
        if (imuDataCount % 500 == 0) {
            LOG_DEBUG("IMU 数据更新 %d: time=%f, acc=[%f, %f, %f], gyro=[%f, %f, %f], mag_heading=%f",
                      imuDataCount,
                      imuData.time,
                      imuData.acceleration.x(), imuData.acceleration.y(), imuData.acceleration.z(),
                      imuData.angular_velocity.x(), imuData.angular_velocity.y(), imuData.angular_velocity.z(),
                      imuData.magnetic_heading);
        }
    } catch (const std::exception& e) {
        LOG_Error("IMU数据更新异常: %s", e.what());
    } catch (...) {
        LOG_Error("IMU数据更新未知异常");
    }
}

/**
 * @brief 处理GPS数据更新事件
 * @param gnssData GNSS数据
 */
void MainWindow::onGnssDataUpdated(const GNSS& gnssData)
{
    try {
        // 检查数据是否有效
        if (!gnssData.isvalid) {
            return;
        }

        lastValidGnssData = gnssData;
        hasValidGnssData = true;
        gpsDataCount++; // 只统计有效的GNSS数据

        // 始终保存最新的有效GNSS数据，用于地图更新
        latestGnssData = gnssData;

        // 如果坐标系已配置，将GPS数据喂给状态估计器
        if (coordinate && coordinate->isInitialized() && stateEstimator) {
            SensorData sensorData;
            sensorData.lat = gnssData.blh.x();
            sensorData.lon = gnssData.blh.y();
            sensorData.altitude = gnssData.blh.z();
            sensorData.heading = gnssData.magnetic_heading;
            sensorData.speed = std::sqrt(gnssData.vel.x() * gnssData.vel.x() + gnssData.vel.y() * gnssData.vel.y());
            sensorData.hasIMU = false;
            sensorData.timestamp = gnssData.time;

            stateEstimator->update(sensorData);
        }

        const GNSS& dataToUse = lastValidGnssData;

        // ---------- timeAndSensorGroupBox（概要信息）----------
        ui->gpsDataCountLabel->setText(QString("有效GPS数据: %1").arg(gpsDataCount));
        ui->gpsConfigLabel->setText(QString("GPS有效数据计数: %1").arg(gpsDataCount));

        ui->gpsPositionLabel->setText(QString("经纬度/高度: (%1, %2, %3 m)")
            .arg(dataToUse.blh[0], 0, 'f', 6)
            .arg(dataToUse.blh[1], 0, 'f', 6)
            .arg(dataToUse.blh[2], 0, 'f', 2));

        const double speed = std::sqrt(dataToUse.vel[0] * dataToUse.vel[0] + dataToUse.vel[1] * dataToUse.vel[1]);
        ui->speedLabel->setText(QString("速度: %1 m/s, 北向: %2 m/s, 东向: %3 m/s")
            .arg(speed, 0, 'f', 2)
            .arg(dataToUse.vel[0], 0, 'f', 2)
            .arg(dataToUse.vel[1], 0, 'f', 2));

        ui->magneticHeadingLabel->setText(QString("磁方位角: %1 °").arg(dataToUse.magnetic_heading, 0, 'f', 1));



        // 每 100 个有效数据点记录一次日志（20Hz 下约 5 秒一次）
        if (gpsDataCount % 100 == 0) {
            LOG_INFO("GNSS 数据更新 %d: time=%f, pos=[%.6f, %.6f, %.2f], vel=[%.2f, %.2f, %.2f], hdop=%.2f, used_sv=%d",
                    gpsDataCount,
                     gnssData.time,
                     gnssData.blh.x(), gnssData.blh.y(), gnssData.blh.z(),
                     gnssData.vel.x(), gnssData.vel.y(), gnssData.vel.z(),
                     gnssData.hdop, gnssData.used_sv);
        }
    } catch (const std::exception& e) {
        LOG_Error("GNSS数据更新异常: %s", e.what());
    } catch (...) {
        LOG_Error("GNSS数据更新未知异常");
    }
}

/**
 * @brief 更新IMU状态标签
 * @param status 状态：0-未连接，1-已连接，2-连接错误
 */
void MainWindow::updateImuStatusLabel(int status)
{
    if (ui->imuStatusLabel) {
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
        ui->imuStatusLabel->setText(statusText);
    }
}

/**
 * @brief 更新GPS状态标签
 * @param status 状态：0-未连接，1-已连接，2-连接错误
 */
void MainWindow::updateGpsStatusLabel(int status)
{
    if (ui->gpsStatusLabel) {
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
        ui->gpsStatusLabel->setText(statusText);
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

    // 禁用按钮，防止重复点击
    ui->imuConnectButton->setEnabled(false);

    // 从UI获取IMU串口参数
    QString imuPort = ui->imuPortComboBox->currentData().toString();
    QString baudrateText = ui->imuBaudComboBox->currentText();
    
    // 验证参数
    if (imuPort.isEmpty() || baudrateText == "请选择波特率") {
        ui->statusbar->showMessage("请选择有效的串口和波特率");
        LOG_WARN("IMU连接参数无效: port=%s, baudrate=%s", imuPort.toStdString().c_str(), baudrateText.toStdString().c_str());
        updateImuStatusLabel(2);
        // 重新启用按钮
        ui->imuConnectButton->setEnabled(true);
        return;
    }
    
    int imuBaudrate = baudrateText.toInt();

    try {
        // 先停止IMU数据采集并断开连接
        imuThread->stop();
        imuThread->disconnect();

        // 设置IMU参数
        imuThread->setSerialParams(imuPort, imuBaudrate);

        // 注意：在新的传感器架构中，加载器会在connect()方法中自动创建和管理
        // 不需要手动设置加载器和回调函数
        // 注意：setSerialParams方法中已经包含了断开连接和重新连接的逻辑

        // 连接IMU模块
        bool connected = imuThread->connect();
        if (connected) {
            // 启动IMU数据采集
            imuThread->start();
            ui->statusbar->showMessage("IMU串口连接成功");
            LOG_INFO("IMU串口连接成功: %s, 波特率: %d", imuPort.toStdString().c_str(), imuBaudrate);
            // 更新状态标签为已连接
            updateImuStatusLabel(1);
        } else {
            ui->statusbar->showMessage("IMU串口连接失败");
            LOG_Error("IMU串口连接失败: %s, 波特率: %d", imuPort.toStdString().c_str(), imuBaudrate);
            // 更新状态标签为连接错误
            updateImuStatusLabel(2);
        }
    } catch (const std::exception& e) {
        LOG_Error("Exception in IMU connect: %s", e.what());
        ui->statusbar->showMessage("IMU连接过程中发生错误");
        updateImuStatusLabel(2);
    }

    // 重新启用按钮
    ui->imuConnectButton->setEnabled(true);
}

/**
 * @brief 处理IMU断开连接按钮点击事件
 */
void MainWindow::on_imuDisconnectButton_clicked()
{
    LOG_INFO("IMU断开连接按钮被点击");

    // 禁用按钮，防止重复点击
    ui->imuDisconnectButton->setEnabled(false);

    try {
        // 停止IMU数据采集
        imuThread->stop();

        // 断开IMU模块连接
        imuThread->disconnect();

        ui->statusbar->showMessage("IMU串口已断开");
        LOG_INFO("IMU串口已断开");
        // 更新状态标签为未连接
        updateImuStatusLabel(0);
    } catch (const std::exception& e) {
        LOG_Error("Exception in IMU disconnect: %s", e.what());
        ui->statusbar->showMessage("IMU断开连接过程中发生错误");
    }

    // 重新启用按钮
    ui->imuDisconnectButton->setEnabled(true);
}

/**
 * @brief 处理GPS连接按钮点击事件
 */
void MainWindow::on_gpsConnectButton_clicked()
{
    LOG_INFO("GPS连接按钮被点击");

    // 禁用按钮，防止重复点击
    ui->gpsConnectButton->setEnabled(false);

    // 从UI获取GPS串口参数
    QString gpsPort = ui->gpsPortComboBox->currentData().toString();
    QString baudrateText = ui->gpsBaudComboBox->currentText();
    
    // 验证参数
    if (gpsPort.isEmpty() || baudrateText == "请选择波特率") {
        ui->statusbar->showMessage("请选择有效的串口和波特率");
        LOG_WARN("GPS连接参数无效: port=%s, baudrate=%s", gpsPort.toStdString().c_str(), baudrateText.toStdString().c_str());
        updateGpsStatusLabel(2);
        // 重新启用按钮
        ui->gpsConnectButton->setEnabled(true);
        return;
    }
    
    int gpsBaudrate = baudrateText.toInt();

    try {
        // 先停止GPS数据采集并断开连接
        gpsThread->stop();
        gpsThread->disconnect();

        // 设置GPS参数
        gpsThread->setSerialParams(gpsPort, gpsBaudrate);

        // 注意：在新的传感器架构中，加载器会在connect()方法中自动创建和管理
        // 不需要手动设置加载器和回调函数
        // 注意：setSerialParams方法中已经包含了断开连接和重新连接的逻辑

        // 连接GPS模块
        bool connected = gpsThread->connect();
        if (connected) {
            // 启动GPS数据采集
            gpsThread->start();
            ui->statusbar->showMessage("GPS串口连接成功");
            LOG_INFO("GPS串口连接成功: %s, 波特率: %d", gpsPort.toStdString().c_str(), gpsBaudrate);
            // 更新状态标签为已连接
            updateGpsStatusLabel(1);
        } else {
            ui->statusbar->showMessage("GPS串口连接失败");
            LOG_Error("GPS串口连接失败: %s, 波特率: %d", gpsPort.toStdString().c_str(), gpsBaudrate);
            // 更新状态标签为连接错误
            updateGpsStatusLabel(2);
        }
    } catch (const std::exception& e) {
        LOG_Error("Exception in GPS connect: %s", e.what());
        ui->statusbar->showMessage("GPS连接过程中发生错误");
        updateGpsStatusLabel(2);
    }

    // 重新启用按钮
    ui->gpsConnectButton->setEnabled(true);
}

/**
 * @brief 处理GPS断开连接按钮点击事件
 */
void MainWindow::on_gpsDisconnectButton_clicked()
{
    LOG_INFO("GPS断开连接按钮被点击");

    // 禁用按钮，防止重复点击
    ui->gpsDisconnectButton->setEnabled(false);

    try {
        // 停止GPS数据采集
        gpsThread->stop();

        // 断开GPS模块连接
        gpsThread->disconnect();

        ui->statusbar->showMessage("GPS串口已断开");
        LOG_INFO("GPS串口已断开");
        // 更新状态标签为未连接
        updateGpsStatusLabel(0);
    } catch (const std::exception& e) {
        LOG_Error("Exception in GPS disconnect: %s", e.what());
        ui->statusbar->showMessage("GPS断开连接过程中发生错误");
    }

    // 重新启用按钮
    ui->gpsDisconnectButton->setEnabled(true);
}

/**
 * @brief 处理PLC保存按钮点击事件
 */
void MainWindow::on_plcConnectButton_clicked()
{
    LOG_INFO("PLC保存按钮被点击");

    // 从UI获取PLC网络参数
    QString plcIp = ui->plcIpLineEdit->text();
    QString plcPort = ui->plcPortLineEdit->text();
    
    // 验证参数
    if (plcIp.isEmpty() || plcPort.isEmpty()) {
        ui->statusbar->showMessage("请填写有效的IP地址和端口");
        LOG_WARN("PLC网络参数无效: ip=%s, port=%s", plcIp.toStdString().c_str(), plcPort.toStdString().c_str());
        return;
    }
    
    // 保存PLC网络参数
    LOG_INFO("保存PLC网络参数: ip=%s, port=%s", plcIp.toStdString().c_str(), plcPort.toStdString().c_str());
    
    // 显示保存成功提示
    ui->statusbar->showMessage("PLC网络参数保存成功");
    QMessageBox::information(this, "保存成功", QString("已保存PLC网络参数: IP=%1, 端口=%2").arg(plcIp).arg(plcPort));
}

/**
 * @brief 处理PLC测试按钮点击事件
 */
void MainWindow::on_plcDisconnectButton_clicked()
{
    LOG_INFO("PLC测试按钮被点击");

    // 从UI获取PLC网络参数
    QString plcIp = ui->plcIpLineEdit->text();
    QString plcPortStr = ui->plcPortLineEdit->text();
    
    // 验证参数
    if (plcIp.isEmpty() || plcPortStr.isEmpty()) {
        ui->statusbar->showMessage("请填写有效的IP地址和端口");
        LOG_WARN("PLC网络参数无效: ip=%s, port=%s", plcIp.toStdString().c_str(), plcPortStr.toStdString().c_str());
        return;
    }
    
    int plcPort = plcPortStr.toInt();
    
    // 测试PLC设备连通性
    LOG_INFO("测试PLC设备连通性: ip=%s, port=%d", plcIp.toStdString().c_str(), plcPort);
    
    // 创建TCP socket进行测试
    QTcpSocket socket;
    socket.connectToHost(plcIp, plcPort);
    
    if (socket.waitForConnected(2000)) {
        // 连接成功
        ui->statusbar->showMessage("PLC设备连通性测试成功");
        LOG_INFO("PLC设备连通性测试成功: ip=%s, port=%d", plcIp.toStdString().c_str(), plcPort);
        QMessageBox::information(this, "测试成功", QString("PLC设备连通性测试成功: IP=%1, 端口=%2").arg(plcIp).arg(plcPort));
        socket.disconnectFromHost();
    } else {
        // 连接失败
        ui->statusbar->showMessage("PLC设备连通性测试失败");
        LOG_Error("PLC设备连通性测试失败: ip=%s, port=%d, error=%s", plcIp.toStdString().c_str(), plcPort, socket.errorString().toStdString().c_str());
        QMessageBox::warning(this, "测试失败", QString("PLC设备连通性测试失败: IP=%1, 端口=%2\n错误: %3").arg(plcIp).arg(plcPort).arg(socket.errorString()));
    }
}

/**
 * @brief 更新PLC状态标签
 * @param status 状态：0-未连接，1-已连接，2-连接错误
 */
void MainWindow::updatePlcStatusLabel(int status)
{
    if (ui->plcStatusLabel) {
        QString statusText;
        switch (status) {
        case 0: // 未连接
            statusText = "PLC状态: ⚪ 未连接";
            break;
        case 1: // 已连接
            statusText = "PLC状态: 🟢 已连接";
            break;
        case 2: // 连接错误
            statusText = "PLC状态: 🔴 连接错误";
            break;
        default:
            statusText = "PLC状态: ⚪ 未知";
            break;
        }
        ui->plcStatusLabel->setText(statusText);
    }
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
 * @brief 处理设置为原点按钮点击事件
 */
void MainWindow::on_loadConfigButton_clicked()
{
    LOG_INFO("设置为原点按钮被点击");
    
    // 获取最新的GNSS数据
    if (hasValidGnssData) {
        // 使用真实的GPS数据
        GNSS gnssData = lastValidGnssData;
        
        // 将当前经纬度赋值给O点输入框
        ui->oPointLatitudeSpinBox->setValue(gnssData.blh.x());
        ui->oPointLongitudeSpinBox->setValue(gnssData.blh.y());
        
        // 记录操作
        LOG_INFO("已将当前位置设置为原点: (%.6f, %.6f)", gnssData.blh.x(), gnssData.blh.y());
        
        // 显示提示信息
        QMessageBox::information(this, "设置成功", QString("已将当前位置 (%.6f, %.6f) 设置为原点").arg(gnssData.blh.x()).arg(gnssData.blh.y()));
    } else {
        // 没有有效GPS数据时的处理
        LOG_WARN("没有有效的GPS数据，无法设置原点");
        QMessageBox::warning(this, "设置失败", "没有有效的GPS数据，无法设置原点");
    }

}

/**
 * @brief 处理设置为A点按钮点击事件
 */
void MainWindow::on_saveConfigButton_clicked()
{
    LOG_INFO("设置为A点按钮被点击");
    
    // 获取最新的GNSS数据
    if (hasValidGnssData) {
        // 使用真实的GPS数据
        GNSS gnssData = lastValidGnssData;
        
        // 将当前经纬度赋值给A点输入框
        ui->aPointLatitudeSpinBox->setValue(gnssData.blh.x());
        ui->aPointLongitudeSpinBox->setValue(gnssData.blh.y());
        
        // 记录操作
        LOG_INFO("已将当前位置设置为A点: (%.6f, %.6f)", gnssData.blh.x(), gnssData.blh.y());
        
        // 显示提示信息
        QMessageBox::information(this, "设置成功", QString("已将当前位置 (%.6f, %.6f) 设置为A点").arg(gnssData.blh.x()).arg(gnssData.blh.y()));
    } else {
        // 没有有效GPS数据时的处理
        LOG_WARN("没有有效的GPS数据，无法设置A点");
        QMessageBox::warning(this, "设置失败", "没有有效的GPS数据，无法设置A点");
    }
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
 * @brief 处理生成配置按钮点击事件
 */
void MainWindow::on_generateConfigButton_clicked()
{
    LOG_INFO("生成配置按钮被点击");

    if (!coordinate || !blockGenerator) {
        return;
    }

    const double oLat = ui->oPointLatitudeSpinBox->value();
    const double oLon = ui->oPointLongitudeSpinBox->value();
    const double aLat = ui->aPointLatitudeSpinBox->value();
    const double aLon = ui->aPointLongitudeSpinBox->value();

    coordinate->setOriginAndDirection(oLat, oLon, aLat, aLon);
    LOG_INFO("已设置坐标系原点和方向: O(%.6f, %.6f), A(%.6f, %.6f)", oLat, oLon, aLat, aLon);

    hasLocalFrameConfigured = true;
    blockStartWorldPosition_ = QPointF(0.0, 0.0);

    if (stateEstimator) {
        stateEstimator->reset();
    }

    blockGenerator->setAreaRange(ui->areaLengthSpinBox->value(), ui->areaWidthSpinBox->value());
    blockGenerator->setRowSpacing(ui->rowSpacingSpinBox->value());
    blockGenerator->setRowsPerBlock(ui->rowsPerBlockSpinBox->value());
    blockGenerator->setFieldLength(ui->fieldLengthSpinBox->value());
    blockGenerator->setHeadlandWidth(ui->headlandWidthSpinBox->value());

    const BlockPlanResult result = blockGenerator->generate();

    if (ui->coordinateSystemWidget) {
        if (TableWidget* tableWidget = dynamic_cast<TableWidget*>(ui->coordinateSystemWidget)) {
            tableWidget->setBlockPlanResult(result);
        }
    }

    if (ui->mapWidget) {
        if (MapWidget* mapWidget = dynamic_cast<MapWidget*>(ui->mapWidget)) {
            mapWidget->setCoordinate(coordinate);
            mapWidget->setBlockPlanResult(result);
            mapWidget->clearPath();
        }
    }

    LOG_INFO("已生成小区规划: %d个小区, %d个过道", result.totalBlocks, static_cast<int>(result.headlands.size()));
    QMessageBox::information(this, "生成成功", QString("已生成%1个小区，%2个过道").arg(result.totalBlocks).arg(result.headlands.size()));
}



/**
 * @brief 更新本地时间显示
 */
void MainWindow::updateLocalTime()
{
    // 获取当前本地时间
    QDateTime currentTime = QDateTime::currentDateTime();
    
    // 格式化时间字符串
    QString timeString = currentTime.toString("yyyy-MM-dd HH:mm:ss");
    
    // 更新本地时间标签
    ui->localTimeLabel->setText(timeString);
}

/**
 * @brief 更新卫星时间显示
 */
void MainWindow::updateGpsTime()
{
    // 检查GPS传感器是否已连接且时间已同步
    if (gpsThread && gpsThread->isConnected() && gpsThread->isTimeSynced()) {
        // 获取当前本地时间
        QDateTime currentLocalTime = QDateTime::currentDateTime();
        double localTimeSecs = currentLocalTime.toSecsSinceEpoch() + currentLocalTime.time().msec() / 1000.0;
        
        // 获取时间同步的dt值
        double dt = gpsThread->getTimeSyncDt();
        
        // 计算卫星时间（本地时间 + dt）
        double gpsTimeSecs = localTimeSecs + dt;
        
        // 转换为QDateTime
        QDateTime gpsTime = QDateTime::fromSecsSinceEpoch(static_cast<qint64>(gpsTimeSecs));
        gpsTime = gpsTime.addMSecs(static_cast<int>((gpsTimeSecs - static_cast<qint64>(gpsTimeSecs)) * 1000));
        
        // 格式化时间字符串
        QString timeString = gpsTime.toString("yyyy-MM-dd HH:mm:ss");
        
        // 更新卫星时间标签
        if (ui->gpsTimeValueLabel) {
            ui->gpsTimeValueLabel->setText(timeString);
        }
    }
}

/**
 * @brief 处理GPS时间同步完成事件
 * @param dt 时间同步的dt值
 */
void MainWindow::onTimeSyncCompleted(double dt)
{
    LOG_INFO("GPS time sync completed with dt=%.3f seconds", dt);
    
    // 检查IMU传感器是否已连接
    if (imuThread && imuThread->isConnected() && imuThread->isRunning()) {
        // 设置IMU传感器的时间同步状态
        imuThread->setTimeSynced(true);
        imuThread->setTimeSyncDt(dt);
        LOG_INFO("IMU time sync status updated");
    }
    
    // 立即更新卫星时间显示
    updateGpsTime();
}

/**
 * @brief 更新数据计数显示
 */
void MainWindow::resetDataCounts()
{
    // 不再重置计数，只更新标签显示
    ui->imuConfigLabel->setText(QString("IMU有效数据计数: %1").arg(imuDataCount));
    ui->gpsConfigLabel->setText(QString("GPS有效数据计数: %1").arg(gpsDataCount));
}

/**
 * @brief 定时更新地图显示，每秒调用一次
 * @details 从latestGnssData获取最新的GPS数据并更新地图显示
 */
void MainWindow::updateMapFromTimer()
{
    if (!hasValidGnssData || !latestGnssData.isvalid) {
        return;
    }

    MapWidget* mapWidget = dynamic_cast<MapWidget*>(ui->mapWidget);
    if (!mapWidget) {
        return;
    }

    if (hasLocalFrameConfigured && coordinate && coordinate->isInitialized() && stateEstimator) {
        EstimatedPose pose = stateEstimator->getPose();
        mapWidget->updateVehiclePose(pose);
        return;
    }

    EstimatedPose pose{};
    pose.x = latestGnssData.blh.y();
    pose.y = latestGnssData.blh.x();
    pose.vx = latestGnssData.vel.y();
    pose.vy = latestGnssData.vel.x();
    pose.heading = latestGnssData.magnetic_heading;
    pose.magneticHeading = latestGnssData.magnetic_heading;
    pose.latitude = latestGnssData.blh.x();
    pose.longitude = latestGnssData.blh.y();
    pose.timestamp = latestGnssData.time;

    mapWidget->updateVehiclePose(pose);
}
