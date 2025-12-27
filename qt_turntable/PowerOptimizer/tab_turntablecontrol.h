#ifndef TAB_TURNTABLECONTROL_H
#define TAB_TURNTABLECONTROL_H


#include <QObject>
#include <QPointF>
#include <QVector>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QJsonObject>

class MainWindow;
class TurntableController;
class PIDController;
class QTimer;


class TabTurntableControl : public QObject
{
    Q_OBJECT
public:
    explicit TabTurntableControl(MainWindow *mw);
    ~TabTurntableControl();

    /// 在构造后调用以绑定 UI 信号槽
    void setupConnections();

    /// 初始化转台轨迹图（如果想让这个类自己创建 chart 可在此实现）
    void initializeTurntableChart();

public slots:
    /// UI 相关槽（由 ui 按钮触发）
    void update_turntable_image();
    void on_btn_set_speed_clicked();
    void on_btn_stop_x_turntable_run_clicked();
    void on_btn_stop_y_turntable_run_clicked();
    void on_btn_set_x_pos_clicked();
    void on_btn_set_y_pos_clicked();
    void on_btn_x_zero_clicked();
    void on_btn_y_zero_clicked();

    void on_btn_set_target_pos_clicked();              // 设置参考位置


    // X 轴 PID
    void on_btn_set_x_pidcontroller_parameter_clicked();
    void on_btn_stop_x_pidcontrol_clicked();
    void closedLoopTickX();/// 定时器 tick：用于 PID 闭环（连接到 MainWindow::closedLoopTimer）

    // Y 轴 PID
    void on_btn_set_y_pidcontroller_parameter_clicked();
    void on_btn_stop_y_pidcontrol_clicked();
    void closedLoopTickY();/// 定时器 tick：用于 PID 闭环（连接到 MainWindow::closedLoopTimer）

    /// 定时器 tick：用于刷新转台数据（连接到 MainWindow::turntableMonitorTimer）
    void updateTurntableData();

    void on_controller_selection_changed(int index);//转台控制选择

    void on_data_monitor_section_stateChanged(int state);

    void on_ref_mode_changed(int index); // 参考源切换

private:
    MainWindow *mw;                          ///< 主窗口指针（不拥有）
    PIDController *pidX;                    ///< 指向主窗口 PID 对象（不拥有）
    PIDController *pidY;
    QTimer *monitorTimer;                   ///< 指向主窗口的监控定时器（不拥有）
    QTimer *closedLoopTimerX;             ///< 指向主窗口闭环定时器（不拥有） 
    QTimer *closedLoopTimerY;              

   
    // // 图表
    // QtCharts::QChart *turntableChart;
    // QtCharts::QChartView *turntableChartView;
    // QtCharts::QLineSeries *series_target_x, *series_target_y, *series_current_x, *series_current_y;
    // QtCharts::QValueAxis *axisX;
    // QtCharts::QValueAxis *axisY;

    // double chart_time = 0.0;   // 曲线横轴时间

    // ===== X 轴图 =====
    QtCharts::QChart *chartX = nullptr;
    QtCharts::QChartView *chartViewX = nullptr;
    QtCharts::QLineSeries *seriesX_current = nullptr;
    QtCharts::QLineSeries *seriesX_target  = nullptr;

    // ===== Y 轴图 =====
    QtCharts::QChart *chartY = nullptr;
    QtCharts::QChartView *chartViewY = nullptr;
    QtCharts::QLineSeries *seriesY_current = nullptr;
    QtCharts::QLineSeries *seriesY_target  = nullptr;

    double chartTimeX = 0.0;// 曲线横轴时间
    double chartTimeY = 0.0;

    double target_x = 0, target_y = 0;


    // === 参考模式 ===
    bool useTrajectoryX = false;
    bool useTrajectoryY = false;

    QJsonObject trajConfigX;
    QJsonObject trajConfigY;

   // === 轨迹时间 ===
    double trajTimeX = 0.0;
    double trajTimeY = 0.0;
    // === 控制使能 ===
    bool closedLoopXEnabled = false;      //// 是否启用 X 轴闭环控制
    bool closedLoopYEnabled = false;     //// 是否启用 Y 轴闭环控制

    //初始化
    void initializeTurntablePositionChart();

    // 轨迹与点位统一参考更新
    double evaluateTrajectory(const QJsonObject &cfg,const QString &axisName,double t,bool &finished);


    //JSON 轨迹加载
    bool loadTrajectoryJson(QJsonObject &cfg,const QString &axisName);
    bool validateTrajectoryJson(const QJsonObject &obj,const QString &axisName,QString &errorMsg);
    QString defaultTrajectoryTemplate(const QString &axisName);
};

#endif // TAB_TURNTABLE_CONTROL_H
