#pragma once

#include <cstdio>
#include <cstring>
#include <memory>
#include <condition_variable>
#include <thread>
#include <queue>
#include <atomic>
#include <mutex>

#include "opencv2/core.hpp"
#include "opencv2/videoio.hpp" 
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/opencv.hpp"

#include "MvCameraControl.h"
#include "dma-buffer-pool.h"
#include "ThreadSafeQueue.h"

/**
 * @brief 相机状态强类型枚举
 */
enum class CameraStatus {
    WAIT_FOR_INIT = 0, ///< 等待初始化
    INITED,            ///< 已初始化
    OPENED,            ///< 已打开并正在取流
    CLOSED             ///< 已关闭
};

/**
 * @brief 海康 USB 工业相机控制与图像采集分发类
 * @details 负责底层 SDK 交互、DMA 零拷贝内存分配，以及 RGA 硬件加速的异步数据流转。
 */
class USBHikvisioner {
public:
    /**
     * @brief 构造函数
     * @param videoDeviceIndex 目标相机的设备索引号 (通常为0)
     */
    explicit USBHikvisioner(int videoDeviceIndex);

    /**
     * @brief 析构函数，负责安全释放硬件资源与停止线程
     */
    ~USBHikvisioner();

    /* 规范：Rule of Five。因包含底层硬件句柄和线程，绝对禁止拷贝和赋值！ */
    USBHikvisioner(const USBHikvisioner&) = delete;
    USBHikvisioner& operator=(const USBHikvisioner&) = delete;

    /**
     * @brief 初始化相机设备
     * @return true 成功 | false 失败
     */
    bool camera_init();

    /**
     * @brief 打开相机并开始取流
     * @return true 成功 | false 失败
     */
    bool camera_open();

    /**
     * @brief 关闭相机并停止取流
     * @return true 成功 | false 失败
     */
    bool camera_close();

    /**
     * @brief 阻塞读取一帧图像 (传统的轮询方式)
     * @param img 输出的 OpenCV Mat 图像
     * @param nMsec 超时时间 (毫秒)
     * @return true 读取成功 | false 读取失败超时
     */
    bool read_img(cv::Mat& img, unsigned int nMsec);

    /**
     * @brief 将底层数据包转换为 OpenCV Mat
     * @param stImageInfo 海康 SDK 输出的图像信息
     * @param pData 底层图像数据指针
     * @param dstImg 转换后输出的 Mat 对象
     * @return true 成功 | false 失败
     */
    bool convert_to_mat(MV_FRAME_OUT_INFO_EX& stImageInfo, 
                        std::unique_ptr<uint8_t[]>& pData,
                        cv::Mat& dstImg);

    /**
     * @brief 开启异步抓图回调模式的使能标志
     */
    void open_grab_callback();

    /**
     * @brief 启动 RGA 硬件加速的后台分发线程
     * @return true 成功 | false 失败
     */
    bool start_rga_thread();

    /**
     * @brief 安全停止并回收 RGA 分发线程
     */
    void stop_rga_thread();

    /* --- 公开的队列与内存池资源 --- */
    // 允许外部业务层（如 AntennaVisioner）直接交互的基础设施
    ThreadSafeQueue<DmaBuffer_t*> yoloTaskQueue;        ///< 对外输出的 YOLO 推理任务队列 (装载 RGB 数据)
    ThreadSafeQueue<DmaBuffer_t*> pushFlowTaskQueue;    ///< 对外输出的推流队列 (装载 NV12 数据)
    DmaBufferPool sourcePool;                           ///< 源图内存池 (存放相机 YUYV 原始数据)
    DmaBufferPool yoloPool;                             ///< YOLO 内存池 (存放 RGA 转换后的 RGB 数据)
    DmaBufferPool pushFlowPool;                         ///< YOLO 内存池 (存放 RGA 转换后的 RGB 数据)

private:
    /**
     * @brief 海康 SDK 底层抓图回调函数 (静态成员)
     * @param pstFrame 帧数据信息
     * @param pUser 透传的用户指针 (传入 USBHikvisioner 实例的 this 指针)
     * @param bAutoFree 是否由底层 SDK 自动释放内存
     */
    static void __stdcall image_callback_ex2(MV_FRAME_OUT* pstFrame, void *pUser, bool bAutoFree);

    /**
     * @brief RGA 异步调度与转换的线程本体函数
     */
    void rga_dispatch_thread_func();

    /**
     * @brief 内部调用 RGA 硬件实现 YUYV 到 RGB 的转换和缩放
     * @param srcBuf 源端 DMA Buffer
     * @param dstBuf 目标端 DMA Buffer
     * @return true 转换成功 | false 转换失败
     */
    bool rga_process_to_rgb(DmaBuffer_t* srcBuf, DmaBuffer_t* dstBuf);
    bool rga_process_to_nv12(DmaBuffer_t* srcBuf, DmaBuffer_t* dstBuf);

    /* --- 私有成员变量 --- */
    int cameraDeviceIndex_;                   ///< 相机设备索引
    void* deviceHandle_;                      ///< 海康 SDK 设备句柄
    std::unique_ptr<uint8_t[]> pData_;        ///< 轮询模式下的数据缓存 (独占智能指针)
    unsigned int nPayloadSize_;               ///< 单帧数据最大载荷大小
    MV_FRAME_OUT_INFO_EX stImageInfo_;        ///< 图像输出信息结构体
    CameraStatus cameraStatus_;               ///< 当前相机状态
    bool openGrabCallback_;                   ///< 是否启用了回调模式标记

    std::queue<DmaBuffer_t*> rgaTaskQueue_;   ///< RGA 线程的内部输入队列
    std::mutex queueMutex_;                   ///< 队列读写互斥锁
    std::condition_variable queueCv_;         ///< 队列唤醒条件变量
    std::atomic<bool> isRunning_{false};      ///< RGA 线程运行标志位
    std::thread rgaThread_;                   ///< RGA 消费者线程对象
};