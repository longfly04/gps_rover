#define _USE_MATH_DEFINES
#include "coordinateconverter.h"
#include <cmath>
#include <QDebug>
#include <stdexcept>

/**
 * @brief 构造函数，初始化默认值
 */
CoordinateConverter::CoordinateConverter() 
    : latA(0.0), lonA(0.0), eB(0.0), nB(0.0), initialized(false)
{
    // 初始化单位向量为默认值
    unitX[0] = 1.0; unitX[1] = 0.0;
    unitY[0] = 0.0; unitY[1] = 1.0;
}

/**
 * @brief 析构函数
 */
CoordinateConverter::~CoordinateConverter()
{
}

/**
 * @brief 设置参考点A（原点）
 * @param latA 参考点A的纬度
 * @param lonA 参考点A的经度
 */
void CoordinateConverter::setReferencePoint(double latA, double lonA)
{
    this->latA = latA;
    this->lonA = lonA;
    initialized = false; // 重置初始化状态
}

/**
 * @brief 设置参考点B（用于定义Y轴方向）
 * @param latB 参考点B的纬度
 * @param lonB 参考点B的经度
 */
void CoordinateConverter::setDirectionPoint(double latB, double lonB)
{
    // 计算参考点B在ENU坐标系中的坐标
    QPointF enuB = wgs84ToENU(latB, lonB);
    eB = enuB.x();
    nB = enuB.y();
    
    // 初始化坐标系
    initialize();
}

/**
 * @brief 初始化坐标系转换参数
 */
void CoordinateConverter::initialize()
{
    try {
        // 计算AB向量的长度
        double norm = std::sqrt(eB * eB + nB * nB);
        
        // 检查向量长度是否为零
        if (norm < 1e-6) {
            throw std::runtime_error("Reference points A and B are too close or identical");
        }
        
        // 计算Y轴单位向量（沿AB方向）
        unitY[0] = eB / norm;
        unitY[1] = nB / norm;
        
        // 计算X轴单位向量（垂直于Y轴，逆时针旋转90度）
        unitX[0] = -unitY[1];
        unitX[1] = unitY[0];
        
        initialized = true;
        qDebug() << "Coordinate system initialized successfully";
        qDebug() << "Y-axis unit vector:" << unitY[0] << unitY[1];
        qDebug() << "X-axis unit vector:" << unitX[0] << unitX[1];
    } catch (const std::exception& e) {
        qDebug() << "Error initializing coordinate system:" << e.what();
        initialized = false;
    }
}

/**
 * @brief 将WGS84经纬度转换为ENU坐标系
 * @param lat 纬度
 * @param lon 经度
 * @return ENU坐标点（东向，北向）
 */
QPointF CoordinateConverter::wgs84ToENU(double lat, double lon) const
{
    // 将角度转换为弧度
    double latRad = lat * M_PI / 180.0;
    double lonRad = lon * M_PI / 180.0;
    double latARad = latA * M_PI / 180.0;
    double lonARad = lonA * M_PI / 180.0;
    
    // 计算纬度差和经度差
    double dLat = latRad - latARad;
    double dLon = lonRad - lonARad;
    
    // 计算ENU坐标
    double e = EARTH_RADIUS * dLon * std::cos(latARad);
    double n = EARTH_RADIUS * dLat;
    
    return QPointF(e, n);
}

/**
 * @brief 将ENU坐标系转换为作业XY坐标系
 * @param e 东向坐标
 * @param n 北向坐标
 * @return 作业XY坐标系坐标点
 */
QPointF CoordinateConverter::enuToXY(double e, double n) const
{
    if (!initialized) {
        return QPointF(e, n); // 未初始化时直接返回ENU坐标
    }
    
    // 计算XY坐标（点积运算）
    double x = unitX[0] * e + unitX[1] * n;
    double y = unitY[0] * e + unitY[1] * n;
    
    return QPointF(x, y);
}

/**
 * @brief 将WGS84经纬度直接转换为作业XY坐标系
 * @param lat 纬度
 * @param lon 经度
 * @return 作业XY坐标系坐标点
 */
QPointF CoordinateConverter::wgs84ToXY(double lat, double lon) const
{
    // 先转换为ENU坐标系，再转换为XY坐标系
    QPointF enu = wgs84ToENU(lat, lon);
    return enuToXY(enu.x(), enu.y());
}

/**
 * @brief 将作业XY坐标系转换为ENU坐标系
 * @param x X坐标
 * @param y Y坐标
 * @return ENU坐标点（东向，北向）
 */
QPointF CoordinateConverter::xyToENU(double x, double y) const
{
    if (!initialized) {
        return QPointF(x, y); // 未初始化时直接返回XY坐标
    }
    
    // 计算ENU坐标（逆变换）
    double e = unitX[0] * x + unitY[0] * y;
    double n = unitX[1] * x + unitY[1] * y;
    
    return QPointF(e, n);
}

/**
 * @brief 将作业XY坐标系转换为WGS84经纬度
 * @param x X坐标
 * @param y Y坐标
 * @return WGS84坐标点（纬度，经度）
 */
QPointF CoordinateConverter::xyToWGS84(double x, double y) const
{
    // 先转换为ENU坐标系
    QPointF enu = xyToENU(x, y);
    double e = enu.x();
    double n = enu.y();
    
    // 将角度转换为弧度
    double latARad = latA * M_PI / 180.0;
    double lonARad = lonA * M_PI / 180.0;
    
    // 计算纬度和经度的变化
    double dLat = n / EARTH_RADIUS;
    double dLon = e / (EARTH_RADIUS * std::cos(latARad));
    
    // 计算目标点的经纬度
    double latRad = latARad + dLat;
    double lonRad = lonARad + dLon;
    
    // 将弧度转换为角度
    double lat = latRad * 180.0 / M_PI;
    double lon = lonRad * 180.0 / M_PI;
    
    return QPointF(lat, lon);
}

/**
 * @brief 检查坐标系是否已初始化
 * @return 是否已初始化
 */
bool CoordinateConverter::isInitialized() const
{
    return initialized;
}