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

#include "common/time_sync.h"

#include <algorithm>
#include <chrono>

/**
 * @brief 获取当前本地时间（秒）
 * @return 当前本地时间，单位：秒
 */
double TimeSync::getLocalTime() {
    // 使用系统时钟获取当前时间
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    
    // 转换为秒
    double seconds = std::chrono::duration<double>(duration).count();
    
    return seconds;
}

/**
 * @brief 计算滑动窗口平均值
 * @param history 历史数据数组
 * @param count 历史数据数量
 * @return 滑动窗口平均值
 */
double TimeSync::calculateAverage(const double* history, int count) const {
    if (count == 0) {
        return 0.0;
    }
    
    double sum = 0.0;
    for (int i = 0; i < count; i++) {
        sum += history[i];
    }
    
    return sum / count;
}

/**
 * @brief 使用IMU数据更新同步
 * @param imu_time IMU模块时间，单位：秒
 */
void TimeSync::updateIMU(double imu_time) {
    // 获取当前本地时间（数据接收时刻）
    double local_time = getLocalTime();
    
    // 计算当前差值：IMU时间 - 本地时间
    double current_delta = imu_time - local_time;
    
    // 保存到历史记录
    imu_offset_history_[imu_history_index_] = current_delta;
    imu_history_index_ = (imu_history_index_ + 1) % WINDOW_SIZE;
    imu_history_count_ = std::min(imu_history_count_ + 1, WINDOW_SIZE);
    
    // 更新IMU时间记录
    imu_last_time_ = imu_time;
    
    // 更新同步状态
    if (sync_status_ == UNSYNCED) {
        sync_status_ = SYNCING;
    }
    
    LOG_DEBUG("IMU sync update: imu=%.3f, local=%.3f, delta=%.6f, avg_delta=%.6f",
             imu_time, local_time, current_delta, getAverageIMUOffset());
}

/**
 * @brief 使用GNSS时间更新同步
 * @param gnss_time GNSS卫星时间，单位：秒
 */
void TimeSync::updateGNSS(double gnss_time) {
    // 获取当前本地时间（数据接收时刻）
    double local_time = getLocalTime();
    
    // 计算当前差值：GNSS时间 - 本地时间
    double current_delta = gnss_time - local_time;
    
    // 保存到历史记录
    gnss_offset_history_[gnss_history_index_] = current_delta;
    gnss_history_index_ = (gnss_history_index_ + 1) % WINDOW_SIZE;
    gnss_history_count_ = std::min(gnss_history_count_ + 1, WINDOW_SIZE);
    
    // 更新GNSS时间记录
    gnss_last_time_ = gnss_time;
    
    // 更新同步状态
    sync_status_ = SYNCED;
    
    LOG_DEBUG("GNSS sync update: gnss=%.3f, local=%.3f, delta=%.6f, avg_delta=%.6f",
             gnss_time, local_time, current_delta, getAverageGNSSOffset());
}

/**
 * @brief 获取当前同步时间（优先使用GNSS时间，否则使用本地时间）
 * @return 当前同步时间，单位：秒
 */
double TimeSync::getSyncedTime() const {
    if (sync_status_ == SYNCED && gnss_history_count_ > 0) {
        // 有GNSS数据时，使用GNSS时间
        double local_time = getLocalTime();
        return local_time + getAverageGNSSOffset();
    } else {
        // 无GNSS数据时，使用本地时间
        double current_time = getLocalTime();
        if (current_time - last_warn_time_ > 60.0) { // 每分钟只输出一次警告
            LOG_WARN("No GNSS data, using local time");
            last_warn_time_ = current_time;
        }
        return current_time;
    }
}

/**
 * @brief 为IMU数据分配同步时间戳
 * @param imu_time IMU模块时间，单位：秒
 * @return IMU数据的同步时间戳，单位：秒
 */
double TimeSync::assignIMUTime(double imu_time) const {
    double synced_time;
    
    if (sync_status_ == SYNCED && gnss_history_count_ > 0) {
        // 有GNSS数据时，使用GNSS时间校正IMU时间
        // 计算公式：synced_time = imu_time - (IMU偏移) + (GNSS偏移)
        synced_time = imu_time - getAverageIMUOffset() + getAverageGNSSOffset();
        
        // 确保时间单调递增，添加一个小的阈值来避免频繁的警告
        const double time_threshold = 0.001; // 1ms阈值
        if (synced_time < gnss_last_time_ - time_threshold) {
            LOG_WARN("IMU time rollback detected: synced=%.3f < last_gnss=%.3f, using last_gnss",
                    synced_time, gnss_last_time_);
            return gnss_last_time_ + 0.0001; // 微小增量，确保单调
        } else if (synced_time < gnss_last_time_) {
            // 时间差很小，直接使用GNSS最后时间，不输出警告
            return gnss_last_time_;
        }
    } else {
        // 无GNSS数据时，使用本地时间校正IMU时间
        // 计算公式：synced_time = imu_time - (IMU偏移)
        synced_time = imu_time - getAverageIMUOffset();
        
        // 确保时间单调递增，添加一个小的阈值来避免频繁的警告
        const double time_threshold = 0.001; // 1ms阈值
        if (synced_time < imu_last_time_ - time_threshold) {
            LOG_WARN("IMU time rollback detected: synced=%.3f < last_imu=%.3f, using last_imu",
                    synced_time, imu_last_time_);
            return imu_last_time_ + 0.0001; // 微小增量，确保单调
        } else if (synced_time < imu_last_time_) {
            // 时间差很小，直接使用IMU最后时间，不输出警告
            return imu_last_time_;
        }
    }
    
    return synced_time;
}

/**
 * @brief 重置时间同步
 */
void TimeSync::reset() {
    sync_status_ = UNSYNCED;
    imu_last_time_ = 0.0;
    gnss_last_time_ = 0.0;
    last_warn_time_ = 0.0;
    imu_history_count_ = 0;
    gnss_history_count_ = 0;
    imu_history_index_ = 0;
    gnss_history_index_ = 0;
    
    // 清空历史记录
    for (int i = 0; i < WINDOW_SIZE; i++) {
        imu_offset_history_[i] = 0.0;
        gnss_offset_history_[i] = 0.0;
    }
    
    LOG_INFO("Time sync reset");
}