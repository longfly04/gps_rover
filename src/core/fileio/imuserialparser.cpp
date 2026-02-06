/*
 * KF-GINS: An EKF-Based GNSS/INS Integrated Navigation System
 *
 * Copyright (C) 2022 i2Nav Group, Wuhan University
 *
 *     Author : Your Name
 *    Contact : your.email@whu.edu.cn
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

#include <iostream>
#include <string>
#include <iomanip>
#include <chrono>
#include <thread>
#include <yaml-cpp/yaml.h>
#include <queue>

#include <ctime>

#include "common/types.h"
#include "fileio/imuserialloader.h"

/**
 * @brief 带时间戳的IMU数据结构
 * 用于存储解析后的IMU数据及其接收时间
 */
struct TimestampedIMU {
    IMU data;                   // IMU数据
    std::chrono::steady_clock::time_point timestamp; // 接收时间戳
    
    TimestampedIMU(const IMU& imu) : data(imu) {
        timestamp = std::chrono::steady_clock::now();
    }
};

/**
 * @brief 计算水平方向角（磁航向）
 * @param magnetic_field 磁场数据 [uT]
 * @return 水平方向角 [度]
 */
double calculateHorizontalHeading(const Vector3d& magnetic_field) {
    // 计算水平方向角（磁航向）
    // 公式：heading = arctan2(Hy, Hx) * 180/π
    // 其中，Hx和Hy是磁场在水平面上的分量
    double heading = std::atan2(magnetic_field[1], magnetic_field[0]) * 180.0 / M_PI;
    
    // 转换为0-360度范围
    if (heading < 0) {
        heading += 360.0;
    }
    
    return heading;
}

/**
 * @brief 计算真航向（需要磁偏角修正）
 * @param magnetic_heading 磁航向 [度]
 * @param declination 磁偏角 [度]
 * @return 真航向 [度]
 */
double calculateTrueHeading(double magnetic_heading, double declination = 0.0) {
    // 真航向 = 磁航向 + 磁偏角
    double true_heading = magnetic_heading + declination;
    
    // 转换为0-360度范围
    if (true_heading < 0) {
        true_heading += 360.0;
    } else if (true_heading >= 360.0) {
        true_heading -= 360.0;
    }
    
    return true_heading;
}

/**
 * @brief 根据方向角计算方向标签
 * @param heading 方向角 [度]
 * @return 方向标签（N, S, E, W, NE, NW, SE, SW）
 */
std::string getDirectionLabel(double heading) {
    // 将方向角转换为0-360度范围
    if (heading < 0) {
        heading += 360.0;
    } else if (heading >= 360.0) {
        heading -= 360.0;
    }
    
    // 根据角度计算方向标签
    if (heading >= 337.5 || heading < 22.5) {
        return "N";
    } else if (heading >= 22.5 && heading < 67.5) {
        return "NE";
    } else if (heading >= 67.5 && heading < 112.5) {
        return "E";
    } else if (heading >= 112.5 && heading < 157.5) {
        return "SE";
    } else if (heading >= 157.5 && heading < 202.5) {
        return "S";
    } else if (heading >= 202.5 && heading < 247.5) {
        return "SW";
    } else if (heading >= 247.5 && heading < 292.5) {
        return "W";
    } else {
        return "NW";
    }
}

/**
 * @brief 主函数：IMU串口解析器
 * @return 0表示成功，-1表示失败
 */
int main(int argc, char *argv[]) {
    // 设置全局输出刷新
    std::cout.rdbuf()->pubsetbuf(nullptr, 0);
    std::cout << std::unitbuf;
    std::cerr.rdbuf()->pubsetbuf(nullptr, 0);
    std::cerr << std::unitbuf;
    
    // 检查命令行参数
    if (argc != 2) {
        std::cerr << "ERROR: usage: imuserialparser config.yaml" << std::endl;
        return -1;
    }
    
    // 加载YAML配置文件
    YAML::Node config;
    try {
        config = YAML::LoadFile(argv[1]);
    } catch (YAML::Exception &) {
        std::cerr << "ERROR: Failed to read configuration file. Please check the path and format!" << std::endl;
        return -1;
    }
    
    // 读取串口配置
    std::string imu_serial_port;
    int imu_baudrate = 115200;
    int imu_rate = 200; // IMU采样率，默认200Hz
    double output_frequency = 10.0; // 默认输出频率：10Hz
    
    try {
        imu_serial_port = config["imu_serial_port"].as<std::string>();
        imu_baudrate = config["imu_baudrate"].as<int>(115200);
        imu_rate = config["imu_rate"].as<int>(200);
        output_frequency = config["output_frequency"].as<double>(10.0);
    } catch (YAML::Exception &) {
        std::cerr << "ERROR: Failed when loading serial port configuration!" << std::endl;
        return -1;
    }
    
    std::cout << "IMU Serial Parser" << std::endl;
    std::cout << "================" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Testing IMU Serial Parser with Real Serial Data..." << std::endl;
    std::cout << "Opening IMU serial port: " << imu_serial_port << ", " << imu_baudrate << " baud..." << std::endl;
    std::cout << "IMU sampling rate: " << imu_rate << " Hz" << std::endl;
    std::cout << "Output frequency: " << output_frequency << " Hz" << std::endl;
    
    // 创建IMU Serial Loader实例
    ImuSerialLoader imu_loader(imu_serial_port, imu_baudrate, imu_rate);
    
    if (!imu_loader.isOpen()) {
        std::cerr << "ERROR: Failed to open IMU serial port " << imu_serial_port << std::endl;
        std::cout << "Check if the port is available and not being used by another process." << std::endl;
        return -1;
    }
    
    std::cout << "IMU serial port opened successfully!" << std::endl;
    std::cout << "Reading and parsing IMU data..." << std::endl;
    std::cout << std::endl;
    std::cout << "date         time          ang_vel_x ang_vel_y ang_vel_z acc_x    acc_y    acc_z    mag_x    mag_y    mag_z    mag_heading true_heading direction" << std::endl;
    std::cout << "==============================================================================================================================" << std::endl;
    
    // 计算输出间隔时间（毫秒）
    int output_interval_ms = static_cast<int>(1000.0 / output_frequency);
    
    // 记录上次输出时间
    auto last_output_time = std::chrono::steady_clock::now();
    
    // IMU数据队列，用于暂存解析的数据
    std::queue<TimestampedIMU> imu_data_queue;
    
    // 主循环
    while (true) {
        try {
            // 从串口读取并解析IMU数据
            const IMU& imu = imu_loader.next();
            
            // 检查数据是否有效
            if (imu.time > 0) {
                // 数据有效性验证
                if (std::isnan(imu.dtheta[0]) || std::isinf(imu.dtheta[0]) ||
                    std::isnan(imu.dtheta[1]) || std::isinf(imu.dtheta[1]) ||
                    std::isnan(imu.dtheta[2]) || std::isinf(imu.dtheta[2])) {
                    std::cerr << "WARNING: Invalid IMU angular velocity data detected!" << std::endl;
                    continue;
                }
                
                if (std::isnan(imu.dvel[0]) || std::isinf(imu.dvel[0]) ||
                    std::isnan(imu.dvel[1]) || std::isinf(imu.dvel[1]) ||
                    std::isnan(imu.dvel[2]) || std::isinf(imu.dvel[2])) {
                    std::cerr << "WARNING: Invalid IMU acceleration data detected!" << std::endl;
                    continue;
                }
                
                // 计算角速度（度/秒）
                Vector3d angular_vel = imu.dtheta / imu.dt * 180.0 / M_PI;
                
                // 计算加速度（m/s²）
                Vector3d acceleration = imu.dvel / imu.dt;
                
                // 复制IMU数据到本地结构体
                IMU imu_data;
                imu_data.time = imu.time;
                imu_data.dt = imu.dt;
                imu_data.dtheta = imu.dtheta;
                imu_data.dvel = imu.dvel;
                imu_data.odovel = imu.odovel;
                imu_data.magnetic_field = imu.magnetic_field;
                imu_data.temperature = imu.temperature;
                imu_data.magnetic_heading = imu.magnetic_heading;
                imu_data.true_heading = imu.true_heading;
                
                // 使用IMU结构体中已经计算好的磁航向和真航向
                double magnetic_heading = imu.magnetic_heading;
                double true_heading = imu.true_heading;
                
                // 将解析的数据存入队列
                imu_data_queue.emplace(imu_data);
                
                // 限制队列大小，避免内存占用过高
                // 保留足够的历史数据以确保能获取到最新数据
                const int MAX_QUEUE_SIZE = 100;
                if (imu_data_queue.size() > MAX_QUEUE_SIZE) {
                    imu_data_queue.pop();
                }
                
            } else {
                // 数据无效，打印警告
                static int invalid_count = 0;
                if (++invalid_count % 10 == 0) {
                    std::cerr << "WARNING: Received " << invalid_count << " invalid IMU data packets" << std::endl;
                }
            }
            
            // 检查是否需要输出数据
            auto current_time = std::chrono::steady_clock::now();
            auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_output_time).count();
            
            if (elapsed_ms >= output_interval_ms && !imu_data_queue.empty()) {
                // 更新上次输出时间
                last_output_time = current_time;
                
                // 获取队列中最新的数据（队尾）
                TimestampedIMU latest_imu = imu_data_queue.back();
                const IMU& imu_data = latest_imu.data;
                
                // 使用IMU数据中解析的时间（用于时间戳）
                // 获取整数部分（秒）和小数部分（毫秒）
                long long seconds = static_cast<long long>(imu_data.time);
                int milliseconds = static_cast<int>((imu_data.time - seconds) * 1000);
                
                // 转换为系统时间
                time_t now = static_cast<time_t>(seconds);
                struct tm tm_now;
                localtime_s(&tm_now, &now);
                
                // 构建时间戳字符串（YYYY-MM-DD HH:MM:SS.ms）
                char timestamp_str[30];
                sprintf_s(timestamp_str, sizeof(timestamp_str), "%04d-%02d-%02d %02d:%02d:%02d.%03d", 
                        tm_now.tm_year + 1900, tm_now.tm_mon + 1, tm_now.tm_mday,
                        tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec, milliseconds);
                
                // 计算角速度（度/秒）和加速度（m/s²）
                Vector3d angular_vel = imu_data.dtheta / imu_data.dt * 180.0 / M_PI;
                Vector3d acceleration = imu_data.dvel / imu_data.dt;
                
                // 计算方向标签
                std::string direction_label = getDirectionLabel(imu_data.true_heading);
                
                // 按行输出数据，包含时间戳
                std::cout << timestamp_str << ", "
                          << std::fixed << std::setprecision(2) << angular_vel[0] << ", "
                          << std::fixed << std::setprecision(2) << angular_vel[1] << ", "
                          << std::fixed << std::setprecision(2) << angular_vel[2] << ", "
                          << std::fixed << std::setprecision(2) << acceleration[0] << ", "
                          << std::fixed << std::setprecision(2) << acceleration[1] << ", "
                          << std::fixed << std::setprecision(2) << acceleration[2] << ", "
                          << std::fixed << std::setprecision(1) << imu_data.magnetic_field[0] << ", "
                          << std::fixed << std::setprecision(1) << imu_data.magnetic_field[1] << ", "
                          << std::fixed << std::setprecision(1) << imu_data.magnetic_field[2] << ", "
                          << std::fixed << std::setprecision(1) << imu_data.magnetic_heading << ", "
                          << std::fixed << std::setprecision(1) << imu_data.true_heading << ", "
                          << direction_label << std::endl;
                
                // 输出后，清空队列中比当前输出数据陈旧的数据
                // 由于std::queue不支持直接访问队尾，我们使用一个临时队列来重建
                std::queue<TimestampedIMU> temp_queue;
                
                // 保留最新的数据（确保队列不为空）
                if (!imu_data_queue.empty()) {
                    temp_queue.emplace(imu_data_queue.back());
                }
                
                // 替换原队列
                imu_data_queue.swap(temp_queue);
            }
            
            // 短暂休眠，避免CPU占用过高
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        } catch (const std::exception& e) {
            std::cerr << "ERROR: Exception in main loop: " << e.what() << std::endl;
            // 短暂休眠，避免错误循环导致CPU占用过高
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    
    return 0;
}
