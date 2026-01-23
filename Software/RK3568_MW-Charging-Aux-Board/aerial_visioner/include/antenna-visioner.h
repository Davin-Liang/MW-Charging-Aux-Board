/**
 * @file AntennaVisioner.h
 * @brief 天线视觉识别器类定义 (C++14 Modernized)
 */

#ifndef ANTENNA_VISIONER_H
#define ANTENNA_VISIONER_H

#include <string>
#include <vector>
#include <memory> // std::unique_ptr
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp> // VideoCapture
#include <mutex>

#define USE_YOLOV8

#include "yolov8.h"

// RKNN 和 RGA 头文件
#ifdef USE_YOLOV8
#include "yolov8/postprocess.h"
#else
#include "include/yolov5/postprocess.h"
#endif
#include "rknn_api.h"
#include "image_utils.h"
#include "file_utils.h"
#include "image_drawing.h"


#define TIME_CONM_CALC 1

class AntennaVisioner {
public:
    /**
     * @brief 构造函数
     * @param modelPath RKNN模型路径
     * @param videoDeviceIndex 视频设备索引 (如 0 -> /dev/video0)
     */
    AntennaVisioner(const std::string& modelPath, const std::string& labelPath, int videoDeviceIndex);

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
    bool detect_once(cv::Mat& outputFrame);

private:
    /* --- 内部辅助方法 --- */
    /**
     * @brief 加载模型数据
     * @param filename 文件名
     * @param data 模型数据输出
     */
    bool load_model_data(const std::string& filename, std::vector<unsigned char>& data);
    /**
     * @brief 打印张量属性值-RKNN 模型输入或输出“张量（Tensor）”的元数据（身份证信息）
     */
    void dump_tensor_attr(const rknn_tensor_attr& attr);

private:
    /* --- 配置常量 (C++14 constexpr) --- */
    static constexpr float NMS_THRESHOLD = 0.45f;      // 对应原宏 NMS_THRESH
    static constexpr float BOX_CONF_THRESHOLD = 0.25f; // 对应原宏 BOX_THRESH

    /* --- 成员变量 --- */
    std::string modelPath_;
    std::string labelPath_;
    int videoDeviceIndex_;

    /* RKNN 上下文 (句柄本质是 uint64，非指针，需手动管理 destroy) */
    // rknn_context rknnCtx_ = 0;
    rknn_app_context_t rknnAppCtx_;

    /* 内存资源 */
    std::vector<unsigned char> modelData_; // 模型数据
    
    /* RGA 缩放缓冲区 (喂给 NPU 的数据) */
    std::vector<unsigned char> resizeBuf_; 

    /* RKNN 属性 */
    std::unique_ptr<rknn_tensor_attr[]> inputAttrs_;
    std::unique_ptr<rknn_tensor_attr[]> outputAttrs_;
    rknn_input_output_num ioNum_;

    /* 模型维度参数 */
    // AI 模型不像人眼那样灵活（既能看大电视也能看小手机）。在模型训练好并导出后，它的输入尺寸是写死固定的
    int modelWidth_ = 0; // 输入尺寸之宽
    int modelHeight_ = 0; // 输入尺寸之高
    int modelChannel_ = 3; // 3 代表 RGB 彩色图

    std::mutex mtx_;

    /* 摄像头对象 */
    cv::VideoCapture cap_;
};

#endif // ANTENNA_VISIONER_H
