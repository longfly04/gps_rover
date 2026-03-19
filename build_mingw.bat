@echo off
REM 使用MinGW编译器构建项目

REM 设置Qt路径
set QT_DIR=D:\Qt6\6.10.2\mingw_64
set PATH=%QT_DIR%\bin;D:\msys64\mingw64\bin;%PATH%

REM 清理旧的构建文件
if exist build (
    echo Cleaning old build directory...
    rmdir /s /q build
)

REM 创建构建目录
mkdir build
cd build

REM 使用MinGW生成器配置CMake
echo Configuring with MinGW...
cmake -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH=%QT_DIR% ..

REM 编译
echo Building...
cmake --build .

echo.
echo Build complete!
pause
