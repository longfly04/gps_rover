#ifndef STATEESTIMATOR_H
#define STATEESTIMATOR_H

#include <vector>
#include "coordinate.h"

/**
 * @brief 估计姿态结构体，包含位置、速度和航向
 * 
 * 所有坐标都在OA本地坐标系下：
 * - x: OA坐标系的X轴坐标（米）
 * - y: OA坐标系的Y轴坐标（米）
 */
struct EstimatedPose {
    double x;              ///< OA本地X坐标（米）
    double y;              ///< OA本地Y坐标（米）
    double vx;             ///< X方向速度（米/秒）
    double vy;             ///< Y方向速度（米/秒）
    double heading;        ///< 航向角（度）
    double magneticHeading; ///< 磁航向角（度）
    double latitude;       ///< 当前纬度（WGS84）
    double longitude;      ///< 当前经度（WGS84）
    double timestamp;      ///< 时间戳（秒）
};

/**
 * @brief 传感器数据结构体，包含GPS和IMU数据
 */
struct SensorData {
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
};

/**
 * @brief 本地坐标点结构体，用于滑动窗口
 */
struct LocalPoint {
    double x;           ///< OA本地X坐标（米）
    double y;           ///< OA本地Y坐标（米）
    double timestamp;   ///< 时间戳（秒）
};

/**
 * @brief 状态估计模块，实现卡尔曼滤波或低通滤波
 * 
 * 所有位置估计都在OA本地坐标系下进行：
 * - 原点O为参考点
 * - +Y轴沿OA方向
 * - +X轴为+Y轴顺时针旋转90°
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
     * @brief 设置坐标转换器
     * @param coordinate 坐标转换器指针
     */
    void setCoordinate(Coordinate* coordinate);
    
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
     * @brief 获取估计的姿态
     * @return 估计的姿态
     */
    EstimatedPose getPose() const;
    
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
     * @param x OA本地X坐标
     * @param y OA本地Y坐标
     */
    void updateLowPass(const SensorData& data, double x, double y);
    
    /**
     * @brief 使用卡尔曼滤波更新状态
     * @param data 传感器数据
     * @param x OA本地X坐标
     * @param y OA本地Y坐标
     */
    void updateKalman(const SensorData& data, double x, double y);
    
    /**
     * @brief 计算速度
     * @param data 传感器数据
     * @return 计算的速度（vx, vy）
     */
    std::pair<double, double> calculateVelocity(const SensorData& data);
    
    /**
     * @brief 使用滑动窗口线性回归估算速度
     * @param vx 输出x方向速度
     * @param vy 输出y方向速度
     */
    void estimateVelocity(double& vx, double& vy);
    
    /**
     * @brief 检查是否穿越触发线
     */
    void checkCrossing();
    
    Coordinate* m_coordinate; ///< 坐标转换器指针（外部管理）
    bool m_useKalman; ///< 是否使用卡尔曼滤波
    double m_lowPassAlpha; ///< 低通滤波系数
    EstimatedPose m_currentPose; ///< 当前估计的姿态
    double m_lastTimestamp; ///< 上一次更新的时间戳
    double m_lastLat; ///< 上一次的纬度
    double m_lastLon; ///< 上一次的经度
    double m_lastSpeed; ///< 上一次的速度
    double m_lastHeading; ///< 上一次的航向
    
    std::vector<LocalPoint> m_buffer; ///< 本地坐标点缓冲区
    int m_historyMaxSize; ///< 缓冲区最大大小
    
    double m_interval; ///< 触发间隔（米）
    int m_nextTargetIndex; ///< 下一个目标索引
    double m_currentTargetY; ///< 当前目标Y坐标
    bool m_isArmed; ///< 是否已锁定当前目标
    bool m_hasTriggered; ///< 当前目标是否已触发
    int m_sysDelayMs; ///< 系统延迟（毫秒）
    double m_deadzoneM; ///< 触发后死区（米）
};

#endif // STATEESTIMATOR_H
