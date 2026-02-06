#include "fieldmanager.h"
#include <cmath>
#include <algorithm>

/**
 * @brief 构造函数
 */
FieldManager::FieldManager()
{
}

/**
 * @brief 析构函数
 */
FieldManager::~FieldManager()
{
}

/**
 * @brief 设置田块边界多边形（WGS84经纬度）
 * @param boundary 边界点列表，按顺时针或逆时针顺序排列
 */
void FieldManager::setBoundary(const std::vector<QPointF>& boundary)
{
    // 注意：这里需要通过CoordinateConverter转换为XY坐标系
    // 但由于FieldManager不直接依赖CoordinateConverter，
    // 实际使用时应该先转换后调用setBoundaryXY
    boundaryXY = boundary;
}

/**
 * @brief 设置田块边界多边形（作业XY坐标系）
 * @param boundary 边界点列表，按顺时针或逆时针顺序排列
 */
void FieldManager::setBoundaryXY(const std::vector<QPointF>& boundary)
{
    boundaryXY = boundary;
}

/**
 * @brief 获取田块边界多边形（作业XY坐标系）
 * @return 边界点列表
 */
std::vector<QPointF> FieldManager::getBoundaryXY() const
{
    return boundaryXY;
}

/**
 * @brief 计算田块的最小外接矩形
 * @param minX 最小X坐标
 * @param minY 最小Y坐标
 * @param maxX 最大X坐标
 * @param maxY 最大Y坐标
 */
void FieldManager::getBoundingBox(double& minX, double& minY, double& maxX, double& maxY) const
{
    if (boundaryXY.empty()) {
        minX = minY = maxX = maxY = 0.0;
        return;
    }
    
    minX = boundaryXY[0].x();
    minY = boundaryXY[0].y();
    maxX = boundaryXY[0].x();
    maxY = boundaryXY[0].y();
    
    for (const auto& point : boundaryXY) {
        minX = std::min(minX, point.x());
        minY = std::min(minY, point.y());
        maxX = std::max(maxX, point.x());
        maxY = std::max(maxY, point.y());
    }
}

/**
 * @brief 计算田块的Y轴范围
 * @param minY 最小Y坐标
 * @param maxY 最大Y坐标
 */
void FieldManager::getYRange(double& minY, double& maxY) const
{
    double minX, maxX;
    getBoundingBox(minX, minY, maxX, maxY);
}

/**
 * @brief 计算田块的X轴范围
 * @param minX 最小X坐标
 * @param maxX 最大X坐标
 */
void FieldManager::getXRange(double& minX, double& maxX) const
{
    double minY, maxY;
    getBoundingBox(minX, minY, maxX, maxY);
}

/**
 * @brief 计算线段与水平线的交点
 * @param p1 线段起点
 * @param p2 线段终点
 * @param y Y坐标
 * @param x 交点X坐标（输出）
 * @return 是否有交点
 */
bool FieldManager::lineHorizontalIntersection(const QPointF& p1, const QPointF& p2, double y, double& x) const
{
    // 检查线段是否与水平线相交
    if ((p1.y() - y) * (p2.y() - y) > 0) {
        return false; // 线段两端点在水平线同侧
    }
    
    // 检查线段是否水平
    if (std::abs(p1.y() - p2.y()) < 1e-6) {
        return false; // 线段水平，视为无交点
    }
    
    // 计算交点X坐标
    x = p1.x() + (p2.x() - p1.x()) * (y - p1.y()) / (p2.y() - p1.y());
    
    // 检查交点是否在线段上
    if (x < std::min(p1.x(), p2.x()) - 1e-6 || x > std::max(p1.x(), p2.x()) + 1e-6) {
        return false;
    }
    
    return true;
}

/**
 * @brief 计算水平线与田块边界的交线段
 * @param y Y坐标
 * @return 交线段列表，每个线段为(x_start, x_end)
 */
std::vector<std::pair<double, double>> FieldManager::getIntersectionsWithY(double y) const
{
    std::vector<double> intersections;
    
    // 遍历田块边界的所有边
    int n = boundaryXY.size();
    for (int i = 0; i < n; ++i) {
        const QPointF& p1 = boundaryXY[i];
        const QPointF& p2 = boundaryXY[(i + 1) % n];
        
        double x;
        if (lineHorizontalIntersection(p1, p2, y, x)) {
            intersections.push_back(x);
        }
    }
    
    // 对交点按X坐标排序
    std::sort(intersections.begin(), intersections.end());
    
    // 生成交线段
    std::vector<std::pair<double, double>> segments;
    for (size_t i = 0; i < intersections.size(); i += 2) {
        if (i + 1 < intersections.size()) {
            segments.emplace_back(intersections[i], intersections[i + 1]);
        }
    }
    
    return segments;
}

/**
 * @brief 检查点是否在多边形内（射线法）
 * @param point 点坐标
 * @param polygon 多边形点列表
 * @return 是否在多边形内
 */
bool FieldManager::pointInPolygon(const QPointF& point, const std::vector<QPointF>& polygon) const
{
    int n = polygon.size();
    bool inside = false;
    
    for (int i = 0, j = n - 1; i < n; j = i++) {
        if (((polygon[i].y() > point.y()) != (polygon[j].y() > point.y())) &&
            (point.x() < (polygon[j].x() - polygon[i].x()) * (point.y() - polygon[i].y()) / (polygon[j].y() - polygon[i].y()) + polygon[i].x())) {
            inside = !inside;
        }
    }
    
    return inside;
}

/**
 * @brief 检查点是否在田块内
 * @param x X坐标
 * @param y Y坐标
 * @return 是否在田块内
 */
bool FieldManager::isPointInside(double x, double y) const
{
    return pointInPolygon(QPointF(x, y), boundaryXY);
}

/**
 * @brief 计算多边形面积
 * @param polygon 多边形点列表
 * @return 面积
 */
double FieldManager::polygonArea(const std::vector<QPointF>& polygon) const
{
    double area = 0.0;
    int n = polygon.size();
    
    for (int i = 0; i < n; ++i) {
        int j = (i + 1) % n;
        area += polygon[i].x() * polygon[j].y();
        area -= polygon[j].x() * polygon[i].y();
    }
    
    return std::abs(area) * 0.5;
}

/**
 * @brief 计算田块面积
 * @return 面积（平方米）
 */
double FieldManager::calculateArea() const
{
    return polygonArea(boundaryXY);
}

/**
 * @brief 计算原点O（播种起始点）
 * @return 原点坐标
 */
QPointF FieldManager::calculateOrigin() const
{
    if (boundaryXY.empty()) {
        return QPointF(0.0, 0.0);
    }
    
    // 计算Y轴最小值
    double minY = boundaryXY[0].y();
    for (const auto& point : boundaryXY) {
        minY = std::min(minY, point.y());
    }
    
    // 查找Y≈minY的点
    std::vector<QPointF> candidates;
    for (const auto& point : boundaryXY) {
        if (std::abs(point.y() - minY) < 0.1) {
            candidates.push_back(point);
        }
    }
    
    if (!candidates.empty()) {
        // 计算这些点的平均X坐标
        double avgX = 0.0;
        for (const auto& point : candidates) {
            avgX += point.x();
        }
        avgX /= candidates.size();
        return QPointF(avgX, minY);
    } else {
        // 否则，取边界框的左下角
        double minX, maxX, maxY;
        getBoundingBox(minX, minY, maxX, maxY);
        return QPointF(minX, minY);
    }
}