#ifndef SENSORINTERFACE_H
#define SENSORINTERFACE_H

#include <QObject>
#include <QThread>
#include <QSerialPortInfo>
// 包含KF-GINS的头文件
#include "../common/types.h"
#include "../fileio/imuserialloader.h"
#include "../fileio/gnssserialloader.h"

/**
 * @brief 传感器接口类，负责与后端传感器通信和数据获取
 */
class SensorInterface : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父对象指针
     */
    explicit SensorInterface(QObject *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~SensorInterface();
    
    
    /**
     * @brief 启动IMU数据采集
     * @return 是否启动成功
     */
    bool startIMU();
    
    /**
     * @brief 停止IMU数据采集
     */
    void stopIMU();
    
    /**
     * @brief 启动GPS数据采集
     * @return 是否启动成功
     */
    bool startGPS();
    
    /**
     * @brief 停止GPS数据采集
     */
    void stopGPS();
    
    
    /**
     * @brief 连接IMU模块
     * @return 是否连接成功
     */
    bool connectIMU();
    
    /**
     * @brief 断开IMU模块连接
     */
    void disconnectIMU();
    
    /**
     * @brief 连接GPS模块
     * @return 是否连接成功
     */
    bool connectGPS();
    
    /**
     * @brief 断开GPS模块连接
     */
    void disconnectGPS();

    /**
     * @brief 获取IMU原始数据
     * @return IMU原始数据
     */
    QString getRawImuData() const;

    /**
     * @brief 获取GNSS原始数据
     * @return GNSS原始数据
     */
    QString getRawGnssData() const;

    /**
     * @brief 设置IMU串口参数
     * @param imuPort IMU串口端口
     * @param imuBaudrate IMU串口波特率
     */
    void setImuSerialParams(const QString &imuPort, int imuBaudrate);

    /**
     * @brief 设置GPS串口参数
     * @param gnssPort GNSS串口端口
     * @param gnssBaudrate GNSS串口波特率
     */
    void setGpsSerialParams(const QString &gnssPort, int gnssBaudrate);


    
    /**
     * @brief 获取IMU加载器指针
     * @return IMU加载器指针
     */
    ImuSerialLoader* getImuLoader() const;
    
    /**
     * @brief 获取GNSS加载器指针
     * @return GNSS加载器指针
     */
    GnssSerialLoader* getGnssLoader() const;
    
    /**
     * @brief 检查IMU是否连接
     * @return IMU是否连接
     */
    bool isImuConnected() const;
    
    /**
     * @brief 检查GNSS是否连接
     * @return GNSS是否连接
     */
    bool isGnssConnected() const;

    /**
     * @brief 获取系统中所有可用的串口列表
     * @return 可用串口列表
     */
    static QList<QSerialPortInfo> getAvailableSerialPorts();

signals:
    
    /**
     * @brief IMU数据接收信号
     * @param imuData IMU数据
     */
    void imuDataReceived(const IMU& imuData);
    
    /**
     * @brief GNSS数据接收信号
     * @param gnssData GNSS数据
     */
    void gnssDataReceived(const GNSS& gnssData);

private slots:
    /**
     * @brief IMU数据采集线程函数
     * @note 在独立线程中运行，实时读取和处理IMU数据
     */
    void imuDataCollectionThread();
    
    /**
     * @brief GPS数据采集线程函数
     * @note 在独立线程中运行，实时读取和处理GPS数据
     */
    void gnssDataCollectionThread();

private:
    // KF-GINS加载器
    std::unique_ptr<ImuSerialLoader> imuLoader_;
    std::unique_ptr<GnssSerialLoader> gnssLoader_;

    // 数据采集线程
    QThread *imuThread_;
    QThread *gnssThread_;

    int updateFrequency; ///< 更新频率（Hz）
    bool imuRunning_; ///< IMU是否正在运行
    bool gnssRunning_; ///< GPS是否正在运行
    bool imuConnected_; ///< IMU是否连接
    bool gnssConnected_; ///< GNSS是否连接

    // 串口参数
    QString imuPort_;
    int imuBaudrate_;
    QString gnssPort_;
    int gnssBaudrate_;

    // 原始数据缓存
    QString rawImuData_;
    QString rawGnssData_;

    // 最新数据缓存
    IMU latestImuData_;
    GNSS latestGnssData_;

public:
    /**
     * @brief 获取最新的IMU数据
     * @return 最新的IMU数据
     */
    IMU getLatestImuData() const;

    /**
     * @brief 获取最新的GNSS数据
     * @return 最新的GNSS数据
     */
    GNSS getLatestGnssData() const;
};

#endif // SENSORINTERFACE_H