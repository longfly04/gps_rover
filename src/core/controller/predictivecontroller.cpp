#include "predictivecontroller.h"
#include <QPointF>
#include <QDebug>
#include <stdexcept>

/**
 * @brief 构造函数，初始化默认参数
 */
PredictiveController::PredictiveController() 
    : pathGenerator(nullptr),
      systemDelay(0.3),
      hysteresis(0.02),
      epsilon(0.01),
      isRunning(false),
      seedingState(false)
{
    // 初始化当前状态
    currentState.x = 0.0;
    currentState.y = 0.0;
    currentState.vx = 0.0;
    currentState.vy = 0.0;
    currentState.heading = 0.0;
    
    // 初始化预测位置
    predictedPosition = QPointF(0.0, 0.0);
}

/**
 * @brief 析构函数
 */
PredictiveController::~PredictiveController()
{
}

/**
 * @brief 设置路径生成器
 * @param pathGenerator 路径生成器指针
 */
void PredictiveController::setPathGenerator(PathGenerator* pathGenerator)
{
    this->pathGenerator = pathGenerator;
}

/**
 * @brief 设置系统延迟
 * @param delay 系统延迟（秒），默认0.3秒
 */
void PredictiveController::setSystemDelay(double delay)
{
    this->systemDelay = std::max(0.0, delay);
}

/**
 * @brief 设置迟滞阈值
 * @param hysteresis 迟滞阈值（米），默认0.02米
 */
void PredictiveController::setHysteresis(double hysteresis)
{
    this->hysteresis = std::max(0.0, hysteresis);
}

/**
 * @brief 设置预测阈值
 * @param epsilon 预测阈值（米），默认0.01米
 */
void PredictiveController::setEpsilon(double epsilon)
{
    this->epsilon = std::max(0.0, epsilon);
}

/**
 * @brief 预测未来位置
 * @param state 当前状态
 * @return 预测位置
 */
QPointF PredictiveController::predictFuturePosition(const State& state) const
{
    // 计算未来位置：当前位置 + 速度 * 系统延迟
    double predictedX = state.x + state.vx * systemDelay;
    double predictedY = state.y + state.vy * systemDelay;
    
    return QPointF(predictedX, predictedY);
}

/**
 * @brief 触发播种开始
 */
void PredictiveController::startSeeding()
{
    if (!seedingState) {
        seedingState = true;
        // 这里可以添加实际的播种启动代码
        // 例如：发送控制指令到播种机
    }
}

/**
 * @brief 触发播种停止
 */
void PredictiveController::stopSeeding()
{
    if (seedingState) {
        seedingState = false;
        // 这里可以添加实际的播种停止代码
        // 例如：发送控制指令到播种机
    }
}

/**
 * @brief 检查是否需要触发播种启停
 * @param predictedPos 预测位置
 */
void PredictiveController::checkBoundaryCrossing(const QPointF& predictedPos)
{
    if (!pathGenerator) {
        return;
    }
    
    double y = predictedPos.y();
    
    // 检查预测位置是否在播种区
    bool shouldBeSeeding = pathGenerator->isSeedingAtY(y);
    
    // 使用迟滞阈值避免在边界附近反复切换
    if (shouldBeSeeding && !seedingState) {
        // 检查是否超过启动阈值
        startSeeding();
    } else if (!shouldBeSeeding && seedingState) {
        // 检查是否超过停止阈值
        stopSeeding();
    }
}

/**
 * @brief 更新控制器状态
 * @param state 当前状态
 */
void PredictiveController::update(const State& state)
{
    try {
        if (!isRunning) {
            return;
        }
        
        if (!pathGenerator) {
            throw std::runtime_error("Path generator not set");
        }
        
        // 更新当前状态
        currentState = state;
        
        // 预测未来位置
        predictedPosition = predictFuturePosition(state);
        
        // 检查边界穿越
        checkBoundaryCrossing(predictedPosition);
        
        // 定期记录状态（每10次更新记录一次）
        static int counter = 0;
        if (++counter % 10 == 0) {
            qDebug() << "Controller update - Position:" << state.x << state.y;
            qDebug() << "Controller update - Predicted position:" << predictedPosition;
            qDebug() << "Controller update - Seeding state:" << seedingState;
        }
    } catch (const std::exception& e) {
        qDebug() << "Error updating controller:" << e.what();
        // 保持当前状态，不进行更新
    }
}

/**
 * @brief 启动控制器
 */
void PredictiveController::start()
{
    isRunning = true;
    // 初始状态设为停止
    seedingState = false;
}

/**
 * @brief 停止控制器
 */
void PredictiveController::stop()
{
    isRunning = false;
    // 停止播种
    seedingState = false;
}

/**
 * @brief 检查是否正在播种
 * @return 是否正在播种
 */
bool PredictiveController::isSeeding() const
{
    return seedingState;
}

/**
 * @brief 获取预测位置
 * @return 预测位置
 */
QPointF PredictiveController::getPredictedPosition() const
{
    return predictedPosition;
}

/**
 * @brief 获取当前状态
 * @return 当前状态
 */
State PredictiveController::getCurrentState() const
{
    return currentState;
}