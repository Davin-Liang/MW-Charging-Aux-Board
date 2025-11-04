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
    , motorChart(nullptr)
    , motorTrajectorySeries(nullptr)
    , currentPositionSeries(nullptr)
    , axisX(nullptr)
    , axisY(nullptr)
    , chartView(nullptr)
{
    ui->setupUi(this);


    // 设置窗口属性
    setWindowTitle("数据采集系统可视化界面");
    applyModernStyle();
    ui->label_status->setObjectName("statusLabel");

    // 设置数据监控区域为只读
    setupReadOnlyDataMonitoring();

    // 添加网络代理设置
    QNetworkProxyFactory::setUseSystemConfiguration(false);
    QNetworkProxy::setApplicationProxy(QNetworkProxy::NoProxy);

    // 不再初始化原来的TCP客户端，因为CommandTransmitter是服务器
    tcpSocket = nullptr;

    sdCardQueryTimer = new QTimer(this);
    sdCardQueryTimer->setInterval(2000);

    // 初始化命令传输器
    setupCommandTransmitter();

    // 初始化电机状态图表
    initializeMotorChart();

    // 连接新按钮的槽函数
    connect(ui->pushButton_motor_control, &QPushButton::clicked, this, &MainWindow::on_pushButton_motor_control_clicked);
    connect(ui->pushButton_find_optimal, &QPushButton::clicked, this, &MainWindow::on_pushButton_find_optimal_clicked);
    // 确保轨迹类型变化信号正确连接
    connect(ui->comboBox_traj_type, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::on_traj_type_changed);
    // 连接文件读取相关信号槽
    connect(ui->pushButton_filenamedisplay, &QPushButton::clicked,
            this, &MainWindow::on_pushButton_filenamedisplay_clicked);
    connect(ui->pushButton_read_file, &QPushButton::clicked,
            this, &MainWindow::on_pushButton_read_file_clicked);
    connect(ui->listWidget_files, &QListWidget::itemSelectionChanged,
            this, &MainWindow::on_listWidget_files_itemSelectionChanged);
    // 在构造函数中添加连接  双击直接查看文件内容
    connect(ui->listWidget_files, &QListWidget::itemDoubleClicked,
            this, [this](QListWidgetItem *item) {
                QString filePath = item->data(Qt::UserRole).toString();
                displayFileContent(filePath);
            });

    // 设置界面初始状态
    ui->pushButton_disconnect->setEnabled(false);
    ui->label_status->setText("未连接");
    ui->label_status->setStyleSheet("color: red;");

    // 初始化轨迹类型下拉框
    ui->comboBox_traj_type->addItem("方形轨迹");
    ui->comboBox_traj_type->addItem("圆形轨迹");
    ui->comboBox_traj_type->setCurrentIndex(0);


    // 设置默认服务器端口（可选）
   // ui->lineEdit_local_port->setText("8080");
    // 初始化UI显示配置值
    initializeUIWithConfig();
}
void MainWindow::setupCommandTransmitter()
{
    commandTransmitter = new CommandTransmitter(this);
    // 连接数据接收信号
    connect(commandTransmitter, &CommandTransmitter::motorDataReceived,
            this, &MainWindow::onMotorDataReceived);
    connect(commandTransmitter, &CommandTransmitter::channelDataReceived,
            this, &MainWindow::onChannelDataReceived);
    connect(commandTransmitter, &CommandTransmitter::optResDataReceived,
            this, &MainWindow::onOptResDataReceived);
}
// 初始化电机状态图表
void MainWindow::initializeMotorChart()
{
    // 创建图表
    motorChart = new QChart();
    motorChart->setTitle("");  // 设置为空字符串，不显示标题
    motorChart->setAnimationOptions(QChart::NoAnimation);

    // 创建轨迹序列
    motorTrajectorySeries = new QLineSeries();
    // 移除名称设置，不显示图例
    motorTrajectorySeries->setColor(Qt::blue);
    motorTrajectorySeries->setPointsVisible(false);  // 不显示轨迹上的点，只显示线条

    // 创建当前位置序列
    currentPositionSeries = new QScatterSeries();
    // 移除名称设置，不显示图例
    currentPositionSeries->setColor(Qt::red);
    currentPositionSeries->setMarkerSize(4);
    currentPositionSeries->setBorderColor(Qt::darkRed);

    // 添加到图表
    motorChart->addSeries(motorTrajectorySeries);
    motorChart->addSeries(currentPositionSeries);

    // 创建坐标轴
    axisX = new QValueAxis();
    axisX->setTitleText("x (mm)");
    axisX->setTitleFont(QFont("宋体", 8));
    axisX->setLabelsFont(QFont("宋体", 8));
    axisX->setRange(-50, 50);  // 初始范围
    axisX->setTickCount(11);

    axisY = new QValueAxis();
    axisY->setTitleText("y (mm)");
    axisY->setTitleFont(QFont("宋体", 8));
    axisY->setLabelsFont(QFont("宋体", 8));
    axisY->setRange(-50, 50);  // 初始范围
    axisY->setTickCount(11);

    // 设置坐标轴
    motorChart->addAxis(axisX, Qt::AlignBottom);
    motorChart->addAxis(axisY, Qt::AlignLeft);

    motorTrajectorySeries->attachAxis(axisX);
    motorTrajectorySeries->attachAxis(axisY);
    currentPositionSeries->attachAxis(axisX);
    currentPositionSeries->attachAxis(axisY);

    // 隐藏图例
    motorChart->legend()->setVisible(false);

    // 设置图表边距为0，使其完全填充
    motorChart->setMargins(QMargins(0, 0, 0, 0));
    motorChart->setBackgroundRoundness(0);

    // 创建图表视图
    chartView = new QChartView(motorChart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // 设置图表视图的边距为0
    chartView->setContentsMargins(0, 0, 0, 0);

    // 将图表添加到UI中的widget_chart
    QVBoxLayout *chartLayout = new QVBoxLayout(ui->widget_chart);
    chartLayout->addWidget(chartView);
    chartLayout->setContentsMargins(0, 0, 0, 0);  // 设置布局边距为0
    chartLayout->setSpacing(0);  // 设置间距为0

    // 初始化历史数据
    positionHistory.clear();

    qDebug()<<"电机位置轨迹图初始化完成";
}
// 新增：处理电机数据接收
void MainWindow::onMotorDataReceived(const MotorData_t &data)
{
    // 更新电机状态读取区域
    ui->lineEdit_read_motor_x->setText(QString::number(data.motorX, 'f', 2));
    ui->lineEdit_read_motor_y->setText(QString::number(data.motorY, 'f', 2));
    ui->lineEdit_read_motor_speed->setText(QString::number(data.motorSpeed));

    // 更新图表
    updateMotorChart(data.motorX, data.motorY);

    QString message = QString("收到电机数据 - X: %1, Y: %2, 速度: %3")
                          .arg(data.motorX).arg(data.motorY).arg(data.motorSpeed);
    qDebug()<<message;
}
// 更新电机状态图表
void MainWindow::updateMotorChart(double x, double y)
{
    QPointF newPosition(x, y);

    // 添加到历史轨迹
    positionHistory.append(newPosition);

    // 限制历史数据数量，避免内存过度增长
    if (positionHistory.size() > 1000) {
        positionHistory.removeFirst();
    }

    // 更新轨迹序列
    motorTrajectorySeries->clear();
    for (const QPointF &point : positionHistory) {
        *motorTrajectorySeries << point;
    }

    // 更新当前位置标记
    currentPositionSeries->clear();
    *currentPositionSeries << newPosition;

    // 自动调整坐标轴范围
    if (!positionHistory.isEmpty()) {
        double minX = x, maxX = x, minY = y, maxY = y;
        for (const QPointF &point : positionHistory) {
            minX = qMin(minX, point.x());
            maxX = qMax(maxX, point.x());
            minY = qMin(minY, point.y());
            maxY = qMax(maxY, point.y());
        }

        // 添加一些边距
        double marginX = (maxX - minX) * 0.1;
        double marginY = (maxY - minY) * 0.1;

        if (marginX < 10) marginX = 10;
        if (marginY < 10) marginY = 10;

        axisX->setRange(minX - marginX, maxX + marginX);
        axisY->setRange(minY - marginY, maxY + marginY);
    }
}
// 新增：处理通道数据接收
void MainWindow::onChannelDataReceived(const CurrentVPCh_t &data)
{
    // 更新寻优过程数据显示区域
    ui->lineEdit_read_currentchannel->setText(QString::number(data.currentChannel));
    ui->lineEdit_read_currentvoltage->setText(QString::number(data.currentV, 'f', 2));
    ui->lineEdit_read_currentpower->setText(QString::number(data.currentP, 'f', 2));

    QString message = QString("收到通道信息 - 通道: %1, 电压: %2V, 功率: %3W")
                          .arg(data.currentChannel).arg(data.currentV).arg(data.currentP);

    qDebug()<<message;
}

// 新增：处理优化结果数据接收
void MainWindow::onOptResDataReceived(const OptResData_t &data)
{
    // 更新寻优结果显示区域
    ui->lineEdit_read_channel1voltagemax->setText(QString::number(data.optimalVs[0], 'f', 2));
    ui->lineEdit_read_channel2voltagemax->setText(QString::number(data.optimalVs[1], 'f', 2));
    ui->lineEdit_read_channel3voltagemax->setText(QString::number(data.optimalVs[2], 'f', 2));
    ui->lineEdit_read_channel4voltagemax->setText(QString::number(data.optimalVs[3], 'f', 2));
    ui->lineEdit_read_powermax->setText(QString::number(data.optimalPower, 'f', 2));

    QString message = QString("收到优化结果 - 最大功率: %1W, 电压: [%2, %3, %4, %5]V")
                          .arg(data.optimalPower)
                          .arg(data.optimalVs[0]).arg(data.optimalVs[1])
                          .arg(data.optimalVs[2]).arg(data.optimalVs[3]);
    qDebug()<<message;
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

    ui->lineEdit_square_step_2->setText(QString::number(findOptCmd.squThajLen, 'f', 2));
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
        ui->lineEdit_square_step_2->setEnabled(true); // 方形边长
        ui->lineEdit_square_step->setEnabled(true);
        ui->lineEdit_circle_radius->setEnabled(false);
        ui->label_11->setEnabled(true);  // 方形边长标签
        ui->label_8->setEnabled(true);  // 方形步长标签
        ui->label_7->setEnabled(false); // 圆形半径标签

        // 使用更明确的样式设置
        ui->lineEdit_square_step_2->setStyleSheet("QLineEdit { background-color: white; color: black; }");
        ui->lineEdit_square_step->setStyleSheet("QLineEdit { background-color: white; color: black; }");
        ui->lineEdit_circle_radius->setStyleSheet("QLineEdit { background-color: #f0f0f0; color: #999999; border: 1px solid #cccccc; }");

        // 强制更新
        ui->lineEdit_square_step_2->update();
        ui->lineEdit_square_step->update();
        ui->lineEdit_circle_radius->update();

        qDebug()<<"已切换到方形轨迹模式";
    } else { // 圆形轨迹
        ui->lineEdit_square_step_2->setEnabled(false); // 方形边长
        ui->lineEdit_square_step->setEnabled(false);
        ui->lineEdit_circle_radius->setEnabled(true);
        ui->label_11->setEnabled(false); // 方形边长标签
        ui->label_8->setEnabled(false); // 方形步长标签
        ui->label_7->setEnabled(true);  // 圆形半径标签

        // 使用更明确的样式设置
        ui->lineEdit_square_step_2->setStyleSheet("QLineEdit { background-color: #f0f0f0; color: #999999; border: 1px solid #cccccc; }");
        ui->lineEdit_square_step->setStyleSheet("QLineEdit { background-color: #f0f0f0; color: #999999; border: 1px solid #cccccc; }");
        ui->lineEdit_circle_radius->setStyleSheet("QLineEdit { background-color: white; color: black; }");

        // 强制更新
        ui->lineEdit_square_step_2->update();
        ui->lineEdit_square_step->update();
        ui->lineEdit_circle_radius->update();

       qDebug()<<"已切换到圆形轨迹模式";
    }
}



MainWindow::~MainWindow()
{
    if (commandTransmitter) {
        commandTransmitter->stop_server();
    }
    // 清理图表资源
    // 注意：由于Qt的对象父子关系，大部分资源会自动清理
    // 但为了安全，可以显式删除主要对象
    if (chartView) {
        delete chartView;
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
        qDebug() << "电机控制命令发送成功: X=" << motorCmd.x << ", Y=" << motorCmd.y << ", Speed=" << motorCmd.speed;
    } else {
        qDebug() << "电机控制命令发送失败，错误码:" <<result ;
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
     float squThajLen = 0.0f;  // 新增：方形轨迹边长
    uint8_t squThajStepLen = 0;
    float maxVol = ui->lineEdit_max_voltage->text().toFloat();
    float volStepLen = ui->lineEdit_voltage_step->text().toFloat();

    if (whichThaj == SQU_TRAJ) {
        // 方形轨迹：只使用方形步长，圆形半径设为0
        squThajLen = ui->lineEdit_square_step_2->text().toFloat(); // 获取方形边长
        squThajStepLen = ui->lineEdit_square_step->text().toUShort();
        cirTrajRad = 0.0f;

        // 确保UI状态正确（方形步长可用，圆形半径禁用）
        ui->lineEdit_square_step_2->setEnabled(true);
        ui->lineEdit_square_step->setEnabled(true);
        ui->lineEdit_circle_radius->setEnabled(false);

        // 验证方形轨迹参数
        if (squThajLen <= 0) {
            QMessageBox::warning(this, "参数错误", "方形轨迹必须设置有效的方形边长");
            return;
        }
        if (squThajStepLen == 0) {
            QMessageBox::warning(this, "参数错误", "方形轨迹必须设置方形步长");
            return;
        }

    } else {
        // 圆形轨迹：只使用圆形半径，方形参数设为0
        cirTrajRad = ui->lineEdit_circle_radius->text().toFloat();
        squThajLen = 0.0f;
        squThajStepLen = 0;

        // 确保UI状态正确（圆形半径可用，方形参数禁用）
        ui->lineEdit_circle_radius->setEnabled(true);
        ui->lineEdit_square_step_2->setEnabled(false);
        ui->lineEdit_square_step->setEnabled(false);
        // 验证圆形轨迹参数
        if (cirTrajRad <= 0) {
            QMessageBox::warning(this, "参数错误", "圆形轨迹必须设置有效的圆形半径");
            return;
        }
    }

    if (maxVol <= 0 || volStepLen <= 0) {
        QMessageBox::warning(this, "参数错误", "最大电压和电压步长必须大于0");
        return;
    }
    // 第一步：先发送时间命令
    int timeResult = commandTransmitter->send_time_command();
    if (timeResult == 0) {
        qDebug()<<"时间命令发送成功";
    } else {
        qDebug() << "时间命令发送失败，错误码:" << timeResult;
        // 即使时间命令失败，也继续发送寻优命令
    }

    int result = commandTransmitter->send_find_opt_command(whichThaj, cirTrajRad, squThajLen, squThajStepLen, maxVol, volStepLen);
    if (result == 0) {
        qDebug()<<"寻优控制命令发送成功";

        // 更新commandTransmitter中的配置
        FindOptimalCmd_t findOptCmd = commandTransmitter->getFindOptCmd();
        findOptCmd.whichThaj = whichThaj;
        findOptCmd.cirTrajRad = cirTrajRad;
        findOptCmd.squThajLen = squThajLen;   // 新增：保存方形边长
        findOptCmd.squThajStepLen = squThajStepLen;
        findOptCmd.maxVol = maxVol;
        findOptCmd.volStepLen = volStepLen;
        commandTransmitter->setFindOptCmd(findOptCmd);

        // 显示发送的参数信息
        QString paramInfo;
        if (whichThaj == SQU_TRAJ) {
            paramInfo = QString("轨迹类型: 方形轨迹, 方形边长: %1mm, 方形步长: %2mm, 最大电压: %3V, 电压步长: %4V")
                            .arg(squThajLen).arg(squThajStepLen).arg(maxVol).arg(volStepLen);
        } else {
            paramInfo = QString("轨迹类型: 圆形轨迹, 圆形半径: %1m, 最大电压: %2V, 电压步长: %3V")
                            .arg(cirTrajRad).arg(maxVol).arg(volStepLen);
        }
       qDebug()<<paramInfo;
    } else {
        qDebug() << "寻优控制命令发送失败，错误码:" << result;
    }
}


// 网络连接按钮点击事件
void MainWindow::on_pushButton_connect_clicked()
{
    // 获取用户输入的端口（现在作为服务器端口）
    QString portText = ui->lineEdit_local_port->text().trimmed();
    quint16 serverPort = portText.toUShort();

    // 验证输入
    if (serverPort == 0) {
        QMessageBox::warning(this, "输入错误", "请输入正确的服务器端口");
        return;
    }

    // 启动服务器
    if (commandTransmitter->start_server(serverPort)) {
        ui->label_status->setText("等待连接...");
        ui->label_status->setStyleSheet("color: orange;");
        ui->pushButton_connect->setEnabled(false);
        ui->pushButton_disconnect->setEnabled(true);
        ui->label_status->setText("已连接");
        ui->label_status->setStyleSheet("color: green;");
        qDebug() << "服务器已启动，监听端口:" << serverPort;
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
    qDebug()<<"服务器已停止";
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
    }
}
// 设置数据监控区域为只读
void MainWindow::setupReadOnlyDataMonitoring()
{
    // 电机当前状态读取区域
    ui->lineEdit_read_motor_x->setReadOnly(true);
    ui->lineEdit_read_motor_y->setReadOnly(true);
    ui->lineEdit_read_motor_speed->setReadOnly(true);

    // 寻优过程数据显示区域
    ui->lineEdit_read_currentchannel->setReadOnly(true);
    ui->lineEdit_read_currentvoltage->setReadOnly(true);
    ui->lineEdit_read_currentpower->setReadOnly(true);

    // 寻优结果显示区域
    ui->lineEdit_read_channel1voltagemax->setReadOnly(true);
    ui->lineEdit_read_channel2voltagemax->setReadOnly(true);
    ui->lineEdit_read_channel3voltagemax->setReadOnly(true);
    ui->lineEdit_read_channel4voltagemax->setReadOnly(true);
    ui->lineEdit_read_powermax->setReadOnly(true);

    // 设置只读状态下的样式，使其看起来更明显
    QString readOnlyStyle = "QLineEdit:read-only { background-color: #f0f0f0; color: #666666; border: 1px solid #cccccc; }";

    ui->lineEdit_read_motor_x->setStyleSheet(readOnlyStyle);
    ui->lineEdit_read_motor_y->setStyleSheet(readOnlyStyle);
    ui->lineEdit_read_motor_speed->setStyleSheet(readOnlyStyle);
    ui->lineEdit_read_currentchannel->setStyleSheet(readOnlyStyle);
    ui->lineEdit_read_currentvoltage->setStyleSheet(readOnlyStyle);
    ui->lineEdit_read_currentpower->setStyleSheet(readOnlyStyle);
    ui->lineEdit_read_channel1voltagemax->setStyleSheet(readOnlyStyle);
    ui->lineEdit_read_channel2voltagemax->setStyleSheet(readOnlyStyle);
    ui->lineEdit_read_channel3voltagemax->setStyleSheet(readOnlyStyle);
    ui->lineEdit_read_channel4voltagemax->setStyleSheet(readOnlyStyle);
    ui->lineEdit_read_powermax->setStyleSheet(readOnlyStyle);
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
            qDebug()<<message;
        }
    } else {
        // 直接显示原始数据
        qDebug() << "以太网原始数据:"+ receivedData;
    }

    qDebug() << "解析以太网数据:" << data;
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
// 扫描数据文件
void MainWindow::scanDataFiles()
{
    // 清空当前文件列表
    ui->listWidget_files->clear();
    m_currentSelectedFile.clear();
    ui->label_selected_file->setText("当前选中文件：无");

    // 获取当前工作目录
    QDir currentDir(QDir::currentPath());

    // 设置文件过滤器，查找CSV文件
    QStringList filters;
    filters << "*.csv";
    currentDir.setNameFilters(filters);

    // 获取文件列表并按修改时间排序（最新的在前）
    QFileInfoList fileList = currentDir.entryInfoList(QDir::Files, QDir::Time);

    if (fileList.isEmpty()) {
        qDebug() << "未找到任何CSV数据文件";
        return;
    }

    // 添加到列表控件
    for (const QFileInfo &fileInfo : fileList) {
        QString displayText = QString("%1 (%2)")
        .arg(fileInfo.fileName())
            .arg(QString::fromLocal8Bit("大小: %1字节").arg(fileInfo.size()));

        QListWidgetItem *item = new QListWidgetItem(displayText, ui->listWidget_files);
        item->setData(Qt::UserRole, fileInfo.absoluteFilePath()); // 保存完整路径
    }

    qDebug() << "找到" << fileList.size() << "个数据文件";
}
// 文件选择变化处理
void MainWindow::on_listWidget_files_itemSelectionChanged()
{
    QList<QListWidgetItem*> selectedItems = ui->listWidget_files->selectedItems();

    if (selectedItems.isEmpty()) {
        m_currentSelectedFile.clear();
        ui->label_selected_file->setText("当前选中文件：无");
        return;
    }

    QListWidgetItem *selectedItem = selectedItems.first();
    m_currentSelectedFile = selectedItem->data(Qt::UserRole).toString();

    QString displayText = QString("当前选中文件：%1").arg(QFileInfo(m_currentSelectedFile).fileName());
    ui->label_selected_file->setText(displayText);

    qDebug() << "选中文件：" << m_currentSelectedFile;
}
/*
// 增强文件选择变化处理，显示更多文件信息
void MainWindow::on_listWidget_files_itemSelectionChanged()
{
    QList<QListWidgetItem*> selectedItems = ui->listWidget_files->selectedItems();

    if (selectedItems.isEmpty()) {
        m_currentSelectedFile.clear();
        ui->label_selected_file->setText("当前选中文件：无");
        return;
    }

    QListWidgetItem *selectedItem = selectedItems.first();
    m_currentSelectedFile = selectedItem->data(Qt::UserRole).toString();

    QFileInfo fileInfo(m_currentSelectedFile);
    QString displayText = QString("当前选中文件：%1 (大小: %2字节, 修改时间: %3)")
                              .arg(fileInfo.fileName())
                              .arg(fileInfo.size())
                              .arg(fileInfo.lastModified().toString("yyyy-MM-dd hh:mm:ss"));

    ui->label_selected_file->setText(displayText);

    qDebug() << "选中文件：" << m_currentSelectedFile;
}
*/

// 显示文件内容
void MainWindow::displayFileContent(const QString &filePath)
{
    if (filePath.isEmpty()) {
        QMessageBox::warning(this, "警告", "请先选择要查看的文件");
        return;
    }

    QFile file(filePath);
    if (!file.exists()) {
        QMessageBox::critical(this, "错误", QString("文件不存在: %1").arg(filePath));
        return;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "错误", QString("无法打开文件: %1").arg(filePath));
        return;
    }

    // 读取文件内容
    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    // 显示文件内容
    ui->textEdit_file_content->setPlainText(content);

    qDebug() << "已显示文件内容：" << filePath;
}
// 文件存储情况显示按钮点击
void MainWindow::on_pushButton_filenamedisplay_clicked()
{
    qDebug() << "开始扫描数据文件...";
    scanDataFiles();
}

// 文件内容查看按钮点击
void MainWindow::on_pushButton_read_file_clicked()
{
    if (m_currentSelectedFile.isEmpty()) {
        QMessageBox::warning(this, "警告", "请先选择要查看的文件");
        return;
    }

    qDebug() << "查看文件内容：" << m_currentSelectedFile;
    displayFileContent(m_currentSelectedFile);
}
