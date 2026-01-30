#include "tab_deviceconnect.h"
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "turntable_controller.h"
//#include "command_transmitter.h"
#include <QMessageBox>
#include <QNetworkProxy>
#include <QNetworkProxyFactory>
#include <QDebug>



TabDeviceConnect::TabDeviceConnect(MainWindow *mw_)
    : QObject(mw_), mw(mw_)
{
    // 从 MainWindow 提取控制对象，使本类实现真正的模块化
    //transmitter = mw->commandTransmitter;
    MonitorTimer = mw->turntableMonitorTimer;

}
TabDeviceConnect::~TabDeviceConnect()
{
    // 这里不用 delete transmitter 或 turntable，因为他们由 MainWindow 负责析构
}

void TabDeviceConnect::setupConnections()
{
    // 初始化 UI 状态（以 MainWindow 的 ui 为准）
    if (!mw || !mw->ui) return;
    // 禁用系统代理，强制走直连
    QNetworkProxyFactory::setUseSystemConfiguration(false);
    QNetworkProxy::setApplicationProxy(QNetworkProxy::NoProxy);

    //数据采集系统的单片机连接状态设置
    // mw->ui->pushButton_disconnect->setEnabled(false);
    // mw->ui->label_status->setText("未连接");
    // mw->ui->label_status->setStyleSheet("color: red;");

    // 串口参数（只读）
    mw->ui->lineEdit_baudrate->setText("115200");
    mw->ui->lineEdit_parity->setText("N");
    mw->ui->lineEdit_dataBit->setText("8");
    mw->ui->lineEdit_stopBit->setText("1");
    mw->ui->lineEdit_port->setText("/dev/ttyUSB0");
    mw->ui->lineEdit_baudrate->setReadOnly(true);
    mw->ui->lineEdit_parity->setReadOnly(true);
    mw->ui->lineEdit_dataBit->setReadOnly(true);
    mw->ui->lineEdit_stopBit->setReadOnly(true);

    //TCP连接(只读)
    mw->ui->lineEdit_local_ip->setText("192.168.1.30");
    mw->ui->lineEdit_local_port->setText("8080");
    mw->ui->lineEdit_local_ip->setReadOnly(true);
    mw->ui->lineEdit_local_port->setReadOnly(true);
    //初始化STM32连接状态
    setTurntableConnectionStatus(false);
    // STM32 连接按钮
    connect(mw->ui->pushButton_connect, &QPushButton::clicked,
            this, &TabDeviceConnect::on_pushButton_connect_clicked);

    connect(mw->ui->pushButton_disconnect, &QPushButton::clicked,
            this, &TabDeviceConnect::on_pushButton_disconnect_clicked);

    // 转台连接/断开
    connect(mw->ui->pushButton_connection, &QPushButton::clicked,
            this, &TabDeviceConnect::on_pushButton_connection_clicked);

    connect(mw->ui->pushButton_disconnection, &QPushButton::clicked,
            this, &TabDeviceConnect::on_pushButton_disconnection_clicked);          
}


/****************************************************
 *                  STM32 连接
 ****************************************************/
/**
 * @brief “连接”按钮点击事件。
 *        启动服务器并等待 STM32 客户端连接。
 */
void TabDeviceConnect::on_pushButton_connect_clicked()
{
    if (!mw || !mw->ui) return;
    // 获取用户输入的IP地址和端口
    QString ip = mw->ui->lineEdit_local_ip->text().trimmed();
    QString portText = mw->ui->lineEdit_local_port->text().trimmed();
    int port = portText.toInt();
    //const quint16 port = 8080;

    // 验证输入
    if (ip.isEmpty()) {
        QMessageBox::warning(mw, "输入错误", "请输入服务器IP地址");
        return;
    }

    // const quint16 serverPort = portText.toUShort();
    // if (serverPort == 0) {
    //     QMessageBox::warning(mw, "输入错误", "请输入正确的服务器端口");
    //     return;
    // }

    const QHostAddress serverAddress(ip);
    if (serverAddress.isNull()) {
        QMessageBox::warning(mw, "输入错误", "请输入正确的IP地址格式");
        return;
    }

    qDebug() << "尝试启动服务器，IP:" << ip << "端口:" << port;

    // 已存在则先断开
    if (mw->stm32_mb_ctx) {
        modbus_close(mw->stm32_mb_ctx);
        modbus_free(mw->stm32_mb_ctx);
        mw->stm32_mb_ctx = nullptr;
    }
    //  创建 Modbus TCP Client
    mw->stm32_mb_ctx = modbus_new_tcp(ip.toStdString().c_str(), port);
    // mw->stm32_mb_ctx = modbus_new_tcp("192.168.1.30", 8080);
    if (!mw->stm32_mb_ctx) {
        QMessageBox::critical(mw, "错误", "modbus_new_tcp 失败");
        return;
    }
    //  连接 STM32（Server）
    if (modbus_connect(mw->stm32_mb_ctx) == -1) {
        QMessageBox::critical(
            mw,
            "连接失败",
            QString("Modbus 连接失败: %1")
                .arg(modbus_strerror(errno))
            );
        modbus_free(mw->stm32_mb_ctx);
        mw->stm32_mb_ctx = nullptr;
        updateConnectionStatus(false);
        return;
    }

    qDebug() << "STM32 Modbus TCP 已连接";
    isSTM32Connected = true;
    updateConnectionStatus(true);

}
/**
 * @brief “断开连接”按钮点击事件。
 */
void TabDeviceConnect::on_pushButton_disconnect_clicked()
{
    if (mw->stm32_mb_ctx) {
        modbus_close(mw->stm32_mb_ctx);
        modbus_free(mw->stm32_mb_ctx);
        mw->stm32_mb_ctx = nullptr;
    }

    isSTM32Connected = false;
    qDebug() << "STM32 Modbus TCP 已断开";
    updateConnectionStatus(false);
    qDebug() << "服务器已停止";
}
/**
 * @brief 更新网络连接状态的 UI 显示。
 * @param connection 连接状态：true 表示已连接，false 表示未连接。
 */
void TabDeviceConnect::updateConnectionStatus(bool connected)
{
    if (connected) {
        mw->ui->pushButton_connect->setEnabled(false);
        mw->ui->pushButton_disconnect->setEnabled(true);
        mw->ui->label_status->setText("客户端已连接");
        mw->ui->label_status->setStyleSheet("color: green;");
    } else {
        mw->ui->pushButton_connect->setEnabled(true);
        mw->ui->pushButton_disconnect->setEnabled(false);
        mw->ui->label_status->setText("未连接");
        mw->ui->label_status->setStyleSheet("color: red;");
    }
}

/****************************************************
 *                转台连接
 ****************************************************/

/**
 * @brief 连接转台按钮槽函数。负责创建转台控制器，并尝试进行串口连接。
 */
void TabDeviceConnect::on_pushButton_connection_clicked()
{
    QString port = mw->ui->lineEdit_port->text().trimmed();
    const int baudrate = 115200;
    const char parity = 'N';
    const int dataBit = 8;
    const int stopBit = 1;

    if (port.isEmpty()) {
        QMessageBox::warning(mw, "错误", "请填写串口路径");
        return;
    }
    // 如果主窗口已有控制器，先断开并删除（确保只有一个实例）
    if (mw->turntable_controller) {
        mw->turntable_controller->disconnect();
        delete mw->turntable_controller;
        mw->turntable_controller = nullptr;
    }

    // 创建新的控制器对象
    mw->turntable_controller = new TurntableController(port.toStdString().c_str());
    bool ok = mw->turntable_controller->connect(baudrate, parity, dataBit, stopBit);
    if (ok) {
        // 保存为主窗口的指针
        isTurntableConnected = true;
        setTurntableConnectionStatus(true);
        // if (MonitorTimer)
        // MonitorTimer->start(40); // 每40ms刷新数据
        QMessageBox::information(mw, "成功", "转台连接成功");
    } else {
        delete mw->turntable_controller;
        mw->turntable_controller = nullptr;
        isTurntableConnected = false;
        setTurntableConnectionStatus(false);
        QMessageBox::critical(mw, "失败", "转台连接失败");
    }
}
/**
 * @brief 断开转台按钮槽函数。释放控制器并停止监控。
 */
void TabDeviceConnect::on_pushButton_disconnection_clicked()
{
    if (!mw->turntable_controller) {
        setTurntableConnectionStatus(false);
        QMessageBox::information(mw, "提示", "转台未连接");
        return;
    }

    // 由本类负责停止 & 释放创建的控制器（MainWindow 指针会被置空）
    if (mw->turntable_controller) {
        mw->turntable_controller->disconnect();
        delete mw->turntable_controller;
        mw->turntable_controller = nullptr;
    }
    if (MonitorTimer) {
        MonitorTimer->stop();
    }

    isTurntableConnected = false;

    setTurntableConnectionStatus(false);
    QMessageBox::information(mw, tr("提示"), tr("转台已断开连接"));
}
/**
 * @brief 更新 UI 中的连接状态相关组件。
 * @param isConnected 当前是否已连接
 */
void TabDeviceConnect::setTurntableConnectionStatus(bool connected)
{
    mw->ui->connection_status->setText(connected ? "已连接" : "未连接");
    mw->ui->connection_status->setStyleSheet(connected ? "color: green;" : "color: red;");
    mw->ui->pushButton_connection->setEnabled(!connected);
    mw->ui->pushButton_disconnection->setEnabled(connected);
}
