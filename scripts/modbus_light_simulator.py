#!/usr/bin/env python3
"""Modbus TCP模拟从站 - 显示灯控制器

该脚本实现了一个Modbus TCP从站，模拟显示灯的控制功能。
通过写入寄存器来控制显示灯的状态（0：断开，1：接通）。
"""

import argparse
import asyncio
import logging
import signal
import sys
from typing import Optional
from pymodbus import FramerType
from pymodbus.server import StartAsyncTcpServer
from pymodbus.datastore import ModbusSequentialDataBlock, ModbusDeviceContext, ModbusServerContext


class LightController:
    """显示灯控制器类

    通过Modbus寄存器控制显示灯的开关状态
    """

    def __init__(self, register_address: int = 0):
        """初始化显示灯控制器

        Args:
            register_address: 控制显示灯状态的寄存器地址（十进制）
        """
        self.register_address = register_address
        self.light_state = 0  # 0: 断开, 1: 接通

        self._logger = logging.getLogger(__name__)

    def set_light_state(self, state: int):
        """设置显示灯状态

        Args:
            state: 状态值，0表示断开，1表示接通
        """
        if state != 0 and state != 1:
            self._logger.warning(f"无效的灯状态值: {state}，请使用0或1")
            return

        old_state = self.light_state
        self.light_state = state

        if old_state != state:
            status_str = "接通" if state == 1 else "断开"
            self._logger.info(f"显示灯状态已更改: {status_str}")

    def get_light_state(self) -> int:
        """获取显示灯状态

        Returns:
            int: 灯状态，0表示断开，1表示接通
        """
        return self.light_state


async def update_light_task(light: LightController, hr_block: ModbusSequentialDataBlock):
    """定期更新显示灯状态的任务

    Args:
        light: 显示灯控制器实例
        hr_block: 保持寄存器数据块
    """
    while True:
        # 从寄存器读取当前灯状态
        try:
            registers = hr_block.getValues(light.register_address, 1)
            if registers:
                current_value = registers[0]
                # 确保值在有效范围内（0或1）
                if current_value not in (0, 1):
                    current_value = 1 if current_value != 0 else 0
                    hr_block.setValues(light.register_address, [current_value])

                # 更新灯状态
                if current_value != light.get_light_state():
                    light.set_light_state(current_value)

            # 将当前灯状态写入寄存器（保持同步）
            hr_block.setValues(light.register_address, [light.get_light_state()])

        except Exception as e:
            logging.getLogger(__name__).error(f"读取寄存器错误: {e}")

        await asyncio.sleep(0.1)  # 100ms更新频率


async def main():
    """主函数"""
    parser = argparse.ArgumentParser(
        description="Modbus TCP显示灯控制器模拟器",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
示例用法:
  python modbus_light_simulator.py                           # 使用默认寄存器地址0
  python modbus_light_simulator.py --address 100            # 使用寄存器地址100
  python modbus_light_simulator.py -a 256 --port 5021       # 使用地址256，端口5021
        """
    )

    parser.add_argument(
        "-a", "--address",
        type=int,
        default=17,
        help="控制显示灯的寄存器地址（十进制，默认: 0）"
    )

    parser.add_argument(
        "-p", "--port",
        type=int,
        default=5020,
        help="Modbus TCP服务器端口（默认: 5020）"
    )

    parser.add_argument(
        "--host",
        type=str,
        default="0.0.0.0",
        help="Modbus TCP服务器监听地址（默认: 0.0.0.0）"
    )

    parser.add_argument(
        "-v", "--verbose",
        action="store_true",
        help="启用详细日志输出"
    )

    args = parser.parse_args()

    log_level = logging.DEBUG if args.verbose else logging.INFO
    logging.basicConfig(
        level=log_level,
        format="%(asctime)s - %(name)s - %(levelname)s - %(message)s"
    )
    logger = logging.getLogger(__name__)

    if args.address < 0:
        logger.error("寄存器地址不能为负数")
        sys.exit(1)

    logger.info("=" * 50)
    logger.info("Modbus TCP 显示灯控制器模拟器")
    logger.info("=" * 50)

    light = LightController(register_address=args.address)

    register_count = args.address + 10
    hr_block = ModbusSequentialDataBlock(0, [0] * register_count)

    hr_block.setValues(args.address, [0])

    context = ModbusDeviceContext(hr=hr_block)
    server_context = ModbusServerContext(devices=context)

    logger.info("")
    logger.info("显示灯控制器配置:")
    logger.info(f"  寄存器地址: {args.address} (十进制)")
    logger.info(f"  初始状态: 断开 (0)")
    logger.info("")
    logger.info("寄存器说明:")
    logger.info(f"  地址 {args.address}: 显示灯控制寄存器")
    logger.info("    - 写入 0: 断开显示灯")
    logger.info("    - 写入 1: 接通显示灯")
    logger.info("")
    logger.info(f"Modbus TCP 服务器启动: {args.host}:{args.port}")
    logger.info("按 Ctrl+C 停止服务器")
    logger.info("=" * 50)
    logger.info("")

    update_task = asyncio.create_task(update_light_task(light, hr_block))

    server_address = (args.host, args.port)

    stop_event = asyncio.Event()

    def signal_handler(sig, frame):
        logger.info("接收到中断信号，正在关闭服务器...")
        stop_event.set()

    signal.signal(signal.SIGINT, signal_handler)
    signal.signal(signal.SIGTERM, signal_handler)

    try:
        await StartAsyncTcpServer(
            context=server_context,
            address=server_address,
            framer=FramerType.SOCKET,
        )
    except asyncio.CancelledError:
        logger.info("服务器任务已取消")
    except Exception as e:
        logger.error(f"服务器错误: {e}")
    finally:
        update_task.cancel()
        try:
            await update_task
        except asyncio.CancelledError:
            pass
        logger.info("服务器已关闭")


if __name__ == "__main__":
    asyncio.run(main())
