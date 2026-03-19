#ifndef SENSORINTERFACE_H
#define SENSORINTERFACE_H

#include <QObject>
#include <QThread>
#include <QSerialPortInfo>
#include <QTimer>
#include <QElapsedTimer>
#include <vector>
// 包含 KF-GINS 的头文件
#include "../common/types.h"
#include "../fileio/imuserialloader.h"
#include "../fileio/gnssserialloader.h"

// 传感器基类
class SensorBase : public QObject
{
    Q_OBJECT

protected:
    QString port_;
    int baudrate_;
    bool connected_;
    bool running_;
    QThread *thread_;
    QTimer *watchdogTimer_; // 看门狗定时器，防止信号丢失
    
    // 时间同步相关变量
    double timeSyncDt_; // 时间同步的 dt 值
    bool timeSynced_; // 时间是否已同步

public:
    /**
     * @brief 构造函数
     * @param parent 父对象指针
     */
    explicit SensorBase(QObject *parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~SensorBase();

    /**
     * @brief 设置串口参数
     * @param port 串口端口
     * @param baudrate 波特率
     */
    void setSerialParams(const QString &port, int baudrate);

    /**
     * @brief 连接传感器
     * @return 是否连接成功
     */
    virtual bool connect() = 0;

    /**
     * @brief 断开传感器连接
     */
    virtual void disconnect() = 0;

    /**
     * @brief 启动传感器数据采集
     * @return 是否启动成功
     */
    virtual bool start() = 0;

    /**
     * @brief 停止传感器数据采集
     */
    virtual void stop() = 0;
    
    /**
     * @brief 启动传感器线程
     * @return 是否启动成功
     */
    bool startThread();
    
    /**
     * @brief 停止传感器线程
     */
    void stopThread();
    
    /**
     * @brief 传感器时钟方法，用于记录传感器上的时间
     * @return 传感器上的时间戳
     */
    virtual double clock();

    /**
     * @brief 获取最新传感器数据
     * @return 数据指针
     */
    virtual void* getLatestData() const = 0;

    /**
     * @brief 检查是否连接
     * @return 是否连接
     */
    bool isConnected() const;

    /**
     * @brief 检查是否运行
     * @return 是否运行
     */
    bool isRunning() const;

    /**
     * @brief 获取串口端口
     * @return 串口端口
     */
    QString getPort() const;

    /**
     * @brief 获取波特率
     * @return 波特率
     */
    int getBaudrate() const;
    
    /**
     * @brief 获取系统中所有可用的串口列表
     * @return 可用串口列表
     */
    static QList<QSerialPortInfo> getAvailableSerialPorts();
    
    /**
     * @brief 时间同步方法，为所有传感器同步时间
     * @param systemTimestamp 系统时间戳
     * @param utcTime UTC时间
     * @return 是否同步成功
     */
    virtual bool syncTime(double systemTimestamp, double utcTime);
    
    /**
     * @brief 设置时间同步的dt值
     * @param dt 时间同步的dt值
     */
    void setTimeSyncDt(double dt);
    
    /**
     * @brief 设置时间同步状态
     * @param synced 时间是否已同步
     */
    void setTimeSynced(bool synced);
    
    /**
     * @brief 获取时间同步的dt值
     * @return 时间同步的dt值
     */
    double getTimeSyncDt() const;
    
    /**
     * @brief 检查时间是否已同步
     * @return 时间是否已同步
     */
    bool isTimeSynced() const;
};

// IMU 传感器类
class ImuSensor : public SensorBase
{
    Q_OBJECT

private:
    ImuSerialLoader* imuLoader_;

    // 看门狗：最后一次收到有效 IMU 数据的时间
    QElapsedTimer lastValidDataTimer_;

    // 数据缓存和批量发送相关
    std::vector<IMU> imuDataBuffer_; // IMU 数据缓存
    static const int MAX_BUFFER_SIZE = 15; // 最大缓存 15 帧数据
    QTimer *dataSendTimer_; // 定时发送数据的定时器
    QTimer *watchdogTimer_; // 看门狗定时器，防止信号丢失

public:
    /**
     * @brief 构造函数
     * @param parent 父对象指针
     */
    explicit ImuSensor(QObject *parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~ImuSensor();

    /**
     * @brief 连接IMU传感器
     * @return 是否连接成功
     */
    bool connect() override;

    /**
     * @brief 断开IMU传感器连接
     */
    void disconnect() override;

    /**
     * @brief 启动IMU数据采集
     * @return 是否启动成功
     */
    bool start() override;

    /**
     * @brief 停止IMU数据采集
     */
    void stop() override;

    /**
     * @brief 获取最新IMU数据
     * @return 数据指针
     */
    void* getLatestData() const override;

    /**
     * @brief 获取IMU加载器
     * @return IMU加载器指针
     */
    ImuSerialLoader* getLoader() const;
    
    /**
     * @brief 设置 IMU 串口参数
     * @param port 串口端口
     * @param baudrate 波特率
     */
    void setSerialParams(const QString &port, int baudrate);
    
    /**
     * @brief 看门狗定时器超时处理
     */
    void onWatchdogTimeout();

private slots:
    /**
     * @brief IMU 数据采集线程函数
     */
    void imuDataCollectionThread();
    
    /**
     * @brief 定时发送缓存中的数据
     */
    void sendBufferedData();

signals:
    /**
     * @brief IMU 数据接收信号
     * @param imuData IMU 数据
     */
    void imuDataReceived(const IMU& imuData);
};

// GPS 传感器类
class GpsSensor : public SensorBase
{
    Q_OBJECT

private:
    GnssSerialLoader* gnssLoader_;

    // 看门狗：最后一次收到有效 GNSS 数据的时间
    QElapsedTimer lastValidDataTimer_;
    int watchdogTimeoutStrikes_ = 0; // 连续超时次数（用于 1Hz GPS 的抖动容忍）

    // 时间同步相关变量
    std::vector<double> localTimeDifferences_;
    int validGgaCount_; // 有效的 GGA 消息计数
    
    // 数据缓存和批量发送相关
    std::vector<GNSS> gnssDataBuffer_; // GNSS 数据缓存
    static const int MAX_BUFFER_SIZE = 15; // 最大缓存 15 帧数据
    QTimer *dataSendTimer_; // 定时发送数据的定时器
    QTimer *watchdogTimer_; // 看门狗定时器，防止信号丢失

public:
    /**
     * @brief 构造函数
     * @param parent 父对象指针
     */
    explicit GpsSensor(QObject *parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~GpsSensor();

    /**
     * @brief 连接GPS传感器
     * @return 是否连接成功
     */
    bool connect() override;

    /**
     * @brief 断开GPS传感器连接
     */
    void disconnect() override;

    /**
     * @brief 启动GPS数据采集
     * @return 是否启动成功
     */
    bool start() override;

    /**
     * @brief 停止GPS数据采集
     */
    void stop() override;

    /**
     * @brief 获取最新GPS数据
     * @return 数据指针
     */
    void* getLatestData() const override;

    /**
     * @brief 获取GPS加载器
     * @return GPS加载器指针
     */
    GnssSerialLoader* getLoader() const;
    
    /**
     * @brief 设置GPS串口参数
     * @param port 串口端口
     * @param baudrate 波特率
     */
    void setSerialParams(const QString &port, int baudrate);
    
    /**
     * @brief 时间同步方法，为所有传感器同步时间
     * @param gnssData GNSS 数据
     */
    void syncTime(const GNSS& gnssData);
    
    /**
     * @brief 看门狗定时器超时处理
     */
    void onWatchdogTimeout();

private slots:
    /**
     * @brief GPS 数据采集线程函数
     */
    void gpsDataCollectionThread();
    
    /**
     * @brief 定时发送缓存中的数据
     */
    void sendBufferedData();

signals:
    /**
     * @brief GNSS 数据接收信号
     * @param gnssData GNSS 数据
     */
    void gnssDataReceived(const GNSS& gnssData);
    
    /**
     * @brief 时间同步完成信号
     * @param dt 时间同步的 dt 值
     */
    void timeSyncCompleted(double dt);
};



#endif // SENSORINTERFACE_H