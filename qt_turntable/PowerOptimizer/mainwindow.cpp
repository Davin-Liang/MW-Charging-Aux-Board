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
    , turntable_controller(nullptr)
{
    ui->setupUi(this);

    // 在setupUi之后调用
    ui->comboBox_traj_type->setObjectName(""); // 清空对象名，避免�?动连�?

    ui->tabWidget->setCurrentIndex(0);   //显示�?0�?

    // 设置窗口属�?
    setWindowTitle("数据采集系统�?视化界面");
    applyModernStyle();
    ui->label_status->setObjectName("statusLabel");

    // 设置数据监控区域为只�?
    setupReadOnlyDataMonitoring();

    // 添加网络代理设置
    QNetworkProxyFactory::setUseSystemConfiguration(false);
    QNetworkProxy::setApplicationProxy(QNetworkProxy::NoProxy);

    // 不再初�?�化原来的TCP客户�?，因为CommandTransmitter�?服务�?
    tcpSocket = nullptr;

    // sdCardQueryTimer = new QTimer(this);
    // sdCardQueryTimer->setInterval(2000);

    // 初�?�化命令传输�?
    setupCommandTransmitter();

    // 初�?�化电机状态图�?
    initializeMotorChart();
    // 初�?�化表格控件
    setupTableWidget();


    // 连接新按�?的槽函数
    connect(ui->pushButton_motor_control, &QPushButton::clicked, this, &MainWindow::on_pushButton_motor_control_clicked);
    connect(ui->pushButton_find_optimal, &QPushButton::clicked, this, &MainWindow::on_pushButton_find_optimal_clicked);
    // �?保轨迹类型变化信号�?�确连接
    connect(ui->comboBox_traj_type, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &MainWindow::trajTypeChanged);
    // connect(ui->comboBox_traj_type, QOverload<int>::of(&QComboBox::currentIndexChanged),
            // this, &MainWindow::on_traj_type_changed);
    // 连接文件读取相关信号�?
    connect(ui->pushButton_filenamedisplay, &QPushButton::clicked,
            this, &MainWindow::on_pushButton_filenamedisplay_clicked);
    connect(ui->pushButton_read_file, &QPushButton::clicked,
            this, &MainWindow::on_pushButton_read_file_clicked);
    connect(ui->listWidget_files, &QListWidget::itemSelectionChanged,
            this, &MainWindow::on_listWidget_files_itemSelectionChanged);
    // 在构造函数中添加连接  双击直接查看文件内�??
    connect(ui->listWidget_files, &QListWidget::itemDoubleClicked,
            this, [this](QListWidgetItem *item) {
                QString filePath = item->data(Qt::UserRole).toString();
                displayFileContent(filePath);
            });
    update_turntable_image();

    turntableMonitorTimer = new QTimer(this);
    connect(turntableMonitorTimer, &QTimer::timeout, this, &MainWindow::updateTurntableData);
    turntableMonitorTimer->start(500);  
    
    setTurntableConnectionStatus(false);

    // 设置界面初�?�状�?
    ui->pushButton_disconnect->setEnabled(false);
    ui->label_status->setText("�?连接");
    ui->label_status->setStyleSheet("color: red;");

    // 初�?�化轨迹类型下拉�?
    ui->comboBox_traj_type->addItem("方形轨迹");
    ui->comboBox_traj_type->addItem("圆形轨迹");
    ui->comboBox_traj_type->setCurrentIndex(0);


    ui->lineEdit_baudrate->setText("9600");
    ui->lineEdit_parity->setText("N");
    ui->lineEdit_dataBit->setText("8");
    ui->lineEdit_stopBit->setText("1");

    // 设置为只读（让它�?一直显示，但用户不能修改）
    ui->lineEdit_baudrate->setReadOnly(true);
    ui->lineEdit_parity->setReadOnly(true);
    ui->lineEdit_dataBit->setReadOnly(true);
    ui->lineEdit_stopBit->setReadOnly(true);


    // 设置默�?�服务器�?口（�?选）
   // ui->lineEdit_local_port->setText("8080");
    // 初�?�化UI显示配置�?
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
// 初�?�化�?台型号�?�图片（�?显示一次）
void MainWindow::update_turntable_image()
{
    QString imgPath = "./image/turntable.png";  // 图片�?�?
    QPixmap pix(imgPath);

    if (!pix.isNull()) {
        // 将图片按照控件大小等比缩放并显示
        ui->label_turntable_img->setPixmap(
            pix.scaled(ui->label_turntable_img->size(),
                       Qt::KeepAspectRatio,
                       Qt::SmoothTransformation));
    } else {
        // 如果图片加载失败，可显示默�?�文字提�?
        ui->label_turntable_img->setText("�?台图片未找到");
        ui->label_turntable_img->setAlignment(Qt::AlignCenter);
    }
}

// 初�?�化电机状态图�?
void MainWindow::initializeMotorChart()
{
    // 创建图表
    motorChart = new QChart();
    motorChart->setTitle("");  // 设置为空字�?�串，不显示标�??
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

    // 添加到图�?
    motorChart->addSeries(motorTrajectorySeries);
    motorChart->addSeries(currentPositionSeries);

    // 创建坐标�?
    axisX = new QValueAxis();
    axisX->setTitleText("x (mm)");
    axisX->setTitleFont(QFont("宋体", 8));
    axisX->setLabelsFont(QFont("宋体", 8));
    axisX->setRange(-50, 50);  // 初�?�范�?
    axisX->setTickCount(11);

    axisY = new QValueAxis();
    axisY->setTitleText("y (mm)");
    axisY->setTitleFont(QFont("宋体", 8));
    axisY->setLabelsFont(QFont("宋体", 8));
    axisY->setRange(-50, 50);  // 初�?�范�?
    axisY->setTickCount(11);

    // 设置坐标�?
    motorChart->addAxis(axisX, Qt::AlignBottom);
    motorChart->addAxis(axisY, Qt::AlignLeft);

    motorTrajectorySeries->attachAxis(axisX);
    motorTrajectorySeries->attachAxis(axisY);
    currentPositionSeries->attachAxis(axisX);
    currentPositionSeries->attachAxis(axisY);

    // 隐藏图例
    motorChart->legend()->setVisible(false);

    // 设置图表边距�?0，使其完全填�?
    motorChart->setMargins(QMargins(0, 0, 0, 0));
    motorChart->setBackgroundRoundness(0);

    // 创建图表视图
    chartView = new QChartView(motorChart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // 设置图表视图的边距为0
    chartView->setContentsMargins(0, 0, 0, 0);

    // 将图表添加到UI�?的widget_chart
    QVBoxLayout *chartLayout = new QVBoxLayout(ui->widget_chart);
    chartLayout->addWidget(chartView);
    chartLayout->setContentsMargins(0, 0, 0, 0);  // 设置布局边距�?0
    chartLayout->setSpacing(0);  // 设置间距�?0

    // 初�?�化历史数据
    positionHistory.clear();

    qDebug()<<"电机位置轨迹图初始化完成";
}
// 初�?�化表格控件
void MainWindow::setupTableWidget()
{
    // 设置表格属�?
    ui->tableWidget_file_content->setEditTriggers(QAbstractItemView::NoEditTriggers); // 不可编辑
    ui->tableWidget_file_content->setSelectionBehavior(QAbstractItemView::SelectRows); // 选择整�??
    ui->tableWidget_file_content->setSelectionMode(QAbstractItemView::SingleSelection); // 单�?
    ui->tableWidget_file_content->setAlternatingRowColors(true); // 交替行�?�色
    // �?用排�?
   // ui->tableWidget_file_content->setSortingEnabled(true);

    // 设置表头属�?
    QHeaderView *horizontalHeader = ui->tableWidget_file_content->horizontalHeader();
    horizontalHeader->setSectionResizeMode(QHeaderView::Stretch); // �?动拉伸列�?
    horizontalHeader->setStretchLastSection(true); // 最后一列自动拉�?

    QHeaderView *verticalHeader = ui->tableWidget_file_content->verticalHeader();
    verticalHeader->setVisible(true); // 显示行号

    // 设置表格样式
    ui->tableWidget_file_content->setStyleSheet(
        "QTableWidget {"
        "    gridline-color: #d0d0d0;"
        "    background-color: white;"
        "}"
        "QTableWidget::item {"
        "    padding: 5px;"
        "}"
        "QTableWidget::item:selected {"
        "    background-color: #0078d7;"
        "    color: white;"
        "}"
        );
}
// 新�?�：处理电机数据接收
void MainWindow::onMotorDataReceived(const MotorData_t &data)
{
    // 更新电机状态�?�取区域
    ui->lineEdit_read_motor_x->setText(QString::number(data.motorX, 'f', 2));
    ui->lineEdit_read_motor_y->setText(QString::number(data.motorY, 'f', 2));
    ui->lineEdit_read_motor_speed->setText(QString::number(data.motorSpeed));

    // 更新图表
    updateMotorChart(data.motorX, data.motorY);

    QString message = QString("收到电机数据 - X: %1, Y: %2, 速度: %3")
                          .arg(data.motorX).arg(data.motorY).arg(data.motorSpeed);
    qDebug()<<message;
}
// 更新电机状态图�?
void MainWindow::updateMotorChart(double x, double y)
{
    QPointF newPosition(x, y);

    // 添加到历史轨�?
    positionHistory.append(newPosition);

    // 限制历史数据数量，避免内存过度�?�长
    if (positionHistory.size() > 1000) {
        positionHistory.removeFirst();
    }

    // 更新轨迹序列
    motorTrajectorySeries->clear();
    for (const QPointF &point : positionHistory) {
        *motorTrajectorySeries << point;
    }

    // 更新当前位置标�??
    currentPositionSeries->clear();
    *currentPositionSeries << newPosition;

    // �?动调整坐标轴范围
    if (!positionHistory.isEmpty()) {
        double minX = x, maxX = x, minY = y, maxY = y;
        for (const QPointF &point : positionHistory) {
            minX = qMin(minX, point.x());
            maxX = qMax(maxX, point.x());
            minY = qMin(minY, point.y());
            maxY = qMax(maxY, point.y());
        }

        // 添加一些边�?
        double marginX = (maxX - minX) * 0.1;
        double marginY = (maxY - minY) * 0.1;

        if (marginX < 10) marginX = 10;
        if (marginY < 10) marginY = 10;

        axisX->setRange(minX - marginX, maxX + marginX);
        axisY->setRange(minY - marginY, maxY + marginY);
    }
}
// 新�?�：处理通道数据接收
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

// 新�?�：处理优化结果数据接收
void MainWindow::onOptResDataReceived(const OptResData_t &data)
{
    // 更新寻优结果显示区域
    ui->lineEdit_read_channel1voltagemax->setText(QString::number(data.optimalVs[0], 'f', 2));
    ui->lineEdit_read_channel2voltagemax->setText(QString::number(data.optimalVs[1], 'f', 2));
    ui->lineEdit_read_channel3voltagemax->setText(QString::number(data.optimalVs[2], 'f', 2));
    ui->lineEdit_read_channel4voltagemax->setText(QString::number(data.optimalVs[3], 'f', 2));
    ui->lineEdit_read_powermax->setText(QString::number(data.optimalPower, 'f', 2));

    QString message = QString("收到优化结果 - 最大功�?: %1W, 电压: [%2, %3, %4, %5]V")
                          .arg(data.optimalPower)
                          .arg(data.optimalVs[0]).arg(data.optimalVs[1])
                          .arg(data.optimalVs[2]).arg(data.optimalVs[3]);
    qDebug()<<message;
}

// 用配�?值初始化UI
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
    ui->lineEdit_initial_voltage->setText(QString::number(findOptCmd.initialVol, 'f', 2));
    ui->lineEdit_voltage_step->setText(QString::number(findOptCmd.volStepLen, 'f', 2));

    // 初�?�化工况状�? - �?保UI状态�?�确
    trajTypeChanged(ui->comboBox_traj_type->currentIndex());
    
}

// 新�?�：轨迹类型变化处理
void MainWindow::trajTypeChanged(int index)
{
    if (index == 0) { // 方形轨迹
        ui->lineEdit_square_step_2->setEnabled(true); // 方形边长
        ui->lineEdit_square_step->setEnabled(true);
        ui->lineEdit_circle_radius->setEnabled(false);
        ui->label_11->setEnabled(true);  // 方形边长标�??
        ui->label_8->setEnabled(true);  // 方形步长标�??
        ui->label_7->setEnabled(false); // 圆形半径标�??

        // 使用更明�?的样式�?�置
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
        ui->label_11->setEnabled(false); // 方形边长标�??
        ui->label_8->setEnabled(false); // 方形步长标�??
        ui->label_7->setEnabled(true);  // 圆形半径标�??

        // 使用更明�?的样式�?�置
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
    // 注意：由于Qt的�?�象父子关系，大部分资源会自动清�?
    // 但为了安�?，可以显式删除主要�?�象
    if (chartView) {
        delete chartView;
    }
    delete ui;
}

// 电机控制按钮点击事件
void MainWindow::on_pushButton_motor_control_clicked()
{
    if (!commandTransmitter || !commandTransmitter->m_clientSocket) {
        QMessageBox::warning(this, "错�??", "请先�?动服务器并等待STM32连接");
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
        qDebug() << "电机控制命令发送成�?: X=" << motorCmd.x << ", Y=" << motorCmd.y << ", Speed=" << motorCmd.speed;
    } else {
        qDebug() << "电机控制命令发送失败，错�??�?:" <<result ;
    }
}

// 寻优控制按钮点击事件
void MainWindow::on_pushButton_find_optimal_clicked()
{
    if (!commandTransmitter || !commandTransmitter->m_clientSocket) {
        QMessageBox::warning(this, "错�??", "请先�?动服务器并等待STM32连接");
        return;
    }

    // 获取轨迹类型
    int trajTypeIndex = ui->comboBox_traj_type->currentIndex();
    ThajType_t whichThaj = (trajTypeIndex == 0) ? SQU_TRAJ : CIR_TRAJ;

    // 根据轨迹类型获取正确的参�?
    float cirTrajRad = 0.0f;
    float squThajLen = 0.0f;  // 新�?�：方形轨迹边长
    uint8_t squThajStepLen = 0;
    float maxVol = ui->lineEdit_max_voltage->text().toFloat();
    float initialVol = ui->lineEdit_initial_voltage->text().toFloat();
    float volStepLen = ui->lineEdit_voltage_step->text().toFloat();

    if (whichThaj == SQU_TRAJ) {
        // 方形轨迹：只使用方形步长，圆形半径�?�为0
        squThajLen = ui->lineEdit_square_step_2->text().toFloat(); // 获取方形边长
        squThajStepLen = ui->lineEdit_square_step->text().toUShort();
        cirTrajRad = 0.0f;

        // �?保UI状态�?�确（方形�?�长�?�?，圆形半径�?�用�?
        ui->lineEdit_square_step_2->setEnabled(true);
        ui->lineEdit_square_step->setEnabled(true);
        ui->lineEdit_circle_radius->setEnabled(false);

        // 验证方形轨迹参数
        if (squThajLen <= 0) {
            QMessageBox::warning(this, "参数错�??", "方形轨迹必须设置有效的方形边�?");
            return;
        }
        if (squThajStepLen == 0) {
            QMessageBox::warning(this, "参数错�??", "方形轨迹必须设置方形步长");
            return;
        }

    } else {
        // 圆形轨迹：只使用圆形半径，方形参数�?�为0
        cirTrajRad = ui->lineEdit_circle_radius->text().toFloat();
        squThajLen = 0.0f;
        squThajStepLen = 0;

        // �?保UI状态�?�确（圆形半径可�?，方形参数�?�用�?
        ui->lineEdit_circle_radius->setEnabled(true);
        ui->lineEdit_square_step_2->setEnabled(false);
        ui->lineEdit_square_step->setEnabled(false);
        // 验证圆形轨迹参数
        if (cirTrajRad <= 0) {
            QMessageBox::warning(this, "参数错�??", "圆形轨迹必须设置有效的圆形半�?");
            return;
        }
    }

    if (maxVol <= 0 || volStepLen <= 0) {
        QMessageBox::warning(this, "参数错�??", "最大电压和电压步长必须大于0");
        return;
    }
    // �?一步：先发送时间命�?
    int timeResult = commandTransmitter->send_time_command();
    if (timeResult == 0) {
        qDebug()<<"时间命令发送成�?";
    } else {
        qDebug() << "时间命令发送失败，错�??�?:" << timeResult;
        // 即使时间命令失败，也继续发送�?�优命令
    }

    int result = commandTransmitter->send_find_opt_command(whichThaj, cirTrajRad, squThajLen, squThajStepLen, maxVol, volStepLen,initialVol);
    if (result == 0) {
        qDebug()<<"寻优控制命令发送成�?";

        // 更新commandTransmitter�?的配�?
        FindOptimalCmd_t findOptCmd = commandTransmitter->getFindOptCmd();
        findOptCmd.whichThaj = whichThaj;
        findOptCmd.cirTrajRad = cirTrajRad;
        findOptCmd.squThajLen = squThajLen;   // 新�?�：保存方形边长
        findOptCmd.squThajStepLen = squThajStepLen;
        findOptCmd.maxVol = maxVol;
        findOptCmd.volStepLen = volStepLen;
        findOptCmd.initialVol = initialVol;
        commandTransmitter->setFindOptCmd(findOptCmd);

        // 显示发送的参数信息
        QString paramInfo;
        if (whichThaj == SQU_TRAJ) {
            paramInfo = QString("轨迹类型: 方形轨迹, 方形边长: %1mm, 方形步长: %2mm, 最大电�?: %3V, 电压步长: %4V, 初�?�电�?: %3V")
                            .arg(squThajLen).arg(squThajStepLen).arg(maxVol).arg(volStepLen).arg(initialVol);
        } else {
            paramInfo = QString("轨迹类型: 圆形轨迹, 圆形半径: %1m, 最大电�?: %2V, 电压步长: %3V, 初�?�电�?: %2V")
                            .arg(cirTrajRad).arg(maxVol).arg(volStepLen).arg(initialVol);
        }
       qDebug()<<paramInfo;
    } else {
        qDebug() << "寻优控制命令发送失败，错�??�?:" << result;
    }
}


// 网络连接按钮点击事件
void MainWindow::on_pushButton_connect_clicked()
{
    // 获取用户输入的IP地址和�??�?
    QString ipText = ui->lineEdit_local_ip->text().trimmed();
    QString portText = ui->lineEdit_local_port->text().trimmed();

    // 验证输入
    if (ipText.isEmpty()) {
        QMessageBox::warning(this, "输入错�??", "请输入服务器IP地址");
        return;
    }

    quint16 serverPort = portText.toUShort();
    if (serverPort == 0) {
        QMessageBox::warning(this, "输入错�??", "请输入�?�确的服务器�?�?");
        return;
    }

    QHostAddress serverAddress(ipText);
    if (serverAddress.isNull()) {
        QMessageBox::warning(this, "输入错�??", "请输入�?�确的IP地址格式");
        return;
    }

    qDebug() << "尝试�?动服务器，IP:" << ipText << "�?�?:" << serverPort;

    // �?动服务器
    if (commandTransmitter->start_server(serverPort, serverAddress)) {
        ui->label_status->setText("已连�?");
        ui->label_status->setStyleSheet("color: green;");
        ui->pushButton_connect->setEnabled(false);
        ui->pushButton_disconnect->setEnabled(true);
        qDebug() << "服务器启动成功，等待客户�?连接";
    } else {
        QMessageBox::critical(this, "错�??", "无法�?动服务器");
        ui->label_status->setText("�?动失�?");
        ui->label_status->setStyleSheet("color: red;");
        qDebug() << "服务器启动失�?";
    }
}

// �?开连接按钮点击事件
void MainWindow::on_pushButton_disconnect_clicked()
{
    if (commandTransmitter) {
        commandTransmitter->stop_server();
    }

    updateConnectionStatus(false);
    qDebug() << "服务器已停�??";
}

// 更新连接状�?
void MainWindow::updateConnectionStatus(bool connected)
{
    if (connected) {
        ui->pushButton_connect->setEnabled(false);
        ui->pushButton_disconnect->setEnabled(true);
        ui->label_status->setText("客户�?已连�?");
        ui->label_status->setStyleSheet("color: green;");
    } else {
        ui->pushButton_connect->setEnabled(true);
        ui->pushButton_disconnect->setEnabled(false);
        ui->label_status->setText("�?连接");
        ui->label_status->setStyleSheet("color: red;");
    }
}
// 设置数据监控区域为只�?
void MainWindow::setupReadOnlyDataMonitoring()
{
    // 电机当前状态�?�取区域
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

    // 设置�?读状态下的样式，使其看起来更明显
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

// TCP连接成功 - 现在不需要这�?函数，因为CommandTransmitter�?服务�?
void MainWindow::onSocketConnected()
{
    // 空实现，因为我们现在�?服务器模�?
}

// TCP连接�?开 - 现在不需要这�?函数
void MainWindow::onSocketDisconnected()
{
    // 空实现，因为我们现在�?服务器模�?
}

// 接收数据 - 现在不需要这�?函数，因为CommandTransmitter处理数据接收
void MainWindow::onSocketReadyRead()
{
    // 空实现，因为CommandTransmitter处理数据接收
}

// TCP错�??处理
void MainWindow::onSocketError(QAbstractSocket::SocketError error)
{
    QString errorMsg;
    switch (error) {
    case QAbstractSocket::ConnectionRefusedError:
        errorMsg = "连接�?拒绝";
        break;
    case QAbstractSocket::RemoteHostClosedError:
        errorMsg = "远程主机关闭连接";
        break;
    case QAbstractSocket::HostNotFoundError:
        errorMsg = "找不到主�?";
        break;
    case QAbstractSocket::SocketTimeoutError:
        errorMsg = "连接超时";
        break;
    default:
        errorMsg = "连接错�??";
    }

    QMessageBox::critical(this, "连接错�??", errorMsg);
    updateConnectionStatus(false);
}


// 解析以太网数�?
void MainWindow::parseEthernetData(const QByteArray &data)
{
    QString receivedData = QString::fromUtf8(data);

    // 示例解析：假设数�?格式�? "TYPE:DATA"
    if (receivedData.contains(":")) {
        QStringList parts = receivedData.split(":");
        if (parts.size() >= 2) {
            QString dataType = parts[0];
            QString dataContent = parts[1];

            QString message = QString("以太网数�? - 类型: %1, 内�??: %2")
                                  .arg(dataType).arg(dataContent);
            qDebug()<<message;
        }
    } else {
        // 直接显示原�?�数�?
        qDebug() << "以太网原始数�?:"+ receivedData;
    }

    qDebug() << "解析以太网数�?:" << data;
}


// // 文本编辑框�?�制�?用状态变化的槽函�?
// void MainWindow::on_textEdit_message_copyAvailable(bool available)
// {
//     Q_UNUSED(available)
// }

// void MainWindow::on_textEdit_sd_data_copyAvailable(bool available)
// {
//     Q_UNUSED(available)
// }

// 现代化样�?
void MainWindow::applyModernStyle()
{
    // 设置基本样式，但不�?�盖轨迹相关的特殊样�?
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

        /* 禁用的输入�?�样�? - 会�??更具体的样式覆盖 */
        QLineEdit:disabled {
            background-color: #f0f0f0;
            color: #999999;
        }
    )";

    setStyleSheet(style);
}
// �?描数�?文件
// �?描数�?文件
void MainWindow::scanDataFiles()
{
    // 清空当前文件列表
    ui->listWidget_files->clear();
    m_currentSelectedFile.clear();
    ui->label_selected_file->setText("当前选中文件：无");

    // 获取当前工作�?�?
    QDir currentDir(QDir::currentPath());

    // 设置文件过滤�?，查找CSV文件
    QStringList filters;
    filters << "*.csv";
    currentDir.setNameFilters(filters);

    // 获取文件列表并按�?改时间排序（最新的在前�?
    QFileInfoList fileList = currentDir.entryInfoList(QDir::Files, QDir::Time);

    if (fileList.isEmpty()) {
        qDebug() << "�?找到任何CSV数据文件";

        // 添加提示�?
        QListWidgetItem *item = new QListWidgetItem("�?找到CSV数据文件", ui->listWidget_files);
        item->setFlags(item->flags() & ~Qt::ItemIsSelectable); // 设置为不�?选择
        return;
    }

    // 添加到列表控�?
    for (const QFileInfo &fileInfo : fileList) {
        // �?显示文件名，不显示完整路�?
        QString fileName = fileInfo.fileName();

        // �?保文件名�?.csv结尾，防止显示其他类型的文件
        if (fileName.toLower().endsWith(".csv")) {
            QString displayText = QString("%1 (大小: %2字节)")
                                      .arg(fileName)
                                      .arg(fileInfo.size());

            QListWidgetItem *item = new QListWidgetItem(displayText, ui->listWidget_files);
            item->setData(Qt::UserRole, fileInfo.absoluteFilePath()); // 保存完整�?�?

            qDebug() << "添加文件到列�?:" << fileName << "完整�?�?:" << fileInfo.absoluteFilePath();
        }
    }

    qDebug() << "找到" << fileList.size() << "�?数据文件";
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

    QString displayText = QString("当前选中文件�?%1").arg(QFileInfo(m_currentSelectedFile).fileName());
    ui->label_selected_file->setText(displayText);

    qDebug() << "选中文件�?" << m_currentSelectedFile;
}

// 显示文件内�??
void MainWindow::displayFileContent(const QString &filePath)
{
    if (filePath.isEmpty()) {
        QMessageBox::warning(this, "警告", "请先选择要查看的文件");
        return;
    }

    loadCsvToTable(filePath);
}
/*
// 更健�?的CSV解析函数
QStringList parseCsvLine(const QString &line)
{
    QStringList result;
    QString field;
    bool inQuotes = false;

    for (int i = 0; i < line.length(); ++i) {
        QChar current = line[i];

        if (current == '"') {
            inQuotes = !inQuotes;
        } else if (current == ',' && !inQuotes) {
            result.append(field.trimmed());
            field.clear();
        } else {
            field.append(current);
        }
    }

    // 添加最后一�?字�??
    result.append(field.trimmed());

    return result;
}*/
// 加载CSV文件到表�?
void MainWindow::loadCsvToTable(const QString &filePath)
{
    QFile file(filePath);
    if (!file.exists()) {
        QMessageBox::critical(this, "错�??", QString("文件不存�?: %1").arg(filePath));
        return;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "错�??", QString("无法打开文件: %1").arg(filePath));
        return;
    }

    // 清空表格
    ui->tableWidget_file_content->clear();
    ui->tableWidget_file_content->setRowCount(0);
    ui->tableWidget_file_content->setColumnCount(0);

    QTextStream in(&file);
    QStringList lines;

    // 读取所有�??
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (!line.trimmed().isEmpty()) {
            lines.append(line);
        }
    }
    file.close();

    if (lines.isEmpty()) {
        qDebug() << "文件为空:" << filePath;
        QMessageBox::information(this, "提示", "选中的文件为�?");
        return;
    }

    // 解析CSV内�??
    QStringList headers;
    QList<QStringList> data;

    for (int i = 0; i < lines.size(); ++i) {
        QString line = lines[i];
        // 简单的CSV解析（�?�理逗号分隔，但不�?�理引号内的逗号�?
        QStringList fields = line.split(',');
        //QStringList fields = parseCsvLine(line);

        // 移除每个字�?�的空白字�??
        for (int j = 0; j < fields.size(); ++j) {
            fields[j] = fields[j].trimmed();
        }

        if (i == 0) {
            // �?一行作为表�?
            headers = fields;
        } else {
            // 数据�?
            data.append(fields);
        }
    }

    // 设置表格列数和表�?
    ui->tableWidget_file_content->setColumnCount(headers.size());
    ui->tableWidget_file_content->setHorizontalHeaderLabels(headers);

    // 设置行数
    ui->tableWidget_file_content->setRowCount(data.size());

    // �?充数�?
    for (int row = 0; row < data.size(); ++row) {
        const QStringList &rowData = data[row];

        for (int col = 0; col < rowData.size() && col < headers.size(); ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(rowData[col]);

            // 尝试将数值列右�?�齐
            bool isNumber;
            rowData[col].toDouble(&isNumber);
            if (isNumber) {
                item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            } else {
                item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            }

            ui->tableWidget_file_content->setItem(row, col, item);
        }
    }

    // 调整列�??
    ui->tableWidget_file_content->resizeColumnsToContents();

    // 显示文件信息
    QString info = QString("文件: %1 | 行数: %2 | 列数: %3")
                       .arg(QFileInfo(filePath).fileName())
                       .arg(data.size())
                       .arg(headers.size());

    qDebug() << "已加载CSV文件到表�?:" << filePath << "行数:" << data.size() << "列数:" << headers.size();
}
// 文件存储情况显示按钮点击
void MainWindow::on_pushButton_filenamedisplay_clicked()
{
    qDebug() << "开始扫描数�?文件...";
    scanDataFiles();
}

// 文件内�?�查看按�?点击
void MainWindow::on_pushButton_read_file_clicked()
{
    if (m_currentSelectedFile.isEmpty()) {
        QMessageBox::warning(this, "警告", "请先选择要查看的文件");
        return;
    }

    qDebug() << "查看文件内�?�：" << m_currentSelectedFile;
    displayFileContent(m_currentSelectedFile);
}
/**********************�?台开�?测试部分*****************************/
//按钮：btn_set_speed
void MainWindow::on_btn_set_speed_clicked()
{
    // ---- 1. 读取 UI 输入 ----
    float x_speed = ui->line_edit_x_speed_cmd->text().toFloat();
    float y_speed = ui->line_edit_y_speed_cmd->text().toFloat();

    QString x_dir = ui->combo_box_x_speed_cmd->currentText();
    QString y_dir = ui->combo_box_y_speed_cmd->currentText();

    // ---- 2. 设置 X 轴速度 ----
    turntable_controller->set_axis_speed(Yaw, x_speed);

    // ---- 设置 X 轴方�? ----
    if (x_dir == "正转")
    turntable_controller->set_manual_rotation(Yaw, Left);
    else
        turntable_controller->set_manual_rotation(Yaw, Right);

    // ---- 3. 设置 Y 轴速度 ----
    turntable_controller->set_axis_speed(Pitch, y_speed);

    // ---- 设置 Y 轴方�? ----
    if (y_dir == "正转")
        turntable_controller->set_manual_rotation(Pitch, Left);
    else
        turntable_controller->set_manual_rotation(Pitch, Right);
}
//停�?? X�?
void MainWindow::on_btn_stop_x_turntable_run_clicked()
{
    turntable_controller->stop_manual_rotation(Yaw);
}
//停�?? Y�?
void MainWindow::on_btn_stop_y_turntable_run_clicked()
{
    turntable_controller->stop_manual_rotation(Pitch);
}
//设置 X 轴位�?�?
void MainWindow::on_btn_set_x_pos_clicked()
{
    float x_pos = ui->line_edit_x_pos->text().toFloat();
    turntable_controller->set_axis_speed(Yaw, x_pos);
}
//设置 Y 轴位�?�?
void MainWindow::on_btn_set_y_pos_clicked()
{
    float y_pos = ui->line_edit_y_pos->text().toFloat();
    turntable_controller->set_axis_speed(Pitch, y_pos);
}
//x坐标清零功能
void MainWindow::on_btn_x_zero_clicked()
{
    turntable_controller->reset_axis_coord(Yaw);
}
//y坐标清零功能
void MainWindow::on_btn_y_zero_clicked()
{
    turntable_controller->reset_axis_coord(Pitch);
}

//连接�?台按�?槽函�?
void MainWindow::on_pushButton_connection_clicked()
{
    QString port = ui->lineEdit_port->text().trimmed();
    const int baudrate = 115200;
    const char parity = 'N';
    const int dataBit = 8;
    const int stopBit = 1;

    if (port.isEmpty()) {
        QMessageBox::warning(this, "错�??", "请填写串口路�?");
        return;
    }

    // 创建�?台控制�?�象
    if (!turntable_controller) {
        turntable_controller = new TurntableController(port.toStdString().c_str());
    }

    // 尝试连接
    bool ok = turntable_controller->connect(baudrate, parity, dataBit, stopBit);
    if (ok) {
        setTurntableConnectionStatus(true);
       // turntableMonitorTimer->start(200); // �?200ms刷新数据
        QMessageBox::information(this, "成功", "�?台连接成�?");
    } else {
        setTurntableConnectionStatus(false);
        QMessageBox::critical(this, "失败", "�?台连接失�?");
    }
}
//�?开�?台按�?槽函�?
void MainWindow::on_pushButton_disconnection_clicked()
{
    if (turntable_controller) {
        turntable_controller->disconnect();
        turntableMonitorTimer->stop();
        setTurntableConnectionStatus(false);
        QMessageBox::information(this, "提示", "�?台已�?开连接");
    }
}
//更新�?台状态显示函数（定时器调�?�?
void MainWindow::updateTurntableData()
{
    if (!turntable_controller) return;

    float xPos = 0.0f, yPos = 0.0f, xSpeed = 0.0f, ySpeed = 0.0f;

    bool ok1 = turntable_controller->read_axis_angle(Yaw, &xPos);
    bool ok2 = turntable_controller->read_axis_angle(Pitch, &yPos);
    bool ok3 = turntable_controller->read_axis_speed(Yaw, &xSpeed);
    bool ok4 = turntable_controller->read_axis_speed(Pitch, &ySpeed);

    if (ok1 && ok2 && ok3 && ok4) {
         ui->line_edit_monitor_x_pos->setText(QString::number(xPos, 'f', 2));
         ui->line_edit_monitor_y_pos->setText(QString::number(yPos, 'f', 2));
         ui->line_edit_monitor_x_speed->setText(QString::number(xSpeed, 'f', 2));
         ui->line_edit_monitor_y_speed->setText(QString::number(ySpeed, 'f', 2));
     }
}
//更新连接状态辅助函�?
void MainWindow::setTurntableConnectionStatus(bool connected)
{
    if (connected) {
        ui->connection_status->setText("已连�?");
        ui->connection_status->setStyleSheet("color: green;");
        ui->pushButton_connection->setEnabled(false);
        ui->pushButton_disconnection->setEnabled(true);
    } else {
        ui->connection_status->setText("�?连接");
        ui->connection_status->setStyleSheet("color: red;");
        ui->pushButton_connection->setEnabled(true);
        ui->pushButton_disconnection->setEnabled(false);
    }
}
