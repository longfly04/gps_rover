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
    double time;

    Vector3d blh;
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
} GNSS;

typedef struct IMU {
    double time;
    double dt;

    Vector3d dtheta;
    Vector3d dvel;

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