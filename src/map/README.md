# Map 模块文档

## 概述

`src/map` 目录包含地图显示与交互相关的核心模块，负责将GPS定位数据、小区规划结果渲染到界面上，并提供坐标转换、状态估计等功能。本模块采用分层架构，各组件职责清晰，通过接口进行解耦。

---

## 模块结构与功能边界

### 1. Coordinate - 坐标转换模块

**文件**: `coordinate.h`, `coordinate.cpp`

**功能**: 负责地理坐标系统之间的转换，支持WGS84经纬度、ENU地心地固坐标系、OA本地坐标系之间的相互转换。

**核心数据结构**:
- `DMSCoordinate`: 度分秒格式坐标
- OA本地坐标系定义：
  - 原点O为参考点
  - OA方向为+Y轴
  - +X轴为+Y轴顺时针旋转90°

**核心函数**:

| 函数 | 功能 | 边界说明 |
|------|------|----------|
| `setOrigin(latO, lonO)` | 设置原点O（参考点） | 纯内部状态管理 |
| `setDirection(latA, lonA)` | 设置方向点A（定义Y轴方向） | 触发坐标系初始化 |
| `setOriginAndDirection()` | 一次性设置原点和方向点 | 组合调用上述两函数 |
| `wgs84ToENU(lat, lon)` | WGS84 → ENU | 静态转换逻辑 |
| `enuToXY(e, n)` | ENU → OA本地XY | 需要先调用setDirection |
| `wgs84ToXY(lat, lon)` | WGS84 → OA本地XY | 组合wgs84ToENU和enuToXY |
| `xyToENU(x, y)` | OA本地XY → ENU | enuToXY的逆变换 |
| `xyToWGS84(x, y)` | OA本地XY → WGS84 | 组合xyToENU和ENU反变换 |
| `isInitialized()` | 检查坐标系是否已初始化 | 状态查询 |
| `calculateDistance()` | 计算两点间WGS84距离 | 工具函数 |
| `getOADirectionAngle()` | 获取OA方向角 | 静态方法 |
| `calculateWGS84Distance()` | 静态方法：WGS84距离 | Haversine公式 |

**依赖**: `logger.h`（日志）

---

### 2. BlockGenerator - 小区规划生成模块

**文件**: `blockgenerator.h`, `blockgenerator.cpp`

**功能**: 根据区域参数生成虚拟小区、过道、边界和控制点，负责播种区域的几何计算。

**核心数据结构**:
- `BlockSpec`: 小区规格（编号、垄号范围、位置、尺寸）
- `HeadlandSpec`: 过道规格
- `BoundaryVertex`: 边界顶点
- `BlockBoundary`: 小区边界（含正向/反向边界）
- `BlockControlPoint`: 控制点
- `BlockPlanResult`: 完整的小区规划结果

**核心函数**:

| 函数 | 功能 | 边界说明 |
|------|------|----------|
| `setRowSpacing()` | 设置垄距 | 参数校验 |
| `setHeadlandWidth()` | 设置过道宽度 | 参数校验 |
| `setRowsPerBlock()` | 设置每小区垄数 | 参数校验 |
| `setFieldLength()` | 设置小区长度 | 参数校验 |
| `setAreaRange()` | 设置区域范围 | 参数校验 |
| `setTriggerDistance()` | 设置开始触发距离 | 参数校验 |
| `setStopTriggerDistance()` | 设置中断触发距离 | 参数校验 |
| `generate()` | 生成完整小区规划 | 主入口，内部调用3个子生成器 |
| `getResult()` | 获取当前规划结果 | 状态查询 |
| `generateBlocksAndHeadlands()` | 生成小区和过道 | 私有，核心几何计算 |
| `generateBoundaries()` | 生成边界 | 私有 |
| `generateControlPoints()` | 生成控制点 | 私有 |
| `generateSeedingGrid()` | 生成播种网格 | 私有 |
| `isSeedingAtY(y)` | 检查Y坐标是否在播种区 | 工具查询 |
| `getBlockIdAtY(y)` | 获取Y坐标所属小区编号 | 工具查询 |

**内部调用关系**:
```
generate()
├── generateBlocksAndHeadlands()  → blocks, headlands
├── generateBoundaries()          → boundaries
├── generateControlPoints()      → controlPoints
└── generateSeedingGrid()        → m_seedingGrid
```

**依赖**: `logger.h`, `<cmath>`, `<algorithm>`, `<sstream>`

---

### 3. StateEstimator - 状态估计模块

**文件**: `statestimator.h`, `statestimator.cpp`

**功能**: 接收传感器数据（GPS/IMU），进行滤波处理后输出估计的设备姿态（位置、速度、航向）。支持低通滤波和卡尔曼滤波两种模式。

**核心数据结构**:
- `EstimatedPose`: 估计姿态（x, y, vx, vy, heading, latitude, longitude, timestamp）
- `SensorData`: 传感器数据（GPS + IMU）
- `LocalPoint`: 本地坐标点（用于滑动窗口）

**核心函数**:

| 函数 | 功能 | 边界说明 |
|------|------|----------|
| `setCoordinate()` | 注入坐标转换器 | 依赖注入 |
| `setUseKalman()` | 设置滤波模式 | 参数配置 |
| `setLowPassAlpha()` | 设置低通滤波系数 | 参数配置 |
| `update(data)` | 更新状态估计 | 主入口，内部路由到具体滤波 |
| `getPose()` | 获取当前姿态 | 状态查询 |
| `reset()` | 重置估计器 | 状态清零 |
| `calibrate()` | 校准（设航向为0） | 相对角度归零 |
| `updateLowPass()` | 低通滤波更新 | 私有 |
| `updateKalman()` | 卡尔曼滤波更新 | 私有 |
| `calculateVelocity()` | 从传感器数据计算速度 | 私有工具 |
| `estimateVelocity()` | 滑动窗口线性回归 | 私有 |
| `checkCrossing()` | 检查是否穿越触发线 | 私有，播种触发逻辑 |

**内部调用关系**:
```
update(data)
├── wgs84ToXY() ← Coordinate
├── estimateVelocity()
├── calculateVelocity()
├── updateLowPass() / updateKalman()
└── checkCrossing()
```

**依赖**: `coordinate.h`, `logger.h`

---

### 4. OsmTileLayer - OSM底图模块

**文件**: `osmtilelayer.h`, `osmtilelayer.cpp`

**功能**: 加载和绘制OSM卫星影像底图，支持瓦片缓存、网络请求、视口计算。

**核心数据结构**:
- `ViewState`: 视口状态（尺寸、中心坐标、缩放、旋转）

**核心函数**:

| 函数 | 功能 | 边界说明 |
|------|------|----------|
| `draw(painter, viewState)` | 绘制底图 | 主入口 |
| `attributionText()` | 获取底图署名 | 静态文本 |
| `geographicToWorld()` | 经纬度→墨卡托世界坐标 | 静态工具 |
| `worldToGeographic()` | 墨卡托世界坐标→经纬度 | 静态工具 |
| `loadTile()` | 加载瓦片（本地/网络） | 私有 |
| `scheduleVisibleTiles()` | 调度可见瓦片请求 | 私有 |
| `processRequestQueue()` | 处理请求队列 | 私有 |
| `startTileRequest()` | 发起网络请求 | 私有 |
| `chooseTileZoom()` | 选择合适的缩放级别 | 私有 |
| `worldToScreen()` | 世界坐标→屏幕坐标 | 私有 |

**底图来源**: Esri World Imagery

**依赖**: `logger.h`, `QNetworkAccessManager`

---

### 5. MapWidget - 地图主组件

**文件**: `mapwidget.h`, `mapwidget.cpp`

**功能**: 地图最上层抽象，统一管理显示、交互、缩放和平移。是整个map模块的UI入口。

**核心职责**:
1. 统一管理显示、交互、缩放和平移
2. 调用Coordinate获取坐标映射
3. 调用BlockGenerator结果绘制块、过道、边界、控制点
4. 绘制实时设备位置与离散轨迹
5. 管理地图缩放、平移、选中状态

**核心函数**:

| 函数 | 功能 | 边界说明 |
|------|------|----------|
| `setCoordinate()` | 注入坐标转换器 | 依赖注入 |
| `setBlockPlanResult()` | 设置小区规划结果 | 数据注入 |
| `highlightBlock()` | 高亮选中小区 | UI状态 |
| `updateVehiclePose()` | 更新设备位置 | 主数据更新入口 |
| `clearPath()` | 清除轨迹 | UI状态 |
| `clearAll()` | 清除所有显示 | 完整重置 |
| `paintEvent()` | 绘制事件 | Qt渲染入口，调用各draw方法 |
| `mousePressEvent()` | 鼠标按下 | 交互处理 |
| `mouseMoveEvent()` | 鼠标移动 | 拖动平移 |
| `mouseReleaseEvent()` | 鼠标释放 | 释放拖动/点击 |
| `wheelEvent()` | 滚轮缩放 | 缩放控制 |
| `drawBaseMap()` | 绘制OSM底图 | 委托OsmTileLayer |
| `drawFallbackGrid()` | 绘制退化网格 | 无底图时降级 |
| `drawLocalAxes()` | 绘制本地坐标轴 | UI绘制 |
| `drawBlocks()` | 绘制小区和边界 | UI绘制 |
| `drawSelectedBoundary()` | 绘制选中边界 | UI绘制 |
| `drawTrajectory()` | 绘制轨迹点 | UI绘制 |
| `drawVehiclePose()` | 绘制设备位置 | UI绘制 |
| `drawCompass()` | 绘制指南针 | UI绘制 |
| `drawRecenterButton()` | 绘制归位按钮 | UI绘制 |
| `drawScaleBar()` | 绘制比例尺 | UI绘制 |
| `worldToScreen()` | 世界→屏幕坐标 | 坐标转换 |
| `screenToWorld()` | 屏幕→世界坐标 | 坐标转换 |
| `localToWorld()` | 本地→世界坐标 | 坐标转换，调用Coordinate |
| `geographicToWorld()` | 经纬度→世界坐标 | 委托OsmTileLayer |
| `worldToGeographic()` | 世界坐标→经纬度 | 委托OsmTileLayer |

**内部调用关系**:
```
paintEvent()
├── drawBaseMap() → OsmTileLayer::draw()
├── drawFallbackGrid()
├── drawLocalAxes()
├── drawBlocks()
├── drawSelectedBoundary()
├── drawTrajectory()
├── drawVehiclePose()
├── drawCompass()
├── drawRecenterButton()
├── drawScaleBar()
└── drawAttribution()

updateVehiclePose()
├── resolvePoseGeographicPosition()
├── ensureGeographicContext()
└── 轨迹更新

交互事件 → 视图变换/缩放 → update()
```

**依赖**: `coordinate.h`, `blockgenerator.h`, `osmtilelayer.h`, `statestimator.h`

---

### 6. TableWidget - 表格组件

**文件**: `tablewidget.h`, `tablewidget.cpp`

**功能**: 以表格形式显示小区数据（类似Excel界面），仅负责展示和选择，不负责几何计算。

**核心函数**:

| 函数 | 功能 | 边界说明 |
|------|------|----------|
| `setBlockPlanResult()` | 设置小区数据 | 数据注入 |
| `updateBlockStatus()` | 更新播种状态 | UI更新 |
| `clearData()` | 清空数据 | UI清空 |
| `getSelectedBlockId()` | 获取选中小区 | 状态查询 |
| `getBlockPlanResult()` | 获取规划结果 | 数据回溯 |
| `initTable()` | 初始化表格 | 私有配置 |
| `updateTableData()` | 更新表格内容 | 私有实现 |

**信号**:
- `blockSelected(int blockId)`: 用户点击小区行时发出

**依赖**: `blockgenerator.h`, `logger.h`

---

## 模块调用关系图

```
┌─────────────────────────────────────────────────────────────────────┐
│                              UI 层                                   │
│  ┌─────────────────────┐              ┌─────────────────────────┐  │
│  │     MapWidget       │              │      TableWidget        │  │
│  │   (地图主组件)       │◄────────────►│    (表格显示组件)        │  │
│  └──────────┬──────────┘              └────────────┬────────────┘  │
│             │                                         │              │
│             │ 依赖注入                                │ 数据同步     │
│             ▼                                         ▼              │
│  ┌─────────────────────────────────────────────────────────────┐    │
│  │                    Coordinate (坐标转换)                    │    │
│  │         WGS84 ↔ ENU ↔ OA本地XY                            │    │
│  └─────────────────────────────────────────────────────────────┘    │
│             ▲                                                       │
│             │ 依赖注入                                              │
│             │                                                       │
│  ┌──────────┴──────────┐                                         │
│  │   BlockGenerator    │                                         │
│  │    (小区规划生成)    │                                         │
│  └─────────────────────┘                                         │
│                                                                       │
└───────────────────────────────────────────────────────────────────────┘
                                    ▲
                                    │ 依赖注入
                                    │
┌───────────────────────────────────┴───────────────────────────────────┐
│                         业务逻辑层                                     │
│  ┌─────────────────────┐                                           │
│  │   StateEstimator    │                                           │
│  │    (状态估计)        │                                           │
│  └─────────────────────┘                                           │
└───────────────────────────────────────────────────────────────────────┘
                                    ▲
                                    │ 网络请求
                                    │
┌───────────────────────────────────┴───────────────────────────────────┐
│                         外部服务                                      │
│  ┌─────────────────────┐                                           │
│  │    OsmTileLayer     │                                           │
│  │    (OSM底图)        │                                           │
│  └─────────────────────┘                                           │
└───────────────────────────────────────────────────────────────────────┘
```

---

## 依赖关系总结

| 模块 | 依赖 | 被依赖 |
|------|------|--------|
| Coordinate | logger.h | StateEstimator, MapWidget |
| BlockGenerator | logger.h | MapWidget, TableWidget |
| StateEstimator | Coordinate, logger.h | - |
| OsmTileLayer | logger.h, QNetworkAccessManager | MapWidget |
| MapWidget | Coordinate, BlockGenerator, OsmTileLayer, StateEstimator, logger.h | UI层主入口 |
| TableWidget | BlockGenerator, logger.h | UI层辅助 |

---

## 关键设计原则

1. **依赖注入**: MapWidget和StateEstimator通过setCoordinate()接收Coordinate指针，避免内部耦合
2. **分层解耦**: UI层(MapWidget/TableWidget) → 业务层(StateEstimator/BlockGenerator) → 底层工具(Coordinate/OsmTileLayer)
3. **静态工具方法**: Coordinate和OsmTileLayer提供静态坐标转换方法，无需实例化
4. **降级处理**: MapWidget在无底图时自动降级到网格背景
5. **坐标系约定**: 所有本地坐标均采用OA坐标系（原点在O，+Y沿OA方向）
