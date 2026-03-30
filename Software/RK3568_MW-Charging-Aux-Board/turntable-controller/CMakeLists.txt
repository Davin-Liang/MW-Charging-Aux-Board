cmake_minimum_required(VERSION 3.4.1)

project(turntable_controller)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,--allow-shlib-undefined")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--allow-shlib-undefined") 
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

# 查找 libmodbus 依赖
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/ 3rdparty.out)

# 编译库
add_library(turntable_controller_lib STATIC
    src/turntable_controller.cpp
)

# 包含目录设置
target_include_directories(turntable_controller_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${LIBMODBUS_INCLUDES}
)

find_package(Threads REQUIRED)

# 链接依赖库
target_link_libraries(turntable_controller_lib PUBLIC
    ${LIBMODBUS}
    Threads::Threads
)