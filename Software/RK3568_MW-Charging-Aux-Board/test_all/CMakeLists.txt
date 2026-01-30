cmake_minimum_required(VERSION 3.4.1)
project(my_robot_app)

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/install)

add_subdirectory(../usb-hikvisioner build_usb_hikvisioner_lib)
add_subdirectory(../aerial-visioner build_aerial_visioner_lib)

add_executable(test_all src/test_demo.cpp)

target_link_libraries(test_all PRIVATE 
    usb_hikvisioner_lib
    aerial_visioner_lib
)

install(TARGETS test_all DESTINATION ./)
