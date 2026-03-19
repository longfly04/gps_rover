#define _USE_MATH_DEFINES
#include "coordinate.h"
#include "../core/common/logger.h"
#include <cmath>
#include <algorithm>

/**
 * @brief 构造函数，初始化默认值
 */
Coordinate::Coordinate() 
    : m_eA(0.0), m_nA(0.0), m_initialized(false)
{
    m_unitX[0] = 1.0; m_unitX[1] = 0.0;
    m_unitY[0] = 0.0; m_unitY[1] = 1.0;
    
    m_originLat.degrees = 0.0;
    m_originLat.minutes = 0.0;
    m_originLat.seconds = 0.0;
    m_originLat.direction = 'N';
    
    m_originLon.degrees = 0.0;
    m_originLon.minutes = 0.0;
    m_originLon.seconds = 0.0;
    m_originLon.direction = 'E';
    
    m_directionLat.degrees = 0.0;
    m_directionLat.minutes = 0.0;
    m_directionLat.seconds = 0.0;
    m_directionLat.direction = 'N';
    
    m_directionLon.degrees = 0.0;
    m_directionLon.minutes = 0.0;
    m_directionLon.seconds = 0.0;
    m_directionLon.direction = 'E';
}

/**
 * @brief 析构函数
 */
Coordinate::~Coordinate()
{
}

/**
 * @brief 将十进制度转换为度分秒
 * @param decimalDeg 十进制度
 * @param isLatitude 是否为纬度
 * @return 度分秒坐标
 */
DMSCoordinate Coordinate::decimalToDMS(double decimalDeg, bool isLatitude) {
    DMSCoordinate dms;
    
    if (decimalDeg < 0) {
        dms.direction = isLatitude ? 'S' : 'W';
        decimalDeg = -decimalDeg;
    } else {
        dms.direction = isLatitude ? 'N' : 'E';
    }
    
    dms.degrees = static_cast<int>(decimalDeg);
    double minutesDecimal = (decimalDeg - dms.degrees) * 60.0;
    dms.minutes = static_cast<int>(minutesDecimal);
    dms.seconds = (minutesDecimal - dms.minutes) * 60.0;
    
    return dms;
}

/**
 * @brief 将度分秒转换为十进制度
 * @param dms 度分秒坐标
 * @return 十进制度
 */
double Coordinate::dmsToDecimal(const DMSCoordinate& dms) {
    double decimalDeg = dms.degrees + dms.minutes / 60.0 + dms.seconds / 3600.0;
    
    if (dms.direction == 'S' || dms.direction == 'W') {
        decimalDeg = -decimalDeg;
    }
    
    return decimalDeg;
}

/**
 * @brief 设置原点O（参考点）
 * @param latO 原点O的纬度（十进制度）
 * @param lonO 原点O的经度（十进制度）
 */
void Coordinate::setOrigin(double latO, double lonO)
{
    m_originLat = decimalToDMS(latO, true);
    m_originLon = decimalToDMS(lonO, false);
    m_initialized = false;
    LOG_INFO("Origin point O set: lat=%.6f, lon=%.6f", latO, lonO);
}

/**
 * @brief 设置方向点A（用于定义Y轴方向）
 * @param latA 方向点A的纬度（十进制度）
 * @param lonA 方向点A的经度（十进制度）
 */
void Coordinate::setDirection(double latA, double lonA)
{
    m_directionLat = decimalToDMS(latA, true);
    m_directionLon = decimalToDMS(lonA, false);
    
    QPointF enuA = wgs84ToENU(latA, lonA);
    m_eA = enuA.x();
    m_nA = enuA.y();
    
    initialize();
    LOG_INFO("Direction point A set: lat=%.6f, lon=%.6f", latA, lonA);
    LOG_INFO("ENU coordinates: e=%.3f, n=%.3f", m_eA, m_nA);
}

/**
 * @brief 设置原点O和方向点A（用于定义OA坐标系）
 * @param latO 原点O的纬度
 * @param lonO 原点O的经度
 * @param latA 方向点A的纬度
 * @param lonA 方向点A的经度
 */
void Coordinate::setOriginAndDirection(double latO, double lonO, double latA, double lonA)
{
    setOrigin(latO, lonO);
    setDirection(latA, lonA);
    LOG_INFO("OA coordinate system set: O(%.6f, %.6f), A(%.6f, %.6f)", latO, lonO, latA, lonA);
}

/**
 * @brief 初始化坐标系转换参数
 * 
 * 计算OA坐标系的单位向量：
 * - unitY: OA方向的单位向量
 * - unitX: unitY顺时针旋转90°的单位向量
 */
void Coordinate::initialize()
{
    double length = std::sqrt(m_eA * m_eA + m_nA * m_nA);
    
    if (length < 1e-6) {
        LOG_WARN("Direction point A is too close to origin O, using default axes");
        m_unitY[0] = 0.0; m_unitY[1] = 1.0;
        m_unitX[0] = 1.0; m_unitX[1] = 0.0;
    } else {
        m_unitY[0] = m_eA / length;
        m_unitY[1] = m_nA / length;
        
        m_unitX[0] = m_unitY[1];
        m_unitX[1] = -m_unitY[0];
    }
    
    m_initialized = true;
    LOG_INFO("OA coordinate system initialized: unitY=(%.6f, %.6f), unitX=(%.6f, %.6f)", 
             m_unitY[0], m_unitY[1], m_unitX[0], m_unitX[1]);
}

/**
 * @brief 将WGS84经纬度转换为ENU坐标系
 * @param lat 纬度（十进制度）
 * @param lon 经度（十进制度）
 * @return ENU坐标点（东向，北向）
 */
QPointF Coordinate::wgs84ToENU(double lat, double lon) const
{
    DMSCoordinate latDMS = decimalToDMS(lat, true);
    DMSCoordinate lonDMS = decimalToDMS(lon, false);
    
    double latDecimal = dmsToDecimal(latDMS);
    double lonDecimal = dmsToDecimal(lonDMS);
    double originLatDecimal = dmsToDecimal(m_originLat);
    double originLonDecimal = dmsToDecimal(m_originLon);
    
    double latRad = latDecimal * M_PI / 180.0;
    double lonRad = lonDecimal * M_PI / 180.0;
    double originLatRad = originLatDecimal * M_PI / 180.0;
    double originLonRad = originLonDecimal * M_PI / 180.0;
    
    double dLat = latRad - originLatRad;
    double dLon = lonRad - originLonRad;
    
    double e = EARTH_RADIUS * dLon * std::cos(originLatRad);
    double n = EARTH_RADIUS * dLat;
    
    return QPointF(e, n);
}

/**
 * @brief 将ENU坐标系转换为OA本地XY坐标系
 * @param e 东向坐标
 * @param n 北向坐标
 * @return OA本地XY坐标系坐标点
 * 
 * 使用点积进行坐标变换：
 * x = e * unitX[0] + n * unitX[1]
 * y = e * unitY[0] + n * unitY[1]
 */
QPointF Coordinate::enuToXY(double e, double n) const
{
    if (!m_initialized) {
        return QPointF(e, n);
    }
    
    double x = e * m_unitX[0] + n * m_unitX[1];
    double y = e * m_unitY[0] + n * m_unitY[1];
    
    return QPointF(x, y);
}

/**
 * @brief 将WGS84经纬度直接转换为OA本地XY坐标系
 * @param lat 纬度
 * @param lon 经度
 * @return OA本地XY坐标系坐标点
 */
QPointF Coordinate::wgs84ToXY(double lat, double lon) const
{
    QPointF enu = wgs84ToENU(lat, lon);
    QPointF xy = enuToXY(enu.x(), enu.y());
    return xy;
}

/**
 * @brief 将OA本地XY坐标系转换为ENU坐标系
 * @param x X坐标
 * @param y Y坐标
 * @return ENU坐标点（东向，北向）
 * 
 * 使用逆变换（转置）：
 * e = x * unitX[0] + y * unitY[0]
 * n = x * unitX[1] + y * unitY[1]
 */
QPointF Coordinate::xyToENU(double x, double y) const
{
    if (!m_initialized) {
        return QPointF(x, y);
    }
    
    double e = x * m_unitX[0] + y * m_unitY[0];
    double n = x * m_unitX[1] + y * m_unitY[1];
    
    return QPointF(e, n);
}

/**
 * @brief 将OA本地XY坐标系转换为WGS84经纬度
 * @param x X坐标
 * @param y Y坐标
 * @return WGS84坐标点（纬度，经度）
 */
QPointF Coordinate::xyToWGS84(double x, double y) const
{
    QPointF enu = xyToENU(x, y);
    double e = enu.x();
    double n = enu.y();
    
    double originLatDecimal = dmsToDecimal(m_originLat);
    double originLonDecimal = dmsToDecimal(m_originLon);
    
    double originLatRad = originLatDecimal * M_PI / 180.0;
    double originLonRad = originLonDecimal * M_PI / 180.0;
    
    double dLat = n / EARTH_RADIUS;
    double dLon = e / (EARTH_RADIUS * std::cos(originLatRad));
    
    double latRad = originLatRad + dLat;
    double lonRad = originLonRad + dLon;
    
    double latDecimal = latRad * 180.0 / M_PI;
    double lonDecimal = lonRad * 180.0 / M_PI;
    
    DMSCoordinate latDMS = decimalToDMS(latDecimal, true);
    DMSCoordinate lonDMS = decimalToDMS(lonDecimal, false);
    
    double finalLat = dmsToDecimal(latDMS);
    double finalLon = dmsToDecimal(lonDMS);
    
    return QPointF(finalLat, finalLon);
}

/**
 * @brief 检查坐标系是否已初始化
 * @return 是否已初始化
 */
bool Coordinate::isInitialized() const
{
    return m_initialized;
}

/**
 * @brief 获取原点O的纬度
 * @return 纬度（十进制度）
 */
double Coordinate::getOriginLatitude() const
{
    return dmsToDecimal(m_originLat);
}

/**
 * @brief 获取原点O的经度
 * @return 经度（十进制度）
 */
double Coordinate::getOriginLongitude() const
{
    return dmsToDecimal(m_originLon);
}

/**
 * @brief 获取方向点A的纬度
 * @return 纬度（十进制度）
 */
double Coordinate::getDirectionLatitude() const
{
    return dmsToDecimal(m_directionLat);
}

/**
 * @brief 获取方向点A的经度
 * @return 经度（十进制度）
 */
double Coordinate::getDirectionLongitude() const
{
    return dmsToDecimal(m_directionLon);
}

/**
 * @brief 计算两点之间的距离（米）
 * @param lat1 第一点纬度
 * @param lon1 第一点经度
 * @param lat2 第二点纬度
 * @param lon2 第二点经度
 * @return 距离（米）
 */
double Coordinate::calculateDistance(double lat1, double lon1, double lat2, double lon2) const
{
    return calculateWGS84Distance(lat1, lon1, lat2, lon2);
}

/**
 * @brief 计算OA方向角（从O到A的方位角）
 * @return 方位角（度，从正北顺时针）
 */
double Coordinate::getOADirectionAngle() const
{
    if (!m_initialized) {
        return 0.0;
    }
    
    double angle = std::atan2(m_eA, m_nA) * 180.0 / M_PI;
    if (angle < 0) {
        angle += 360.0;
    }
    return angle;
}

/**
 * @brief 计算两个WGS84点之间的距离（米）
 * @param lat1 第一点纬度
 * @param lon1 第一点经度
 * @param lat2 第二点纬度
 * @param lon2 第二点经度
 * @return 距离（米）
 */
double Coordinate::calculateWGS84Distance(double lat1, double lon1, double lat2, double lon2)
{
    double lat1Rad = lat1 * M_PI / 180.0;
    double lat2Rad = lat2 * M_PI / 180.0;
    double dLat = (lat2 - lat1) * M_PI / 180.0;
    double dLon = (lon2 - lon1) * M_PI / 180.0;
    
    double a = std::sin(dLat / 2) * std::sin(dLat / 2) +
               std::cos(lat1Rad) * std::cos(lat2Rad) *
               std::sin(dLon / 2) * std::sin(dLon / 2);
    double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));
    
    return EARTH_RADIUS * c;
}
