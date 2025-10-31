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
    , commandTransmitter(nullptr)
{
    ui->setupUi(this);

    // 设置窗口属性
    setWindowTitle("数据采集系统可视化界面");
    applyModernStyle();
    ui->label_status->setObjectName("statusLabel");

    // 添加网络代理设置
    QNetworkProxyFactory::setUseSystemConfiguration(false);
    QNetworkProxy::setApplicationProxy(QNetworkProxy::NoProxy);

    // 不再初始化原来的TCP客户端，因为CommandTransmitter是服务器
    tcpSocket = nullptr;

    sdCardQueryTimer = new QTimer(this);
    sdCardQueryTimer->setInterval(2000);

    // 初始化命令传输器
    setupCommandTransmitter();

    // 连接按钮槽函数
    connect(ui->pushButton_read_sd, &QPushButton::clicked, this, &MainWindow::on_pushButton_read_sd_clicked);

    // 连接新按钮的槽函数
    connect(ui->pushButton_motor_control, &QPushButton::clicked, this, &MainWindow::on_pushButton_motor_control_clicked);
    connect(ui->pushButton_find_optimal, &QPushButton::clicked, this, &MainWindow::on_pushButton_find_optimal_clicked);
    connect(ui->pushButton_send_time, &QPushButton::clicked, this, &MainWindow::on_pushButton_send_time_clicked);
    connect(ui->pushButton_read_channel, &QPushButton::clicked, this, &MainWindow::on_pushButton_read_channel_clicked);
    connect(ui->pushButton_read_motor, &QPushButton::clicked, this, &MainWindow::on_pushButton_read_motor_clicked);
    connect(ui->pushButton_read_optimal, &QPushButton::clicked, this, &MainWindow::on_pushButton_read_optimal_clicked);
    // 确保轨迹类型变化信号正确连接
    connect(ui->comboBox_traj_type, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::on_traj_type_changed);

    // 设置界面初始状态
    ui->pushButton_disconnect->setEnabled(false);
    ui->label_status->setText("未连接");
    ui->label_status->setStyleSheet("color: red;");

    // 初始化轨迹类型下拉框
    ui->comboBox_traj_type->addItem("方形轨迹");
    ui->comboBox_traj_type->addItem("圆形轨迹");
    ui->comboBox_traj_type->setCurrentIndex(0);
    // 初始化UI显示配置值
    initializeUIWithConfig();
}

// 用配置值初始化UI
void MainWindow::initializeUIWithConfig()
{
    if (!commandTransmitter) return;

    // 获取电机配置
    MotorCmd_t motorCmd = commandTransmitter->getMotorCmd();
    ui->lineEdit_motor_x->setText(QString::number(motorCmd.x, 'f', 2));
    ui->lineEdit_motor_y->setText(QString::number(motorCmd.y, 'f', 2));
    ui->lineEdit_motor_speed->setText(QString::number(motorCmd.speed));

    // 获取寻优配置
    FindOptimalCmd_t findOptCmd = commandTransmitter->getFindOptCmd();

    // 设置轨迹类型
    if (findOptCmd.whichThaj == SQU_TRAJ) {
        ui->comboBox_traj_type->setCurrentIndex(0);
    } else if (findOptCmd.whichThaj == CIR_TRAJ) {
        ui->comboBox_traj_type->setCurrentIndex(1);
    }

    ui->lineEdit_square_step->setText(QString::number(findOptCmd.squThajStepLen));
    ui->lineEdit_circle_radius->setText(QString::number(findOptCmd.cirTrajRad, 'f', 2));
    ui->lineEdit_max_voltage->setText(QString::number(findOptCmd.maxVol, 'f', 2));
    ui->lineEdit_voltage_step->setText(QString::number(findOptCmd.volStepLen, 'f', 2));

    // 初始化工况状态 - 确保UI状态正确
    on_traj_type_changed(ui->comboBox_traj_type->currentIndex());
}

// 新增：轨迹类型变化处理
void MainWindow::on_traj_type_changed(int index)
{
    if (index == 0) { // 方形轨迹
        ui->lineEdit_square_step->setEnabled(true);
        ui->lineEdit_circle_radius->setEnabled(false);
        ui->label_8->setEnabled(true);  // 方形步长标签
        ui->label_7->setEnabled(false); // 圆形半径标签

        // 使用更明确的样式设置
        ui->lineEdit_square_step->setStyleSheet("QLineEdit { background-color: white; color: black; }");
        ui->lineEdit_circle_radius->setStyleSheet("QLineEdit { background-color: #f0f0f0; color: #999999; border: 1px solid #cccccc; }");

        // 强制更新
        ui->lineEdit_square_step->update();
        ui->lineEdit_circle_radius->update();

        displayMessage("已切换到方形轨迹模式");
    } else { // 圆形轨迹
        ui->lineEdit_square_step->setEnabled(false);
        ui->lineEdit_circle_radius->setEnabled(true);
        ui->label_8->setEnabled(false); // 方形步长标签
        ui->label_7->setEnabled(true);  // 圆形半径标签

        // 使用更明确的样式设置
        ui->lineEdit_square_step->setStyleSheet("QLineEdit { background-color: #f0f0f0; color: #999999; border: 1px solid #cccccc; }");
        ui->lineEdit_circle_radius->setStyleSheet("QLineEdit { background-color: white; color: black; }");

        // 强制更新
        ui->lineEdit_square_step->update();
        ui->lineEdit_circle_radius->update();

        displayMessage("已切换到圆形轨迹模式");
    }
}

void MainWindow::setupCommandTransmitter()
{
    commandTransmitter = new CommandTransmitter(this);
}

MainWindow::~MainWindow()
{
    if (commandTransmitter) {
        commandTransmitter->stop_server();
    }
    delete ui;
}

// 电机控制按钮点击事件
void MainWindow::on_pushButton_motor_control_clicked()
{
    if (!commandTransmitter || !commandTransmitter->m_clientSocket) {
        QMessageBox::warning(this, "错误", "请先启动服务器并等待STM32连接");
        return;
    }

    // 获取电机控制参数并更新到commandTransmitter
    MotorCmd_t motorCmd;
    motorCmd.x = ui->lineEdit_motor_x->text().toFloat();
    motorCmd.y = ui->lineEdit_motor_y->text().toFloat();
    motorCmd.speed = ui->lineEdit_motor_speed->text().toUShort();

    // 更新到commandTransmitter
    commandTransmitter->setMotorCmd(motorCmd);

    int result = commandTransmitter->send_motor_command();
    if (result == 0) {
        displayMessage(QString("电机控制命令发送成功: X=%1, Y=%2, Speed=%3").arg(motorCmd.x).arg(motorCmd.y).arg(motorCmd.speed));
    } else {
        displayMessage(QString("电机控制命令发送失败，错误码: %1").arg(result));
    }
}

// 寻优控制按钮点击事件
void MainWindow::on_pushButton_find_optimal_clicked()
{
    if (!commandTransmitter || !commandTransmitter->m_clientSocket) {
        QMessageBox::warning(this, "错误", "请先启动服务器并等待STM32连接");
        return;
    }

    // 获取轨迹类型
    int trajTypeIndex = ui->comboBox_traj_type->currentIndex();
    ThajType_t whichThaj = (trajTypeIndex == 0) ? SQU_TRAJ : CIR_TRAJ;

    // 根据轨迹类型获取正确的参数
    float cirTrajRad = 0.0f;
    uint8_t squThajStepLen = 0;
    float maxVol = ui->lineEdit_max_voltage->text().toFloat();
    float volStepLen = ui->lineEdit_voltage_step->text().toFloat();

    if (whichThaj == SQU_TRAJ) {
        // 方形轨迹：只使用方形步长，圆形半径设为0
        squThajStepLen = ui->lineEdit_square_step->text().toUShort();
        cirTrajRad = 0.0f;

        // 确保UI状态正确（方形步长可用，圆形半径禁用）
        ui->lineEdit_square_step->setEnabled(true);
        ui->lineEdit_circle_radius->setEnabled(false);
    } else {
        // 圆形轨迹：只使用圆形半径，方形步长设为0
        cirTrajRad = ui->lineEdit_circle_radius->text().toFloat();
        squThajStepLen = 0;

        // 确保UI状态正确（圆形半径可用，方形步长禁用）
        ui->lineEdit_circle_radius->setEnabled(true);
        ui->lineEdit_square_step->setEnabled(false);
    }

    // 验证必要参数
    if (whichThaj == SQU_TRAJ && squThajStepLen == 0) {
        QMessageBox::warning(this, "参数错误", "方形轨迹必须设置方形步长");
        return;
    }

    if (whichThaj == CIR_TRAJ && cirTrajRad <= 0) {
        QMessageBox::warning(this, "参数错误", "圆形轨迹必须设置有效的圆形半径");
        return;
    }

    if (maxVol <= 0 || volStepLen <= 0) {
        QMessageBox::warning(this, "参数错误", "最大电压和电压步长必须大于0");
        return;
    }

    int result = commandTransmitter->send_find_opt_command(whichThaj, cirTrajRad, squThajStepLen, maxVol, volStepLen);
    if (result == 0) {
        displayMessage("寻优控制命令发送成功");

        // 更新commandTransmitter中的配置
        FindOptimalCmd_t findOptCmd = commandTransmitter->getFindOptCmd();
        findOptCmd.whichThaj = whichThaj;
        findOptCmd.cirTrajRad = cirTrajRad;
        findOptCmd.squThajStepLen = squThajStepLen;
        findOptCmd.maxVol = maxVol;
        findOptCmd.volStepLen = volStepLen;
        commandTransmitter->setFindOptCmd(findOptCmd);

        // 显示发送的参数信息
        QString paramInfo;
        if (whichThaj == SQU_TRAJ) {
            paramInfo = QString("轨迹类型: 方形轨迹, 方形步长: %1mm, 最大电压: %2V, 电压步长: %3V")
                            .arg(squThajStepLen).arg(maxVol).arg(volStepLen);
        } else {
            paramInfo = QString("轨迹类型: 圆形轨迹, 圆形半径: %1m, 最大电压: %2V, 电压步长: %3V")
                            .arg(cirTrajRad).arg(maxVol).arg(volStepLen);
        }
        displayMessage(paramInfo);
    } else {
        displayMessage(QString("寻优控制命令发送失败，错误码: %1").arg(result));
    }
}

// 发送时间命令
void MainWindow::on_pushButton_send_time_clicked()
{
    if (!commandTransmitter || !commandTransmitter->m_clientSocket) {
        QMessageBox::warning(this, "错误", "请先启动服务器并等待STM32连接");
        return;
    }

    int result = commandTransmitter->send_time_command();
    if (result == 0) {
        displayMessage("时间命令发送成功");
    } else {
        displayMessage(QString("时间命令发送失败，错误码: %1").arg(result));
    }
}

// 读取通道数据
void MainWindow::on_pushButton_read_channel_clicked()
{
    displayMessage("读取通道数据功能需要STM32主动发送数据");
    // 注意：通道数据由STM32主动发送，通过execute_command处理
}

// 读取电机数据
void MainWindow::on_pushButton_read_motor_clicked()
{
    displayMessage("读取电机数据功能需要STM32主动发送数据");
    // 注意：电机数据由STM32主动发送，通过execute_command处理
}

// 读取优化结果
void MainWindow::on_pushButton_read_optimal_clicked()
{
    displayMessage("读取优化结果功能需要STM32主动发送数据");
    // 注意：优化结果由STM32主动发送，通过execute_command处理
}

// ========== 连接管理 ==========

// 连接按钮点击事件
void MainWindow::on_pushButton_connect_clicked()
{
    // 获取用户输入的端口（现在作为服务器端口）
    QString portText = ui->lineEdit_remote_port->text().trimmed();
    quint16 serverPort = portText.toUShort();

    // 验证输入
    if (serverPort == 0) {
        QMessageBox::warning(this, "输入错误", "请输入正确的服务器端口");
        return;
    }

    // 启动服务器
    if (commandTransmitter->start_server(serverPort)) {
        ui->label_status->setText("服务器已启动，等待STM32连接...");
        ui->label_status->setStyleSheet("color: orange;");
        ui->pushButton_connect->setEnabled(false);
        ui->pushButton_disconnect->setEnabled(true);
        displayMessage(QString("服务器已启动，监听端口: %1").arg(serverPort));

        // 启动SD卡状态查询定时器
        sdCardQueryTimer->start();
    } else {
        QMessageBox::critical(this, "错误", "无法启动服务器");
    }
}

// 断开连接按钮点击事件
void MainWindow::on_pushButton_disconnect_clicked()
{
    if (commandTransmitter) {
        commandTransmitter->stop_server();
    }

    updateConnectionStatus(false);
    displayMessage("服务器已停止");
}

// 更新连接状态
void MainWindow::updateConnectionStatus(bool connected)
{
    if (connected) {
        ui->pushButton_connect->setEnabled(false);
        ui->pushButton_disconnect->setEnabled(true);
        ui->label_status->setText("STM32已连接");
        ui->label_status->setStyleSheet("color: green;");
    } else {
        ui->pushButton_connect->setEnabled(true);
        ui->pushButton_disconnect->setEnabled(false);
        ui->label_status->setText("未连接");
        ui->label_status->setStyleSheet("color: red;");

        // 停止SD卡状态查询定时器
        sdCardQueryTimer->stop();
    }
}

// ========== 网络通信处理 ==========

// TCP连接成功 - 现在不需要这个函数，因为CommandTransmitter是服务器
void MainWindow::onSocketConnected()
{
    // 空实现，因为我们现在是服务器模式
}

// TCP连接断开 - 现在不需要这个函数
void MainWindow::onSocketDisconnected()
{
    // 空实现，因为我们现在是服务器模式
}

// 接收数据 - 现在不需要这个函数，因为CommandTransmitter处理数据接收
void MainWindow::onSocketReadyRead()
{
    // 空实现，因为CommandTransmitter处理数据接收
}

// TCP错误处理
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
        errorMsg = "连接错误";
    }

    QMessageBox::critical(this, "连接错误", errorMsg);
    updateConnectionStatus(false);
}

// ========== 其他功能 ==========

// 查询SD卡状态
void MainWindow::querySDCardStatus()
{
    if (commandTransmitter && commandTransmitter->m_clientSocket) {
        // 可以通过CommandTransmitter发送查询命令
        displayMessage("SD卡状态查询");
    }
}

// 读取SD卡数据按钮点击事件
void MainWindow::on_pushButton_read_sd_clicked()
{
    if (!commandTransmitter || !commandTransmitter->m_clientSocket) {
        QMessageBox::warning(this, "错误", "请先启动服务器并等待STM32连接");
        return;
    }

    // 可以通过CommandTransmitter发送读取命令
    displayMessage("发送SD卡数据读取命令");
}

// 解析以太网数据
void MainWindow::parseEthernetData(const QByteArray &data)
{
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

// 文本编辑框复制可用状态变化的槽函数
void MainWindow::on_textEdit_message_copyAvailable(bool available)
{
    Q_UNUSED(available)
}

void MainWindow::on_textEdit_sd_data_copyAvailable(bool available)
{
    Q_UNUSED(available)
}

// 现代化样式
void MainWindow::applyModernStyle()
{
    // 设置基本样式，但不覆盖轨迹相关的特殊样式
    QString style = R"(
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

        /* 禁用的输入框样式 - 会被更具体的样式覆盖 */
        QLineEdit:disabled {
            background-color: #f0f0f0;
            color: #999999;
        }
    )";

    setStyleSheet(style);
}
