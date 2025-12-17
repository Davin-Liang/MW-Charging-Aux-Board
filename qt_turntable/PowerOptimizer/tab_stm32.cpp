#include "tab_stm32.h"
#include "mainwindow.h"
#include "./ui_mainwindow.h"


#include <QMessageBox>
#include <QVBoxLayout>
#include <QDebug>
#include <QtCharts/QValueAxis>

TabSTM32::TabSTM32(MainWindow *mw_)
    : QObject(mw_), mw(mw_)
{
    transmitter = mw->commandTransmitter;
    stm32_MonitorTimer = mw.stm32MonitorTimer;
}

TabSTM32::~TabSTM32()
{
        // 主图表视图资源显式释放（其父对象未托管）
        delete chartView;
        chartView = nullptr;
    // 不 delete transmitter（由 MainWindow 管理）
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
            this, &TabStm32::onStm32MonitorTimeout);

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
 * @brief 初始化 UI 中的电机参数与寻优参数
 */
void TabSTM32::initializeUIWithConfig()
{
    if (!transmitter) return;

    // 获取电机配置
    MotorCmd_t motorCmd = transmitter->getMotorCmd();
    mw->ui->lineEdit_motor_x->setText(QString::number(motorCmd.x, 'f', 2));
    mw->ui->lineEdit_motor_y->setText(QString::number(motorCmd.y, 'f', 2));
    mw->ui->lineEdit_motor_speed->setText(QString::number(motorCmd.speed));

    // 获取寻优配置
    FindOptimalCmd_t findOptCmd = transmitter->getFindOptCmd();

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
    motorChart = new QtCharts::QChart();
    motorChart->setTitle("");  // 不显示标题
    motorChart->setAnimationOptions(QtCharts::QChart::NoAnimation);
    motorChart->legend()->hide();
    motorChart->setMargins(QMargins(0, 0, 0, 0)); // 去除图表边距
    motorChart->setBackgroundRoundness(0);   // 设置图表边距为0，使其完全填充

    // 创建轨迹曲线（蓝色线条）
    motorTrajectorySeries = new QtCharts::QLineSeries();
    // 移除名称设置，不显示图例
    motorTrajectorySeries->setColor(Qt::blue);
    motorTrajectorySeries->setPointsVisible(false);  // 不显示轨迹上的点，只显示线条

    // 创建当前位置点（红色散点）
    currentPositionSeries = new QtCharts::QScatterSeries();
    // 移除名称设置，不显示图例
    currentPositionSeries->setColor(Qt::red);
    currentPositionSeries->setBorderColor(Qt::darkRed);
    currentPositionSeries->setMarkerSize(4);

    // 添加到图表
    motorChart->addSeries(motorTrajectorySeries);
    motorChart->addSeries(currentPositionSeries);

    // 坐标轴 X
    axisX = new QtCharts::QValueAxis();
    axisX->setTitleText("x (mm)");
    axisX->setTitleFont(QFont("宋体", 8));
    axisX->setLabelsFont(QFont("宋体", 8));
    axisX->setRange(-50, 50);  // 初始范围
    axisX->setTickCount(11);

    // 坐标轴 Y
    axisY = new QtCharts::QValueAxis();
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
    chartView = new QtCharts::QChartView(motorChart);
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
    // ===== 1. 写电机参数（保持寄存器）=====
    writeMotorRegisters();

    // ===== 2. 打开电机控制线圈 =====
    modbus_write_bit(mw->stm32_mb_ctx, 0x0002, 1);

    // ===== 3. 写时间 =====
    writeTimeRegisters();

    // ===== 4. 更新标志位：bit0(电机) + bit2(时间) =====
    setUpdateFlag((1 << 0) | (1 << 2));

    qDebug() << "电机控制参数已发送";
}
/**
 * @brief 写电机参数（保持寄存器）
 */
void TabSTM32::writeMotorRegisters()
{
    int16_t regs[3];

    regs[0] = static_cast<int16_t>(mw->ui->lineEdit_motor_x->text().toInt());
    regs[1] = static_cast<int16_t>(mw->ui->lineEdit_motor_y->text().toInt());
    regs[2] = static_cast<uint16_t>(mw->ui->lineEdit_motor_speed->text().toUInt());

    modbus_write_registers(mw->stm32_mb_ctx,0x0001,3,reinterpret_cast<uint16_t*>(regs));
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
    // ===== 1. 写寻优参数 =====
    writeFindOptRegisters();

    // ===== 2. 打开寻优控制线圈 =====
    modbus_write_bit(mw->stm32_mb_ctx, 0x0001, 1);

    // ===== 3. 写时间 =====
    writeTimeRegisters();

    // ===== 4. 更新标志位：bit1(寻优) + bit2(时间) =====
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

    // float 参数
    float v;
    uint16_t fbuf[2];

    v = mw->ui->lineEdit_max_voltage->text().toFloat();
    modbus_set_float_abcd(v, fbuf);
    modbus_write_registers(mw->stm32_mb_ctx, 0x0008, 2, fbuf);

    v = mw->ui->lineEdit_voltage_step->text().toFloat();
    modbus_set_float_abcd(v, fbuf);
    modbus_write_registers(mw->stm32_mb_ctx, 0x000A, 2, fbuf);

    v = mw->ui->lineEdit_initial_voltage->text().toFloat();
    modbus_set_float_abcd(v, fbuf);
    modbus_write_registers(mw->stm32_mb_ctx, 0x000C, 2, fbuf);
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
void TabStm32::onStm32MonitorTimeout()
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
    uint8_t channel = regs[3] & 0xFF;     // 0x0004


    float voltage = modbus_get_float_abcd(&regs[4]); // 0x0005
    float power   = modbus_get_float_abcd(&regs[5]); // 0x0007

    int16_t opt_x = (int16_t)regs[6];      // 0x0009
    int16_t opt_y = (int16_t)regs[7];      // 0x000A
    float opt_pwr = modbus_get_float_abcd(&regs[8]); // 0x000B

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

    mw->ui->lineEdit_read_channel1voltagemax->setText(QString::number(modbus_get_float_abcd(&regs[9]), 'f', 2));
    mw->ui->lineEdit_read_channel2voltagemax->setText(QString::number(modbus_get_float_abcd(&regs[10]), 'f', 2));
    mw->ui->lineEdit_read_channel3voltagemax->setText(QString::number(modbus_get_float_abcd(&regs[11]), 'f', 2));
    mw->ui->lineEdit_read_channel4voltagemax->setText(QString::number(modbus_get_float_abcd(&regs[12]), 'f', 2));


    QString message = QString("收到电机数据 - X: %1, Y: %2, 速度: %3")
                          .arg(motor_x).arg(motor_y).arg(speed);
    QString message = QString("收到通道信息 - 通道: %1, 电压: %2V, 功率: %3W")
                          .arg(channel).arg(voltage).arg(power);
    QString message = QString("收到优化结果 - 最大功率: %1W, 电压: [%2, %3, %4, %5]V")
                          .arg(opt_pwr)
                          .arg(modbus_get_float_abcd(&regs[9])).arg(modbus_get_float_abcd(&regs[10]))
                          .arg(modbus_get_float_abcd(&regs[11])).arg(modbus_get_float_abcd(&regs[12]));
    qDebug()<<message;
}


