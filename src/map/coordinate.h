#ifndef COORDINATE_H
#define COORDINATE_H

#include <QPointF>
#include <vector>

/**
 * @brief 度分秒坐标结构体
 */
struct DMSCoordinate {
    double degrees; ///< 度
    double minutes; ///< 分
    double seconds; ///< 秒，精度至少4位小数
    char direction; ///< 方向：N, S, E, W
};

/**
 * @brief 坐标转换模块，负责地理坐标和局部坐标系之间的转换
 * 
 * 本地OA坐标系定义：
 * - O为原点（参考点）
 * - OA方向为+Y轴
 * - +X轴为+Y轴顺时针旋转90°
 * 
 * 坐标转换流程：
 * WGS84 <-> ENU <-> Local XY(OA坐标系)
 */
class Coordinate
{
public:
    /**
     * @brief 构造函数
     */
    Coordinate();
    
    /**
     * @brief 析构函数
     */
    ~Coordinate();
    
    /**
     * @brief 设置原点O（参考点）
     * @param latO 原点O的纬度
     * @param lonO 原点O的经度
     */
    void setOrigin(double latO, double lonO);
    
    /**
     * @brief 设置方向点A（用于定义Y轴方向）
     * @param latA 方向点A的纬度
     * @param lonA 方向点A的经度
     */
    void setDirection(double latA, double lonA);
    
    /**
     * @brief 设置原点O和方向点A（用于定义OA坐标系）
     * @param latO 原点O的纬度
     * @param lonO 原点O的经度
     * @param latA 方向点A的纬度
     * @param lonA 方向点A的经度
     */
    void setOriginAndDirection(double latO, double lonO, double latA, double lonA);
    
    /**
     * @brief 将WGS84经纬度转换为ENU坐标系
     * @param lat 纬度
     * @param lon 经度
     * @return ENU坐标点（东向，北向）
     */
    QPointF wgs84ToENU(double lat, double lon) const;
    
    /**
     * @brief 将ENU坐标系转换为OA本地XY坐标系
     * @param e 东向坐标
     * @param n 北向坐标
     * @return OA本地XY坐标系坐标点
     */
    QPointF enuToXY(double e, double n) const;
    
    /**
     * @brief 将WGS84经纬度直接转换为OA本地XY坐标系
     * @param lat 纬度
     * @param lon 经度
     * @return OA本地XY坐标系坐标点
     */
    QPointF wgs84ToXY(double lat, double lon) const;
    
    /**
     * @brief 将OA本地XY坐标系转换为ENU坐标系
     * @param x X坐标
     * @param y Y坐标
     * @return ENU坐标点（东向，北向）
     */
    QPointF xyToENU(double x, double y) const;
    
    /**
     * @brief 将OA本地XY坐标系转换为WGS84经纬度
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
    
    /**
     * @brief 获取原点O的纬度
     * @return 纬度（十进制度）
     */
    double getOriginLatitude() const;
    
    /**
     * @brief 获取原点O的经度
     * @return 经度（十进制度）
     */
    double getOriginLongitude() const;
    
    /**
     * @brief 获取方向点A的纬度
     * @return 纬度（十进制度）
     */
    double getDirectionLatitude() const;
    
    /**
     * @brief 获取方向点A的经度
     * @return 经度（十进制度）
     */
    double getDirectionLongitude() const;
    
    /**
     * @brief 计算两点之间的距离（米）
     * @param lat1 第一点纬度
     * @param lon1 第一点经度
     * @param lat2 第二点纬度
     * @param lon2 第二点经度
     * @return 距离（米）
     */
    double calculateDistance(double lat1, double lon1, double lat2, double lon2) const;
    
    /**
     * @brief 计算OA方向角（从O到A的方位角）
     * @return 方位角（度，从正北顺时针）
     */
    double getOADirectionAngle() const;

public:
    /**
     * @brief 将十进制度转换为度分秒
     * @param decimalDeg 十进制度
     * @param isLatitude 是否为纬度
     * @return 度分秒坐标
     */
    static DMSCoordinate decimalToDMS(double decimalDeg, bool isLatitude);
    
    /**
     * @brief 将度分秒转换为十进制度
     * @param dms 度分秒坐标
     * @return 十进制度
     */
    static double dmsToDecimal(const DMSCoordinate& dms);
    
    /**
     * @brief 计算两个WGS84点之间的距离（米）
     * @param lat1 第一点纬度
     * @param lon1 第一点经度
     * @param lat2 第二点纬度
     * @param lon2 第二点经度
     * @return 距离（米）
     */
    static double calculateWGS84Distance(double lat1, double lon1, double lat2, double lon2);

private:
    /**
     * @brief 初始化坐标系转换参数
     */
    void initialize();
    
    DMSCoordinate m_originLat; ///< 原点O的纬度（度分秒格式）
    DMSCoordinate m_originLon; ///< 原点O的经度（度分秒格式）
    DMSCoordinate m_directionLat; ///< 方向点A的纬度（度分秒格式）
    DMSCoordinate m_directionLon; ///< 方向点A的经度（度分秒格式）
    
    double m_eA;   ///< 方向点A的东向坐标（ENU）
    double m_nA;   ///< 方向点A的北向坐标（ENU）
    
    double m_unitX[2]; ///< X轴单位向量（ENU坐标系下，+Y顺时针90°）
    double m_unitY[2]; ///< Y轴单位向量（ENU坐标系下，OA方向）
    
    bool m_initialized; ///< 是否已初始化
    
    static constexpr double EARTH_RADIUS = 6378137.0; ///< 地球半径（米）
};

#endif // COORDINATE_H
