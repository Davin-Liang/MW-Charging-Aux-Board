#ifndef ANTENNA_VISIONER_H
#define ANTENNA_VISIONER_H

#include <string>
#include <vector>
#include <memory> // std::unique_ptr
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp> // VideoCapture
#include <mutex>

#include "yolov8.h"

// RKNN 和 RGA 头文件
#include "postprocess.h"
#include "rknn_api.h"
#include "image_utils.h"
#include "file_utils.h"
#include "image_drawing.h"

#include "dma_alloc.h"

struct DetectionResult {
    std::string className;
    int classId; 
    float prop; 
};

#define TIME_CONM_CALC 1

class AntennaVisioner {
public:
    /**
     * @brief 构造函数
     * @param modelPath RKNN模型路径
     * @param labelPath 标签文件路径
     * @param videoDeviceIndex 视频设备索引 (如 0 -> /dev/video0)
     */
    AntennaVisioner(const std::string& modelPath, 
                    const std::string& labelPath, 
                    int videoDeviceIndex);

    /**
     * @brief 构造函数
     * @param modelPath RKNN模型路径
     * @param labelPath 标签文件路径
     * @param width 图像宽度
     * @param height 图像高度
     */
    AntennaVisioner(const std::string& modelPath, 
                    const std::string& labelPath, 
                    const int width, 
                    const int height);

    /**
     * @brief 析构函数
     * 自动释放 vector 和 unique_ptr 管理的内存，手动释放 rknn_context
     */
    ~AntennaVisioner();

    /**
     * @brief 禁止拷贝和赋值 (Rule of Five)
     * 因为包含硬件上下文句柄，拷贝是不安全的
     */
    AntennaVisioner(const AntennaVisioner&) = delete;
    AntennaVisioner& operator=(const AntennaVisioner&) = delete;

    /**
     * @brief 初始化系统资源
     * @return true 成功, false 失败
     */
    bool init_system();

    /**
     * @brief 执行单次识别
     * @param outputFrame [输出] 绘制结果的图像 (BGR格式，可直接转Qt显示)
     * @return true 识别流程正常完成
     */
    bool detect_once(cv::Mat& outputFrame, std::vector<DetectionResult>& outResults);

    /**
     * @brief 返回类中用dma_alloc分配的图像
     */
    cv::Mat& get_rga_mat(void);

private:
    /* --- 配置常量 (C++14 constexpr) --- */
    /* --- 成员变量 --- */
    std::string modelPath_;
    std::string labelPath_;
    int videoDeviceIndex_;

    /* RKNN 上下文 */
    rknn_app_context_t rknnAppCtx_;

    std::mutex mtx_;

    cv::Mat rgaMat_;

    int width_;
    int height_;
    int bufSize_;
    
    /* DMA 相关资源 */
    int dmaFd_;
    void* virtAddr_;
    /* 摄像头对象 */
    cv::VideoCapture cap_;

    /* 是否使用海康威视摄像头 */
    bool useHikvision;
};

#endif // ANTENNA_VISIONER_H
