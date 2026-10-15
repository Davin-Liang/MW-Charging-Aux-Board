cmake_minimum_required(VERSION 3.4.1)

project(pid_controller)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置安装路径
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    message(STATUS "Running as STANDALONE project. Setting local install prefix.")
    set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/install)
else()
    message(STATUS "Running as SUB-PROJECT. Using parent's install prefix: ${CMAKE_INSTALL_PREFIX}")
endif()

# RPATH 设置 (解决运行找不到库的问题)
set(CMAKE_SKIP_INSTALL_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

if(NOT TARGET turntable_controller_lib)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../turntable-controller ${CMAKE_CURRENT_BINARY_DIR}/pid_ctrl_build)
endif()

# 编译 PID 动态库
add_library(pid_controller_lib STATIC
    src/PID_Controller.cpp
)

# 暴露 PID 的头文件目录
target_include_directories(pid_controller_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(pid_controller_lib PUBLIC
    turntable_controller_lib
)
