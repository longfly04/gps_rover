cmake_minimum_required(VERSION 3.16)

project(GPSSeeder VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Windows MSVC编译器特殊处理
if(WIN32 AND MSVC)
    # MSVC需要特殊标志来正确支持C++17和Qt
    add_compile_options(/Zc:__cplusplus)  # 正确报告__cplusplus宏
    add_compile_options(/permissive-)      # 严格标准一致性
    add_compile_options(/utf-8)            # UTF-8源文件编码
endif()

# 先找到核心组件
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network SerialPort SerialBus Quick QuickWidgets Positioning Location)

# 部署开关：构建后自动把 Qt 依赖 DLL/插件输出到 build 目录（供 GPSSeeder.iss 打包）
option(GPSSEEDER_WINDEPLOYQT "Run windeployqt after build" OFF)

qt_add_executable(GPSSeeder
    src/main.cpp
    src/ui/mainwindow.cpp
    src/map/tablewidget.cpp
    src/map/mapwidget.cpp
    src/map/osmtilelayer.cpp

    src/map/blockgenerator.cpp
    src/core/sensor/sensorinterface.cpp
    src/database/dbmanager.cpp
    # KF-GINS相关源文件
    src/core/common/time_sync.cpp
    src/core/common/logger.cpp
    src/core/fileio/gnssserialloader.cpp

    src/core/fileio/imuserialloader.cpp

    src/core/fileio/serialport.cpp
    src/map/statestimator.cpp
    src/map/coordinate.cpp
    src/ui/mainwindow.ui
)

# 添加包含目录
target_include_directories(GPSSeeder PRIVATE
    src
    src/core
    src/core/sensor
    src/core/common
    src/core/fileio
    src/map
    src/database
    src/ui
    # ThirdParty库路径
    src/ThirdParty/eigen-3.3.9
    src/ThirdParty/yaml-cpp-0.7.0/include
)

target_link_libraries(GPSSeeder PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Network Qt6::SerialPort Qt6::SerialBus Qt6::Quick Qt6::QuickWidgets Qt6::Positioning Qt6::Location)

# 链接QtSql库（用于数据库操作）
find_package(Qt6 REQUIRED COMPONENTS Sql)
target_link_libraries(GPSSeeder PRIVATE Qt6::Sql)

set_target_properties(GPSSeeder PROPERTIES
    WIN32_EXECUTABLE TRUE
)

# -----------------------------
# Windows 部署：将运行所需 DLL/插件拷贝到 build 目录
# 目标：匹配 GPSSeeder.iss 的打包输入：
#   build/GPSSeeder.exe
#   build/*.dll
#   build/platforms/*, iconengines/*, imageformats/*, styles/*
# -----------------------------
if(WIN32 AND GPSSEEDER_WINDEPLOYQT)
    # Qt6::qmake 提供 qmake 可执行程序路径，可用于定位 windeployqt
    get_target_property(_qt_qmake_exe Qt6::qmake IMPORTED_LOCATION)
    if(_qt_qmake_exe)
        get_filename_component(_qt_bin_dir "${_qt_qmake_exe}" DIRECTORY)
        set(_qt_windeployqt_exe "${_qt_bin_dir}/windeployqt.exe")

        if(EXISTS "${_qt_windeployqt_exe}")
            add_custom_command(TARGET GPSSeeder POST_BUILD
                COMMAND "${_qt_windeployqt_exe}"
                        --no-translations
                        --no-system-d3d-compiler
                        --no-opengl-sw
                        --verbose 0
                        "$<TARGET_FILE:GPSSeeder>"
                WORKING_DIRECTORY "$<TARGET_FILE_DIR:GPSSeeder>"
                COMMENT "Running windeployqt to stage Qt DLLs/plugins into build output directory"
            )
        else()
            message(WARNING "windeployqt.exe not found next to qmake: ${_qt_windeployqt_exe}. Packaging may miss Qt DLLs/plugins.")
        endif()
    else()
        message(WARNING "Qt6::qmake IMPORTED_LOCATION not found; cannot locate windeployqt.exe automatically.")
    endif()
endif()

