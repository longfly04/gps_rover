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

#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <string>

#include <QSerialPort>
#include <QTimer>

class SerialPort {
public:
    /**
     * @brief 构造函数，初始化成员变量
     */
    SerialPort();
    /**
     * @brief 析构函数，关闭串口
     */
    ~SerialPort();

    /**
     * @brief 打开串口
     * @param port 串口设备路径
     * @param baudrate 波特率
     * @return 是否打开成功
     */
    bool open(const std::string& port, int baudrate);
    /**
     * @brief 关闭串口
     */
    void close();
    /**
     * @brief 检查串口是否打开
     * @return 是否打开
     */
    bool isOpen() const;
    /**
     * @brief 读取串口数据
     * @param buffer 数据缓冲区
     * @param size 缓冲区大小
     * @return 读取的字节数，-1 表示失败
     */
    int read(char* buffer, int size);
    /**
     * @brief 写入串口数据
     * @param buffer 数据缓冲区
     * @param size 数据大小
     * @return 写入的字节数，-1 表示失败
     */
    int write(const char* buffer, int size);
    /**
     * @brief 获取可用的字节数
     * @return 可用字节数
     */
    qint64 bytesAvailable() const;
    /**
     * @brief 清空接收缓冲区
     */
    void clearBuffer();
    /**
     * @brief 启动看门狗定时器
     * @param interval 定时器间隔（毫秒）
     * @param callback 超时回调函数
     */
    void startWatchdog(int interval, std::function<void()> callback);
    /**
     * @brief 停止看门狗定时器
     */
    void stopWatchdog();

private:
    QSerialPort* serial_port_;
    bool is_open_;
    QTimer* watchdog_timer_;
    std::function<void()> watchdog_callback_;
};

#endif // SERIALPORT_H