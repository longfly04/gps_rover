#ifndef STATEESTIMATOR_H
#define STATEESTIMATOR_H

#include "predictivecontroller.h" // 包含State结构体定义

/**
 * @brief 传感器数据结构体，包含GPS和IMU数据
 */
typedef struct {
    double lat;      ///< 纬度
    double lon;      ///< 经度
    double altitude; ///< 海拔高度
    double heading;  ///< 航向角（度）
    double speed;    ///< 速度（米/秒）
    double accelX;   ///< X方向加速度（可选）
    double accelY;   ///< Y方向加速度（可选）
    double accelZ;   ///< Z方向加速度（可选）
    double gyroX;    ///< X方向角速度（可选）
    double gyroY;    ///< Y方向角速度（可选）
    double gyroZ;    ///< Z方向角速度（可选）
    double magX;     ///< X方向磁场强度（可选）
    double magY;     ///< Y方向磁场强度（可选）
    double magZ;     ///< Z方向磁场强度（可选）
    double temperature; ///< 温度（可选）
    bool hasIMU;     ///< 是否有IMU数据
    double timestamp; ///< 时间戳（秒）
} SensorData;

/**
 * @brief 状态估计模块，实现卡尔曼滤波或低通滤波
 */
class StateEstimator
{
public:
    /**
     * @brief 构造函数
     */
    StateEstimator();
    
    /**
     * @brief 析构函数
     */
    ~StateEstimator();
    
    /**
     * @brief 设置使用卡尔曼滤波
     * @param useKalman 是否使用卡尔曼滤波
     */
    void setUseKalman(bool useKalman);
    
    /**
     * @brief 设置低通滤波系数
     * @param alpha 低通滤波系数（0-1），默认0.2
     */
    void setLowPassAlpha(double alpha);
    
    /**
     * @brief 更新状态估计
     * @param data 传感器数据
     */
    void update(const SensorData& data);
    
    /**
     * @brief 获取估计的状态
     * @return 估计的状态
     */
    State getState() const;
    
    /**
     * @brief 重置状态估计器
     */
    void reset();
    
    /**
     * @brief 校准状态估计器
     * 以当前设备的姿态作为水平姿态，角度为0度
     */
    void calibrate();

private:
    /**
     * @brief 使用低通滤波更新状态
     * @param data 传感器数据
     */
    void updateLowPass(const SensorData& data);
    
    /**
     * @brief 使用卡尔曼滤波更新状态
     * @param data 传感器数据
     */
    void updateKalman(const SensorData& data);
    
    /**
     * @brief 计算速度
     * @param data 传感器数据
     * @return 计算的速度（vx, vy）
     */
    std::pair<double, double> calculateVelocity(const SensorData& data);
    
    bool useKalman; ///< 是否使用卡尔曼滤波
    double lowPassAlpha; ///< 低通滤波系数
    State currentState; ///< 当前估计的状态
    double lastTimestamp; ///< 上一次更新的时间戳
    double lastLat; ///< 上一次的纬度
    double lastLon; ///< 上一次的经度
    double lastSpeed; ///< 上一次的速度
    double lastHeading; ///< 上一次的航向
};

#endif // STATEESTIMATOR_H