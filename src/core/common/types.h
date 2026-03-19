/*
 * OB_GINS: An Optimization-Based GNSS/INS Integrated Navigation System
 *
 * Copyright (C) 2022 i2Nav Group, Wuhan University
 *
 *     Author : Hailiang Tang
 *    Contact : thl@whu.edu.cn
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef TYPES_H
#define TYPES_H
// 定义M_PI（如果未定义）
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <Eigen/Geometry>
#include <string>

using Eigen::Matrix3d;
using Eigen::Quaterniond;
using Eigen::Vector3d;

typedef struct GNSS {
    double timestamp; // 系统时间戳
    double time; // 本地时间（经过时区转换后的时间戳）
    double UtcTime; // UTC时间（从GGA消息中解析的时间戳）
    double dt; // 时间同步的dt值
    std::string UtcDate; // UTC日期（从GGA消息中解析的日期，格式：YYYY-MM-DD）

    Vector3d blh;
    Vector3d min;
    Vector3d sec;
    Vector3d vel;
    Vector3d std;

    bool isvalid;
    
    // 定位质量和模式
    int quality;
    std::string fix_mode;
    
    // DOP值
    double hdop;
    double vdop;
    double pdop;
    
    // 卫星数
    int visible_sv;
    int used_sv;
    
    // VTG消息相关
    double ground_speed; // 地面速度（km/h）
    double true_heading; // 真航向（度）
    double magnetic_heading; // 磁航向（度）
    
    // GST消息相关
    double sigma_lat_gst; // 纬度标准差（米）
    double sigma_lon_gst; // 经度标准差（米）
    double sigma_alt_gst; // 高度标准差（米）
    double sigma_range; // 伪距标准差（米）
    double pr_rms; // PrRMS值
} GNSS;

typedef struct IMU {
    double timestamp; // 系统时间戳
    double time; // IMU模块的片上时间

    double gpstime; // GPS获取的卫星时间转换得到的精确时间
    bool synced; // 是否同步到GNSS时间
    double dt; // 片上时间与GPS时间的差值

    Vector3d dtheta;
    Vector3d dvel;
    Vector3d angle; // 角度 [rad]
    Vector3d acceleration; // 加速度 [m/s^2]
    Vector3d angular_velocity; // 角速度 [rad/s]

    double odovel;
    
    // 磁场数据
    Vector3d magnetic_field; // 磁场 [uT]
    
    // 温度数据
    double temperature; // 温度 [℃]
    
    // 航向数据
    double magnetic_heading; // 磁航向 [度]
    double true_heading; // 真航向 [度]
} IMU;

typedef struct Pose {
    Matrix3d R;
    Vector3d t;
} Pose;

// 时间转换函数
std::string gpsTimeToStandardFormat(double gps_seconds, int gps_week, int year, int month, int day);
std::string localTimeToStandardFormat(double local_time);

#endif // TYPES_H