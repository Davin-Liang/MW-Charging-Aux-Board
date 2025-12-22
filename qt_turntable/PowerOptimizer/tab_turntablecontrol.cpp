#include "tab_turntablecontrol.h"
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "turntable_controller.h"
#include "PID_Controller.h"
#include "command_transmitter.h" 
#include "TrajectoryJsonDialog.h"
#include <QMessageBox>
#include <QDebug>
#include <QTimer>
#include <QtCharts>
#include <QFileDialog>
#include <QJsonDocument>

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
    , useTrajectoryX(false)
    , useTrajectoryY(false)
{
    // 不 new 主资源；只引用 MainWindow 持有的对象
    if (mw) {
        pidX = mw->pid_x;
        pidY = mw->pid_y;
        monitorTimer = mw->turntableMonitorTimer;
        closedLoopTimerX = mw->closedLoopTimer_x;
        closedLoopTimerY = mw->closedLoopTimer_y;
    } else {
    
        pidX = pidY = nullptr;
        monitorTimer = closedLoopTimerX = closedLoopTimerY = nullptr;
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

    // 跟踪轨迹类型
    mw->ui->combo_ref_mode->addItem("X点位 / Y点位");
    mw->ui->combo_ref_mode->addItem("X轨迹 / Y轨迹");
    mw->ui->combo_ref_mode->addItem("X点位 / Y轨迹");
    mw->ui->combo_ref_mode->addItem("X轨迹 / Y点位");

    //连接参考轨迹源
    connect(mw->ui->combo_ref_mode, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,&TabTurntableControl::on_ref_mode_changed,Qt::UniqueConnection);
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

    connect(mw->ui->btn_set_x_pidcontroller_parameter, &QPushButton::clicked,
            this, &TabTurntableControl::on_btn_set_x_pidcontroller_parameter_clicked, Qt::UniqueConnection);

    connect(mw->ui->btn_stop_x_pidcontrol, &QPushButton::clicked,
            this, &TabTurntableControl::on_btn_stop_x_pidcontrol_clicked, Qt::UniqueConnection);

    connect(mw->ui->btn_set_y_pidcontroller_parameter, &QPushButton::clicked,
            this, &TabTurntableControl::on_btn_set_y_pidcontroller_parameter_clicked, Qt::UniqueConnection);

    connect(mw->ui->btn_stop_y_pidcontrol, &QPushButton::clicked,
            this, &TabTurntableControl::on_btn_stop_y_pidcontrol_clicked, Qt::UniqueConnection);
            
    // 将监控定时器连接到本类的 updateTurntableData（如果存在）
    if (monitorTimer) {
        connect(monitorTimer, &QTimer::timeout,
                this, &TabTurntableControl::updateTurntableData, Qt::UniqueConnection);
    }

    // 将闭环定时器连接到 closedLoopTick  /转台定时器（闭环控制）
    if (closedLoopTimerX) {
        connect(closedLoopTimerX, &QTimer::timeout,
                this, &TabTurntableControl::closedLoopTickX, Qt::UniqueConnection);
    }
    if (closedLoopTimerY) {
        connect(closedLoopTimerY, &QTimer::timeout,
                this, &TabTurntableControl::closedLoopTickY, Qt::UniqueConnection);
    }
    // controller selection change
    connect(mw->ui->controller_selection,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &TabTurntableControl::on_controller_selection_changed, Qt::UniqueConnection);
    // 数据监控开关（QCheckBox）
    connect(mw->ui->data_monitor_section,   &QCheckBox::stateChanged,this,&TabTurntableControl::on_data_monitor_section_stateChanged, Qt::UniqueConnection);

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
 * @brief 设置闭环控制的参考轨迹源
 */
void TabTurntableControl::on_ref_mode_changed(int index)
{
    int mode = mw->ui->combo_ref_mode->currentIndex();

    useTrajectoryX = (mode == 1 || mode == 3);
    useTrajectoryY = (mode == 1 || mode == 2);

    mw->ui->line_edit_x_pos_ref->setEnabled(!useTrajectoryX);
    mw->ui->line_edit_y_pos_ref->setEnabled(!useTrajectoryY);
}
/**
 * @brief 闭环参考轨迹配置弹出模板 JSON
 */
QString TabTurntableControl::defaultTrajectoryTemplate(const QString &axisName)
{
    return QString(R"({
  "_comment": "这是 %1 轴的参考轨迹配置，请只修改数值，不要删除字段",

  "time": {
    "dt": 0.05,
    "duration": 60.0
  },

  "%1": {
    "type": "sine",        // sine | circle
    "offset": 0.0,
    "amplitude": 10.0,
    "frequency": 0.2,
    "phase": 0.0
  }
})").arg(axisName);
}
/**
 * @brief 闭环参考轨迹配置用户编辑后解析
 */
bool TabTurntableControl::validateTrajectoryJson(
    const QJsonObject &obj,
    const QString &axisName,
    QString &errorMsg)
{
    if (!obj.contains("time") || !obj["time"].isObject()) {
        errorMsg = "缺少 time 配置";
        return false;
    }

    auto timeObj = obj["time"].toObject();
    if (!timeObj.contains("dt") || !timeObj.contains("duration")) {
        errorMsg = "time 中必须包含 dt 和 duration";
        return false;
    }

    if (!obj.contains(axisName) || !obj[axisName].isObject()) {
        errorMsg = QString("缺少 %1 轴轨迹配置").arg(axisName);
        return false;
    }

    auto axisObj = obj[axisName].toObject();
    if (!axisObj.contains("type")) {
        errorMsg = "轨迹必须包含 type 字段";
        return false;
    }

    QString type = axisObj["type"].toString();
    if (type != "sine" && type != "circle") {
        errorMsg = "type 仅支持 sine 或 circle";
        return false;
    }

    return true;
}
/**
 * @brief 闭环参考轨迹配置（弹出模板 JSON，用户编辑后解析）
 */
bool TabTurntableControl::loadTrajectoryJson(
    QJsonObject &cfg,
    const QString &axisName)
{
        while (true) {
            QString jsonTemplate = defaultTrajectoryTemplate(axisName);

            TrajectoryJsonDialog dlg(jsonTemplate, mw);

            if (dlg.exec() != QDialog::Accepted) {
                return false;   // 用户取消
            }

            QByteArray data = dlg.jsonText().toUtf8();

            QJsonParseError err;
            auto doc = QJsonDocument::fromJson(data, &err);

            if (err.error != QJsonParseError::NoError || !doc.isObject()) {
                QMessageBox::warning(
                    mw, "JSON错误", "JSON 格式错误，请重新填写");
                continue;
            }

            QString errorMsg;
            if (!validateTrajectoryJson(doc.object(), axisName, errorMsg)) {
                QMessageBox::warning(
                    mw, "配置错误", errorMsg);
                continue;
            }

            cfg = doc.object();

            QMessageBox::information(
                mw, "成功",
                QString("%1 轴参考轨迹设置成功").arg(axisName));

            return true;
        }
}
/**
 * @brief 设置闭环控制的参考点
 */
void TabTurntableControl::on_btn_set_target_pos_clicked()
{
    // ===== X 轴 =====
    if (!useTrajectoryX) {
        target_x = mw->ui->line_edit_x_pos_ref->text().toDouble();
        QMessageBox::information(
            mw, "X轴点位参考设置成功",
            QString("X=%1").arg(target_x));
    } else {
        if (!loadTrajectoryJson(trajConfigX, "yaw")) {
            QMessageBox::warning(mw, "错误", "X轴轨迹配置加载失败");
            return;
        }
    }
    // ===== Y 轴 =====
    if (!useTrajectoryY) {
        target_y = mw->ui->line_edit_y_pos_ref->text().toDouble();
        QMessageBox::information(
            mw, "Y轴点位参考设置成功",
            QString("Y=%1").arg(target_y));
    } else {
        if (!loadTrajectoryJson(trajConfigY, "pitch")) {
            QMessageBox::warning(mw, "错误", "Y轴轨迹配置加载失败");
            return;
        }
    }

    traj_time = 0.0;   // 初始化轨迹时间
    //QMessageBox::information(mw, "轨迹参考", "轨迹配置加载成功");

}
/**
 * @brief 真正驱动轨迹
 */
void TabTurntableControl::updateTrajectoryTarget()
{
    if (useTrajectoryX) {
        double A = trajConfigX["A"].toDouble(10.0);
        double w = trajConfigX["w"].toDouble(1.0);
        double bias = trajConfigX["bias"].toDouble(0.0);

        target_x = bias + A * std::sin(w * traj_time);
    }

    if (useTrajectoryY) {
        double A = trajConfigY["A"].toDouble(5.0);
        double w = trajConfigY["w"].toDouble(0.5);
        double bias = trajConfigY["bias"].toDouble(0.0);

        target_y = bias + A * std::cos(w * traj_time);
    }

    traj_time += 0.05; // 与定时器周期一致
}

/**
 * @brief 设置 X轴 PID 参数并开启X轴闭环控制
 */

void TabTurntableControl::on_btn_set_x_pidcontroller_parameter_clicked()
{
    if (!pidX ) {
        QMessageBox::warning(mw, "错误", "PID 控制器未配置");
        return;
    }
    if (mw->ui->controller_selection->currentText() != "PID 控制器") {
        QMessageBox::warning(mw, "错误", "请选择 PID 控制器");
        return;
    }
    PIDController::Gains g;
    g.kp = mw->ui->line_edit_kp_x_parameter->text().toDouble();
    g.ki = mw->ui->line_edit_ki_x_parameter->text().toDouble();
    g.kd = mw->ui->line_edit_kd_x_parameter->text().toDouble();

    pidX->setGains(g);
    pidX->reset();
    pidX->setController(mw->turntable_controller);
 
    if (closedLoopTimerX) {
        closedLoopTimerX->start(50);
        QMessageBox::information(mw, "PID 启动", "X轴PID 参数已设置，X轴闭环控制开始");
        mw->ui->control_status->setText("X轴闭环控制：开启");
        mw->ui->control_status->setStyleSheet("color: green;");
    } else {
        QMessageBox::warning(mw, "警告", "X轴闭环定时器不可用");
    }
}
/**
 * @brief 设置 y轴 PID 参数并开启y轴闭环控制
 */

 void TabTurntableControl::on_btn_set_y_pidcontroller_parameter_clicked()
 {
     if (!pidY ) {
         QMessageBox::warning(mw, "错误", "PID 控制器未配置");
         return;
     }
     if (mw->ui->controller_selection->currentText() != "PID 控制器") {
         QMessageBox::warning(mw, "错误", "请选择 PID 控制器");
         return;
     }
     PIDController::Gains g;
     g.kp = mw->ui->line_edit_kp_y_parameter->text().toDouble();
     g.ki = mw->ui->line_edit_ki_y_parameter->text().toDouble();
     g.kd = mw->ui->line_edit_kd_y_parameter->text().toDouble(); 
 
     pidY->setGains(g);
     pidY->reset();
     pidY->setController(mw->turntable_controller);
  
     if (closedLoopTimerY) {
        closedLoopTimerY->start(50);
         QMessageBox::information(mw, "PID 启动", "y轴PID 参数已设置，y轴闭环控制开始");
         mw->ui->control_status->setText("y轴闭环控制：开启");
         mw->ui->control_status->setStyleSheet("color: green;");
     } else {
         QMessageBox::warning(mw, "警告", "y轴闭环定时器不可用");
     }
 }
/**
 * @brief x轴PID闭环停止
 */

void TabTurntableControl::on_btn_stop_x_pidcontrol_clicked()
{
    if (closedLoopTimerX) closedLoopTimerX->stop();
    mw->ui->control_status->setText("X轴闭环控制：停止");
    mw->ui->control_status->setStyleSheet("color: red;");
}
/**
 * @brief y轴PID闭环停止
 */

 void TabTurntableControl::on_btn_stop_y_pidcontrol_clicked()
 {
     if (closedLoopTimerY) closedLoopTimerY->stop();
     mw->ui->control_status->setText("y轴闭环控制：停止");
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
    if (useTrajectoryX || useTrajectoryY) {
        updateTrajectoryTarget();
    }
}

/**
 * @brief 闭环x轴 PID 控制定时器回调（周期性执行）
 */
void TabTurntableControl::closedLoopTickX()
{
    if (!pidX ) return;
    // 使用 pid 控制器的 controlLoop 接口（假定定义类似于你的实现）
    bool doneX = pidX->controlLoop(target_x, 0.01, 0.05);

    if (!useTrajectoryX  && doneX && closedLoopTimerX) {
        closedLoopTimerX->stop();
        mw->ui->control_status->setText("X轴闭环控制：完成");
        mw->ui->control_status->setStyleSheet("color: blue;");
        QMessageBox::information(mw, "完成", "转台已到达目标点（误差 ≤ 0.01）");
    }
    if (useTrajectoryX && doneX && closedLoopTimerX){
        mw->ui->control_status->setText("X轴闭环控制：完成");
        mw->ui->control_status->setStyleSheet("color: blue;");
        QMessageBox::information(mw, "完成", "转台已到达目标轨迹（误差 ≤ 0.01）");
    }
}
/**
 * @brief 闭环 y轴PID 控制定时器回调（周期性执行）
 */
void TabTurntableControl::closedLoopTickY()
{

    if (!pidY ) return;
    // 使用 pid 控制器的 controlLoop 接口（假定定义类似于你的实现）
    bool doneY = pidY->controlLoop(target_y, 0.01, 0.05);
    if ( !useTrajectoryY  && closedLoopTimerY&& doneY) {
        closedLoopTimerY->stop();
        mw->ui->control_status->setText("y轴闭环控制：完成");
        mw->ui->control_status->setStyleSheet("color: blue;");
        QMessageBox::information(mw, "完成", "转台已到达目标点（误差 ≤ 0.01）");
    }
    if (useTrajectoryY  && doneY && closedLoopTimerY){
        mw->ui->control_status->setText("Y轴闭环控制：完成");
        mw->ui->control_status->setStyleSheet("color: blue;");
        QMessageBox::information(mw, "完成", "转台已到达目标轨迹（误差 ≤ 0.01）");
    }
}

void TabTurntableControl::on_data_monitor_section_stateChanged(int state)
{
    if (!monitorTimer) {
        QMessageBox::warning(mw, "错误", "监控定时器不可用");
        return;
    }

    if (state == Qt::Checked) {
        //  开始数据监控
        monitorTimer->start(50);   // 200 ms，根据你之前 chart_time += 0.5 来看是合理的
        mw->ui->control_status->setText("数据监控：开启");
        mw->ui->control_status->setStyleSheet("color: green;");
    } else {
        //  停止数据监控
        monitorTimer->stop();
        mw->ui->control_status->setText("数据监控：关闭");
        mw->ui->control_status->setStyleSheet("color: gray;");
    }
}
