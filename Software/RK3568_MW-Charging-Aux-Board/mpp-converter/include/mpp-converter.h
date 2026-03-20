/**
 * @file mpp_converter.h
 * @brief 基于 RK3568 MPP 硬件的多路并发视频编码器 (NV12 -> H.265)
 * @details 采用生产者-消费者模型，每添加一路流会自动分配一个独立线程。
 * 线程会阻塞式读取指定的 ThreadSafeQueue 以实现零拷贝编码。
 */

#ifndef MPP_CONVERTER_H
#define MPP_CONVERTER_H

#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <memory>
#include <functional>
#include <cstring>

#include "rk_mpi.h"
#include "mpp_buffer.h"
#include "mpp_frame.h"
#include "mpp_packet.h"

#include "ThreadSafeQueue.h"
#include "dma-buffer-pool.h"
#include "usb-hikvisioner.h"

// 引入 ZLMediaKit 相关头文件
#include "Common/config.h"
#include "Common/Device.h"
#include "../ext-codec/H265.h"
#include "Util/TimeTicker.h"

#include "Poller/EventPoller.h"
#include "Network/TcpServer.h"

#include "Thread/WorkThreadPool.h"
#include "Util/logger.h"
#include "Rtsp/RtspSession.h"
#include "Rtmp/RtmpSession.h"

// 内存 16 字节对齐宏 (VPU 硬件要求)
#define MPP_ALIGN(x, a) (((x) + (a) - 1) & ~((a) - 1))

/**
 * @class MppConverter
 * @brief 多路硬件编码器管理类
 */
class MppConverter {
public:
    /**
     * @brief 编码完成后的回调函数类型定义
     * @param streamId 视频流的唯一标识符
     * @param h265Data 编码后的 H.265 (NALU) 数据指针
     * @param dataSize 数据长度（字节）
     */
    using OnEncodedCallback = std::function<void(int streamId, const uint8_t* h265Data, size_t dataSize)>;

    /**
     * @brief 构造函数
     */
    MppConverter(USBHikvisioner* camera);

    /**
     * @brief 析构函数，负责安全停止所有工作线程并释放底层资源
     */
    ~MppConverter();

    /**
     * @brief 添加并启动一路 H.265 编码转换流
     * @param streamId          该路流的唯一标识 ID（例如：1 表示 720p，0 表示 1080p）
     * @param imageWidth        图像宽度（真实分辨率，如 1280）
     * @param imageHeight       图像高度（真实分辨率，如 720）
     * @param frameRate         帧率（如 30）
     * @param targetBps         目标码率（如 2000000 表示 2Mbps）
     * @param taskQueue         提供 NV12 图像数据的线程安全队列指针
     * @param bufferPool        NV12 图像数据的内存池
     * @param encodedCallback   编码输出 H.265 数据后的异步回调函数
     * @return true             初始化 MPP 硬件并启动线程成功
     * @return false            硬件初始化失败或参数无效
     */
    bool add_stream_task(int streamId, 
                         int imageWidth, 
                         int imageHeight, 
                         int frameRate, 
                         int targetBps,
                         ThreadSafeQueue<DmaBuffer_t*>* taskQueue,
                         DmaBufferPool* bufferPool,
                         OnEncodedCallback encodedCallback);

    /**
     * @brief 停止所有编码线程并清理所有底层分配的硬件资源
     */
    void stop_all_streams();

    // class enum StreamStatus {
    //     DISABLE = 0,
    //     ENABLE,
    // };

    /**
     * @brief 使能/失能特定编码转换流的工作
     * @param streamId 该路流的唯一标识 ID（例如：1 表示 720p，0 表示 1080p）
     * @param status true 使能/ DISABLE 失能
     */
    void ctrl_stream(int streamId, bool status);

    /**
     * @brief 使能/失能特定编码流的录像工作
     * @param streamId 该路流的唯一标识 ID（例如：1 表示 720p，0 表示 1080p）
     * @param status true 使能/ DISABLE 失能
     */
    void ctrl_record(int streamId, bool status);

    /**
     * @brief 初始化日志和线程池 (全局调一次即可)
     */
    static void init_zlm_env();

    /**
     * @brief 启动 RTSP / RTMP 监听
     */
    bool start_media_servers();

    /**
     * @brief 停止流媒体服务器 (完美解决生命周期析构问题)
     */
    void stop_media_servers();

private:
    /**
     * @struct StreamWorkerContext
     * @brief 内部结构体，保存每一路视频流的独立上下文和线程状态
     */
    struct StreamWorkerContext {
        int streamId_;                                      ///< 该路流的唯一标识
        int imageWidth_;                                    ///< 真实宽度
        int imageHeight_;                                   ///< 真实高度
        int horStride_;                                     ///< 16 字节对齐后的水平跨度
        int verStride_;                                     ///< 16 字节对齐后的垂直跨度
        size_t frameSize_;                                  ///< 一帧 NV12 占用的内存大小
        
        MppCtx mppCtx_;                                     ///< MPP 硬件上下文
        MppApi* mpi_;                                       ///< MPP 硬件调用接口
        
        ThreadSafeQueue<DmaBuffer_t*>* taskQueue_;          ///< 绑定的输入数据队列
        DmaBufferPool* bufferPool_;                         ///< 绑定的内存池
        OnEncodedCallback onEncodedCallback_;               ///< 编码完成回调函数
        
        std::thread workerThread_;                          ///< 处理该路视频流的独立线程
        std::atomic<bool> isRunning_;                       ///< 线程运行状态控制标志
        std::condition_variable pauseCv_;                   ///< 控制线程睡眠的条件变量
        std::mutex pauseMutex_;                             ///< 

        bool enableStream_ = false;                         ///< 是否使能编码转换流，初始化为失能
        bool enableRecord_ = false;
    };

    /**
     * @brief 内部线程的主循环函数，采用消费者模型阻塞提取任务队列
     * @param workerContext 当前工作线程绑定的上下文指针
     */
    void encoding_thread_loop(std::shared_ptr<StreamWorkerContext> workerContext);

private:
    std::vector<std::shared_ptr<StreamWorkerContext>> streamWorkers_; ///< 保存所有编码流上下文的容器
    USBHikvisioner* uhv_ = nullptr;

    /* ZLMediaKit 推流相关对象 (720p 专用) */
    std::shared_ptr<mediakit::DevChannel> zlmChannel720p_;
    toolkit::EventPoller::Ptr poller720p_;

    /* ZLMediaKit 录像相关对象 (1080p 专用) */
    std::shared_ptr<mediakit::DevChannel> zlmChannel1080p_;
    toolkit::EventPoller::Ptr poller1080p_;

    toolkit::TcpServer::Ptr rtspSrv_;
    toolkit::TcpServer::Ptr rtmpSrv_;
};

#endif // MPP_CONVERTER_H