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
#include "fileio/gnssserialloader.h"

/**
 * @brief 带时间戳的GNSS数据结构
 * 用于存储解析后的GNSS数据及其接收时间
 */
struct TimestampedGNSS {
    GNSS data;                   // GNSS数据
    std::chrono::steady_clock::time_point timestamp; // 接收时间戳
    
    TimestampedGNSS(const GNSS& gnss) : data(gnss) {
        timestamp = std::chrono::steady_clock::now();
    }
};

/**
 * @brief 主函数：GNSS串口解析器
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
        std::cerr << "ERROR: usage: gnssserialparser config.yaml" << std::endl;
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
    std::string gnss_serial_port;
    int gnss_baudrate = 115200;
    double output_frequency = 1.0; // 默认输出频率：1Hz
    
    try {
        gnss_serial_port = config["gnss_serial_port"].as<std::string>();
        gnss_baudrate = config["gnss_baudrate"].as<int>(115200);
        output_frequency = config["output_frequency"].as<double>(1.0);
    } catch (YAML::Exception &) {
        std::cerr << "ERROR: Failed when loading serial port configuration!" << std::endl;
        return -1;
    }
    
    std::cout << "GNSS Serial Parser" << std::endl;
    std::cout << "=================" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Testing GNSS Serial Parser with Real Serial Data..." << std::endl;
    std::cout << "Opening GNSS serial port: " << gnss_serial_port << ", " << gnss_baudrate << " baud..." << std::endl;
    std::cout << "Output frequency: " << output_frequency << " Hz" << std::endl;
    
    // 创建GNSS Serial Loader实例
    GnssSerialLoader gnss_loader(gnss_serial_port, gnss_baudrate);
    
    if (!gnss_loader.isOpen()) {
        std::cerr << "ERROR: Failed to open GNSS serial port " << gnss_serial_port << std::endl;
        std::cout << "Check if the port is available and not being used by another process." << std::endl;
        return -1;
    }
    
    std::cout << "GNSS serial port opened successfully!" << std::endl;
    std::cout << "Reading and parsing GNSS data..." << std::endl;
    std::cout << std::endl;
    std::cout << "date         time          lat          lon          alt    speed    heading    HDOP    VDOP    PDOP    std_lat    std_lon    std_alt    used_s    avai_s" << std::endl;
    std::cout << "=========================================================================================================================================================" << std::endl;
    
    
    // 计算输出间隔时间（毫秒）
    int output_interval_ms = static_cast<int>(1000.0 / output_frequency);
    
    // 记录上次输出时间
    auto last_output_time = std::chrono::steady_clock::now();
    
    // GNSS数据队列，用于暂存解析的数据
    std::queue<TimestampedGNSS> gnss_data_queue;
    
    // 主循环
    while (true) {
        // 从串口读取并解析GNSS数据
        const GNSS& gnss = gnss_loader.next();
        
        // 检查数据是否有效
        if (gnss.isvalid && gnss.time > 0) {
            // 将解析的数据存入队列
            gnss_data_queue.emplace(gnss);
            
            // 限制队列大小，避免内存占用过高
            // 保留足够的历史数据以确保能获取到最新数据
            const int MAX_QUEUE_SIZE = 100;
            if (gnss_data_queue.size() > MAX_QUEUE_SIZE) {
                gnss_data_queue.pop();
            }
        }
        
        // 检查是否需要输出数据
        auto current_time = std::chrono::steady_clock::now();
        auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_output_time).count();
        
        if (elapsed_ms >= output_interval_ms && !gnss_data_queue.empty()) {
            // 更新上次输出时间
            last_output_time = current_time;
            
            // 获取队列中最新的数据（队尾）
            TimestampedGNSS latest_gnss = gnss_data_queue.back();
            const GNSS& gnss_data = latest_gnss.data;
            
            // 使用GNSS数据中解析的时间（用于时间戳）
            // 假设gnss_data.time是一个浮点数，表示从某个epoch开始的秒数
            // 这里简单处理，将其转换为时间戳格式
            // 注意：实际应用中需要根据GNSS时间的具体格式进行转换
            
            // 获取整数部分（秒）和小数部分（毫秒）
            long long seconds = static_cast<long long>(gnss_data.time);
            int milliseconds = static_cast<int>((gnss_data.time - seconds) * 1000);
            
            // 转换为系统时间
            time_t now = static_cast<time_t>(seconds);
            struct tm tm_now;
            localtime_s(&tm_now, &now);
            
            // 构建时间戳字符串（YYYY-MM-DD HH:MM:SS.ms）
            char timestamp_str[30];
            sprintf_s(timestamp_str, sizeof(timestamp_str), "%04d-%02d-%02d %02d:%02d:%02d.%03d", 
                    tm_now.tm_year + 1900, tm_now.tm_mon + 1, tm_now.tm_mday,
                    tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec, milliseconds);
            
            // 使用从VTG消息中解析的速度和航向信息（如果有）
            double speed_kmh = 0.0;
            double heading_deg = 0.0;
            
            if (gnss_data.ground_speed > 0) {
                speed_kmh = gnss_data.ground_speed;
            } else {
                // 计算速度（转换为km/h）
                double speed_mps = gnss_data.vel.norm();
                speed_kmh = speed_mps * 3.6;
            }
            
            if (gnss_data.true_heading > 0) {
                heading_deg = gnss_data.true_heading;
            } else if (gnss_data.vel.norm() > 0.1) {
                // 计算航向角（度）
                heading_deg = atan2(gnss_data.vel[1], gnss_data.vel[0]) * 180.0 / M_PI;
                if (heading_deg < 0) {
                    heading_deg += 360.0;
                }
            }
            
            // 从GNSS结构体中获取卫星数
            int visible_sv = gnss_data.visible_sv > 0 ? gnss_data.visible_sv : 0;
            int used_sv = gnss_data.used_sv > 0 ? gnss_data.used_sv : 0;
            
            // 从GNSS结构体中获取DOP值
            double hdop = gnss_data.hdop > 0 ? gnss_data.hdop : 0.0;
            double vdop = gnss_data.vdop > 0 ? gnss_data.vdop : 0.0;
            double pdop = gnss_data.pdop > 0 ? gnss_data.pdop : 0.0;
            
            // 按行输出数据，包含时间戳
            std::cout << timestamp_str << ", "
                      << std::fixed << std::setprecision(7) << gnss_data.blh[0] << ", "
                      << std::fixed << std::setprecision(7) << gnss_data.blh[1] << ", "
                      << std::fixed << std::setprecision(3) << gnss_data.blh[2] << ", "
                      << std::fixed << std::setprecision(2) << speed_kmh << ", "
                      << std::fixed << std::setprecision(1) << heading_deg << ", "
                      << std::fixed << std::setprecision(2) << hdop << ", "
                      << std::fixed << std::setprecision(2) << vdop << ", "
                      << std::fixed << std::setprecision(2) << pdop << ", "
                      << std::fixed << std::setprecision(3) << gnss_data.std[0] << ", "
                      << std::fixed << std::setprecision(3) << gnss_data.std[1] << ", "
                      << std::fixed << std::setprecision(3) << gnss_data.std[2] << ", "
                      << used_sv << ", "
                      << visible_sv << std::endl;
            
            // 输出后，清空队列中比当前输出数据陈旧的数据
            // 由于std::queue不支持直接访问队尾，我们使用一个临时队列来重建
            std::queue<TimestampedGNSS> temp_queue;
            
            // 保留最新的数据（确保队列不为空）
            if (!gnss_data_queue.empty()) {
                temp_queue.emplace(gnss_data_queue.back());
            }
            
            // 替换原队列
            gnss_data_queue.swap(temp_queue);
        }
        
        // 短暂休眠，避免CPU占用过高
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    return 0;
}
