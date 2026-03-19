#include "fileio/gnssserialloader.h"

#include <vector>
#include <ctime>
#include <cmath>

#include <Eigen/Geometry>

#include "common/logger.h"


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
        // 增加缓冲区大小，提高数据处理能力
        buffer_.reserve(4096);
    }
}

const GNSS& GnssSerialLoader::next() {
    gnss_.isvalid = false;

    char buffer[2048] = {0};
    int bytes_read = serial_.read(buffer, sizeof(buffer) - 1);
    if (bytes_read > 0) {
        // 记录系统时间戳（缓存，避免重复调用）
        static double last_timestamp = 0;
        static time_t last_time_t = 0;
        time_t current_time = time(nullptr);
        if (current_time != last_time_t) {
            last_time_t = current_time;
            last_timestamp = static_cast<double>(current_time);
        }
        double batch_timestamp = last_timestamp;

        append(buffer, bytes_read);

        // 优化：使用索引而不是substr来避免内存分配
        size_t processed_pos = 0;
        size_t start_idx = buffer_.find('$', processed_pos);

        // 标记是否找到了GGA消息（位置数据）
        bool hasPosition = false;

        while (start_idx != std::string::npos) {
            size_t end_idx = buffer_.find('\n', start_idx);
            if (end_idx != std::string::npos) {
                std::string nmea = buffer_.substr(start_idx, end_idx - start_idx + 1);
                processed_pos = end_idx + 1;

                // 解析NMEA消息，如果是GGA则标记有位置数据
                if (parseNMEA(nmea)) {
                    hasPosition = true;
                    // 不要立即break，继续解析后续消息（特别是RMC速度）
                }

                // 如果已经有位置数据，继续解析几条消息以获取速度
                if (hasPosition) {
                    // 再解析最多5条消息，寻找RMC速度数据
                    int extraMessages = 0;
                    const int MAX_EXTRA = 5;

                    while (extraMessages < MAX_EXTRA) {
                        start_idx = buffer_.find('$', processed_pos);
                        if (start_idx == std::string::npos) break;

                        end_idx = buffer_.find('\n', start_idx);
                        if (end_idx == std::string::npos) break;

                        nmea = buffer_.substr(start_idx, end_idx - start_idx + 1);
                        processed_pos = end_idx + 1;
                        parseNMEA(nmea);
                        extraMessages++;

                        // 如果已经获取到速度数据，可以退出
                        if (gnss_.vel[0] != 0.0 || gnss_.vel[1] != 0.0) {
                            break;
                        }
                    }

                    // 移除已处理的数据
                    buffer_.erase(0, processed_pos);
                    break;
                }

                start_idx = buffer_.find('$', processed_pos);
            } else {
                // 没有完整消息，保留未处理的数据
                break;
            }
        }

        // 如果已处理位置大于0但没有找到有效数据，清理已处理部分
        if (processed_pos > 0 && !hasPosition) {
            buffer_.erase(0, processed_pos);
        }

        // 计算时间戳
        if (gnss_.UtcTime > 0) {
            gnss_.timestamp = batch_timestamp;
            gnss_.dt = gnss_.time - batch_timestamp;
        }

        // 简化日志：只在有效数据时输出关键信息
        static int valid_count = 0;
        if (gnss_.isvalid) {
            valid_count++;
            // 每100条有效数据输出一次统计
            if (valid_count % 100 == 0) {
                LOG_INFO("GNSS: Received %d valid messages, latest - Lat: %.6f, Lon: %.6f, Alt: %.2f, Quality: %d, SV: %d",
                         valid_count, gnss_.blh[0], gnss_.blh[1], gnss_.blh[2], gnss_.quality, gnss_.used_sv);
            }
        }
    }

    return gnss_;
}

bool GnssSerialLoader::isOpen() const {
    return is_open_;
}

qint64 GnssSerialLoader::bytesAvailable() const {
    if (!is_open_) {
        return 0;
    }
    // 返回串口可用的字节数
    return serial_.bytesAvailable();
}

void GnssSerialLoader::clearBuffer() {
    // 清空内部缓冲区
    buffer_.clear();
    // 清空串口缓冲区
    serial_.clearBuffer();
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

// 度分格式转换为十进制度，并提取度、分、秒
 double convert_dm_to_dec(const std::string& dm_str, const std::string& dir_str, double& degrees, double& minutes, double& seconds) {
    if (dm_str.empty() || dir_str.empty()) {
        degrees = 0.0;
        minutes = 0.0;
        seconds = 0.0;
        return 0.0;
    }
    
    // 根据纬度和经度选择不同的度数字符数
    if (dir_str == "N" || dir_str == "S") {
        // 纬度：2位度数
        degrees = safe_stod(dm_str.substr(0, 2));
        minutes = safe_stod(dm_str.substr(2));
    } else if (dir_str == "E" || dir_str == "W") {
        // 经度：3位度数
        degrees = safe_stod(dm_str.substr(0, 3));
        minutes = safe_stod(dm_str.substr(3));
    } else {
        degrees = 0.0;
        minutes = 0.0;
    }
    
    // 计算秒（保留至少4位小数）
    seconds = (minutes - static_cast<int>(minutes)) * 60.0;
    minutes = static_cast<int>(minutes);
    
    double decimal_deg = degrees + minutes / 60.0 + seconds / 3600.0;
    
    // 处理方向
    if (dir_str == "S" || dir_str == "W") {
        decimal_deg = -decimal_deg;
        degrees = -degrees; // 负数表示南纬或西经
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

/**
 * @brief 解析NMEA消息中的UTC时间
 * @param utc_time_str UTC时间字符串
 * @param gnss 要更新的GNSS结构体
 * @return 是否解析成功
 */
bool GnssSerialLoader::parseTime(const std::string& utc_time_str, GNSS& gnss) {
    if (utc_time_str.empty() || utc_time_str.length() < 6) {
        return false;
    }

    // 解析时分秒
    int hour = 0, minute = 0;
    double second = 0.0;

    hour = safe_stoi(utc_time_str.substr(0, 2));
    minute = safe_stoi(utc_time_str.substr(2, 2));
    second = safe_stod(utc_time_str.substr(4));

    // 验证时间有效性
    if (hour < 0 || hour >= 24 || minute < 0 || minute >= 60 || second < 0 || second >= 60) {
        return false;
    }

    // 优化：缓存时区偏移量和日期信息，避免重复计算
    static int cached_timezone_offset = -1;
    static int cached_year = 0;
    static int cached_month = 0;
    static int cached_day = 0;
    static time_t cached_date_base = 0;

    // 首次运行或日期变化时计算
    time_t now = time(nullptr);
    struct tm tm_now;
    localtime_s(&tm_now, &now);

    int current_year = tm_now.tm_year + 1900;
    int current_month = tm_now.tm_mon + 1;
    int current_day = tm_now.tm_mday;

    // 如果有日期信息，使用GPS提供的日期
    if (has_date_) {
        current_year = year_;
        current_month = month_;
        current_day = day_;
    }

    // 检查是否需要重新计算基准时间
    if (cached_timezone_offset == -1 ||
        cached_year != current_year ||
        cached_month != current_month ||
        cached_day != current_day) {

        // 计算时区偏移（只计算一次）
        if (cached_timezone_offset == -1) {
            struct tm tm_local;
            localtime_s(&tm_local, &now);
            time_t local_timestamp = mktime(&tm_local);

            struct tm tm_utc;
            gmtime_s(&tm_utc, &now);
            time_t utc_now = mktime(&tm_utc);

            cached_timezone_offset = static_cast<int>(difftime(local_timestamp, utc_now));
        }

        // 计算当天0点的时间戳
        struct tm tm_base = {0};
        tm_base.tm_year = current_year - 1900;
        tm_base.tm_mon = current_month - 1;
        tm_base.tm_mday = current_day;
        tm_base.tm_hour = 0;
        tm_base.tm_min = 0;
        tm_base.tm_sec = 0;
        tm_base.tm_isdst = -1;

        cached_date_base = mktime(&tm_base);
        cached_year = current_year;
        cached_month = current_month;
        cached_day = current_day;
    }

    // 快速计算时间戳：基准时间 + 当天秒数
    double day_seconds = hour * 3600.0 + minute * 60.0 + second;
    double utc_time = static_cast<double>(cached_date_base) + day_seconds;
    double local_time = utc_time + cached_timezone_offset;

    // 格式化UTC日期（使用缓存的日期）
    char date_buf[20];
    sprintf(date_buf, "%04d-%02d-%02d", current_year, current_month, current_day);
    std::string utc_date = date_buf;

    // 保存时间信息
    gnss.time = local_time;
    gnss.UtcTime = utc_time;
    gnss.UtcDate = utc_date;

    return true;
}


bool GnssSerialLoader::parseNMEA(const std::string& nmea) {
    // 快速检查：最小长度和起始字符
    if (nmea.length() < 7 || nmea[0] != '$') {
        return false;
    }

    // 使用字符数组比较，避免substr
    const char* msg = nmea.c_str();

    // 优先解析GGA消息（最重要：位置、高度、质量）
    if ((msg[1] == 'G' && msg[2] == 'P' && msg[3] == 'G' && msg[4] == 'G' && msg[5] == 'A') ||
        (msg[1] == 'G' && msg[2] == 'N' && msg[3] == 'G' && msg[4] == 'G' && msg[5] == 'A')) {

        std::vector<std::string> fields = split(nmea, ',');
        if (fields.size() >= 15) {
            // 解析UTC时间
            parseTime(fields[1], gnss_);

            // 解析纬度
            double lat_deg, lat_min, lat_sec;
            gnss_.blh[0] = convert_dm_to_dec(fields[2], fields[3], lat_deg, lat_min, lat_sec);
            gnss_.min[0] = lat_min;
            gnss_.sec[0] = lat_sec;

            // 解析经度
            double lon_deg, lon_min, lon_sec;
            gnss_.blh[1] = convert_dm_to_dec(fields[4], fields[5], lon_deg, lon_min, lon_sec);
            gnss_.min[1] = lon_min;
            gnss_.sec[1] = lon_sec;

            // 解析高度
            gnss_.blh[2] = safe_stod(fields[9]);

            // 解析quality
            gnss_.quality = safe_stoi(fields[6]);
            gnss_.isvalid = (gnss_.quality >= 1);

            // 解析used_sv
            gnss_.used_sv = safe_stoi(fields[7]);

            // 解析HDOP
            gnss_.hdop = safe_stod(fields[8]);

            return true; // GGA是完整数据，立即返回
        }
    }

    // 次优先：RMC消息（速度和航向）
    if ((msg[1] == 'G' && msg[2] == 'P' && msg[3] == 'R' && msg[4] == 'M' && msg[5] == 'C') ||
        (msg[1] == 'G' && msg[2] == 'N' && msg[3] == 'R' && msg[4] == 'M' && msg[5] == 'C')) {

        std::vector<std::string> fields = split(nmea, ',');
        if (fields.size() >= 9 && !fields[7].empty() && !fields[8].empty()) {
            // 始终解析速度数据，确保实时更新
            double speed_knots = safe_stod(fields[7]);
            double course_deg = safe_stod(fields[8]);
            double speed_mps = speed_knots * 0.514444;
            double course_rad = course_deg * M_PI / 180.0;

            gnss_.vel[0] = speed_mps * cos(course_rad);
            gnss_.vel[1] = speed_mps * sin(course_rad);
            gnss_.vel[2] = 0.0;
            gnss_.magnetic_heading = course_deg;
        }
        return false;
    }

    // ZDA消息（日期信息）- 优先级较高，因为影响时间计算
    if ((msg[1] == 'G' && msg[2] == 'N' && msg[3] == 'Z' && msg[4] == 'D' && msg[5] == 'A') ||
        (msg[1] == 'G' && msg[2] == 'P' && msg[3] == 'Z' && msg[4] == 'D' && msg[5] == 'A')) {

        if (!has_date_) { // 只在未设置日期时解析
            std::vector<std::string> fields = split(nmea, ',');
            if (fields.size() >= 7 && !fields[2].empty() && !fields[3].empty() && !fields[4].empty()) {
                day_ = safe_stoi(fields[2]);
                month_ = safe_stoi(fields[3]);
                year_ = safe_stoi(fields[4]);
                has_date_ = true;
            }
        }
        return false;
    }

    // GSA消息（DOP值）- 只在HDOP未设置或为0时解析
    if (gnss_.hdop == 0.0 || gnss_.pdop == 0.0) {
        if ((msg[1] == 'G' && msg[2] == 'N' && msg[3] == 'G' && msg[4] == 'S' && msg[5] == 'A') ||
            (msg[1] == 'G' && msg[2] == 'P' && msg[3] == 'G' && msg[4] == 'S' && msg[5] == 'A')) {

            std::vector<std::string> fields = split(nmea, ',');
            if (fields.size() >= 18) {
                gnss_.pdop = safe_stod(fields[15]);
                gnss_.hdop = safe_stod(fields[16]);
                gnss_.vdop = safe_stod(fields[17]);
            }
            return false;
        }
    }

    // VTG消息（地面速度）- 只在ground_speed未设置时解析
    if (gnss_.ground_speed == 0.0) {
        if ((msg[1] == 'G' && msg[2] == 'P' && msg[3] == 'V' && msg[4] == 'T' && msg[5] == 'G') ||
            (msg[1] == 'G' && msg[2] == 'N' && msg[3] == 'V' && msg[4] == 'T' && msg[5] == 'G')) {

            std::vector<std::string> fields = split(nmea, ',');
            if (fields.size() >= 10) {
                if (!fields[7].empty()) {
                    gnss_.ground_speed = safe_stod(fields[7]) / 3.6;
                } else if (!fields[5].empty()) {
                    gnss_.ground_speed = safe_stod(fields[5]) * 0.514444;
                }

                if (!fields[1].empty()) {
                    gnss_.true_heading = safe_stod(fields[1]);
                }
                if (!fields[3].empty() && gnss_.magnetic_heading == 0.0) {
                    gnss_.magnetic_heading = safe_stod(fields[3]);
                }
            }
            return false;
        }
    }

    // GSV消息（可见卫星数）- 只解析第一条消息
    if ((msg[1] == 'G' && msg[2] == 'P' && msg[3] == 'G' && msg[4] == 'S' && msg[5] == 'V') ||
        (msg[1] == 'B' && msg[2] == 'D' && msg[3] == 'G' && msg[4] == 'S' && msg[5] == 'V')) {

        // 快速检查是否是第一条消息（避免split）
        size_t comma1 = nmea.find(',');
        if (comma1 != std::string::npos) {
            size_t comma2 = nmea.find(',', comma1 + 1);
            if (comma2 != std::string::npos) {
                size_t comma3 = nmea.find(',', comma2 + 1);
                if (comma3 != std::string::npos) {
                    // 检查第二个字段是否为'1'
                    if (nmea[comma2 + 1] == '1' && (nmea[comma2 + 2] == ',' || nmea[comma2 + 2] == '\r')) {
                        std::vector<std::string> fields = split(nmea, ',');
                        if (fields.size() >= 4) {
                            int visible_sv = safe_stoi(fields[3]);
                            if (msg[1] == 'G' && msg[2] == 'P') {
                                gnss_.visible_sv = visible_sv;
                            } else if (msg[1] == 'B' && msg[2] == 'D') {
                                gnss_.visible_sv += visible_sv;
                            }
                        }
                    }
                }
            }
        }
        return false;
    }

    // GST消息（误差统计）- 低优先级，只在需要时解析
    if (gnss_.sigma_lat_gst == 0.0) {
        if ((msg[1] == 'G' && msg[2] == 'P' && msg[3] == 'G' && msg[4] == 'S' && msg[5] == 'T') ||
            (msg[1] == 'G' && msg[2] == 'N' && msg[3] == 'G' && msg[4] == 'S' && msg[5] == 'T')) {

            std::vector<std::string> fields = split(nmea, ',');
            if (fields.size() >= 9) {
                gnss_.sigma_lat_gst = safe_stod(fields[2]);
                gnss_.sigma_lon_gst = safe_stod(fields[3]);
                gnss_.sigma_alt_gst = safe_stod(fields[4]);
                gnss_.pr_rms = safe_stod(fields[5]);
            }
            return false;
        }
    }

    // 忽略其他消息类型（GLL, DHV等），减少不必要的解析
    return false;
}

std::vector<std::string> GnssSerialLoader::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    tokens.reserve(20); // 预分配空间，NMEA消息通常有10-20个字段

    size_t start = 0;
    size_t end = str.find(delimiter);

    while (end != std::string::npos) {
        tokens.emplace_back(str.substr(start, end - start));
        start = end + 1;
        end = str.find(delimiter, start);
    }

    tokens.emplace_back(str.substr(start));
    return tokens;
}


