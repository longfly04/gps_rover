#include "mapwidget.h"

#include <QFont>
#include <QPainter>
#include <QPaintEvent>
#include <QPainterPath>
#include <QPen>
#include <QPolygonF>
#include <QStringList>
#include <QTimer>
#include <QVariantAnimation>
#include <QWheelEvent>

#include <algorithm>
#include <array>
#include <cmath>

#include "osmtilelayer.h"
#include "../core/common/logger.h"

namespace {
constexpr int kMaxTrajectoryPoints = 10000;
constexpr int kPaintIntervalMs = 100;
constexpr double kDefaultZoomFactor = 2.0;
constexpr double kMinZoomFactor = 0.05;
constexpr double kMaxZoomFactor = 5000.0;
constexpr double kGridWorldStep = 10.0;
constexpr int kCompassMargin = 16;
constexpr int kCompassRadius = 28;
constexpr int kRecenterButtonSize = 36;
constexpr int kRecenterButtonGap = 12;
constexpr int kScaleBarMargin = 16;
constexpr int kScaleBarMaxWidthPx = 120;
constexpr int kScaleBarMinWidthPx = 60;
constexpr int kRotationAnimationDurationMs = 220;
constexpr double kPi = 3.14159265358979323846;

bool isValidLatitudeLongitude(double lat, double lon)
{
    return std::isfinite(lat) && std::isfinite(lon)
        && std::abs(lat) <= 90.0 && std::abs(lon) <= 180.0
        && !(std::abs(lat) < 1e-12 && std::abs(lon) < 1e-12);
}

QPointF rotateWorldOffset(double dx, double dy, double rotationRadians)
{
    if (std::abs(rotationRadians) < 1e-12) {
        return QPointF(dx, dy);
    }

    const double cosTheta = std::cos(rotationRadians);
    const double sinTheta = std::sin(rotationRadians);
    return QPointF(dx * cosTheta - dy * sinTheta,
                   dx * sinTheta + dy * cosTheta);
}
}

/**
 * @brief 构造函数
 */
MapWidget::MapWidget(QWidget *parent)
    : QWidget(parent)
    , m_coordinate(nullptr)
    , m_selectedBlockId(0)
    , m_worldCenterX(0.0)
    , m_worldCenterY(0.0)
    , m_zoomFactor(kDefaultZoomFactor)
    , m_orientationMode(OrientationMode::OAUp)
    , m_displayRotationRadians(0.0)
    , m_rotationAnimation(new QVariantAnimation(this))
    , m_currentPose()
    , m_lastPoseTimestamp(-1.0)
    , m_isDragging(false)
    , m_compassPressed(false)
    , m_recenterPressed(false)
    , m_lastMousePos(0, 0)
    , m_updatePending(false)
    , m_timerInitialized(false)
    , m_hasGeographicContext(false)
    , m_basemapEnabled(true)
    , m_osmTileLayer(new OsmTileLayer(this))
{
    setMinimumSize(400, 400);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);

    connect(m_osmTileLayer, &OsmTileLayer::repaintRequested, this, [this]() {
        update();
    });

    m_rotationAnimation->setDuration(kRotationAnimationDurationMs);
    m_rotationAnimation->setEasingCurve(QEasingCurve::InOutCubic);
    connect(m_rotationAnimation, &QVariantAnimation::valueChanged, this, [this](const QVariant &value) {
        m_displayRotationRadians = value.toDouble();
        update();
    });
    connect(m_rotationAnimation, &QVariantAnimation::finished, this, [this]() {
        m_displayRotationRadians = normalizeAngleRadians(m_displayRotationRadians);
        update();
    });
    m_displayRotationRadians = targetDisplayRotationRadians();
}

/**
 * @brief 析构函数
 */
MapWidget::~MapWidget() = default;

/**
 * @brief 设置坐标转换器
 */
void MapWidget::setCoordinate(Coordinate* coordinate)
{
    m_coordinate = coordinate;
    if (m_coordinate && m_coordinate->isInitialized() && !m_hasGeographicContext) {
        ensureGeographicContext(m_coordinate->getOriginLatitude(), m_coordinate->getOriginLongitude());
    }

    if (!m_coordinate || !m_coordinate->isInitialized()) {
        m_orientationMode = OrientationMode::OAUp;
    }

    m_rotationAnimation->stop();
    m_displayRotationRadians = targetDisplayRotationRadians();
    update();
    LOG_INFO("Coordinate converter set for MapWidget");
}

/**
 * @brief 设置小区规划结果
 */
void MapWidget::setBlockPlanResult(const BlockPlanResult& result)
{
    m_blockResult = result;
    update();
}

/**
 * @brief 高亮选中的小区
 */
void MapWidget::highlightBlock(int blockId)
{
    m_selectedBlockId = blockId;
    update();
}

/**
 * @brief 更新设备位置
 */
void MapWidget::updateVehiclePose(const EstimatedPose& pose)
{
    m_currentPose = pose;

    QPointF geographicPos;
    const bool hasGeographicPose = resolvePoseGeographicPosition(pose, geographicPos);
    if (hasGeographicPose) {
        ensureGeographicContext(geographicPos.x(), geographicPos.y());
    }

    const QPointF worldPos = hasGeographicPose
        ? geographicToWorld(geographicPos.x(), geographicPos.y())
        : QPointF(pose.x, pose.y);

    if (m_trajectoryPoints.isEmpty()) {
        m_worldCenterX = worldPos.x();
        m_worldCenterY = worldPos.y();
    }

    if (m_lastPoseTimestamp < 0.0 || std::abs(pose.timestamp - m_lastPoseTimestamp) > 1e-6) {
        m_trajectoryPoints.append(worldPos);
        if (m_trajectoryPoints.size() > kMaxTrajectoryPoints) {
            m_trajectoryPoints.remove(0, m_trajectoryPoints.size() - kMaxTrajectoryPoints);
        }
        m_lastPoseTimestamp = pose.timestamp;
    } else if (!m_trajectoryPoints.isEmpty()) {
        m_trajectoryPoints.back() = worldPos;
    }

    if (!m_timerInitialized) {
        m_updateTimer.start();
        m_timerInitialized = true;
    }

    if (m_updateTimer.elapsed() >= kPaintIntervalMs) {
        update();
        m_updateTimer.restart();
        m_updatePending = false;
    } else if (!m_updatePending) {
        m_updatePending = true;
        const int remainingTime = std::max(0, kPaintIntervalMs - static_cast<int>(m_updateTimer.elapsed()));
        QTimer::singleShot(remainingTime, this, [this]() {
            update();
            m_updateTimer.restart();
            m_updatePending = false;
        });
    }
}

/**
 * @brief 清除轨迹
 */
void MapWidget::clearPath()
{
    m_trajectoryPoints.clear();
    m_lastPoseTimestamp = -1.0;
    update();
}

/**
 * @brief 清除所有显示
 */
void MapWidget::clearAll()
{
    m_trajectoryPoints.clear();
    m_selectedBlockId = 0;
    m_currentPose = EstimatedPose();
    m_lastPoseTimestamp = -1.0;
    m_hasGeographicContext = false;
    m_compassPressed = false;
    m_recenterPressed = false;
    m_isDragging = false;
    m_orientationMode = OrientationMode::OAUp;
    m_rotationAnimation->stop();
    m_displayRotationRadians = targetDisplayRotationRadians();
    update();
}

/**
 * @brief 绘制事件
 */
void MapWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.fillRect(rect(), Qt::white);

    const bool hasBaseMap = drawBaseMap(painter);
    if (!hasBaseMap || !m_basemapEnabled) {
        drawFallbackGrid(painter);
    }
    drawLocalAxes(painter);
    drawBlocks(painter);
    drawSelectedBoundary(painter);
    drawTrajectory(painter);
    drawVehiclePose(painter);
    drawCompass(painter);
    drawRecenterButton(painter);
    drawScaleBar(painter);
    drawAttribution(painter);
}

/**
 * @brief 鼠标按下事件
 */
void MapWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && isPointInCompass(event->position())) {
        m_compassPressed = true;
        event->accept();
        return;
    }

    if (event->button() == Qt::LeftButton && isPointInRecenterButton(event->position())) {
        m_recenterPressed = true;
        event->accept();
        return;
    }

    if (event->button() == Qt::LeftButton) {
        m_isDragging = true;
        m_lastMousePos = event->pos();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
        return;
    }

    QWidget::mousePressEvent(event);
}

/**
 * @brief 鼠标移动事件
 */
void MapWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_recenterPressed) {
        event->accept();
        return;
    }

    if (m_isDragging && m_zoomFactor > 0.0) {
        const QPointF deltaScreen = event->position() - QPointF(m_lastMousePos);
        const QPointF deltaWorld = rotateWorldOffset(-deltaScreen.x() / m_zoomFactor,
                                                     deltaScreen.y() / m_zoomFactor,
                                                     -displayRotationRadians());
        m_worldCenterX += deltaWorld.x();
        m_worldCenterY += deltaWorld.y();
        m_lastMousePos = event->pos();
        update();
        event->accept();
        return;
    }

    QWidget::mouseMoveEvent(event);
}

/**
 * @brief 鼠标释放事件
 */
void MapWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_compassPressed) {
        m_compassPressed = false;
        if (isPointInCompass(event->position())) {
            const OrientationMode nextMode = m_orientationMode == OrientationMode::OAUp
                ? OrientationMode::NorthUp
                : OrientationMode::OAUp;
            animateToOrientationMode(nextMode);
        }
        event->accept();
        return;
    }

    if (event->button() == Qt::LeftButton && m_recenterPressed) {
        m_recenterPressed = false;
        if (isPointInRecenterButton(event->position())) {
            recenterToLocalOrigin();
        }
        event->accept();
        return;
    }

    if (event->button() == Qt::LeftButton) {
        m_isDragging = false;
        setCursor(Qt::ArrowCursor);
        event->accept();
        return;
    }

    QWidget::mouseReleaseEvent(event);
}

/**
 * @brief 鼠标滚轮事件
 */
void MapWidget::wheelEvent(QWheelEvent *event)
{
    double worldXBefore = 0.0;
    double worldYBefore = 0.0;
    screenToWorld(event->position(), worldXBefore, worldYBefore);

    const double factor = event->angleDelta().y() > 0 ? 1.1 : 0.9;
    m_zoomFactor = std::clamp(m_zoomFactor * factor, kMinZoomFactor, kMaxZoomFactor);

    double worldXAfter = 0.0;
    double worldYAfter = 0.0;
    screenToWorld(event->position(), worldXAfter, worldYAfter);
    m_worldCenterX += worldXBefore - worldXAfter;
    m_worldCenterY += worldYBefore - worldYAfter;

    update();
}

/**
 * @brief 绘制OSM底图
 */
bool MapWidget::drawBaseMap(QPainter &painter)
{
    if (!m_basemapEnabled || !m_osmTileLayer || !hasGeographicContext()) {
        return false;
    }

    const OsmTileLayer::ViewState viewState{
        size(),
        m_worldCenterX,
        m_worldCenterY,
        m_zoomFactor,
        displayRotationRadians()
    };
    return m_osmTileLayer->draw(painter, viewState);
}

/**
 * @brief 绘制退化网格背景
 */
void MapWidget::drawFallbackGrid(QPainter &painter)
{
    painter.setPen(QPen(QColor(220, 220, 220, m_basemapEnabled ? 120 : 255), 1));

    const double leftWorld = m_worldCenterX - width() / (2.0 * m_zoomFactor);
    const double rightWorld = m_worldCenterX + width() / (2.0 * m_zoomFactor);
    const double bottomWorld = m_worldCenterY - height() / (2.0 * m_zoomFactor);
    const double topWorld = m_worldCenterY + height() / (2.0 * m_zoomFactor);

    const double firstX = std::floor(leftWorld / kGridWorldStep) * kGridWorldStep;
    const double firstY = std::floor(bottomWorld / kGridWorldStep) * kGridWorldStep;

    for (double x = firstX; x <= rightWorld; x += kGridWorldStep) {
        const QPointF p1 = worldToScreen(x, bottomWorld);
        const QPointF p2 = worldToScreen(x, topWorld);
        painter.drawLine(p1, p2);
    }

    for (double y = firstY; y <= topWorld; y += kGridWorldStep) {
        const QPointF p1 = worldToScreen(leftWorld, y);
        const QPointF p2 = worldToScreen(rightWorld, y);
        painter.drawLine(p1, p2);
    }
}

/**
 * @brief 绘制本地坐标轴
 */
void MapWidget::drawLocalAxes(QPainter &painter)
{
    if (!m_coordinate || !m_coordinate->isInitialized() || !m_blockResult.isValid) {
        return;
    }

    const auto drawLabelBubble = [&painter](const QPointF &anchor,
                                            const QString &text,
                                            const QPointF &offset = QPointF(8.0, -8.0)) {
        painter.save();
        const QFontMetrics metrics(painter.font());
        const int paddingX = 6;
        const int paddingY = 3;
        const QSize textSize(metrics.horizontalAdvance(text), metrics.height());
        const QRectF bubbleRect(anchor.x() + offset.x(),
                                anchor.y() + offset.y() - textSize.height(),
                                textSize.width() + paddingX * 2,
                                textSize.height() + paddingY * 2);
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(255, 255, 255, 170));
        painter.drawRoundedRect(bubbleRect, 4, 4);
        painter.setPen(QColor(35, 35, 35, 230));
        painter.drawText(bubbleRect.adjusted(paddingX, paddingY, -paddingX, -paddingY),
                         Qt::AlignLeft | Qt::AlignVCenter,
                         text);
        painter.restore();
    };

    const double xMax = std::max(0.0, m_blockResult.totalWidth);
    const double yMax = std::max(0.0, m_blockResult.totalLength);
    const QPointF originWorld = localToWorld(0.0, 0.0);
    const QPointF xAxisWorld = localToWorld(xMax, 0.0);
    const QPointF yAxisWorld = localToWorld(0.0, yMax);
    const QPointF originScreen = worldToScreen(originWorld.x(), originWorld.y());
    const QPointF xAxisEnd = worldToScreen(xAxisWorld.x(), xAxisWorld.y());
    const QPointF yAxisEnd = worldToScreen(yAxisWorld.x(), yAxisWorld.y());
    const QColor axisColor(0, 102, 204);

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(QColor(255, 255, 255, 160), 5, Qt::SolidLine, Qt::RoundCap));
    painter.drawLine(originScreen, xAxisEnd);
    painter.drawLine(originScreen, yAxisEnd);
    painter.setPen(QPen(axisColor, 2.2, Qt::SolidLine, Qt::RoundCap));
    painter.drawLine(originScreen, xAxisEnd);
    painter.drawLine(originScreen, yAxisEnd);

    painter.setPen(QPen(QColor(255, 255, 255, 180), 3, Qt::SolidLine, Qt::RoundCap));
    painter.setFont(QFont("Arial", 8));

    const int tickCount = 5;
    for (int i = 1; i <= tickCount; ++i) {
        const double yValue = yMax * static_cast<double>(i) / tickCount;
        const QPointF tickWorld = localToWorld(0.0, yValue);
        const QPointF tickPoint = worldToScreen(tickWorld.x(), tickWorld.y());
        painter.drawLine(QPointF(tickPoint.x() - 4, tickPoint.y()), QPointF(tickPoint.x() + 4, tickPoint.y()));
        painter.setPen(QPen(QColor(35, 35, 35, 220), 1.2));
        painter.drawLine(QPointF(tickPoint.x() - 4, tickPoint.y()), QPointF(tickPoint.x() + 4, tickPoint.y()));
        drawLabelBubble(tickPoint, QString::number(yValue, 'f', 4) + " m", QPointF(8.0, 6.0));
        painter.setPen(QPen(QColor(255, 255, 255, 180), 3, Qt::SolidLine, Qt::RoundCap));
    }

    for (int i = 1; i <= tickCount; ++i) {
        const double xValue = xMax * static_cast<double>(i) / tickCount;
        const QPointF tickWorld = localToWorld(xValue, 0.0);
        const QPointF tickPoint = worldToScreen(tickWorld.x(), tickWorld.y());
        painter.drawLine(QPointF(tickPoint.x(), tickPoint.y() - 4), QPointF(tickPoint.x(), tickPoint.y() + 4));
        painter.setPen(QPen(QColor(35, 35, 35, 220), 1.2));
        painter.drawLine(QPointF(tickPoint.x(), tickPoint.y() - 4), QPointF(tickPoint.x(), tickPoint.y() + 4));
        drawLabelBubble(tickPoint, QString::number(xValue, 'f', 4) + " m", QPointF(6.0, -6.0));
        painter.setPen(QPen(QColor(255, 255, 255, 180), 3, Qt::SolidLine, Qt::RoundCap));
    }

    painter.setPen(QPen(Qt::white, 3));
    painter.setBrush(axisColor);
    painter.drawEllipse(originScreen, 5, 5);
    painter.restore();

    painter.setFont(QFont("Arial", 8));
    drawLabelBubble(originScreen, QStringLiteral("O(0.0000, 0.0000)"));
}

/**
 * @brief 绘制小区和边界
 */
void MapWidget::drawBlocks(QPainter &painter)
{
    if (!m_coordinate || !m_coordinate->isInitialized() || !m_blockResult.isValid) {
        return;
    }

    painter.setBrush(Qt::NoBrush);

    auto toScreen = [this](double localX, double localY) {
        const QPointF worldPoint = localToWorld(localX, localY);
        return worldToScreen(worldPoint.x(), worldPoint.y());
    };

    for (const auto& block : m_blockResult.blocks) {
        QPolygonF blockPolygon;
        blockPolygon << toScreen(0.0, block.yStart)
                     << toScreen(block.width, block.yStart)
                     << toScreen(block.width, block.yEnd)
                     << toScreen(0.0, block.yEnd);
        painter.setPen(QPen(QColor(52, 102, 204), 1));
        painter.drawPolygon(blockPolygon);
    }

    for (const auto& boundary : m_blockResult.boundaries) {
        QPolygonF forwardPolygon;
        for (const auto& vertex : boundary.forwardBoundary) {
            forwardPolygon << toScreen(vertex.x, vertex.y);
        }

        QPolygonF reversePolygon;
        for (const auto& vertex : boundary.reverseBoundary) {
            reversePolygon << toScreen(vertex.x, vertex.y);
        }

        if (boundary.blockId == m_selectedBlockId) {
            painter.setPen(QPen(QColor(255, 170, 0), 2, Qt::DashLine));
        } else {
            painter.setPen(QPen(QColor(130, 130, 130), 1, Qt::DashLine));
        }

        if (!forwardPolygon.isEmpty()) {
            painter.drawPolyline(forwardPolygon);
        }
        if (!reversePolygon.isEmpty()) {
            painter.drawPolyline(reversePolygon);
        }
    }
}

/**
 * @brief 绘制选中的小区边界
 */
void MapWidget::drawSelectedBoundary(QPainter &painter)
{
    if (!m_coordinate || !m_coordinate->isInitialized() || m_selectedBlockId == 0 || !m_blockResult.isValid) {
        return;
    }

    auto toScreen = [this](double localX, double localY) {
        const QPointF worldPoint = localToWorld(localX, localY);
        return worldToScreen(worldPoint.x(), worldPoint.y());
    };

    for (const auto& boundary : m_blockResult.boundaries) {
        if (boundary.blockId != m_selectedBlockId) {
            continue;
        }

        QPolygonF polygon;
        for (const auto& vertex : boundary.forwardBoundary) {
            polygon << toScreen(vertex.x, vertex.y);
        }

        if (!polygon.isEmpty()) {
            painter.setPen(QPen(QColor(255, 196, 0), 3));
            painter.setBrush(QColor(255, 235, 59, 40));
            painter.drawPolygon(polygon);
        }
        return;
    }
}

/**
 * @brief 绘制离散轨迹点
 */
void MapWidget::drawTrajectory(QPainter &painter)
{
    if (m_trajectoryPoints.isEmpty()) {
        return;
    }

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);

    for (const QPointF& point : m_trajectoryPoints) {
        const QPointF screenPoint = worldToScreen(point.x(), point.y());
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(255, 255, 255, 95));
        painter.drawEllipse(screenPoint, 4, 4);
        painter.setBrush(QColor(220, 20, 60, 145));
        painter.drawEllipse(screenPoint, 2, 2);
    }

    painter.restore();
}

/**
 * @brief 绘制设备位置
 */
void MapWidget::drawVehiclePose(QPainter &painter)
{
    if (m_currentPose.timestamp <= 0.0 && m_trajectoryPoints.isEmpty()) {
        return;
    }

    QPointF geographicPos;
    const bool hasGeographicPose = resolvePoseGeographicPosition(m_currentPose, geographicPos);
    const QPointF worldPos = hasGeographicPose
        ? geographicToWorld(geographicPos.x(), geographicPos.y())
        : QPointF(m_currentPose.x, m_currentPose.y);
    const QPointF screenPos = worldToScreen(worldPos.x(), worldPos.y());

    painter.setPen(QPen(Qt::white, 3));
    painter.setBrush(QColor(220, 20, 60));
    painter.drawEllipse(screenPos, 7, 7);

    const bool usingLocalFrame = m_coordinate && m_coordinate->isInitialized();
    QStringList lines;
    if (hasGeographicPose) {
        lines << QString("WGS84: %1, %2")
                    .arg(geographicPos.x(), 0, 'f', 6)
                    .arg(geographicPos.y(), 0, 'f', 6);
    } else {
        lines << QStringLiteral("WGS84: pending GNSS fix");
    }

    if (usingLocalFrame) {
        lines << QString("Local: X=%1 m, Y=%2 m")
                    .arg(m_currentPose.x, 0, 'f', 4)
                    .arg(m_currentPose.y, 0, 'f', 4);
    } else {
        lines << QStringLiteral("Local: pending OA frame");
    }

    painter.setFont(QFont("Arial", 9, QFont::Bold));
    const int lineHeight = painter.fontMetrics().height();
    int maxWidth = 0;
    for (const QString& line : lines) {
        maxWidth = std::max(maxWidth, painter.fontMetrics().horizontalAdvance(line));
    }

    QRectF textRect(screenPos.x() + 12,
                    screenPos.y() - 10,
                    maxWidth + 12,
                    lineHeight * lines.size() + 10);

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(255, 255, 255, 220));
    painter.drawRoundedRect(textRect, 4, 4);

    painter.setPen(Qt::black);
    for (int i = 0; i < lines.size(); ++i) {
        painter.drawText(QPointF(textRect.left() + 6, textRect.top() + lineHeight * (i + 1)), lines[i]);
    }
}

QPointF MapWidget::compassCenter() const
{
    return QPointF(width() - kCompassMargin - kCompassRadius,
                   kCompassMargin + kCompassRadius);
}

QRectF MapWidget::recenterButtonRect() const
{
    const QPointF compass = compassCenter();
    const double x = compass.x() - kRecenterButtonSize / 2.0;
    const double y = compass.y() + kCompassRadius + kRecenterButtonGap;
    return QRectF(x,
                  y,
                  kRecenterButtonSize,
                  kRecenterButtonSize);
}

bool MapWidget::isPointInCompass(const QPointF &point) const
{
    const QPointF delta = point - compassCenter();
    const double hitRadius = static_cast<double>(kCompassRadius + 6);
    return delta.x() * delta.x() + delta.y() * delta.y() <= hitRadius * hitRadius;
}

bool MapWidget::isPointInRecenterButton(const QPointF &point) const
{
    return recenterButtonRect().contains(point);
}

std::tuple<double, double, QString> MapWidget::currentScaleBarSpec() const
{
    const double metersPerPixel = 1.0 / std::max(m_zoomFactor, 1e-9);
    const double targetMeters = metersPerPixel * kScaleBarMaxWidthPx;
    const double exponent = std::floor(std::log10(std::max(targetMeters, 1e-9)));
    const double magnitude = std::pow(10.0, exponent);
    const std::array<double, 3> preferredSteps{1.0, 2.0, 5.0};

    double chosenMeters = magnitude;
    for (double step : preferredSteps) {
        const double candidateMeters = step * magnitude;
        const double candidatePixels = candidateMeters / metersPerPixel;
        if (candidatePixels >= kScaleBarMinWidthPx && candidatePixels <= kScaleBarMaxWidthPx) {
            chosenMeters = candidateMeters;
            break;
        }
        if (candidatePixels <= kScaleBarMaxWidthPx) {
            chosenMeters = candidateMeters;
        }
    }

    const double pixelLength = chosenMeters / metersPerPixel;
    QString label;
    if (chosenMeters >= 1000.0) {
        const double km = chosenMeters / 1000.0;
        label = std::abs(km - std::round(km)) < 1e-6
            ? QString::number(static_cast<int>(std::round(km))) + QStringLiteral(" km")
            : QString::number(km, 'f', 1) + QStringLiteral(" km");
    } else {
        label = std::abs(chosenMeters - std::round(chosenMeters)) < 1e-6
            ? QString::number(static_cast<int>(std::round(chosenMeters))) + QStringLiteral(" m")
            : QString::number(chosenMeters, 'f', 1) + QStringLiteral(" m");
    }
    return {pixelLength, chosenMeters, label};
}

double MapWidget::oaUpRotationRadians() const
{
    if (!m_coordinate || !m_coordinate->isInitialized()) {
        return 0.0;
    }

    return m_coordinate->getOADirectionAngle() * kPi / 180.0;
}

double MapWidget::targetDisplayRotationRadians() const
{
    return m_orientationMode == OrientationMode::OAUp ? oaUpRotationRadians() : 0.0;
}

double MapWidget::compassNorthRotationRadians() const
{
    return -displayRotationRadians();
}

double MapWidget::normalizeAngleRadians(double angle)
{
    while (angle <= -kPi) {
        angle += 2.0 * kPi;
    }
    while (angle > kPi) {
        angle -= 2.0 * kPi;
    }
    return angle;
}

void MapWidget::animateToOrientationMode(OrientationMode mode)
{
    m_orientationMode = mode;
    const double current = displayRotationRadians();
    const double target = targetDisplayRotationRadians();
    const double endValue = current + normalizeAngleRadians(target - current);

    m_rotationAnimation->stop();
    m_rotationAnimation->setStartValue(current);
    m_rotationAnimation->setEndValue(endValue);
    m_rotationAnimation->start();
}

/**
 * @brief 绘制底图署名
 */
void MapWidget::drawAttribution(QPainter &painter)
{
    if (!m_basemapEnabled || !m_osmTileLayer) {
        return;
    }

    const QString attribution = m_osmTileLayer->attributionText();
    if (attribution.isEmpty()) {
        return;
    }

    painter.setFont(QFont("Arial", 8));
    const int padding = 6;
    const int textWidth = painter.fontMetrics().horizontalAdvance(attribution);
    const int textHeight = painter.fontMetrics().height();
    const QRectF textRect(width() - textWidth - padding * 2 - 8,
                          height() - textHeight - padding * 2 - 8,
                          textWidth + padding * 2,
                          textHeight + padding * 2);

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(255, 255, 255, 220));
    painter.drawRoundedRect(textRect, 4, 4);
    painter.setPen(Qt::black);
    painter.drawText(textRect.adjusted(padding, padding, -padding, -padding), Qt::AlignCenter, attribution);
}

/**
 * @brief 绘制指南针
 */
void MapWidget::drawCompass(QPainter &painter)
{
    painter.save();
    const QPointF center = compassCenter();

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(QColor(50, 50, 50, 200), 1.5));
    painter.setBrush(QColor(255, 255, 255, 220));
    painter.drawEllipse(center, kCompassRadius, kCompassRadius);

    painter.translate(center);
    painter.rotate(compassNorthRotationRadians() * 180.0 / kPi);

    QPainterPath northArrow;
    northArrow.moveTo(0.0, -kCompassRadius + 6.0);
    northArrow.lineTo(7.0, 0.0);
    northArrow.lineTo(0.0, -6.0);
    northArrow.lineTo(-7.0, 0.0);
    northArrow.closeSubpath();

    QPainterPath southArrow;
    southArrow.moveTo(0.0, kCompassRadius - 6.0);
    southArrow.lineTo(5.0, 2.0);
    southArrow.lineTo(0.0, 8.0);
    southArrow.lineTo(-5.0, 2.0);
    southArrow.closeSubpath();

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(220, 20, 60));
    painter.drawPath(northArrow);
    painter.setBrush(QColor(120, 120, 120));
    painter.drawPath(southArrow);

    painter.setPen(QPen(QColor(50, 50, 50, 220), 1.2));
    painter.drawLine(QPointF(0.0, -kCompassRadius + 4.0), QPointF(0.0, kCompassRadius - 4.0));
    painter.drawLine(QPointF(-6.0, 0.0), QPointF(6.0, 0.0));

    painter.setPen(Qt::black);
    painter.setFont(QFont("Arial", 10, QFont::Bold));
    painter.drawText(QRectF(-10.0, -kCompassRadius - 8.0, 20.0, 18.0), Qt::AlignCenter, QStringLiteral("N"));
    painter.restore();
}

void MapWidget::drawRecenterButton(QPainter &painter)
{
    const QRectF buttonRect = recenterButtonRect();

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(QColor(70, 70, 70, 180), 1.0));
    painter.setBrush(QColor(255, 255, 255, m_recenterPressed ? 240 : 220));
    painter.drawRoundedRect(buttonRect, 8, 8);

    const QPointF center = buttonRect.center();
    painter.setPen(QPen(QColor(35, 35, 35, 210), 1.6, Qt::SolidLine, Qt::RoundCap));
    painter.drawEllipse(center, 7.0, 7.0);
    painter.drawLine(QPointF(center.x() - 10.0, center.y()), QPointF(center.x() - 3.5, center.y()));
    painter.drawLine(QPointF(center.x() + 3.5, center.y()), QPointF(center.x() + 10.0, center.y()));
    painter.drawLine(QPointF(center.x(), center.y() - 10.0), QPointF(center.x(), center.y() - 3.5));
    painter.drawLine(QPointF(center.x(), center.y() + 3.5), QPointF(center.x(), center.y() + 10.0));
    painter.setBrush(QColor(0, 102, 204, 210));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(center, 2.6, 2.6);
    painter.restore();
}

void MapWidget::drawScaleBar(QPainter &painter)
{
    const auto [pixelLength, meters, label] = currentScaleBarSpec();
    Q_UNUSED(meters);

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setFont(QFont("Arial", 8));

    const int paddingX = 8;
    const int paddingY = 6;
    const int barHeight = 8;
    const int labelGap = 4;
    const int textWidth = painter.fontMetrics().horizontalAdvance(label);
    const int textHeight = painter.fontMetrics().height();
    const QRectF panelRect(kScaleBarMargin,
                           height() - kScaleBarMargin - (paddingY * 2 + barHeight + labelGap + textHeight),
                           std::max(pixelLength, static_cast<double>(textWidth)) + paddingX * 2,
                           paddingY * 2 + barHeight + labelGap + textHeight);

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(255, 255, 255, 200));
    painter.drawRoundedRect(panelRect, 6, 6);

    const double barLeft = panelRect.left() + paddingX;
    const double barTop = panelRect.top() + paddingY;
    const double barBottom = barTop + barHeight;
    const double barRight = barLeft + pixelLength;

    painter.setPen(QPen(QColor(35, 35, 35, 220), 1.4));
    painter.drawLine(QPointF(barLeft, barBottom), QPointF(barRight, barBottom));
    painter.drawLine(QPointF(barLeft, barTop), QPointF(barLeft, barBottom));
    painter.drawLine(QPointF(barRight, barTop), QPointF(barRight, barBottom));
    painter.drawLine(QPointF((barLeft + barRight) / 2.0, barBottom - 4.0), QPointF((barLeft + barRight) / 2.0, barBottom));

    painter.setPen(QColor(35, 35, 35, 230));
    painter.drawText(QRectF(panelRect.left() + paddingX,
                            barBottom + labelGap,
                            panelRect.width() - paddingX * 2,
                            textHeight),
                     Qt::AlignLeft | Qt::AlignVCenter,
                     label);
    painter.restore();
}

/**
 * @brief 获取当前显示旋转角（弧度）
 */
double MapWidget::displayRotationRadians() const
{
    return m_displayRotationRadians;
}

/**
 * @brief 将世界坐标转换为屏幕坐标
 */
QPointF MapWidget::worldToScreen(double worldX, double worldY) const
{
    const QPointF center(width() / 2.0, height() / 2.0);
    const QPointF worldOffset = rotateWorldOffset(worldX - m_worldCenterX,
                                                  worldY - m_worldCenterY,
                                                  displayRotationRadians());
    const double screenX = center.x() + worldOffset.x() * m_zoomFactor;
    const double screenY = center.y() - worldOffset.y() * m_zoomFactor;
    return QPointF(screenX, screenY);
}

/**
 * @brief 将屏幕坐标转换为世界坐标
 */
void MapWidget::screenToWorld(const QPointF &screenPos, double &worldX, double &worldY) const
{
    const QPointF center(width() / 2.0, height() / 2.0);
    const QPointF unrotatedOffset((screenPos.x() - center.x()) / m_zoomFactor,
                                  -(screenPos.y() - center.y()) / m_zoomFactor);
    const QPointF worldOffset = rotateWorldOffset(unrotatedOffset.x(),
                                                  unrotatedOffset.y(),
                                                  -displayRotationRadians());
    worldX = m_worldCenterX + worldOffset.x();
    worldY = m_worldCenterY + worldOffset.y();
}

/**
 * @brief 将经纬度转换为屏幕坐标
 */
QPointF MapWidget::geoToScreen(double lat, double lon) const
{
    const QPointF worldPos = geographicToWorld(lat, lon);
    return worldToScreen(worldPos.x(), worldPos.y());
}

/**
 * @brief 将经纬度转换为当前地图世界坐标
 */
QPointF MapWidget::geographicToWorld(double lat, double lon) const
{
    return OsmTileLayer::geographicToWorld(lat, lon);
}

/**
 * @brief 将当前地图世界坐标转换为经纬度
 */
QPointF MapWidget::worldToGeographic(double worldX, double worldY) const
{
    return OsmTileLayer::worldToGeographic(worldX, worldY);
}

/**
 * @brief 将本地坐标转换为当前地图世界坐标
 */
QPointF MapWidget::localToWorld(double localX, double localY) const
{
    if (!m_coordinate || !m_coordinate->isInitialized()) {
        return QPointF(localX, localY);
    }

    const QPointF geographicPos = m_coordinate->xyToWGS84(localX, localY);
    return geographicToWorld(geographicPos.x(), geographicPos.y());
}

/**
 * @brief 从姿态中解析可用的地理坐标
 */
bool MapWidget::resolvePoseGeographicPosition(const EstimatedPose& pose, QPointF &geographicPos) const
{
    if (isValidLatitudeLongitude(pose.latitude, pose.longitude)) {
        geographicPos = QPointF(pose.latitude, pose.longitude);
        return true;
    }

    if (m_coordinate && m_coordinate->isInitialized()) {
        const QPointF converted = m_coordinate->xyToWGS84(pose.x, pose.y);
        if (isValidLatitudeLongitude(converted.x(), converted.y())) {
            geographicPos = converted;
            return true;
        }
    }

    return false;
}

/**
 * @brief 判断当前是否具备地理上下文以绘制底图
 */
bool MapWidget::hasGeographicContext() const
{
    return m_hasGeographicContext || (m_coordinate && m_coordinate->isInitialized());
}

/**
 * @brief 确保地图已经建立地理上下文
 */
void MapWidget::ensureGeographicContext(double lat, double lon)
{
    if (!isValidLatitudeLongitude(lat, lon)) {
        return;
    }

    if (m_hasGeographicContext) {
        return;
    }

    m_hasGeographicContext = true;
    if (m_trajectoryPoints.isEmpty()) {
        const QPointF worldPos = geographicToWorld(lat, lon);
        m_worldCenterX = worldPos.x();
        m_worldCenterY = worldPos.y();
    }
}

void MapWidget::recenterToLocalOrigin()
{
    const QPointF originWorld = localToWorld(0.0, 0.0);
    m_worldCenterX = originWorld.x();
    m_worldCenterY = originWorld.y();
    update();
}
