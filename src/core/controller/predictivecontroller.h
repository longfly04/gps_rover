#ifndef PREDICTIVECONTROLLER_H
#define PREDICTIVECONTROLLER_H

#include <QPointF>
#include "pathgenerator.h"

/**
 * @brief 状态结构体，包含位置、速度和航向信息
 */
typedef struct {
    double x;      ///< X坐标
    double y;      ///< Y坐标
    double vx;     ///< X方向速度
    double vy;     ///< Y方向速度
    double heading; ///< 航向角（度）
    double magneticHeading; ///< 磁方位角（度）
} State;

/**
 * @brief 预测控制器类，实现基于运动预测的前馈-反馈混合触发机制
 */
class PredictiveController
{
public:
    /**
     * @brief 构造函数
     */
    PredictiveController();
    
    /**
     * @brief 析构函数
     */
    ~PredictiveController();
    
    /**
     * @brief 设置路径生成器
     * @param pathGenerator 路径生成器指针
     */
    void setPathGenerator(PathGenerator* pathGenerator);
    
    /**
     * @brief 设置系统延迟
     * @param delay 系统延迟（秒），默认0.3秒
     */
    void setSystemDelay(double delay);
    
    /**
     * @brief 设置迟滞阈值
     * @param hysteresis 迟滞阈值（米），默认0.02米
     */
    void setHysteresis(double hysteresis);
    
    /**
     * @brief 设置预测阈值
     * @param epsilon 预测阈值（米），默认0.01米
     */
    void setEpsilon(double epsilon);
    
    /**
     * @brief 更新控制器状态
     * @param state 当前状态
     */
    void update(const State& state);
    
    /**
     * @brief 启动控制器
     */
    void start();
    
    /**
     * @brief 停止控制器
     */
    void stop();
    
    /**
     * @brief 检查是否正在播种
     * @return 是否正在播种
     */
    bool isSeeding() const;
    
    /**
     * @brief 获取预测位置
     * @return 预测位置
     */
    QPointF getPredictedPosition() const;
    
    /**
     * @brief 获取当前状态
     * @return 当前状态
     */
    State getCurrentState() const;

private:
    /**
     * @brief 预测未来位置
     * @param state 当前状态
     * @return 预测位置
     */
    QPointF predictFuturePosition(const State& state) const;
    
    /**
     * @brief 检查是否需要触发播种启停
     * @param predictedPos 预测位置
     */
    void checkBoundaryCrossing(const QPointF& predictedPos);
    
    /**
     * @brief 触发播种开始
     */
    void startSeeding();
    
    /**
     * @brief 触发播种停止
     */
    void stopSeeding();
    
    PathGenerator* pathGenerator; ///< 路径生成器指针
    double systemDelay; ///< 系统延迟（秒）
    double hysteresis; ///< 迟滞阈值（米）
    double epsilon; ///< 预测阈值（米）
    bool isRunning; ///< 是否正在运行
    bool seedingState; ///< 播种状态
    State currentState; ///< 当前状态
    QPointF predictedPosition; ///< 预测位置
};

#endif // PREDICTIVECONTROLLER_H