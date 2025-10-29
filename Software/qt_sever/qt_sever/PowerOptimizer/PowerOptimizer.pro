QT       += core gui network widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# 包含第三方库路径
INCLUDEPATH += $$PWD/thirdparty
# 添加JSON库支持（需要安装nlohmann/json）
# 如果使用vcpkg: win32: CONFIG += vcpkg
# 或者手动指定包含路径: INCLUDEPATH += /path/to/json/include

SOURCES += \
    command_transmitter.cpp \
    logindialog.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    command_struct.h \
    command_transmitter.h \
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
