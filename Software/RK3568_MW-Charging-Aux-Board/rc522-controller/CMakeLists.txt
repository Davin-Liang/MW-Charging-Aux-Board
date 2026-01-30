cmake_minimum_required(VERSION 3.10)

project(rc522_controller VERSION 1.0)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/install/)

add_compile_options(-Wall -Wextra)

find_package(Threads REQUIRED)

add_executable(rc522_test
    src/test_demo.cpp
    src/rc522-controller.cpp
)

target_link_libraries(rc522_test PRIVATE Threads::Threads)

target_include_directories(rc522_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

install(TARGETS rc522_test DESTINATION ./)