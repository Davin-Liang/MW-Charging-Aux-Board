#ifndef TAB_DEVICE_CONNECT_H
#define TAB_DEVICE_CONNECT_H

#include <QObject>

class MainWindow;

class TabDeviceConnect : public QObject
{
    Q_OBJECT

public:
    explicit TabDeviceConnect(MainWindow *mw);
    void setupConnections();

private slots:
    void onConnectSTM32();
    void onDisconnectSTM32();
    void onConnectTurntable();
    void onDisconnectTurntable();

private:
    MainWindow *mw; // 直接访问 mw->ui
};

#endif
