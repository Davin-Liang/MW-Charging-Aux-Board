#include "tab_turntablecontrol.h"
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "turntable_controller.h"
#include "PID_Controller.h"
#include "command_transmitter.h" 
#include <QMessageBox>
#include <QDebug>
#include <QTimer>
#include <QtCharts>

using namespace QtCharts;

TabTurntableControl::TabTurntableControl(MainWindow *mw_)
    : QObject(mw_), mw(mw_)
    , turntableChart(nullptr)
    , turntableChartView(nullptr)
    , series_target_x(nullptr)
    , series_target_y(nullptr)
    , series_current_x(nullptr)
    , series_current_y(nullptr)
    , chart_time(0.0)
    , target_x(0.0)
    , target_y(0.0)
{
    // 不 new 主资源；只引用 MainWindow 持有的对象
    if (mw) {
        pidX = mw->pid_x;
        pidY = mw->pid_y;
        monitorTimer = mw->turntableMonitorTimer;
        closedLoopTimer = mw->closedLoopTimer;
    } else {
    
        pidX = pidY = nullptr;
        monitorTimer = closedLoopTimer = nullptr;
    }
}

TabTurntableControl::~TabTurntableControl()
{
}

void TabTurntableControl::setupConnections()
{
    if (!mw) return;
    // 转台控制器选择
    mw->ui->controller_selection->addItem("PID 控制器");
    mw->ui->controller_selection->setCurrentIndex(0);

    mw->ui->combo_box_x_speed_cmd->addItem("正转");
    mw->ui->combo_box_x_speed_cmd->addItem("反转");
    mw->ui->combo_box_x_speed_cmd->setCurrentIndex(0);

    mw->ui->combo_box_y_speed_cmd->addItem("正转");
    mw->ui->combo_box_y_speed_cmd->addItem("反转");
    mw->ui->combo_box_y_speed_cmd->setCurrentIndex(0);
    // 按钮连接到本类槽（使用 UniqueConnection 防止重复连接）
    connect(mw->ui->btn_set_speed, &QPushButton::clicked,
            this, &TabTurntableControl::on_btn_set_speed_clicked, Qt::UniqueConnection);

    connect(mw->ui->btn_stop_x_turntable_run, &QPushButton::clicked,
            this, &TabTurntableControl::on_btn_stop_x_turntable_run_clicked, Qt::UniqueConnection);

    connect(mw->ui->btn_stop_y_turntable_run, &QPushButton::clicked,
            this, &TabTurntableControl::on_btn_stop_y_turntable_run_clicked, Qt::UniqueConnection);

    connect(mw->ui->btn_set_x_pos, &QPushButton::clicked,
            this, &TabTurntableControl::on_btn_set_x_pos_clicked, Qt::UniqueConnection);

    connect(mw->ui->btn_set_y_pos, &QPushButton::clicked,
            this, &TabTurntableControl::on_btn_set_y_pos_clicked, Qt::UniqueConnection);

    connect(mw->ui->btn_x_zero, &QPushButton::clicked,
            this, &TabTurntableControl::on_btn_x_zero_clicked, Qt::UniqueConnection);

    connect(mw->ui->btn_y_zero, &QPushButton::clicked,
            this, &TabTurntableControl::on_btn_y_zero_clicked, Qt::UniqueConnection);

    connect(mw->ui->btn_set_target_pos, &QPushButton::clicked,
            this, &TabTurntableControl::on_btn_set_target_pos_clicked, Qt::UniqueConnection);

    connect(mw->ui->btn_set_pidcontroller_parameter, &QPushButton::clicked,
            this, &TabTurntableControl::on_btn_set_pidcontroller_parameter_clicked, Qt::UniqueConnection);

    connect(mw->ui->btn_stop_pidcontrol, &QPushButton::clicked,
            this, &TabTurntableControl::on_btn_stop_pidcontrol_clicked, Qt::UniqueConnection);

    // 将监控定时器连接到本类的 updateTurntableData（如果存在）
    if (monitorTimer) {
        connect(monitorTimer, &QTimer::timeout,
                this, &TabTurntableControl::updateTurntableData, Qt::UniqueConnection);
    }

    // 将闭环定时器连接到 closedLoopTick  /转台定时器（闭环控制）
    if (closedLoopTimer) {
        connect(closedLoopTimer, &QTimer::timeout,
                this, &TabTurntableControl::closedLoopTick, Qt::UniqueConnection);
    }

    // controller selection change
    connect(mw->ui->controller_selection,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &TabTurntableControl::on_controller_selection_changed, Qt::UniqueConnection);
    // 初始化转台图像与状态
    update_turntable_image();
    //初始化转台位置跟踪控制图
    initializeTurntablePositionChart();
}
/**
 * @brief 加载并显示转台图片
 */
void TabTurntableControl::update_turntable_image()
{
    QString imgPath = "./image/turntable.png";
    QPixmap pix(imgPath);
    // 如果图片加载失败，可显示默认文字提示
    if (pix.isNull()) {
        mw->ui->label_turntable_img->setText("转台图片未找到");
        mw->ui->label_turntable_img->setAlignment(Qt::AlignCenter);
        return;
    }

    mw->ui->label_turntable_img->setPixmap(
        pix.scaled(mw->ui->label_turntable_img->size(),
                   Qt::KeepAspectRatio,
                   Qt::SmoothTransformation));
}
/**
 * @brief 初始化转台位置实时曲线图表
 */
void TabTurntableControl::initializeTurntablePositionChart()
{

    turntableChart = new QChart();
    // turntableChart->setTitle("转台实时位置跟踪");

    // 创建四条曲线
    series_target_x = new QLineSeries();
    series_target_y = new QLineSeries();
    series_current_x = new QLineSeries();
    series_current_y = new QLineSeries();

    series_target_x->setName("target_x");
    series_target_y->setName("target_y");
    series_current_x->setName("current_x");
    series_current_y->setName("current_y");

    turntableChart->addSeries(series_target_x);
    turntableChart->addSeries(series_target_y);
    turntableChart->addSeries(series_current_x);
    turntableChart->addSeries(series_current_y);

    // 坐标轴
    QValueAxis *axisX = new QValueAxis();
    QValueAxis *axisY = new QValueAxis();

    axisX->setTitleText("时间 (s)");
    axisY->setTitleText("角度 (°)");

    axisX->setRange(0, 10);  // 显示最近 10 秒
    axisY->setRange(-180, 180);

    turntableChart->addAxis(axisX, Qt::AlignBottom);
    turntableChart->addAxis(axisY, Qt::AlignLeft);

    // 绑定轴
    series_target_x->attachAxis(axisX);
    series_target_x->attachAxis(axisY);

    series_target_y->attachAxis(axisX);
    series_target_y->attachAxis(axisY);

    series_current_x->attachAxis(axisX);
    series_current_x->attachAxis(axisY);

    series_current_y->attachAxis(axisX);
    series_current_y->attachAxis(axisY);

    // ChartView 放入 UI
    turntableChartView = new QChartView(turntableChart, mw->ui->turntable_position_chart);
    turntableChartView->setRenderHint(QPainter::Antialiasing);

    // 将 turntable_position_chart（QWidget）替换为 ChartView
    // QVBoxLayout *layout = new QVBoxLayout(ui->turntable_position_chart);
    // layout->addWidget(turntableChartView);
    auto *layout = new QVBoxLayout(mw->ui->turntable_position_chart);
    layout->addWidget(turntableChartView);
    layout->setContentsMargins(0, 0, 0, 0);

}

/**
 * @brief 设置转台两个轴的速度与方向。
 */
void TabTurntableControl::on_btn_set_speed_clicked()
{
    if (!mw->turntable_controller) {
        QMessageBox::warning(mw, "错误", "未连接转台");
        return;
    }

    float x_speed = mw->ui->line_edit_x_speed_cmd->text().toFloat();
    float y_speed = mw->ui->line_edit_y_speed_cmd->text().toFloat();
    QString x_dir = mw->ui->combo_box_x_speed_cmd->currentText();
    QString y_dir = mw->ui->combo_box_y_speed_cmd->currentText();

    mw->turntable_controller->set_axis_speed(Yaw, x_speed);
    mw->turntable_controller->set_manual_rotation(Yaw,(x_dir == "正转") ? Left : Right);

    mw->turntable_controller->set_axis_speed(Pitch, y_speed);
    mw->turntable_controller->set_manual_rotation(Pitch, (y_dir == "正转") ? Left : Right);

}
/**
 * @brief 停止 X 轴手动旋转（Yaw）
 */
void TabTurntableControl::on_btn_stop_x_turntable_run_clicked()
{
    if (!mw->turntable_controller) return;
    mw->turntable_controller->stop_manual_rotation(Yaw);
}

/**
 * @brief 停止 Y 轴手动旋转（Pitch）
 */
void TabTurntableControl::on_btn_stop_y_turntable_run_clicked()
{
    if (!mw->turntable_controller) return;
    mw->turntable_controller->stop_manual_rotation(Pitch);
}
/**
 * @brief 设置 X 轴（Yaw）角度
 */
void TabTurntableControl::on_btn_set_x_pos_clicked()
{
    if (!mw->turntable_controller) {
        QMessageBox::information(mw, "提示", "未连接转台");
        return;
    }
    float x_pos = mw->ui->line_edit_x_pos->text().toFloat();
    mw->turntable_controller->set_axis_angle(Yaw, x_pos);
}

/**
 * @brief 设置 Y 轴（Pitch）角度
 */
void TabTurntableControl::on_btn_set_y_pos_clicked()
{
    if (!mw->turntable_controller) {
        QMessageBox::information(mw, "提示", "未连接转台");
        return;
    }
    float y_pos = mw->ui->line_edit_y_pos->text().toFloat();
    mw->turntable_controller->set_axis_angle(Pitch, y_pos);
}
/**
 * @brief X 轴（Yaw）归零
 */
void TabTurntableControl::on_btn_x_zero_clicked()
{
    if (!mw->turntable_controller) return;
    mw->turntable_controller->reset_axis_coord(Yaw);
}
/**
 * @brief Y 轴（Pitch）归零
 */
void TabTurntableControl::on_btn_y_zero_clicked()
{
    if (!mw->turntable_controller) return;
    mw->turntable_controller->reset_axis_coord(Pitch);
}
/**
 * @brief 控制器选择下拉框变化时触发
 * @param index 选项索引
 */
void TabTurntableControl::on_controller_selection_changed(int index)
{
    Q_UNUSED(index);
    const QString selected =  mw->ui->controller_selection->currentText();

    if (selected == "PID 控制器") {
        mw->ui->control_status->setText("已选择PID控制算法");
        mw->ui->control_status->setStyleSheet("color: green;");
    }

}
/**
 * @brief 设置闭环控制的参考点
 */
void TabTurntableControl::on_btn_set_target_pos_clicked()
{
    // 读取目标并显示
    target_x = mw->ui->line_edit_x_pos_ref->text().toDouble();
    target_y = mw->ui->line_edit_y_pos_ref->text().toDouble();
    QMessageBox::information(mw, "成功",
                             QString("已设置参考坐标：X=%1, Y=%2")
                                 .arg(target_x).arg(target_y));
}
/**
 * @brief 设置 PID 参数并开启闭环控制
 */

void TabTurntableControl::on_btn_set_pidcontroller_parameter_clicked()
{
    if (!pidX || !pidY) {
        QMessageBox::warning(mw, "错误", "PID 控制器未配置");
        return;
    }
    if (mw->ui->controller_selection->currentText() != "PID 控制器") {
        QMessageBox::warning(mw, "错误", "请选择 PID 控制器");
        return;
    }
    PIDController::Gains g;
    g.kp = mw->ui->line_edit_kp_parameter->text().toDouble();
    g.ki = mw->ui->line_edit_ki_parameter->text().toDouble();
    g.kd = mw->ui->line_edit_kd_parameter->text().toDouble();

    pidX->setGains(g);
    pidY->setGains(g);

    pidX->reset();
    pidY->reset();

    pidX->setController(mw->turntable_controller);
    pidY->setController(mw->turntable_controller);

    QMessageBox::information(mw, "PID 启动", "PID 参数已设置，闭环控制开始");
    mw->ui->control_status->setText("闭环控制：开启");
    mw->ui->control_status->setStyleSheet("color: green;");


    if (closedLoopTimer) {
        closedLoopTimer->start(50);
    } else {
        QMessageBox::warning(mw, "警告", "闭环定时器不可用");
    }
}
/**
 * @brief PID闭环停止
 */

void TabTurntableControl::on_btn_stop_pidcontrol_clicked()
{
    if (closedLoopTimer) closedLoopTimer->stop();
    mw->ui->control_status->setText("闭环控制：停止");
    mw->ui->control_status->setStyleSheet("color: red;");
}


/**
 * @brief 更新实时曲线。
 */
void TabTurntableControl::updateTurntableData()
{

    if (!mw->turntable_controller) return;

    float xPos = 0.0f, yPos = 0.0f, xSpeed = 0.0f, ySpeed = 0.0f;

    bool ok1 = mw->turntable_controller->read_axis_angle(Yaw, &xPos);
    bool ok2 = mw->turntable_controller->read_axis_angle(Pitch, &yPos);
    bool ok3 = mw->turntable_controller->read_axis_speed(Yaw, &xSpeed);
    bool ok4 = mw->turntable_controller->read_axis_speed(Pitch, &ySpeed);

    if (ok1 && ok2 && ok3 && ok4) {
        mw->ui->line_edit_monitor_x_pos->setText(QString::number(xPos, 'f', 2));
        mw->ui->line_edit_monitor_y_pos->setText(QString::number(yPos, 'f', 2));
        mw->ui->line_edit_monitor_x_speed->setText(QString::number(xSpeed, 'f', 2));
        mw->ui->line_edit_monitor_y_speed->setText(QString::number(ySpeed, 'f', 2));
    }
    //===================  更新实时曲线 ===================//
    chart_time += 0.5;  // update interval = 200ms

    series_current_x->append(chart_time, xPos);
    series_current_y->append(chart_time, yPos);

    // target_x, target_y 来自 UI 设置
    series_target_x->append(chart_time, target_x);
    series_target_y->append(chart_time, target_y);

    // 维持最多 10 秒数据
    if (chart_time > 10.0) {
        turntableChart->axisX()->setRange(chart_time - 10.0, chart_time);
    }

}

/**
 * @brief 闭环 PID 控制定时器回调（周期性执行）
 */
void TabTurntableControl::closedLoopTick()
{

    if (!pidX || !pidY) return;
    // 使用 pid 控制器的 controlLoop 接口（假定定义类似于你的实现）
    bool doneX = pidX->controlLoop(target_x, 0.01, 0.05);
    bool doneY = pidY->controlLoop(target_y, 0.01, 0.05);

    if (doneX && doneY) {
        if (closedLoopTimer) closedLoopTimer->stop();
        mw->ui->control_status->setText("闭环控制：完成");
        mw->ui->control_status->setStyleSheet("color: blue;");
        QMessageBox::information(mw, "完成", "转台已到达目标点（误差 ≤ 0.01）");
    }
}
