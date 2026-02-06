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

#ifndef TIME_SYNC_H
#define TIME_SYNC_H

#include <chrono>

#include "common/logger.h"

class TimeSync {
public:
    // 时间同步状态
    enum SyncStatus {
        UNSYNCED = 0,   // 未同步
        SYNCING,        // 同步中
        SYNCED          // 已同步
    };

    TimeSync() : sync_status_(UNSYNCED), 
                 imu_last_time_(0.0), gnss_last_time_(0.0), 
                 last_warn_time_(0.0) {
        // 初始化历史记录
        for (int i = 0; i < WINDOW_SIZE; i++) {
            imu_offset_history_[i] = 0.0;
            gnss_offset_history_[i] = 0.0;
        }
    }

    ~TimeSync() = default;

    // 禁止拷贝和移动
    TimeSync(const TimeSync&) = delete;
    TimeSync& operator=(const TimeSync&) = delete;
    TimeSync(TimeSync&&) = delete;
    TimeSync& operator=(TimeSync&&) = delete;

    // 获取单例实例
    static TimeSync& getInstance() {
        static TimeSync instance;
        return instance;
    }

    // 使用IMU数据更新同步
    void updateIMU(double imu_time);

    // 使用GNSS时间更新同步
    void updateGNSS(double gnss_time);

    // 获取当前同步时间（优先使用GNSS时间，否则使用本地时间）
    double getSyncedTime() const;

    // 为IMU数据分配同步时间戳
    double assignIMUTime(double imu_time) const;

    // 获取同步状态
    SyncStatus getStatus() const {
        return sync_status_;
    }

    // 重置时间同步
    void reset();

    // 获取本地时间（秒）
    static double getLocalTime();

private:
    // 同步状态
    SyncStatus sync_status_;                 // 同步状态
    
    // 时间记录
    double imu_last_time_;                   // 上次IMU时间
    double gnss_last_time_;                  // 上次GNSS时间
    mutable double last_warn_time_;          // 上次警告输出时间
    
    // 滑动窗口参数
    static constexpr int WINDOW_SIZE = 10;   // 滑动窗口大小
    
    // 差值历史记录
    double imu_offset_history_[WINDOW_SIZE];  // IMU偏移历史 (IMU时间 - 本地时间)
    double gnss_offset_history_[WINDOW_SIZE]; // GNSS偏移历史 (GNSS时间 - 本地时间)
    int imu_history_index_ = 0;              // IMU历史索引
    int gnss_history_index_ = 0;             // GNSS历史索引
    int imu_history_count_ = 0;              // IMU历史计数
    int gnss_history_count_ = 0;             // GNSS历史计数
    
    // 计算滑动窗口平均值
    double calculateAverage(const double* history, int count) const;
    
    // 获取当前平均差值
    double getAverageIMUOffset() const {
        return calculateAverage(imu_offset_history_, imu_history_count_);
    }
    
    double getAverageGNSSOffset() const {
        return calculateAverage(gnss_offset_history_, gnss_history_count_);
    }
};

// 全局时间同步宏定义
#define TIME_SYNC TimeSync::getInstance()
#define GET_SYNCED_TIME() TIME_SYNC.getSyncedTime()
#define ASSIGN_IMU_TIME(imu_time) TIME_SYNC.assignIMUTime(imu_time)

#endif // TIME_SYNC_H