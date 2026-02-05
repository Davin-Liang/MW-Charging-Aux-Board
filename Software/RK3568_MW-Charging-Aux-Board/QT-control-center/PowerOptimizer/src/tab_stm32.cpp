#include "tab_stm32.h"
#include "mainwindow.h"
#include "./ui_mainwindow.h"


#include <QMessageBox>
#include <QVBoxLayout>
#include <QDebug>
#include <QtCharts/QValueAxis>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

#include <fstream>
#include <iostream>
// JSON 库
#include "nlohmann/json.hpp"

using json = nlohmann::json;

TabSTM32::TabSTM32(MainWindow *mw_)
    : QObject(mw_), mw(mw_)
    ,isMotorRecording(false)
    ,isOptimizationRecording(false)
    ,isChannelRecording(false)
    ,channelInfoIndex(0)
    ,motorChart(nullptr)
    ,motorTrajectorySeries(nullptr)
    ,currentPositionSeries(nullptr)
    ,axisX(nullptr)
    ,axisY(nullptr)
    ,chartView(nullptr)
{
    stm32_MonitorTimer = mw->stm32MonitorTimer;
     
}

TabSTM32::~TabSTM32()
{
    // 主图表视图资源显式释放（其父对象未托管）
    // 添加空指针检查
    if (chartView) {
        delete chartView;
        chartView = nullptr;
    }
    
}

void TabSTM32::setupConnections()
{
    setupReadOnlyDataMonitoring();
    initializeUIWithConfig();
    initializeMotorChart();
    // 按钮绑定
    //电机控制指令
    connect(mw->ui->pushButton_motor_control, &QPushButton::clicked,
            this, &TabSTM32::on_pushButton_motor_control_clicked);
    //寻优按钮
    connect(mw->ui->pushButton_find_optimal, &QPushButton::clicked,
            this, &TabSTM32::on_pushButton_find_optimal_clicked);
    // 确保轨迹类型变化信号正确连接
    connect(mw->ui->comboBox_traj_type,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &TabSTM32::trajTypeChanged);
    // === 数据接收信号 ===


    connect(stm32_MonitorTimer, &QTimer::timeout,
            this, &TabSTM32::onStm32MonitorTimeout);
    stm32_MonitorTimer->start(150); 

}
/**
 * @brief 设置监控区域的 LineEdit 为只读并统一样式。
 */
void TabSTM32::setupReadOnlyDataMonitoring()
{
    static const QString readOnlyStyle =
        "QLineEdit:read-only {"
        " background-color: #f0f0f0;"
        " color: #666666;"
        " border: 1px solid #cccccc;"
        "}";
    auto setRO = [&](QLineEdit *edit) {
        edit->setReadOnly(true);
        edit->setStyleSheet(readOnlyStyle);
    };

    // 电机读数
    setRO(mw->ui->lineEdit_read_motor_x);
    setRO(mw->ui->lineEdit_read_motor_y);
    setRO(mw->ui->lineEdit_read_motor_speed);

    // 寻优实时数据
    setRO(mw->ui->lineEdit_read_currentchannel);
    setRO(mw->ui->lineEdit_read_currentvoltage);
    setRO(mw->ui->lineEdit_read_currentpower);

    // 寻优结果
    setRO(mw->ui->lineEdit_read_channel1voltagemax);
    setRO(mw->ui->lineEdit_read_channel2voltagemax);
    setRO(mw->ui->lineEdit_read_channel3voltagemax);
    setRO(mw->ui->lineEdit_read_channel4voltagemax);
    setRO(mw->ui->lineEdit_read_powermax);
    setRO(mw->ui->lineEdit_opt_motor_x);
    setRO(mw->ui->lineEdit_opt_motor_y);

}
/**
  * @brief 从JSON文件初始化参数
  * @param filename JSON文件名
  * @return true 成功 \ false 失败
  **/
bool TabSTM32::readConfigFromJSON(const std::string &filename)
{
    try {
        // 打开并解析 JSON 文件
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "无法打开文件 " << filename << std::endl;
            return false;
        }

        json config;
        file >> config;
        file.close();

        // 初始化电机配置
        if (config.contains("motorCmd")) {
            json motorConfig = config["motorCmd"];
            motorCmd.x = motorConfig["x"];
            motorCmd.y = motorConfig["y"];
            motorCmd.speed = motorConfig["speed"];
        } else {
            std::cerr << "错误: JSON 中缺少 motorCmd 部分" << std::endl;
            return false;
        }

        // 初始化寻优配置
        if (config.contains("findOptCmd")) {
            json optConfig = config["findOptCmd"];
            findOptCmd.whichThaj = optConfig["whichThaj"];
            findOptCmd.cirTrajRad = optConfig["cirTrajRad"];
            findOptCmd.squThajLen = optConfig["squThajLen"];// 方形轨迹边长[mm]
            findOptCmd.squThajStepLen =  optConfig["squThajStepLen"];// 执行方形轨迹的步长[mm]
            findOptCmd.maxVol = optConfig["maxVol"];
            findOptCmd.volStepLen = optConfig["volStepLen"];
            findOptCmd.initialVol = optConfig["initialVol"];
        } else {
            std::cerr << "错误: JSON 中缺少 findOptCmd 部分" << std::endl;
            return false;
        }

        std::cout << "参数初始化成功！" << std::endl;
        return true;

    } catch (const std::exception &e) {
        std::cerr << "参数初始化异常: " << e.what() << std::endl;
        return false;
    }
}
/**
  * @brief 将 readConfigFromJSON 读取到的配置参数映射到 Modbus 寄存器
  **/
void TabSTM32::writeConfigToModbus()
{
    if (!mw || !mw->stm32_mb_ctx) {
        std::cerr << "Modbus 未连接！" << std::endl;
        return;
    }

    // 写电机配置到 Modbus
    modbus_write_register(mw->stm32_mb_ctx, 0x0001, motorCmd.x);
    modbus_write_register(mw->stm32_mb_ctx, 0x0002, motorCmd.y);
    modbus_write_register(mw->stm32_mb_ctx, 0x0003, motorCmd.speed);

    // 写寻优配置到 Modbus
    modbus_write_register(mw->stm32_mb_ctx, 0x0004, findOptCmd.whichThaj);
    modbus_write_register(mw->stm32_mb_ctx, 0x0005, (findOptCmd.cirTrajRad));
    modbus_write_register(mw->stm32_mb_ctx, 0x0006, findOptCmd.squThajLen);
    modbus_write_register(mw->stm32_mb_ctx, 0x0007, findOptCmd.squThajStepLen);


    modbus_write_register(mw->stm32_mb_ctx, 0x0008, static_cast<uint16_t>(findOptCmd.maxVol));
    modbus_write_register(mw->stm32_mb_ctx, 0x000A, static_cast<uint16_t>(findOptCmd.volStepLen));// 如果是float类型，可以转为uint16处理
    modbus_write_register(mw->stm32_mb_ctx, 0x000C, static_cast<uint16_t>(findOptCmd.initialVol));
    std::cout << "配置已成功写入 Modbus 寄存器" << std::endl;
}
/**
 * @brief 初始化 UI 中的电机参数与寻优参数
 */
void TabSTM32::initializeUIWithConfig()
{
    // 读取配置文件（JSON 格式）
    if (!readConfigFromJSON("config/config.json")) {
        std::cerr << "读取配置失败" << std::endl;
        return;
    }
    // 将配置写入 Modbus 寄存器
    writeConfigToModbus();

    // 获取电机配置
    mw->ui->lineEdit_motor_x->setText(QString::number(motorCmd.x, 'f', 2));
    mw->ui->lineEdit_motor_y->setText(QString::number(motorCmd.y, 'f', 2));
    mw->ui->lineEdit_motor_speed->setText(QString::number(motorCmd.speed));

    // 默认轨迹类型
    mw->ui->comboBox_traj_type->clear();
    mw->ui->comboBox_traj_type->addItem("方形轨迹");
    mw->ui->comboBox_traj_type->addItem("圆形轨迹");

    // 设置轨迹类型
    mw->ui->comboBox_traj_type->setCurrentIndex(findOptCmd.whichThaj == SQU_TRAJ ? 0 : 1);

    mw->ui->lineEdit_square_step_2->setText(QString::number(findOptCmd.squThajLen, 'f', 2));
    mw->ui->lineEdit_square_step->setText(QString::number(findOptCmd.squThajStepLen));
    mw->ui->lineEdit_circle_radius->setText(QString::number(findOptCmd.cirTrajRad, 'f', 2));
    mw->ui->lineEdit_max_voltage->setText(QString::number(findOptCmd.maxVol, 'f', 2));
    mw->ui->lineEdit_initial_voltage->setText(QString::number(findOptCmd.initialVol, 'f', 2));
    mw->ui->lineEdit_voltage_step->setText(QString::number(findOptCmd.volStepLen, 'f', 2));
    // 根据轨迹模式更新 UI
    trajTypeChanged(mw->ui->comboBox_traj_type->currentIndex());

}
/**
 * @brief 初始化电机状态图表，包括轨迹曲线、当前位置标记和坐标轴样式
 */
void TabSTM32::initializeMotorChart()
{
    // 创建图表
    motorChart = new QChart();
    motorChart->setTitle("");  // 不显示标题
    motorChart->setAnimationOptions(QChart::NoAnimation);
    motorChart->legend()->hide();
    motorChart->setMargins(QMargins(0, 0, 0, 0)); // 去除图表边距
    motorChart->setBackgroundRoundness(0);   // 设置图表边距为0，使其完全填充

    // 创建轨迹曲线（蓝色线条）
    motorTrajectorySeries = new QLineSeries();
    // 移除名称设置，不显示图例
    motorTrajectorySeries->setColor(Qt::blue);
    motorTrajectorySeries->setPointsVisible(false);  // 不显示轨迹上的点，只显示线条

    // 创建当前位置点（红色散点）
    currentPositionSeries = new QScatterSeries();
    // 移除名称设置，不显示图例
    currentPositionSeries->setColor(Qt::red);
    currentPositionSeries->setBorderColor(Qt::darkRed);
    currentPositionSeries->setMarkerSize(4);

    // 添加到图表
    motorChart->addSeries(motorTrajectorySeries);
    motorChart->addSeries(currentPositionSeries);

    // 坐标轴 X
    axisX = new QValueAxis();
    axisX->setTitleText("x (mm)");
    axisX->setTitleFont(QFont("宋体", 8));
    axisX->setLabelsFont(QFont("宋体", 8));
    axisX->setRange(-50, 50);  // 初始范围
    axisX->setTickCount(11);

    // 坐标轴 Y
    axisY = new QValueAxis();
    axisY->setTitleText("y (mm)");
    axisY->setTitleFont(QFont("宋体", 8));
    axisY->setLabelsFont(QFont("宋体", 8));
    axisY->setRange(-50, 50);  // 初始范围
    axisY->setTickCount(11);

    // 绑定坐标轴
    motorChart->addAxis(axisX, Qt::AlignBottom);
    motorChart->addAxis(axisY, Qt::AlignLeft);

    motorTrajectorySeries->attachAxis(axisX);
    motorTrajectorySeries->attachAxis(axisY);
    currentPositionSeries->attachAxis(axisX);
    currentPositionSeries->attachAxis(axisY);


    // 创建图表视图
    chartView = new QChartView(motorChart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setContentsMargins(0, 0, 0, 0);  // 设置图表视图的边距为0

    // 将图表添加到UI中的widget_chart
    QVBoxLayout *chartLayout = new QVBoxLayout(mw->ui->widget_chart);
    chartLayout->addWidget(chartView);
    chartLayout->setContentsMargins(0, 0, 0, 0);  // 设置布局边距为0
    chartLayout->setSpacing(0);  // 设置间距为0

    // 初始化历史数据
    positionHistory.clear();

    qDebug()<<"电机位置轨迹图初始化完成";
}

/**
 * @brief 更新电机图表：轨迹曲线 + 实时位置点 + 自适应坐标轴
 */
void TabSTM32::updateMotorChart(double x, double y)
{
    QPointF newPosition(x, y);
    // 添加到历史轨迹
    positionHistory.append(newPosition);

    // 限制历史数据数量，避免内存过度增长 // 限制缓存大小
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
        double marginX = qMax(10.0, (maxX - minX) * 0.1);
        double marginY = qMax(10.0, (maxY - minY) * 0.1);

        axisX->setRange(minX - marginX, maxX + marginX);
        axisY->setRange(minY - marginY, maxY + marginY);
    }
}



/**
 * @brief 当轨迹类型切换时，调整 UI 使对应输入框可用/禁用
 */
void TabSTM32::trajTypeChanged(int index)
{

    const bool isSquare = (index == 0);

    mw->ui->lineEdit_square_step_2->setEnabled(isSquare);
    mw->ui->lineEdit_square_step->setEnabled(isSquare);
    mw->ui->lineEdit_circle_radius->setEnabled(!isSquare);

    mw->ui->label_11->setEnabled(isSquare);
    mw->ui->label_8->setEnabled(isSquare);
    mw->ui->label_7->setEnabled(!isSquare);

    // 样式切换
    mw->ui->lineEdit_square_step_2->setStyleSheet(isSquare ? "QLineEdit{ background:white; color:black; }"
                                                           : "QLineEdit{ background:#f0f0f0; color:#999; border:1px solid #ccc;}");

    mw->ui->lineEdit_square_step->setStyleSheet(isSquare ? "QLineEdit{ background:white; color:black; }"
                                                         : "QLineEdit{ background:#f0f0f0; color:#999; border:1px solid #ccc;}");

    mw->ui->lineEdit_circle_radius->setStyleSheet(!isSquare ? "QLineEdit{ background:white; color:black; }"
                                                            : "QLineEdit{ background:#f0f0f0; color:#999; border:1px solid #ccc;}");

    qDebug() << (isSquare ? "已切换到方形轨迹模式" : "已切换到圆形轨迹模式");
}
/**
 * @brief “电机控制”按钮点击事件。 读取界面输入的电机控制参数并发送给下位机。
 */
void TabSTM32::on_pushButton_motor_control_clicked()
{
    if (!mw || !mw->stm32_mb_ctx) {
        QMessageBox::warning(mw, "错误", "Modbus 未连接");
        return;
    }
    // ===== 1. 生成新的电机数据文件名 =====
    time_t now = time(nullptr);
    tm *lt = localtime(&now);
    DateTime_t dt;
    dt.year = lt->tm_year + 1900;
    dt.month = lt->tm_mon + 1;
    dt.day = lt->tm_mday;
    dt.hour = lt->tm_hour;
    dt.minute = lt->tm_min;
    dt.week_day = lt->tm_wday;
    QString filePath = QString("csv_data") + "/" + QString::fromStdString(generate_file_name("motor_data.csv", &dt));
    currentMotorFile = filePath.toStdString();
    // currentMotorFile = generate_file_name("motor_data.csv", &dt);
    isMotorRecording = true;  // 开始记录电机数据

    // ===== 2. 创建文件并写入表头 =====
    // csv/ + currentMotorFile
    std::ofstream motorFile(currentMotorFile, std::ios::out);
    if (motorFile.is_open()) {
        motorFile << "X,Y,Speed\n";
        motorFile.close();
        qDebug() << "开始记录电机数据到文件：" << QString::fromStdString(currentMotorFile);
    }

    // ===== 3. 写电机参数（保持寄存器）=====
    writeMotorRegisters();

    // ===== 4. 打开电机控制线圈 =====
    modbus_write_bit(mw->stm32_mb_ctx, 0x0002, 1);

    // ===== 5. 写时间 =====
    writeTimeRegisters();

    // ===== 6. 更新标志位：bit0(电机) + bit2(时间) =====
    setUpdateFlag((1 << 0) | (1 << 2));

    qDebug() << "电机控制参数已发送";
}
/**
 * @brief 写电机参数（保持寄存器）
 */
void TabSTM32::writeMotorRegisters()
{
    uint16_t regs[3];

    int x = mw->ui->lineEdit_motor_x->text().toInt();
    int y = mw->ui->lineEdit_motor_y->text().toInt();
    uint16_t speed = mw->ui->lineEdit_motor_speed->text().toUInt();

    // 显式转换成 int16，再映射到 uint16
    regs[0] = static_cast<uint16_t>(static_cast<int16_t>(x));
    regs[1] = static_cast<uint16_t>(static_cast<int16_t>(y));
    regs[2] = speed;

    qDebug() << "X =" <<  regs[0]
             << "Y =" << regs[1]
             << "Speed =" << regs[2];



    modbus_write_registers(mw->stm32_mb_ctx, 0x0001, 3, regs);
}


/**
 * @brief “寻优控制”按钮点击事件。根据用户选择的轨迹类型读取参数并发送寻优控制命令。
 */
void TabSTM32::on_pushButton_find_optimal_clicked()
{
    if (!mw || !mw->stm32_mb_ctx) {
        QMessageBox::warning(mw, "错误", "Modbus 未连接");
        return;
    }
    // ===== 1. 生成新的寻优文件名 =====
    time_t now = time(nullptr);
    tm *lt = localtime(&now);
    DateTime_t dt;
    dt.year = lt->tm_year + 1900;
    dt.month = lt->tm_mon + 1;
    dt.day = lt->tm_mday;
    dt.hour = lt->tm_hour;
    dt.minute = lt->tm_min;
    dt.week_day = lt->tm_wday;
    // QString baseDir = QCoreApplication::applicationDirPath() + "/csv_data";
    // QDir dir(baseDir);
    // if (!dir.exists())
    //     dir.mkpath(".");

    // currentOptResultFile = generate_file_name("optimal_result.csv", &dt);
    // currentChannelFile = generate_file_name("channel_info.csv", &dt);

    QString filePath = QString("csv_data") + "/" + QString::fromStdString(generate_file_name("optimal_result.csv", &dt));
    currentOptResultFile = filePath.toStdString();
    filePath = QString("csv_data") + "/" + QString::fromStdString(generate_file_name("channel_info.csv", &dt));
    currentChannelFile = filePath.toStdString();
    isOptimizationRecording = true;  // 开始记录寻优结果
    isChannelRecording = true;       // 开始记录通道信息
    channelInfoIndex = 0;            // 重置索引


    // ===== 2. 创建文件并写入表头 =====
    std::ofstream optFile(currentOptResultFile, std::ios::out);
    if (optFile.is_open()) {
        optFile << "X,Y,P,V1,V2,V3,V4\n";
        optFile.close();
        qDebug() << "开始记录寻优结果到文件：" << QString::fromStdString(currentOptResultFile);
    }

    std::ofstream channelFile(currentChannelFile, std::ios::out);
    if (channelFile.is_open()) {
        channelFile << "Index,Voltage,Power,Channel\n";
        channelFile.close();
        qDebug() << "开始记录通道信息到文件：" << QString::fromStdString(currentChannelFile);
    }

    // ===== 3. 写寻优参数 =====
    writeFindOptRegisters();

    // ===== 4. 打开寻优控制线圈 =====
    modbus_write_bit(mw->stm32_mb_ctx, 0x0001, 1);

    // ===== 5. 写时间 =====
    writeTimeRegisters();

    // ===== 6. 更新标志位：bit1(寻优) + bit2(时间) =====
    setUpdateFlag((1 << 1) | (1 << 2));

    qDebug() << "寻优控制参数已发送";

}

/**
 * @brief 写寻优参数
 */
void TabSTM32::writeFindOptRegisters()
{
    uint16_t reg;

    // 轨迹类型
    reg = (mw->ui->comboBox_traj_type->currentIndex() == 0) ? 1 : 2;
    modbus_write_register(mw->stm32_mb_ctx, 0x0004, reg);

    // 圆 / 方参数
    modbus_write_register(mw->stm32_mb_ctx, 0x0005, static_cast<uint16_t>(mw->ui->lineEdit_circle_radius->text().toUInt()));
    modbus_write_register(mw->stm32_mb_ctx, 0x0006, static_cast<uint16_t>(mw->ui->lineEdit_square_step_2->text().toUInt()));
    modbus_write_register(mw->stm32_mb_ctx, 0x0007, static_cast<uint16_t>(mw->ui->lineEdit_square_step->text().toUInt()));

    // ================= float 参数转换为大端序 =================
    auto floatToBigEndianRegs = [](float value, uint16_t buf[2]){
        uint32_t temp;
        std::memcpy(&temp, &value, sizeof(float));  // 把 float 按位复制到 uint32_t
        buf[0] = static_cast<uint16_t>((temp >> 16) & 0xFFFF); // 高 16 位 -> 寄存器 0
        buf[1] = static_cast<uint16_t>(temp & 0xFFFF);         // 低 16 位 -> 寄存器 1
    };
    // float 参数
    float v;
    uint16_t fbuf[2];

    v = mw->ui->lineEdit_max_voltage->text().toFloat();
    floatToBigEndianRegs(v, fbuf);
    modbus_write_registers(mw->stm32_mb_ctx, 0x0008, 2, fbuf);
    qDebug().nospace()
    << "[Modbus TX] MaxVoltage = " << v
    << ", Addr=0x0008"
    << ", Reg[0]=0x" << QString::number(fbuf[0], 16).rightJustified(4, '0')
    << ", Reg[1]=0x" << QString::number(fbuf[1], 16).rightJustified(4, '0');
    v = mw->ui->lineEdit_voltage_step->text().toFloat();
    floatToBigEndianRegs(v, fbuf);
    modbus_write_registers(mw->stm32_mb_ctx, 0x000A, 2, fbuf);
    qDebug().nospace()
    << "[Modbus TX] VoltageStep = " << v
    << ", Addr=0x000A"
    << ", Reg[0]=0x" << QString::number(fbuf[0], 16).rightJustified(4, '0')
    << ", Reg[1]=0x" << QString::number(fbuf[1], 16).rightJustified(4, '0');
    v = mw->ui->lineEdit_initial_voltage->text().toFloat();
    floatToBigEndianRegs(v, fbuf);
    modbus_write_registers(mw->stm32_mb_ctx, 0x000C, 2, fbuf);
    qDebug().nospace()
    << "[Modbus TX] InitialVoltage = " << v
    << ", Addr=0x000C"
    << ", Reg[0]=0x" << QString::number(fbuf[0], 16).rightJustified(4, '0')
    << ", Reg[1]=0x" << QString::number(fbuf[1], 16).rightJustified(4, '0');
    

}
/**
 * @brief 获取时间并写入
 */
void TabSTM32::writeTimeRegisters()
{
    time_t now = time(nullptr);
    tm *lt = localtime(&now);

    uint16_t year = lt->tm_year + 1900;
    uint16_t month_day = ((lt->tm_mon + 1) << 8) | lt->tm_mday;
    uint16_t hour_min = (lt->tm_hour << 8) | lt->tm_min;

    modbus_write_register(mw->stm32_mb_ctx, 0x000E, year);
    modbus_write_register(mw->stm32_mb_ctx, 0x000F, month_day);
    modbus_write_register(mw->stm32_mb_ctx, 0x0010, hour_min);

    // 时间线圈
    modbus_write_bit(mw->stm32_mb_ctx, 0x0003, 1);
}
/**
 * @brief 0x0011 数据更新标志位
 */
void TabSTM32::setUpdateFlag(uint16_t bitMask)
{
    uint16_t flag = 0;
    modbus_read_registers(mw->stm32_mb_ctx, 0x0011, 1, &flag);
    flag |= bitMask;
    modbus_write_register(mw->stm32_mb_ctx, 0x0011, flag);
}

/**
 * @brief 获取实时数据，实现数据监控
 */
void TabSTM32::onStm32MonitorTimeout()
{
    if (!mw || !mw->stm32_mb_ctx)
        return;


    uint16_t regs[20] = {0};

    // 从 0x0001 连续读 0x0013
    int rc = modbus_read_input_registers(
        mw->stm32_mb_ctx,0x0001,0x0013 - 0x0001 + 1,regs);

    if (rc == -1) {
        qDebug() << "Modbus read failed:" << modbus_strerror(errno);
        return;
    }

    // ========= 数据解析 =========
    int16_t motor_x = (int16_t)regs[0];   // 0x0001
    int16_t motor_y = (int16_t)regs[1];   // 0x0002
    uint16_t speed  = regs[2];            // 0x0003
    uint8_t channel = regs[3]& 0x00FF;     // 0x0004 （只取低8位）

    updateMotorChart(motor_x, motor_y);

    // ========= 浮点数据（大端）=========
    float voltage = modbus_get_float_abcd(&regs[4]);   // 0x0005 + 0x0006
    float power   = modbus_get_float_abcd(&regs[6]);   // 0x0007 + 0x0008

    // ========= 优化结果 =========
    int16_t opt_x  = (int16_t)regs[8];      // 0x0009
    int16_t opt_y  = (int16_t)regs[9];      // 0x000A
    float opt_pwr  = modbus_get_float_abcd(&regs[10]); // 0x000B + 0x000C

    float opt_v1 = modbus_get_float_abcd(&regs[12]);  // 0x000D + 0x000E
    float opt_v2 = modbus_get_float_abcd(&regs[14]);  // 0x000F + 0x0010
    float opt_v3 = modbus_get_float_abcd(&regs[16]);  // 0x0011 + 0x0012
    float opt_v4 = modbus_get_float_abcd(&regs[18]);  // 0x0013 + 0x0014

    // ========= UI 更新 =========
    mw->ui->lineEdit_read_motor_x->setText(QString::number(motor_x));
    mw->ui->lineEdit_read_motor_y->setText(QString::number(motor_y));
    mw->ui->lineEdit_read_motor_speed->setText(QString::number(speed));
    mw->ui->lineEdit_read_currentchannel->setText(QString::number(channel));

    mw->ui->lineEdit_read_currentvoltage->setText(QString::number(voltage, 'f', 2));
    mw->ui->lineEdit_read_currentpower->setText(QString::number(power, 'f', 2));

    mw->ui->lineEdit_opt_motor_x->setText(QString::number(opt_x));
    mw->ui->lineEdit_opt_motor_y->setText(QString::number(opt_y));
    mw->ui->lineEdit_read_powermax->setText(QString::number(opt_pwr, 'f', 2));

    mw->ui->lineEdit_read_channel1voltagemax->setText(QString::number(opt_v1, 'f', 2));
    mw->ui->lineEdit_read_channel2voltagemax->setText(QString::number(opt_v2, 'f', 2));
    mw->ui->lineEdit_read_channel3voltagemax->setText(QString::number(opt_v3, 'f', 2));
    mw->ui->lineEdit_read_channel4voltagemax->setText(QString::number(opt_v4, 'f', 2));

    qDebug().nospace()
        << "Motor(" << motor_x << "," << motor_y << "), "
        << "Speed=" << speed
        << ", Ch=" << channel
        << ", V=" << voltage
        << ", P=" << power
        << ", Opt=(" << opt_x << "," << opt_y << "), "
        << "Pmax=" << opt_pwr
        << ", Vs=[" << opt_v1 << "," << opt_v2 << "," << opt_v3 << "," << opt_v4 << "]";
    // 填充电机数据结构
    motorData.motorX = motor_x;
    motorData.motorY = motor_y;
    motorData.motorSpeed = speed;
    // 填充优化结果数据结构
    optResData.optimalx = opt_x;
    optResData.optimaly = opt_y;
    optResData.optimalPower = opt_pwr;
    optResData.optimalVs[0] = opt_v1;
    optResData.optimalVs[1] = opt_v2;
    optResData.optimalVs[2] = opt_v3;
    optResData.optimalVs[3] = opt_v4;
    // 填充通道数据结构
    currentVPCh.currentChannel = channel;
    currentVPCh.currentV = voltage;
    currentVPCh.currentP = power;


    // 保存读取的数据到文件（只追加，不创建新文件）
    if (isMotorRecording)
    write_motor_data_to_csv(currentMotorFile, &motorData);
    if (isOptimizationRecording)
        write_opt_res_to_csv(currentOptResultFile, &optResData);
    if (isChannelRecording)
        write_cur_channel_info_to_csv(currentChannelFile, &currentVPCh);
}

/**
 * @brief 将电机数据追加写入CSV文件（不写表头，只追加数据）
 * @param filename CSV文件名（含路径）
 * @param motorData 电机数据结构体指针
 * @return true 写入成功 \ false 写入失败
 */
bool TabSTM32::write_motor_data_to_csv(const std::string& filename, const MotorData_t * motorData)
{
    // 打开文件，追加写入
    std::ofstream file(filename, std::ios::app);

    if (!file.is_open())
        return false; // 文件打开失败

    // 设置浮点数输出精度
    file << std::fixed << std::setprecision(6);

    // 写入数据
    file << motorData->motorX << ","
         << motorData->motorY << ","
         << motorData->motorSpeed << "\n";

    file.close();
    return true;
}

/**
 * @brief 将寻优结果数据追加写入CSV文件（不写表头，只追加数据）
 * @param filename CSV文件名（含路径）
 * @param optData 寻优结果数据结构体指针
 * @return true 写入成功 \ false 写入失败
 */
bool TabSTM32::write_opt_res_to_csv(const std::string& filename, const OptResData_t * optData)
{
    std::ofstream file(filename, std::ios::app);

    if (!file.is_open())
        return false; // 文件打开失败

    file << std::fixed << std::setprecision(6);

    file << optData->optimalx << ","
         << optData->optimaly << ","
         << optData->optimalPower << ","
         << optData->optimalVs[0] << ","
         << optData->optimalVs[1] << ","
         << optData->optimalVs[2] << ","
         << optData->optimalVs[3] << "\n";

    file.close();
    return true;
}

/**
 * @brief 将通道信息数据追加写入CSV文件（不写表头，只追加数据）
 * @param filename CSV文件名（含路径）
 * @param channelData 通道信息数据结构体指针
 * @return true 写入成功 \ false 写入失败
 */
bool TabSTM32::write_cur_channel_info_to_csv(const std::string& filename, const CurrentVPCh_t * channelData)
{
    std::ofstream file(filename, std::ios::app);

    if (!file.is_open())
        return false; // 文件打开失败

    file << std::fixed << std::setprecision(6);

    file << channelInfoIndex << ","
         << channelData->currentV << ","
         << channelData->currentP << ","
         << static_cast<int>(channelData->currentChannel) << "\n";

    file.close();
    channelInfoIndex++;  // 递增索引
    return true;
}
/**
 * @brief 根据时间戳和文件后缀生成唯一的文件名
 * @param fileSuffix 文件后缀（如："motor_data.csv"）
 * @param datetime 时间结构体指针，包含年月日时分和星期信息
 * @return string 生成的文件名，格式："YYYYMMDD_HHMM_Weekday_filesuffix"
 */
std::string TabSTM32::generate_file_name(const std::string& fileSuffix, const DateTime_t* datetime)
{
    if (!datetime)
        return "default_" + fileSuffix;

    const char* weekDays[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

    std::ostringstream filename;

    filename << std::setw(4) << std::setfill('0') << datetime->year
             << std::setw(2) << std::setfill('0') << static_cast<int>(datetime->month)
             << std::setw(2) << std::setfill('0') << static_cast<int>(datetime->day)
             << "_"
             << std::setw(2) << std::setfill('0') << static_cast<int>(datetime->hour)
             << std::setw(2) << std::setfill('0') << static_cast<int>(datetime->minute)
             << "_"
             << weekDays[datetime->week_day % 7];

    filename << "_" << fileSuffix;

    return filename.str();
}

// void TabSTM32::stopMotorRecording()
// {
//     isMotorRecording = false;
//     qDebug() << "停止记录电机数据";
// }

// void TabSTM32::stopOptimizationRecording()
// {
//     isOptimizationRecording = false;
//     isChannelRecording = false;
//     qDebug() << "停止记录寻优数据和通道信息";
// }
