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
#include <ctime>

#include "command_struct.h"


class MainWindow;
//class CommandTransmitter;

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


    // void onMotorDataReceived(const MotorData_t &data);
    // void onChannelDataReceived(const CurrentVPCh_t &data);
    // void onOptResDataReceived(const OptResData_t &data);

    void onStm32MonitorTimeout();


private:
    MainWindow *mw;
    // 文件记录状态
    bool isMotorRecording;
    bool isOptimizationRecording;
    bool isChannelRecording;
    // 当前记录的文件名
    std::string currentMotorFile;
    std::string currentOptResultFile;
    std::string currentChannelFile;

    // 通道信息索引（每次新文件从0开始）
    uint32_t channelInfoIndex;
    // 电机轨迹显示图相关
    QChart *motorChart;
    QLineSeries *motorTrajectorySeries;
    QScatterSeries *currentPositionSeries;
    QValueAxis *axisX;
    QValueAxis *axisY;
    QChartView *chartView;
    QVector<QPointF> positionHistory;

    MotorCmd_t motorCmd;
    FindOptimalCmd_t findOptCmd;
    MotorData_t motorData;
    DateTime_t timeData;
    OptResData_t optResData;
    CurrentVPCh_t currentVPCh;

    QTimer *stm32_MonitorTimer;
    // 图表相关函数
    bool readConfigFromJSON(const std::string &filename);
    void writeConfigToModbus();
    void initializeMotorChart();               // 初始化电机状态图表
    void updateMotorChart(double x, double y); // 更新电机状态图表

    void initializeUIWithConfig();
    void setupReadOnlyDataMonitoring();
    void parseEthernetData(const QByteArray &data);

    bool write_motor_data_to_csv(const std::string& filename, const MotorData_t * motorData);
    bool write_opt_res_to_csv(const std::string& filename, const OptResData_t * optData);
    bool write_cur_channel_info_to_csv(const std::string& filename, const CurrentVPCh_t * channelData);
    std::string generate_file_name(const std::string& fileSuffix, const DateTime_t* datetime);

    // ===== 新增：Modbus 写入辅助函数 =====
    void writeMotorRegisters();
    void writeFindOptRegisters();
    void writeTimeRegisters();
    void setUpdateFlag(uint16_t bitMask);
};

#endif // TAB_STM32_H
