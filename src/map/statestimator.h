#ifndef STATEESTIMATOR_H
#define STATEESTIMATOR_H

#include <QtGlobal>

#include <utility>
#include <vector>

#include "blockgenerator.h"
#include "coordinate.h"

/**
 * @brief 估计姿态结构体，包含位置、速度和航向
 *
 * 所有坐标都在OA本地坐标系下：
 * - x: OA坐标系的X轴坐标（米）
 * - y: OA坐标系的Y轴坐标（米）
 */
struct EstimatedPose {
    double x = 0.0;              ///< OA本地X坐标（米）
    double y = 0.0;              ///< OA本地Y坐标（米）
    double vx = 0.0;             ///< X方向速度（米/秒）
    double vy = 0.0;             ///< Y方向速度（米/秒）
    double heading = 0.0;        ///< 航向角（度）
    double magneticHeading = 0.0; ///< 磁航向角（度）
    double latitude = 0.0;       ///< 当前纬度（WGS84）
    double longitude = 0.0;      ///< 当前经度（WGS84）
    double timestamp = 0.0;      ///< 时间戳（秒）

    bool hasNextTrigger = false; ///< 是否存在下一条待触发线
    int nextTriggerIndex = 0;    ///< 下一条待触发线编号（从1开始）
    double nextTriggerLineY = 0.0; ///< 下一条待触发线Y坐标（米）
    double nextTriggerCountdownSec = -1.0; ///< 下一次触发倒计时（秒）
    double predictedTriggerX = 0.0; ///< 预测触发点X坐标（米）
    double predictedTriggerY = 0.0; ///< 预测触发点Y坐标（米）
    qint64 predictedTriggerTimeUs = 0; ///< 预测触发时间戳（微秒）

    bool hasLastTriggerEvent = false; ///< 是否已有最近一次触发事件
    double lastTriggerX = 0.0;   ///< 最近一次触发点X坐标（米）
    double lastTriggerY = 0.0;   ///< 最近一次触发点Y坐标（米）
    qint64 lastTriggerTimeUs = 0; ///< 最近一次触发时间戳（微秒）
    int triggerSequence = 0;     ///< 已提交触发事件序号
};

/**
 * @brief 传感器数据结构体，包含GPS和IMU数据
 */
struct SensorData {
    double lat = 0.0;      ///< 纬度
    double lon = 0.0;      ///< 经度
    double altitude = 0.0; ///< 海拔高度
    double heading = 0.0;  ///< 航向角（度）
    double speed = 0.0;    ///< 速度（米/秒）
    double accelX = 0.0;   ///< X方向加速度（可选）
    double accelY = 0.0;   ///< Y方向加速度（可选）
    double accelZ = 0.0;   ///< Z方向加速度（可选）
    double gyroX = 0.0;    ///< X方向角速度（可选）
    double gyroY = 0.0;    ///< Y方向角速度（可选）
    double gyroZ = 0.0;    ///< Z方向角速度（可选）
    double magX = 0.0;     ///< X方向磁场强度（可选）
    double magY = 0.0;     ///< Y方向磁场强度（可选）
    double magZ = 0.0;     ///< Z方向磁场强度（可选）
    double temperature = 0.0; ///< 温度（可选）
    bool hasIMU = false;   ///< 是否有IMU数据
    double timestamp = 0.0; ///< 时间戳（秒）
};

/**
 * @brief 本地坐标点结构体，用于滑动窗口
 */
struct LocalPoint {
    double x = 0.0;         ///< OA本地X坐标（米）
    double y = 0.0;         ///< OA本地Y坐标（米）
    double timestamp = 0.0; ///< 时间戳（秒）
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
    StateEstimator();
    ~StateEstimator();

    void setCoordinate(Coordinate* coordinate);
    void setUseKalman(bool useKalman);
    void setLowPassAlpha(double alpha);
    void setTriggerPlan(const BlockPlanResult& plan);

    void update(const SensorData& data);
    EstimatedPose getPose() const;
    bool commitPredictedTrigger();

    void reset();
    void calibrate();

private:
    void updateLowPass(const SensorData& data, double x, double y);
    void updateKalman(const SensorData& data, double x, double y);
    std::pair<double, double> calculateVelocity(const SensorData& data);
    void estimateVelocity(double& vx, double& vy);
    void updateWindowStats(double regressionVx, double regressionVy);
    void checkCrossing();
    void updateTriggerPoseFields();
    void clearTriggerPrediction();
    bool hasActiveTrigger() const;
    const TriggerLineSpec* currentTriggerLine() const;
    int inferTravelDirection(const LocalPoint* previousPoint = nullptr,
                             const LocalPoint* currentPoint = nullptr) const;
    int findNextTriggerIndex(double currentY, int direction) const;
    void syncTriggerTraversal(double currentY, int direction);
    void advancePastCurrentPosition(double currentY, int direction, bool logSkip);
    bool interpolateCrossing(const LocalPoint& p1,
                             const LocalPoint& p2,
                             double targetY,
                             double& triggerX,
                             qint64& triggerTimeUs) const;
    void commitTrigger(double triggerX, double triggerY, qint64 triggerTimeUs, const char* source);

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

    std::vector<TriggerLineSpec> m_triggerLines; ///< 触发线计划
    int m_nextTriggerVectorIndex; ///< 下一条待触发线在triggerLines中的下标
    double m_triggerStopY; ///< 停止触发判定Y坐标
    bool m_inFineMode; ///< 是否处于细估计模式
    double m_lastCoarseEvalTimestamp; ///< 上次粗估计时间
    int m_lastTravelDirection; ///< 最近一次推断的行进方向（-1/0/+1）

    double m_averageSpeed; ///< 最近窗口平均速度（米/秒）
    double m_averageDirectionDeg; ///< 最近窗口平均速度方向（度）
    double m_averagePointSpacing; ///< 最近窗口平均点间距（米）
    double m_windowVx; ///< 窗口回归X速度（米/秒）
    double m_windowVy; ///< 窗口回归Y速度（米/秒）

    bool m_hasPrediction; ///< 是否存在待提交预测触发
    double m_predictedTriggerX; ///< 预测触发点X坐标
    double m_predictedTriggerY; ///< 预测触发点Y坐标
    qint64 m_predictedTriggerTimeUs; ///< 预测触发时间戳
    double m_predictedCountdownSec; ///< 预测倒计时（秒）
    int m_predictedTriggerVectorIndex; ///< 预测对应的触发线下标

    bool m_hasLastTriggerEvent; ///< 是否已有最近一次触发事件
    double m_lastTriggerX; ///< 最近一次触发点X坐标
    double m_lastTriggerY; ///< 最近一次触发点Y坐标
    qint64 m_lastTriggerTimeUs; ///< 最近一次触发时间戳
    int m_triggerSequence; ///< 已触发序号

    double m_deadzoneM; ///< 越线后的死区（米）
};

#endif // STATEESTIMATOR_H
