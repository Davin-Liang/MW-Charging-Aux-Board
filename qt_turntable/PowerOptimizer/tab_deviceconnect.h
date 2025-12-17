#ifndef TAB_DEVICECONNECT_H
#define TAB_DEVICECONNECT_H

#include <QObject>
#include <QHostAddress>
#include <QAbstractSocket>
#include <QTimer>
class MainWindow;
class TurntableController;
class CommandTransmitter;

/**
 * @brief 负责“设备连接”Tab 页面的逻辑控制类。
 *
 * 本类不负责 UI 创建，仅操作 MainWindow 中的 UI 控件，
 * 实现 STM32 / 转台连接等功能。
 */
class TabDeviceConnect : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param mw 传入 MainWindow 指针，用于访问 UI 控件和核心对象
     */
    explicit TabDeviceConnect(MainWindow *mw);

    /**
     * @brief 析构函数（不负责删除外部对象）
     */
    ~TabDeviceConnect();

    /**
     * @brief 绑定所有按钮信号与槽函数
     */
    void setupConnections();

private slots:

    // —— STM32 连接 —— //

    void on_pushButton_connect_clicked();
    void on_pushButton_disconnect_clicked();
    // socket 错误回调（可连接到 CommandTransmitter 的 error 信号）
    void onSocketError(QAbstractSocket::SocketError error);

    // —— 转台连接 —— //
    void on_pushButton_connection_clicked();
    void on_pushButton_disconnection_clicked();

    
private:

    // —— 访问主窗口及其成员 —— //
    MainWindow *mw;  // 非拥有：指向主窗口（用于访问 ui 与全局对象）

    // —— 外部核心对象（由 MainWindow 创建和销毁） —— //
    // CommandTransmitter *transmitter; // 缓存指针（不拥有），等于 mw->commandTransmitter

    QTimer *MonitorTimer;  // 监控转台数据的定时器

    // —— 本类内部状态 —— //
    bool isTurntableConnected = false;
    bool isSTM32Connected = false;

private:
    // —— UI 更新 —— //
    void updateConnectionStatus(bool connected);
    void setTurntableConnectionStatus(bool connected);

};

#endif // TAB_DEVICE_CONNECT_H
