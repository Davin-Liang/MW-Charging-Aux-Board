#ifndef TAB_TURNTABLE_CONTROL_H
#define TAB_TURNTABLE_CONTROL_H

#include <QObject>
#include <QPointF>
#include <QVector>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis> 
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
    void on_btn_set_pidcontroller_parameter_clicked(); // 设置PID参数并启动控制

    /// 定时器 tick：用于刷新转台数据（连接到 MainWindow::turntableMonitorTimer）
    void updateTurntableData();

    /// 定时器 tick：用于 PID 闭环（连接到 MainWindow::closedLoopTimer）
    void closedLoopTick(); // 定时执行 PID.controlLoop()


    void on_controller_selection_changed(int index);//转台控制选择
    void on_btn_stop_pidcontrol_clicked();

private:
    MainWindow *mw;                          ///< 主窗口指针（不拥有）
    TurntableController *turntable;         ///< 指向主窗口的转台控制器（不拥有）
    PIDController *pidX;                    ///< 指向主窗口 PID 对象（不拥有）
    PIDController *pidY;
    QTimer *monitorTimer;                   ///< 指向主窗口的监控定时器（不拥有）
    QTimer *closedLoopTimer;                ///< 指向主窗口闭环定时器（不拥有）
    
    
    // 图表
    QtCharts::QChart *turntableChart;
    QtCharts::QChartView *turntableChartView;
    QtCharts::QLineSeries *series_target_x, *series_target_y, *series_current_x, *series_current_y;
    QtCharts::QValueAxis *axisX;
    QtCharts::QValueAxis *axisY;

    double chart_time = 0.0;   // 曲线横轴时间

    //初始化
    void initializeTurntablePositionChart();

    double target_x;
    double target_y;
};

#endif // TAB_TURNTABLE_CONTROL_H
