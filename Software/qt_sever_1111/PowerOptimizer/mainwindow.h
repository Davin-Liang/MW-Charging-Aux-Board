#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTimer>
#include <QComboBox>
#include <QtCharts>
#include <QListWidget>
#include <QTableWidget>      // 新增
#include <QTableWidgetItem>  // 新增
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QHeaderView>       // 新增：用于表格列设置
#include "command_transmitter.h"  // 添加这行

QT_USE_NAMESPACE  // 使用Qt Charts命名空间

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_connect_clicked();      // 连接按钮点击
    void on_pushButton_disconnect_clicked();   // 断开按钮点击

   // void onSocketConnected();                  // 连接成功
   // void onSocketDisconnected();               // 连接断开
   // void onSocketReadyRead();                  // 数据接收
   // void onSocketError(QAbstractSocket::SocketError error); // 连接错误

    // 新增的按钮槽函数
    void on_pushButton_motor_control_clicked();    // 电机控制
    void on_pushButton_find_optimal_clicked();     // 寻优控制

    void initializeUIWithConfig();
    void on_traj_type_changed(int index);


    void onMotorDataReceived(const MotorData_t &data);
    void onChannelDataReceived(const CurrentVPCh_t &data);
    void onOptResDataReceived(const OptResData_t &data);

    // 新增：文件读取相关槽函数
    void on_pushButton_filenamedisplay_clicked();  // 文件存储情况显示
    void on_pushButton_read_file_clicked();        // 文件内容查看
    void on_listWidget_files_itemSelectionChanged(); // 文件选择变化

private:
    Ui::MainWindow *ui;
   // QTcpSocket *tcpSocket;                     // TCP套接字
    QTimer *sdCardQueryTimer;                  // SD卡状态查询定时器
    bool isConnected;                          // 连接状态标志
   // QByteArray receiveBuffer;                 // 重新添加 receiveBuffer

    CommandTransmitter *commandTransmitter;

    // 图表相关成员变量
    QChart *motorChart;
    QLineSeries *motorTrajectorySeries;
    QScatterSeries *currentPositionSeries;
    QValueAxis *axisX;
    QValueAxis *axisY;
    QChartView *chartView;
    QVector<QPointF> positionHistory;          // 历史轨迹数据

    // 文件读取相关变量
    QString m_currentSelectedFile;             // 当前选中的文件路径

    void applyModernStyle();
    void parseEthernetData(const QByteArray &data);
    void updateSDCardDisplay(const QByteArray &sdData);

    // 新增辅助函数
    void setupCommandTransmitter();
    void updateConnectionStatus(bool connected);

    // 图表相关函数
    void initializeMotorChart();               // 初始化电机状态图表
    void updateMotorChart(double x, double y); // 更新电机状态图表

    void setupReadOnlyDataMonitoring();

    // 新增：文件读取相关函数
    void scanDataFiles();                      // 扫描数据文件
    void displayFileContent(const QString &filePath); // 显示文件内容（表格形式）
    void setupTableWidget();                   // 初始化表格控件
    void loadCsvToTable(const QString &filePath); // 加载CSV到表格
};

#endif // MAINWINDOW_H
