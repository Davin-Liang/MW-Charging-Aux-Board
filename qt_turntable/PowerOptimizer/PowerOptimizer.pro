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
    TrajectoryJsonDialog.cpp \
    command_transmitter.cpp \
    tab_file_read.cpp \
    tab_stm32.cpp \
    tab_turntablecontrol.cpp \
    tab_deviceconnect.cpp \
    turntable_controller.cpp\
    logindialog.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    PID_Controller.h \
    TrajectoryJsonDialog.h \
    command_struct.h \
    command_transmitter.h \
    tab_file_read.h \
    tab_stm32.h \
    tab_turntablecontrol.h \
    tab_deviceconnect.h \
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
