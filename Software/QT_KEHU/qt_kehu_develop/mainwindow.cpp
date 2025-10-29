#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QDebug>
#include <QNetworkProxy>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , isConnected(false)
// commandAdapter 会自动调用默认构造函数
{
    ui->setupUi(this);

    // 设置窗口属性
    setWindowTitle("数据采集系统可视化界面");
    //setMinimumSize(700, 700);
    //setMaximumSize(700,700);
    applyModernStyle();
   // 为状态标签设置特殊样式
    ui->label_status->setObjectName("statusLabel");


    // 添加网络代理设置，避免连接问题
    QNetworkProxyFactory::setUseSystemConfiguration(false);
    QNetworkProxy::setApplicationProxy(QNetworkProxy::NoProxy);
    // 初始化TCP套接字
    tcpSocket = new QTcpSocket(this);
    tcpSocket->setProxy(QNetworkProxy::NoProxy);  // 为套接字单独设置

    sdCardQueryTimer = new QTimer(this);

    // 设置定时器间隔为2秒
    sdCardQueryTimer->setInterval(2000);

    // 连接信号和槽
    connect(tcpSocket, &QTcpSocket::connected, this, &MainWindow::onSocketConnected);
    connect(tcpSocket, &QTcpSocket::disconnected, this, &MainWindow::onSocketDisconnected);
    connect(tcpSocket, &QTcpSocket::readyRead, this, &MainWindow::onSocketReadyRead);
    connect(tcpSocket, &QTcpSocket::errorOccurred, this, &MainWindow::onSocketError);
    connect(sdCardQueryTimer, &QTimer::timeout, this, &MainWindow::querySDCardStatus);

    // 连接新的命令按钮
    connect(ui->pushButton_motor_control, &QPushButton::clicked, this, &MainWindow::on_pushButton_motor_control_clicked);
    connect(ui->pushButton_find_optimal, &QPushButton::clicked, this, &MainWindow::on_pushButton_find_optimal_clicked);
    connect(ui->pushButton_read_motor, &QPushButton::clicked, this, &MainWindow::on_pushButton_read_motor_clicked);
    connect(ui->pushButton_read_optimal, &QPushButton::clicked, this, &MainWindow::on_pushButton_read_optimal_clicked);
    connect(ui->pushButton_read_channel, &QPushButton::clicked, this, &MainWindow::on_pushButton_read_channel_clicked);
    connect(ui->pushButton_send_time, &QPushButton::clicked, this, &MainWindow::on_pushButton_send_time_clicked);
    connect(ui->comboBox_traj_type, QOverload<int>::of(&QComboBox::activated), this, &MainWindow::on_comboBox_traj_type_activated);
    connect(ui->pushButton_read_sd, &QPushButton::clicked, this, &MainWindow::on_pushButton_read_sd_clicked);

    // 设置界面初始状态
    ui->pushButton_disconnect->setEnabled(false);
    ui->label_status->setText("未连接");
    ui->label_status->setStyleSheet("color: red;");

    // 初始化控制指令
    initializeControlCommands();
}

MainWindow::~MainWindow()
{
    delete ui;
}
// mainwindow.cpp - 槽函数
void MainWindow::on_pushButton_motor_control_clicked()
{
    if (!isConnected) {
        QMessageBox::warning(this, "错误", "请先连接设备");
        return;
    }

    float x = ui->lineEdit_motor_x->text().toFloat();
    float y = ui->lineEdit_motor_y->text().toFloat();
    uint16_t speed = ui->lineEdit_motor_speed->text().toUShort();

    QByteArray command = commandAdapter.buildMotorCommand(x, y, speed);  // 使用 . 操作符
    tcpSocket->write(command);

    displayMessage(QString("发送电机控制命令: X=%1, Y=%2, Speed=%3").arg(x).arg(y).arg(speed));
}

void MainWindow::on_pushButton_find_optimal_clicked()
{
    if (!isConnected) {
        QMessageBox::warning(this, "错误", "请先连接设备");
        return;
    }

    uint8_t trajType = ui->comboBox_traj_type->currentData().toUInt();
    float circleRadius = ui->lineEdit_circle_radius->text().toFloat();
    uint8_t squareStep = ui->lineEdit_square_step->text().toUInt();
    float maxVoltage = ui->lineEdit_max_voltage->text().toFloat();
    float voltageStep = ui->lineEdit_voltage_step->text().toFloat();

    QByteArray command = commandAdapter.buildFindOptimalCommand(trajType, circleRadius,
                                                                squareStep, maxVoltage, voltageStep);
    tcpSocket->write(command);

    displayMessage("发送寻优控制命令");
}

void MainWindow::on_pushButton_read_motor_clicked()
{
    if (!isConnected) {
        QMessageBox::warning(this, "错误", "请先连接设备");
        return;
    }

    QByteArray command = commandAdapter.buildReadCommand(MOTOR_DATA_READ);
    tcpSocket->write(command);
    displayMessage("发送电机数据读取命令");
}

void MainWindow::on_pushButton_read_optimal_clicked()
{
    if (!isConnected) {
        QMessageBox::warning(this, "错误", "请先连接设备");
        return;
    }

    QByteArray command = commandAdapter.buildReadCommand(CMD_OPT_RES_READ);
    tcpSocket->write(command);
    displayMessage("发送寻优结果读取命令");
}

void MainWindow::on_pushButton_read_channel_clicked()
{
    if (!isConnected) {
        QMessageBox::warning(this, "错误", "请先连接设备");
        return;
    }

    QByteArray command = commandAdapter.buildReadCommand(CURRENT_VPCH_READ);
    tcpSocket->write(command);
    displayMessage("发送通道信息读取命令");
}

void MainWindow::on_pushButton_send_time_clicked()
{
    if (!isConnected) {
        QMessageBox::warning(this, "错误", "请先连接设备");
        return;
    }

    QByteArray command = commandAdapter.buildTimeCommand();
    tcpSocket->write(command);
    displayMessage("发送时间同步命令");
}

// 轨迹类型选择
void MainWindow::on_comboBox_traj_type_activated(int index)
{
    bool isCircleTraj = (index == 1); // 假设索引1是圆形轨迹
    ui->lineEdit_circle_radius->setEnabled(isCircleTraj);
    ui->lineEdit_square_step->setEnabled(!isCircleTraj);
}

// 初始化控制参数
void MainWindow::initializeControlCommands()
{
    // 初始化轨迹类型
    ui->comboBox_traj_type->clear();
    ui->comboBox_traj_type->addItem("方形轨迹", SQU_TRAJ);
    ui->comboBox_traj_type->addItem("圆形轨迹", CIR_TRAJ);

    // 设置默认值
    ui->lineEdit_motor_x->setText("0.0");
    ui->lineEdit_motor_y->setText("0.0");
    ui->lineEdit_motor_speed->setText("100");
    ui->lineEdit_circle_radius->setText("1.0");
    ui->lineEdit_square_step->setText("10");
    ui->lineEdit_max_voltage->setText("5.0");
    ui->lineEdit_voltage_step->setText("0.1");

    on_comboBox_traj_type_activated(0);
}

// 处理接收到的数据
void MainWindow::onSocketReadyRead()
{
    QByteArray newData = tcpSocket->readAll();
    receiveBuffer.append(newData);

    // 尝试解析命令帧
    CommandFrame_t frame;
    while (commandAdapter.parseCommandFrame(receiveBuffer, frame)) {
        // 处理完整的命令帧
        handleReceivedCommand(frame);

        // 从缓冲区移除已处理的数据
        int frameLength = sizeof(CmdHeader_t) + frame.header.dataLen;
        receiveBuffer = receiveBuffer.mid(frameLength);
    }
}

// 处理接收到的命令帧
void MainWindow::handleReceivedCommand(const CommandFrame_t &frame)
{
    switch(frame.header.cmdId) {
    case MOTOR_DATA_READ:
        if (frame.header.dataLen == sizeof(MotorData_t)) {
            QString msg = QString("电机数据 - X:%1, Y:%2, Speed:%3")
                              .arg(frame.payload.motorData.motorX)
                              .arg(frame.payload.motorData.motorY)
                              .arg(frame.payload.motorData.motorSpeed);
            displayMessage(msg);
        }
        break;

    case CMD_OPT_RES_READ:
        if (frame.header.dataLen == sizeof(OptResData_t)) {
            QString msg = QString("寻优结果 - 功率:%1, 电压:[%2,%3,%4,%5]")
                              .arg(frame.payload.optResData.optimalPower)
                              .arg(frame.payload.optResData.optimalVs[0])
                              .arg(frame.payload.optResData.optimalVs[1])
                              .arg(frame.payload.optResData.optimalVs[2])
                              .arg(frame.payload.optResData.optimalVs[3]);
            displayMessage(msg);
        }
        break;

    case CURRENT_VPCH_READ:
        if (frame.header.dataLen == sizeof(CurrentVPCh_t)) {
            QString msg = QString("通道信息 - 通道:%1, 电压:%2, 功率:%3")
                              .arg(frame.payload.currentVPCh.currentChannel)
                              .arg(frame.payload.currentVPCh.currentV)
                              .arg(frame.payload.currentVPCh.currentP);
            displayMessage(msg);
        }
        break;

    default:
        displayMessage(QString("未知命令类型: 0x%1").arg(frame.header.cmdId, 2, 16, QChar('0')));
        break;
    }
}

void MainWindow::applyModernStyle()
{
    // 使用更保守的样式，避免影响功能
    setStyleSheet(R"(
        QMainWindow {
            background: #f0f0f0;
            font-family: 'Microsoft YaHei';
        }

        QGroupBox {
            background-color: white;
            border: 1px solid #cccccc;
            border-radius: 5px;
            margin-top: 10px;
            padding-top: 10px;
            font-weight: bold;
        }

        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px 0 5px;
        }

        QPushButton {
            background-color: #4CAF50;
            color: white;
            border: none;
            border-radius: 4px;
            padding: 6px 12px;
            font-weight: normal;
        }

        QPushButton:hover {
            background-color: #45a049;
        }

        QPushButton:pressed {
            background-color: #3d8b40;
        }

        QPushButton:disabled {
            background-color: #cccccc;
            color: #666666;
        }

        QLineEdit {
            border: 1px solid #cccccc;
            border-radius: 3px;
            padding: 4px 8px;
            background-color: white;
        }

        QLineEdit:focus {
            border-color: #4CAF50;
        }

        QLabel {
            color: #333333;
        }
    )");
}
// 读取SD卡数据按钮点击事件
void MainWindow::on_pushButton_read_sd_clicked()
{
    if (!isConnected) {
        QMessageBox::warning(this, "错误", "请先连接设备");
        return;
    }

    // 发送读取SD卡数据的命令
    QString readCommand = "READ_SD_DATA\n";
    tcpSocket->write(readCommand.toUtf8());

    displayMessage("发送SD卡数据读取命令");
    qDebug() << "发送SD卡数据读取命令";
}
// 连接设备按钮点击事件
void MainWindow::on_pushButton_connect_clicked()
{
    // 获取用户输入的IP和端口
    QString remoteIp = ui->lineEdit_remote_ip->text().trimmed();
    QString localIp = ui->lineEdit_local_ip->text().trimmed();
    quint16 remotePort = ui->lineEdit_remote_port->text().toUShort();
    quint16 localPort = ui->lineEdit_local_port->text().toUShort();

    // 验证输入
    if (remoteIp.isEmpty() || remotePort == 0) {
        QMessageBox::warning(this, "输入错误", "请输入正确的远程IP地址和端口");
        return;
    }

    // 尝试连接
    tcpSocket->abort(); // 中止现有连接
    tcpSocket->connectToHost(remoteIp, remotePort);

    // 可选：绑定本地IP和端口
    if (!localIp.isEmpty() && localPort != 0) {
        tcpSocket->bind(QHostAddress(localIp), localPort);
    }

    ui->label_status->setText("正在连接...");
    ui->label_status->setStyleSheet("color: orange;");


}

// 断开连接按钮点击事件
void MainWindow::on_pushButton_disconnect_clicked()
{
    tcpSocket->disconnectFromHost();
    if (tcpSocket->state() != QAbstractSocket::UnconnectedState) {
        tcpSocket->waitForDisconnected(1000);
    }
}

// TCP连接成功
void MainWindow::onSocketConnected()
{
    isConnected = true;
    ui->pushButton_connect->setEnabled(false);
    ui->pushButton_disconnect->setEnabled(true);
    ui->label_status->setText("设备已连接");
    ui->label_status->setStyleSheet("color: green;");

    // 启动SD卡状态查询定时器
    sdCardQueryTimer->start();

    qDebug() << "设备连接成功";
}

// TCP连接断开
void MainWindow::onSocketDisconnected()
{
    isConnected = false;
    ui->pushButton_connect->setEnabled(true);
    ui->pushButton_disconnect->setEnabled(false);
    ui->label_status->setText("未连接");
    ui->label_status->setStyleSheet("color: red;");

    // 停止SD卡状态查询定时器
    sdCardQueryTimer->stop();

    qDebug() << "设备连接已断开";
}

// TCP连接错误处理
void MainWindow::onSocketError(QAbstractSocket::SocketError error)
{
    QString errorMsg;
    switch (error) {
    case QAbstractSocket::ConnectionRefusedError:
        errorMsg = "连接被拒绝";
        break;
    case QAbstractSocket::RemoteHostClosedError:
        errorMsg = "远程主机关闭连接";
        break;
    case QAbstractSocket::HostNotFoundError:
        errorMsg = "找不到主机";
        break;
    case QAbstractSocket::SocketTimeoutError:
        errorMsg = "连接超时";
        break;
    default:
        errorMsg = "连接错误: " + tcpSocket->errorString();
    }

    QMessageBox::critical(this, "连接错误", errorMsg);
    ui->label_status->setText("连接失败");
    ui->label_status->setStyleSheet("color: red;");

    // 停止定时器
    sdCardQueryTimer->stop();
}

// 查询SD卡状态
void MainWindow::querySDCardStatus()
{
    if (isConnected && tcpSocket->state() == QAbstractSocket::ConnectedState) {
        // 发送SD卡状态查询命令（根据实际协议调整）
        // 使用更明确的命令格式
        QString queryCommand = "QUERY_SD_STATUS\n";
        tcpSocket->write(queryCommand.toUtf8());
        qDebug() << "发送SD卡状态查询命令:" << queryCommand.trimmed();
    }
}

// 更新SD卡数据显示
void MainWindow::updateSDCardDisplay(const QByteArray &sdData)
{
    // 这里实现SD卡数据的显示逻辑
    // 例如在文本框中显示或解析为特定格式

    QString dataStr = QString::fromUtf8(sdData);
    ui->textEdit_sd_data->append("SD卡数据: " + dataStr);

    qDebug() << "更新SD卡显示:" << dataStr;
}

// 显示消息到接收框
void MainWindow::displayMessage(const QString &message)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    ui->textEdit_message->append(QString("[%1] %2").arg(timestamp).arg(message));
}



