QT       += core gui network widgets charts 

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++14

# 包含第三方库路径
INCLUDEPATH += $$PWD/thirdparty
#INCLUDEPATH += /usr/include/modbus

# # 链接libmodbus库
#LIBS += -lmodbus
# libmodbus 源码路径
INCLUDEPATH += $$PWD/thirdparty/libmodbus/src
INCLUDEPATH += src 
INCLUDEPATH += include 

SOURCES += \
    src/PID_Controller.cpp \
    src/TrajectoryJsonDialog.cpp \
    src/command_transmitter.cpp \
    src/tab_file_read.cpp \
    src/tab_stm32.cpp \
    src/tab_turntablecontrol.cpp \
    src/tab_deviceconnect.cpp \
    src/turntable_controller.cpp\
    src/logindialog.cpp \
    src/main.cpp \
    src/mainwindow.cpp

HEADERS += \
    include/PID_Controller.h \
    include/TrajectoryJsonDialog.h \
    include/command_struct.h \
    include/command_transmitter.h \
    include/tab_file_read.h \
    include/tab_stm32.h \
    include/tab_turntablecontrol.h \
    include/tab_deviceconnect.h \
    include/turntable_controller.h\
    include/logindialog.h \
    include/mainwindow.h


# ================= libmodbus sources =================
SOURCES += \
    $$PWD/thirdparty/libmodbus/src/modbus.c \
    $$PWD/thirdparty/libmodbus/src/modbus-data.c \
    $$PWD/thirdparty/libmodbus/src/modbus-rtu.c \
    $$PWD/thirdparty/libmodbus/src/modbus-tcp.c

HEADERS += \
    $$PWD/thirdparty/libmodbus/src/modbus.h \
    #$$PWD/thirdparty/libmodbus/src/modbus-data.h \
    $$PWD/thirdparty/libmodbus/src/modbus-rtu.h \
    $$PWD/thirdparty/libmodbus/src/modbus-tcp.h

FORMS += \
    mainwindow.ui

# TRANSLATIONS += \
#     PowerOptimizer_zh_CN.ts

# CONFIG += lrelease
# CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# 设置构建目录
DESTDIR = $$PWD/build  # 最终可执行文件的输出目录
OBJECTS_DIR = build/.obj  # 对象文件(.o)的输出目录
MOC_DIR = build/.moc      # moc文件的输出目录
RCC_DIR = build/.rcc      # 资源文件(rcc)的输出目录
UI_DIR = build/.ui        # UI头文件的输出目录

# 确保构建目录存在
!exists($$DESTDIR) {
    mkpath($$DESTDIR)
}

!exists($$OBJECTS_DIR) {
    mkpath($$OBJECTS_DIR)
}

!exists($$MOC_DIR) {
    mkpath($$MOC_DIR)
}

!exists($$RCC_DIR) {
    mkpath($$RCC_DIR)
}

!exists($$UI_DIR) {
    mkpath($$UI_DIR)
}
win32 {
    LIBS += -lws2_32
}
