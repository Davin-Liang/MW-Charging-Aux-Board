#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTimer>
#include <QComboBox>
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE
// 控制指令结构体
struct ControlCommand {
    int command1;
    int command2;
    int command3;
    int command4;
    int command5;
    QString toString() const {
        return QString("CMD1:%1, CMD2:%2, CMD3:%3, CMD4:%4, CMD5:%5")
        .arg(command1).arg(command2).arg(command3).arg(command4).arg(command5);
    }
};

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

    void on_pushButton_execute_clicked();  // 执行控制功能
    void on_pushButton_read_sd_clicked();  // 读取SD卡数据

    void on_comboBox_cmd1_activated(int index);
    void on_comboBox_cmd2_activated(int index);
    void on_comboBox_cmd3_activated(int index);
    void on_comboBox_cmd4_activated(int index);
    void on_comboBox_cmd5_activated(int index);

    void on_textEdit_message_copyAvailable(bool b);
    void on_textEdit_sd_data_copyAvailable(bool b);

private:
    Ui::MainWindow *ui;
    QTcpSocket *tcpSocket;                     // TCP套接字
    QTimer *sdCardQueryTimer;                  // SD卡状态查询定时器
    bool isConnected;                          // 连接状态标志

    ControlCommand currentCommand;  // 当前控制指令
    QByteArray receiveBuffer;       // 接收数据缓冲区


    void applyModernStyle();
    void initializeControlCommands();
    void sendToSTM32(const ControlCommand &command);
    void parseEthernetData(const QByteArray &data);
    void displayMessage(const QString &message);
    void updateSDCardDisplay(const QByteArray &sdData);
    int getComboBoxValue(const QString &objectName);
};

#endif // MAINWINDOW_H
