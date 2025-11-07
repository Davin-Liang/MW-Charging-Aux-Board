# 项目名称和类型
TARGET = CommandTransmitter
TEMPLATE = app

# C++标准
CONFIG += c++17

# Qt模块
QT += core network

# 编译器警告
CONFIG += warn_on

# 源文件和头文件
SOURCES += command_transmitter.cpp \
           main.cpp

HEADERS += command_transmitter.h \
           command_struct.h

# nlohmann/json 包含路径
# 如果系统安装了 nlohmann-json3-dev，通常不需要额外配置
# 如果需要手动指定路径，取消下面的注释：
# INCLUDEPATH += /usr/include/nlohmann
# 或者如果json头文件在项目目录中：
# INCLUDEPATH += $$PWD/third_party

# 编译设置
QMAKE_CXXFLAGS += -std=c++17

# 调试信息
CONFIG += debug

# 发布版本优化
CONFIG(release, debug|release) {
    QMAKE_CXXFLAGS += -O2
}

# 链接库
LIBS += -lstdc++fs  # 如果需要文件系统支持

# 目标文件输出目录
DESTDIR = bin
OBJECTS_DIR = build/obj
MOC_DIR = build/moc
RCC_DIR = build/rcc
UI_DIR = build/ui