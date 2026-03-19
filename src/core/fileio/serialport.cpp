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

#include "serialport.h"

#include <QDebug>
#include <functional>

SerialPort::SerialPort()
    : serial_port_(nullptr), is_open_(false), watchdog_timer_(nullptr) {
}

SerialPort::~SerialPort() {
    stopWatchdog();
    close();
}

bool SerialPort::open(const std::string& port, int baudrate) {
    if (is_open_) {
        close();
    }

    serial_port_ = new QSerialPort();

    // 处理端口名称
    std::string processedPort = port;
    
    // 移除可能的后缀，如"s"
    if (processedPort.find("COM") == 0) {
        // 提取COM后面的数字部分
        std::string portNumber = processedPort.substr(3);
        // 只保留数字
        portNumber.erase(std::remove_if(portNumber.begin(), portNumber.end(), [](char c) { return !std::isdigit(c); }), portNumber.end());
        if (!portNumber.empty()) {
            // 重新构建端口名称
            processedPort = "COM" + portNumber;
        }
    }

    serial_port_->setPortName(QString::fromStdString(processedPort));

    serial_port_->setBaudRate(baudrate);

    serial_port_->setDataBits(QSerialPort::Data8);
    serial_port_->setParity(QSerialPort::NoParity);
    serial_port_->setStopBits(QSerialPort::OneStop);
    serial_port_->setFlowControl(QSerialPort::NoFlowControl);
    
    // 【关键优化】设置读取缓冲区大小
    // 设置为 8KB，避免过大导致数据积压
    serial_port_->setReadBufferSize(8 * 1024);

    // 打开串口，使用默认的缓冲模式
    if (serial_port_->open(QIODevice::ReadWrite)) {
        is_open_ = true;
        qDebug() << "SerialPort:" << processedPort << "opened successfully with baudrate" << baudrate;
        return true;
    } else {
        qDebug() << "SerialPort: Failed to open" << processedPort
                 << "Error:" << serial_port_->errorString();
        delete serial_port_;
        serial_port_ = nullptr;
        is_open_ = false;
        return false;
    }
}

void SerialPort::close() {
    if (serial_port_ != nullptr) {
        if (serial_port_->isOpen()) {
            serial_port_->close();
        }
        delete serial_port_;
        serial_port_ = nullptr;
    }
    is_open_ = false;
}

bool SerialPort::isOpen() const {
    return is_open_ && serial_port_ != nullptr && serial_port_->isOpen();
}

int SerialPort::read(char* buffer, int size) {
    if (!isOpen()) {
        return -1;
    }

    // ✅ 优化：使用 waitForReadyRead 避免空转，但设置很短的超时
    // 如果没有数据，最多等待1ms，避免CPU空转
    if (serial_port_->bytesAvailable() == 0) {
        if (!serial_port_->waitForReadyRead(1)) {
            return 0; // 超时或没有数据
        }
    }

    // 读取可用数据（不超过缓冲区大小）
    qint64 available = serial_port_->bytesAvailable();
    if (available == 0) {
        return 0;
    }

    qint64 bytes_to_read = qMin(static_cast<qint64>(size), available);
    qint64 bytes_read = serial_port_->read(buffer, bytes_to_read);

    if (bytes_read < 0) {
        qDebug() << "SerialPort: Read error" << serial_port_->errorString();
        return -1;
    }

    return static_cast<int>(bytes_read);
}

int SerialPort::write(const char* buffer, int size) {
    if (!isOpen()) {
        return -1;
    }

    qint64 bytes_written = serial_port_->write(buffer, size);
    if (bytes_written < 0) {
        qDebug() << "SerialPort: Write error" << serial_port_->errorString();
        return -1;
    }

    // 等待数据写入完成
    if (!serial_port_->waitForBytesWritten(1000)) {
        qDebug() << "SerialPort: Write timeout";
        return -1;
    }

    return static_cast<int>(bytes_written);
}

qint64 SerialPort::bytesAvailable() const {
    if (!isOpen()) {
        return 0;
    }
    return serial_port_->bytesAvailable();
}

void SerialPort::clearBuffer() {
    if (!isOpen()) {
        return;
    }
    // 清空接收缓冲区中的所有数据
    serial_port_->clear(QSerialPort::Input);
}

void SerialPort::startWatchdog(int interval, std::function<void()> callback) {
    stopWatchdog(); // 先停止之前的定时器
    
    watchdog_callback_ = callback;
    watchdog_timer_ = new QTimer();
    
    QObject::connect(watchdog_timer_, &QTimer::timeout, [this]() {
        if (watchdog_callback_) {
            watchdog_callback_();
        }
    });
    
    watchdog_timer_->start(interval);
    qDebug() << "SerialPort: Watchdog timer started with interval" << interval << "ms";
}

void SerialPort::stopWatchdog() {
    if (watchdog_timer_) {
        watchdog_timer_->stop();
        delete watchdog_timer_;
        watchdog_timer_ = nullptr;
        watchdog_callback_ = nullptr;
        qDebug() << "SerialPort: Watchdog timer stopped";
    }
}
