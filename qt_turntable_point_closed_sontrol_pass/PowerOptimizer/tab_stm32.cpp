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
    connect(transmitter,
            &CommandTransmitter::motorDataReceived,
            this,
            &TabSTM32::onMotorDataReceived);

    connect(transmitter,
            &CommandTransmitter::channelDataReceived,
            this,
            &TabSTM32::onChannelDataReceived);

    connect(transmitter,
            &CommandTransmitter::optResDataReceived,
            this,
            &TabSTM32::onOptResDataReceived);

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
 * @brief 接收电机数据并更新界面与图表
 * @param data 解析后的电机数据结构体
 */
void TabSTM32::onMotorDataReceived(const MotorData_t &data)
{
    // 更新电机状态读取区域
    mw->ui->lineEdit_read_motor_x->setText(QString::number(data.motorX, 'f', 2));
    mw->ui->lineEdit_read_motor_y->setText(QString::number(data.motorY, 'f', 2));
    mw->ui->lineEdit_read_motor_speed->setText(QString::number(data.motorSpeed));

    // 更新图表
    updateMotorChart(data.motorX, data.motorY);

    QString message = QString("收到电机数据 - X: %1, Y: %2, 速度: %3")
                          .arg(data.motorX).arg(data.motorY).arg(data.motorSpeed);
    qDebug()<<message;
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
 * @brief 接收并更新当前通道、电压与功率信息
 */
void TabSTM32::onChannelDataReceived(const CurrentVPCh_t &data)
{
    // 更新寻优过程数据显示区域
    mw->ui->lineEdit_read_currentchannel->setText(QString::number(data.currentChannel));
    mw->ui->lineEdit_read_currentvoltage->setText(QString::number(data.currentV, 'f', 2));
    mw->ui->lineEdit_read_currentpower->setText(QString::number(data.currentP, 'f', 2));

    QString message = QString("收到通道信息 - 通道: %1, 电压: %2V, 功率: %3W")
                          .arg(data.currentChannel).arg(data.currentV).arg(data.currentP);

    qDebug()<<message;
}

/**
 * @brief 接收寻优结果并更新显示区域
 */
void TabSTM32::onOptResDataReceived(const OptResData_t &data)
{
    // 更新寻优结果显示区域
    mw->ui->lineEdit_read_channel1voltagemax->setText(QString::number(data.optimalVs[0], 'f', 2));
    mw->ui->lineEdit_read_channel2voltagemax->setText(QString::number(data.optimalVs[1], 'f', 2));
    mw->ui->lineEdit_read_channel3voltagemax->setText(QString::number(data.optimalVs[2], 'f', 2));
    mw->ui->lineEdit_read_channel4voltagemax->setText(QString::number(data.optimalVs[3], 'f', 2));
    mw->ui->lineEdit_read_powermax->setText(QString::number(data.optimalPower, 'f', 2));

    QString message = QString("收到优化结果 - 最大功率: %1W, 电压: [%2, %3, %4, %5]V")
                          .arg(data.optimalPower)
                          .arg(data.optimalVs[0]).arg(data.optimalVs[1])
                          .arg(data.optimalVs[2]).arg(data.optimalVs[3]);
    qDebug()<<message;
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
    if (transmitter == nullptr || transmitter->m_clientSocket == nullptr) {
        QMessageBox::warning(mw, "错误", "请先启动服务器并等待 STM32 连接");
        return;
    }
    // 获取电机控制参数并更新到commandTransmitter
    MotorCmd_t motorCmd;
    motorCmd.x = mw->ui->lineEdit_motor_x->text().toFloat();
    motorCmd.y = mw->ui->lineEdit_motor_y->text().toFloat();
    motorCmd.speed = mw->ui->lineEdit_motor_speed->text().toUShort();

    // 更新到commandTransmitter
    transmitter->setMotorCmd(motorCmd);

    int result = transmitter->send_motor_command();
    if (result == 0) {
        qDebug() << "电机控制命令发送成功: X=" << motorCmd.x << ", Y=" << motorCmd.y << ", Speed=" << motorCmd.speed;
    } else {
        qDebug() << "电机控制命令发送失败，错误码:" <<result ;
    }
}
/**
 * @brief “寻优控制”按钮点击事件。根据用户选择的轨迹类型读取参数并发送寻优控制命令。
 */
void TabSTM32::on_pushButton_find_optimal_clicked()
{
    if (transmitter == nullptr || transmitter->m_clientSocket == nullptr) {
        QMessageBox::warning(mw, "错误", "请先启动服务器并等待 STM32 连接");
        return;
    }
    // 获取轨迹类型
    int trajTypeIndex = mw->ui->comboBox_traj_type->currentIndex();
    ThajType_t whichThaj = (trajTypeIndex == 0) ? SQU_TRAJ : CIR_TRAJ;//只有两个index的情况下

    // 根据轨迹类型获取正确的参数
    float cirTrajRad = 0.0f;
    float squThajLen = 0.0f;  // 新增：方形轨迹边长
    uint8_t squThajStepLen = 0;

    float maxVol = mw->ui->lineEdit_max_voltage->text().toFloat();
    float initialVol = mw->ui->lineEdit_initial_voltage->text().toFloat();
    float volStepLen = mw->ui->lineEdit_voltage_step->text().toFloat();


    if (whichThaj == SQU_TRAJ) {
        // 方形轨迹：只使用方形步长，圆形半径设为0
        squThajLen = mw->ui->lineEdit_square_step_2->text().toFloat(); // 获取方形边长
        squThajStepLen = mw->ui->lineEdit_square_step->text().toUShort();
        cirTrajRad = 0.0f;

        // 确保UI状态正确（方形步长可用，圆形半径禁用）
        mw->ui->lineEdit_square_step_2->setEnabled(true);
        mw->ui->lineEdit_square_step->setEnabled(true);
        mw->ui->lineEdit_circle_radius->setEnabled(false);

        // 验证方形轨迹参数
        if (squThajLen <= 0) {
            QMessageBox::warning(mw, "参数错误", "方形轨迹必须设置有效的方形边长");
            return;
        }
        if (squThajStepLen == 0) {
            QMessageBox::warning(mw, "参数错误", "方形轨迹必须设置方形步长");
            return;
        }

    } else {
        // 圆形轨迹：只使用圆形半径，方形参数设为0
        cirTrajRad = mw->ui->lineEdit_circle_radius->text().toFloat();
        squThajLen = 0.0f;
        squThajStepLen = 0;

        // 确保UI状态正确（圆形半径可用，方形参数禁用）
        mw->ui->lineEdit_circle_radius->setEnabled(true);
        mw->ui->lineEdit_square_step_2->setEnabled(false);
        mw->ui->lineEdit_square_step->setEnabled(false);
        // 验证圆形轨迹参数
        if (cirTrajRad <= 0) {
            QMessageBox::warning(mw, "参数错误", "圆形轨迹必须设置有效的圆形半径");
            return;
        }
    }

    if (maxVol <= 0 || volStepLen <= 0) {
        QMessageBox::warning(mw, "参数错误", "最大电压和电压步长必须大于0");
        return;
    }
    // 发送时间同步命令
    int timeResult = transmitter->send_time_command();
    if (timeResult == 0) {
        qDebug()<<"时间命令发送成功";
    } else {
        qDebug() << "时间命令发送失败，错误码:" << timeResult;
        // 即使时间命令失败，也继续发送寻优命令
    }
    // 发送寻优任务命令
    const int result = transmitter->send_find_opt_command(whichThaj, cirTrajRad, squThajLen, squThajStepLen, maxVol, volStepLen,initialVol);
    if (result == 0) {
        qDebug()<<"寻优控制命令发送成功";

        // 更新commandTransmitter中的配置
        FindOptimalCmd_t findOptCmd = transmitter->getFindOptCmd();
        findOptCmd.whichThaj = whichThaj;
        findOptCmd.cirTrajRad = cirTrajRad;
        findOptCmd.squThajLen = squThajLen;
        findOptCmd.squThajStepLen = squThajStepLen;
        findOptCmd.maxVol = maxVol;
        findOptCmd.volStepLen = volStepLen;
        findOptCmd.initialVol = initialVol;
        transmitter->setFindOptCmd(findOptCmd);

        // 显示发送的参数信息
        QString paramInfo;
        if (whichThaj == SQU_TRAJ) {
            paramInfo = QString("轨迹类型: 方形轨迹, 方形边长: %1mm, 方形步长: %2mm, 最大电压: %3V, 电压步长: %4V, 初始电压: %3V")
                            .arg(squThajLen).arg(squThajStepLen).arg(maxVol).arg(volStepLen).arg(initialVol);
        } else {
            paramInfo = QString("轨迹类型: 圆形轨迹, 圆形半径: %1m, 最大电压: %2V, 电压步长: %3V, 初始电压: %2V")
                            .arg(cirTrajRad).arg(maxVol).arg(volStepLen).arg(initialVol);
        }
        qDebug()<<paramInfo;
    } else {
        qDebug() << "寻优控制命令发送失败，错误码:" << result;
    }
}




