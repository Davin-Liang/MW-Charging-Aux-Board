cmake_minimum_required(VERSION 3.4.1)

project(antenna_visioner)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,--allow-shlib-undefined")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--allow-shlib-undefined") 

# 启用 C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 通过命令行参数为类选择视觉模型 
if (NOT DEFINED YOLO_VERSION)
    set(YOLO_VERSION "yolov8") # 设置默认值
endif()

# 设置安装路径
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/install/)

# RPATH 设置 (解决运行找不到库的问题)
set(CMAKE_SKIP_INSTALL_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# 头文件路径
include_directories(${CMAKE_SOURCE_DIR}/include) # 包含公共头文件
# include_directories(${CMAKE_SOURCE_DIR}/3rdparty)

# 库路径
set(RKNN_RT_LIB ${CMAKE_SOURCE_DIR}/lib/librknnrt.so) # RKNN 库路径 v2.3.0
set(RGA_LIB ${CMAKE_SOURCE_DIR}/lib/librga.so) # RGA 库

# OpenCV 配置
set(OpenCV_DIR ${CMAKE_SOURCE_DIR}/3rdparty/opencv/share/OpenCV)
find_package(OpenCV REQUIRED)
message(STATUS "OpenCV libraries found: ${OpenCV_LIBS}")
set(OPENCV_LIB_SRC_DIR "${CMAKE_SOURCE_DIR}/3rdparty/opencv/lib")

if ("${YOLO_VERSION}" STREQUAL "yolov8")
    message(STATUS "正在使用yolov8!")

    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/ 3rdparty.out)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/utils/ utils.out)

    include_directories(${CMAKE_SOURCE_DIR}/include/${YOLO_VERSION})

    add_executable(test_demo
        src/test_demo.cpp
        src/${YOLO_VERSION}/postprocess.cc
        src/antenna-visioner.cpp
        src/${YOLO_VERSION}/yolov8_zero_copy.cc
    )

    target_compile_definitions(test_demo PRIVATE ZERO_COPY)
    target_compile_definitions(test_demo PRIVATE USE_YOLOV8)

    target_link_libraries(test_demo
        imageutils
        fileutils
        imagedrawing    
        ${RKNN_RT_LIB}
        ${OpenCV_LIBS}
        dl
    )

    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    target_link_libraries(test_demo Threads::Threads)

elseif ("${YOLO_VERSION}" STREQUAL "yolov5")
    message(STATUS "正在使用yolov5!")

    include_directories(${CMAKE_SOURCE_DIR}/include/${YOLO_VERSION})
    
    # 定义可执行文件
    add_executable(test_demo
        src/test_demo.cpp
        src/${YOLO_VERSION}/postprocess.cpp
        src/antenna-visioner.cpp
    )

    target_link_libraries(test_demo
        ${RKNN_RT_LIB}
        ${RGA_LIB}
        ${OpenCV_LIBS} 
    )
endif()

# 安装规则
install(TARGETS test_demo DESTINATION ./)
install(PROGRAMS ${RKNN_RT_LIB} DESTINATION lib)
install(PROGRAMS ${RGA_LIB} DESTINATION lib)
# install(DIRECTORY ${OPENCV_LIB_SRC_DIR}/ DESTINATION lib)
install(DIRECTORY model/${YOLO_VERSION}/ DESTINATION ./${YOLO_VERSION}_model)