QT       += core gui network widgets charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# 包含第三方库路径
INCLUDEPATH += $$PWD/thirdparty
INCLUDEPATH += /usr/include/modbus

# 链接libmodbus库
LIBS += -lmodbus
SOURCES += \
    PID_Controller.cpp \
    command_transmitter.cpp \
    tab_file_read.cpp \
    tab_stm32.cpp \
    tab_turntable_control.cpp \
    tabdeviceconnect.cpp \
    turntable_controller.cpp\
    logindialog.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    PID_Controller.h \
    command_struct.h \
    command_transmitter.h \
    tab_file_read.h \
    tab_stm32.h \
    tab_turntable_control.h \
    tabdeviceconnect.h \
    turntable_controller.h\
    logindialog.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    PowerOptimizer_zh_CN.ts

CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# 设置构建目录为项目目录下的build文件夹
DESTDIR = $$PWD/build

# 确保构建目录存在
!exists($$DESTDIR) {
    mkpath($$DESTDIR)
}
