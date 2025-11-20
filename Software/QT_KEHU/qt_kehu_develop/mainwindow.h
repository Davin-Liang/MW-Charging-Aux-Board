#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTimer>
#include "command_struct.h"
#include "CommandAdapter.h"

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
    void on_pushButton_connect_clicked();
    void on_pushButton_disconnect_clicked();
    void onSocketConnected();
    void onSocketDisconnected();
    void onSocketReadyRead();
    void onSocketError(QAbstractSocket::SocketError error);
    void querySDCardStatus();

    // 新的命令按钮槽函数
    void on_pushButton_motor_control_clicked();
    void on_pushButton_find_optimal_clicked();
    void on_pushButton_read_motor_clicked();
    void on_pushButton_read_optimal_clicked();
    void on_pushButton_read_channel_clicked();
    void on_pushButton_send_time_clicked();
    void on_pushButton_read_sd_clicked();

    void on_comboBox_traj_type_activated(int index);

private:
    Ui::MainWindow *ui;
    QTcpSocket *tcpSocket;
    QTimer *sdCardQueryTimer;
    bool isConnected;
    QByteArray receiveBuffer;

    CommandAdapter commandAdapter; // 命令构建器

    void applyModernStyle();
    void initializeControlCommands();
    void displayMessage(const QString &message);
    void updateSDCardDisplay(const QByteArray &sdData);

    // 处理接收到的命令帧
    void handleReceivedCommand(const CommandFrame_t &frame);
};

#endif // MAINWINDOW_H
