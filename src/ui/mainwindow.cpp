#include "mainwindow.h"
#include "ui_mainwindow.h"

// 包含核心组件
#include "core/sensor/sensorinterface.h"
#include "map/statestimator.h"

// 包含UI组件
#include "map/mapwidget.h"

// 包含Qt组件
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QDateTime>
#include <QDate>
#include <QDir>
#include <QTime>
#include <QTimer>
#include <QLabel>
#include <QElapsedTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QTcpSocket>
#include <QBrush>
#include <QCheckBox>
#include <QHeaderView>
#include <QLineEdit>
#include <QTableWidget>

#include <algorithm>
#include <cmath>

// 包含日志组件
#include "core/common/logger.h"

namespace {
constexpr double kGravityMps2 = 9.80665;
constexpr double kDefaultLeadingClearanceM = 1.0;
constexpr double kMinReferenceSpeedMps = 0.05;
constexpr double kFallbackReferenceSpeedMps = 1.0;

QString normalizeSeparators(QString text)
{
    return text.trimmed().replace(QStringLiteral("，"), QStringLiteral(",")).replace(';', ',');
}

QString formatPlanDouble(double value)
{
    return std::isfinite(value) ? QString::number(value, 'f', 4) : QStringLiteral("--");
}

QString formatTriggerResultText(const QString& coord, const QString& time)
{
    if (coord.isEmpty() && time.isEmpty()) {
        return QString();
    }
    if (coord.isEmpty()) {
        return time;
    }
    if (time.isEmpty()) {
        return coord;
    }
    return coord + QStringLiteral("\n") + time;
}

struct PlcCommandConfig {
    QLineEdit* addressLineEdit = nullptr;
    QLineEdit* valueLineEdit = nullptr;
    QCheckBox* forwardCheckBox = nullptr;
    QCheckBox* reverseCheckBox = nullptr;
};

PlcCommandConfig plcCommandConfigForIndex(Ui::MainWindow* ui, int index)
{
    if (!ui) {
        return {};
    }

    switch (index) {
    case 1:
        return {ui->plcAddr1LineEdit, ui->plcCmdValue1LineEdit, ui->forwardTriggerLine1, ui->reverseTriggerLine1};
    case 2:
        return {ui->plcAddr2LineEdit, ui->plcCmdValue2LineEdit, ui->forwardTriggerLine2, ui->reverseTriggerLine2};
    case 3:
        return {ui->plcAddr3LineEdit, ui->plcCmdValue3LineEdit, ui->forwardTriggerLine3, ui->reverseTriggerLine3};
    case 4:
        return {ui->plcAddr4LineEdit, ui->plcCmdValue4LineEdit, ui->forwardTriggerLine4, ui->reverseTriggerLine4};
    case 5:
        return {ui->plcAddr5LineEdit, ui->plcCmdValue5LineEdit, ui->forwardTriggerLine5, ui->reverseTriggerLine5};
    default:
        return {};
    }
}

}

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
    triggerCommitTimer(nullptr),
    modbusClient(nullptr),
    plcReadTimer(nullptr),
    isPlcConnected(false),
    plcReadCount(0),
    plcSendCount(0),
    lastImuUpdateTime(0),
    lastGnssUpdateTime(0),
    hasValidImuData(false),
    hasValidGnssData(false),
    lastValidImuData(),
    lastValidGnssData(),
    latestGnssData(),
    triggerPlanResult_(),
    lastProcessedTriggerSequence_(0)
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
    triggerCommitTimer = new QTimer(this);
    triggerCommitTimer->setSingleShot(true);
    triggerCommitTimer->setTimerType(Qt::PreciseTimer);

    // 连接定时器信号和槽
    connect(dataCountTimer, &QTimer::timeout, this, &MainWindow::resetDataCounts);
    connect(localTimeTimer, &QTimer::timeout, this, &MainWindow::updateLocalTime);
    connect(gpsTimeTimer, &QTimer::timeout, this, &MainWindow::updateGpsTime);
    connect(mapUpdateTimer, &QTimer::timeout, this, &MainWindow::updateMapFromTimer);
    connect(triggerCommitTimer, &QTimer::timeout, this, &MainWindow::commitPendingTriggerFromTimer);
    
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

    ui->statusbar->showMessage("系统就绪");
    initSerialPortComboBoxes();

    if (ui->coordinateSystemGroupBox) {
        ui->coordinateSystemGroupBox->hide();
    }

    if (ui->cellConfigTableWidget) {
        ui->cellConfigTableWidget->setColumnCount(7);
        ui->cellConfigTableWidget->setHorizontalHeaderLabels(
            {QStringLiteral("序号"),
             QStringLiteral("方号"),
             QStringLiteral("小区名称"),
             QStringLiteral("小区宽度(m)"),
             QStringLiteral("过道宽度(m)"),
             QStringLiteral("株距(cm)"),
             QStringLiteral("垄数")});
        ui->cellConfigTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        ui->cellConfigTableWidget->verticalHeader()->setVisible(false);
    }

    if (ui->seedingPlanTableWidget) {
        ui->seedingPlanTableWidget->setColumnCount(15);
        ui->seedingPlanTableWidget->setHorizontalHeaderLabels(
            {QStringLiteral("序号"),
             QStringLiteral("方号"),
             QStringLiteral("小区名称"),
             QStringLiteral("前分割线(m)"),
             QStringLiteral("后分割线(m)"),
             QStringLiteral("正向落种点(m)"),
             QStringLiteral("反向落种点(m)"),
             QStringLiteral("正向触发线(m)"),
             QStringLiteral("反向触发线(m)"),
             QStringLiteral("正向触发参数"),
             QStringLiteral("反向触发参数"),
             QStringLiteral("株距参数"),
             QStringLiteral("正向触发结果"),
             QStringLiteral("反向触发结果"),
             QStringLiteral("状态")});
        ui->seedingPlanTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        ui->seedingPlanTableWidget->horizontalHeader()->setStretchLastSection(true);
        ui->seedingPlanTableWidget->verticalHeader()->setVisible(false);
        ui->seedingPlanTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }

    if (ui->triggerAheadLabel) {
        ui->triggerAheadLabel->setVisible(true);
    }
    if (ui->triggerAheadLabelValue) {
        ui->triggerAheadLabelValue->setVisible(true);
        ui->triggerAheadLabelValue->setText(QStringLiteral("--"));
    }

    LOG_INFO("UI初始化完成");
}



/**
 * @brief 初始化坐标系统组件
 */
void MainWindow::initCoordinateWidget()
{
    LOG_INFO("正在初始化坐标系统组件...");

    auto invalidateCoordinateFrame = [this]() {
        hasLocalFrameConfigured = false;
        blockStartWorldPosition_ = QPointF(0.0, 0.0);

        if (triggerCommitTimer) {
            triggerCommitTimer->stop();
        }

        if (stateEstimator) {
            stateEstimator->reset();
            stateEstimator->setTriggerPlan(BlockPlanResult());
        }

        if (ui->mapWidget) {
            if (MapWidget* mapWidget = dynamic_cast<MapWidget*>(ui->mapWidget)) {
                mapWidget->setCoordinate(nullptr);
                mapWidget->clearPath();
            }
        }

        refreshTriggerUi(EstimatedPose());
        updatePlanObservationLabels(QVector<SeedingPlanRow>());
        if (ui->statusbar) {
            ui->statusbar->showMessage(QStringLiteral("本地坐标系参数已更新，请点击“生成本地坐标系”应用"), 3000);
        }
    };

    if (ui->oPointLineEdit) {
        connect(ui->oPointLineEdit, &QLineEdit::textChanged, this, invalidateCoordinateFrame);
    }
    if (ui->aPointLineEdit) {
        connect(ui->aPointLineEdit, &QLineEdit::textChanged, this, invalidateCoordinateFrame);
    }

    if (ui->mapWidget) {
        if (MapWidget* mapWidget = dynamic_cast<MapWidget*>(ui->mapWidget)) {
            mapWidget->setBlockPlanResult(BlockPlanResult());
            mapWidget->setCoordinate(nullptr);
        }
    }

    generatedSeedingPlanRows_.clear();
    lockedSeedingPlanRows_.clear();
    updatePlanObservationLabels(QVector<SeedingPlanRow>());
    triggerPlanResult_ = BlockPlanResult();
    highlightedPlanRow_ = -1;
    nextPlanRow_ = -1;
    lastProcessedTriggerSequence_ = 0;

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
    

    
    // plcConnectButton / plcDisconnectButton 通过 connectSlotsByName 自动连接，
    // 不再手动 connect，避免点击一次触发两次。

    // 连接PLC寄存器保存按钮信号
    connect(ui->plcSave1Button, &QPushButton::clicked, this, [this]() { this->on_plcSaveButton_clicked(1); });
    connect(ui->plcSave2Button, &QPushButton::clicked, this, [this]() { this->on_plcSaveButton_clicked(2); });
    connect(ui->plcSave3Button, &QPushButton::clicked, this, [this]() { this->on_plcSaveButton_clicked(3); });
    connect(ui->plcSave4Button, &QPushButton::clicked, this, [this]() { this->on_plcSaveButton_clicked(4); });
    connect(ui->plcSave5Button, &QPushButton::clicked, this, [this]() { this->on_plcSaveButton_clicked(5); });

    // 连接PLC寄存器发送按钮信号
    connect(ui->plcSend1Button, &QPushButton::clicked, this, [this]() { this->on_plcSendButton_clicked(1); });
    connect(ui->plcSend2Button, &QPushButton::clicked, this, [this]() { this->on_plcSendButton_clicked(2); });
    connect(ui->plcSend3Button, &QPushButton::clicked, this, [this]() { this->on_plcSendButton_clicked(3); });
    connect(ui->plcSend4Button, &QPushButton::clicked, this, [this]() { this->on_plcSendButton_clicked(4); });
    connect(ui->plcSend5Button, &QPushButton::clicked, this, [this]() { this->on_plcSendButton_clicked(5); });


    if (ui->mapWidget) {
        if (MapWidget* mapWidget = dynamic_cast<MapWidget*>(ui->mapWidget)) {
            if (QCheckBox* trajectoryCheckBox = findChild<QCheckBox*>(QStringLiteral("trajectoryVisibleCheckBox"))) {
                connect(trajectoryCheckBox, &QCheckBox::toggled, mapWidget, &MapWidget::setTrajectoryVisible);
                mapWidget->setTrajectoryVisible(trajectoryCheckBox->isChecked());
            }
        }
    }

    // 连接倒序复选框信号
    connect(ui->reverseOrderCheckBox, &QCheckBox::toggled, this, &MainWindow::on_reverseOrderCheckBox_toggled);
    
    // 连接生成播种计划按钮信号
    connect(ui->generateSeedingPlanButton, &QPushButton::clicked, this, &MainWindow::on_generateSeedingPlanButton_clicked);

    
    // 连接天线校准按钮信号
    connect(ui->antennaCalibrationButton, &QPushButton::clicked, this, &MainWindow::on_antennaCalibrationButton_clicked);

    
    // 连接锁定播种计划按钮信号
    connect(ui->lockSeedingPlanButton, &QPushButton::clicked, this, &MainWindow::on_lockSeedingPlanButton_clicked);

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

bool MainWindow::sendPlcCommandByIndex(int index, bool interactive, const QString& sourceContext)
{
    LOG_INFO("PLC发送请求: index=%d, interactive=%d, source=%s",
             index,
             interactive ? 1 : 0,
             sourceContext.toStdString().c_str());

    if (!modbusClient || modbusClient->state() != QModbusDevice::ConnectedState) {
        if (ui->statusbar) {
            ui->statusbar->showMessage(QStringLiteral("PLC未连接，无法发送指令"));
        }
        if (interactive) {
            QMessageBox::warning(this, QStringLiteral("发送失败"), QStringLiteral("PLC未连接，请先连接PLC"));
        }
        LOG_WARN("PLC未连接，无法发送: index=%d, source=%s", index, sourceContext.toStdString().c_str());
        return false;
    }

    const PlcCommandConfig config = plcCommandConfigForIndex(ui, index);
    if (!config.addressLineEdit || !config.valueLineEdit) {
        LOG_WARN("PLC配置不存在: index=%d", index);
        return false;
    }

    const QString addrStr = config.addressLineEdit->text().trimmed();
    const QString valueStr = config.valueLineEdit->text().trimmed();
    if (addrStr.isEmpty() || valueStr.isEmpty()) {
        if (interactive && ui->statusbar) {
            ui->statusbar->showMessage(QStringLiteral("请填写寄存器地址和指令值"));
        }
        LOG_WARN("寄存器参数无效: index=%d, addr=%s, value=%s, source=%s",
                 index,
                 addrStr.toStdString().c_str(),
                 valueStr.toStdString().c_str(),
                 sourceContext.toStdString().c_str());
        return false;
    }

    bool ok = false;
    const int address = addrStr.startsWith("0x", Qt::CaseInsensitive)
        ? addrStr.toInt(&ok, 16)
        : addrStr.toInt(&ok, 10);
    if (!ok || address < 0) {
        if (interactive && ui->statusbar) {
            ui->statusbar->showMessage(QStringLiteral("无效的寄存器地址"));
        }
        LOG_WARN("无效的寄存器地址: index=%d, addr=%s, source=%s",
                 index,
                 addrStr.toStdString().c_str(),
                 sourceContext.toStdString().c_str());
        return false;
    }

    bool valueOk = false;
    const quint16 value = valueStr.toUInt(&valueOk);
    if (!valueOk) {
        if (interactive && ui->statusbar) {
            ui->statusbar->showMessage(QStringLiteral("无效的指令值"));
        }
        LOG_WARN("无效的指令值: index=%d, value=%s, source=%s",
                 index,
                 valueStr.toStdString().c_str(),
                 sourceContext.toStdString().c_str());
        return false;
    }

    const bool restartReadTimer = plcReadTimer && plcReadTimer->isActive();
    if (restartReadTimer) {
        plcReadTimer->stop();
    }

    QModbusDataUnit writeUnit(QModbusDataUnit::HoldingRegisters, address, QList<quint16>{value});
    LOG_INFO("正在写入寄存器: index=%d, address=%d, value=%d, source=%s",
             index,
             address,
             value,
             sourceContext.toStdString().c_str());

    if (auto *reply = modbusClient->sendWriteRequest(writeUnit, 1)) {
        if (!reply->isFinished()) {
            connect(reply, &QModbusReply::finished, this, [this, reply, index, address, value, restartReadTimer, interactive, sourceContext]() {
                if (reply->error() == QModbusDevice::NoError) {
                    LOG_INFO("寄存器写入成功: index=%d, addr=%d, value=%d, source=%s",
                             index,
                             address,
                             value,
                             sourceContext.toStdString().c_str());
                    this->plcSendCount++;
                    if (this->ui->statusbar) {
                        this->ui->statusbar->showMessage(
                            QStringLiteral("寄存器%1写入成功: 地址=%2, 值=%3")
                                .arg(index)
                                .arg(address)
                                .arg(value));
                    }
                    this->updatePlcReadValue(index, value);

                    if (this->ui->plcConfigLabel) {
                        this->ui->plcConfigLabel->setText(
                            QStringLiteral("PLC发送计数: %1, 接收计数: %2")
                                .arg(this->plcSendCount)
                                .arg(this->plcReadCount));
                    }
                } else {
                    LOG_Error("寄存器写入失败: index=%d, addr=%d, error=%s, source=%s",
                              index,
                              address,
                              reply->errorString().toStdString().c_str(),
                              sourceContext.toStdString().c_str());
                    if (this->ui->statusbar) {
                        this->ui->statusbar->showMessage(
                            QStringLiteral("寄存器%1写入失败: %2")
                                .arg(index)
                                .arg(reply->errorString()));
                    }
                    if (interactive) {
                        QMessageBox::warning(this,
                                             QStringLiteral("写入失败"),
                                             QStringLiteral("寄存器%1写入失败:\n%2")
                                                 .arg(index)
                                                 .arg(reply->errorString()));
                    }
                }

                if (restartReadTimer && this->plcReadTimer) {
                    this->plcReadTimer->start(500);
                }
                reply->deleteLater();
            });
        } else {
            if (restartReadTimer && plcReadTimer) {
                plcReadTimer->start(500);
            }
            reply->deleteLater();
        }
        return true;
    }

    if (restartReadTimer && plcReadTimer) {
        plcReadTimer->start(500);
    }
    LOG_Error("发送写请求失败: index=%d, source=%s", index, sourceContext.toStdString().c_str());
    return false;
}

void MainWindow::triggerConfiguredPlcActions(const EstimatedPose& pose)
{
    if (!pose.hasLastTriggerEvent) {
        return;
    }

    const bool isForward = pose.lastTriggerDirection > 0;
    const QString sourceContext = QStringLiteral("trigger seq=%1 line=%2 direction=%3")
        .arg(pose.triggerSequence)
        .arg(pose.lastTriggerLineIndex)
        .arg(pose.lastTriggerDirection);

    for (int index = 1; index <= 5; ++index) {
        const PlcCommandConfig config = plcCommandConfigForIndex(ui, index);
        QCheckBox* targetCheckBox = isForward ? config.forwardCheckBox : config.reverseCheckBox;
        if (!targetCheckBox || !targetCheckBox->isChecked()) {
            continue;
        }
        sendPlcCommandByIndex(index, false, sourceContext);
    }
}

bool MainWindow::parseCoordinateLineEdit(QLineEdit* lineEdit,
                                         double& latitude,
                                         double& longitude,
                                         const QString& fieldName,
                                         bool showMessage) const
{
    QString errorMessage;
    if (!lineEdit) {
        errorMessage = QStringLiteral("%1输入框不存在").arg(fieldName);
    } else {
        const QStringList parts = normalizeSeparators(lineEdit->text()).split(',', Qt::SkipEmptyParts);
        if (parts.size() != 2) {
            errorMessage = QStringLiteral("%1格式应为“纬度,经度”").arg(fieldName);
        } else {
            bool latOk = false;
            bool lonOk = false;
            latitude = parts[0].trimmed().toDouble(&latOk);
            longitude = parts[1].trimmed().toDouble(&lonOk);
            if (!latOk || !lonOk || !std::isfinite(latitude) || !std::isfinite(longitude)
                || std::abs(latitude) > 90.0 || std::abs(longitude) > 180.0) {
                errorMessage = QStringLiteral("%1坐标无效，请输入合法的纬度和经度").arg(fieldName);
            }
        }
    }

    if (!errorMessage.isEmpty()) {
        if (showMessage) {
            QMessageBox::warning(const_cast<MainWindow*>(this), QStringLiteral("输入错误"), errorMessage);
        }
        return false;
    }
    return true;
}

bool MainWindow::parseDoubleLineEdit(QLineEdit* lineEdit,
                                     const QString& fieldName,
                                     double minValue,
                                     double& value,
                                     QString* errorMessage) const
{
    if (!lineEdit) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("%1输入框不存在").arg(fieldName);
        }
        return false;
    }

    bool ok = false;
    const QString text = lineEdit->text().trimmed();
    value = text.toDouble(&ok);
    if (!ok || !std::isfinite(value) || value < minValue) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("%1必须是大于等于%2的数字").arg(fieldName).arg(minValue);
        }
        return false;
    }
    return true;
}

bool MainWindow::parseIntLineEdit(QLineEdit* lineEdit,
                                  const QString& fieldName,
                                  int minValue,
                                  int& value,
                                  QString* errorMessage) const
{
    if (!lineEdit) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("%1输入框不存在").arg(fieldName);
        }
        return false;
    }

    bool ok = false;
    const QString text = lineEdit->text().trimmed();
    value = text.toInt(&ok);
    if (!ok || value < minValue) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("%1必须是不小于%2的整数").arg(fieldName).arg(minValue);
        }
        return false;
    }
    return true;
}

bool MainWindow::parseThetaOmega(const QString& text,
                                 double& thetaDisk,
                                 double& omega0,
                                 QString* errorMessage) const
{
    const QStringList parts = normalizeSeparators(text).split(',', Qt::SkipEmptyParts);
    if (parts.size() != 2) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("转速参数格式应为“θ,ω”");
        }
        return false;
    }

    bool thetaOk = false;
    bool omegaOk = false;
    thetaDisk = parts[0].trimmed().toDouble(&thetaOk);
    omega0 = parts[1].trimmed().toDouble(&omegaOk);
    if (!thetaOk || !omegaOk || !std::isfinite(thetaDisk) || !std::isfinite(omega0)) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("转速参数包含无效数字");
        }
        return false;
    }
    return true;
}

bool MainWindow::parseAntennaArm(const QString& text,
                                 QPointF& xy,
                                 double& z,
                                 QString* errorMessage) const
{
    const QStringList parts = normalizeSeparators(text).split(',', Qt::SkipEmptyParts);
    if (parts.size() != 3) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("天线杆臂格式应为“X,Y,Z”");
        }
        return false;
    }

    bool xOk = false;
    bool yOk = false;
    bool zOk = false;
    const double x = parts[0].trimmed().toDouble(&xOk);
    const double y = parts[1].trimmed().toDouble(&yOk);
    z = parts[2].trimmed().toDouble(&zOk);
    if (!xOk || !yOk || !zOk || !std::isfinite(x) || !std::isfinite(y) || !std::isfinite(z)) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("天线杆臂包含无效数字");
        }
        return false;
    }

    xy = QPointF(x, y);
    return true;
}

bool MainWindow::parseCellConfigTable(QVector<CellConfigRow>& rows, QString* errorMessage) const
{
    rows.clear();
    if (!ui->cellConfigTableWidget) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("小区配置表不存在");
        }
        return false;
    }

    double lastSpacingCm = 0.0;
    for (int rowIndex = 0; rowIndex < ui->cellConfigTableWidget->rowCount(); ++rowIndex) {
        auto itemText = [this, rowIndex](int column) {
            QTableWidgetItem* item = ui->cellConfigTableWidget->item(rowIndex, column);
            return item ? item->text().trimmed() : QString();
        };

        const QString blockNoText = itemText(1);
        const QString blockNameText = itemText(2);
        const QString blockWidthText = itemText(3);
        const QString clearanceText = itemText(4);
        const QString spacingText = itemText(5);
        const QString ridgesText = itemText(6);

        if (blockNoText.isEmpty() && blockNameText.isEmpty() && blockWidthText.isEmpty()
            && clearanceText.isEmpty() && spacingText.isEmpty() && ridgesText.isEmpty()) {
            continue;
        }

        CellConfigRow row;
        bool ok = false;
        row.blockNo = blockNoText.toInt(&ok);
        if (!ok) {
            if (errorMessage) {
                *errorMessage = QStringLiteral("第%1行方号无效").arg(rowIndex + 1);
            }
            return false;
        }

        row.blockName = blockNameText.isEmpty()
            ? (row.blockNo == -1 ? QStringLiteral("展示区") : QStringLiteral("小区%1").arg(row.blockNo))
            : blockNameText;

        row.blockWidthM = blockWidthText.toDouble(&ok);
        if (!ok || !std::isfinite(row.blockWidthM) || row.blockWidthM <= 0.0) {
            if (errorMessage) {
                *errorMessage = QStringLiteral("第%1行小区宽度无效").arg(rowIndex + 1);
            }
            return false;
        }

        if (clearanceText.isEmpty()) {
            row.clearanceM = 1.0;
        } else {
            row.clearanceM = clearanceText.toDouble(&ok);
            if (!ok || !std::isfinite(row.clearanceM) || row.clearanceM <= 0.0) {
                if (errorMessage) {
                    *errorMessage = QStringLiteral("第%1行过道宽度无效").arg(rowIndex + 1);
                }
                return false;
            }
        }

        if (spacingText.isEmpty()) {
            if (row.blockNo == -1 && lastSpacingCm > 0.0) {
                row.spacingCm = lastSpacingCm;
            } else {
                if (errorMessage) {
                    *errorMessage = QStringLiteral("第%1行株距为空").arg(rowIndex + 1);
                }
                return false;
            }
        } else {
            row.spacingCm = spacingText.toDouble(&ok);
            if (!ok || !std::isfinite(row.spacingCm) || row.spacingCm <= 0.0) {
                if (errorMessage) {
                    *errorMessage = QStringLiteral("第%1行株距无效").arg(rowIndex + 1);
                }
                return false;
            }
            lastSpacingCm = row.spacingCm;
        }

        row.ridges = ridgesText.toInt(&ok);
        if (!ok || row.ridges <= 0) {
            if (errorMessage) {
                *errorMessage = QStringLiteral("第%1行垄数无效").arg(rowIndex + 1);
            }
            return false;
        }

        rows.push_back(row);
    }

    if (rows.isEmpty()) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("小区配置表为空");
        }
        return false;
    }
    return true;
}

bool MainWindow::parseSeederParams(SeederSystemParams& params, QString* errorMessage) const
{
    double seedHoleDistanceCm = 0.0;
    double seedRadiusCm = 0.0;
    double transmissionDelayMs = 0.0;

    if (!parseDoubleLineEdit(ui->rowSpacingLineEdit, QStringLiteral("垄距"), 0.001, params.rowSpacingM, errorMessage)
        || !parseDoubleLineEdit(ui->seedHoleDistanceLineEdit, QStringLiteral("排种器孔距(cm)"), 0.001, seedHoleDistanceCm, errorMessage)
        || !parseThetaOmega(ui->seedSpeedRatioLineEdit ? ui->seedSpeedRatioLineEdit->text() : QString(), params.thetaDisk, params.omega0, errorMessage)
        || !parseDoubleLineEdit(ui->seedRadiusLineEdit, QStringLiteral("排种器半径(cm)"), 0.0, seedRadiusCm, errorMessage)
        || !parseIntLineEdit(ui->seedAdsorbCountLineEdit, QStringLiteral("排种器吸附数"), 1, params.adsorbCount, errorMessage)
        || !parseDoubleLineEdit(ui->seedDropHeightLineEdit, QStringLiteral("落种口高度(m)"), 0.0, params.seedDropHeightM, errorMessage)
        || !parseAntennaArm(ui->antennaArmLineEdit ? ui->antennaArmLineEdit->text() : QString(), params.antennaArmXY, params.antennaArmZ, errorMessage)
        || !parseDoubleLineEdit(ui->distancePulseRatioLineEdit, QStringLiteral("距离脉冲比例"), 0.0, params.distancePulseRatio, errorMessage)
        || !parseDoubleLineEdit(ui->signalTransmissionDelayLineEdit, QStringLiteral("信号传输延迟(ms)"), 0.0, transmissionDelayMs, errorMessage)) {
        return false;
    }

    params.seedHoleDistanceM = seedHoleDistanceCm / 100.0;
    params.seedRadiusM = seedRadiusCm / 100.0;
    params.transmissionDelayS = transmissionDelayMs / 1000.0;
    return true;
}

double MainWindow::currentReferenceSpeedMps() const
{
    if (hasValidGnssData && latestGnssData.isvalid) {
        return std::hypot(latestGnssData.vel.x(), latestGnssData.vel.y());
    }
    if (stateEstimator) {
        const EstimatedPose pose = stateEstimator->getPose();
        return std::hypot(pose.vx, pose.vy);
    }
    return 0.0;
}

double MainWindow::safeReferenceSpeedMps() const
{
    const double speedMps = currentReferenceSpeedMps();
    return speedMps >= kMinReferenceSpeedMps ? speedMps : kFallbackReferenceSpeedMps;
}

QVector<MainWindow::SeedingPlanRow> MainWindow::buildSeedingPlanRows(const QVector<CellConfigRow>& rows,
                                                                     const SeederSystemParams& params,
                                                                     double speedMps,
                                                                     QString* errorMessage) const
{
    QVector<SeedingPlanRow> planRows;
    if (rows.isEmpty()) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("没有可用的小区配置");
        }
        return planRows;
    }

    const double usedSpeedMps = std::max(speedMps, kMinReferenceSpeedMps);
    double previousClearanceM = kDefaultLeadingClearanceM;
    double currentForCutlineY = 0.0;
    double lastSpacingCm = rows.first().spacingCm;

    for (int i = 0; i < rows.size(); ++i) {
        const CellConfigRow& source = rows[i];
        SeedingPlanRow row;
        row.blockId = i + 1;
        row.blockNo = source.blockNo;
        row.blockName = source.blockName;
        row.blockWidthM = source.blockWidthM;
        row.clearanceM = source.clearanceM;
        row.spacingCm = source.spacingCm > 0.0 ? source.spacingCm : lastSpacingCm;
        row.ridges = source.ridges;

        const double spacingM = row.spacingCm / 100.0;
        double thetaDisk = params.thetaDisk;
        if (spacingM > 1e-9) {
            thetaDisk = params.seedHoleDistanceM / spacingM - params.omega0 / usedSpeedMps;
        }
        row.thetaDisk = thetaDisk;

        const double omegaDisk = usedSpeedMps * row.thetaDisk + params.omega0;
        row.x1DistanceM = spacingM * std::max(0, params.adsorbCount - 1);

        const double seedExitSpeed = params.seedRadiusM * omegaDisk;
        const double discriminant = seedExitSpeed * seedExitSpeed + 2.0 * kGravityMps2 * std::max(0.0, params.seedDropHeightM);
        const double fallTime = discriminant > 0.0
            ? std::max(0.0, (-seedExitSpeed + std::sqrt(discriminant)) / kGravityMps2)
            : 0.0;
        row.x2DistanceM = usedSpeedMps * fallTime;
        row.x4DistanceM = usedSpeedMps * params.transmissionDelayS;
        row.x5DistanceM = params.antennaArmXY.y();
        row.effectiveAdvanceM = row.x1DistanceM + row.x2DistanceM + row.x4DistanceM + row.x5DistanceM;

        row.forCutlineY = currentForCutlineY;
        row.revCutlineY = row.forCutlineY + row.blockWidthM + 0.5 * (previousClearanceM + row.clearanceM);
        row.forwardSeedDropY = row.forCutlineY + previousClearanceM * 0.5;
        row.reverseSeedDropY = row.revCutlineY - row.clearanceM * 0.5;
        row.forwardTriggerY = row.forCutlineY - (row.effectiveAdvanceM + previousClearanceM * 0.5);
        row.reverseTriggerY = row.revCutlineY + row.effectiveAdvanceM + row.clearanceM * 0.5;
        row.forwardTriggerParam = previousClearanceM * params.distancePulseRatio;
        row.reverseTriggerParam = row.clearanceM * params.distancePulseRatio;
        row.forwardLineIndex = row.blockId * 2 - 1;
        row.reverseLineIndex = row.blockId * 2;
        row.statusText = QStringLiteral("静态基准");

        planRows.push_back(row);
        currentForCutlineY = row.revCutlineY;
        previousClearanceM = row.clearanceM;
        lastSpacingCm = row.spacingCm;
    }

    return planRows;
}

BlockPlanResult MainWindow::buildBlockPlanResultFromPlanRows(const QVector<SeedingPlanRow>& rows,
                                                             const SeederSystemParams& params) const
{
    BlockPlanResult result;
    if (rows.isEmpty()) {
        result.errorMessage = "empty seeding plan";
        return result;
    }

    double totalWidth = 0.0;
    double totalLength = 0.0;
    int totalRows = 0;

    for (const SeedingPlanRow& row : rows) {
        totalWidth = std::max(totalWidth, row.ridges * params.rowSpacingM);
        totalLength = std::max(totalLength, std::max(row.revCutlineY, row.reverseTriggerY));
        totalRows = std::max(totalRows, row.ridges);
    }

    result.totalWidth = totalWidth;
    result.totalLength = totalLength;
    result.totalBlocks = rows.size();
    result.totalRows = totalRows;
    result.triggerStartY = rows.first().forwardTriggerY;
    result.triggerStopY = rows.last().reverseTriggerY;
    result.triggerAdvanceOffset = rows.first().effectiveAdvanceM;

    int headlandId = 1;
    for (const SeedingPlanRow& row : rows) {
        const double blockWidthX = row.ridges * params.rowSpacingM;
        const double blockStartY = row.forwardSeedDropY;
        const double blockEndY = row.reverseSeedDropY;

        BlockSpec block;
        block.blockId = row.blockId;
        block.startRow = 1;
        block.endRow = row.ridges;
        block.yStart = blockStartY;
        block.yEnd = blockEndY;
        block.width = blockWidthX;
        block.length = std::max(0.0, blockEndY - blockStartY);
        block.name = row.blockName.toStdString();
        result.blocks.push_back(block);

        BlockBoundary boundary;
        boundary.blockId = row.blockId;
        boundary.forwardBoundary = {
            {0.0, blockStartY},
            {blockWidthX, blockStartY},
            {blockWidthX, blockEndY},
            {0.0, blockEndY}
        };
        boundary.reverseBoundary = {
            {0.0, blockEndY},
            {blockWidthX, blockEndY},
            {blockWidthX, blockStartY},
            {0.0, blockStartY}
        };
        result.boundaries.push_back(boundary);

        HeadlandSpec headland;
        headland.headlandId = headlandId++;
        headland.yStart = row.forCutlineY - row.clearanceM * 0.5;
        headland.yEnd = row.forCutlineY + row.clearanceM * 0.5;
        headland.width = row.clearanceM;
        result.headlands.push_back(headland);

        TriggerLineSpec forwardLine;
        forwardLine.lineIndex = row.forwardLineIndex;
        forwardLine.blockId = row.blockId;
        forwardLine.direction = 1;
        forwardLine.y = row.forwardTriggerY;
        forwardLine.boundaryY = row.forwardSeedDropY;
        forwardLine.xStart = 0.0;
        forwardLine.xEnd = blockWidthX;
        forwardLine.blockName = row.blockName.toStdString();
        result.triggerLines.push_back(forwardLine);

        TriggerLineSpec reverseLine;
        reverseLine.lineIndex = row.reverseLineIndex;
        reverseLine.blockId = row.blockId;
        reverseLine.direction = -1;
        reverseLine.y = row.reverseTriggerY;
        reverseLine.boundaryY = row.reverseSeedDropY;
        reverseLine.xStart = 0.0;
        reverseLine.xEnd = blockWidthX;
        reverseLine.blockName = row.blockName.toStdString();
        result.triggerLines.push_back(reverseLine);
    }

    std::sort(result.triggerLines.begin(), result.triggerLines.end(), [](const TriggerLineSpec& lhs, const TriggerLineSpec& rhs) {
        if (std::abs(lhs.y - rhs.y) > 1e-9) {
            return lhs.y < rhs.y;
        }
        return lhs.lineIndex < rhs.lineIndex;
    });

    result.isValid = true;
    return result;
}

void MainWindow::populateSeedingPlanTable(const QVector<SeedingPlanRow>& rows)
{
    if (!ui->seedingPlanTableWidget) {
        return;
    }

    auto ensureItem = [this](int row, int column) {
        QTableWidgetItem* item = ui->seedingPlanTableWidget->item(row, column);
        if (!item) {
            item = new QTableWidgetItem();
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            ui->seedingPlanTableWidget->setItem(row, column, item);
        }
        return item;
    };

    ui->seedingPlanTableWidget->setRowCount(rows.size());
    for (int rowIndex = 0; rowIndex < rows.size(); ++rowIndex) {
        const SeedingPlanRow& row = rows[rowIndex];
        ensureItem(rowIndex, 0)->setText(QString::number(rowIndex + 1));
        ensureItem(rowIndex, 1)->setText(QString::number(row.blockNo));
        ensureItem(rowIndex, 2)->setText(row.blockName);
        ensureItem(rowIndex, 3)->setText(formatPlanDouble(row.forCutlineY));
        ensureItem(rowIndex, 4)->setText(formatPlanDouble(row.revCutlineY));
        ensureItem(rowIndex, 5)->setText(formatPlanDouble(row.forwardSeedDropY));
        ensureItem(rowIndex, 6)->setText(formatPlanDouble(row.reverseSeedDropY));
        ensureItem(rowIndex, 7)->setText(formatPlanDouble(row.forwardTriggerY));
        ensureItem(rowIndex, 8)->setText(formatPlanDouble(row.reverseTriggerY));
        ensureItem(rowIndex, 9)->setText(formatPlanDouble(row.forwardTriggerParam));
        ensureItem(rowIndex, 10)->setText(formatPlanDouble(row.reverseTriggerParam));
        ensureItem(rowIndex, 11)->setText(formatPlanDouble(row.thetaDisk));
        ensureItem(rowIndex, 12)->setText(formatTriggerResultText(row.forwardTriggerCoord, row.forwardTriggerTime));
        ensureItem(rowIndex, 13)->setText(formatTriggerResultText(row.reverseTriggerCoord, row.reverseTriggerTime));
        ensureItem(rowIndex, 14)->setText(row.statusText);

        const QString tooltip = QStringLiteral("X1=%1 m\nX2=%2 m\nX4=%3 m\nX5=%4 m\n综合提前量=%5 m")
            .arg(formatPlanDouble(row.x1DistanceM))
            .arg(formatPlanDouble(row.x2DistanceM))
            .arg(formatPlanDouble(row.x4DistanceM))
            .arg(formatPlanDouble(row.x5DistanceM))
            .arg(formatPlanDouble(row.effectiveAdvanceM));
        for (int column = 3; column <= 11; ++column) {
            ensureItem(rowIndex, column)->setToolTip(tooltip);
        }
    }

    if (ui->triggerAheadLabelValue) {
        const int displayRow = highlightedPlanRow_ >= 0 && highlightedPlanRow_ < rows.size() ? highlightedPlanRow_ : 0;
        ui->triggerAheadLabelValue->setText(rows.isEmpty() ? QStringLiteral("--") : formatPlanDouble(rows[displayRow].effectiveAdvanceM));
    }
}

void MainWindow::updatePlanStatusDisplay(const QVector<SeedingPlanRow>& rows, int currentRow, int nextRow)
{
    highlightedPlanRow_ = currentRow;
    nextPlanRow_ = nextRow;

    if (!ui->seedingPlanTableWidget) {
        return;
    }

    const QBrush normalBrush(Qt::white);
    const QBrush currentBrush(QColor(255, 245, 157));
    const QBrush nextBrush(QColor(187, 222, 251));

    for (int rowIndex = 0; rowIndex < ui->seedingPlanTableWidget->rowCount(); ++rowIndex) {
        QString status;
        if (rowIndex >= 0 && rowIndex < rows.size()) {
            status = rows[rowIndex].statusText;
        }
        if (rowIndex == currentRow) {
            status = QStringLiteral("当前小区");
        } else if (rowIndex == nextRow) {
            status = QStringLiteral("下一小区");
        } else if (status.isEmpty()) {
            status = seedingPlanLocked_ ? QStringLiteral("已锁定") : QStringLiteral("静态基准");
        }

        QBrush brush = normalBrush;
        if (rowIndex == currentRow) {
            brush = currentBrush;
        } else if (rowIndex == nextRow) {
            brush = nextBrush;
        }

        for (int column = 0; column < ui->seedingPlanTableWidget->columnCount(); ++column) {
            QTableWidgetItem* item = ui->seedingPlanTableWidget->item(rowIndex, column);
            if (!item) {
                item = new QTableWidgetItem();
                item->setFlags(item->flags() & ~Qt::ItemIsEditable);
                ui->seedingPlanTableWidget->setItem(rowIndex, column, item);
            }
            item->setBackground(brush);
            if (column == 14) {
                item->setText(status);
            }
        }
    }
}

void MainWindow::setSeedingPlanEditingLocked(bool locked)
{
    seedingPlanLocked_ = locked;

    if (ui->cellConfigTableWidget) {
        ui->cellConfigTableWidget->setEditTriggers(
            locked ? QAbstractItemView::NoEditTriggers
                   : (QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed));
    }
    if (ui->seedingPlanTableWidget) {
        ui->seedingPlanTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }
    if (ui->reverseOrderCheckBox) {
        ui->reverseOrderCheckBox->setEnabled(!locked);
    }

    const QList<QLineEdit*> lineEdits = {
        ui->rowSpacingLineEdit,
        ui->seedHoleDistanceLineEdit,
        ui->seedSpeedRatioLineEdit,
        ui->seedRadiusLineEdit,
        ui->seedAdsorbCountLineEdit,
        ui->seedDropHeightLineEdit,
        ui->antennaArmLineEdit,
        ui->distancePulseRatioLineEdit,
        ui->signalTransmissionDelayLineEdit
    };
    for (QLineEdit* lineEdit : lineEdits) {
        if (lineEdit) {
            lineEdit->setReadOnly(locked);
        }
    }
}

void MainWindow::updateCellAreaLabel()
{
    if (!ui->cellAreaLabel) {
        return;
    }

    if (generatedSeedingPlanRows_.isEmpty()) {
        if (!cellConfigRows_.isEmpty()) {
            ui->cellAreaLabel->setText(QStringLiteral("小区总长宽: 待生成播种计划"));
        } else {
            ui->cellAreaLabel->setText(QStringLiteral("小区总长宽: --"));
        }
        return;
    }

    double rowSpacingM = 0.0;
    if (!parseDoubleLineEdit(ui->rowSpacingLineEdit, QStringLiteral("垄距"), 0.001, rowSpacingM, nullptr)) {
        ui->cellAreaLabel->setText(QStringLiteral("小区总长宽: --"));
        return;
    }

    int maxRidges = 0;
    for (const CellConfigRow& row : cellConfigRows_) {
        maxRidges = std::max(maxRidges, row.ridges);
    }
    if (maxRidges <= 0) {
        ui->cellAreaLabel->setText(QStringLiteral("小区总长宽: --"));
        return;
    }

    const double totalLength = generatedSeedingPlanRows_.last().revCutlineY;
    const double totalWidth = rowSpacingM * static_cast<double>(maxRidges);
    ui->cellAreaLabel->setText(
        QStringLiteral("小区总长宽: 长 %1 m，宽 %2 m")
            .arg(totalLength, 0, 'f', 2)
            .arg(totalWidth, 0, 'f', 2));
}

void MainWindow::updatePlanObservationLabels(const QVector<SeedingPlanRow>& rows, int preferredRow, int secondaryRow)
{
    auto setFallback = [this]() {
        if (ui->x1ValueLabel) {
            ui->x1ValueLabel->setText(QStringLiteral("X1值: --"));
        }
        if (ui->x2ValueLabel) {
            ui->x2ValueLabel->setText(QStringLiteral("X2值: --"));
        }
    };

    if (rows.isEmpty()) {
        setFallback();
        return;
    }

    int chosenRow = -1;
    const auto isValidRow = [&rows](int row) {
        return row >= 0 && row < rows.size();
    };

    if (isValidRow(preferredRow)) {
        chosenRow = preferredRow;
    } else if (isValidRow(secondaryRow)) {
        chosenRow = secondaryRow;
    } else {
        chosenRow = rows.size() - 1;
    }

    const SeedingPlanRow& row = rows[chosenRow];
    if (ui->x1ValueLabel) {
        ui->x1ValueLabel->setText(QStringLiteral("X1值: %1 m").arg(row.x1DistanceM, 0, 'f', 4));
    }
    if (ui->x2ValueLabel) {
        ui->x2ValueLabel->setText(QStringLiteral("X2值: %1 m").arg(row.x2DistanceM, 0, 'f', 4));
    }
}

int MainWindow::findCurrentPlanRow(double y) const
{
    const QVector<SeedingPlanRow>& rows = seedingPlanLocked_ && !lockedSeedingPlanRows_.isEmpty()
        ? lockedSeedingPlanRows_
        : generatedSeedingPlanRows_;
    for (int i = 0; i < rows.size(); ++i) {
        if (y >= rows[i].forCutlineY && y <= rows[i].revCutlineY) {
            return i;
        }
    }
    return -1;
}

void MainWindow::refreshDynamicTriggerPlan(const EstimatedPose& pose)
{
    if (!seedingPlanLocked_ || cellConfigRows_.isEmpty() || lockedSeedingPlanRows_.isEmpty()) {
        return;
    }

    SeederSystemParams params;
    if (!parseSeederParams(params, nullptr)) {
        return;
    }

    QString errorMessage;
    const double speedMps = safeReferenceSpeedMps();
    const QVector<SeedingPlanRow> recalculatedRows = buildSeedingPlanRows(cellConfigRows_, params, speedMps, &errorMessage);
    if (recalculatedRows.size() != generatedSeedingPlanRows_.size()) {
        return;
    }

    QVector<SeedingPlanRow> runtimeRows = generatedSeedingPlanRows_;
    for (int i = 0; i < runtimeRows.size() && i < lockedSeedingPlanRows_.size(); ++i) {
        runtimeRows[i].forwardTriggerCoord = lockedSeedingPlanRows_[i].forwardTriggerCoord;
        runtimeRows[i].forwardTriggerTime = lockedSeedingPlanRows_[i].forwardTriggerTime;
        runtimeRows[i].reverseTriggerCoord = lockedSeedingPlanRows_[i].reverseTriggerCoord;
        runtimeRows[i].reverseTriggerTime = lockedSeedingPlanRows_[i].reverseTriggerTime;
    }

    int direction = 0;
    if (pose.vy > kMinReferenceSpeedMps) {
        direction = 1;
    } else if (pose.vy < -kMinReferenceSpeedMps) {
        direction = -1;
    }

    const int currentRow = findCurrentPlanRow(pose.y);
    int nextRow = -1;
    if (currentRow >= 0 && direction != 0) {
        nextRow = currentRow + direction;
        if (nextRow < 0 || nextRow >= runtimeRows.size()) {
            nextRow = -1;
        }
    }

    auto applyComputedFields = [](SeedingPlanRow& target, const SeedingPlanRow& source) {
        target.forCutlineY = source.forCutlineY;
        target.revCutlineY = source.revCutlineY;
        target.forwardSeedDropY = source.forwardSeedDropY;
        target.reverseSeedDropY = source.reverseSeedDropY;
        target.forwardTriggerY = source.forwardTriggerY;
        target.reverseTriggerY = source.reverseTriggerY;
        target.forwardTriggerParam = source.forwardTriggerParam;
        target.reverseTriggerParam = source.reverseTriggerParam;
        target.thetaDisk = source.thetaDisk;
        target.x1DistanceM = source.x1DistanceM;
        target.x2DistanceM = source.x2DistanceM;
        target.x4DistanceM = source.x4DistanceM;
        target.x5DistanceM = source.x5DistanceM;
        target.effectiveAdvanceM = source.effectiveAdvanceM;
        target.forwardLineIndex = source.forwardLineIndex;
        target.reverseLineIndex = source.reverseLineIndex;
    };

    if (currentRow >= 0 && currentRow < runtimeRows.size()) {
        applyComputedFields(runtimeRows[currentRow], recalculatedRows[currentRow]);
    }
    if (nextRow >= 0 && nextRow < runtimeRows.size()) {
        applyComputedFields(runtimeRows[nextRow], recalculatedRows[nextRow]);
    }

    for (int i = 0; i < runtimeRows.size(); ++i) {
        if (i == currentRow) {
            runtimeRows[i].statusText = QStringLiteral("当前小区");
        } else if (i == nextRow) {
            runtimeRows[i].statusText = QStringLiteral("下一小区");
        } else {
            runtimeRows[i].statusText = QStringLiteral("已锁定");
        }
    }

    const BlockPlanResult runtimePlan = buildBlockPlanResultFromPlanRows(runtimeRows, params);
    if (runtimePlan.isValid) {
        if (stateEstimator) {
            stateEstimator->updatePendingTriggerPlan(runtimePlan);
        }
        triggerPlanResult_ = runtimePlan;
        if (MapWidget* mapWidget = dynamic_cast<MapWidget*>(ui->mapWidget)) {
            mapWidget->setBlockPlanResult(triggerPlanResult_);
        }
    }

    lockedSeedingPlanRows_ = runtimeRows;
    dynamicPlanSpeedMps_ = speedMps;
    populateSeedingPlanTable(lockedSeedingPlanRows_);
    updatePlanStatusDisplay(lockedSeedingPlanRows_, currentRow, nextRow);
    updatePlanObservationLabels(lockedSeedingPlanRows_, currentRow, nextRow);
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
        ui->imuConfigLabel->setText(QString("IMU有效数据: %1").arg(imuDataCount));

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

        EstimatedPose triggerPose{};
        bool hasEstimatorPose = false;

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
            triggerPose = stateEstimator->getPose();
            hasEstimatorPose = true;
            refreshDynamicTriggerPlan(triggerPose);
            handleCommittedTrigger(triggerPose);
            if (MapWidget* mapWidget = dynamic_cast<MapWidget*>(ui->mapWidget)) {
                mapWidget->updateVehiclePose(triggerPose);
            }
            refreshTriggerUi(triggerPose);

            if (triggerCommitTimer) {
                if (triggerPose.predictedTriggerTimeUs > 0 && triggerPose.nextTriggerCountdownSec >= 0.0) {
                    const qint64 nowUs = static_cast<qint64>(std::llround(sensorData.timestamp * 1e6));
                    const qint64 delayUs = triggerPose.predictedTriggerTimeUs - nowUs;
                    const int delayMs = std::max(0, static_cast<int>((delayUs + 999) / 1000));
                    if (!triggerCommitTimer->isActive() || std::abs(triggerCommitTimer->remainingTime() - delayMs) > 20) {
                        triggerCommitTimer->start(delayMs);
                    }
                } else {
                    triggerCommitTimer->stop();
                }
            }
        }

        const GNSS& dataToUse = lastValidGnssData;

        // ---------- timeAndSensorGroupBox（概要信息）----------
        ui->gpsDataCountLabel->setText(QString("有效GPS数据: %1").arg(gpsDataCount));
        ui->gpsConfigLabel->setText(QString("GPS有效数据: %1").arg(gpsDataCount));

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

        if (!hasEstimatorPose) {
            refreshTriggerUi(EstimatedPose());
        }

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
 * @brief 处理PLC连接按钮点击事件
 */
void MainWindow::on_plcConnectButton_clicked()
{
    LOG_INFO("PLC连接按钮被点击");

    // 从UI获取PLC网络参数
    QString plcIp = ui->plcIpLineEdit->text().trimmed();
    QString plcPortStr = ui->plcPortLineEdit->text().trimmed();

    if (plcIp == "0.0.0.0") {
        plcIp = "127.0.0.1";
        ui->plcIpLineEdit->setText(plcIp);
        LOG_WARN("PLC目标地址 0.0.0.0 无效，已自动改为 127.0.0.1");
    }

    // 验证参数
    if (plcIp.isEmpty() || plcPortStr.isEmpty()) {
        ui->statusbar->showMessage("请填写有效的IP地址和端口");
        LOG_WARN("PLC网络参数无效: ip=%s, port=%s", plcIp.toStdString().c_str(), plcPortStr.toStdString().c_str());
        return;
    }

    int plcPort = plcPortStr.toInt();

    // 如果已存在连接，先断开
    if (modbusClient) {
        if (modbusClient->state() == QModbusDevice::ConnectedState) {
            modbusClient->disconnectDevice();
        }
        delete modbusClient;
        modbusClient = nullptr;
    }

    // 创建Modbus TCP客户端
    modbusClient = new QModbusTcpClient(this);

    // 连接信号槽
    connect(modbusClient, &QModbusTcpClient::stateChanged, this, &MainWindow::onModbusStateChanged);
    connect(modbusClient, &QModbusTcpClient::errorOccurred, this, &MainWindow::onModbusErrorOccurred);

    // 连接到PLC
    LOG_INFO("正在连接PLC: ip=%s, port=%d", plcIp.toStdString().c_str(), plcPort);
    ui->statusbar->showMessage("正在连接PLC...");

    modbusClient->setConnectionParameter(QModbusClient::NetworkAddressParameter, plcIp);
    modbusClient->setConnectionParameter(QModbusClient::NetworkPortParameter, plcPort);

    // 设置超时时间（毫秒）- 需要在连接之前设置
    modbusClient->setTimeout(2000);  // 2秒超时
    modbusClient->setNumberOfRetries(3);  // 重试3次

    if (!modbusClient->connectDevice()) {
        LOG_Error("PLC连接失败: ip=%s, port=%d, error=%s",
                  plcIp.toStdString().c_str(), plcPort,
                  modbusClient->errorString().toStdString().c_str());
        ui->statusbar->showMessage("PLC连接失败: " + modbusClient->errorString());
        QMessageBox::warning(this, "连接失败",
                           QString("无法连接到PLC: IP=%1, 端口=%2\n错误: %3")
                           .arg(plcIp).arg(plcPort).arg(modbusClient->errorString()));
        return;
    }

    LOG_INFO("PLC连接请求已发送: ip=%s, port=%d", plcIp.toStdString().c_str(), plcPort);
}

/**
 * @brief Modbus设备状态变化处理
 */
void MainWindow::onModbusStateChanged(QModbusDevice::State state)
{
    if (!modbusClient) return;

    if (state == QModbusDevice::ConnectedState) {
        // 连接成功
        LOG_INFO("PLC连接成功");
        isPlcConnected = true;

        // 更新状态标签 - 绿灯
        if (ui->plcStatusLabel) {
            ui->plcStatusLabel->setText("PLC状态: 🟢 已连接");
        }

        // 更新计数显示
        if (ui->plcConfigLabel) {
            ui->plcConfigLabel->setText(QString("PLC发送计数: %1, 接收计数: %2")
                                       .arg(plcSendCount).arg(plcReadCount));
        }

        // 显示成功提示
        QString plcIp = ui->plcIpLineEdit->text();
        QString plcPort = ui->plcPortLineEdit->text();
        ui->statusbar->showMessage("PLC连接成功");
        QMessageBox::information(this, "连接成功",
                               QString("已成功连接到PLC: IP=%1, 端口=%2")
                               .arg(plcIp).arg(plcPort));

        // 读取所有非空寄存器
        readAllValidRegisters();

        // 启动定时器定期读取寄存器
        if (!plcReadTimer) {
            plcReadTimer = new QTimer(this);
            connect(plcReadTimer, &QTimer::timeout, this, &MainWindow::readAllValidRegisters);
        }
        plcReadTimer->start(500);  // 每500ms读取一次

    } else if (state == QModbusDevice::UnconnectedState) {
        // 断开连接
        LOG_INFO("PLC已断开连接");
        isPlcConnected = false;

        // 停止读取定时器
        if (plcReadTimer && plcReadTimer->isActive()) {
            plcReadTimer->stop();
        }

        // 更新状态标签 - 未连接
        if (ui->plcStatusLabel) {
            ui->plcStatusLabel->setText("PLC状态: ⚪ 未连接");
        }
    }
}

/**
 * @brief Modbus错误处理
 */
void MainWindow::onModbusErrorOccurred(QModbusDevice::Error error)
{
    if (modbusClient) {
        LOG_Error("PLC错误: %s", modbusClient->errorString().toStdString().c_str());
        ui->statusbar->showMessage("PLC错误: " + modbusClient->errorString());
    }
}

/**
 * @brief 读取所有有效的寄存器
 */
void MainWindow::readAllValidRegisters()
{
    if (!modbusClient || modbusClient->state() != QModbusDevice::ConnectedState) {
        return;
    }

    // 读取保持寄存器 (功能码 0x03)
    for (int i = 1; i <= 5; ++i) {
        QString addrStr;
        switch (i) {
            case 1: addrStr = ui->plcAddr1LineEdit->text(); break;
            case 2: addrStr = ui->plcAddr2LineEdit->text(); break;
            case 3: addrStr = ui->plcAddr3LineEdit->text(); break;
            case 4: addrStr = ui->plcAddr4LineEdit->text(); break;
            case 5: addrStr = ui->plcAddr5LineEdit->text(); break;
        }

        // 跳过空地址
        if (addrStr.isEmpty()) {
            continue;
        }

        // 解析地址（支持十进制和0x开头的十六进制）
        bool ok;
        int address = addrStr.startsWith("0x") || addrStr.startsWith("0X") ?
                      addrStr.toInt(&ok, 16) : addrStr.toInt(&ok, 10);

        if (!ok || address < 0) {
            LOG_WARN("无效的寄存器地址: %s", addrStr.toStdString().c_str());
            continue;
        }

        // 发送读请求
        QModbusDataUnit readUnit(QModbusDataUnit::HoldingRegisters);
        readUnit.setStartAddress(address);
        readUnit.setValueCount(1);

        LOG_INFO("正在读取寄存器: index=%d, address=%d", i, address);

        if (auto *reply = modbusClient->sendReadRequest(readUnit, 1)) {
            if (!reply->isFinished()) {
                connect(reply, &QModbusReply::finished, this, [this, reply, i, address]() {
                    if (reply->error() == QModbusDevice::NoError) {
                        const QModbusDataUnit unit = reply->result();
                        if (unit.valueCount() > 0) {
                            quint16 value = unit.value(0);
                            LOG_INFO("读取寄存器成功: index=%d, address=%d, value=%d", i, address, value);
                            this->updatePlcReadValue(i, value);
                            this->plcReadCount++;
                        }
                    } else {
                        LOG_Error("读取寄存器%d (地址=%d) 失败: %s", i, address, reply->errorString().toStdString().c_str());
                    }
                    reply->deleteLater();
                });
            } else {
                LOG_WARN("读请求立即完成: index=%d, address=%d", i, address);
                reply->deleteLater();
            }
        } else {
            LOG_Error("发送读请求失败: index=%d, address=%d", i, address);
        }
    }

    // 更新计数显示
    if (ui->plcConfigLabel) {
        ui->plcConfigLabel->setText(QString("PLC发送计数: %1, 接收计数: %2")
                                   .arg(plcSendCount).arg(plcReadCount));
    }
}

/**
 * @brief 更新PLC读取值显示
 */
void MainWindow::updatePlcReadValue(int index, quint16 value)
{
    QLabel *label = nullptr;
    switch (index) {
        case 1: label = ui->plcReadValue1Label; break;
        case 2: label = ui->plcReadValue2Label; break;
        case 3: label = ui->plcReadValue3Label; break;
        case 4: label = ui->plcReadValue4Label; break;
        case 5: label = ui->plcReadValue5Label; break;
    }

    if (label) {
        label->setText(QString::number(value));
    }
}

/**
 * @brief 更新PLC读取值显示（浮点数）
 */
void MainWindow::updatePlcReadValueFloat(int index, float value)
{
    QLabel *label = nullptr;
    switch (index) {
        case 1: label = ui->plcReadValue1Label; break;
        case 2: label = ui->plcReadValue2Label; break;
        case 3: label = ui->plcReadValue3Label; break;
        case 4: label = ui->plcReadValue4Label; break;
        case 5: label = ui->plcReadValue5Label; break;
    }

    if (label) {
        label->setText(QString::number(value, 'f', 2));
    }
}

/**
 * @brief 处理PLC断开连接按钮点击事件
 */
void MainWindow::on_plcDisconnectButton_clicked()
{
    LOG_INFO("PLC断开按钮被点击");

    if (modbusClient && modbusClient->state() == QModbusDevice::ConnectedState) {
        modbusClient->disconnectDevice();
    }

    // 停止读取定时器
    if (plcReadTimer && plcReadTimer->isActive()) {
        plcReadTimer->stop();
    }

    isPlcConnected = false;

    // 更新状态标签
    if (ui->plcStatusLabel) {
        ui->plcStatusLabel->setText("PLC状态: ⚪ 未连接");
    }

    ui->statusbar->showMessage("PLC已断开连接");
    LOG_INFO("PLC已断开连接");
}

/**
 * @brief 处理PLC寄存器保存按钮点击事件
 * @param index 寄存器编号(1-5)
 */
void MainWindow::on_plcSaveButton_clicked(int index)
{
    LOG_INFO("PLC保存按钮被点击: index=%d", index);

    // 获取对应寄存器的地址和值
    QString addrStr;
    QString valueStr;

    switch (index) {
        case 1:
            addrStr = ui->plcAddr1LineEdit->text();
            valueStr = ui->plcCmdValue1LineEdit->text();
            break;
        case 2:
            addrStr = ui->plcAddr2LineEdit->text();
            valueStr = ui->plcCmdValue2LineEdit->text();
            break;
        case 3:
            addrStr = ui->plcAddr3LineEdit->text();
            valueStr = ui->plcCmdValue3LineEdit->text();
            break;
        case 4:
            addrStr = ui->plcAddr4LineEdit->text();
            valueStr = ui->plcCmdValue4LineEdit->text();
            break;
        case 5:
            addrStr = ui->plcAddr5LineEdit->text();
            valueStr = ui->plcCmdValue5LineEdit->text();
            break;
    }

    // 验证参数
    if (addrStr.isEmpty() || valueStr.isEmpty()) {
        ui->statusbar->showMessage("请填写寄存器地址和指令值");
        LOG_WARN("寄存器参数无效: index=%d, addr=%s, value=%s",
                 index, addrStr.toStdString().c_str(), valueStr.toStdString().c_str());
        return;
    }

    // 解析地址
    bool ok;
    int address = addrStr.startsWith("0x") || addrStr.startsWith("0X") ?
                  addrStr.toInt(&ok, 16) : addrStr.toInt(&ok, 10);

    if (!ok || address < 0) {
        ui->statusbar->showMessage("无效的寄存器地址");
        LOG_WARN("无效的寄存器地址: %s", addrStr.toStdString().c_str());
        return;
    }

    // 解析值
    bool valueOk;
    quint16 value = valueStr.toUInt(&valueOk);
    if (!valueOk) {
        ui->statusbar->showMessage("无效的指令值");
        LOG_WARN("无效的指令值: %s", valueStr.toStdString().c_str());
        return;
    }

    LOG_INFO("保存寄存器: index=%d, addr=%d, value=%d", index, address, value);

    // 保存到本地配置（这里可以扩展为保存到配置文件）
    ui->statusbar->showMessage(QString("寄存器%1已保存: 地址=%2, 值=%3").arg(index).arg(address).arg(value));
}

/**
 * @brief 处理PLC寄存器发送按钮点击事件
 * @param index 寄存器编号(1-5)
 */
void MainWindow::on_plcSendButton_clicked(int index)
{
    sendPlcCommandByIndex(index, true, QStringLiteral("manual-button"));
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

    if (hasValidGnssData) {
        const GNSS gnssData = lastValidGnssData;
        if (ui->oPointLineEdit) {
            ui->oPointLineEdit->setText(QString("%1,%2")
                                            .arg(gnssData.blh.x(), 0, 'f', 6)
                                            .arg(gnssData.blh.y(), 0, 'f', 6));
        }

        LOG_INFO("已将当前位置设置为原点: (%.6f, %.6f)", gnssData.blh.x(), gnssData.blh.y());
        QMessageBox::information(this,
                                 "设置成功",
                                 QString("已将当前位置 (%1, %2) 设置为原点")
                                     .arg(gnssData.blh.x(), 0, 'f', 6)
                                     .arg(gnssData.blh.y(), 0, 'f', 6));
    } else {
        LOG_WARN("没有有效的GPS数据，无法设置原点");
        QMessageBox::warning(this, "设置失败", "没有有效的GPS数据，无法设置原点");
    }
}

/**
 * @brief 处理设置为A点按钮点击事件
 */
/**
 * @brief 保存配置到JSON文件
 */
void MainWindow::on_saveConfigFileButton_clicked()
{
    LOG_INFO("保存配置文件按钮被点击");

    QString fileName = QFileDialog::getSaveFileName(this, "保存配置文件", "", "JSON文件 (*.json)");
    if (fileName.isEmpty()) {
        return;
    }

    QVector<CellConfigRow> rows;
    SeederSystemParams params;
    QString parseError;
    parseCellConfigTable(rows, &parseError);
    parseSeederParams(params, nullptr);

    QJsonObject config;

    QJsonObject deviceSettings;
    deviceSettings["imuPort"] = ui->imuPortComboBox->currentData().toString();
    deviceSettings["imuBaud"] = ui->imuBaudComboBox->currentText();
    deviceSettings["gpsPort"] = ui->gpsPortComboBox->currentData().toString();
    deviceSettings["gpsBaud"] = ui->gpsBaudComboBox->currentText();
    deviceSettings["plcIp"] = ui->plcIpLineEdit->text();
    deviceSettings["plcPort"] = ui->plcPortLineEdit->text();
    config["deviceSettings"] = deviceSettings;

    QJsonArray plcRegisters;
    for (int i = 1; i <= 5; ++i) {
        QJsonObject registerObj;
        QLineEdit *addrLineEdit = findChild<QLineEdit*>(QString("plcAddr%1LineEdit").arg(i));
        QLineEdit *valueLineEdit = findChild<QLineEdit*>(QString("plcCmdValue%1LineEdit").arg(i));
        QCheckBox *forwardTrigger = findChild<QCheckBox*>(QString("forwardTriggerLine%1").arg(i));
        QCheckBox *reverseTrigger = findChild<QCheckBox*>(QString("reverseTriggerLine%1").arg(i));

        if (addrLineEdit && valueLineEdit) {
            registerObj["address"] = addrLineEdit->text();
            registerObj["value"] = valueLineEdit->text();
            registerObj["forwardTrigger"] = forwardTrigger ? forwardTrigger->isChecked() : false;
            registerObj["reverseTrigger"] = reverseTrigger ? reverseTrigger->isChecked() : false;
            plcRegisters.append(registerObj);
        }
    }
    config["plcRegisters"] = plcRegisters;

    QJsonObject pathSettings;
    pathSettings["oPoint"] = ui->oPointLineEdit ? ui->oPointLineEdit->text().trimmed() : QString();
    pathSettings["aPoint"] = ui->aPointLineEdit ? ui->aPointLineEdit->text().trimmed() : QString();
    config["pathSettings"] = pathSettings;

    QJsonObject seederSettings;
    seederSettings["rowSpacingM"] = ui->rowSpacingLineEdit ? ui->rowSpacingLineEdit->text().trimmed() : QString();
    seederSettings["seedHoleDistanceCm"] = ui->seedHoleDistanceLineEdit ? ui->seedHoleDistanceLineEdit->text().trimmed() : QString();
    seederSettings["seedSpeedRatio"] = ui->seedSpeedRatioLineEdit ? ui->seedSpeedRatioLineEdit->text().trimmed() : QString();
    seederSettings["seedRadiusCm"] = ui->seedRadiusLineEdit ? ui->seedRadiusLineEdit->text().trimmed() : QString();
    seederSettings["seedAdsorbCount"] = ui->seedAdsorbCountLineEdit ? ui->seedAdsorbCountLineEdit->text().trimmed() : QString();
    seederSettings["seedDropHeightM"] = ui->seedDropHeightLineEdit ? ui->seedDropHeightLineEdit->text().trimmed() : QString();
    seederSettings["antennaArm"] = ui->antennaArmLineEdit ? ui->antennaArmLineEdit->text().trimmed() : QString();
    seederSettings["distancePulseRatio"] = ui->distancePulseRatioLineEdit ? ui->distancePulseRatioLineEdit->text().trimmed() : QString();
    seederSettings["signalTransmissionDelayMs"] = ui->signalTransmissionDelayLineEdit ? ui->signalTransmissionDelayLineEdit->text().trimmed() : QString();
    config["seederSettings"] = seederSettings;

    QJsonArray cellConfigArray;
    for (const CellConfigRow& row : rows) {
        QJsonObject rowObj;
        rowObj["blockNo"] = row.blockNo;
        rowObj["blockName"] = row.blockName;
        rowObj["blockWidthM"] = row.blockWidthM;
        rowObj["clearanceM"] = row.clearanceM;
        rowObj["spacingCm"] = row.spacingCm;
        rowObj["ridges"] = row.ridges;
        cellConfigArray.append(rowObj);
    }
    config["cellConfigRows"] = cellConfigArray;
    config["reverseOrder"] = ui->reverseOrderCheckBox && ui->reverseOrderCheckBox->isChecked();

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        LOG_Error("无法打开文件: %s", fileName.toStdString().c_str());
        QMessageBox::warning(this, "保存失败", "无法打开文件进行写入");
        return;
    }

    QJsonDocument doc(config);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    LOG_INFO("配置文件保存成功: %s", fileName.toStdString().c_str());
    ui->statusbar->showMessage("配置文件保存成功");
    QMessageBox::information(this, "保存成功", "配置文件已成功保存");
}

/**
 * @brief 从JSON文件加载配置
 */
void MainWindow::on_loadConfigFileButton_clicked()
{
    LOG_INFO("加载配置文件按钮被点击");

    QString fileName = QFileDialog::getOpenFileName(this, "加载配置文件", "", "JSON文件 (*.json)");
    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        LOG_Error("无法打开文件: %s", fileName.toStdString().c_str());
        QMessageBox::warning(this, "加载失败", "无法打开文件进行读取");
        return;
    }

    const QByteArray jsonData = file.readAll();
    file.close();

    const QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (!doc.isObject()) {
        LOG_Error("无效的JSON文件: %s", fileName.toStdString().c_str());
        QMessageBox::warning(this, "加载失败", "无效的JSON文件格式");
        return;
    }

    const QJsonObject config = doc.object();

    if (config.contains("deviceSettings")) {
        const QJsonObject deviceSettings = config["deviceSettings"].toObject();

        if (deviceSettings.contains("imuPort")) {
            const QString imuPort = deviceSettings["imuPort"].toString();
            for (int i = 0; i < ui->imuPortComboBox->count(); ++i) {
                if (ui->imuPortComboBox->itemData(i).toString() == imuPort) {
                    ui->imuPortComboBox->setCurrentIndex(i);
                    break;
                }
            }
        }
        if (deviceSettings.contains("imuBaud")) {
            const int index = ui->imuBaudComboBox->findText(deviceSettings["imuBaud"].toString());
            if (index != -1) {
                ui->imuBaudComboBox->setCurrentIndex(index);
            }
        }
        if (deviceSettings.contains("gpsPort")) {
            const QString gpsPort = deviceSettings["gpsPort"].toString();
            for (int i = 0; i < ui->gpsPortComboBox->count(); ++i) {
                if (ui->gpsPortComboBox->itemData(i).toString() == gpsPort) {
                    ui->gpsPortComboBox->setCurrentIndex(i);
                    break;
                }
            }
        }
        if (deviceSettings.contains("gpsBaud")) {
            const int index = ui->gpsBaudComboBox->findText(deviceSettings["gpsBaud"].toString());
            if (index != -1) {
                ui->gpsBaudComboBox->setCurrentIndex(index);
            }
        }
        if (deviceSettings.contains("plcIp")) {
            ui->plcIpLineEdit->setText(deviceSettings["plcIp"].toString());
        }
        if (deviceSettings.contains("plcPort")) {
            ui->plcPortLineEdit->setText(deviceSettings["plcPort"].toString());
        }
    }

    if (config.contains("plcRegisters")) {
        const QJsonArray plcRegisters = config["plcRegisters"].toArray();
        for (int i = 0; i < plcRegisters.size() && i < 5; ++i) {
            const QJsonObject registerObj = plcRegisters[i].toObject();
            const int index = i + 1;
            QLineEdit *addrLineEdit = findChild<QLineEdit*>(QString("plcAddr%1LineEdit").arg(index));
            QLineEdit *valueLineEdit = findChild<QLineEdit*>(QString("plcCmdValue%1LineEdit").arg(index));
            QCheckBox *forwardTrigger = findChild<QCheckBox*>(QString("forwardTriggerLine%1").arg(index));
            QCheckBox *reverseTrigger = findChild<QCheckBox*>(QString("reverseTriggerLine%1").arg(index));

            if (addrLineEdit && registerObj.contains("address")) {
                addrLineEdit->setText(registerObj["address"].toString());
            }
            if (valueLineEdit && registerObj.contains("value")) {
                valueLineEdit->setText(registerObj["value"].toString());
            }
            if (forwardTrigger && registerObj.contains("forwardTrigger")) {
                forwardTrigger->setChecked(registerObj["forwardTrigger"].toBool());
            }
            if (reverseTrigger && registerObj.contains("reverseTrigger")) {
                reverseTrigger->setChecked(registerObj["reverseTrigger"].toBool());
            }
        }
    }

    if (config.contains("pathSettings")) {
        const QJsonObject pathSettings = config["pathSettings"].toObject();
        if (pathSettings.contains("oPoint") && ui->oPointLineEdit) {
            ui->oPointLineEdit->setText(pathSettings["oPoint"].toString());
        }
        if (pathSettings.contains("aPoint") && ui->aPointLineEdit) {
            ui->aPointLineEdit->setText(pathSettings["aPoint"].toString());
        }
    }

    if (config.contains("seederSettings")) {
        const QJsonObject seederSettings = config["seederSettings"].toObject();
        if (ui->rowSpacingLineEdit && seederSettings.contains("rowSpacingM")) {
            ui->rowSpacingLineEdit->setText(seederSettings["rowSpacingM"].toString());
        }
        if (ui->seedHoleDistanceLineEdit && seederSettings.contains("seedHoleDistanceCm")) {
            ui->seedHoleDistanceLineEdit->setText(seederSettings["seedHoleDistanceCm"].toString());
        }
        if (ui->seedSpeedRatioLineEdit && seederSettings.contains("seedSpeedRatio")) {
            ui->seedSpeedRatioLineEdit->setText(seederSettings["seedSpeedRatio"].toString());
        }
        if (ui->seedRadiusLineEdit && seederSettings.contains("seedRadiusCm")) {
            ui->seedRadiusLineEdit->setText(seederSettings["seedRadiusCm"].toString());
        }
        if (ui->seedAdsorbCountLineEdit && seederSettings.contains("seedAdsorbCount")) {
            ui->seedAdsorbCountLineEdit->setText(seederSettings["seedAdsorbCount"].toString());
        }
        if (ui->seedDropHeightLineEdit && seederSettings.contains("seedDropHeightM")) {
            ui->seedDropHeightLineEdit->setText(seederSettings["seedDropHeightM"].toString());
        }
        if (ui->antennaArmLineEdit && seederSettings.contains("antennaArm")) {
            ui->antennaArmLineEdit->setText(seederSettings["antennaArm"].toString());
        }
        if (ui->distancePulseRatioLineEdit && seederSettings.contains("distancePulseRatio")) {
            ui->distancePulseRatioLineEdit->setText(seederSettings["distancePulseRatio"].toString());
        }
        if (ui->signalTransmissionDelayLineEdit && seederSettings.contains("signalTransmissionDelayMs")) {
            ui->signalTransmissionDelayLineEdit->setText(seederSettings["signalTransmissionDelayMs"].toString());
        }
    }

    if (config.contains("cellConfigRows") && ui->cellConfigTableWidget) {
        const QJsonArray cellConfigArray = config["cellConfigRows"].toArray();
        ui->cellConfigTableWidget->setRowCount(0);
        for (int i = 0; i < cellConfigArray.size(); ++i) {
            const QJsonObject rowObj = cellConfigArray[i].toObject();
            ui->cellConfigTableWidget->insertRow(i);
            ui->cellConfigTableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
            ui->cellConfigTableWidget->setItem(i, 1, new QTableWidgetItem(QString::number(rowObj["blockNo"].toInt())));
            ui->cellConfigTableWidget->setItem(i, 2, new QTableWidgetItem(rowObj["blockName"].toString()));
            ui->cellConfigTableWidget->setItem(i, 3, new QTableWidgetItem(QString::number(rowObj["blockWidthM"].toDouble())));
            ui->cellConfigTableWidget->setItem(i, 4, new QTableWidgetItem(QString::number(rowObj["clearanceM"].toDouble())));
            ui->cellConfigTableWidget->setItem(i, 5, new QTableWidgetItem(QString::number(rowObj["spacingCm"].toDouble())));
            ui->cellConfigTableWidget->setItem(i, 6, new QTableWidgetItem(QString::number(rowObj["ridges"].toInt())));
        }
        updateTableIndexColumn(ui->cellConfigTableWidget);
    }

    if (config.contains("reverseOrder") && ui->reverseOrderCheckBox) {
        ui->reverseOrderCheckBox->setChecked(config["reverseOrder"].toBool());
    }

    QString parseError;
    if (!parseCellConfigTable(cellConfigRows_, &parseError)) {
        generatedSeedingPlanRows_.clear();
        lockedSeedingPlanRows_.clear();
        updatePlanObservationLabels(QVector<SeedingPlanRow>());
        triggerPlanResult_ = BlockPlanResult();
        highlightedPlanRow_ = -1;
        nextPlanRow_ = -1;
        lastProcessedTriggerSequence_ = 0;
        if (triggerCommitTimer) {
            triggerCommitTimer->stop();
        }
        if (stateEstimator) {
            stateEstimator->setTriggerPlan(BlockPlanResult());
        }
        if (ui->mapWidget) {
            if (MapWidget* mapWidget = dynamic_cast<MapWidget*>(ui->mapWidget)) {
                mapWidget->setBlockPlanResult(BlockPlanResult());
            }
        }
        if (ui->seedingPlanTableWidget) {
            ui->seedingPlanTableWidget->setRowCount(0);
        }
        setSeedingPlanEditingLocked(false);
        QMessageBox::warning(this, tr("错误"), parseError);
        ui->databaseStatusLabel->setText(QStringLiteral("播种计划：解析失败"));
        updateCellAreaLabel();
        return;
    }

    generatedSeedingPlanRows_.clear();
    lockedSeedingPlanRows_.clear();
    updatePlanObservationLabels(QVector<SeedingPlanRow>());
    setSeedingPlanEditingLocked(false);
    triggerPlanResult_ = BlockPlanResult();
    highlightedPlanRow_ = -1;
    nextPlanRow_ = -1;
    lastProcessedTriggerSequence_ = 0;
    if (triggerCommitTimer) {
        triggerCommitTimer->stop();
    }
    if (stateEstimator) {
        stateEstimator->setTriggerPlan(BlockPlanResult());
    }
    if (ui->mapWidget) {
        if (MapWidget* mapWidget = dynamic_cast<MapWidget*>(ui->mapWidget)) {
            if (hasLocalFrameConfigured && coordinate && coordinate->isInitialized()) {
                mapWidget->setCoordinate(coordinate);
            }
            mapWidget->setBlockPlanResult(BlockPlanResult());
        }
    }
    if (ui->seedingPlanTableWidget) {
        ui->seedingPlanTableWidget->setRowCount(0);
    }

    updateCellAreaLabel();
    if (ui->databaseStatusLabel) {
        ui->databaseStatusLabel->setText(QStringLiteral("播种计划：配置已加载，请重新生成"));
    }

    LOG_INFO("配置文件加载成功: %s", fileName.toStdString().c_str());
    ui->statusbar->showMessage("配置文件加载成功");
    QMessageBox::information(this, "加载成功", "配置文件已成功加载");
}

/**
 * @brief 处理倒序复选框点击事件
 */
void MainWindow::on_reverseOrderCheckBox_toggled(bool checked)
{
    LOG_INFO("倒序复选框被点击: %s", checked ? "true" : "false");

    if (!ui->cellConfigTableWidget) {
        return;
    }

    const int rowCount = ui->cellConfigTableWidget->rowCount();
    if (rowCount <= 0) {
        cellConfigRows_.clear();
        generatedSeedingPlanRows_.clear();
        lockedSeedingPlanRows_.clear();
        updatePlanObservationLabels(QVector<SeedingPlanRow>());
        triggerPlanResult_ = BlockPlanResult();
        highlightedPlanRow_ = -1;
        nextPlanRow_ = -1;
        lastProcessedTriggerSequence_ = 0;
        if (triggerCommitTimer) {
            triggerCommitTimer->stop();
        }
        if (stateEstimator) {
            stateEstimator->setTriggerPlan(BlockPlanResult());
        }
        if (ui->mapWidget) {
            if (MapWidget* mapWidget = dynamic_cast<MapWidget*>(ui->mapWidget)) {
                if (hasLocalFrameConfigured && coordinate && coordinate->isInitialized()) {
                    mapWidget->setCoordinate(coordinate);
                }
                mapWidget->setBlockPlanResult(BlockPlanResult());
            }
        }
        if (ui->seedingPlanTableWidget) {
            ui->seedingPlanTableWidget->setRowCount(0);
        }
        setSeedingPlanEditingLocked(false);
        updateCellAreaLabel();
        return;
    }

    QList<QList<QString>> rows;
    rows.reserve(rowCount);
    for (int i = 0; i < rowCount; ++i) {
        QList<QString> row;
        for (int j = 1; j < ui->cellConfigTableWidget->columnCount(); ++j) {
            QTableWidgetItem* item = ui->cellConfigTableWidget->item(i, j);
            row.append(item ? item->text() : QString());
        }
        rows.append(row);
    }

    std::sort(rows.begin(), rows.end(), [checked](const QList<QString>& a, const QList<QString>& b) {
        if (a.isEmpty() || b.isEmpty()) {
            return false;
        }
        bool ok1 = false;
        bool ok2 = false;
        const int num1 = a[0].toInt(&ok1);
        const int num2 = b[0].toInt(&ok2);
        if (!ok1 || !ok2) {
            return false;
        }
        return checked ? (num1 > num2) : (num1 < num2);
    });

    ui->cellConfigTableWidget->setRowCount(0);
    for (int i = 0; i < rows.size(); ++i) {
        ui->cellConfigTableWidget->insertRow(i);
        ui->cellConfigTableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));

        const QList<QString>& row = rows[i];
        for (int j = 0; j < row.size(); ++j) {
            ui->cellConfigTableWidget->setItem(i, j + 1, new QTableWidgetItem(row[j]));
        }
    }

    QString parseError;
    if (!parseCellConfigTable(cellConfigRows_, &parseError)) {
        cellConfigRows_.clear();
        generatedSeedingPlanRows_.clear();
        lockedSeedingPlanRows_.clear();
        updatePlanObservationLabels(QVector<SeedingPlanRow>());
        triggerPlanResult_ = BlockPlanResult();
        highlightedPlanRow_ = -1;
        nextPlanRow_ = -1;
        lastProcessedTriggerSequence_ = 0;
        if (triggerCommitTimer) {
            triggerCommitTimer->stop();
        }
        if (stateEstimator) {
            stateEstimator->setTriggerPlan(BlockPlanResult());
        }
        if (ui->mapWidget) {
            if (MapWidget* mapWidget = dynamic_cast<MapWidget*>(ui->mapWidget)) {
                if (hasLocalFrameConfigured && coordinate && coordinate->isInitialized()) {
                    mapWidget->setCoordinate(coordinate);
                }
                mapWidget->setBlockPlanResult(BlockPlanResult());
            }
        }
        if (ui->seedingPlanTableWidget) {
            ui->seedingPlanTableWidget->setRowCount(0);
        }
        setSeedingPlanEditingLocked(false);
        updateCellAreaLabel();
        if (ui->databaseStatusLabel) {
            ui->databaseStatusLabel->setText(QStringLiteral("播种计划：顺序更新后数据无效"));
        }
        if (ui->statusbar) {
            ui->statusbar->showMessage(QStringLiteral("小区顺序更新后存在无效数据，请检查配置表"), 3000);
        }
        return;
    }

    generatedSeedingPlanRows_.clear();
    lockedSeedingPlanRows_.clear();
    updatePlanObservationLabels(QVector<SeedingPlanRow>());
    triggerPlanResult_ = BlockPlanResult();
    highlightedPlanRow_ = -1;
    nextPlanRow_ = -1;
    lastProcessedTriggerSequence_ = 0;
    if (triggerCommitTimer) {
        triggerCommitTimer->stop();
    }
    if (stateEstimator) {
        stateEstimator->setTriggerPlan(BlockPlanResult());
    }
    if (ui->mapWidget) {
        if (MapWidget* mapWidget = dynamic_cast<MapWidget*>(ui->mapWidget)) {
            if (hasLocalFrameConfigured && coordinate && coordinate->isInitialized()) {
                mapWidget->setCoordinate(coordinate);
            }
            mapWidget->setBlockPlanResult(BlockPlanResult());
        }
    }
    if (ui->seedingPlanTableWidget) {
        ui->seedingPlanTableWidget->setRowCount(0);
    }

    setSeedingPlanEditingLocked(false);
    updateCellAreaLabel();

    if (ui->databaseStatusLabel) {
        ui->databaseStatusLabel->setText(QStringLiteral("播种计划：顺序已更新，请重新生成"));
    }
    if (ui->statusbar) {
        ui->statusbar->showMessage(QStringLiteral("小区顺序已更新，请重新生成播种计划"), 3000);
    }
}



void MainWindow::on_saveConfigButton_clicked()
{
    LOG_INFO("设置为A点按钮被点击");

    if (hasValidGnssData) {
        const GNSS gnssData = lastValidGnssData;
        if (ui->aPointLineEdit) {
            ui->aPointLineEdit->setText(QString("%1,%2")
                                            .arg(gnssData.blh.x(), 0, 'f', 6)
                                            .arg(gnssData.blh.y(), 0, 'f', 6));
        }

        LOG_INFO("已将当前位置设置为A点: (%.6f, %.6f)", gnssData.blh.x(), gnssData.blh.y());
        QMessageBox::information(this,
                                 "设置成功",
                                 QString("已将当前位置 (%1, %2) 设置为A点")
                                     .arg(gnssData.blh.x(), 0, 'f', 6)
                                     .arg(gnssData.blh.y(), 0, 'f', 6));
    } else {
        LOG_WARN("没有有效的GPS数据，无法设置A点");
        QMessageBox::warning(this, "设置失败", "没有有效的GPS数据，无法设置A点");
    }
}

/**
 * @brief 更新表格的序号列，确保始终递增
 * @param tableWidget 要更新的表格组件
 */
void MainWindow::updateTableIndexColumn(QTableWidget* tableWidget)
{
    if (!tableWidget) {
        return;
    }

    int rowCount = tableWidget->rowCount();
    for (int i = 0; i < rowCount; ++i) {
        QTableWidgetItem* indexItem = new QTableWidgetItem(QString::number(i + 1));
        tableWidget->setItem(i, 0, indexItem);
    }
}

/**
 * @brief 处理生成播种计划按钮点击事件
 */
void MainWindow::on_generateSeedingPlanButton_clicked()
{
    LOG_INFO("生成播种计划按钮被点击");

    QString errorMessage;
    QVector<CellConfigRow> rows;
    SeederSystemParams params;
    if (!parseCellConfigTable(rows, &errorMessage) || !parseSeederParams(params, &errorMessage)) {
        QMessageBox::warning(this, QStringLiteral("生成失败"), errorMessage);
        return;
    }

    cellConfigRows_ = rows;
    generatedPlanReferenceSpeedMps_ = safeReferenceSpeedMps();
    generatedSeedingPlanRows_ = buildSeedingPlanRows(cellConfigRows_, params, generatedPlanReferenceSpeedMps_, &errorMessage);
    if (generatedSeedingPlanRows_.isEmpty()) {
        updatePlanObservationLabels(QVector<SeedingPlanRow>());
        QMessageBox::warning(this, QStringLiteral("生成失败"), errorMessage.isEmpty() ? QStringLiteral("播种计划生成失败") : errorMessage);
        return;
    }

    for (SeedingPlanRow& row : generatedSeedingPlanRows_) {
        row.forwardTriggerCoord.clear();
        row.forwardTriggerTime.clear();
        row.reverseTriggerCoord.clear();
        row.reverseTriggerTime.clear();
        row.statusText = QStringLiteral("静态基准");
    }

    lockedSeedingPlanRows_.clear();
    setSeedingPlanEditingLocked(false);

    const BlockPlanResult previewPlan = buildBlockPlanResultFromPlanRows(generatedSeedingPlanRows_, params);
    triggerPlanResult_ = previewPlan;
    resetTriggerHistory(previewPlan);

    if (stateEstimator) {
        stateEstimator->setTriggerPlan(BlockPlanResult());
    }

    if (ui->mapWidget) {
        if (MapWidget* mapWidget = dynamic_cast<MapWidget*>(ui->mapWidget)) {
            if (hasLocalFrameConfigured && coordinate && coordinate->isInitialized()) {
                mapWidget->setCoordinate(coordinate);
                mapWidget->setBlockPlanResult(previewPlan);
            } else {
                mapWidget->setBlockPlanResult(BlockPlanResult());
            }
        }
    }

    populateSeedingPlanTable(generatedSeedingPlanRows_);
    updatePlanStatusDisplay(generatedSeedingPlanRows_, -1, -1);
    updatePlanObservationLabels(generatedSeedingPlanRows_);
    updateCellAreaLabel();

    QString tip = QStringLiteral("播种计划生成成功，共%1行").arg(generatedSeedingPlanRows_.size());
    if (currentReferenceSpeedMps() < kMinReferenceSpeedMps) {
        tip += QStringLiteral("，当前无有效速度，已使用%1 m/s作为静态基准").arg(generatedPlanReferenceSpeedMps_, 0, 'f', 2);
    }
    ui->statusbar->showMessage(tip);
    QMessageBox::information(this, QStringLiteral("生成成功"), tip);
}

/**
 * @brief 处理计算分割线按钮点击事件
 */
void MainWindow::on_calculateSplitLineButton_clicked()
{
    LOG_INFO("计算分割线按钮被点击");

    // 这里可以添加计算分割线的逻辑
    // 目前只是一个示例，实际功能需要根据业务逻辑实现

    ui->statusbar->showMessage("计算分割线功能待实现");
    QMessageBox::information(this, "提示", "计算分割线功能待实现");
}

/**
 * @brief 处理天线校准按钮点击事件
 */
void MainWindow::on_antennaCalibrationButton_clicked()
{
    LOG_INFO("天线校准按钮被点击");

    // 这里可以添加天线校准的逻辑
    // 目前只是一个示例，实际功能需要根据业务逻辑实现

    ui->statusbar->showMessage("天线校准功能待实现");
    QMessageBox::information(this, "提示", "天线校准功能待实现");
}

/**
 * @brief 处理锁定播种计划按钮点击事件
 */
void MainWindow::on_lockSeedingPlanButton_clicked()
{
    LOG_INFO("锁定播种计划按钮被点击");

    if (!hasLocalFrameConfigured || !coordinate || !coordinate->isInitialized()) {
        QMessageBox::warning(this, QStringLiteral("无法锁定"), QStringLiteral("请先生成本地坐标系"));
        return;
    }
    if (generatedSeedingPlanRows_.isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("无法锁定"), QStringLiteral("请先生成播种计划"));
        return;
    }

    SeederSystemParams params;
    QString errorMessage;
    if (!parseSeederParams(params, &errorMessage)) {
        QMessageBox::warning(this, QStringLiteral("无法锁定"), errorMessage);
        return;
    }

    lockedSeedingPlanRows_ = generatedSeedingPlanRows_;
    for (SeedingPlanRow& row : lockedSeedingPlanRows_) {
        row.statusText = QStringLiteral("已锁定");
    }

    triggerPlanResult_ = buildBlockPlanResultFromPlanRows(lockedSeedingPlanRows_, params);
    if (!triggerPlanResult_.isValid) {
        QMessageBox::warning(this, QStringLiteral("无法锁定"), QStringLiteral("运行态触发计划生成失败"));
        return;
    }

    resetTriggerHistory(triggerPlanResult_);
    setSeedingPlanEditingLocked(true);

    if (stateEstimator) {
        stateEstimator->reset();
        stateEstimator->setTriggerPlan(triggerPlanResult_);
    }

    if (ui->mapWidget) {
        if (MapWidget* mapWidget = dynamic_cast<MapWidget*>(ui->mapWidget)) {
            mapWidget->setCoordinate(coordinate);
            mapWidget->setBlockPlanResult(triggerPlanResult_);
            mapWidget->clearPath();
        }
    }

    if (triggerCommitTimer) {
        triggerCommitTimer->stop();
    }

    populateSeedingPlanTable(lockedSeedingPlanRows_);
    updatePlanStatusDisplay(lockedSeedingPlanRows_, -1, -1);
    updatePlanObservationLabels(lockedSeedingPlanRows_);
    refreshTriggerUi(EstimatedPose());

    ui->statusbar->showMessage(QStringLiteral("播种计划已锁定并投入运行"));
    QMessageBox::information(this,
                             QStringLiteral("锁定成功"),
                             QStringLiteral("播种计划已锁定，共启用%1条触发线")
                                 .arg(triggerPlanResult_.triggerLines.size()));
}

/**
 * @brief 处理连接数据库按钮点击事件
 */
void MainWindow::on_connectDatabaseButton_clicked()
{
    LOG_INFO("加载播种计划按钮被点击");

    const QString fileName = QFileDialog::getOpenFileName(this,
                                                          tr("选择播种计划CSV文件"),
                                                          "",
                                                          tr("CSV文件 (*.csv);;所有文件 (*.*)"));
    if (fileName.isEmpty()) {
        LOG_INFO("用户取消选择文件");
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        LOG_Error("无法打开文件: %s", fileName.toStdString().c_str());
        QMessageBox::warning(this, tr("错误"), tr("无法打开文件: %1").arg(fileName));
        ui->databaseStatusLabel->setText(QStringLiteral("播种计划：解析失败"));
        return;
    }

    ui->cellConfigTableWidget->setRowCount(0);

    QTextStream in(&file);
    const QString headerLine = in.readLine().trimmed();
    const QStringList headers = headerLine.split(',');
    const QStringList expectedHeaders = {
        QStringLiteral("方号 No"),
        QStringLiteral("小区名称 Block"),
        QStringLiteral("小区宽度(m) blockwidth"),
        QStringLiteral("过道宽度(m) clearance"),
        QStringLiteral("株距(cm) spacing"),
        QStringLiteral("垄数 ridges")
    };
    if (headers.size() != expectedHeaders.size()) {
        LOG_Error("CSV文件列数不正确");
        generatedSeedingPlanRows_.clear();
        lockedSeedingPlanRows_.clear();
        updatePlanObservationLabels(QVector<SeedingPlanRow>());
        triggerPlanResult_ = BlockPlanResult();
        highlightedPlanRow_ = -1;
        nextPlanRow_ = -1;
        lastProcessedTriggerSequence_ = 0;
        if (triggerCommitTimer) {
            triggerCommitTimer->stop();
        }
        if (stateEstimator) {
            stateEstimator->setTriggerPlan(BlockPlanResult());
        }
        if (ui->mapWidget) {
            if (MapWidget* mapWidget = dynamic_cast<MapWidget*>(ui->mapWidget)) {
                mapWidget->setBlockPlanResult(BlockPlanResult());
            }
        }
        if (ui->seedingPlanTableWidget) {
            ui->seedingPlanTableWidget->setRowCount(0);
        }
        setSeedingPlanEditingLocked(false);
        updateCellAreaLabel();
        QMessageBox::warning(this, tr("错误"), tr("CSV文件列数不正确，应为6列"));
        ui->databaseStatusLabel->setText(QStringLiteral("播种计划：解析失败"));
        return;
    }

    int row = 0;
    while (!in.atEnd()) {
        const QString line = in.readLine();
        if (line.trimmed().isEmpty()) {
            continue;
        }

        const QStringList fields = line.split(',');
        if (fields.size() != expectedHeaders.size()) {
            LOG_WARN("跳过列数不正确的CSV行: %d", row + 1);
            continue;
        }

        ui->cellConfigTableWidget->insertRow(row);
        ui->cellConfigTableWidget->setItem(row, 0, new QTableWidgetItem(QString::number(row + 1)));
        for (int col = 0; col < fields.size(); ++col) {
            ui->cellConfigTableWidget->setItem(row, col + 1, new QTableWidgetItem(fields[col].trimmed()));
        }
        ++row;
    }

    file.close();
    updateTableIndexColumn(ui->cellConfigTableWidget);

    QString parseError;
    if (!parseCellConfigTable(cellConfigRows_, &parseError)) {
        cellConfigRows_.clear();
        generatedSeedingPlanRows_.clear();
        lockedSeedingPlanRows_.clear();
        updatePlanObservationLabels(QVector<SeedingPlanRow>());
        triggerPlanResult_ = BlockPlanResult();
        highlightedPlanRow_ = -1;
        nextPlanRow_ = -1;
        lastProcessedTriggerSequence_ = 0;
        if (triggerCommitTimer) {
            triggerCommitTimer->stop();
        }
        if (stateEstimator) {
            stateEstimator->setTriggerPlan(BlockPlanResult());
        }
        if (ui->mapWidget) {
            if (MapWidget* mapWidget = dynamic_cast<MapWidget*>(ui->mapWidget)) {
                mapWidget->setBlockPlanResult(BlockPlanResult());
            }
        }
        if (ui->seedingPlanTableWidget) {
            ui->seedingPlanTableWidget->setRowCount(0);
        }
        setSeedingPlanEditingLocked(false);
        QMessageBox::warning(this, tr("错误"), parseError);
        ui->databaseStatusLabel->setText(QStringLiteral("播种计划：解析失败"));
        updateCellAreaLabel();
        return;
    }

    generatedSeedingPlanRows_.clear();
    lockedSeedingPlanRows_.clear();
    updatePlanObservationLabels(QVector<SeedingPlanRow>());
    setSeedingPlanEditingLocked(false);
    triggerPlanResult_ = BlockPlanResult();
    highlightedPlanRow_ = -1;
    nextPlanRow_ = -1;
    lastProcessedTriggerSequence_ = 0;
    if (stateEstimator) {
        stateEstimator->setTriggerPlan(BlockPlanResult());
    }
    if (ui->mapWidget) {
        if (MapWidget* mapWidget = dynamic_cast<MapWidget*>(ui->mapWidget)) {
            mapWidget->setBlockPlanResult(BlockPlanResult());
        }
    }
    if (triggerCommitTimer) {
        triggerCommitTimer->stop();
    }
    if (ui->seedingPlanTableWidget) {
        ui->seedingPlanTableWidget->setRowCount(0);
    }

    updateCellAreaLabel();
    ui->databaseStatusLabel->setText(QStringLiteral("播种计划：已加载"));
    ui->statusbar->showMessage(QStringLiteral("已载入%1行小区配置").arg(cellConfigRows_.size()));
    QMessageBox::information(this, tr("成功"), tr("成功加载 %1 行播种计划数据").arg(cellConfigRows_.size()));
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

    if (!coordinate) {
        return;
    }

    double oLat = 0.0;
    double oLon = 0.0;
    double aLat = 0.0;
    double aLon = 0.0;
    if (!parseCoordinateLineEdit(ui->oPointLineEdit, oLat, oLon, QStringLiteral("O点"), true)
        || !parseCoordinateLineEdit(ui->aPointLineEdit, aLat, aLon, QStringLiteral("A点"), true)) {
        return;
    }

    coordinate->setOriginAndDirection(oLat, oLon, aLat, aLon);
    LOG_INFO("已设置坐标系原点和方向: O(%.6f, %.6f), A(%.6f, %.6f)", oLat, oLon, aLat, aLon);

    hasLocalFrameConfigured = true;
    blockStartWorldPosition_ = QPointF(0.0, 0.0);

    if (stateEstimator) {
        stateEstimator->reset();
        stateEstimator->setTriggerPlan(BlockPlanResult());
    }

    if (triggerCommitTimer) {
        triggerCommitTimer->stop();
    }

    if (ui->mapWidget) {
        if (MapWidget* mapWidget = dynamic_cast<MapWidget*>(ui->mapWidget)) {
            mapWidget->setCoordinate(coordinate);
            mapWidget->clearPath();
            if (seedingPlanLocked_ && triggerPlanResult_.isValid) {
                mapWidget->setBlockPlanResult(triggerPlanResult_);
            } else {
                mapWidget->setBlockPlanResult(BlockPlanResult());
            }
        }
    }

    refreshTriggerUi(EstimatedPose());
    ui->statusbar->showMessage(QStringLiteral("本地坐标系已生成"));
    QMessageBox::information(this,
                             QStringLiteral("生成成功"),
                             QStringLiteral("本地坐标系已生成，当前步骤不会生成触发计划"));
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
    ui->imuConfigLabel->setText(QString("IMU有效数据: %1").arg(imuDataCount));
    ui->gpsConfigLabel->setText(QString("GPS有效数据: %1").arg(gpsDataCount));
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
        const EstimatedPose pose = stateEstimator->getPose();
        refreshDynamicTriggerPlan(pose);
        mapWidget->updateVehiclePose(pose);
        refreshTriggerUi(pose);
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
    refreshTriggerUi(pose);
}

void MainWindow::commitPendingTriggerFromTimer()
{
    if (!stateEstimator) {
        return;
    }

    if (stateEstimator->commitPredictedTrigger()) {
        const EstimatedPose pose = stateEstimator->getPose();
        handleCommittedTrigger(pose);
        if (MapWidget* mapWidget = dynamic_cast<MapWidget*>(ui->mapWidget)) {
            mapWidget->updateVehiclePose(pose);
        }
        refreshTriggerUi(pose);
    }
}

void MainWindow::refreshTriggerUi(const EstimatedPose& pose)
{
    // 只更新仍然存在的nextTriggerLabel
    if (!ui->nextTriggerLabel) {
        return;
    }

    if (pose.predictedTriggerTimeUs > 0 && pose.nextTriggerCountdownSec >= 0.0) {
        ui->nextTriggerLabel->setText(
            QString("距离下一次触发: %1 s")
                .arg(pose.nextTriggerCountdownSec, 0, 'f', 3));
    } else if (pose.hasNextTrigger && pose.isInFineMode) {
        ui->nextTriggerLabel->setText(
            QString("距离下一次触发: 已进入精估计 (T%1)")
                .arg(pose.nextTriggerIndex));
    } else if (pose.hasNextTrigger) {
        ui->nextTriggerLabel->setText(
            QString("距离下一次触发: 待进入精估计 (T%1)")
                .arg(pose.nextTriggerIndex));
    } else {
        ui->nextTriggerLabel->setText(QStringLiteral("距离下一次触发: 无"));
    }
}

void MainWindow::resetTriggerHistory(const BlockPlanResult& plan)
{
    triggerPlanResult_ = plan;
    lastProcessedTriggerSequence_ = 0;

    QVector<SeedingPlanRow>* rows = nullptr;
    if (seedingPlanLocked_ && !lockedSeedingPlanRows_.isEmpty()) {
        rows = &lockedSeedingPlanRows_;
    } else if (!generatedSeedingPlanRows_.isEmpty()) {
        rows = &generatedSeedingPlanRows_;
    }

    if (rows) {
        for (SeedingPlanRow& row : *rows) {
            row.forwardTriggerCoord.clear();
            row.forwardTriggerTime.clear();
            row.reverseTriggerCoord.clear();
            row.reverseTriggerTime.clear();
            if (row.statusText.isEmpty()) {
                row.statusText = seedingPlanLocked_ ? QStringLiteral("已锁定") : QStringLiteral("静态基准");
            }
        }
        populateSeedingPlanTable(*rows);
        updatePlanStatusDisplay(*rows, highlightedPlanRow_, nextPlanRow_);
    }
}

void MainWindow::handleCommittedTrigger(const EstimatedPose& pose)
{
    if (!pose.hasLastTriggerEvent || pose.triggerSequence <= lastProcessedTriggerSequence_) {
        return;
    }

    QVector<SeedingPlanRow>* rows = nullptr;
    if (seedingPlanLocked_ && !lockedSeedingPlanRows_.isEmpty()) {
        rows = &lockedSeedingPlanRows_;
    } else if (!generatedSeedingPlanRows_.isEmpty()) {
        rows = &generatedSeedingPlanRows_;
    }
    if (!rows) {
        lastProcessedTriggerSequence_ = pose.triggerSequence;
        triggerConfiguredPlcActions(pose);
        return;
    }

    int rowIndex = -1;
    bool isForward = pose.lastTriggerDirection > 0;
    for (int i = 0; i < rows->size(); ++i) {
        const SeedingPlanRow& row = rows->at(i);
        if ((isForward && row.forwardLineIndex == pose.lastTriggerLineIndex)
            || (!isForward && row.reverseLineIndex == pose.lastTriggerLineIndex)) {
            rowIndex = i;
            break;
        }
    }
    if (rowIndex < 0 || rowIndex >= rows->size()) {
        lastProcessedTriggerSequence_ = pose.triggerSequence;
        triggerConfiguredPlcActions(pose);
        return;
    }

    SeedingPlanRow& row = (*rows)[rowIndex];
    const QString coord = QString("(%1, %2)")
        .arg(pose.lastTriggerX, 0, 'f', 4)
        .arg(pose.lastTriggerY, 0, 'f', 4);
    const QString time = timestampToDateTime(static_cast<double>(pose.lastTriggerTimeUs) / 1e6);
    if (isForward) {
        row.forwardTriggerCoord = coord;
        row.forwardTriggerTime = time;
    } else {
        row.reverseTriggerCoord = coord;
        row.reverseTriggerTime = time;
    }

    lastProcessedTriggerSequence_ = pose.triggerSequence;
    populateSeedingPlanTable(*rows);
    updatePlanStatusDisplay(*rows, highlightedPlanRow_, nextPlanRow_);
    updatePlanObservationLabels(*rows, highlightedPlanRow_, nextPlanRow_);
    triggerConfiguredPlcActions(pose);
}
