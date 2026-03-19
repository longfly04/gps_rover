# 纯依赖GPS估算的厘米级定距离触发方案

方案核心需求：

1. 仅依赖GPS数据，不使用IMU，实现每隔一段距离精确触发控制指令
2. 能够在车辆以低速（如2-5m/s）时，每隔5米间隔（此参数可配置，可变化），误差不超过厘米级，准确触发一次控制指令
3. 车辆在野外空旷环境中，做相对匀速的直线运动，GPS信号相对稳定
4. 以上触发动作避免误差积累，确保在车辆以低速运行时，保证每隔触发指令位置之间间隔的相对稳定性

一、核心思路：从“瞬时跳变”到“趋势拟合”

痛点：
频率低：10Hz意味着每100ms才有一个点，车跑得快时，两点间距可能达1-2米。
噪声大：GPS点位会在真实轨迹周围随机跳动（几米范围）。
速度不准：直接用 (P\_n - P\_{n-1}) / Delta t 算出的速度会有巨大抖动，导致预估时间 t=d/v 极不稳定。

解决方案：
坐标系转换：依然必须转为 ENU (局部平面直角坐标)，避免经纬度计算误差。
滑动窗口速度滤波：不只看最近两个点，而是看最近5-10个点，用最小二乘法拟合出一条直线，求出平均速度和运动方向。这能极大消除GPS噪点带来的速度突变。
线性插值预测：假设车辆在短时间内（100ms内）做匀速直线运动，利用最后一段的向量进行线性外推。
动态阈值触发：不再死板地倒计时，而是根据“当前点”和“下一点”的连线方程，计算连线与触发线的交点，如果交点在两点之间，则判定为“即将穿过”，并根据比例系数估算精确时刻。

二、算法流程梳理

步骤 1：数据缓冲与预处理
维护一个长度为 N（例如 N=8，对应约0.8秒数据）的环形缓冲区 HistoryBuffer，存储最近的ENU坐标 (x, y) 和时间戳 t。
每次收到新GPS点，转换坐标后推入缓冲区。
如果缓冲区未满，不进行触发计算（等待数据积累）。

步骤 2：状态估算（滑动窗口拟合）
利用缓冲区中的 N 个点，计算当前的最佳估计速度 (v\_{est}) 和 航向 (theta\_{est})。
方法：对 x(t) 和 y(t) 分别进行线性回归（Linear Regression）。
拟合公式：x = k\_x cdot t + b\_x, y = k\_y cdot t + b\_y
斜率 k\_x, k\_y 即为 x, y 方向的分速度。
v\_{est} = sqrt{k\_x^2 + k\_y^2}
theta\_{est} = text{atan2}(k\_x, k\_y)
优势：这种方法天然过滤了单个点的GPS跳变，得到的速度非常平滑，适合用于预测。

步骤 3：穿越判定与时间预估
设下一个触发线的坐标为 Y\_{target}（假设主要沿Y轴运动）。
取缓冲区最后两个点：P\_{last} (x\_1, y\_1, t\_1) 和 P\_{curr} (x\_2, y\_2, t\_2)。
判断穿越：
如果 (y\_1  Y\_{target} ge y\_2)，说明线段 P\_{last} to P\_{curr} 穿过了触发线。
计算穿越比例 (alpha)：
alpha = frac{Y\_{target} - y\_1}{y\_2 - y\_1}
alpha 代表触发点在线段上的位置比例（0\~1之间）。
计算预估时间偏移 (Delta t\_{offset})：
Delta t\_{total} = t\_2 - t\_1 (通常是0.1s)
Delta t\_{offset} = alpha cdot Delta t\_{total}
这意味着：触发时刻大约在 t\_1 + Delta t\_{offset}。
计算剩余时间 (t\_{remain})：
当前时间是 t\_{now} (通常接近 t\_2)。
t\_{trigger\_estimated} = t\_1 + Delta t\_{offset}
t\_{remain} = t\_{triggerestimated} - t{now}
注意：由于GPS延迟，t\_{now} 可能已经略大于 t\_{triggerestimated}，此时 t{remain} 为负，表示已经过了，需要立即触发并记录误差。

步骤 4：执行逻辑（带迟滞比较器）
为了防止GPS噪声导致在触发线附近反复横跳（例如：点A在5.01m，点B在4.99m，点C在5.02m），必须引入状态机锁。

状态 IDLE：寻找下一个目标 Y\_{target}。
状态 ARMED：当检测到“即将穿越”（例如距离目标小于 2米，且速度方向朝向目标）时进入。
在 ARMED 状态下，持续运行上述“步骤3”计算 t\_{remain}。
一旦计算出 t\_{remain} le T\_{delay}（或者发现已经穿越），立即触发。
触发后，强制切换到 COOLDOWN 状态。
状态 COOLDOWN：忽略所有触发逻辑，直到车辆明确越过目标线一定距离（例如 Y\_{curr} > Y\_{target} + 1.0m），然后回到 IDLE 并指向下一个目标。

三、伪代码实现 (纯GPS版)

include
include
include

struct GpsPoint {
double x; // ENU East
double y; // ENU North
qint64 timestamp\_ms;
};

class PureGpsTriggerController : public QObject {
Q\_OBJECT
public:
PureGpsTriggerController() {
m\_historyMaxSize = 10; // 保留最近1秒数据 (10Hz \* 1s)
m\_sysDelayMs = 50;     // 系统延迟 50ms
m\_deadzoneM = 1.0;     // 触发后死区 1米
}

```
void setTargetInterval(double meters) { m_interval = meters; }
void updateGpsData(double lat, double lon, qint64 ts);
```

signals:
void triggerExecuted(int targetId, double estimatedLat, double estimatedLon);

private:
// 核心算法：线性回归求速度
void estimateVelocity(double \&vx, double \&vy);

```
// 核心算法：计算穿越时间
void checkCrossing();

QList m_buffer;
int m_historyMaxSize;

double m_interval = 5.0;
int m_nextTargetIndex = 0;
double m_currentTargetY = 0.0;

bool m_isArmed = false; // 是否已锁定当前目标
bool m_hasTriggered = false; // 当前目标是否已触发

int m_sysDelayMs;
double m_deadzoneM;

// 参考点 (需在初始化时设置)
double m_refLat = 0.0;
double m_refLon = 0.0;
```

};

void PureGpsTriggerController::updateGpsData(double lat, double lon, qint64 ts) {
// 1. 坐标转换 (假设已有函数)
double x, y;
geoToENU(lat, lon, m\_refLat, m\_refLon, x, y);

```
GpsPoint p = {x, y, ts};
m_buffer.append(p);
if (m_buffer.size() > m_historyMaxSize) {
    m_buffer.removeFirst();
}

// 需要足够的数据点才能开始估算
if (m_buffer.size()  0 && dist  m_currentTargetY + m_deadzoneM)) {
        m_isArmed = false;
        m_nextTargetIndex++;
        // 可选：在这里打印误差分析
    }
    // 防止漏触发：如果已经跑远还没触发（比如信号丢失后恢复），强制跳过
    else if (!m_hasTriggered && (m_buffer.back().y > m_currentTargetY + m_deadzoneM)) {
        m_isArmed = false;
        m_nextTargetIndex++; 
    }
}
```

}

void PureGpsTriggerController::estimateVelocity(double \&vx, double \&vy) {
// 使用最小二乘法拟合 x(t) 和 y(t)
// y = t + b => k = (Nsum(xy) - sum(xsum(y)) / (Nsum(x^2) - (sum(x))^2)
// 这里 x代表时间t, y代表坐标值

```
int n = m_buffer.size();
double sumT = 0, sumX = 0, sumY = 0, sumTX = 0, sumTY = 0, sumT2 = 0;

double t0 = m_buffer.front().timestamp_ms; // 相对时间，防止数值过大

for (const auto &p : m_buffer) {
    double t = (p.timestamp_ms - t0) / 1000.0; // 转为秒
    sumT += t;
    sumX += p.x;
    sumY += p.y;
    sumTX += t * p.x;
    sumTY += t * p.y;
    sumT2 += t * t;
}

double denom = n * sumT2 - sumT * sumT;
if (std::abs(denom) p2 是否跨越 m_currentTargetY
bool crossing = false;
if (p1.y = m_currentTargetY) crossing = true;
if (p1.y > m_currentTargetY && p2.y  0.1) {
        double distToGo = m_currentTargetY - p2.y;
        if (distToGo > 0 && distToGo  V_{max} times Delta t times 1.5，说明发生了GPS跳变，丢弃该点，不要让它污染缓冲区。
```

触发补偿策略：
由于纯GPS必然有滞后，如果发现连续几次触发都偏晚（误差为正），可以动态减小 m\_sysDelayMs 的值，进行软件校准。
显示策略：
界面上显示的“触发点坐标”，建议使用插值计算出的坐标（即代码中的 triggerX, triggerY），而不是触发那一刻收到的GPS坐标。前者代表了算法认为的“理想过线点”，后者充满了噪声。

五、总结

这个纯GPS方案的核心在于“用空间换时间”和“用统计换瞬时”：
利用多点线性回归代替瞬时差分，获得稳定的速度矢量。
利用线段插值代替简单的阈值判断，将触发精度从“100ms级别”提升到“插值计算的微秒级别”（虽然物理上还是受限于GPS更新率，但在逻辑上实现了平滑过渡）。

虽然没有IMU那样完美的预测能力，但在直线往复运动（工况简单）的场景下，这套算法足以将触发误差控制在0.5米以内，对于大多数非极端精密的农业机械或物流小车来说，已经是一个高性价比的解决方案。
