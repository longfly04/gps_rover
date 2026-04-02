# 解决MCP server找不到uvx的方案

## 问题分析

错误信息 `spawn uvx ENOENT` 表明MCP server无法找到 `uvx` 可执行文件。通过检查，我发现：

1. `uv` 工具已经安装成功（版本 0.9.27）
2. `uvx` 可执行文件存在于路径 `D:\miniconda\envs\gps_seeder\Scripts\uvx.exe`
3. 该路径在当前终端的PATH中存在，但在系统级别的PATH中不存在

## 解决方案

### 步骤 1: 将uv的路径添加到系统环境变量

在Windows系统中：

1. 右键点击「此电脑」→「属性」→「高级系统设置」→「环境变量」
2. 在「系统变量」中找到「Path」变量，点击「编辑」
3. 点击「新建」，添加路径 `D:\miniconda\envs\gps_seeder\Scripts`
4. 点击「确定」保存所有更改

### 步骤 2: 重启计算机

为了确保环境变量更改完全生效，建议重启计算机。

### 步骤 3: 验证环境变量

重启后，在PowerShell中运行以下命令验证：

```powershell
$env:PATH
```

确保输出中包含 `D:\miniconda\envs\gps_seeder\Scripts` 路径。

### 步骤 4: 重启Trae IDE

重启Trae IDE以确保它使用更新后的环境变量。

### 步骤 5: 重新启动MCP server

在IDE中重新启动MCP server，检查是否还有错误。

## 备选方案

如果上述方法不起作用，可以尝试以下备选方案：

### 方案 A: 重新安装uv工具

使用官方安装脚本重新安装uv，确保它被安装到系统级别的路径中：

```powershell
powershell -ExecutionPolicy ByPass -c " irm https://astral.sh/uv/install.ps1 | iex"
```

### 方案 B: 创建uvx的符号链接

在系统PATH中已存在的目录中创建uvx的符号链接：

```powershell
New-Item -ItemType SymbolicLink -Path "C:\Windows\System32\uvx.exe" -Target "D:\miniconda\envs\gps_seeder\Scripts\uvx.exe"
```

## 预期结果

通过以上步骤，MCP server应该能够成功找到并启动uvx，不再出现 `spawn uvx ENOENT` 错误。
