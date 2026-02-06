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
#include "core/estimator/statestimator.h"
#include "core/sensor/sensorinterface.h"
#include "core/common/logger.h"



QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

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

private slots:
    /**
     * @brief 处理数据校准按钮点击事件
     */
    void on_calibrateDataButton_clicked();
    
    /**
     * @brief 在UI中追加日志消息
     * @param message 日志消息
     */
    void appendLogMessage(const QString& message);
    
    /**
     * @brief 处理位置更新事件
     */
#ifdef QT_POSITIONING_LIB
    void onPositionUpdated(const QGeoPositionInfo &info);
#endif
    
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
     * @brief 处理IMU数据更新事件
     */
    void onImuDataUpdated();
    
    /**
     * @brief 处理GPS数据更新事件
     */
    void onGnssDataUpdated();
    
    /**
     * @brief 处理地图类型切换事件
     */
    void on_mapTypeComboBox_currentIndexChanged(int index);
    
    /**
     * @brief 处理数据源切换事件
     */
    void on_dataSourceComboBox_currentIndexChanged(int index);
    
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
     * @brief 处理历史作业列表项点击事件
     */
    void on_historyListWidget_itemClicked(QListWidgetItem* item);
    
    /**
     * @brief 处理日志级别下拉框选择事件
     */
    void on_logLevelComboBox_currentIndexChanged(int index);
    
    // 暂时移除updateLocalTime槽函数的声明，避免编译错误
    // /**
    //  * @brief 更新本地时间显示
    //  */
    // void updateLocalTime();

private:
    Ui::MainWindow *ui; ///< UI界面指针
    StateEstimator *stateEstimator; ///< 状态估计模块
    
    // 静态指针，用于在静态方法中访问MainWindow实例
    static MainWindow* instance;
    
    // 当前日志显示级别
    Logger::LogLevel currentLogLevel; ///< 当前日志显示级别
    
    /**
     * @brief 日志消息处理器
     * @param message 日志消息
     */
    static void logMessageHandler(const std::string& message);
    
    /**
     * @brief 解析日志消息中的级别
     * @param message 日志消息
     * @return 日志级别
     */
    Logger::LogLevel parseLogLevel(const QString& message);

    // 传感器接口
    std::unique_ptr<SensorInterface> sensorInterface; ///< 传感器接口

    // 原始数据显示组件不再作为成员变量，而是在initUI中创建并添加到布局
    
    QTimer *uiUpdateTimer; ///< 用于更新UI的定时器，每0.1秒刷新一次
    
    /**
     * @brief 初始化UI组件
     */
    void initUI();
    
    /**
     * @brief 初始化各个功能模块
     */
    void initModules();
    
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