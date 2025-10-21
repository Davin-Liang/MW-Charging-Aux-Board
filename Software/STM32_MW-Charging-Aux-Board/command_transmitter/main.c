#include "command_transmitter.h"
#include <QCoreApplication>
#include <QTimer>
#include <iostream>

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
    
    // 可选：设置定时器来发送测试命令
    QTimer::singleShot(3000, [&transmitter]() 
    {
        std::cout << "发送测试命令..." << std::endl;
        transmitter.send_motor_command(100.0, 200.0, 50);
    });
    
    std::cout << "按 Ctrl+C 退出程序" << std::endl;
    
    return app.exec();
}