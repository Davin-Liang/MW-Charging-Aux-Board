#include "usb-hikvisioner.h"
#include "antenna-visioner.h"
#include "mpp-converter.h"
#include <iostream>
#include <csignal>   // 【新增】用于捕获操作系统信号
#include <atomic>    // 【新增】用于线程安全的全局标志位
#include <chrono>

// 引入 ZLM 全局环境头文件
#include "Util/logger.h"
#include "Network/TcpServer.h"
#include "Thread/WorkThreadPool.h"
#include "Rtsp/RtspSession.h"
#include "Rtmp/RtmpSession.h"

// 【新增】全局运行标志位
std::atomic<bool> g_isRunning{true};

// 【新增】Ctrl+C 信号处理回调函数
void signal_handler(int signal) {
    std::cout << "\n[系统提示] 接收到退出信号 (Ctrl+C)，正在安全停止流水线并关闭相机..." << std::endl;
    g_isRunning = false; // 改变标志位，打破死循环
}

int main() {
    // ==========================================================
    // 第一步：先开门营业 (启动 ZLM 基础网络服务)
    // ==========================================================
    toolkit::Logger::Instance().add(std::make_shared<toolkit::ConsoleChannel>());
    toolkit::Logger::Instance().setWriter(std::make_shared<toolkit::AsyncLogWriter>());
    toolkit::WorkThreadPool::setPoolSize(4); // 启动四核线程池

// 【修复】：加上 static，保证这俩服务器对象的生命周期直到程序结束才销毁
    static toolkit::TcpServer::Ptr rtspSrv(new toolkit::TcpServer());
    static toolkit::TcpServer::Ptr rtmpSrv(new toolkit::TcpServer());

    try {
        rtspSrv->start<mediakit::RtspSession>(8554); 
        rtmpSrv->start<mediakit::RtmpSession>(1935); 
        
        std::cout << "[系统提示] ZLM 流媒体服务器已启动 (RTSP: 8554, RTMP: 1935)!" << std::endl;
    } catch (std::exception &ex) {
        std::cerr << "[严重错误] 启动失败: " << ex.what() << std::endl;
        return -1;
    }

    // 【新增】注册 SIGINT (Ctrl+C) 信号给操作系统
    std::signal(SIGINT, signal_handler);

    // 1. 初始化底层的海康相机与 DMA 流水线
    USBHikvisioner camera(0);
    camera.camera_init();
    camera.open_grab_callback(); // 开启 DMA 回调模式
    
    // 实例化编码管理器，传入相机指针 (用于内存回收)
    MppConverter encoderManager(&camera);

    if (!camera.camera_open()) {
        std::cerr << "相机打开失败，程序退出！" << std::endl;
        return -1;
    }

    // 2. 实例化 AntennaVisioner，传入相机指针进行绑定
    AntennaVisioner visioner("./yolov8_model/yolov8n.rknn", 
                             "./yolov8_model/coco_80_labels_list.txt", 
                             640, 640, &camera);

    if (!visioner.init_system()) return -1;

    // 3. 循环推理
    cv::Mat outputFrame;
    std::vector<DetectionResult> results;
    
    // 【新增】：FPS 统计专用的变量
    int frame_count = 0;
    auto fps_start_time = std::chrono::high_resolution_clock::now();

    // 【修改】：使用全局标志位代替 true
    while (g_isRunning) { 
        // 记录单帧开始时间
        auto frame_start_time = std::chrono::high_resolution_clock::now();

        // 极速单次检测！直接从 DMA 池拿数据，NPU 推理，并在物理内存上画框
        if (visioner.detect_once(outputFrame, results)) {
            // 【新增】：成功处理一帧，计数器加 1
            frame_count++;

            // 终端打印识别结果 (如果觉得每帧打印太刷屏，可以注释掉这块)
            for (const auto& res : results) {
                std::cout << "Detected: " << res.className 
                          << " Prob: " << res.prop * 100 << "%" << std::endl;
            }
            
            // 计算并打印【单帧】耗时
            auto frame_end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(frame_end_time - frame_start_time);
            // std::cout << "[time]: 目标识别单帧耗时: " << duration.count() << "ms." << std::endl;
        }

        // ==========================================
        // 【核心新增】：计算并打印 1 秒内的平均 FPS
        // ==========================================
        auto current_time = std::chrono::high_resolution_clock::now();
        auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - fps_start_time).count();

        // 如果时间经过了 1000 毫秒（1秒）
        if (elapsed_time >= 1000) {
            std::cout << "\n========================================" << std::endl;
            std::cout << "🚀 [性能统计] 过去 1 秒内共识别: " << frame_count << " 帧 (FPS: " << frame_count << ")" << std::endl;
            std::cout << "========================================\n" << std::endl;
            
            // 重置计数器和计时器，开始下一秒的统计
            frame_count = 0;
            fps_start_time = current_time;
        }
    }

    // ==========================================================
    // 4. 安全退出区 (当按下 Ctrl+C 后，程序会安全运行到这里)
    // ==========================================================
    std::cout << "---------------------------------" << std::endl;
    std::cout << "开始执行安全清理流程..." << std::endl;

    if (camera.camera_close())
        std::cout << "成功关闭相机." << std::endl;
    else
        std::cerr << "无法关闭相机." << std::endl;

    // 当 main 函数 return 时，visioner 和 camera 对象的析构函数会自动触发
    std::cout << "程序安全退出完毕！" << std::endl;
    
    return 0;
}