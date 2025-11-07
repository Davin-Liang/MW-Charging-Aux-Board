#include "command_transmitter.h"
#include <QCoreApplication>
#include <QTimer>
#include <iostream>
#include <QThread>
#include <unistd.h>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    CommandTransmitter transmitter;
    
    /* 启动服务器 */
    if (transmitter.start_server(8080)) 
    {
        std::cout << "服务器启动成功，端口: 8080" << std::endl;
    } 
    else 
    {
        std::cerr << "服务器启动失败" << std::endl;
        return -1;
    }

    sleep(10);
    
    QTimer timer = QTimer(&app);
    // 设置定时器每3000毫秒触发一次
    timer.setInterval(8000);
    // 连接定时器的超时信号
    QObject::connect(&timer, &QTimer::timeout, [&transmitter]() {
        std::cout << "发送测试命令..." << std::endl;
        // transmitter.send_motor_command();
        // std::cout << "1" << std::endl;
        // QThread::msleep(3000); // 延时1秒
        // transmitter.send_motor_command(0.23, 0.23, 50);
        // std::cout << "2" << std::endl;
        // QThread::msleep(3000); // 延时1秒
        // transmitter.send_find_opt_command();
        // transmitter.send_time_command();
        // std::cout << "3" << std::endl;
        // QThread::msleep(1000); // 延时1秒
        // transmitter.send_find_opt_command(CIR_TRAJ, 100, 200, 14, 0.1);
        // std::cout << "4" << std::endl;
        // QThread::msleep(1000); // 延时1秒
    });

    // // 每3秒触发一次
    timer.start(5000);
    
    std::cout << "按 Ctrl+C 退出程序" << std::endl;
    
    return app.exec();
}