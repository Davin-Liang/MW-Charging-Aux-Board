#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QDebug>
#include <QNetworkProxy>
#include <QDataStream>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , isConnected(false)
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
    connect(ui->pushButton_execute, &QPushButton::clicked, this, &MainWindow::on_pushButton_execute_clicked);
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
// 文本编辑框复制可用状态变化的槽函数
void MainWindow::on_textEdit_message_copyAvailable(bool available)
{
    Q_UNUSED(available)
    // 这个函数可以留空，不需要实际处理
}

void MainWindow::on_textEdit_sd_data_copyAvailable(bool available)
{
    Q_UNUSED(available)
    // 这个函数可以留空，不需要实际处理
}


// 初始化控制指令下拉菜单
void MainWindow::initializeControlCommands()
{
    // 定义五个指令的选项
    QStringList commandOptions = {
        "无操作", "启动采集", "停止采集", "设置参数", "读取状态",
        "复位设备", "校准传感器", "保存配置", "读取配置", "系统自检"
    };

    // 为五个下拉菜单添加相同的选项
    for (int i = 1; i <= 5; i++) {
        QComboBox *comboBox = findChild<QComboBox*>(QString("comboBox_cmd%1").arg(i));
        if (comboBox) {
            comboBox->addItems(commandOptions);
            comboBox->setCurrentIndex(0); // 默认选择"无操作"
        }
    }
}

// 下拉菜单选择变化的槽函数
void MainWindow::on_comboBox_cmd1_activated(int index)
{
    QString commandName = ui->comboBox_cmd1->itemText(index);
    displayMessage(QString("指令1设置为: %1").arg(commandName));
}

void MainWindow::on_comboBox_cmd2_activated(int index)
{
    QString commandName = ui->comboBox_cmd2->itemText(index);
    displayMessage(QString("指令2设置为: %1").arg(commandName));
}

void MainWindow::on_comboBox_cmd3_activated(int index)
{
    QString commandName = ui->comboBox_cmd3->itemText(index);
    displayMessage(QString("指令3设置为: %1").arg(commandName));
}

void MainWindow::on_comboBox_cmd4_activated(int index)
{
    QString commandName = ui->comboBox_cmd4->itemText(index);
    displayMessage(QString("指令4设置为: %1").arg(commandName));
}

void MainWindow::on_comboBox_cmd5_activated(int index)
{
    QString commandName = ui->comboBox_cmd5->itemText(index);
    displayMessage(QString("指令5设置为: %1").arg(commandName));
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
// 接收数据
void MainWindow::onSocketReadyRead()
{
    QByteArray newData = tcpSocket->readAll();
    receiveBuffer.append(newData);

    // 处理完整的数据包（假设数据包以换行符分隔）
    while (receiveBuffer.contains('\n')) {
        int endIndex = receiveBuffer.indexOf('\n');
        QByteArray completePacket = receiveBuffer.left(endIndex);
        receiveBuffer = receiveBuffer.mid(endIndex + 1);

        QString receivedData = QString::fromUtf8(completePacket).trimmed();

        qDebug() << "收到数据:" << receivedData;

        // 解析SD卡状态回复
        if (receivedData.contains("SD_STATUS:")) {
            if (receivedData.contains("SD_STATUS:OK")) {
                QString currentStatus = ui->label_status->text();
                if (currentStatus.contains("SD卡")) {
                    ui->label_status->setText("设备已连接 - SD卡: ● 正常");
                } else {
                    ui->label_status->setText("设备已连接 - SD卡: ● 正常");
                }
                displayMessage("SD卡状态: 正常");
            } else if (receivedData.contains("SD_STATUS:ERROR")) {
                QString currentStatus = ui->label_status->text();
                if (currentStatus.contains("SD卡")) {
                    ui->label_status->setText("设备已连接 - SD卡: ● 异常");
                } else {
                    ui->label_status->setText("设备已连接 - SD卡: ● 异常");
                }
                displayMessage("SD卡状态: 异常");
            }
        }
        // 解析SD卡数据
        else if (receivedData.contains("SD_DATA:")) {
            QByteArray sdData = receivedData.toUtf8();
            updateSDCardDisplay(sdData);
            displayMessage("收到SD卡数据");
        }
        // 解析以太网数据
        else {
            parseEthernetData(completePacket);
        }
    }
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

// 解析以太网数据
void MainWindow::parseEthernetData(const QByteArray &data)
{
    // 这里实现你的以太网数据解析函数
    // 根据实际协议解析数据

    QString receivedData = QString::fromUtf8(data);

    // 示例解析：假设数据格式为 "TYPE:DATA"
    if (receivedData.contains(":")) {
        QStringList parts = receivedData.split(":");
        if (parts.size() >= 2) {
            QString dataType = parts[0];
            QString dataContent = parts[1];

            QString message = QString("以太网数据 - 类型: %1, 内容: %2")
                                  .arg(dataType).arg(dataContent);
            displayMessage(message);
        }
    } else {
        // 直接显示原始数据
        displayMessage("以太网原始数据: " + receivedData);
    }

    qDebug() << "解析以太网数据:" << data;
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

// 执行控制功能按钮点击事件
void MainWindow::on_pushButton_execute_clicked()
{
    if (!isConnected) {
        QMessageBox::warning(this, "错误", "请先连接设备");
        return;
    }

    // 获取五个下拉菜单的选择
    currentCommand.command1 = getComboBoxValue("comboBox_cmd1");
    currentCommand.command2 = getComboBoxValue("comboBox_cmd2");
    currentCommand.command3 = getComboBoxValue("comboBox_cmd3");
    currentCommand.command4 = getComboBoxValue("comboBox_cmd4");
    currentCommand.command5 = getComboBoxValue("comboBox_cmd5");

    // 显示选择的指令
    QString commandInfo = "执行控制指令: " + currentCommand.toString();
    displayMessage(commandInfo);

    // 发送指令到STM32
    sendToSTM32(currentCommand);

    qDebug() << "发送控制指令:" << currentCommand.toString();
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

// 获取下拉菜单的值
int MainWindow::getComboBoxValue(const QString &objectName)
{
    QComboBox *comboBox = findChild<QComboBox*>(objectName);
    if (comboBox) {
        return comboBox->currentIndex(); // 返回选项索引作为指令值
    }
    return 0; // 默认值
}

// 发送指令到STM32
void MainWindow::sendToSTM32(const ControlCommand &command)
{
    if (!isConnected || tcpSocket->state() != QAbstractSocket::ConnectedState) {
        displayMessage("错误: 设备未连接，无法发送指令");
        return;
    }

    // 构造指令字符串（根据实际协议调整）
    QString commandStr = QString("EXEC_CMD %1 %2 %3 %4 %5\n")
                             .arg(command.command1)
                             .arg(command.command2)
                             .arg(command.command3)
                             .arg(command.command4)
                             .arg(command.command5);

    // 发送指令
    tcpSocket->write(commandStr.toUtf8());

    displayMessage("指令已发送到STM32: " + commandStr.trimmed());
}




