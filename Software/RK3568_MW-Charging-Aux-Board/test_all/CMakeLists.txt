cmake_minimum_required(VERSION 3.4.1)
project(my_robot_app)

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/install)

# 1. 关闭 ZLM 一些在纯内网推流不需要的重量级特性 (加快编译速度，避免寻找依赖失败)
# set(ENABLE_WEBRTC OFF CACHE BOOL "Disable WebRTC to save cross-compile effort")
# set(ENABLE_OPENSSL OFF CACHE BOOL "Disable OpenSSL if you only need RTSP/RTMP")
# set(ENABLE_CXX_API ON CACHE BOOL "Enable C++ API for direct embedding")

add_subdirectory(../usb-hikvisioner build_usb_hikvisioner_lib)
add_subdirectory(../aerial-visioner build_aerial_visioner_lib)
add_subdirectory(../mpp-converter build_mpp_converter_lib)
# add_subdirectory(../3rdparty/ZLMediaKit build_ZLMediaKit)

add_executable(test_all_demo src/test_demo.cpp)
add_executable(test_all_demo1 src/test_demo1.cpp)
add_executable(test_all_demo2 src/test_demo2.cpp)
add_executable(test_all_demo3 src/test_demo3.cpp)
add_executable(test_all_demo4 src/test_demo4.cpp)

target_link_libraries(test_all_demo PRIVATE 
    # usb_hikvisioner_lib
    aerial_visioner_lib
)

target_link_libraries(test_all_demo1 PRIVATE 
    # usb_hikvisioner_lib
    aerial_visioner_lib
)
target_link_libraries(test_all_demo2 PRIVATE 
    # usb_hikvisioner_lib
    aerial_visioner_lib
)
target_link_libraries(test_all_demo3 PRIVATE 
    # usb_hikvisioner_lib
    aerial_visioner_lib
    mpp_converter_lib
    # zlmediakit
    # ZLToolKit
)

target_link_libraries(test_all_demo4 PRIVATE 
    # usb_hikvisioner_lib
    aerial_visioner_lib
    mpp_converter_lib
    # zlmediakit
    # ZLToolKit
)

# target_include_directories(test_all_demo3 PUBLIC
#     ../3rdparty/ZLMediaKit/src
#     ../3rdparty/ZLMediaKit/3rdpart/ZLToolKit/src
# )

install(TARGETS test_all_demo DESTINATION ./)
install(TARGETS test_all_demo1 DESTINATION ./)
install(TARGETS test_all_demo2 DESTINATION ./)
install(TARGETS test_all_demo3 DESTINATION ./)
install(TARGETS test_all_demo4 DESTINATION ./)
