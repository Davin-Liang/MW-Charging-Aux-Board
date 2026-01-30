#include <iostream>
#include "rc522-controller.h"

int main(int argc, char* argv[]) {
    std::cout << "Initializing RC522 Controller..." << std::endl;

    // 实例化控制器，传入设备节点路径
    // 如果你的设备节点不同，请修改此处字符串
    RC522Controller rfid("/dev/RFID-RC522");

    if (rfid.is_valid()) {
        // 启动类内部的测试循环
        rfid.run_test_loop();
    } else {
        std::cerr << "Failed to initialize RC522 driver." << std::endl;
        return -1;
    }

    return 0;
}