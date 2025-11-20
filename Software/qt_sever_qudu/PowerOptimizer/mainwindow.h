#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTimer>
#include <QComboBox>
#include "command_transmitter.h"  // 添加这行

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_connect_clicked();      // 连接按钮点击
    void on_pushButton_disconnect_clicked();   // 断开按钮点击

    void onSocketConnected();                  // 连接成功
    void onSocketDisconnected();               // 连接断开
    void onSocketReadyRead();                  // 数据接收
    void onSocketError(QAbstractSocket::SocketError error); // 连接错误

    void querySDCardStatus();                  // 查询SD卡状态

    void on_pushButton_read_sd_clicked();  // 读取SD卡数据

    // 新增的按钮槽函数
    void on_pushButton_motor_control_clicked();    // 电机控制
    void on_pushButton_find_optimal_clicked();     // 寻优控制
    void on_pushButton_send_time_clicked();        // 发送时间
    void on_pushButton_read_channel_clicked();     // 读取通道
    void on_pushButton_read_motor_clicked();       // 读取电机
    void on_pushButton_read_optimal_clicked();     // 读取优化结果


    void on_textEdit_message_copyAvailable(bool b);
    void on_textEdit_sd_data_copyAvailable(bool b);

    void initializeUIWithConfig();
    void on_traj_type_changed(int index);

private:
    Ui::MainWindow *ui;
    QTcpSocket *tcpSocket;                     // TCP套接字（保留用于兼容性）
    QTimer *sdCardQueryTimer;                  // SD卡状态查询定时器
    bool isConnected;                          // 连接状态标志
    // 重新添加 receiveBuffer
    QByteArray receiveBuffer;

    CommandTransmitter *commandTransmitter;

    void applyModernStyle();
    void parseEthernetData(const QByteArray &data);
    void displayMessage(const QString &message);
    void updateSDCardDisplay(const QByteArray &sdData);

    // 新增辅助函数
    void setupCommandTransmitter();
    void updateConnectionStatus(bool connected);
};

#endif // MAINWINDOW_H
