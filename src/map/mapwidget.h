#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QElapsedTimer>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPoint>
#include <QPointF>
#include <QRectF>
#include <QString>
#include <tuple>
#include <QVector>
#include <QWheelEvent>
#include <QWidget>

class QVariantAnimation;

#include "blockgenerator.h"
#include "coordinate.h"
#include "osmtilelayer.h"
#include "statestimator.h"

/**
 * @brief MapWidget类作为地图最上层抽象
 *
 * 功能包括：
 * 1. 统一管理显示、交互、缩放和平移
 * 2. 调用Coordinate获取坐标映射
 * 3. 调用BlockGenerator结果绘制块、过道、边界、控制点
 * 4. 绘制实时设备位置与离散轨迹
 * 5. 管理地图缩放、平移、选中状态
 */
class MapWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父窗口
     */
    explicit MapWidget(QWidget *parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~MapWidget();

    /**
     * @brief 设置坐标转换器
     * @param coordinate 坐标转换器指针
     */
    void setCoordinate(Coordinate* coordinate);

    /**
     * @brief 设置小区规划结果
     * @param result 小区规划结果
     */
    void setBlockPlanResult(const BlockPlanResult& result);

    /**
     * @brief 高亮选中的小区
     * @param blockId 小区编号
     */
    void highlightBlock(int blockId);

    /**
     * @brief 更新设备位置
     * @param pose 估计的姿态
     */
    void updateVehiclePose(const EstimatedPose& pose);

    /**
     * @brief 设置运动轨迹是否可见
     * @param visible 是否显示历史轨迹
     */
    void setTrajectoryVisible(bool visible);

    /**
     * @brief 清除轨迹
     */
    void clearPath();

    /**
     * @brief 清除所有显示
     */
    void clearAll();

protected:
    /**
     * @brief 绘制事件
     * @param event 绘制事件
     */
    void paintEvent(QPaintEvent *event) override;

    /**
     * @brief 鼠标按下事件
     * @param event 鼠标事件
     */
    void mousePressEvent(QMouseEvent *event) override;

    /**
     * @brief 鼠标移动事件
     * @param event 鼠标事件
     */
    void mouseMoveEvent(QMouseEvent *event) override;

    /**
     * @brief 鼠标释放事件
     * @param event 鼠标事件
     */
    void mouseReleaseEvent(QMouseEvent *event) override;

    /**
     * @brief 鼠标滚轮事件
     * @param event 鼠标事件
     */
    void wheelEvent(QWheelEvent *event) override;

private:
    enum class OrientationMode {
        OAUp,
        NorthUp
    };

    /**
     * @brief 绘制OSM底图
     * @param painter 画笔
     */
    bool drawBaseMap(QPainter &painter);

    /**
     * @brief 绘制退化网格背景
     * @param painter 画笔
     */
    void drawFallbackGrid(QPainter &painter);

    /**
     * @brief 绘制本地坐标轴
     * @param painter 画笔
     */
    void drawLocalAxes(QPainter &painter);

    /**
     * @brief 绘制本地坐标标签气泡
     * @param painter 画笔
     * @param anchor 锚点
     * @param text 文本
     * @param offset 偏移
     */
    void drawLabelBubble(QPainter &painter, const QPointF &anchor, const QString &text,
                         const QPointF &offset = QPointF(8.0, -8.0)) const;

    /**
     * @brief 绘制触发线
     * @param painter 画笔
     */
    void drawTriggerLines(QPainter &painter);

    /**
     * @brief 绘制触发运行态
     * @param painter 画笔
     */
    void drawTriggerRuntime(QPainter &painter);

    /**
     * @brief 绘制小区和边界
     * @param painter 画笔
     */
    void drawBlocks(QPainter &painter);

    /**
     * @brief 绘制选中的小区边界
     * @param painter 画笔
     */
    void drawSelectedBoundary(QPainter &painter);

    /**
     * @brief 绘制离散轨迹点
     * @param painter 画笔
     */
    void drawTrajectory(QPainter &painter);

    /**
     * @brief 绘制设备位置
     * @param painter 画笔
     */
    void drawVehiclePose(QPainter &painter);

    /**
     * @brief 绘制指南针
     * @param painter 画笔
     */
    void drawCompass(QPainter &painter);

    /**
     * @brief 绘制右下角归位按钮
     * @param painter 画笔
     */
    void drawRecenterButton(QPainter &painter);

    /**
     * @brief 绘制左下角比例尺
     * @param painter 画笔
     */
    void drawScaleBar(QPainter &painter);

    /**
     * @brief 获取当前显示旋转角（弧度）
     * @return 旋转角
     */
    double displayRotationRadians() const;

    /**
     * @brief 绘制底图署名
     * @param painter 画笔
     */
    void drawAttribution(QPainter &painter);

    /**
     * @brief 将世界坐标转换为屏幕坐标
     * @param worldX 世界X坐标（米）
     * @param worldY 世界Y坐标（米）
     * @return 屏幕坐标
     */
    QPointF worldToScreen(double worldX, double worldY) const;

    /**
     * @brief 将屏幕坐标转换为世界坐标
     * @param screenPos 屏幕坐标
     * @param worldX 输出世界X坐标（米）
     * @param worldY 输出世界Y坐标（米）
     */
    void screenToWorld(const QPointF &screenPos, double &worldX, double &worldY) const;

    /**
     * @brief 将经纬度转换为屏幕坐标
     * @param lat 纬度
     * @param lon 经度
     * @return 屏幕坐标
     */
    QPointF geoToScreen(double lat, double lon) const;

    /**
     * @brief 将经纬度转换为当前地图世界坐标
     * @param lat 纬度
     * @param lon 经度
     * @return 世界坐标
     */
    QPointF geographicToWorld(double lat, double lon) const;

    /**
     * @brief 将当前地图世界坐标转换为经纬度
     * @param worldX 世界X坐标
     * @param worldY 世界Y坐标
     * @return WGS84坐标点（纬度，经度）
     */
    QPointF worldToGeographic(double worldX, double worldY) const;

    /**
     * @brief 将本地坐标转换为当前地图世界坐标
     * @param localX 本地X坐标
     * @param localY 本地Y坐标
     * @return 世界坐标
     */
    QPointF localToWorld(double localX, double localY) const;

    /**
     * @brief 获取指南针中心点
     * @return 屏幕坐标中心
     */
    QPointF compassCenter() const;

    /**
     * @brief 获取归位按钮区域
     * @return 按钮矩形
     */
    QRectF recenterButtonRect() const;

    /**
     * @brief 判断点是否命中指南针
     * @param point 屏幕坐标
     * @return 是否命中
     */
    bool isPointInCompass(const QPointF &point) const;

    /**
     * @brief 判断点是否命中归位按钮
     * @param point 屏幕坐标
     * @return 是否命中
     */
    bool isPointInRecenterButton(const QPointF &point) const;

    /**
     * @brief 计算比例尺显示长度
     * @return 比例尺信息（像素长度，对应米数，标签）
     */
    std::tuple<double, double, QString> currentScaleBarSpec() const;

    /**
     * @brief 获取当前OA方向对应的目标显示旋转角
     * @return 旋转角（弧度）
     */
    double oaUpRotationRadians() const;

    /**
     * @brief 获取当前模式对应的目标显示旋转角
     * @return 旋转角（弧度）
     */
    double targetDisplayRotationRadians() const;

    /**
     * @brief 获取指南针指北箭头相对屏幕的旋转角
     * @return 旋转角（弧度）
     */
    double compassNorthRotationRadians() const;

    /**
     * @brief 将角度归一化到[-pi, pi]
     * @param angle 待归一化角度
     * @return 归一化后的角度
     */
    static double normalizeAngleRadians(double angle);

    /**
     * @brief 启动显示旋转动画
     */
    void animateToOrientationMode(OrientationMode mode);

    /**
     * @brief 从姿态中解析可用的地理坐标
     * @param pose 设备姿态
     * @param geographicPos 输出WGS84坐标（纬度，经度）
     * @return 是否解析成功
     */
    bool resolvePoseGeographicPosition(const EstimatedPose& pose, QPointF &geographicPos) const;

    /**
     * @brief 判断当前是否具备地理上下文以绘制底图
     * @return 是否具备地理上下文
     */
    bool hasGeographicContext() const;

    /**
     * @brief 确保地图已经建立地理上下文
     * @param lat 纬度
     * @param lon 经度
     */
    void ensureGeographicContext(double lat, double lon);

    /**
     * @brief 将视图中心快速归位到本地坐标系O点
     */
    void recenterToLocalOrigin();

private:
    Coordinate* m_coordinate; ///< 坐标转换器（外部管理）
    BlockPlanResult m_blockResult; ///< 小区规划结果

    int m_selectedBlockId; ///< 选中的小区编号

    double m_worldCenterX; ///< 世界坐标中心X（米）
    double m_worldCenterY; ///< 世界坐标中心Y（米）
    double m_zoomFactor; ///< 统一缩放因子（像素/米）
    OrientationMode m_orientationMode; ///< 当前显示朝向模式
    double m_displayRotationRadians; ///< 当前显示旋转角（弧度）
    QVariantAnimation* m_rotationAnimation; ///< 显示旋转动画

    EstimatedPose m_currentPose; ///< 当前设备姿态
    QVector<QPointF> m_trajectoryPoints; ///< 轨迹点（世界坐标）
    QVector<QPointF> m_triggerHistoryPoints; ///< 已触发点历史（世界坐标）
    bool m_trajectoryVisible; ///< 是否显示历史运动轨迹
    double m_lastPoseTimestamp; ///< 上次已记录的位姿时间戳
    int m_lastTriggerSequence; ///< 上次已同步的触发序号

    bool m_isDragging; ///< 是否正在拖动
    bool m_compassPressed; ///< 是否按下指南针
    bool m_recenterPressed; ///< 是否按下归位按钮
    QPoint m_lastMousePos; ///< 上次鼠标位置

    QElapsedTimer m_updateTimer; ///< 更新计时器
    bool m_updatePending; ///< 是否有待处理的更新
    bool m_timerInitialized; ///< 计时器是否已初始化

    bool m_hasGeographicContext; ///< 是否已经建立地图地理上下文（首次有效地理定位后）
    bool m_basemapEnabled; ///< 是否启用底图
    OsmTileLayer* m_osmTileLayer; ///< OSM底图图层
};

#endif // MAPWIDGET_H
