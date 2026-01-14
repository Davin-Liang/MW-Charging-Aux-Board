cmake_minimum_required(VERSION 3.4.1)

project(antenna_visioner)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,--allow-shlib-undefined")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--allow-shlib-undefined") 

# 启用 C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置安装路径
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/install/)

# RPATH 设置 (解决运行找不到库的问题)
set(CMAKE_SKIP_INSTALL_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# RKNN 库路径
set(RKNN_RT_LIB ${CMAKE_SOURCE_DIR}/lib/librknnrt.so)

# RGA 库
set(RGA_LIB ${CMAKE_SOURCE_DIR}/lib/librga.so)

# 头文件路径
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/3rdparty)

# OpenCV 配置
set(OpenCV_DIR ${CMAKE_SOURCE_DIR}/3rdparty/opencv/share/OpenCV)
find_package(OpenCV REQUIRED)
message(STATUS "OpenCV libraries found: ${OpenCV_LIBS}")
set(OPENCV_LIB_SRC_DIR "${CMAKE_SOURCE_DIR}/3rdparty/opencv/lib")

# 定义可执行文件
add_executable(test_demo
    src/test_demo.cpp
    src/postprocess.cpp
    src/antenna-visioner.cpp
)

target_link_libraries(test_demo
    ${RKNN_RT_LIB}
    ${RGA_LIB}
    ${OpenCV_LIBS} 
)

# 安装规则
install(TARGETS test_demo DESTINATION ./)
install(PROGRAMS ${RKNN_RT_LIB} DESTINATION lib)
install(PROGRAMS ${RGA_LIB} DESTINATION lib)
install(DIRECTORY ${OPENCV_LIB_SRC_DIR}/
    DESTINATION lib
)
install(DIRECTORY model DESTINATION ./)