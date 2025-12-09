#ifndef TAB_STM32_H
#define TAB_STM32_H

#include <QObject>
#include <QVector>
#include <QPointF>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QValueAxis>



class MainWindow;
class CommandTransmitter;

class TabSTM32 : public QObject
{
    Q_OBJECT
public:
    explicit TabSTM32(MainWindow *mw);
    ~TabSTM32();

    /// 连接 UI 按钮 / 信号
    void setupConnections();

public slots:
    // 新增的按钮槽函数
    void on_pushButton_motor_control_clicked();    // 电机控制
    void on_pushButton_find_optimal_clicked();     // 寻优控制


    void trajTypeChanged(int index);


    void onMotorDataReceived(const MotorData_t &data);
    void onChannelDataReceived(const CurrentVPCh_t &data);
    void onOptResDataReceived(const OptResData_t &data);


private:
    MainWindow *mw;
    CommandTransmitter *transmitter; // 只缓存指针（不拥有）

    // 电机轨迹显示图相关
    QChart *motorChart;
    QLineSeries *motorTrajectorySeries;
    QScatterSeries *currentPositionSeries;
    QValueAxis *axisX;
    QValueAxis *axisY;
    QChartView *chartView;
    QVector<QPointF> positionHistory;

    // 图表相关函数
    void initializeMotorChart();               // 初始化电机状态图表
    void updateMotorChart(double x, double y); // 更新电机状态图表

    void initializeUIWithConfig();
    void setupReadOnlyDataMonitoring();
    void parseEthernetData(const QByteArray &data);
};

#endif // TAB_STM32_H
