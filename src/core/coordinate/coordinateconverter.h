#ifndef COORDINATECONVERTER_H
#define COORDINATECONVERTER_H

#include <QPointF>

/**
 * @brief 坐标转换模块，负责地理坐标和局部坐标系之间的转换
 */
class CoordinateConverter
{
public:
    /**
     * @brief 构造函数
     */
    CoordinateConverter();
    
    /**
     * @brief 析构函数
     */
    ~CoordinateConverter();
    
    /**
     * @brief 设置参考点A（原点）
     * @param latA 参考点A的纬度
     * @param lonA 参考点A的经度
     */
    void setReferencePoint(double latA, double lonA);
    
    /**
     * @brief 设置参考点B（用于定义Y轴方向）
     * @param latB 参考点B的纬度
     * @param lonB 参考点B的经度
     */
    void setDirectionPoint(double latB, double lonB);
    
    /**
     * @brief 将WGS84经纬度转换为ENU坐标系
     * @param lat 纬度
     * @param lon 经度
     * @return ENU坐标点（东向，北向）
     */
    QPointF wgs84ToENU(double lat, double lon) const;
    
    /**
     * @brief 将ENU坐标系转换为作业XY坐标系
     * @param e 东向坐标
     * @param n 北向坐标
     * @return 作业XY坐标系坐标点
     */
    QPointF enuToXY(double e, double n) const;
    
    /**
     * @brief 将WGS84经纬度直接转换为作业XY坐标系
     * @param lat 纬度
     * @param lon 经度
     * @return 作业XY坐标系坐标点
     */
    QPointF wgs84ToXY(double lat, double lon) const;
    
    /**
     * @brief 将作业XY坐标系转换为ENU坐标系
     * @param x X坐标
     * @param y Y坐标
     * @return ENU坐标点（东向，北向）
     */
    QPointF xyToENU(double x, double y) const;
    
    /**
     * @brief 将作业XY坐标系转换为WGS84经纬度
     * @param x X坐标
     * @param y Y坐标
     * @return WGS84坐标点（纬度，经度）
     */
    QPointF xyToWGS84(double x, double y) const;
    
    /**
     * @brief 检查坐标系是否已初始化
     * @return 是否已初始化
     */
    bool isInitialized() const;

private:
    /**
     * @brief 初始化坐标系转换参数
     */
    void initialize();
    
    double latA; ///< 参考点A的纬度
    double lonA; ///< 参考点A的经度
    double eB;   ///< 参考点B的东向坐标
    double nB;   ///< 参考点B的北向坐标
    
    // 单位向量
    double unitX[2]; ///< X轴单位向量（ENU坐标系下）
    double unitY[2]; ///< Y轴单位向量（ENU坐标系下）
    
    bool initialized; ///< 是否已初始化
    
    // 地球半径（米）
    static constexpr double EARTH_RADIUS = 6378137.0;
};

#endif // COORDINATECONVERTER_H