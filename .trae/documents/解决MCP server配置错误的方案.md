# 优化pip源为国内高速源的方案

## 当前状态

通过 `pip config list` 命令检查，当前系统没有设置任何pip配置。

## 解决方案

### 方法 1: 临时使用国内源

在安装包时通过 `-i` 参数指定源：

```powershell
pip install 包名 -i https://pypi.tuna.tsinghua.edu.cn/simple/
```

### 方法 2: 永久设置国内源

#### 选项 1: 使用 pip config 命令设置

**清华源（推荐）**：
```powershell
pip config set global.index-url https://pypi.tuna.tsinghua.edu.cn/simple/
pip config set global.trusted-host pypi.tuna.tsinghua.edu.cn
```

**阿里云源**：
```powershell
pip config set global.index-url https://mirrors.aliyun.com/pypi/simple/
pip config set global.trusted-host mirrors.aliyun.com
```

**豆瓣源**：
```powershell
pip config set global.index-url https://pypi.douban.com/simple/
pip config set global.trusted-host pypi.douban.com
```

#### 选项 2: 手动创建配置文件

在用户目录下创建 `pip.ini` 文件（Windows系统）：

文件路径：`C:\Users\用户名\pip\pip.ini`

内容：
```ini
[global]
index-url = https://pypi.tuna.tsinghua.edu.cn/simple/
trusted-host = pypi.tuna.tsinghua.edu.cn
```

### 步骤 3: 验证设置

```powershell
pip config list
```

### 步骤 4: 测试速度

```powershell
pip install --upgrade pip
```

## 国内pip源推荐

1. **清华大学**：https://pypi.tuna.tsinghua.edu.cn/simple/
2. **阿里云**：https://mirrors.aliyun.com/pypi/simple/
3. **豆瓣**：https://pypi.douban.com/simple/
4. **中国科学技术大学**：https://pypi.mirrors.ustc.edu.cn/simple/
5. **华中科技大学**：https://pypi.hustunique.com/

## 预期结果

设置国内pip源后，包安装速度将显著提升，解决网络延迟和连接不稳定的问题。