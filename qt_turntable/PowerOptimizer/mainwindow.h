#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTimer>
#include <QComboBox>
#include <QtCharts>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QListWidget>
#include <QTableWidget>      // 新增
#include <QTableWidgetItem>  // 新增
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QHeaderView>       // 新增：用于表格列设置
#include "command_transmitter.h"  // 添加这行
#include <QPixmap>
#include <QTimer>
#include "turntable_controller.h"
#include "PID_Controller.h"

#include "tab_file_read.h"
#include "tab_turntable_control.h"
#include "tab_stm32.h"
#include "tab_device_connect.h"
class TabFileRead;
class TabSTM32;
class TabTurntable_Control;
class TabDeviceConnect;
QT_USE_NAMESPACE

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    friend class TabFileRead;
    friend class TabSTM32;
    friend class TabTurntableControl;
    friend class TabDeviceConnect;
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // void on_pushButton_connect_clicked();      // 连接按钮点击
    // void on_pushButton_disconnect_clicked();   // 断开按钮点击

    // void onSocketConnected();                  // 连接成功
    // void onSocketDisconnected();               // 连接断开
    // void onSocketReadyRead();                  // 数据接收
    // void onSocketError(QAbstractSocket::SocketError error); // 连接错误

    // // 新增的按钮槽函数
    // void on_pushButton_motor_control_clicked();    // 电机控制
    // void on_pushButton_find_optimal_clicked();     // 寻优控制


    // void initializeUIWithConfig();
    // void trajTypeChanged(int index);


    // void onMotorDataReceived(const MotorData_t &data);
    // void onChannelDataReceived(const CurrentVPCh_t &data);
    // void onOptResDataReceived(const OptResData_t &data);

    // 新增：文件读取相关槽函数
    // void on_pushButton_filenamedisplay_clicked();  // 文件存储情况显示
    // void on_pushButton_read_file_clicked();        // 文件内容查看
    // void on_listWidget_files_itemSelectionChanged(); // 文件选择变化

    // 新增 UI 控制槽函数
    // void update_turntable_image();

    // void on_btn_set_speed_clicked();
    // void on_btn_stop_x_turntable_run_clicked();
    // void on_btn_stop_y_turntable_run_clicked();

    // void on_btn_set_x_pos_clicked();
    // void on_btn_set_y_pos_clicked();

    // void on_btn_x_zero_clicked();
    // void on_btn_y_zero_clicked();

    // 转台控制相关
    // void on_pushButton_connection_clicked();    // 连接转台
    // void on_pushButton_disconnection_clicked(); // 断开转台
    // void updateTurntableData();                 // 定时刷新转台数据
    

    // void on_btn_set_target_pos_clicked();              // 设置参考位置
    // void on_btn_set_pidcontroller_parameter_clicked(); // 设置PID参数并启动控制
    // void closedLoopTick();        // 定时执行 PID.controlLoop()

    // void on_controller_selection_changed(int index);//转台控制选择


private:
    Ui::MainWindow *ui;
    QTcpSocket *tcpSocket;                     // TCP套接字（保留用于兼容性）

    // bool isConnected;                          // 连接状态标志
    // QByteArray receiveBuffer;                 // 重新添加 receiveBuffer

    CommandTransmitter *commandTransmitter;
    QTimer *turntableMonitorTimer;             // 定时器用于数据监控
    QTimer *closedLoopTimer;

    TurntableController *turntable_controller; // 转台控制对象
    PIDController *pid_x;  //转台PID控制器参数
    PIDController *pid_y;
    // double target_x;
    // double target_y;

    TabDeviceConnect *tabDeviceConnect;
    TabSTM32 *tabSTM32;
    TabTurntableControl *tabTurntableControl;
    TabFileRead *tabFileRead;

    // // 电机位置显示图表相关成员变量
    // QChart *motorChart;
    // QLineSeries *motorTrajectorySeries;
    // QScatterSeries *currentPositionSeries;
    // QValueAxis *axisX;
    // QValueAxis *axisY;
    // QChartView *chartView;
    // QVector<QPointF> positionHistory;          // 历史轨迹数据

    //转台位置显示图表相关成员变量

    // QtCharts::QChart *turntableChart;
    // QtCharts::QChartView *turntableChartView;
    // QtCharts::QLineSeries *series_target_x;
    // QtCharts::QLineSeries *series_target_y;
    // QtCharts::QLineSeries *series_current_x;
    // QtCharts::QLineSeries *series_current_y;
    
    // double chart_time = 0.0;   // 曲线横轴时间

    // // 文件读取相关变量
    // QString m_currentSelectedFile;             // 当前选中的文件路径

    void applyModernStyle();
    // void parseEthernetData(const QByteArray &data);
    void setupUI();
    void setupSignals();
    // 新增辅助函数
    //void setupCommandTransmitter();
    // void updateConnectionStatus(bool connected);
    // void setTurntableConnectionStatus(bool connected);

    // // 图表相关函数
    // void initializeMotorChart();               // 初始化电机状态图表
    // void updateMotorChart(double x, double y); // 更新电机状态图表

    // void setupReadOnlyDataMonitoring();

    // // 文件读取相关函数
    // void scanDataFiles();                      // 扫描数据文件
    // void displayFileContent(const QString &filePath); // 显示文件内容（表格形式）
    // void setupTableWidget();                   // 初始化表格控件
    // void loadCsvToTable(const QString &filePath); // 加载CSV到表格

    //初始化转台的图
    void initializeTurntablePositionChart();



};


#endif // MAINWINDOW_H
