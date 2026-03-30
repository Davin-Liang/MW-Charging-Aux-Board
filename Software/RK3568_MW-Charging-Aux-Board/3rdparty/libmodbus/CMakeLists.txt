cmake_minimum_required(VERSION 3.4.1)

# libmodbus 是纯 C 语言编写的项目
project(libmodbus C)

# ==========================================
# 设置统一的输出目录
# ==========================================
# 静态库 (.a) 的输出路径
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
# 动态库 (.so) 的输出路径 (如果以后需要编译动态库)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
# 可执行文件的输出路径 (如果有测试程序)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

# ==========================================
# 编译源文件
# ==========================================
add_library(modbus STATIC
    src/modbus.c
    src/modbus-data.c
    src/modbus-rtu.c
    src/modbus-tcp.c
)

# ==========================================
# 头文件包含路径
# ==========================================
# 使用 PUBLIC，主项目链接 modbus 时会自动包含此路径
target_include_directories(modbus PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# 可选：解决交叉编译时可能缺少的 Linux 宏定义
# target_compile_definitions(modbus PRIVATE
#     HAVE_DECL_TIOCSRS485=1
#     HAVE_DECL_TIOCM_RTS=1
#     OS_LINUX=1
# )