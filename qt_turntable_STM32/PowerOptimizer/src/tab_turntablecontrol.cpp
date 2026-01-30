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


TabTurntableControl::TabTurntableControl(MainWindow *mw_)
    : QObject(mw_), mw(mw_)
    , chartX(nullptr)
    , chartY(nullptr)
    , chartViewX(nullptr)
    , chartViewY(nullptr)
    , seriesX_target(nullptr)
    , seriesY_target(nullptr)
    , seriesX_current(nullptr)
    , seriesY_current(nullptr)
    , chartTimeX(0.0)
    , chartTimeY(0.0)
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
    //设置只读
    mw->ui->line_edit_monitor_x_pos->setReadOnly(true);
    mw->ui->line_edit_monitor_y_pos->setReadOnly(true);
    mw->ui->line_edit_monitor_x_speed->setReadOnly(true);
    mw->ui->line_edit_monitor_y_speed->setReadOnly(true);
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
    initializeTurntablePositionXChart();
    initializeTurntablePositionYChart();
}
/**
 * @brief 加载并显示转台图片
 */
void TabTurntableControl::update_turntable_image()
{
    QString imgPath = "./image/turntable.png";
    QPixmap pix(imgPath);

    if (pix.isNull()) {
        mw->ui->label_turntable_img->setText("转台图片未找到");
        mw->ui->label_turntable_img->setAlignment(Qt::AlignCenter);
        return;
    }

    auto label = mw->ui->label_turntable_img;

    label->setAlignment(Qt::AlignCenter);
    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QTimer::singleShot(0, this, [label, pix]() {
        if (!label->size().isEmpty()) {
            label->setPixmap(
                pix.scaled(
                    label->size(),
                    Qt::KeepAspectRatio,
                    Qt::SmoothTransformation
                )
            );
        }
    });
}

/**
 * @brief 初始化转台X轴位置实时曲线图表
 */
void TabTurntableControl::initializeTurntablePositionXChart()
{
    chartX = new QChart();
    seriesX_current = new QLineSeries();
    seriesX_target  = new QLineSeries();

    seriesX_current->setName("Current X");
    seriesX_target->setName("Target X");

    chartX->addSeries(seriesX_current);
    chartX->addSeries(seriesX_target);

    auto *axisTime = new QValueAxis();
    auto *axisPos  = new QValueAxis();

    axisTime->setTitleText("时间 (s)");
    axisTime->setRange(0, 10);

    axisPos->setTitleText("角度 (°)");
    axisPos->setRange(-180, 180);

    chartX->addAxis(axisTime, Qt::AlignBottom);
    chartX->addAxis(axisPos, Qt::AlignLeft);

    seriesX_current->attachAxis(axisTime);
    seriesX_current->attachAxis(axisPos);
    seriesX_target->attachAxis(axisTime);
    seriesX_target->attachAxis(axisPos);

    chartViewX = new QChartView(chartX);
    chartViewX->setRenderHint(QPainter::Antialiasing);

    auto *layout = new QVBoxLayout(mw->ui->turntable_position_x_chart);
    layout->addWidget(chartViewX);
    layout->setContentsMargins(0, 0, 0, 0);
}
/**
 * @brief 初始化转台y轴位置实时曲线图表
 */
void TabTurntableControl::initializeTurntablePositionYChart()
{
    chartY = new QChart();

    seriesY_current = new QLineSeries();
    seriesY_target  = new QLineSeries();

    seriesY_current->setName("Current Y");
    seriesY_target->setName("Target Y");

    chartY->addSeries(seriesY_current);
    chartY->addSeries(seriesY_target);

    auto *axisTime = new QValueAxis();
    auto *axisPos  = new QValueAxis();

    axisTime->setTitleText("时间 (s)");
    axisTime->setRange(0, 10);

    axisPos->setTitleText("角度 (°)");
    axisPos->setRange(-60, 60);

    chartY->addAxis(axisTime, Qt::AlignBottom);
    chartY->addAxis(axisPos, Qt::AlignLeft);

    seriesY_current->attachAxis(axisTime);
    seriesY_current->attachAxis(axisPos);
    seriesY_target->attachAxis(axisTime);
    seriesY_target->attachAxis(axisPos);

    chartViewY = new QChartView(chartY);
    chartViewY->setRenderHint(QPainter::Antialiasing);

    auto *layout = new QVBoxLayout(mw->ui->turntable_position_y_chart);
    layout->addWidget(chartViewY);
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
    Q_UNUSED(index);

    int mode = mw->ui->combo_ref_mode->currentIndex();

    useTrajectoryX = (mode == 1 || mode == 3);
    useTrajectoryY = (mode == 1 || mode == 2);

    if (useTrajectoryX) trajTimeX = 0.0;
    if (useTrajectoryY) trajTimeY = 0.0;

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
 * @brief 闭环参考轨迹配置用户编辑后解析输入轨迹是否正确
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


}
/**
 * @brief 根据 JSON 配置和当前时间，计算某一轴的参考轨迹值
 * @param cfg        完整的轨迹 JSON（包含 time + axis）
 * @param axisName   轴名，如 "x" / "y" / "yaw" / "pitch"
 * @param t          当前轨迹时间（由 PID 定时器推进）
 * @param finished   输出：轨迹是否结束
 * @return           当前时刻的参考值
 */
double TabTurntableControl::evaluateTrajectory(
    const QJsonObject &cfg,
    const QString &axisName,
    double t,
    bool &finished)
{
    finished = false;

    // ---------- time ----------
    auto timeObj = cfg["time"].toObject();
    double duration = timeObj["duration"].toDouble(0.0);

    if (duration > 0.0 && t >= duration) {
        finished = true;
        t = duration;   // 防止时间越界
    }

    // ---------- axis ----------
    auto axisObj = cfg[axisName].toObject();
    QString type = axisObj["type"].toString();

    double offset    = axisObj["offset"].toDouble(0.0);
    double amplitude = axisObj["amplitude"].toDouble(0.0);
    double freq      = axisObj["frequency"].toDouble(0.0);
    double phase     = axisObj["phase"].toDouble(0.0);

    double w = 2.0 * M_PI * freq;

    // ---------- waveform ----------
    if (type == "sine") {
        return offset + amplitude * std::sin(w * t + phase);
    }
    else if (type == "circle") {
        // 对单轴来说，circle 只是“正弦/余弦的约定”
        // X = cos, Y = sin（约定）
        if (axisName.toLower().contains("x")) {
            return offset + amplitude * std::cos(w * t + phase);
        } else {
            return offset + amplitude * std::sin(w * t + phase);
        }
    }

    return offset;
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
        closedLoopXEnabled = true;
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
        closedLoopYEnabled = true; 
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
    mw->turntable_controller->set_axis_speed(Yaw, 0);
    closedLoopXEnabled = false; 
    mw->ui->control_status->setText("X轴闭环控制：停止");
    mw->ui->control_status->setStyleSheet("color: red;");
}
/**
 * @brief y轴PID闭环停止
 */

 void TabTurntableControl::on_btn_stop_y_pidcontrol_clicked()
 {
     if (closedLoopTimerY) closedLoopTimerY->stop();
     mw->turntable_controller->set_axis_speed(Pitch, 0);
     closedLoopYEnabled = false; 
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
   // ===== 图表时间推进 =====
   chartTimeX += 0.2;
   chartTimeY += 0.2;
    // ===== X 轴图 =====
    seriesX_current->append(chartTimeX, xPos);
    seriesX_target->append(chartTimeX, target_x);

    if (chartTimeX > 10.0) {
        chartX->axisX()->setRange(chartTimeX - 10.0, chartTimeX);
    }
    // ===== Y 轴图 =====
    seriesY_current->append(chartTimeY, yPos);  
    seriesY_target->append(chartTimeY, target_y);
    if (chartTimeY > 10.0) {
        chartY->axisX()->setRange(chartTimeY - 10.0, chartTimeY);
    }    
}

/**
 * @brief 闭环x轴 PID 控制定时器回调（周期性执行）
 */
void TabTurntableControl::closedLoopTickX()
{
    if (!pidX || !closedLoopXEnabled) return;
     // === Reference Generator（X）===
    if (useTrajectoryX) {
        bool trajFinished = false;
        target_x = evaluateTrajectory(
            trajConfigX,
            "yaw",
            trajTimeX,
            trajFinished
        );
        qDebug() << "计算结果 target_x:" << target_x;
        trajTimeX += trajConfigX["time"].toObject()["dt"].toDouble(0.05);
        if (trajFinished) {
            closedLoopTimerX->stop();
            closedLoopXEnabled = false;
            mw->turntable_controller->set_axis_speed(Yaw, 0);
            QMessageBox::information(mw, "完成", "X轴轨迹跟踪完成");
            return;
        }
    }
    // 使用 pid 控制器的 controlLoop 接口（假定定义类似于你的实现）
    bool doneX = pidX->controlLoop(target_x, 0.01, 0.05);

    if (!useTrajectoryX  && doneX) {
        closedLoopTimerX->stop();
        closedLoopXEnabled = false;
        mw->turntable_controller->set_axis_speed(Yaw, 0);
        mw->ui->control_status->setText("X轴闭环控制：完成");
        mw->ui->control_status->setStyleSheet("color: blue;");
        QMessageBox::information(mw, "完成", "转台已到达目标点（误差 ≤ 0.01）");
    }
}
/**
 * @brief 闭环 y轴PID 控制定时器回调（周期性执行）
 */
void TabTurntableControl::closedLoopTickY()
{
    if (!pidY || !closedLoopYEnabled) return;
    // === Reference Generator（Y）===
    if (useTrajectoryY) {
        bool trajFinished = false;
       
        target_y = evaluateTrajectory(
            trajConfigY,
            "pitch",
            trajTimeY,
            trajFinished
        );

        trajTimeY += trajConfigY["time"].toObject()["dt"].toDouble(0.05);

        if (trajFinished) {
            closedLoopTimerY->stop();
            closedLoopYEnabled = false;
            mw->turntable_controller->set_axis_speed(Pitch, 0);
            QMessageBox::information(mw, "完成", "Y轴轨迹跟踪完成");
            return;
        }
    }
    bool doneY = pidY->controlLoop(target_y, 0.01, 0.05);

    if ( !useTrajectoryY  && doneY) {
        closedLoopTimerY->stop();
        closedLoopYEnabled = false;
        mw->turntable_controller->set_axis_speed(Pitch, 0);
        mw->ui->control_status->setText("y轴闭环控制：完成");
        mw->ui->control_status->setStyleSheet("color: blue;");
        QMessageBox::information(mw, "完成", "转台已到达目标点（误差 ≤ 0.01）");
    }
}
/**
 * @brief 是否开启数据监控
 */
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
