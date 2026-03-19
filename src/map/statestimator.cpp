#include "statestimator.h"

#include <cmath>
#include <vector>
#include <QDebug>
#include <stdexcept>
#include <utility>

#include "../core/common/logger.h"

/**
 * @brief 构造函数，初始化默认参数
 */
StateEstimator::StateEstimator() 
    : m_coordinate(nullptr),
      m_useKalman(false),
      m_lowPassAlpha(0.2),
      m_lastTimestamp(0.0),
      m_lastLat(0.0),
      m_lastLon(0.0),
      m_lastSpeed(0.0),
      m_lastHeading(0.0),
      m_historyMaxSize(10),
      m_interval(5.0),
      m_nextTargetIndex(0),
      m_currentTargetY(0.0),
      m_isArmed(false),
      m_hasTriggered(false),
      m_sysDelayMs(50),
      m_deadzoneM(1.0)
{
    LOG_INFO("StateEstimator initialized with OA local coordinate system");
    LOG_DEBUG("Initial configuration - Use Kalman: %d, Low-pass alpha: %f", m_useKalman, m_lowPassAlpha);
    
    m_currentPose.x = 0.0;
    m_currentPose.y = 0.0;
    m_currentPose.vx = 0.0;
    m_currentPose.vy = 0.0;
    m_currentPose.heading = 0.0;
    m_currentPose.magneticHeading = 0.0;
    m_currentPose.latitude = 0.0;
    m_currentPose.longitude = 0.0;
    m_currentPose.timestamp = 0.0;

    LOG_DEBUG("Initial pose - Position: (%f, %f), Velocity: (%f, %f), Heading: %f",
             m_currentPose.x, m_currentPose.y, m_currentPose.vx, m_currentPose.vy, m_currentPose.heading);
}

/**
 * @brief 析构函数
 */
StateEstimator::~StateEstimator()
{
}

/**
 * @brief 设置坐标转换器
 * @param coordinate 坐标转换器指针
 */
void StateEstimator::setCoordinate(Coordinate* coordinate)
{
    m_coordinate = coordinate;
    LOG_INFO("Coordinate converter set for StateEstimator");
}

/**
 * @brief 设置使用卡尔曼滤波
 * @param useKalman 是否使用卡尔曼滤波
 */
void StateEstimator::setUseKalman(bool useKalman)
{
    LOG_INFO("Setting useKalman to: %d", useKalman);
    m_useKalman = useKalman;
}

/**
 * @brief 设置低通滤波系数
 * @param alpha 低通滤波系数（0-1），默认0.2
 */
void StateEstimator::setLowPassAlpha(double alpha)
{
    double clampedAlpha = std::max(0.0, std::min(1.0, alpha));
    LOG_INFO("Setting lowPassAlpha from %f to %f (original input: %f)", m_lowPassAlpha, clampedAlpha, alpha);
    m_lowPassAlpha = clampedAlpha;
}

/**
 * @brief 更新状态估计
 * @param data 传感器数据
 */
void StateEstimator::update(const SensorData& data)
{
    try {
        if (data.timestamp <= 0) {
            throw std::runtime_error("Invalid timestamp");
        }
        
        if (std::isnan(data.lat) || std::isnan(data.lon)) {
            throw std::runtime_error("Invalid position data");
        }
        
        if (!m_coordinate || !m_coordinate->isInitialized()) {
            LOG_WARN("Coordinate not initialized, skipping state update");
            return;
        }
        
        LOG_DEBUG("State estimator updating - Timestamp: %f, Position: (%f, %f)", 
                 data.timestamp, data.lat, data.lon);
        
        QPointF localXY = m_coordinate->wgs84ToXY(data.lat, data.lon);
        double x = localXY.x();
        double y = localXY.y();
        
        LocalPoint point;
        point.x = x;
        point.y = y;
        point.timestamp = data.timestamp;
        m_buffer.push_back(point);
        
        if (m_buffer.size() > static_cast<size_t>(m_historyMaxSize)) {
            m_buffer.erase(m_buffer.begin());
        }
        
        double vx, vy;
        estimateVelocity(vx, vy);
        
        if (vx != 0 || vy != 0) {
            double headingRad = std::atan2(vx, vy);
            m_currentPose.heading = headingRad * 180.0 / M_PI;
            if (m_currentPose.heading < 0) {
                m_currentPose.heading += 360.0;
            }
        }
        
        if (m_useKalman) {
            LOG_DEBUG("Using Kalman filter for state update");
            updateKalman(data, x, y);
        } else {
            LOG_DEBUG("Using low-pass filter for state update");
            updateLowPass(data, x, y);
        }
        
        m_currentPose.timestamp = data.timestamp;
        m_currentPose.latitude = data.lat;
        m_currentPose.longitude = data.lon;

        m_lastTimestamp = data.timestamp;
        m_lastLat = data.lat;
        m_lastLon = data.lon;
        m_lastSpeed = data.speed;
        m_lastHeading = data.heading;
        
        checkCrossing();
        
        static int counter = 0;
        if (++counter % 10 == 0) {
            LOG_INFO("State estimator update - Local XY: (%f, %f), Velocity: (%f, %f), Heading: %f", 
                     m_currentPose.x, m_currentPose.y, m_currentPose.vx, m_currentPose.vy, m_currentPose.heading);
        }
    } catch (const std::exception& e) {
        LOG_Error("Error updating state estimator: %s", e.what());
    }
}

/**
 * @brief 使用低通滤波更新状态
 * @param data 传感器数据
 * @param x OA本地X坐标
 * @param y OA本地Y坐标
 */
void StateEstimator::updateLowPass(const SensorData& data, double x, double y)
{
    auto velocity = calculateVelocity(data);
    double vx = velocity.first;
    double vy = velocity.second;
    
    LOG_DEBUG("Low-pass filter update - Before: Position (%f, %f), Velocity (%f, %f)", 
             m_currentPose.x, m_currentPose.y, m_currentPose.vx, m_currentPose.vy);
    
    m_currentPose.x = m_lowPassAlpha * x + (1.0 - m_lowPassAlpha) * m_currentPose.x;
    m_currentPose.y = m_lowPassAlpha * y + (1.0 - m_lowPassAlpha) * m_currentPose.y;
    m_currentPose.vx = m_lowPassAlpha * vx + (1.0 - m_lowPassAlpha) * m_currentPose.vx;
    m_currentPose.vy = m_lowPassAlpha * vy + (1.0 - m_lowPassAlpha) * m_currentPose.vy;
    m_currentPose.heading = m_lowPassAlpha * data.heading + (1.0 - m_lowPassAlpha) * m_currentPose.heading;
    
    LOG_DEBUG("Low-pass filter update - After: Position (%f, %f), Velocity (%f, %f)", 
             m_currentPose.x, m_currentPose.y, m_currentPose.vx, m_currentPose.vy);
}

/**
 * @brief 使用卡尔曼滤波更新状态
 * @param data 传感器数据
 * @param x OA本地X坐标
 * @param y OA本地Y坐标
 */
void StateEstimator::updateKalman(const SensorData& data, double x, double y)
{
    auto velocity = calculateVelocity(data);
    double vx = velocity.first;
    double vy = velocity.second;
    
    LOG_DEBUG("Kalman filter update - Before: Position (%f, %f), Velocity (%f, %f)", 
             m_currentPose.x, m_currentPose.y, m_currentPose.vx, m_currentPose.vy);
    
    double kalmanGain = 0.1;
    m_currentPose.x = m_currentPose.x + kalmanGain * (x - m_currentPose.x);
    m_currentPose.y = m_currentPose.y + kalmanGain * (y - m_currentPose.y);
    m_currentPose.vx = m_currentPose.vx + kalmanGain * (vx - m_currentPose.vx);
    m_currentPose.vy = m_currentPose.vy + kalmanGain * (vy - m_currentPose.vy);
    m_currentPose.heading = m_currentPose.heading + kalmanGain * (data.heading - m_currentPose.heading);
    
    LOG_DEBUG("Kalman filter update - After: Position (%f, %f), Velocity (%f, %f)", 
             m_currentPose.x, m_currentPose.y, m_currentPose.vx, m_currentPose.vy);
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
    
    LOG_DEBUG("Calculating velocity - Speed: %f, Heading: %f", data.speed, data.heading);
    
    if (data.speed > 0.0) {
        double headingRad = data.heading * M_PI / 180.0;
        vx = data.speed * std::sin(headingRad);
        vy = data.speed * std::cos(headingRad);
        LOG_DEBUG("Velocity from speed data - Vx: %f, Vy: %f", vx, vy);
    } else if (m_lastTimestamp > 0.0 && data.timestamp > m_lastTimestamp) {
        double dt = data.timestamp - m_lastTimestamp;
        if (dt > 0.0 && m_buffer.size() >= 2) {
            const LocalPoint& lastPoint = m_buffer[m_buffer.size() - 2];
            vx = (m_currentPose.x - lastPoint.x) / dt;
            vy = (m_currentPose.y - lastPoint.y) / dt;
            LOG_DEBUG("Velocity from position difference - Vx: %f, Vy: %f, dt: %f", vx, vy, dt);
        }
    }
    
    return std::make_pair(vx, vy);
}

/**
 * @brief 使用滑动窗口线性回归估算速度
 * @param vx 输出x方向速度
 * @param vy 输出y方向速度
 */
void StateEstimator::estimateVelocity(double& vx, double& vy)
{
    if (m_buffer.size() < 2) {
        vx = 0.0;
        vy = 0.0;
        return;
    }
    
    int n = static_cast<int>(m_buffer.size());
    double sumT = 0, sumX = 0, sumY = 0, sumTX = 0, sumTY = 0, sumT2 = 0;
    
    double t0 = m_buffer.front().timestamp;
    
    for (const auto& p : m_buffer) {
        double t = p.timestamp - t0;
        sumT += t;
        sumX += p.x;
        sumY += p.y;
        sumTX += t * p.x;
        sumTY += t * p.y;
        sumT2 += t * t;
    }
    
    double denom = n * sumT2 - sumT * sumT;
    if (std::abs(denom) < 1e-6) {
        vx = 0.0;
        vy = 0.0;
        return;
    }
    
    vx = (n * sumTX - sumT * sumX) / denom;
    vy = (n * sumTY - sumT * sumY) / denom;
    
    LOG_DEBUG("Velocity estimated using linear regression - Vx: %f, Vy: %f", vx, vy);
}

/**
 * @brief 获取估计的姿态
 * @return 估计的姿态
 */
EstimatedPose StateEstimator::getPose() const
{
    return m_currentPose;
}

/**
 * @brief 重置状态估计器
 */
void StateEstimator::reset()
{
    LOG_INFO("State estimator resetting to initial state");
    
    m_currentPose.x = 0.0;
    m_currentPose.y = 0.0;
    m_currentPose.vx = 0.0;
    m_currentPose.vy = 0.0;
    m_currentPose.heading = 0.0;
    m_currentPose.magneticHeading = 0.0;
    m_currentPose.latitude = 0.0;
    m_currentPose.longitude = 0.0;
    m_currentPose.timestamp = 0.0;
    
    m_lastTimestamp = 0.0;
    m_lastLat = 0.0;
    m_lastLon = 0.0;
    m_lastSpeed = 0.0;
    m_lastHeading = 0.0;
    
    m_buffer.clear();
    
    m_nextTargetIndex = 0;
    m_currentTargetY = 0.0;
    m_isArmed = false;
    m_hasTriggered = false;
    
    LOG_DEBUG("State estimator reset completed");
}

/**
 * @brief 校准状态估计器
 */
void StateEstimator::calibrate()
{
    LOG_INFO("State estimator calibration started");
    
    LOG_DEBUG("Calibration - Before: Heading %f, Magnetic Heading %f", 
             m_currentPose.heading, m_currentPose.magneticHeading);
    
    m_currentPose.heading = 0.0;
    m_currentPose.magneticHeading = 0.0;
    m_lastHeading = 0.0;
    
    LOG_DEBUG("Calibration - After: Heading %f, Magnetic Heading %f", 
             m_currentPose.heading, m_currentPose.magneticHeading);
    
    LOG_INFO("State estimator calibrated: current heading set to 0 degrees");
}

/**
 * @brief 检查是否穿越触发线
 */
void StateEstimator::checkCrossing()
{
    if (m_buffer.size() < 2) {
        return;
    }
    
    LocalPoint p1 = m_buffer[m_buffer.size() - 2];
    LocalPoint p2 = m_buffer.back();
    
    bool crossing = false;
    if ((p1.y <= m_currentTargetY && p2.y >= m_currentTargetY) || 
        (p1.y >= m_currentTargetY && p2.y <= m_currentTargetY)) {
        crossing = true;
    }
    
    if (crossing) {
        double alpha = (m_currentTargetY - p1.y) / (p2.y - p1.y);
        if (alpha < 0 || alpha > 1) {
            return;
        }
        
        double deltaT = p2.timestamp - p1.timestamp;
        double triggerTime = p1.timestamp + alpha * deltaT;
        double currentTime = p2.timestamp;
        double timeRemaining = triggerTime - currentTime;
        int timeRemainingMs = static_cast<int>(timeRemaining * 1000.0);
        
        LOG_DEBUG("Crossing detected - Target Y: %f, P1 Y: %f, P2 Y: %f, Alpha: %f, Time remaining: %d ms", 
                 m_currentTargetY, p1.y, p2.y, alpha, timeRemainingMs);
        
        if (timeRemainingMs <= m_sysDelayMs || timeRemaining < 0) {
            LOG_INFO("Triggering at target Y: %f, Estimated time: %f, Current time: %f", 
                     m_currentTargetY, triggerTime, currentTime);
            
            double triggerX = p1.x + alpha * (p2.x - p1.x);
            double triggerY = m_currentTargetY;
            
            LOG_DEBUG("Trigger point - X: %f, Y: %f", triggerX, triggerY);
            
            m_hasTriggered = true;
            m_isArmed = false;
        } else if (!m_isArmed && (m_currentTargetY - p2.y) < 2.0 && (m_currentTargetY - p2.y) > 0) {
            m_isArmed = true;
            LOG_DEBUG("Armed for target Y: %f, Distance: %f meters", 
                     m_currentTargetY, m_currentTargetY - p2.y);
        }
    }
    
    if (p2.y > m_currentTargetY + m_deadzoneM) {
        if (m_hasTriggered) {
            m_nextTargetIndex++;
            m_currentTargetY = m_nextTargetIndex * m_interval;
            m_hasTriggered = false;
            m_isArmed = false;
            LOG_INFO("Moving to next target Y: %f", m_currentTargetY);
        } else if (!m_hasTriggered && (p2.y > m_currentTargetY + m_deadzoneM)) {
            m_nextTargetIndex++;
            m_currentTargetY = m_nextTargetIndex * m_interval;
            m_isArmed = false;
            LOG_WARN("Skipping target Y: %f (missed trigger)", m_currentTargetY - m_interval);
        }
    }
}
