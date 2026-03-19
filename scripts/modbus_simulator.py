#!/usr/bin/env python3
"""Modbus TCP模拟从站 - 伺服电机模拟器

该脚本实现了一个Modbus TCP从站，模拟伺服电机的控制功能。
支持速度控制、位置控制、状态监控等典型伺服电机功能。
"""

import asyncio
import logging
import struct
from typing import Optional
from pymodbus import FramerType
from pymodbus.server import StartAsyncTcpServer
from pymodbus.datastore import ModbusSequentialDataBlock, ModbusDeviceContext, ModbusServerContext


class ServoMotorSimulator:
    """伺服电机模拟器类
    
    模拟伺服电机的各种控制参数和状态反馈
    """
    
    # 寄存器地址定义
    ADDR_CONTROL_WORD = 0x0000  # 控制字 (RW)
    ADDR_STATUS_WORD = 0x0001  # 状态字 (R)
    ADDR_CONTROL_MODE = 0x0002  # 控制模式 (RW)
    ADDR_ERROR_CODE = 0x0003  # 错误码 (R)
    ADDR_WARNING_CODE = 0x0004  # 警告码 (R)
    
    ADDR_COMMAND_SPEED = 0x0010  # 命令速度 (rpm) (RW) - FLOAT32
    ADDR_ACTUAL_SPEED = 0x0012  # 实际速度 (rpm) (R) - FLOAT32
    ADDR_SPEED_LIMIT = 0x0014  # 速度限制 (rpm) (RW) - FLOAT32
    
    ADDR_COMMAND_POSITION = 0x0020  # 命令位置 (脉冲) (RW) - FLOAT32
    ADDR_ACTUAL_POSITION = 0x0022  # 实际位置 (脉冲) (R) - FLOAT32
    ADDR_POSITION_LIMIT = 0x0024  # 位置限制 (脉冲) (RW) - FLOAT32
    
    ADDR_COMMAND_TORQUE = 0x0030  # 命令转矩 (Nm) (RW) - FLOAT32
    ADDR_ACTUAL_TORQUE = 0x0032  # 实际转矩 (Nm) (R) - FLOAT32
    ADDR_TORQUE_LIMIT = 0x0034  # 转矩限制 (Nm) (RW) - FLOAT32
    
    ADDR_CURRENT = 0x0040  # 电流 (A) (R) - FLOAT32
    ADDR_TEMPERATURE = 0x0042  # 温度 (°C) (R) - FLOAT32
    
    def __init__(self):
        """初始化伺服电机参数"""
        self.is_running = False
        self.direction = 1  # 1: 正向, -1: 反向
        self.command_speed = 0.0  # 命令速度 (rpm)
        self.actual_speed = 0.0  # 实际速度 (rpm)
        self.command_position = 0.0  # 命令位置 (脉冲)
        self.actual_position = 0.0  # 实际位置 (脉冲)
        self.command_torque = 0.0  # 命令转矩 (Nm)
        self.actual_torque = 0.0  # 实际转矩 (Nm)
        self.current = 0.0  # 电流 (A)
        self.temperature = 25.0  # 温度 (°C)
        self.error_code = 0  # 错误码
        self.warning_code = 0  # 警告码
        self.control_mode = 0  # 控制模式: 0:位置, 1:速度, 2:转矩
        self.position_limit = 100000.0  # 位置限制 (脉冲)
        self.speed_limit = 3000.0  # 速度限制 (rpm)
        self.torque_limit = 100.0  # 转矩限制 (Nm)
        
        self._logger = logging.getLogger(__name__)
    
    def update(self):
        """更新电机状态
        
        根据当前命令更新实际速度、位置等状态
        """
        if self.is_running:
            # 速度平滑过渡（模拟电机惯性）
            speed_diff = self.command_speed - self.actual_speed
            self.actual_speed += speed_diff * 0.1
            
            # 限制最大速度
            if abs(self.actual_speed) > self.speed_limit:
                self.actual_speed = self.speed_limit * (1 if self.actual_speed > 0 else -1)
            
            # 更新位置
            self.actual_position += self.actual_speed * self.direction * 0.0167  # 假设采样周期60Hz
            
            # 限制位置
            if abs(self.actual_position) > self.position_limit:
                self.actual_position = self.position_limit * (1 if self.actual_position > 0 else -1)
                self.error_code = 0x0101  # 位置超限错误
            
            # 模拟转矩和电流
            self.actual_torque = self.command_torque
            self.current = abs(self.actual_torque) * 0.5  # 简单的转矩-电流关系
            
            # 模拟温度变化
            target_temp = 25.0 + abs(self.current) * 2.0
            self.temperature += (target_temp - self.temperature) * 0.01
            
            # 温度过高警告
            if self.temperature > 80.0:
                self.warning_code = 0x0201  # 温度过高警告
                if self.temperature > 90.0:
                    self.error_code = 0x0202  # 温度过高错误
                    self.is_running = False
        else:
            # 停止时速度逐渐归零
            self.actual_speed *= 0.9
            if abs(self.actual_speed) < 0.1:
                self.actual_speed = 0.0
            
            # 转矩和电流归零
            self.actual_torque *= 0.9
            self.current *= 0.9
            
            # 温度逐渐冷却
            self.temperature += (25.0 - self.temperature) * 0.005
    
    def get_status_word(self) -> int:
        """获取状态字
        
        Returns:
            int: 状态字，各位含义：
                bit0: 准备好
                bit1: 运行中
                bit2: 报警
                bit3: 错误
                bit4: 到达目标位置
                bit5: 到达目标速度
                bit6-15: 保留
        """
        status = 0
        status |= 0x0001  # 准备好
        if self.is_running:
            status |= 0x0002  # 运行中
        if self.warning_code != 0:
            status |= 0x0004  # 报警
        if self.error_code != 0:
            status |= 0x0008  # 错误
        return status
    
    def set_control_word(self, value: int):
        """设置控制字
        
        Args:
            value: 控制字，各位含义：
                bit0: 启动/停止 (1:启动, 0:停止)
                bit1: 方向 (1:反向, 0:正向)
                bit2: 复位错误
                bit3-15: 保留
        """
        if value & 0x0001:
            self.is_running = True
        else:
            self.is_running = False
        
        if value & 0x0002:
            self.direction = -1
        else:
            self.direction = 1
        
        if value & 0x0004:
            self.error_code = 0
            self.warning_code = 0
    
    def float32_to_registers(self, value: float) -> list:
        """将float32转换为两个16位寄存器
        
        Args:
            value: float32值
            
        Returns:
            list: 包含两个16位寄存器值的列表
        """
        packed = struct.pack(">f", value)
        return [struct.unpack(">H", packed[i:i+2])[0] for i in range(0, 4, 2)]
    
    def registers_to_float32(self, registers: list) -> float:
        """将两个16位寄存器转换为float32
        
        Args:
            registers: 包含两个16位寄存器值的列表
            
        Returns:
            float: float32值
        """
        packed = struct.pack(">HH", registers[0], registers[1])
        return struct.unpack(">f", packed)[0]


async def update_motor_task(motor: ServoMotorSimulator, hr_block: ModbusSequentialDataBlock):
    """定期更新电机状态的任务
    
    Args:
        motor: 伺服电机模拟器实例
        hr_block: 保持寄存器数据块
    """
    while True:
        motor.update()
        
        # 更新寄存器值
        # 状态和控制寄存器
        hr_block.setValues(motor.ADDR_STATUS_WORD, [motor.get_status_word()])
        hr_block.setValues(motor.ADDR_CONTROL_MODE, [motor.control_mode])
        hr_block.setValues(motor.ADDR_ERROR_CODE, [motor.error_code])
        hr_block.setValues(motor.ADDR_WARNING_CODE, [motor.warning_code])
        
        # 速度相关寄存器 (FLOAT32)
        speed_regs = motor.float32_to_registers(motor.command_speed)
        hr_block.setValues(motor.ADDR_COMMAND_SPEED, speed_regs)
        
        speed_actual_regs = motor.float32_to_registers(motor.actual_speed)
        hr_block.setValues(motor.ADDR_ACTUAL_SPEED, speed_actual_regs)
        
        speed_limit_regs = motor.float32_to_registers(motor.speed_limit)
        hr_block.setValues(motor.ADDR_SPEED_LIMIT, speed_limit_regs)
        
        # 位置相关寄存器 (FLOAT32)
        pos_regs = motor.float32_to_registers(motor.command_position)
        hr_block.setValues(motor.ADDR_COMMAND_POSITION, pos_regs)
        
        pos_actual_regs = motor.float32_to_registers(motor.actual_position)
        hr_block.setValues(motor.ADDR_ACTUAL_POSITION, pos_actual_regs)
        
        pos_limit_regs = motor.float32_to_registers(motor.position_limit)
        hr_block.setValues(motor.ADDR_POSITION_LIMIT, pos_limit_regs)
        
        # 转矩相关寄存器 (FLOAT32)
        torque_regs = motor.float32_to_registers(motor.command_torque)
        hr_block.setValues(motor.ADDR_COMMAND_TORQUE, torque_regs)
        
        torque_actual_regs = motor.float32_to_registers(motor.actual_torque)
        hr_block.setValues(motor.ADDR_ACTUAL_TORQUE, torque_actual_regs)
        
        torque_limit_regs = motor.float32_to_registers(motor.torque_limit)
        hr_block.setValues(motor.ADDR_TORQUE_LIMIT, torque_limit_regs)
        
        # 电流和温度寄存器 (FLOAT32)
        current_regs = motor.float32_to_registers(motor.current)
        hr_block.setValues(motor.ADDR_CURRENT, current_regs)
        
        temp_regs = motor.float32_to_registers(motor.temperature)
        hr_block.setValues(motor.ADDR_TEMPERATURE, temp_regs)
        
        await asyncio.sleep(0.016)  # 约60Hz的更新频率


async def main():
    """主函数"""
    logging.basicConfig(
        level=logging.INFO,
        format="%(asctime)s - %(name)s - %(levelname)s - %(message)s"
    )
    logger = logging.getLogger(__name__)
    
    logger.info("正在启动Modbus TCP伺服电机模拟器...")
    
    motor = ServoMotorSimulator()
    
    # 创建保持寄存器数据块，预分配0x0044个寄存器
    hr_block = ModbusSequentialDataBlock(0, [0] * 0x0044)
    
    # 创建设备上下文
    context = ModbusDeviceContext(hr=hr_block)
    
    # 创建服务器上下文
    server_context = ModbusServerContext(devices=context)
    
    logger.info("伺服电机参数:")
    logger.info(f"  速度限制: {motor.speed_limit} rpm")
    logger.info(f"  位置限制: {motor.position_limit} 脉冲")
    logger.info(f"  转矩限制: {motor.torque_limit} Nm")
    logger.info("")
    logger.info("寄存器地址映射:")
    logger.info(f"  0x0000: 控制字 (RW)")
    logger.info(f"  0x0001: 状态字 (R)")
    logger.info(f"  0x0002: 控制模式 (RW)")
    logger.info(f"  0x0003: 错误码 (R)")
    logger.info(f"  0x0004: 警告码 (R)")
    logger.info(f"  0x0010: 命令速度 (rpm) (RW) - FLOAT32")
    logger.info(f"  0x0012: 实际速度 (rpm) (R) - FLOAT32")
    logger.info(f"  0x0014: 速度限制 (rpm) (RW) - FLOAT32")
    logger.info(f"  0x0020: 命令位置 (脉冲) (RW) - FLOAT32")
    logger.info(f"  0x0022: 实际位置 (脉冲) (R) - FLOAT32")
    logger.info(f"  0x0024: 位置限制 (脉冲) (RW) - FLOAT32")
    logger.info(f"  0x0030: 命令转矩 (Nm) (RW) - FLOAT32")
    logger.info(f"  0x0032: 实际转矩 (Nm) (R) - FLOAT32")
    logger.info(f"  0x0034: 转矩限制 (Nm) (RW) - FLOAT32")
    logger.info(f"  0x0040: 电流 (A) (R) - FLOAT32")
    logger.info(f"  0x0042: 温度 (°C) (R) - FLOAT32")
    logger.info("")
    
    update_task = asyncio.create_task(update_motor_task(motor, hr_block))
    
    server_address = ("0.0.0.0", 5020)
    logger.info(f"Modbus TCP服务器启动，监听地址: {server_address[0]}:{server_address[1]}")
    
    try:
        await StartAsyncTcpServer(
            context=server_context,
            address=server_address,
            framer=FramerType.SOCKET,
        )
    except KeyboardInterrupt:
        logger.info("接收到中断信号，正在关闭服务器...")
        update_task.cancel()
        logger.info("服务器已关闭")


if __name__ == "__main__":
    asyncio.run(main())
