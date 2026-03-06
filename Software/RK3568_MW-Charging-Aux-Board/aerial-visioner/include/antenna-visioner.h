#pragma once 

#include <string>
#include <vector>
#include <memory> 
#include <mutex>

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp> 
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>

#include "yolov8.h"

// RKNN 和 RGA 头文件
#include "postprocess.h"
#include "rknn_api.h"
#include "image_utils.h"
#include "file_utils.h"
#include "image_drawing.h"

#include "dma_alloc.h"
#include "usb-hikvisioner.h"

/**
 * @brief 目标检测结果结构体
 */
struct DetectionResult {
    std::string className; ///< 类别名称 (如 "person", "car")
    int classId;           ///< 类别 ID
    float prop;            ///< 置信度 (Probability)
};

#define TIME_CONM_CALC 1

/**
 * @brief 天线视觉检测核心类
 * @details 封装了 RKNN 目标检测模型、OpenCV 图像获取以及海康相机的 DMA 零拷贝流转架构。
 */
class AntennaVisioner {
public:
    /**
     * @brief 构造函数 (普通 USB 相机模式)
     * @param modelPath RKNN 模型文件路径
     * @param labelPath 标签文件路径
     * @param videoDeviceIndex 视频设备索引 (如 0 对应 /dev/video0)
     */
    AntennaVisioner(const std::string& modelPath, 
                    const std::string& labelPath, 
                    int videoDeviceIndex);

    /**
     * @brief 构造函数 (海康威视 DMA 零拷贝模式)
     * @param modelPath RKNN 模型文件路径
     * @param labelPath 标签文件路径
     * @param width 图像宽度 (预期的模型输入或相机输出宽度)
     * @param height 图像高度
     * @param camera 外部传入的已初始化的海康相机实例指针
     */
    AntennaVisioner(const std::string& modelPath, 
                    const std::string& labelPath, 
                    int width,   // 规范修复 1：按值传递的基础类型(int)，声明时无需加 const
                    int height, 
                    USBHikvisioner* camera);

    /**
     * @brief 析构函数
     * @details 自动释放 vector 和 unique_ptr，手动释放 rknn_context、DMA 内存及相机对象。
     */
    ~AntennaVisioner();

    /**
     * @brief 禁止拷贝和赋值 (Rule of Five)
     * @details 因为包含硬件上下文句柄 (RKNN/DMA/OpenCV)，拷贝极度不安全。
     */
    AntennaVisioner(const AntennaVisioner&) = delete;
    AntennaVisioner& operator=(const AntennaVisioner&) = delete;

    /**
     * @brief 初始化系统资源
     * @details 根据构造模式加载 RKNN 模型，分配 DMA 内存或打开普通摄像头。
     * @return true 初始化成功 | false 初始化失败
     */
    bool init_system();

    /**
     * @brief 执行单次推理识别
     * @param outputFrame [输出] 绘制了检测结果框的图像 (BGR格式，可直接供前端/Qt显示)
     * @param outResults  [输出] 识别到的目标信息列表
     * @return true 识别流程正常完成 | false 队列为空或推理失败
     */
    bool detect_once(cv::Mat& outputFrame, std::vector<DetectionResult>& outResults);

    /**
     * @brief 返回类中用 dma_alloc 绑定的 OpenCV Mat 图像引用
     * @return cv::Mat& 图像引用
     */
    cv::Mat& get_rga_mat();

private:
    /* --- 成员变量 --- */
    std::string modelPath_;         ///< RKNN 模型路径
    std::string labelPath_;         ///< 标签文件路径
    int videoDeviceIndex_;          ///< 视频设备索引

    rknn_app_context_t rknnAppCtx_; ///< RKNN 模型运行上下文

    std::mutex mtx_;                ///< 线程互斥锁

    cv::Mat rgaMat_;                ///< 绑定 DMA 虚拟地址的 OpenCV Mat

    int width_;                     ///< 图像宽度
    int height_;                    ///< 图像高度
    int bufSize_;                   ///< DMA 缓冲区总字节大小
    
    /* DMA 相关资源 */
    int dmaFd_;                     ///< DMA 内存文件描述符
    void* virtAddr_;                ///< DMA 虚拟地址指针
    
    cv::VideoCapture cap_;          ///< OpenCV 普通摄像头对象

    bool useHikvision_;             ///< 是否使用海康威视摄像头
    USBHikvisioner* uhv_ = nullptr; ///< 海康相机实例指针
};