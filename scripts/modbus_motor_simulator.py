#!/usr/bin/env python3
"""Modbus TCP模拟从站 - 伺服电机模拟器。"""

import asyncio
import logging
import signal
import sys
import threading
import time

from pymodbus import FramerType
from pymodbus.datastore import ModbusDeviceContext, ModbusSequentialDataBlock, ModbusServerContext
from pymodbus.server import ModbusTcpServer


REGISTER_NAMES = {
    0: "控制字",
    1: "状态字",
    10: "命令速度",
    11: "实际速度",
    20: "命令位置",
    21: "实际位置",
    30: "命令扭矩",
    31: "实际扭矩",
}

SIGNED_REGISTERS = {
    20,
    21,
}

REGISTER_BUFFER_SIZE = max(REGISTER_NAMES) + 2


def public_to_store(address: int) -> int:
    return address + 1


def store_to_public(address: int) -> int:
    return address - 1


def clamp_u16(value: int) -> int:
    return max(0, min(0xFFFF, int(value)))


def clamp_s16(value: int) -> int:
    return max(-32768, min(32767, int(value)))


def encode_register_value(address: int, value: int) -> int:
    if address in SIGNED_REGISTERS:
        return clamp_s16(value) & 0xFFFF
    return clamp_u16(value)


def decode_register_value(address: int, value: int) -> int:
    value = int(value) & 0xFFFF
    if address in SIGNED_REGISTERS and value >= 0x8000:
        return value - 0x10000
    return value


class ServoMotorSimulator:
    """伺服电机模拟器。"""

    ADDR_CONTROL_WORD = 0
    ADDR_STATUS_WORD = 1
    ADDR_COMMAND_SPEED = 10
    ADDR_ACTUAL_SPEED = 11
    ADDR_COMMAND_POSITION = 20
    ADDR_ACTUAL_POSITION = 21
    ADDR_COMMAND_TORQUE = 30
    ADDR_ACTUAL_TORQUE = 31

    def __init__(self):
        self.control_word = 0
        self.is_running = False
        self.direction = 1

        self.command_speed = 250
        self.actual_speed = 0

        self.command_position = 0
        self.actual_position = 0

        self.command_torque = 5
        self.actual_torque = 0

    @staticmethod
    def _approach(current: int, target: int, ratio: float = 0.2) -> int:
        diff = target - current
        if diff == 0:
            return current

        step = int(diff * ratio)
        if step == 0:
            step = 1 if diff > 0 else -1

        next_value = current + step
        if diff > 0:
            return min(next_value, target)
        return max(next_value, target)

    def apply_register_write(self, address: int, value: int):
        if address == self.ADDR_CONTROL_WORD:
            self.control_word = clamp_u16(value)
            self.is_running = bool(self.control_word & 0x0001)
            self.direction = -1 if (self.control_word & 0x0002) else 1
        elif address == self.ADDR_COMMAND_SPEED:
            self.command_speed = clamp_u16(value)
        elif address == self.ADDR_COMMAND_POSITION:
            self.command_position = clamp_s16(value)
        elif address == self.ADDR_COMMAND_TORQUE:
            self.command_torque = clamp_u16(value)

    def update(self):
        if self.is_running:
            self.actual_speed = clamp_u16(self._approach(self.actual_speed, self.command_speed, 0.18))
            self.actual_torque = clamp_u16(self._approach(self.actual_torque, self.command_torque, 0.25))

            if self.actual_speed > 0:
                position_step = max(1, self.actual_speed // 60)
                self.actual_position = clamp_s16(self.actual_position + position_step * self.direction)
        else:
            self.actual_speed = clamp_u16(self._approach(self.actual_speed, 0, 0.25))
            self.actual_torque = clamp_u16(self._approach(self.actual_torque, 0, 0.3))

    def get_status_word(self) -> int:
        status = 0x0001
        if self.is_running:
            status |= 0x0002
        if abs(self.actual_position - self.command_position) <= 1:
            status |= 0x0010
        if (self.is_running and abs(self.actual_speed - self.command_speed) <= 1) or (
            not self.is_running and self.actual_speed == 0
        ):
            status |= 0x0020
        return status

    def get_register_snapshot(self) -> dict[int, int]:
        return {
            self.ADDR_CONTROL_WORD: self.control_word,
            self.ADDR_STATUS_WORD: self.get_status_word(),
            self.ADDR_COMMAND_SPEED: self.command_speed,
            self.ADDR_ACTUAL_SPEED: self.actual_speed,
            self.ADDR_COMMAND_POSITION: self.command_position,
            self.ADDR_ACTUAL_POSITION: self.actual_position,
            self.ADDR_COMMAND_TORQUE: self.command_torque,
            self.ADDR_ACTUAL_TORQUE: self.actual_torque,
        }


class MonitoredDataBlock(ModbusSequentialDataBlock):
    """带主站写入跟踪的数据块。"""

    def __init__(self, address: int, values: list[int], motor: ServoMotorSimulator):
        super().__init__(address, values)
        self.motor = motor
        self.last_external_write = "无"
        self._write_lock = threading.Lock()

    def getValues(self, address, count=1):
        with self._write_lock:
            return super().getValues(address, count)

    def setValues(self, address, values):
        if not isinstance(values, list):
            values = [values]
        if not values:
            return None

        with self._write_lock:
            old_values = super().getValues(address, len(values))
            public_address = store_to_public(address)

            decoded_values = []
            encoded_values = []
            for offset, raw_value in enumerate(values):
                current_address = public_address + offset
                decoded_value = decode_register_value(current_address, raw_value)
                decoded_values.append(decoded_value)
                encoded_values.append(encode_register_value(current_address, decoded_value))
                self.motor.apply_register_write(current_address, decoded_value)

            result = super().setValues(address, encoded_values)
            self.last_external_write = self._format_write(public_address, old_values, decoded_values)
            return result

    def set_internal_value(self, public_address: int, value: int):
        with self._write_lock:
            return super().setValues(
                public_to_store(public_address),
                [encode_register_value(public_address, value)],
            )

    def _format_write(self, public_address: int, old_values, new_values: list[int]) -> str:
        if isinstance(old_values, list) and len(new_values) == 1:
            old_value = decode_register_value(public_address, old_values[0])
            name = REGISTER_NAMES.get(public_address, "未知寄存器")
            return f"地址 {public_address} ({name}): {old_value} -> {new_values[0]}"

        end_address = public_address + len(new_values) - 1
        return f"地址 {public_address}-{end_address}: 写入 {new_values}"


class ConnectionTracker:
    """主站连接状态跟踪。"""

    def __init__(self):
        self.connection_count = 0
        self.last_event = "等待主站连接"
        self._lock = threading.Lock()

    def trace_connect(self, connected: bool):
        now = time.strftime("%H:%M:%S")
        with self._lock:
            if connected:
                self.connection_count += 1
                self.last_event = f"{now} 主站已连接"
            else:
                self.connection_count = max(0, self.connection_count - 1)
                self.last_event = f"{now} 主站已断开"

    def snapshot(self) -> tuple[int, str]:
        with self._lock:
            return self.connection_count, self.last_event


async def update_motor_task(motor: ServoMotorSimulator, hr_block: MonitoredDataBlock):
    last_snapshot: dict[int, int] = {}

    while True:
        motor.update()
        snapshot = motor.get_register_snapshot()

        for address, value in snapshot.items():
            if last_snapshot.get(address) != value:
                hr_block.set_internal_value(address, value)
                last_snapshot[address] = value

        await asyncio.sleep(0.1)


def build_dashboard(
    motor: ServoMotorSimulator,
    hr_block: MonitoredDataBlock,
    connection_tracker: ConnectionTracker,
    server_address: tuple[str, int],
) -> str:
    connection_count, last_event = connection_tracker.snapshot()
    connection_status = "已连接" if connection_count > 0 else "未连接"
    running_text = "运行中" if motor.is_running else "已停止"
    direction_text = "反向" if motor.direction < 0 else "正向"
    status_word = motor.get_status_word()

    status_flags = []
    if status_word & 0x0001:
        status_flags.append("就绪")
    if status_word & 0x0002:
        status_flags.append("运行")
    if status_word & 0x0010:
        status_flags.append("到位")
    if status_word & 0x0020:
        status_flags.append("到速")

    rows = [
        (0, "控制字", motor.control_word, ""),
        (1, "状态字", status_word, ""),
        (10, "命令速度", motor.command_speed, "rpm"),
        (11, "实际速度", motor.actual_speed, "rpm"),
        (20, "命令位置", motor.command_position, "pulse"),
        (21, "实际位置", motor.actual_position, "pulse"),
        (30, "命令扭矩", motor.command_torque, "Nm"),
        (31, "实际扭矩", motor.actual_torque, "Nm"),
    ]

    lines = [
        "=" * 72,
        "Modbus TCP 伺服电机模拟器  (寄存器地址均为十进制，单个值占用 1 个寄存器)",
        f"监听地址: {server_address[0]}:{server_address[1]}    Ctrl+C 退出",
        f"主站连接: {connection_status} ({connection_count})    最近连接事件: {last_event}",
        f"运行状态: {running_text}    方向: {direction_text}",
        f"控制字: {motor.control_word} (0x{motor.control_word:04X})    状态字: {status_word} (0x{status_word:04X})",
        f"状态标志: {' / '.join(status_flags) if status_flags else '无'}",
        f"最近主站写入: {hr_block.last_external_write}",
        "-" * 72,
        f"{'地址':>6}  {'名称':<10} {'当前值':>12}  单位",
        "-" * 72,
    ]

    for address, name, value, unit in rows:
        lines.append(f"{address:>6}  {name:<10} {value:>12}  {unit}")

    lines.append("=" * 72)
    return "\n".join(lines)


def build_status_line(
    motor: ServoMotorSimulator,
    hr_block: MonitoredDataBlock,
    connection_tracker: ConnectionTracker,
    server_address: tuple[str, int],
) -> str:
    connection_count, last_event = connection_tracker.snapshot()
    connection_status = "已连接" if connection_count > 0 else "未连接"
    running_text = "运行" if motor.is_running else "停止"
    direction_text = "反向" if motor.direction < 0 else "正向"
    status_word = motor.get_status_word()

    return (
        f"监听 {server_address[0]}:{server_address[1]} | 主站:{connection_status}({connection_count}) | "
        f"状态:{running_text}/{direction_text} | 控制字:{motor.control_word} | 状态字:{status_word} | "
        f"速度:{motor.actual_speed}/{motor.command_speed} rpm | 位置:{motor.actual_position}/{motor.command_position} | "
        f"扭矩:{motor.actual_torque}/{motor.command_torque} Nm | 最近写入:{hr_block.last_external_write} | {last_event}"
    )


async def display_task(
    motor: ServoMotorSimulator,
    hr_block: MonitoredDataBlock,
    connection_tracker: ConnectionTracker,
    server_address: tuple[str, int],
):
    interactive = sys.stdout.isatty()
    first_frame = True

    while True:
        if interactive:
            dashboard = build_dashboard(motor, hr_block, connection_tracker, server_address)
            prefix = "\033[2J\033[H" if first_frame else "\033[H\033[J"
            sys.stdout.write(prefix + dashboard + "\n")
            sys.stdout.flush()
            first_frame = False
        else:
            line = build_status_line(motor, hr_block, connection_tracker, server_address)
            sys.stdout.write("\r" + line + " " * 8)
            sys.stdout.flush()

        await asyncio.sleep(0.5)


async def main():
    logging.basicConfig(level=logging.CRITICAL)
    logging.getLogger("pymodbus").setLevel(logging.CRITICAL)

    motor = ServoMotorSimulator()
    connection_tracker = ConnectionTracker()
    hr_block = MonitoredDataBlock(1, [0] * REGISTER_BUFFER_SIZE, motor)

    for address, value in motor.get_register_snapshot().items():
        hr_block.set_internal_value(address, value)

    context = ModbusDeviceContext(hr=hr_block)
    server_context = ModbusServerContext(devices=context)

    server_address = ("0.0.0.0", 5020)
    stop_event = asyncio.Event()

    def signal_handler(signum, frame):
        _ = signum, frame
        stop_event.set()

    signal.signal(signal.SIGINT, signal_handler)
    if hasattr(signal, "SIGTERM"):
        signal.signal(signal.SIGTERM, signal_handler)

    server = ModbusTcpServer(
        context=server_context,
        address=server_address,
        framer=FramerType.SOCKET,
        trace_connect=connection_tracker.trace_connect,
    )

    update_task_handle = asyncio.create_task(update_motor_task(motor, hr_block))
    display_task_handle = asyncio.create_task(
        display_task(motor, hr_block, connection_tracker, server_address)
    )

    try:
        await server.serve_forever(background=True)
        await stop_event.wait()
    except KeyboardInterrupt:
        stop_event.set()
    finally:
        display_task_handle.cancel()
        update_task_handle.cancel()

        await server.shutdown()

        for task in (display_task_handle, update_task_handle):
            try:
                await task
            except asyncio.CancelledError:
                pass

        if sys.stdout.isatty():
            sys.stdout.write("\n")
            sys.stdout.flush()

        print("伺服电机模拟器已关闭")


if __name__ == "__main__":
    asyncio.run(main())
