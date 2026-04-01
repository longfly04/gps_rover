# -*- coding: utf-8 -*-
"""
NMEA报文模拟生成器
用于模拟GPS模块输出的NMEA协议报文，支持虚拟串口通信

功能：
- 通过虚拟串口模拟连接GPS模块
- 生成直线运动状态下的NMEA协议报文
- 支持配置参数：初始位置、方向角、速度、更新率、报文类别、扰动率
"""

from operator import truediv
import serial
import serial.tools.list_ports
import time
import math
import random
import struct
import argparse
import threading
import sys
from datetime import datetime, timedelta


class NMEAGenerator:
    """NMEA报文生成器类"""

    def __init__(self, config):
        """
        初始化NMEA生成器

        Args:
            config: 配置参数字典
        """
        self.lat = config.get('latitude', 39.9042)      # 初始纬度 (度)
        self.lon = config.get('longitude', 116.4074)    # 初始经度 (度)
        self.alt = config.get('altitude', 50.0)         # 初始高度 (米)
        self.heading = config.get('heading', 0.0)       # 初始方向角 (度, 正北为0)
        self.speed = config.get('speed', 5.0)           # 初始速度 (米/秒)
        self.update_rate = config.get('update_rate', 1) # 更新率 (Hz)
        self.noise_rate = config.get('noise_rate', 0.1) # 扰动率 (0-1)

        # 报文类型开关
        self.enable_gga = config.get('enable_gga', True)
        self.enable_gll = config.get('enable_gll', True)
        self.enable_gsa = config.get('enable_gsa', True)
        self.enable_gsv = config.get('enable_gsv', True)
        self.enable_rmc = config.get('enable_rmc', True)
        self.enable_vtg = config.get('enable_vtg', True)

        # 卫星信息
        self.num_satellites = config.get('num_satellites', 10)
        self.satellite_ids = list(range(1, 33))  # GPS卫星ID 1-32

        # 稳定的卫星数据 (初始化一次,之后只变化SNR)
        self.visible_satellites = []  # 可见卫星列表
        self._init_visible_satellites()  # 初始化可见卫星

        # 时间
        self.start_time = datetime.now()
        self.utc_time = datetime.utcnow()

        # 累积位移
        self.total_distance = 0.0

        # 路线类型: 'linear' (直线) 或 'roundtrip' (往返)
        self.route_type = config.get('route_type', 'linear')

        # 往返运动参数
        self.roundtrip_distance = config.get('roundtrip_distance', 200.0)  # 往返距离N (米)
        self.roundtrip_gap = config.get('roundtrip_gap', 1.0)  # 往返路线间隔m (米)
        self.roundtrip_turn_speed = config.get('roundtrip_turn_speed', 2.0)  # 折返点速度 (米/秒)
        self.roundtrip_turn_direction = config.get('roundtrip_turn_direction', 'clockwise')  # 折返旋转方向
        self.roundtrip_clockwise = True if self.roundtrip_turn_direction == 'clockwise' else False

        # 圆形路线参数
        self.circle_radius = config.get('circle_radius', 10.0)  # 圆形路线半径 (米)
        self.circle_speed = config.get('circle_speed', 2.0)  # 圆形路线初始线速度绝对值 (米/秒)
        self.circle_heading = config.get('circle_heading', 0.0)  # 圆形路线初始线速度方位角 (度, 正北为0)
        self.circle_direction = config.get('circle_direction', 'clockwise')  # 圆形路线旋转方向
        self.circle_clockwise = True if self.circle_direction == 'clockwise' else False

        # 圆形路线运动状态
        self.is_in_circle = False  # 是否在圆形路线阶段
        self.circle_start_pos = None  # 圆形路线开始位置 (lat, lon)
        self.circle_start_heading = None  # 圆形路线开始时的航向
        self.circle_angle = 0.0  # 圆形路线已转过的角度 (弧度)
        self.circle_center = None  # 圆形路线圆心 (lat, lon)

        # 往返运动状态
        self.is_returning = False  # 是否在折返阶段（半圆形路线阶段）
        self.return_start_pos = None  # 折返开始位置 (lat, lon)
        self.return_start_heading = None  # 折返开始时的航向
        self.return_angle = 0.0  # 折返过程中已转过的角度 (弧度)
        self.return_radius = self.roundtrip_gap / 2.0  # 折返半径 = m/2
        self.return_center = None  # 折返圆心 (lat, lon)
        self.turn_count = 0  # 折返次数计数，用于切换方向
        self.turn_direction = True  # 当前折返的方向 (True=顺时针, False=逆时针)
        self.return_linear_speed = 0.0  # 直线阶段的速度

        # 初始位置备份
        self.start_lat = self.lat
        self.start_lon = self.lon

    def _init_visible_satellites(self):
        """
        初始化可见卫星数据

        生成稳定的卫星数据，之后只更新SNR值而不改变卫星ID、仰角和方位角
        """
        self.visible_satellites = []

        # GPS卫星PRN范围是1-32，选择num_satellites个作为可见卫星
        available_ids = self.satellite_ids.copy()
        random.shuffle(available_ids)

        for i in range(self.num_satellites):
            sat_id = available_ids[i]
            # 仰角: 5-90度，固定不变
            elevation = random.randint(5, 90)
            # 方位角: 0-359度，固定不变
            azimuth = random.randint(0, 359)
            # 基础SNR: 30-60 dB
            base_snr = random.randint(30, 60)

            self.visible_satellites.append({
                'prn': sat_id,
                'elevation': elevation,
                'azimuth': azimuth,
                'base_snr': base_snr,
                'snr': base_snr
            })

    def _update_satellite_snr(self):
        """
        更新卫星SNR值

        在基础SNR上添加小幅随机波动，模拟真实GPS信号强度的微小变化
        """
        for sat in self.visible_satellites:
            # SNR波动范围: ±5 dB
            noise = random.uniform(-5, 5)
            new_snr = sat['base_snr'] + noise
            # 限制在合理范围内
            sat['snr'] = max(20, min(70, int(new_snr)))

    def calculate_nmea_checksum(self, sentence):
        """
        计算NMEA校验和

        NMEA校验和是对$和*之间所有字符进行XOR运算

        Args:
            sentence: NMEA语句（不包含$和*）

        Returns:
            两位十六进制校验和字符串
        """
        checksum = 0
        for char in sentence:
            checksum ^= ord(char)
        return format(checksum, '02X')

    def format_lat_lon(self, lat, lon):
        """
        格式化经纬度为NMEA格式

        Args:
            lat: 纬度 (度)
            lon: 经度 (度)

        Returns:
            (lat_str, lat_dir, lon_str, lon_dir) 元组
        """
        # 纬度：转换为度分格式，分保留 8 位小数
        lat_sign = 1 if lat >= 0 else -1
        lat_abs = abs(lat)
        lat_deg = int(lat_abs)
        lat_min_decimal = (lat_abs - lat_deg) * 60
        # NMEA GGA 格式：DDMM.mmmmmmmmm
        # DD: 2 位度数，MM.mmmmmmmmm: 分（2 位整数 + 8 位小数）
        lat_str = f"{lat_deg:02d}{lat_min_decimal:010.8f}"
        lat_dir = 'N' if lat_sign > 0 else 'S'

        # 经度：转换为度分格式，分保留 8 位小数
        lon_sign = 1 if lon >= 0 else -1
        lon_abs = abs(lon)
        lon_deg = int(lon_abs)
        lon_min_decimal = (lon_abs - lon_deg) * 60
        # NMEA GGA 格式：DDDMM.mmmmmmmmm
        # DDD: 3 位度数，MM.mmmmmmmmm: 分（2 位整数 + 8 位小数）
        lon_str = f"{lon_deg:03d}{lon_min_decimal:010.8f}"
        lon_dir = 'E' if lon_sign > 0 else 'W'

        return lat_str, lat_dir, lon_str, lon_dir
    def add_noise(self, value, max_noise):
        """
        添加随机扰动

        Args:
            value: 原始值
            max_noise: 最大扰动量

        Returns:
            带扰动的值
        """
        if random.random() < self.noise_rate:
            return value + random.uniform(-max_noise, max_noise)
        return value

    def generate_gga(self):
        """
        生成GGA报文 - GPS定位信息

        格式: $GPGGA,HHMMSS.SSS,DDMM.MMMM,N,DDDMM.MMMM,E,Q,SS,HDOP,H.H,H.H,M,M.M*KK

        字段说明:
        - HHMMSS.SSS: UTC时间
        - DDMM.MMMM,N/S: 纬度
        - DDDMM.MMMM,E/W: 经度
        - Q: 定位质量 (0=无效, 1=GPS, 2=差分GPS)
        - SS: 使用的卫星数
        - HDOP: 水平精度因子
        - H.H: 天线高度
        - M: 单位 (米)
        - M.M: 大地水准面差距
        - M: 单位 (米)
        """
        # 时间
        utc_now = datetime.utcnow()
        time_str = utc_now.strftime('%H%M%S') + '.000'

        # 经纬度
        lat_str, lat_dir, lon_str, lon_dir = self.format_lat_lon(self.lat, self.lon)

        # 定位质量
        quality = 1 if self.num_satellites >= 4 else 0

        # 卫星数
        used_sat = min(self.num_satellites, 12)

        # HDOP (精度因子)
        hdop = self.add_noise(1.0, 0.5)

        # 高度
        altitude = self.add_noise(self.alt, 0.5)

        # 大地水准面差距
        geoid = self.add_noise(-10.0, 1.0)

        sentence = (f"GPGGA,{time_str},{lat_str},{lat_dir},"
                    f"{lon_str},{lon_dir},{quality},{used_sat:02d},"
                    f"{hdop:.1f},{altitude:.1f},M,{geoid:.1f},M,,")

        checksum = self.calculate_nmea_checksum(sentence)
        return f"${sentence}{checksum}\r\n"

    def generate_gll(self):
        """
        生成GLL报文 - 地理定位信息

        格式: $GPGLL,DDMM.MMMM,N,DDDMM.MMMM,E,HHMMSS.SSS,A,A*KK

        字段说明:
        - DDMM.MMMM,N/S: 纬度
        - DDDMM.MMMM,E/W: 经度
        - HHMMSS.SSS: UTC时间
        - A: 状态 (A=有效, V=无效)
        - A: 模式 (A=自主定位, D=差分, E=估算)
        """
        # 时间
        utc_now = datetime.utcnow()
        time_str = utc_now.strftime('%H%M%S') + '.000'

        # 经纬度
        lat_str, lat_dir, lon_str, lon_dir = self.format_lat_lon(self.lat, self.lon)

        # 状态
        status = 'A' if self.num_satellites >= 4 else 'V'

        sentence = f"GPGLL,{lat_str},{lat_dir},{lon_str},{lon_dir},{time_str},{status},A"

        checksum = self.calculate_nmea_checksum(sentence)
        return f"${sentence}{checksum}\r\n"

    def generate_gsa(self):
        """
        生成GSA报文 - GPS DOP信息

        格式: $GPGSA,M,3,01,02,03,04,05,06,07,08,09,10,11,12,PDOP,HDOP,VDOP*KK

        字段说明:
        - M: 模式 (M=手动, A=自动)
        - 3: 定位模式 (1=无效, 2=2D, 3=3D)
        - 01-12: 使用的卫星PRN号 (最多12个，空卫星位置用,,表示)
        - PDOP: 位置精度因子
        - HDOP: 水平精度因子
        - VDOP: 垂直精度因子
        """
        # 模式
        mode = 'A'
        mode_fix = '3' if self.num_satellites >= 4 else '1'

        # 使用的卫星列表 - 从稳定的可见卫星列表中随机抽取90%
        # GSA报文显示参与定位的卫星，通常是可见卫星的一个子集
        total_sats = len(self.visible_satellites)
        num_to_use = max(1, min(12, int(total_sats * 0.9)))
        
        # 确保至少有4颗卫星用于3D定位
        if total_sats >= 4 and num_to_use < 4:
            num_to_use = 4
        
        # 随机选择卫星
        import random
        selected_satellites = random.sample(self.visible_satellites, num_to_use)
        
        # 构建使用的卫星列表
        used_sats = []
        for sat in selected_satellites:
            used_sats.append(str(sat['prn']))

        # 补齐到12个位置
        while len(used_sats) < 12:
            used_sats.append('')

        # DOP值 - 基于参与定位的卫星数量计算，更真实
        num_used = len([s for s in used_sats if s])
        if num_used < 4:
            pdop = 1.0
            hdop = 2.0
            vdop = 2.0
        else:
            pdop = self.add_noise(2.0, 1.0)
            hdop = self.add_noise(1.5, 0.8)
            vdop = self.add_noise(3.0, 1.5)

        sentence = (f"GPGSA,{mode},{mode_fix}," + ','.join(used_sats) + f","
                    f"{pdop:.1f},{hdop:.1f},{vdop:.1f}*")

        checksum = self.calculate_nmea_checksum(sentence)
        return f"${sentence}{checksum}\r\n"

    def generate_gsv(self):
        """
        生成GSV报文 - 可见卫星信息

        格式: $GPGSV,TTT,SS,NN,II,EE,SS,NN...,*KK

        字段说明:
        - TTT: 消息总数
        - SS: 消息编号
        - NN: 可见卫星总数
        - II: 卫星PRN号
        - EE: 仰角 (度)
        - SS: 信噪比 (dB-Hz)
        - 重复...
        """
        # 先更新SNR值
        self._update_satellite_snr()

        total_sats = len(self.visible_satellites)
        msgs_needed = math.ceil(total_sats / 4)
        sentences = []

        for msg_num in range(1, msgs_needed + 1):
            # 当前消息中的卫星
            start_idx = (msg_num - 1) * 4
            end_idx = min(start_idx + 4, total_sats)

            satellites = []
            for i in range(start_idx, end_idx):
                sat = self.visible_satellites[i]
                satellites.extend([sat['prn'], sat['elevation'], sat['azimuth'], sat['snr']])

            # 补齐空位
            while len(satellites) < 16:
                satellites.extend([0, 0, 0, 0])

            sentence = (f"GPGSV,{msgs_needed},{msg_num},{total_sats},"
                        f"{satellites[0]},{satellites[1]},{satellites[2]},{satellites[3]},"
                        f"{satellites[4]},{satellites[5]},{satellites[6]},{satellites[7]},"
                        f"{satellites[8]},{satellites[9]},{satellites[10]},{satellites[11]},"
                        f"{satellites[12]},{satellites[13]},{satellites[14]},{satellites[15]}*")

            checksum = self.calculate_nmea_checksum(sentence)
            sentences.append(f"${sentence}{checksum}\r\n")

        return sentences

    def generate_rmc(self):
        """
        生成RMC报文 - 推荐最小定位信息

        格式: $GPRMC,HHMMSS.SSS,A,DDMM.MMMM,N,DDDMM.MMMM,E,SS.S,CCC.D,DDMMYY,,A*KK

        字段说明:
        - HHMMSS.SSS: UTC时间
        - A: 状态 (A=有效, V=无效)
        - DDMM.MMMM,N/S: 纬度
        - DDDMM.MMMM,E/W: 经度
        - SS.S: 速度 (节)
        - CCC.D: 航向 (度)
        - DDMMYY: 日期 (日/月/年)
        - ,: 磁偏角
        - A: 模式 (A=自主定位, D=差分, E=估算)
        """
        # 时间
        utc_now = datetime.utcnow()
        time_str = utc_now.strftime('%H%M%S') + '.000'

        # 状态
        status = 'A' if self.num_satellites >= 4 else 'V'

        # 经纬度
        lat_str, lat_dir, lon_str, lon_dir = self.format_lat_lon(self.lat, self.lon)

        # 速度 (转换为节, 1节 = 0.514444 m/s)
        speed_knots = self.speed * 1.94384
        speed_knots = self.add_noise(speed_knots, speed_knots * 0.1)

        # 航向
        heading = self.add_noise(self.heading, 2.0) % 360

        # 日期
        date_str = utc_now.strftime('%d%m%y')

        sentence = (f"GPRMC,{time_str},{status},{lat_str},{lat_dir},"
                    f"{lon_str},{lon_dir},{speed_knots:.1f},{heading:.1f},"
                    f"{date_str},,A*")

        checksum = self.calculate_nmea_checksum(sentence)
        return f"${sentence}{checksum}\r\n"

    def generate_vtg(self):
        """
        生成VTG报文 - 地面速度信息

        格式: $GPVTG,CCC.T,T,M,SSS.S,N,SSS.S,K,A*KK

        字段说明:
        - CCC.T: 航向 (度, 真北)
        - T: 模式 (T=真北)
        - M: 模式 (M=磁北)
        - SSS.S: 速度 (节)
        - N: 速度单位 (N=节)
        - SSS.S: 速度 (千米/时)
        - K: 速度单位 (K=千米/时)
        - A: 模式指示 (A=自主定位, D=差分, E=估算, N=无效)
        """
        # 航向 (真北)
        heading = self.add_noise(self.heading, 2.0) % 360

        # 速度 (节)
        speed_knots = self.speed * 1.94384
        speed_knots = self.add_noise(speed_knots, speed_knots * 0.1)

        # 速度 (千米/时)
        speed_kmh = self.speed * 3.6
        speed_kmh = self.add_noise(speed_kmh, speed_kmh * 0.1)

        # 模式指示
        mode = 'A' if self.num_satellites >= 4 else 'N'

        sentence = (f"GPVTG,{heading:.1f},T,,M,{speed_knots:.1f},N,{speed_kmh:.1f},K,{mode}")

        checksum = self.calculate_nmea_checksum(sentence)
        return f"${sentence}{checksum}\r\n"

    def update_position(self, dt):
        """
        更新位置 (支持直线和往返运动模式)

        Args:
            dt: 时间间隔 (秒)

        直线模式:
        - 按初始方向角和速度直线移动

        往返模式:
        - 先直线运动到往返距离N
        - 然后以半径m/2走半圆折返
        - 折返时速度恒定为0.2米/秒
        - 折返后继续直线运动
        """
        if self.route_type == 'linear':
            self._update_position_linear(dt)
        elif self.route_type == 'roundtrip':
            self._update_position_roundtrip(dt)
        elif self.route_type == 'circle':
            self._update_position_circle(dt)
        else:
            self._update_position_linear(dt)

    def _update_position_linear(self, dt):
        """
        直线运动模式

        Args:
            dt: 时间间隔 (秒)
        """
        R_earth = 6371000.0
        heading_rad = math.radians(self.heading)

        dlat = (self.speed * math.cos(heading_rad) * dt / R_earth) * (180 / math.pi)
        self.lat += dlat

        dlon = (self.speed * math.sin(heading_rad) * dt / (R_earth * math.cos(math.radians(self.lat)))) * (180 / math.pi)
        self.lon += dlon

        self.total_distance += self.speed * dt
        
        # 更新直线运动距离（用于往返路线）
        if self.route_type == 'roundtrip':
            if not hasattr(self, 'linear_distance'):
                self.linear_distance = 0.0
            self.linear_distance += self.speed * dt

    def _update_position_circle(self, dt):
        """
        圆形运动模式

        运动逻辑:
        1. 以给定半径和初始线速度做圆周运动
        2. 根据初始线速度方位角和旋转方向确定圆心和运动轨迹

        参数:
            - 初始位置经纬度 (lat, lon): 起始点
            - 半径长度 (circle_radius): 圆周运动半径 (米)
            - 初始线速度绝对值 (circle_speed): 线速度大小 (米/秒)
            - 初始线速度方位角 (circle_heading): 初始运动方向 (度, 正北为0)
            - 顺时针/逆时针 (circle_clockwise): 旋转方向

        Args:
            dt: 时间间隔 (秒)
        """
        METERS_PER_DEGREE_LAT = 111000.0

        # 首次调用时，根据初始线速度方位角计算圆心位置
        if self.circle_angle == 0.0:
            heading_rad = math.radians(self.circle_heading)

            # 圆心方向: 速度方向的右侧(顺时针)或左侧(逆时针)
            if self.circle_clockwise:
                center_heading_rad = heading_rad + math.pi / 2
            else:
                center_heading_rad = heading_rad - math.pi / 2

            # 计算圆心相对于起始点的偏移
            dlat = self.circle_radius * math.cos(center_heading_rad) / METERS_PER_DEGREE_LAT
            dlon = self.circle_radius * math.sin(center_heading_rad) / (METERS_PER_DEGREE_LAT * math.cos(math.radians(self.lat)))

            # 保存圆心位置
            self.circle_center = (self.lat + dlat, self.lon + dlon)

            # 初始时刻的位置就是给定起始位置，不需要调整
            # heading 设置为初始线速度方位角
            self.heading = self.circle_heading

        # 计算角速度 (弧度/秒)
        angular_speed = self.circle_speed / self.circle_radius

        # 更新已转过的角度
        self.circle_angle += angular_speed * dt

        # 角度归一化到 [0, 2π)
        if self.circle_angle >= 2 * math.pi:
            self.circle_angle -= 2 * math.pi

        # 根据旋转方向确定当前角度
        if self.circle_clockwise:
            current_theta = self.circle_angle
        else:
            current_theta = -self.circle_angle

        # 计算相对于圆心的位置偏移
        dlat = self.circle_radius * math.sin(current_theta) / METERS_PER_DEGREE_LAT
        dlon = self.circle_radius * math.cos(current_theta) / (METERS_PER_DEGREE_LAT * math.cos(math.radians(self.lat)))

        # 更新位置
        self.lat = self.circle_center[0] + dlat
        self.lon = self.circle_center[1] + dlon

        # 更新航向: 切线方向 = 当前位置相对于圆心的角度 + 90°(顺时针) 或 -90°(逆时针)
        # 当前角度相对于圆心的方向角
        if self.circle_clockwise:
            tangent_heading = math.degrees(current_theta + math.pi / 2)
        else:
            tangent_heading = math.degrees(current_theta - math.pi / 2)

        self.heading = (tangent_heading + 360) % 360

        # 更新累积距离
        self.total_distance += self.circle_speed * dt

    def _update_position_roundtrip(self, dt):
        """
        往返运动模式（弓字型路线）

        运动逻辑:
        1. 首先以 heading 方向，speed 速度直线运行
        2. 到达 roundtrip_distance 距离时，进入折返阶段（半圆形路线）
        3. 半圆形路线旋转 180 度，奇数折返点顺时针，偶数折返点逆时针
        4. 半圆形路线结束后，方向角调转 180 度，继续直线路线
        5. 以此类推，形成弓字型路线

        Args:
            dt: 时间间隔 (秒)
        """
        METERS_PER_DEGREE_LAT = 111000.0

        if not self.is_returning:
            # 直线运动阶段
            self._update_position_linear(dt)

            # 检查是否到达折返点（使用 linear_distance 判断）
            if not hasattr(self, 'linear_distance'):
                self.linear_distance = 0.0
            
            if self.linear_distance >= self.roundtrip_distance:
                # 回退到准确的折返点位置
                excess_distance = self.linear_distance - self.roundtrip_distance
                if excess_distance > 0:
                    heading_rad = math.radians(self.heading)
                    R_earth = 6371000.0
                    dlat = (excess_distance * math.cos(heading_rad) / R_earth) * (180 / math.pi)
                    dlon = (excess_distance * math.sin(heading_rad) / (R_earth * math.cos(math.radians(self.lat)))) * (180 / math.pi)
                    self.lat -= dlat
                    self.lon -= dlon
                    self.linear_distance = self.roundtrip_distance

                # 初始化半圆运动参数
                self.is_returning = True
                self.return_start_heading = self.heading
                self.return_angle = 0.0
                self.turn_radius = self.roundtrip_gap / 2.0
                self.turn_clockwise = self.roundtrip_clockwise if (self.turn_count % 2 == 0) else (not self.roundtrip_clockwise)
                self.turn_distance = 0.0  # 半圆运动的距离
                
                # 计算起始相位角（起点相对于圆心的角度）
                heading_rad = math.radians(self.heading)
                if self.turn_clockwise:
                    # 圆心在右侧，起点在圆心左侧 180 度
                    self.start_phase = heading_rad - math.pi / 2
                else:
                    # 圆心在左侧，起点在圆心右侧 180 度
                    self.start_phase = heading_rad + math.pi / 2

                # 计算圆心位置
                if self.turn_clockwise:
                    center_heading_rad = heading_rad + math.pi / 2
                else:
                    center_heading_rad = heading_rad - math.pi / 2

                dlat = self.turn_radius * math.cos(center_heading_rad) / METERS_PER_DEGREE_LAT
                dlon = self.turn_radius * math.sin(center_heading_rad) / (METERS_PER_DEGREE_LAT * math.cos(math.radians(self.lat)))
                self.turn_center = (self.lat + dlat, self.lon + dlon)

        else:
            # 半圆运动阶段
            angular_speed = self.roundtrip_turn_speed / self.turn_radius
            self.return_angle += angular_speed * dt
            
            # 更新半圆运动距离
            if not hasattr(self, 'turn_distance'):
                self.turn_distance = 0.0
            self.turn_distance += self.roundtrip_turn_speed * dt

            # 检查是否完成半圆
            if self.return_angle >= math.pi:
                # 完成半圆，设置到 180 度位置
                self.return_angle = math.pi
                self.is_returning = False
                self.turn_count += 1
                self.heading = (self.return_start_heading + 180) % 360
                self.return_angle = 0.0
                
                # 计算半圆结束点的精确位置（theta = start_phase + π）
                if self.turn_clockwise:
                    final_theta = self.start_phase + math.pi
                else:
                    final_theta = self.start_phase - math.pi

                dlat = self.turn_radius * math.cos(final_theta) / METERS_PER_DEGREE_LAT
                dlon = self.turn_radius * math.sin(final_theta) / (METERS_PER_DEGREE_LAT * math.cos(math.radians(self.turn_center[0])))

                self.lat = self.turn_center[0] + dlat
                self.lon = self.turn_center[1] + dlon
                
                # 重置直线运动距离，开始新的直线段
                self.linear_distance = 0.0
            else:
                # 正在半圆运动中
                if self.turn_clockwise:
                    current_theta = self.start_phase + self.return_angle
                else:
                    current_theta = self.start_phase - self.return_angle

                # 位置计算：使用起始相位角
                dlat = self.turn_radius * math.cos(current_theta) / METERS_PER_DEGREE_LAT
                dlon = self.turn_radius * math.sin(current_theta) / (METERS_PER_DEGREE_LAT * math.cos(math.radians(self.turn_center[0])))

                self.lat = self.turn_center[0] + dlat
                self.lon = self.turn_center[1] + dlon

                # 更新航向为切线方向
                if self.turn_clockwise:
                    tangent_heading = math.degrees(current_theta + math.pi / 2)
                else:
                    tangent_heading = math.degrees(current_theta - math.pi / 2)

                self.heading = (tangent_heading + 360) % 360

    def generate_all_sentences(self):
        """
        生成所有启用的NMEA报文

        Returns:
            所有NMEA报文列表
        """
        sentences = []

        if self.enable_gga:
            sentences.append(self.generate_gga())

        if self.enable_gll:
            sentences.append(self.generate_gll())

        if self.enable_gsa:
            sentences.append(self.generate_gsa())

        if self.enable_gsv:
            sentences.extend(self.generate_gsv())

        if self.enable_rmc:
            sentences.append(self.generate_rmc())

        if self.enable_vtg:
            sentences.append(self.generate_vtg())

        return sentences


class VirtualSerialNMEASender:
    """虚拟串口NMEA发送器"""

    def __init__(self, config):
        """
        初始化发送器

        Args:
            config: 配置参数字典
        """
        self.config = config
        self.nmea_generator = NMEAGenerator(config)
        self.serial_port = None
        self.running = False
        self.user_stop = False
        self.non_block_mode = config.get('non_block_mode', True)

    def list_virtual_ports(self):
        """
        列出可用的串口

        Returns:
            串口列表
        """
        ports = serial.tools.list_ports.comports()
        return list(ports)

    def open_serial(self, port_name):
        """
        打开串口

        Args:
            port_name: 串口名称

        Returns:
            是否成功
        """
        try:
            non_block_mode = self.config.get('non_block_mode', True)
            write_timeout = 0 if non_block_mode else None

            self.serial_port = serial.Serial(
                port=port_name,
                baudrate=self.config.get('baudrate', 4800),
                bytesize=serial.EIGHTBITS,
                parity=serial.PARITY_NONE,
                stopbits=serial.STOPBITS_ONE,
                timeout=1,
                write_timeout=write_timeout
            )
            return True
        except serial.SerialException as e:
            error_msg = str(e).lower()
            if 'permission' in error_msg or '拒绝访问' in error_msg:
                print(f"\n错误: 串口 {port_name} 被占用或无权限访问")
                print("可能的原因:")
                print(f"  1. 串口 {port_name} 已被其他程序占用")
                print(f"  2. 需要管理员权限运行")
                print(f"  3. 串口设备不存在")
                print(f"\n请尝试:")
                print(f"  - 关闭占用串口的程序")
                print(f"  - 使用其他串口 (可用串口: COM6, COM10)")
                print(f"  - 以管理员身份运行")
            elif 'could not open port' in error_msg:
                print(f"\n错误: 无法打开串口 {port_name}")
                print(f"详细信息: {e}")
                print("请检查串口名称是否正确，或使用 --list-ports 查看可用串口")
            else:
                print(f"\n错误: 打开串口失败: {e}")
            return False
        except Exception as e:
            print(f"\n错误: 打开串口时发生未知错误: {e}")
            return False

    def start_sending(self, duration=None):
        """
        开始发送NMEA报文

        Args:
            duration: 持续时间 (秒), None表示无限
        """
        self.running = True
        self.user_stop = False
        interval = 1.0 / self.config.get('update_rate', 1)
        start_time = time.time()
        sentence_count = 0
        non_block_mode = self.non_block_mode

        if non_block_mode:
            print("模式: 无阻塞模式 (快速发送，不等待缓冲区清空)")
        else:
            print("模式: 阻塞模式 (等待缓冲区清空后发送下一帧)")

        # 启动输入监听线程
        input_thread = threading.Thread(target=self._input_listener, daemon=True)
        input_thread.start()

        while self.running:
            # 检查用户是否主动停止
            if self.user_stop:
                print("\n用户主动停止发送")
                break

            # 检查是否超时
            if duration and (time.time() - start_time) >= duration:
                print(f"\n已达到设定的运行时间 ({duration}秒)")
                break

            # 检查串口是否仍然打开
            if not self.serial_port or not self.serial_port.is_open:
                print("\n串口已关闭，停止发送")
                break

            # 生成并发送报文
            sentences = self.nmea_generator.generate_all_sentences()

            # 回显到终端
            echo_enabled = self.config.get('echo', True)
            if echo_enabled:
                print("-" * 60)

            for sentence in sentences:
                try:
                    self.serial_port.write(sentence.encode('ascii'))
                    if echo_enabled:
                        print(f"[{datetime.now().strftime('%H:%M:%S')}] {sentence.strip()}")

                    if not non_block_mode:
                        self.serial_port.flush()
                except serial.SerialException:
                    pass
                except Exception as e:
                    if not non_block_mode:
                        print(f"\n发送失败: {e}")
                        self.running = False
                        break

            sentence_count += 1

            # 更新位置
            self.nmea_generator.update_position(interval)

            # 等待下一个发送周期 (使用短间隔以便更快响应退出)
            time.sleep(interval)

        print(f"\n共发送 {sentence_count} 组报文")

    def stop_sending(self):
        """停止发送 (用户主动停止)"""
        self.user_stop = True
        self.running = False

    def close(self):
        """关闭串口"""
        if self.serial_port and self.serial_port.is_open:
            self.serial_port.close()

    def _input_listener(self):
        """监听用户输入退出命令"""
        try:
            while self.running:
                user_input = input()
                if user_input.lower() in ['q', 'quit', 'exit', 'x']:
                    print("\n收到退出命令，正在停止...")
                    self.stop_sending()
                    break
        except EOFError:
            pass
        except Exception:
            pass


class FileNMEASender:
    """文件输出NMEA发送器 (用于测试)"""

    def __init__(self, config):
        """
        初始化

        Args:
            config: 配置参文字典
        """
        self.config = config
        self.nmea_generator = NMEAGenerator(config)
        self.output_file = None
        self.running = False

    def open_file(self, filename):
        """
        打开输出文件

        Args:
            filename: 文件名
        """
        try:
            self.output_file = open(filename, 'w', encoding='utf-8')
            return True
        except Exception as e:
            print(f"打开文件失败: {e}")
            return False

    def start_sending(self, duration=None):
        """
        开始发送NMEA报文到文件

        Args:
            duration: 持续时间 (秒), None表示无限
        """
        self.running = True
        interval = 1.0 / self.config.get('update_rate', 1)

        start_time = time.time()

        while self.running:
            if duration and (time.time() - start_time) >= duration:
                break

            sentences = self.nmea_generator.generate_all_sentences()

            for sentence in sentences:
                if self.output_file:
                    self.output_file.write(sentence)
                    print(f"写入: {sentence.strip()}")

            self.nmea_generator.update_position(interval)
            time.sleep(interval)

    def stop_sending(self):
        """停止发送"""
        self.running = False

    def close(self):
        """关闭文件"""
        if self.output_file:
            self.output_file.close()


def print_config_info(config):
    """打印配置信息"""
    print("=" * 50)
    print("NMEA报文模拟器配置")
    print("=" * 50)
    print(f"初始纬度: {config.get('latitude', 39.9042):.6f}°")
    print(f"初始经度: {config.get('longitude', 116.4074):.6f}°")
    print(f"初始高度: {config.get('altitude', 50.0):.1f} 米")
    print(f"初始方向角: {config.get('heading', 0.0):.1f}°")
    print(f"初始速度: {config.get('speed', 5.0):.1f} 米/秒")
    print(f"更新率: {config.get('update_rate', 1)} Hz")
    print(f"波特率: {config.get('baudrate', 4800)} bps")
    print(f"扰动率: {config.get('noise_rate', 0.1):.2f}")
    route_type = config.get('route_type', 'linear')
    route_desc = {'linear': '直线', 'roundtrip': '往返', 'circle': '圆形'}
    print(f"路线类型: {route_type} ({route_desc.get(route_type, '未知')})")
    if route_type == 'roundtrip':
        print(f"往返距离N: {config.get('roundtrip_distance', 200.0):.1f} 米")
        print(f"往返间隔m: {config.get('roundtrip_gap', 1.0):.1f} 米")
        print(f"折返点速度: {config.get('roundtrip_turn_speed', 2.0):.1f} 米/秒")
        turn_dir = config.get('roundtrip_turn_direction', 'clockwise')
        print(f"折返旋转方向: {turn_dir} ({'顺时针' if turn_dir == 'clockwise' else '逆时针'})")
    elif route_type == 'circle':
        print(f"圆形半径: {config.get('circle_radius', 10.0):.1f} 米")
        print(f"圆形初始线速度: {config.get('circle_speed', 2.0):.1f} 米/秒")
        print(f"圆形初始线速度方位角: {config.get('circle_heading', 0.0):.1f} 度")
        circle_dir = config.get('circle_direction', 'clockwise')
        print(f"圆形旋转方向: {circle_dir} ({'顺时针' if circle_dir == 'clockwise' else '逆时针'})")
    print(f"终端回显: {'启用' if config.get('echo', True) else '禁用'}")
    print(f"发送模式: {'无阻塞模式' if config.get('non_block_mode', True) else '阻塞模式'}")
    print("-" * 50)
    print("报文类型:")
    print(f"  GGA: {'启用' if config.get('enable_gga', True) else '禁用'}")
    print(f"  GLL: {'启用' if config.get('enable_gll', True) else '禁用'}")
    print(f"  GSA: {'启用' if config.get('enable_gsa', True) else '禁用'}")
    print(f"  GSV: {'启用' if config.get('enable_gsv', True) else '禁用'}")
    print(f"  RMC: {'启用' if config.get('enable_rmc', True) else '禁用'}")
    print(f"  VTG: {'启用' if config.get('enable_vtg', True) else '禁用'}")
    print("=" * 50)


def print_help_info():
    """打印友好的参数配置说明"""
    print("=" * 60)
    print("NMEA报文模拟生成器 - 参数配置说明")
    print("=" * 60)
    print()
    print("【位置参数】")
    print("  --lat <纬度>      初始纬度 (度, 默认: 39.9042)")
    print("  --lon <经度>      初始经度 (度, 默认: 116.4074)")
    print("  --alt <高度>      初始高度 (米, 默认: 10.0)")
    print()
    print("【运动参数】")
    print("  --heading <角度>  初始方向角 (度, 0=正北, 90=正东, 默认: 135)")
    print("  --speed <速度>   初始速度 (米/秒, 默认: 2.5)")
    print()
    print("【路线类型】")
    print("  --route <类型>    路线类型 (默认: roundtrip)")
    print("                    linear     - 直线运动")
    print("                    roundtrip  - 往返运动")
    print("                    circle     - 圆形运动")
    print()
    print("【往返运动参数】(当 --route=roundtrip 时使用)")
    print("  --roundtrip-distance <距离>      往返距离N (米, 默认: 200.0)")
    print("  --roundtrip-gap <间隔>           往返间隔m (米, 默认: 1.2)")
    print("  --roundtrip-turn-speed <速度>   折返点速度 (米/秒, 默认: 2.0)")
    print("  --roundtrip-turn-direction <方向> 折返旋转方向 (clockwise=顺时针, counterclockwise=逆时针)")
    print()
    print("【圆形运动参数】(当 --route=circle 时使用)")
    print("  --circle-radius <半径>       圆形路线半径 (米, 默认: 10.0)")
    print("  --circle-speed <速度>        圆形初始线速度绝对值 (米/秒, 默认: 2.0)")
    print("  --circle-heading <角度>     圆形初始线速度方位角 (度, 0=正北, 默认: 0)")
    print("  --circle-direction <方向>   圆形旋转方向")
    print("                    clockwise      - 顺时针")
    print("                    counterclockwise - 逆时针")
    print()
    print("【报文参数】")
    print("  --rate <频率>     报文更新率 (Hz, 默认: 1)")
    print("  --noise <扰动>    扰动率 (0-1, 默认: 0.1)")
    print()
    print("【报文类型开关】")
    print("  --gga/--no-gga    GGA报文启用/禁用 (默认: 启用)")
    print("  --gll/--no-gll    GLL报文启用/禁用 (默认: 启用)")
    print("  --gsa/--no-gsa    GSA报文启用/禁用 (默认: 启用)")
    print("  --gsv/--no-gsv    GSV报文启用/禁用 (默认: 启用)")
    print("  --rmc/--no-rmc   RMC报文启用/禁用 (默认: 启用)")
    print("  --vtg/--no-vtg    VTG报文启用/禁用 (默认: 启用)")
    print()
    print("【输出模式】")
    print("  --mode <模式>     输出模式 (默认: serial)")
    print("                    console - 输出到控制台")
    print("                    file    - 输出到文件")
    print("                    serial  - 输出到串口")
    print()
    print("【串口参数】")
    print("  --port <端口>     串口名称 (默认: COM6)")
    print("  --baudrate <波特率> 波特率 (默认: 115200)")
    print("  --non-block     无阻塞模式，数据立即发送 (默认)")
    print("  --block         阻塞模式，等待缓冲区清空后发送下一帧")
    print()
    print("【其他参数】")
    print("  --duration <秒>  运行时长 (秒, 默认: 无限)")
    print("  --output <文件>  输出文件名 (默认: nmea_output.txt)")
    print("  --satellites <数> 可见卫星数量 (默认: 18)")
    print("  --echo/--no-echo 是否回显到终端 (默认: 回显)")
    print("  --list-ports     列出可用串口")
    print()
    print("=" * 60)
    print("【使用示例】")
    print()
    print("# 1. 圆形运动示例 (半径20m, 速度3m/s, 向东, 顺时针)")
    print("python nmea_generator.py --mode console --route circle \\")
    print("    --lat 39.9042 --lon 116.4074 \\")
    print("    --circle-radius 20 --circle-speed 3.0 \\")
    print("    --circle-heading 90 --circle-direction clockwise")
    print()
    print("# 2. 往返运动示例 (往返距离100m, 间隔0.5m)")
    print("python nmea_generator.py --mode console --route roundtrip \\")
    print("    --lat 39.9042 --lon 116.4074 \\")
    print("    --roundtrip-distance 100 --roundtrip-gap 0.5")
    print()
    print("# 3. 直线运动示例 (向正北运动)")
    print("python nmea_generator.py --mode console --route linear \\")
    print("    --lat 39.9042 --lon 116.4074 --heading 0 --speed 5.0")
    print()
    print("=" * 60)


def list_available_ports():
    """列出所有可用的串口"""
    ports = serial.tools.list_ports.comports()
    print("=" * 50)
    print("可用串口列表")
    print("=" * 50)
    if not ports:
        print("未发现可用的串口")
    else:
        for port in ports:
            print(f"  {port.device}: {port.description}")
    print("=" * 50)


def main():
    """主函数"""
    parser = argparse.ArgumentParser(
        description='NMEA报文模拟生成器',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
使用示例:
  # 输出到控制台测试 (10秒)
  python nmea_generator.py --mode console --duration 10

  # 输出到文件
  python nmea_generator.py --mode file --output nmea_output.txt

  # 发送到虚拟串口 (需要先创建虚拟串口对)
  python nmea_generator.py --mode serial --port COM5

  # 自定义参数
  python nmea_generator.py --mode console --lat 39.9 --lon 116.4 --speed 2.5 --heading 45 --rate 5 --noise 0.2
        """
    )

    # 模式选择
    parser.add_argument('--mode', choices=['console', 'file', 'serial'],
                        default='serial', help='输出模式')

    # 位置参数
    parser.add_argument('--lat', type=float, default=36.36,
                        help='初始纬度 (度, 默认: 36.36 - 青岛)')
    parser.add_argument('--lon', type=float, default=120.677777,
                        help='初始经度 (度, 默认: 120.677777 - 青岛)')
    parser.add_argument('--alt', type=float, default=10.0,
                        help='初始高度 (米, 默认: 10.0)')

    # 运动参数
    parser.add_argument('--heading', type=float, default=60,
                        help='初始方向角 (度, 默认: 45)')
    parser.add_argument('--speed', type=float, default=1.0,
                        help='初始速度 (米/秒, 默认: 1.0)')
    parser.add_argument('--route', dest='route_type', type=str, default='roundtrip',
                        choices=['linear', 'roundtrip', 'circle'],
                        help='路线类型: linear=直线, roundtrip=往返, circle=圆形 (默认: roundtrip)')
                        
    parser.add_argument('--roundtrip-distance', type=float, default=240.0,
                        help='往返运动往返距离N (米, 默认: 400.0)')
    parser.add_argument('--roundtrip-gap', type=float, default=2.4,
                        help='往返运动间隔m (米, 默认: 2.4)')
    parser.add_argument('--roundtrip-turn-speed', type=float, default=0.5,
                        help='往返运动折返点速度 (米/秒, 默认: 0.5)')
    parser.add_argument('--roundtrip-turn-direction', type=str, default='clockwise',
                        choices=['clockwise', 'counterclockwise'],
                        help='往返运动折返旋转方向 (默认: clockwise)')

    parser.add_argument('--circle-radius', type=float, default=10.0,
                        help='圆形路线半径 (米, 默认: 10.0)')
    parser.add_argument('--circle-speed', type=float, default=2.0,
                        help='圆形路线初始线速度绝对值 (米/秒, 默认: 2.0)')
    parser.add_argument('--circle-heading', type=float, default=0.0,
                        help='圆形路线初始线速度方位角 (度, 默认: 0, 正北为0度)')
    parser.add_argument('--circle-direction', type=str, default='clockwise',
                        choices=['clockwise', 'counterclockwise'],
                        help='圆形路线旋转方向: clockwise=顺时针, counterclockwise=逆时针 (默认: clockwise)')

    # 报文参数
    parser.add_argument('--rate', type=int, default=20,
                        help='报文更新率 (Hz, 默认: 20)')
    parser.add_argument('--noise', type=float, default=0.0,
                        help='扰动率 (0-1, 默认: 0.2)')

    # 报文类型
    parser.add_argument('--gga', action='store_true', default=True,
                        help='启用GGA报文')
    parser.add_argument('--gll', action='store_true', default=True,
                        help='启用GLL报文')
    parser.add_argument('--gsa', action='store_true', default=True,
                        help='启用GSA报文')
    parser.add_argument('--gsv', action='store_true', default=True,
                        help='启用GSV报文')
    parser.add_argument('--rmc', action='store_true', default=True,
                        help='启用RMC报文')
    parser.add_argument('--vtg', action='store_true', default=True,
                        help='启用VTG报文')

    # 输出参数
    parser.add_argument('--output', type=str, default='nmea_output.txt',
                        help='输出文件名 (默认: nmea_output.txt)')
    parser.add_argument('--port', type=str, default='COM6',
                        help='串口名称 (默认: COM6)')
    parser.add_argument('--baudrate', type=int, default=115200,
                        choices=[4800, 9600, 19200, 38400, 57600, 115200],
                        help='波特率 (默认: 4800, 可选: 4800/9600/19200/38400/57600/115200)')
    parser.add_argument('--duration', type=int, default=None,
                        help='运行时长 (秒, 默认: 无限)')
    parser.add_argument('--satellites', type=int, default=18,
                        help='可见卫星数量 (默认: 18)')
    parser.add_argument('--echo', action='store_true', default=True,
                        help='是否回显报文到终端 (默认: True)')
    parser.add_argument('--no-echo', action='store_false', dest='echo',
                        help='禁止回显报文到终端')
    parser.add_argument('--non-block', action='store_true', default=True,
                        help='无阻塞模式，数据立即发送 (默认)')
    parser.add_argument('--block', action='store_false', dest='non_block_mode',
                        help='阻塞模式，等待缓冲区清空后发送下一帧')
    parser.add_argument('--list-ports', action='store_true',
                        help='列出所有可用的串口并退出')
    parser.add_argument('--instruction', action='store_true',
                        help='打印详细的参数配置说明')

    args = parser.parse_args()

    # 打印友好的帮助信息
    if args.instruction:
        print_help_info()
        return

    # 如果只是列出串口
    if args.list_ports:
        list_available_ports()
        return

    # 构建配置
    config = {
        'latitude': args.lat,
        'longitude': args.lon,
        'altitude': args.alt,
        'heading': args.heading,
        'speed': args.speed,
        'update_rate': args.rate,
        'noise_rate': args.noise,
        'route_type': args.route_type,
        'roundtrip_distance': args.roundtrip_distance,
        'roundtrip_gap': args.roundtrip_gap,
        'roundtrip_turn_speed': args.roundtrip_turn_speed,
        'roundtrip_turn_direction': args.roundtrip_turn_direction,
        'circle_radius': args.circle_radius,
        'circle_speed': args.circle_speed,
        'circle_heading': args.circle_heading,
        'circle_direction': args.circle_direction,
        'enable_gga': args.gga,
        'enable_gll': args.gll,
        'enable_gsa': args.gsa,
        'enable_gsv': args.gsv,
        'enable_rmc': args.rmc,
        'enable_vtg': args.vtg,
        'baudrate': args.baudrate,
        'num_satellites': args.satellites,
        'echo': args.echo,
        'non_block_mode': args.non_block_mode
    }

    # 打印配置
    print_config_info(config)

    # 根据模式运行
    if args.mode == 'console':
        print("\n[控制台模式] 正在生成NMEA报文...")
        sender = FileNMEASender(config)
        sender.start_sending(duration=args.duration)

    elif args.mode == 'file':
        print(f"\n[文件模式] 正在写入NMEA报文到 {args.output}...")
        sender = FileNMEASender(config)
        if sender.open_file(args.output):
            sender.start_sending(duration=args.duration)
            sender.close()
        print(f"报文已写入文件: {args.output}")

    elif args.mode == 'serial':
        # 先列出可用串口
        list_available_ports()
        print(f"\n[串口模式] 正在通过 {args.port} 发送NMEA报文...")
        print(f"提示: 请确保目标程序已连接到配对的串口 (如 COM6 <-> COM10)")
        print("运行期间可输入 'q' 或 'x' 退出，也支持 Ctrl+C\n")
        sender = VirtualSerialNMEASender(config)
        if sender.open_serial(args.port):
            try:
                sender.start_sending(duration=args.duration)
            except KeyboardInterrupt:
                print("\n正在停止...")
                sender.stop_sending()
                sender.close()
        else:
            print(f"无法打开串口 {args.port}")
            print("\n提示: 请检查串口名称是否正确，或使用 --list-ports 查看可用串口")
            print("提示: 请确保虚拟串口已创建并可用")
            print("Windows下可使用 'com0com' 或 'VSPD' 创建虚拟串口对")


if __name__ == '__main__':
    main()


## 折返路线示例
# python nmea_generator.py --mode serial --route roundtrip --rate 5 --noise 0.2 --lat 39.9000 --lon 116.4000 --heading 45 --speed 2.5 --roundtrip-distance 100 --roundtrip-gap 2.4 --roundtrip-turn-speed 0.2 --roundtrip-turn-direction clockwise

## 圆形路线示例
# python nmea_generator.py --mode serial --route circle --lat 39.9045 --lon 116.4000 --circle-radius 0.6 --circle-speed 0.1 --circle-heading 90 --circle-direction clockwise

