#include "statestimator.h"
#include <cmath>

#include <QDebug>
#include <stdexcept>

// 包含日志头文件
#include "../common/logger.h"

/**
 * @brief 构造函数，初始化默认参数
 */
StateEstimator::StateEstimator() 
    : useKalman(false),
      lowPassAlpha(0.2),
      lastTimestamp(0.0),
      lastLat(0.0),
      lastLon(0.0),
      lastSpeed(0.0),
      lastHeading(0.0)
{
    // 记录初始化
    LOG_INFO("StateEstimator initialized with default parameters");
    LOG_DEBUG("Initial configuration - Use Kalman: %d, Low-pass alpha: %f", useKalman, lowPassAlpha);
    
    // 初始化当前状态
    currentState.x = 0.0;
    currentState.y = 0.0;
    currentState.vx = 0.0;
    currentState.vy = 0.0;
    currentState.heading = 0.0;
    currentState.magneticHeading = 0.0;
    
    // 记录初始状态
    LOG_DEBUG("Initial state - Position: (%f, %f), Velocity: (%f, %f), Heading: %f", 
             currentState.x, currentState.y, currentState.vx, currentState.vy, currentState.heading);
}

/**
 * @brief 析构函数
 */
StateEstimator::~StateEstimator()
{
}

/**
 * @brief 设置使用卡尔曼滤波
 * @param useKalman 是否使用卡尔曼滤波
 */
void StateEstimator::setUseKalman(bool useKalman)
{
    LOG_INFO("Setting useKalman to: %d", useKalman);
    this->useKalman = useKalman;
}

/**
 * @brief 设置低通滤波系数
 * @param alpha 低通滤波系数（0-1），默认0.2
 */
void StateEstimator::setLowPassAlpha(double alpha)
{
    // 确保alpha在0-1之间
    double clampedAlpha = std::max(0.0, std::min(1.0, alpha));
    LOG_INFO("Setting lowPassAlpha from %f to %f (original input: %f)", this->lowPassAlpha, clampedAlpha, alpha);
    this->lowPassAlpha = clampedAlpha;
}

/**
 * @brief 计算速度
 * @param data 传感器数据
 * @return 计算的速度（vx, vy）
 */
std::pair<double, double> StateEstimator::calculateVelocity(const SensorData& data)
{
    double vx = 0.0;
    double vy = 0.0;
    
    // 记录速度计算开始
    LOG_DEBUG("Calculating velocity from sensor data - Speed: %f, Heading: %f", data.speed, data.heading);
    
    // 检查是否有速度数据
    if (data.speed > 0.0) {
        // 将航向角转换为弧度
        double headingRad = data.heading * M_PI / 180.0;
        
        // 计算速度分量
        vx = data.speed * std::sin(headingRad);
        vy = data.speed * std::cos(headingRad);
        
        // 记录使用速度数据计算
        LOG_DEBUG("Velocity calculated from speed data - Vx: %f, Vy: %f", vx, vy);
    } else if (lastTimestamp > 0.0 && data.timestamp > lastTimestamp) {
        // 如果没有速度数据，使用位置差分计算速度
        double dt = data.timestamp - lastTimestamp;
        if (dt > 0.0) {
            // 这里简化处理，实际应该使用坐标转换
            // 假设lastLat和lastLon对应的XY坐标已知
            // 这里仅作为示例
            vx = (currentState.x - lastLat) / dt;
            vy = (currentState.y - lastLon) / dt;
            
            // 记录使用位置差分计算速度
            LOG_DEBUG("Velocity calculated from position difference - Vx: %f, Vy: %f, Time delta: %f", vx, vy, dt);
        }
    } else {
        // 记录无法计算速度的原因
        LOG_DEBUG("Unable to calculate velocity - No valid speed data or timestamp");
    }
    
    return std::make_pair(vx, vy);
}

/**
 * @brief 使用低通滤波更新状态
 * @param data 传感器数据
 */
void StateEstimator::updateLowPass(const SensorData& data)
{
    // 计算速度分量
    auto velocity = calculateVelocity(data);
    double vx = velocity.first;
    double vy = velocity.second;
    
    // 更新位置（这里简化处理，实际应该使用坐标转换）
    // 假设data.lat和data.lon已经转换为XY坐标
    // 这里仅作为示例
    double newX = data.lat; // 实际应该是转换后的X坐标
    double newY = data.lon; // 实际应该是转换后的Y坐标
    
    // 记录低通滤波更新前的状态
    LOG_DEBUG("Low-pass filter update - Before: Position (%f, %f), Velocity (%f, %f), Heading %f", 
             currentState.x, currentState.y, currentState.vx, currentState.vy, currentState.heading);
    
    // 应用低通滤波
    currentState.x = lowPassAlpha * newX + (1.0 - lowPassAlpha) * currentState.x;
    currentState.y = lowPassAlpha * newY + (1.0 - lowPassAlpha) * currentState.y;
    currentState.vx = lowPassAlpha * vx + (1.0 - lowPassAlpha) * currentState.vx;
    currentState.vy = lowPassAlpha * vy + (1.0 - lowPassAlpha) * currentState.vy;
    currentState.heading = lowPassAlpha * data.heading + (1.0 - lowPassAlpha) * currentState.heading;
    
    // 记录低通滤波更新后的状态
    LOG_DEBUG("Low-pass filter update - After: Position (%f, %f), Velocity (%f, %f), Heading %f", 
             currentState.x, currentState.y, currentState.vx, currentState.vy, currentState.heading);
    
    // 更新历史数据
    lastTimestamp = data.timestamp;
    lastLat = data.lat;
    lastLon = data.lon;
    lastSpeed = data.speed;
    lastHeading = data.heading;
}

/**
 * @brief 使用卡尔曼滤波更新状态
 * @param data 传感器数据
 */
void StateEstimator::updateKalman(const SensorData& data)
{
    // 这里实现简化版卡尔曼滤波
    // 完整的卡尔曼滤波需要状态转移矩阵、观测矩阵等
    // 这里使用类似低通滤波的实现，仅作为示例
    
    // 计算速度分量
    auto velocity = calculateVelocity(data);
    double vx = velocity.first;
    double vy = velocity.second;
    
    // 更新位置（这里简化处理，实际应该使用坐标转换）
    double newX = data.lat; // 实际应该是转换后的X坐标
    double newY = data.lon; // 实际应该是转换后的Y坐标
    
    // 记录卡尔曼滤波更新前的状态
    LOG_DEBUG("Kalman filter update - Before: Position (%f, %f), Velocity (%f, %f), Heading %f", 
             currentState.x, currentState.y, currentState.vx, currentState.vy, currentState.heading);
    
    // 使用与低通滤波类似的实现
    double kalmanGain = 0.1; // 简化的卡尔曼增益
    currentState.x = currentState.x + kalmanGain * (newX - currentState.x);
    currentState.y = currentState.y + kalmanGain * (newY - currentState.y);
    currentState.vx = currentState.vx + kalmanGain * (vx - currentState.vx);
    currentState.vy = currentState.vy + kalmanGain * (vy - currentState.vy);
    currentState.heading = currentState.heading + kalmanGain * (data.heading - currentState.heading);
    
    // 记录卡尔曼滤波更新后的状态
    LOG_DEBUG("Kalman filter update - After: Position (%f, %f), Velocity (%f, %f), Heading %f", 
             currentState.x, currentState.y, currentState.vx, currentState.vy, currentState.heading);
    
    // 更新历史数据
    lastTimestamp = data.timestamp;
    lastLat = data.lat;
    lastLon = data.lon;
    lastSpeed = data.speed;
    lastHeading = data.heading;
}

/**
 * @brief 更新状态估计
 * @param data 传感器数据
 */
void StateEstimator::update(const SensorData& data)
{
    try {
        // 检查时间戳有效性
        if (data.timestamp <= 0) {
            throw std::runtime_error("Invalid timestamp");
        }
        
        // 检查位置数据有效性
        if (std::isnan(data.lat) || std::isnan(data.lon)) {
            throw std::runtime_error("Invalid position data");
        }
        
        // 记录更新开始
        LOG_DEBUG("State estimator updating with new sensor data - Timestamp: %f, Position: (%f, %f)", 
                 data.timestamp, data.lat, data.lon);
        
        // 根据配置选择滤波方法
        if (useKalman) {
            LOG_DEBUG("Using Kalman filter for state update");
            updateKalman(data);
        } else {
            LOG_DEBUG("Using low-pass filter for state update");
            updateLowPass(data);
        }
        
        // 定期记录状态（每10次更新记录一次）
        static int counter = 0;
        if (++counter % 10 == 0) {
            LOG_INFO("State estimator update - Position: (%f, %f), Velocity: (%f, %f), Heading: %f", 
                     currentState.x, currentState.y, currentState.vx, currentState.vy, currentState.heading);
        }
    } catch (const std::exception& e) {
        LOG_ERROR("Error updating state estimator: %s", e.what());
        // 保持当前状态，不进行更新
    }
}

/**
 * @brief 获取估计的状态
 * @return 估计的状态
 */
State StateEstimator::getState() const
{
    return currentState;
}

/**
 * @brief 重置状态估计器
 */
void StateEstimator::reset()
{
    // 记录重置操作
    LOG_INFO("State estimator resetting to initial state");
    
    // 重置当前状态
    currentState.x = 0.0;
    currentState.y = 0.0;
    currentState.vx = 0.0;
    currentState.vy = 0.0;
    currentState.heading = 0.0;
    currentState.magneticHeading = 0.0;
    
    // 重置历史数据
    lastTimestamp = 0.0;
    lastLat = 0.0;
    lastLon = 0.0;
    lastSpeed = 0.0;
    lastHeading = 0.0;
    
    // 记录重置完成
    LOG_DEBUG("State estimator reset completed");
}

/**
 * @brief 校准状态估计器
 * 以当前设备的姿态作为水平姿态，角度为0度
 */
void StateEstimator::calibrate()
{
    // 记录校准操作开始
    LOG_INFO("State estimator calibration started");
    
    // 这里实现IMU传感器的校准功能
    // 以当前设备的姿态作为水平姿态，角度为0度
    // 实际应用中，这里应该根据IMU传感器的具体实现来进行校准
    
    // 记录校准前的状态
    LOG_DEBUG("State estimator calibration - Before: Heading %f, Magnetic Heading %f", 
             currentState.heading, currentState.magneticHeading);
    
    // 简单实现：将当前航向角设为0度，作为参考基准
    currentState.heading = 0.0;
    currentState.magneticHeading = 0.0;
    
    // 更新历史数据中的航向角
    lastHeading = 0.0;
    
    // 记录校准后的状态
    LOG_DEBUG("State estimator calibration - After: Heading %f, Magnetic Heading %f", 
             currentState.heading, currentState.magneticHeading);
    
    // 记录校准完成
    LOG_INFO("State estimator calibrated: current heading set to 0 degrees");
}