#include "fileio/gnssserialloader.h"

#include <vector>
#include <ctime>
#include <cmath>

#include <Eigen/Geometry>

#include "common/logger.h"
#include "common/time_sync.h"

// 定义M_PI（如果未定义）
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using Eigen::Vector3d;

GnssSerialLoader::GnssSerialLoader(const std::string& port, int baudrate) {
    if (!serial_.open(port, baudrate)) {
        is_open_ = false;
    } else {
        is_open_ = true;
        gnss_.isvalid = false;
    }
}

const GNSS& GnssSerialLoader::next() {
    // gnss_.time = 0;
    // gnss_.blh = Vector3d::Zero();
    // 不要重置速度值，保留之前从RMC消息中获取的速度
    // gnss_.vel = Vector3d::Zero();
    // gnss_.std = Vector3d::Zero();
    gnss_.isvalid = false;

    char buffer[1024] = {0};
    int bytes_read = serial_.read(buffer, sizeof(buffer) - 1);
    if (bytes_read > 0) {
        append(buffer, bytes_read);

        size_t start_idx = buffer_.find('$');
        while (start_idx != std::string::npos) {
            size_t end_idx = buffer_.find('\n', start_idx);
            if (end_idx != std::string::npos) {
                std::string nmea = buffer_.substr(start_idx, end_idx - start_idx + 1);
                buffer_ = buffer_.substr(end_idx + 1);
                if (parseNMEA(nmea)) {
                    break;
                }
            } else {
                break;
            }
            start_idx = buffer_.find('$');
        }
    }

    return gnss_;
}

bool GnssSerialLoader::isOpen() const {
    return is_open_;
}

void GnssSerialLoader::append(const char* data, int size) {
    buffer_.append(data, size);
    // 限制缓冲区大小，防止内存溢出
    if (buffer_.size() > 4096) {
        buffer_ = buffer_.substr(buffer_.size() - 4096);
    }
}

// 安全转换函数，提高解析健壮性
double safe_stod(const std::string& str, double default_val = 0.0) {
    try {
        return str.empty() ? default_val : std::stod(str);
    } catch (const std::exception&) {
        return default_val;
    }
}

int safe_stoi(const std::string& str, int default_val = 0) {
    try {
        return str.empty() ? default_val : std::stoi(str);
    } catch (const std::exception&) {
        return default_val;
    }
}

// 度分格式转换为十进制度
double convert_dm_to_dec(const std::string& dm_str, const std::string& dir_str) {
    if (dm_str.empty() || dir_str.empty()) {
        return 0.0;
    }
    
    double degrees = 0.0;
    double minutes = 0.0;
    
    // 根据纬度和经度选择不同的度数字符数
    if (dir_str == "N" || dir_str == "S") {
        // 纬度：2位度数
        degrees = safe_stod(dm_str.substr(0, 2));
        minutes = safe_stod(dm_str.substr(2));
    } else if (dir_str == "E" || dir_str == "W") {
        // 经度：3位度数
        degrees = safe_stod(dm_str.substr(0, 3));
        minutes = safe_stod(dm_str.substr(3));
    }
    
    double decimal_deg = degrees + minutes / 60.0;
    
    // 处理方向
    if (dir_str == "S" || dir_str == "W") {
        decimal_deg = -decimal_deg;
    }
    
    return decimal_deg;
}

// 计算GPS周数从给定日期
int calculateGPSWeek(int year, int month, int day) {
    // GPS epoch: 1980年1月6日
    int gps_year = 1980;
    int gps_day = 6;
    
    // 计算从GPS epoch到给定日期的天数
    int days = 0;
    
    // 年份差异
    for (int y = gps_year; y < year; y++) {
        days += 365;
        // 闰年
        if ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0) {
            days += 1;
        }
    }
    
    // 月份差异
    int month_days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    for (int m = 0; m < month - 1; m++) {
        days += month_days[m];
        // 闰年2月
        if (m == 1 && ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0)) {
            days += 1;
        }
    }
    
    // 日差异
    days += day - gps_day;
    
    // GPS周数 = 天数 / 7
    return days / 7;
}


bool GnssSerialLoader::parseNMEA(const std::string& nmea) {
    // 解析ZDA消息获取日期信息（支持GPZDA和GNZDA）
    if (nmea.substr(0, 6) == "$GNZDA" || nmea.substr(0, 6) == "$GPZDA") {
        std::vector<std::string> fields = split(nmea, ',');
        if (fields.size() >= 7) {
            // 保存日期信息，用于后续计算完整时间戳
            if (!fields[2].empty() && !fields[3].empty() && !fields[4].empty()) {
                day_ = safe_stoi(fields[2]);
                month_ = safe_stoi(fields[3]);
                year_ = safe_stoi(fields[4]);
                has_date_ = true;
                LOG_INFO("GNSS: Got date information - Year: %d, Month: %d, Day: %d", year_, month_, day_);
                
                // 计算GPS周数
                if (year_ >= 1980) {
                    gps_week_ = calculateGPSWeek(year_, month_, day_);
                    has_gps_week_ = true;
                    LOG_INFO("GNSS: Calculated GPS week: %d", gps_week_);
                }
            }
        }
        return false;
    }
    
    // 解析GNGSA消息获取DOP值（支持GPGSA和GNGSA）
    if (nmea.substr(0, 6) == "$GNGSA" || nmea.substr(0, 6) == "$GPGSA") {
        std::vector<std::string> fields = split(nmea, ',');
        if (fields.size() >= 18) {
            // 解析PDOP、HDOP、VDOP值
            if (!fields[15].empty()) {
                gnss_.pdop = safe_stod(fields[15]);
            }
            if (!fields[16].empty()) {
                gnss_.hdop = safe_stod(fields[16]);
            }
            if (!fields[17].empty()) {
                gnss_.vdop = safe_stod(fields[17]);
            }
            LOG_INFO("GNSS: Got DOP values - PDOP: %.2f, HDOP: %.2f, VDOP: %.2f", 
                     gnss_.pdop, gnss_.hdop, gnss_.vdop);
        }
        return false;
    }
    
    // 解析GPGSV消息获取GPS可见卫星数
    if (nmea.substr(0, 6) == "$GPGSV") {
        std::vector<std::string> fields = split(nmea, ',');
        if (fields.size() >= 4) {
            // 解析可见卫星数
            int current_message = safe_stoi(fields[2]);
            int visible_sv = safe_stoi(fields[3]);
            
            if (current_message == 1) {
                // 只有第一条消息包含总可见卫星数
                gnss_.visible_sv = visible_sv;
                LOG_INFO("GNSS: GPS visible satellites: %d", visible_sv);
            }
        }
        return false;
    }
    
    // 解析BDGSV消息获取北斗可见卫星数
    if (nmea.substr(0, 6) == "$BDGSV") {
        std::vector<std::string> fields = split(nmea, ',');
        if (fields.size() >= 4) {
            // 解析可见卫星数
            int current_message = safe_stoi(fields[2]);
            int visible_sv = safe_stoi(fields[3]);
            
            if (current_message == 1) {
                // 只有第一条消息包含总可见卫星数
                // 累加北斗可见卫星数
                gnss_.visible_sv += visible_sv;
                LOG_INFO("GNSS: BeiDou visible satellites: %d, Total: %d", visible_sv, gnss_.visible_sv);
            }
        }
        return false;
    }
    
    // 解析VTG消息获取速度和航向信息（支持GPVTG和GNVTG）
    if (nmea.substr(0, 6) == "$GPVTG" || nmea.substr(0, 6) == "$GNVTG") {
        std::vector<std::string> fields = split(nmea, ',');
        if (fields.size() >= 10) {
            // 解析真航向
            if (!fields[1].empty()) {
                gnss_.true_heading = safe_stod(fields[1]);
            }
            
            // 解析磁航向
            if (!fields[3].empty()) {
                gnss_.magnetic_heading = safe_stod(fields[3]);
            }
            
            // 解析地面速度（ knots 转换为 km/h）
            if (!fields[5].empty()) {
                double speed_knots = safe_stod(fields[5]);
                gnss_.ground_speed = speed_knots * 1.852; // 1 knot = 1.852 km/h
            }
            
            // 解析地面速度（直接读取 km/h）
            if (!fields[7].empty()) {
                gnss_.ground_speed = safe_stod(fields[7]);
            }
            
            LOG_INFO("GNSS: Got speed and heading from VTG - Speed: %.2f km/h, True Heading: %.1f deg", 
                     gnss_.ground_speed, gnss_.true_heading);
        }
        return false;
    }
    
    // 解析GST消息获取伪距误差统计信息（支持GPGST和GNGST）
    if (nmea.substr(0, 6) == "$GPGST" || nmea.substr(0, 6) == "$GNGST") {
        std::vector<std::string> fields = split(nmea, ',');
        if (fields.size() >= 9) {
            // 解析纬度标准差
            if (!fields[2].empty()) {
                gnss_.sigma_lat_gst = safe_stod(fields[2]);
            }
            
            // 解析经度标准差
            if (!fields[3].empty()) {
                gnss_.sigma_lon_gst = safe_stod(fields[3]);
            }
            
            // 解析高度标准差
            if (!fields[4].empty()) {
                gnss_.sigma_alt_gst = safe_stod(fields[4]);
            }
            
            // 解析伪距标准差
            if (!fields[5].empty()) {
                gnss_.sigma_range = safe_stod(fields[5]);
            }
            
            LOG_INFO("GNSS: Got error statistics from GST - Lat: %.3f m, Lon: %.3f m, Alt: %.3f m, Range: %.3f m", 
                     gnss_.sigma_lat_gst, gnss_.sigma_lon_gst, gnss_.sigma_alt_gst, gnss_.sigma_range);
        }
        return false;
    }    
    // 检查是否是GGA消息（包含位置信息，支持GPGGA和GNGGA）
    if (nmea.substr(0, 6) == "$GPGGA" || nmea.substr(0, 6) == "$GNGGA") {
        std::vector<std::string> fields = split(nmea, ',');
        if (fields.size() >= 15) {
            // 解析时间（HHMMSS.SSS）
            std::string time_str = fields[1];
            if (!time_str.empty() && time_str.length() >= 6) {
                // 解析时分秒，处理边界情况
                int hour = 0, minute = 0;
                double second = 0.0;
                
                // 安全解析小时
                if (time_str.length() >= 2) {
                    hour = safe_stoi(time_str.substr(0, 2));
                }
                
                // 安全解析分钟
                if (time_str.length() >= 4) {
                    minute = safe_stoi(time_str.substr(2, 2));
                }
                
                // 安全解析秒
                if (time_str.length() >= 6) {
                    second = safe_stod(time_str.substr(4));
                }
                
                // 验证时间有效性
                if (hour >= 0 && hour < 24 && minute >= 0 && minute < 60 && second >= 0 && second < 60) {
                    // 计算完整的时间戳，与IMU使用相同的时间系统（从1970年开始的秒数）
                    time_t timestamp = 0;
                    struct tm tm_time = {0};
                    
                    // 获取当前系统时间作为日期参考
                    time_t now = time(nullptr);
                    struct tm tm_now;
                    localtime_s(&tm_now, &now);
                    
                    // 如果有日期信息，使用完整的时间戳
                    if (has_date_) {
                        LOG_DEBUG("GNSS: Using date information - Year: %d, Month: %d, Day: %d", 
                                 year_, month_, day_);
                        
                        tm_time.tm_year = year_ - 1900;
                        tm_time.tm_mon = month_ - 1;
                        tm_time.tm_mday = day_;
                    } else {
                        // 如果没有日期信息，使用当前系统日期
                        int current_year = tm_now.tm_year + 1900;
                        int current_month = tm_now.tm_mon + 1;
                        int current_day = tm_now.tm_mday;
                        
                        tm_time.tm_year = current_year - 1900;
                        tm_time.tm_mon = current_month - 1;
                        tm_time.tm_mday = current_day;
                        LOG_DEBUG("GNSS: Using current system date - Year: %d, Month: %d, Day: %d", 
                                 current_year, current_month, current_day);
                    }
                    
                    // 设置时间
                    tm_time.tm_hour = hour;
                    tm_time.tm_min = minute;
                    tm_time.tm_sec = static_cast<int>(second);
                    tm_time.tm_isdst = -1; // 自动判断夏令时
                    
                    // 转换为时间戳
                    // 注意：GNSS时间是UTC时间，需要转换为本地时间
                    // 先将UTC时间转换为时间戳，然后调整为本地时间
                    // 步骤1：将UTC时间转换为UTC时间戳
                    time_t utc_timestamp = mktime(&tm_time);
                    
                    // 步骤2：获取本地时间和UTC时间的差值
                    time_t local_now = time(nullptr);
                    struct tm tm_local;
                    localtime_s(&tm_local, &local_now);
                    time_t local_timestamp = mktime(&tm_local);
                    
                    struct tm tm_utc;
                    gmtime_s(&tm_utc, &local_now);
                    time_t utc_now = mktime(&tm_utc);
                    
                    int timezone_offset = static_cast<int>(difftime(local_timestamp, utc_now));
                    
                    // 步骤3：调整为本地时间戳
                    timestamp = utc_timestamp + timezone_offset;
                    
                    // 添加毫秒部分
                    double fractional_seconds = second - static_cast<int>(second);
                    double full_timestamp = static_cast<double>(timestamp) + fractional_seconds;
                    
                    // 转换为标准时间格式
                    // 注意：这里直接使用时间戳，不进行格式转换以避免依赖问题
                    LOG_INFO("GNSS: Full timestamp (local time): %.3f", full_timestamp);
                    
                    // 使用GNSS时间更新时间同步模块
                    // 注意：GNSS数据中的时间是UTC时间，需要转换为与本地时间相同的基准
                    // 这里我们直接使用计算出的本地时间戳作为GNSS时间，因为我们已经调整了时区
                    TIME_SYNC.updateGNSS(full_timestamp);
                    
                    // 获取同步后的时间戳
                    gnss_.time = TIME_SYNC.getSyncedTime();
                    LOG_INFO("GNSS: Synced timestamp: %.3f", gnss_.time);
                } else {
                    LOG_WARN("GNSS: Invalid time values - Hour: %d, Minute: %d, Second: %.3f", 
                             hour, minute, second);
                    gnss_.time = 0;
                }
            } else {
                LOG_WARN("GNSS: Invalid time string: '%s'", time_str.c_str());
                gnss_.time = 0;
            }
            
            // 解析纬度（度分格式转换为十进制）
            std::string lat_str = fields[2];
            std::string lat_dir = fields[3];
            gnss_.blh[0] = convert_dm_to_dec(lat_str, lat_dir);
            
            // 解析经度（度分格式转换为十进制）
            std::string lon_str = fields[4];
            std::string lon_dir = fields[5];
            gnss_.blh[1] = convert_dm_to_dec(lon_str, lon_dir);
            
            // 解析高度
            gnss_.blh[2] = safe_stod(fields[9]);
            
            // 解析定位质量
            int quality = safe_stoi(fields[6]);
            gnss_.isvalid = (quality >= 1);
            gnss_.quality = quality;
            
            // 从GGA消息中获取使用卫星数
            int used_sv = 0;
            if (fields.size() > 7) {
                used_sv = safe_stoi(fields[7]);
            }
            gnss_.used_sv = used_sv; // 这里是使用卫星数
            
            // 根据DOP计算标准差，使用用户等效测距误差(UERE)
            // 核心公式：位置标准差(σ) = 用户等效测距误差(UERE) × 对应的DOP值
            
            // 用户等效测距误差(UERE)设置
            // 普通单频GNSS接收机：UERE = 3.0米
            // 高精度RTK/差分接收机：UERE = 0.01-0.05米
            double uere = 3.0; // 使用普通GNSS接收机的典型值
            
            // 获取从GNGSA消息中解析的最新DOP值
            double hdop = gnss_.hdop > 0 ? gnss_.hdop : 1.0; // 默认值为1.0
            double vdop = gnss_.vdop > 0 ? gnss_.vdop : 1.0; // 默认值为1.0
            double pdop = gnss_.pdop > 0 ? gnss_.pdop : 1.0; // 默认值为1.0
            
            // 计算水平位置标准差
            double sigma_h = uere * hdop;
            
            // 计算纬度和经度标准差
            // 假设经纬度方向误差独立且相等
            double sigma_lat = sigma_h / sqrt(2.0);
            double sigma_lon = sigma_h / sqrt(2.0);
            
            // 计算高程标准差
            double sigma_alt = uere * vdop;
            
            // 限制标准差范围，避免异常值
            if (sigma_lat < 0.01) sigma_lat = 0.01;
            if (sigma_lat > 100.0) sigma_lat = 100.0;
            if (sigma_lon < 0.01) sigma_lon = 0.01;
            if (sigma_lon > 100.0) sigma_lon = 100.0;
            if (sigma_alt < 0.01) sigma_alt = 0.01;
            if (sigma_alt > 100.0) sigma_alt = 100.0;
            
            // 优先使用从GST消息中解析的标准差数据（如果有）
            if (gnss_.sigma_lat_gst > 0) {
                gnss_.std[0] = gnss_.sigma_lat_gst;  // 纬度标准差
            } else {
                gnss_.std[0] = sigma_lat;  // 纬度标准差
            }
            
            if (gnss_.sigma_lon_gst > 0) {
                gnss_.std[1] = gnss_.sigma_lon_gst;  // 经度标准差
            } else {
                gnss_.std[1] = sigma_lon;  // 经度标准差
            }
            
            if (gnss_.sigma_alt_gst > 0) {
                gnss_.std[2] = gnss_.sigma_alt_gst;  // 高度标准差
            } else {
                gnss_.std[2] = sigma_alt;  // 高度标准差
            }
            
            LOG_DEBUG("GNSS: HDOP=%.2f, VDOP=%.2f, PDOP=%.2f", hdop, vdop, pdop);
            LOG_DEBUG("GNSS: Calculated std - lat: %.3f, lon: %.3f, alt: %.3f", sigma_lat, sigma_lon, sigma_alt);
            
            // 注意：GGA消息不包含速度信息，速度信息需要从RMC消息中获取
            // 因此这里不设置速度值，保持为0，等待RMC消息更新
            
            return true;
        }
    }
    
    // 检查是否是RMC消息（包含速度信息，支持GPRMC和GNRMC）
    if (nmea.substr(0, 6) == "$GPRMC" || nmea.substr(0, 6) == "$GNRMC") {
        std::vector<std::string> fields = split(nmea, ',');
        if (fields.size() >= 8) {
            // 解析时间（HHMMSS.SSS）
            std::string time_str = fields[1];
            if (!time_str.empty() && time_str.length() >= 6) {
                // 解析时分秒，处理边界情况
                int hour = 0, minute = 0;
                double second = 0.0;
                
                // 安全解析小时
                if (time_str.length() >= 2) {
                    hour = safe_stoi(time_str.substr(0, 2));
                }
                
                // 安全解析分钟
                if (time_str.length() >= 4) {
                    minute = safe_stoi(time_str.substr(2, 2));
                }
                
                // 安全解析秒
                if (time_str.length() >= 6) {
                    second = safe_stod(time_str.substr(4));
                }
                
                // 验证时间有效性
                if (hour >= 0 && hour < 24 && minute >= 0 && minute < 60 && second >= 0 && second < 60) {
                    // 计算完整的时间戳，与IMU使用相同的时间系统（从1970年开始的秒数）
                    time_t timestamp = 0;
                    struct tm tm_time = {0};
                    
                    // 获取当前系统时间作为日期参考
                    time_t now = time(nullptr);
                    struct tm tm_now;
                    localtime_s(&tm_now, &now);
                    
                    // 如果有日期信息，使用完整的时间戳
                    if (has_date_) {
                        tm_time.tm_year = year_ - 1900;
                        tm_time.tm_mon = month_ - 1;
                        tm_time.tm_mday = day_;
                    } else {
                        // 如果没有日期信息，使用当前系统日期
                        int current_year = tm_now.tm_year + 1900;
                        int current_month = tm_now.tm_mon + 1;
                        int current_day = tm_now.tm_mday;
                        
                        tm_time.tm_year = current_year - 1900;
                        tm_time.tm_mon = current_month - 1;
                        tm_time.tm_mday = current_day;
                    }
                    
                    // 设置时间
                    tm_time.tm_hour = hour;
                    tm_time.tm_min = minute;
                    tm_time.tm_sec = static_cast<int>(second);
                    tm_time.tm_isdst = -1; // 自动判断夏令时
                    
                    // 转换为时间戳
                    // 注意：GNSS时间是UTC时间，需要转换为本地时间
                    time_t utc_timestamp = mktime(&tm_time);
                    
                    // 获取本地时间和UTC时间的差值
                    time_t local_now = time(nullptr);
                    struct tm tm_local;
                    localtime_s(&tm_local, &local_now);
                    time_t local_timestamp = mktime(&tm_local);
                    
                    struct tm tm_utc;
                    gmtime_s(&tm_utc, &local_now);
                    time_t utc_now = mktime(&tm_utc);
                    
                    int timezone_offset = static_cast<int>(difftime(local_timestamp, utc_now));
                    
                    // 调整为本地时间戳
                    timestamp = utc_timestamp + timezone_offset;
                    
                    // 添加毫秒部分
                    double fractional_seconds = second - static_cast<int>(second);
                    double full_timestamp = static_cast<double>(timestamp) + fractional_seconds;
                    
                    // 解析速度（ knots 转换为 m/s）
                    double speed_knots = safe_stod(fields[7]);
                    double speed_mps = speed_knots * 0.514444; // 1 knot = 0.514444 m/s
                    
                    // 解析航向角（度）
                    double course_deg = 0.0;
                    if (fields.size() > 8 && !fields[8].empty()) {
                        course_deg = safe_stod(fields[8]);
                    }
                    
                    // 将速度从极坐标转换为直角坐标系（北向、东向、地向）
                    // 注意：航向角是从北开始顺时针计算的
                    double course_rad = course_deg * M_PI / 180.0;
                    
                    // 北向速度
                    gnss_.vel[0] = speed_mps * cos(course_rad);
                    // 东向速度
                    gnss_.vel[1] = speed_mps * sin(course_rad);
                    // 地向速度（RMC消息不包含垂直速度，设为0）
                    gnss_.vel[2] = 0.0;
                    
                    LOG_INFO("GNSS: Got velocity from RMC - Speed: %.3f m/s, Course: %.1f deg", 
                             speed_mps, course_deg);
                    LOG_INFO("GNSS: Velocity components (N,E,D): %.3f, %.3f, %.3f m/s", 
                             gnss_.vel[0], gnss_.vel[1], gnss_.vel[2]);
                    
                    // 使用GNSS时间更新时间同步模块
                    TIME_SYNC.updateGNSS(full_timestamp);
                    
                    // 获取同步后的时间戳
                    gnss_.time = TIME_SYNC.getSyncedTime();
                    LOG_INFO("GNSS: Synced timestamp from RMC: %.3f", gnss_.time);
                    
                    // 保持isvalid状态不变，由GGA消息的定位质量决定
                }
            }
            
            // 注意：RMC消息也包含位置信息，但我们优先使用GGA消息的位置信息
            // 因此这里不更新位置值，只更新速度值
            
            return false; // 不返回true，因为我们希望继续处理GGA消息获取位置信息
        }
    }
    
    return false;
}

std::vector<std::string> GnssSerialLoader::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    for (char c : str) {
        if (c == delimiter) {
            tokens.push_back(token);
            token.clear();
        } else {
            token += c;
        }
    }
    tokens.push_back(token);
    return tokens;
}


