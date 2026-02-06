cmake_minimum_required(VERSION 3.16)

project(GPSSeeder VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 先找到核心组件
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network SerialPort Quick QuickWidgets)

# 尝试找到Positioning和Location组件
find_package(Qt6 COMPONENTS Positioning Location)

# 根据是否找到组件来设置标志
set(Qt6_Location_FOUND ${Qt6_Location_FOUND})
set(Qt6_Positioning_FOUND ${Qt6_Positioning_FOUND})

qt_add_executable(GPSSeeder
    src/main.cpp
    src/ui/mainwindow.cpp
    src/core/coordinate/coordinateconverter.cpp
    src/map/fieldmanager.cpp
    src/map/pathgenerator.cpp
    src/core/controller/predictivecontroller.cpp
    src/core/estimator/statestimator.cpp
    src/core/sensor/sensorinterface.cpp
    src/database/dbmanager.cpp
    # KF-GINS相关源文件
    src/core/common/time_sync.cpp
    src/core/common/logger.cpp
    src/core/fileio/gnssserialloader.cpp
    src/core/fileio/imuserialloader.cpp
    src/core/fileio/serialport.cpp
    src/ui/mainwindow.ui
)

# 添加包含目录
target_include_directories(GPSSeeder PRIVATE
    src
    src/core
    src/core/sensor
    src/core/estimator
    src/core/controller
    src/core/coordinate
    src/map
    src/database
    src/ui
    # KF-GINS相关包含目录

    src/core/common
    src/core/fileio
    # ThirdParty库路径
    src/ThirdParty/eigen-3.3.9
)

target_link_libraries(GPSSeeder PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Network Qt6::SerialPort Qt6::Quick Qt6::QuickWidgets)

# 链接QtPositioning库（如果找到）
if(Qt6_Positioning_FOUND)
    target_link_libraries(GPSSeeder PRIVATE Qt6::Positioning)
endif()

# 链接QtLocation库（如果找到）
if(Qt6_Location_FOUND)
    target_link_libraries(GPSSeeder PRIVATE Qt6::Location)
endif()

# 链接QtSql库（用于数据库操作）
find_package(Qt6 REQUIRED COMPONENTS Sql)
target_link_libraries(GPSSeeder PRIVATE Qt6::Sql)

set_target_properties(GPSSeeder PROPERTIES
    WIN32_EXECUTABLE TRUE
)
