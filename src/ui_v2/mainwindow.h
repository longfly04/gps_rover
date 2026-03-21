#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QObject>
#include <QListWidgetItem>

#include <QTimer>
#include <QTextEdit>
#include <QTextCursor>
#include <QSerialPortInfo>
#include <QPointF>
#include "map/statestimator.h"
#include "map/coordinate.h"
#include "map/blockgenerator.h"

#include "core/sensor/sensorinterface.h"






QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
class MapSubWindow;
class SettingsSubWindow;
class StorageSubWindow;
class SummarySubWindow;
}
QT_END_NAMESPACE

class QMdiSubWindow;

/**
 * @brief 主窗口类，负责整个应用程序的用户界面和控制逻辑
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父窗口指针
     */
    MainWindow(QWidget *parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~MainWindow();

signals:
    // 信号已简化，直接从传感器连接到处理函数

private slots:
    

    
    /**
     * @brief 更新本地时间显示
     */
    void updateLocalTime();
    
    /**
     * @brief 更新卫星时间显示
     */
    void updateGpsTime();
    
    /**
     * @brief 处理GPS时间同步完成事件
     * @param dt 时间同步的dt值
     */
    void onTimeSyncCompleted(double dt);
    
    /**
     * @brief 重置数据计数
     */
    void resetDataCounts();
    
    /**
     * @brief 处理IMU连接按钮点击事件
     */
    void on_imuConnectButton_clicked();
    
    /**
     * @brief 处理IMU断开连接按钮点击事件
     */
    void on_imuDisconnectButton_clicked();
    
    /**
     * @brief 处理GPS连接按钮点击事件
     */
    void on_gpsConnectButton_clicked();
    
    /**
     * @brief 处理GPS断开连接按钮点击事件
     */
    void on_gpsDisconnectButton_clicked();
    
    /**
     * @brief 处理PLC保存按钮点击事件
     */
    void on_plcConnectButton_clicked();
    
    /**
     * @brief 处理PLC测试按钮点击事件
     */
    void on_plcDisconnectButton_clicked();
    
    /**
     * @brief 处理IMU数据更新事件
     * @param imuData IMU数据
     */
    void onImuDataUpdated(const IMU& imuData);
    
    /**
     * @brief 处理GPS数据更新事件
     * @param gnssData GNSS数据
     */
    void onGnssDataUpdated(const GNSS& gnssData);
    
    /**
     * @brief 处理开始按钮点击事件
     */
    void on_startButton_clicked();
    
    /**
     * @brief 处理停止按钮点击事件
     */
    void on_stopButton_clicked();
    
    /**
     * @brief 处理配置按钮点击事件
     */
    void on_configButton_clicked();
    
    /**
     * @brief 处理加载配置按钮点击事件
     */
    void on_loadConfigButton_clicked();
    
    /**
     * @brief 处理保存配置按钮点击事件
     */
    void on_saveConfigButton_clicked();

    /**
     * @brief 处理连接数据库按钮点击事件
     */
    void on_connectDatabaseButton_clicked();
    
    /**
     * @brief 处理地块列表项点击事件
     */
    void on_fieldListWidget_itemClicked(QListWidgetItem* item);
    
    /**
     * @brief 处理地图菜单项点击事件
     */
    void on_actionMap_triggered();
    
    /**
     * @brief 处理配置菜单项点击事件
     */
    void on_actionSettings_triggered();
    
    /**
     * @brief 处理存储菜单项点击事件
     */
    void on_actionStorage_triggered();
    
    /**
     * @brief 处理概要菜单项点击事件
     */
    void on_actionSummary_triggered();
    

    
    /**
     * @brief 处理生成配置按钮点击事件
     */
    void on_generateConfigButton_clicked();
    
    // 暂时移除updateLocalTime槽函数的声明，避免编译错误
    // /**
    //  * @brief 更新本地时间显示
    //  */
    // void updateLocalTime();

private:
    Ui::MainWindow *ui; ///< UI界面指针
    StateEstimator *stateEstimator; ///< 状态估计模块
    Coordinate *coordinate; ///< 坐标转换模块
    BlockGenerator *blockGenerator; ///< 小区生成模块
    
    // 子窗口UI指针
    Ui::MapSubWindow *mapSubWindowUi; ///< 地图子窗口UI指针
    Ui::SettingsSubWindow *settingsSubWindowUi; ///< 配置子窗口UI指针
    Ui::StorageSubWindow *storageSubWindowUi; ///< 存储子窗口UI指针
    Ui::SummarySubWindow *summarySubWindowUi; ///< 概要信息子窗口UI指针
    
    // 子窗口指针
    QMdiSubWindow *mapSubWindow; ///< 地图子窗口指针
    QMdiSubWindow *settingsSubWindow; ///< 配置子窗口指针
    QMdiSubWindow *storageSubWindow; ///< 存储子窗口指针
    QMdiSubWindow *summarySubWindow; ///< 概要信息子窗口指针
    
    // 静态指针，用于在静态方法中访问MainWindow实例
    static MainWindow* instance;
    


    // 传感器
    std::unique_ptr<ImuSensor> imuThread; ///< IMU传感器
    std::unique_ptr<GpsSensor> gpsThread; ///< GPS传感器

    // 原始数据显示组件不再作为成员变量，而是在initUI中创建并添加到布局
    
    QTimer *uiUpdateTimer; ///< 用于更新UI的定时器，每0.1秒刷新一次
    
    // 数据计数和时间更新相关变量
    int imuDataCount; ///< IMU有效数据计数
    int gpsDataCount; ///< GPS有效数据计数
    QTimer *dataCountTimer; ///< 用于数据计数的定时器
    QTimer *localTimeTimer; ///< 用于更新本地时间的定时器
    
    // 上次UI更新时间戳，用于控制更新频率
    double lastImuUpdateTime; ///< 上次IMU数据UI更新时间戳
    double lastGnssUpdateTime; ///< 上次GNSS数据UI更新时间戳
    
    // 数据有效性标志
    bool hasValidImuData; ///< 是否已经获取到过有效的IMU数据
    bool hasValidGnssData; ///< 是否已经获取到过有效的GNSS数据
    
    // 存储上次有效的数据
    IMU lastValidImuData; ///< 上次有效的IMU数据
    GNSS lastValidGnssData; ///< 上次有效的GNSS数据

    // IMU UI 更新合并：避免 200Hz+ 逐帧刷新导致 UI 队列积压。
    // 每次收到 IMU 信号仅缓存最新数据，并在下一轮事件循环合并执行一次真实 UI 刷新。
    bool imuUiUpdatePending_ = false;
    IMU lastImuUiData_;

    // GPS UI 更新合并：GPS 可能 20Hz（模拟）甚至更高，使用与 IMU 相同的合并策略保证“实时显示”且不卡 UI。
    bool gpsUiUpdatePending_ = false;
    GNSS lastGnssUiData_;
    
    // UI更新间隔（毫秒）
    const int uiUpdateInterval = 100; ///< UI更新间隔，100毫秒
    
    // 地图更新定时器（每秒更新一次）
    QTimer *mapUpdateTimer; ///< 用于地图更新的定时器，每秒刷新一次
    
    // 最新的GPS数据（用于定时更新地图）
    GNSS latestGnssData; ///< 最新的有效GNSS数据

    bool hasLocalFrameConfigured = false; ///< 是否已完成O/A本地坐标系配置
    QPointF blockStartWorldPosition_ = QPointF(0.0, 0.0); ///< 用户指定的小区起始位置（本地坐标）

    /**
     * @brief 初始化UI组件
     */
    void initUI();
    
    /**
     * @brief 初始化子窗口
     */
    void initSubWindows();

    /**
     * @brief 初始化各个功能模块
     */
    void initModules();
    
    // 传感器线程初始化已合并到initSignalsSlots方法中
    
    /**
     * @brief 初始化坐标系统组件
     */
    void initCoordinateWidget();
    
    /**
     * @brief 初始化信号槽连接
     */
    void initSignalsSlots();
    
    /**
     * @brief 更新地图显示
     */
    void updateMapDisplay();
    
    /**
     * @brief 初始化串口选择下拉框
     */
    void initSerialPortComboBoxes();
    
    /**
     * @brief 初始化波特率选择下拉框
     */
    void initBaudRateComboBoxes();
    
    /**
     * @brief 初始化日志系统
     */
    void initLogger();
    
    /**
     * @brief 更新IMU状态标签
     * @param status 状态：0-未连接，1-已连接，2-连接错误
     */
    void updateImuStatusLabel(int status);
    
    /**
     * @brief 更新GPS状态标签
     * @param status 状态：0-未连接，1-已连接，2-连接错误
     */
    void updateGpsStatusLabel(int status);
    
    /**
     * @brief 更新PLC状态标签
     * @param status 状态：0-未连接，1-已连接，2-连接错误
     */
    void updatePlcStatusLabel(int status);
    
    /**
     * @brief 从定时器触发的地图更新函数，每秒调用一次
     */
    void updateMapFromTimer();
    
    /**
     * @brief 将时间戳转换为可读的时间格式
     * @param timestamp 时间戳（秒）
     * @return 可读的时间字符串
     */
    QString timestampToDateTime(double timestamp);
    

    
    // 暂时移除initTimeUpdate函数的声明，避免编译错误
    // /**
    //  * @brief 初始化时间更新定时器
    //  */
    // void initTimeUpdate();
};

#endif // MAINWINDOW_H