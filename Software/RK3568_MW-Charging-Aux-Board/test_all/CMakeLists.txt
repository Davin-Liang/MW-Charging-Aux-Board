cmake_minimum_required(VERSION 3.4.1)
project(my_robot_app)

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/install)

add_subdirectory(../usb-hikvisioner build_usb_hikvisioner_lib)
add_subdirectory(../aerial-visioner build_aerial_visioner_lib)

add_executable(test_all_demo src/test_demo.cpp)
add_executable(test_all_demo1 src/test_demo1.cpp)
add_executable(test_all_demo2 src/test_demo2.cpp)

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

install(TARGETS test_all_demo DESTINATION ./)
install(TARGETS test_all_demo1 DESTINATION ./)
install(TARGETS test_all_demo2 DESTINATION ./)
