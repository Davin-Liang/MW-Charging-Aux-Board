#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QMessageBox>
#include <QDebug>


/**
 * @brief 构造函数，初始化主窗口与核心模块
 * @param parent 父窗口对象指针
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , commandTransmitter(nullptr)
    , turntable_controller(nullptr)
    ,turntableMonitorTimer(new QTimer(this))
    ,closedLoopTimer_x(new QTimer(this))
    ,closedLoopTimer_y(new QTimer(this))
    ,pid_x(nullptr)
    ,pid_y(nullptr)

{
    ui->setupUi(this);

    //初始化界面设置
    setupUI();
    // 创建核心通信对象（服务器）
    commandTransmitter = new CommandTransmitter(this);

    // 创建并初始化四个 Tab 页面模块
    tabDeviceConnect     = new TabDeviceConnect(this);
    tabDeviceConnect->setupConnections();

    
    tabSTM32             = new TabSTM32(this);
    tabSTM32->setupConnections();

    tabTurntableControl  = new TabTurntableControl(this);
    tabTurntableControl->setupConnections();

    tabFileRead          = new TabFileRead(this);
    tabFileRead->setupConnections();

}
/**
 * @brief 初始化 UI 的通用设置，例如状态栏、只读框、样式等
 */
void MainWindow::setupUI()
{
    ui->tabWidget->setCurrentIndex(0);   //显示第0页
    setWindowTitle("各类设备控制中心");
    applyModernStyle();

   // 初始化转台 PID 控制器
    pid_x = new PIDController(Yaw, turntable_controller);
    pid_y = new PIDController(Pitch, turntable_controller);

}

/**
 * @brief 应用现代化 UI 样式
 *
 * 设置窗口、按钮、文本框等控件的整体视觉风格，
 * 增加统一性与美观度，同时保留图表等控件的自定义样式空间。
 */
void MainWindow::applyModernStyle()
{
    const QString styleSheet = R"(
        QMainWindow {
            background: #f0f0f0;
            font-family: 'Microsoft YaHei';
        }

        QGroupBox {
            background-color: #ffffff;
            border: 1px solid #cccccc;
            border-radius: 5px;
            margin-top: 10px;
            padding-top: 10px;
            font-weight: bold;
        }

        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px;
        }

        QPushButton {
            background-color: #4CAF50;
            color: white;
            border: none;
            border-radius: 4px;
            padding: 6px 12px;
        }
        QPushButton:hover {
            background-color: #45a049;
        }
        QPushButton:pressed {
            background-color: #3d8b40;
        }
        QPushButton:disabled {
            background-color: #cccccc;
            color: #666666;
        }

        QLineEdit {
            border: 1px solid #cccccc;
            border-radius: 3px;
            padding: 4px 8px;
            background-color: white;
        }
        QLineEdit:focus {
            border-color: #4CAF50;
        }
        QLineEdit:disabled {
            background-color: #f0f0f0;
            color: #999999;
        }

        QLabel {
            color: #333333;
        }
    )";

    setStyleSheet(styleSheet);
}

/**
 * @brief 析构函数，负责释放 MainWindow 使用的资源。
 *        Qt 的对象会根据父子对象自动析构，但对于手动 new 的对象仍需显式释放。
 */
MainWindow::~MainWindow()
{
    if (commandTransmitter != nullptr) {
        commandTransmitter->stop_server();
    }


    // PID 控制器资源释放
    delete pid_x;
    delete pid_y;
    pid_x = nullptr;
    pid_y = nullptr;

    delete turntable_controller;
    turntable_controller = nullptr;
    // ui 最后释放
    delete ui;
    ui = nullptr;
}
