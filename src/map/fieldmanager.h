#ifndef FIELDMANAGER_H
#define FIELDMANAGER_H

#include <QPointF>
#include <vector>

/**
 * @brief 田块管理模块，负责田块边界的处理和管理
 */
class FieldManager
{
public:
    /**
     * @brief 构造函数
     */
    FieldManager();
    
    /**
     * @brief 析构函数
     */
    ~FieldManager();
    
    /**
     * @brief 设置田块边界多边形（WGS84经纬度）
     * @param boundary 边界点列表，按顺时针或逆时针顺序排列
     */
    void setBoundary(const std::vector<QPointF>& boundary);
    
    /**
     * @brief 设置田块边界多边形（作业XY坐标系）
     * @param boundary 边界点列表，按顺时针或逆时针顺序排列
     */
    void setBoundaryXY(const std::vector<QPointF>& boundary);
    
    /**
     * @brief 获取田块边界多边形（作业XY坐标系）
     * @return 边界点列表
     */
    std::vector<QPointF> getBoundaryXY() const;
    
    /**
     * @brief 计算田块的最小外接矩形
     * @param minX 最小X坐标
     * @param minY 最小Y坐标
     * @param maxX 最大X坐标
     * @param maxY 最大Y坐标
     */
    void getBoundingBox(double& minX, double& minY, double& maxX, double& maxY) const;
    
    /**
     * @brief 计算田块的Y轴范围
     * @param minY 最小Y坐标
     * @param maxY 最大Y坐标
     */
    void getYRange(double& minY, double& maxY) const;
    
    /**
     * @brief 计算田块的X轴范围
     * @param minX 最小X坐标
     * @param maxX 最大X坐标
     */
    void getXRange(double& minX, double& maxX) const;
    
    /**
     * @brief 计算水平线与田块边界的交线段
     * @param y Y坐标
     * @return 交线段列表，每个线段为(x_start, x_end)
     */
    std::vector<std::pair<double, double>> getIntersectionsWithY(double y) const;
    
    /**
     * @brief 检查点是否在田块内
     * @param x X坐标
     * @param y Y坐标
     * @return 是否在田块内
     */
    bool isPointInside(double x, double y) const;
    
    /**
     * @brief 计算田块面积
     * @return 面积（平方米）
     */
    double calculateArea() const;
    
    /**
     * @brief 计算原点O（播种起始点）
     * @return 原点坐标
     */
    QPointF calculateOrigin() const;

private:
    /**
     * @brief 计算线段与水平线的交点
     * @param p1 线段起点
     * @param p2 线段终点
     * @param y Y坐标
     * @param x 交点X坐标（输出）
     * @return 是否有交点
     */
    bool lineHorizontalIntersection(const QPointF& p1, const QPointF& p2, double y, double& x) const;
    
    /**
     * @brief 计算多边形面积
     * @param polygon 多边形点列表
     * @return 面积
     */
    double polygonArea(const std::vector<QPointF>& polygon) const;
    
    /**
     * @brief 检查点是否在多边形内（射线法）
     * @param point 点坐标
     * @param polygon 多边形点列表
     * @return 是否在多边形内
     */
    bool pointInPolygon(const QPointF& point, const std::vector<QPointF>& polygon) const;
    
    std::vector<QPointF> boundaryXY; ///< 田块边界（作业XY坐标系）
};

#endif // FIELDMANAGER_H