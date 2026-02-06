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

#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <string>
#include <ctime>
#include <fstream>
#include <chrono>
#include <sstream>

// 前向声明LoggerSignalEmitter类
class LoggerSignalEmitter;

class Logger {
public:
    enum LogLevel {
        DEBUG = 0,
        INFO,
        WARN,
        ERROR,
        FATAL
    };

    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    void setLevel(LogLevel level) {
        level_ = level;
    }

    // 定义日志消息处理器类型
    typedef void (*LogMessageHandler)(const std::string& message);

    // 设置日志消息处理器
    void setLogMessageHandler(LogMessageHandler handler) {
        logMessageHandler_ = handler;
    }

    void setLogFile(const std::string& filename) {
        // 先关闭之前可能打开的日志文件
        if (log_file_.is_open()) {
            // 关闭前先刷新缓冲区
            flushBuffer();
            log_file_.close();
        }
        
        // 尝试打开新的日志文件
        log_file_.open(filename, std::ios::app);
        // 不输出到控制台，只写入日志文件
        if (log_file_.is_open()) {
            // 写入日志文件
            std::string time_str = getCurrentTime();
            std::string log_entry = "[" + time_str + "] [INFO] Logger: Log file opened: " + filename + "\n";
            
            // 直接写入文件，不经过缓冲区
            log_file_ << log_entry;
            log_file_.flush(); // 立即刷新缓冲区
            
            // 调用日志消息处理器
            if (logMessageHandler_) {
                logMessageHandler_(log_entry);
            }
        } else {
            // 日志文件打开失败，无法写入日志，静默处理
        }
    }

    void closeLogFile() {
        if (log_file_.is_open()) {
            // 关闭前先刷新缓冲区
            flushBuffer();
            log_file_.close();
        }
    }

    template<typename... Args>
    void debug(const char* function, const char* format, Args&&... args) {
        log(DEBUG, function, format, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void info(const char* function, const char* format, Args&&... args) {
        log(INFO, function, format, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void warn(const char* function, const char* format, Args&&... args) {
        log(WARN, function, format, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void error(const char* function, const char* format, Args&&... args) {
        log(ERROR, function, format, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void fatal(const char* function, const char* format, Args&&... args) {
        log(FATAL, function, format, std::forward<Args>(args)...);
    }

private:
    static const size_t BUFFER_SIZE = 1024 * 10; // 10KB缓冲区
    
    Logger() : level_(INFO), buffer_size_(0), logMessageHandler_(nullptr) {
        // 初始化日志系统
    }

    ~Logger() {
        closeLogFile();
    }

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::string getCurrentTime() {
        std::time_t now = std::time(nullptr);
        std::tm tm_now;
        localtime_s(&tm_now, &now);
        
        // 获取毫秒部分
        auto now_ms = std::chrono::system_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now_ms.time_since_epoch()) % 1000;
        
        char buffer[30];
        // 格式化时间，包含毫秒，格式为：YYYY-MM-DD HH:MM:SS:SSS
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm_now);
        char buffer_with_ms[30];
        std::sprintf(buffer_with_ms, "%s:%03d", buffer, static_cast<int>(ms.count()));
        
        return std::string(buffer_with_ms);
    }

    std::string levelToString(LogLevel level) {
        switch (level) {
            case DEBUG:
                return "DEBUG";
            case INFO:
                return "INFO";
            case WARN:
                return "WARN";
            case ERROR:
                return "ERROR";
            case FATAL:
                return "FATAL";
            default:
                return "UNKNOWN";
        }
    }

    void flushBuffer() {
        if (log_file_.is_open() && buffer_size_ > 0) {
            // 将缓冲区内容写入文件
            log_file_ << buffer_.str();
            log_file_.flush(); // 立即刷新缓冲区
            
            // 清空缓冲区
            buffer_.str("");
            buffer_.clear();
            buffer_size_ = 0;
        }
    }

    template<typename... Args>
    void log(LogLevel level, const char* function, const char* format, Args&&... args) {
        if (level < level_) {
            return;
        }

        std::string time_str = getCurrentTime();
        std::string level_str = levelToString(level);

        // 构建日志消息
        char buffer[1024];
        std::snprintf(buffer, sizeof(buffer), format, std::forward<Args>(args)...);
        std::string message(buffer);

        // 构建完整的日志条目
        std::string log_entry = "[" + time_str + "] [" + level_str + "] [" + function + "] " + message + "\n";

        // 只输出到文件，不输出到控制台，即使日志文件未打开也不输出
        if (log_file_.is_open()) {
            // 将日志条目添加到缓冲区
            buffer_ << log_entry;
            buffer_size_ += log_entry.size();
            
            // 检查缓冲区大小，如果超过阈值就刷新
            if (buffer_size_ >= BUFFER_SIZE) {
                flushBuffer();
            }
        }

        // 调用日志消息处理器，实时显示日志
        if (logMessageHandler_) {
            logMessageHandler_(log_entry);
        }
    }

    LogLevel level_;
    std::ofstream log_file_;
    std::stringstream buffer_; // 日志缓冲区
    size_t buffer_size_; // 缓冲区当前大小
    LogMessageHandler logMessageHandler_; // 日志消息处理器
};

// 日志宏定义
#define LOG_DEBUG(format, ...) Logger::getInstance().debug(__FUNCTION__, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...) Logger::getInstance().info(__FUNCTION__, format, ##__VA_ARGS__)
#define LOG_WARN(format, ...) Logger::getInstance().warn(__FUNCTION__, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) Logger::getInstance().error(__FUNCTION__, format, ##__VA_ARGS__)
#define LOG_FATAL(format, ...) Logger::getInstance().fatal(__FUNCTION__, format, ##__VA_ARGS__)

#endif // LOGGER_H
