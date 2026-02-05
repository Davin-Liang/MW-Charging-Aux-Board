cmake_minimum_required(VERSION 3.10)

# 1. 环境与项目基础设置
set(Qt5_DIR "/opt/Qt5.14.2/5.14.2/gcc_64/lib/cmake/Qt5")
project(PowerOptimizer VERSION 1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 2. 查找 Qt 组件
find_package(Qt5 REQUIRED COMPONENTS Core Gui Network Widgets Charts)

# 3. 开启 Qt 自动处理工具
# 注意：既然下面使用了 qt5_wrap_ui 手动处理，可以关闭 AUTOUIC 以防冲突
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC OFF) 

# 4. 手动处理 UI 文件 (解决之前的 Autogen 报错)
qt5_wrap_ui(UI_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/mainwindow.ui")

# 5. 列出所有源文件 (PROJECT_SOURCES)
set(PROJECT_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/mainwindow.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tab_netural.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/PID_Controller.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/TrajectoryJsonDialog.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/command_transmitter.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tab_file_read.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tab_stm32.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tab_turntablecontrol.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tab_deviceconnect.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/turntable_controller.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/logindialog.cpp"
    # libmodbus 源码
    "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/libmodbus/src/modbus.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/libmodbus/src/modbus-data.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/libmodbus/src/modbus-rtu.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/libmodbus/src/modbus-tcp.c"
)

# 6. 列出所有头文件 (PROJECT_HEADERS)
set(PROJECT_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/PID_Controller.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/TrajectoryJsonDialog.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/command_struct.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/command_transmitter.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/tab_file_read.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/tab_netural.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/tab_stm32.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/tab_turntablecontrol.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/tab_deviceconnect.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/turntable_controller.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/logindialog.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/mainwindow.h"
    # libmodbus 头文件
    "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/libmodbus/src/modbus.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/libmodbus/src/modbus-rtu.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/libmodbus/src/modbus-tcp.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/nlohmann/json.hpp"
)

# 7. 设置包含目录
include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty"
    "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/libmodbus/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_BINARY_DIR}" # 重要：手动生成的 UI 头文件在这里
)

# 8. 生成可执行程序 (只调用这一次 add_executable)
add_executable(PowerOptimizer
    ${PROJECT_SOURCES}
    ${PROJECT_HEADERS}
    ${UI_HEADERS}
)

# 9. 链接库
target_link_libraries(PowerOptimizer PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Network
    Qt5::Widgets
    Qt5::Charts
)

if(WIN32)
    target_link_libraries(PowerOptimizer PRIVATE ws2_32)
endif()
