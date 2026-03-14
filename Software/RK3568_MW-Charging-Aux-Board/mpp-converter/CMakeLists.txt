cmake_minimum_required(VERSION 3.4.1)

project(mpp_converter)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,--allow-shlib-undefined")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--allow-shlib-undefined") 
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ENABLE_WEBRTC OFF CACHE BOOL "Disable WebRTC to save cross-compile effort")
set(ENABLE_OPENSSL OFF CACHE BOOL "Disable OpenSSL if you only need RTSP/RTMP")
set(ENABLE_CXX_API ON CACHE BOOL "Enable C++ API for direct embedding")

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

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/ 3rdparty.out)
if(NOT TARGET usb_hikvisioner_lib)
    add_subdirectory(../usb-hikvisioner build_usb_hikvisioner_lib)
endif()
add_subdirectory(../3rdparty/ZLMediaKit build_ZLMediaKit)

# 编译库
add_library(mpp_converter_lib STATIC
    src/mpp-converter.cpp
)

target_include_directories(mpp_converter_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${LIBMPP_INCLUDES}
    ../3rdparty/ZLMediaKit/src
    ../3rdparty/ZLMediaKit/3rdpart/ZLToolKit/src
)

target_link_libraries(mpp_converter_lib PUBLIC
    ${MPP_LIBS}
    usb_hikvisioner_lib
    zlmediakit
    ext-codec
    zlmediakit
    ZLToolKit
    "-Wl,--start-group"
    /home/topeet/Linux-Project/3rdparty/ZLMediaKit/release/linux/Debug/libzlmediakit.a
    /home/topeet/Linux-Project/3rdparty/ZLMediaKit/release/linux/Debug/libext-codec.a
    /home/topeet/Linux-Project/3rdparty/ZLMediaKit/release/linux/Debug/libflv.a
    /home/topeet/Linux-Project/3rdparty/ZLMediaKit/release/linux/Debug/libmov.a
    /home/topeet/Linux-Project/3rdparty/ZLMediaKit/release/linux/Debug/libmpeg.a
    /home/topeet/Linux-Project/3rdparty/ZLMediaKit/release/linux/Debug/libjsoncpp.a
    "-Wl,--end-group"
)

# 编译可执行程序
# add_executable(usb_hikvisioner_demo
#     src/test_demo.cpp
# )

# target_link_libraries(usb_hikvisioner_demo
#     usb_hikvisioner_lib
# )
