#include "tab_device_connect.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

TabDeviceConnect::TabDeviceConnect(MainWindow *mw)
    : QObject(mw), mw(mw)
{
}

void TabDeviceConnect::setupConnections()
{
    connect(mw->ui->pushButton_connect, &QPushButton::clicked,
            this, &TabDeviceConnect::onConnectSTM32);

    connect(mw->ui->pushButton_disconnect, &QPushButton::clicked,
            this, &TabDeviceConnect::onDisconnectSTM32);

    connect(mw->ui->pushButton_connection, &QPushButton::clicked,
            this, &TabDeviceConnect::onConnectTurntable);

    connect(mw->ui->pushButton_disconnection, &QPushButton::clicked,
            this, &TabDeviceConnect::onDisconnectTurntable);
}

/*************************************************************
 * STM32 连接（注意：继续使用 MainWindow 的 commandTransmitter）
 *************************************************************/

void TabDeviceConnect::onConnectSTM32()
{
    mw->commandTransmitter->startServer(
        mw->ui->lineEdit_local_ip->text(),
        mw->ui->lineEdit_local_port->text().toInt()
        );

    mw->ui->label_status->setText("已连接");
}

void TabDeviceConnect::onDisconnectSTM32()
{
    mw->commandTransmitter->stopServer();
    mw->ui->label_status->setText("未连接");
}

/*************************************************************
 * 转台连接（仍然使用 MainWindow 的 turntable_controller）
 *************************************************************/

void TabDeviceConnect::onConnectTurntable()
{
    bool ok = mw->turntable_controller->connectSerial(
        mw->ui->lineEdit_port->text(),
        mw->ui->lineEdit_baudrate->text().toInt(),
        mw->ui->lineEdit_dataBit->text().toInt(),
        mw->ui->lineEdit_parity->text().toInt(),
        mw->ui->lineEdit_stopBit->text().toInt()
        );

    mw->ui->connection_status->setText(ok ? "已连接" : "连接失败");
}

void TabDeviceConnect::onDisconnectTurntable()
{
    mw->turntable_controller->disconnectSerial();
    mw->ui->connection_status->setText("未连接");
}

