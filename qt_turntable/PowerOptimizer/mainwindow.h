#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTimer>

#include "command_transmitter.h"
#include "turntable_controller.h"
#include "PID_Controller.h"

#include "tab_file_read.h"
#include "tab_turntablecontrol.h"
#include "tab_stm32.h"
#include "tab_deviceconnect.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    friend class TabFileRead;
    friend class TabSTM32;
    friend class TabTurntableControl;
    friend class TabDeviceConnect;
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
   
private:
    Ui::MainWindow *ui;
    QTcpSocket *tcpSocket;                     // TCP套接字（保留用于兼容性）

  
    // 主通信服务端
    CommandTransmitter *commandTransmitter;

    // 转台控制器（仍然在 MainWindow 中管理）
    TurntableController *turntable_controller;

    // 全局 PID（各 Tab 可访问）
    PIDController *pid_x;
    PIDController *pid_y;
    QTimer *turntableMonitorTimer;
    QTimer *closedLoopTimer;


     // 四个 Tab 页面对象
    TabDeviceConnect *tabDeviceConnect;
    TabSTM32 *tabSTM32;
    TabTurntableControl *tabTurntableControl;
    TabFileRead *tabFileRead;
    
private:
    void setupUI();
    void applyModernStyle();
};


#endif // MAINWINDOW_H
