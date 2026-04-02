# GPSSeeder

GPSSeeder 是一个基于 Qt 6 的桌面应用，用于农机播种场景下的 GNSS/IMU 数据接入、地块可视化、触发控制与 PLC/Modbus 联动。

## 主要功能

- GNSS / IMU 串口数据采集与状态显示
- 地块、分割线与作业区域可视化
- 播种触发流程管理与历史记录展示
- PLC / Modbus TCP 联动控制
- SQLite 本地数据存储

## 构建

> 当前以 **CMake** 为准，`GPSSeeder.pro` 不再作为维护入口。

```sh
cmake -S . -B build
cmake --build build
```

## 运行

```sh
./build/GPSSeeder
```

## 依赖

- C++17
- CMake 3.16+
- Qt 6：Core、Gui、Widgets、Network、SerialPort、SerialBus、Quick、QuickWidgets、Positioning、Location、Sql

## 目录

- `src/core/`：传感器、串口、通用基础能力
- `src/map/`：地图显示、地块生成、作业相关逻辑
- `src/database/`：SQLite 数据存取
- `src/ui/`：主界面与 UI 定义
- `scripts/`：联调辅助脚本

## 说明

- UI 结构修改请以 `src/ui/mainwindow.ui` 为准。
- 仓库默认不提交构建产物、缓存、备份目录、内部说明文档及本地资源。
- 如需 Windows 打包，可参考 `GPSSeeder.iss`。
