#include "antenna-visioner.h"
#include <iostream>
#include <fstream>
#include <chrono> // C++11/14 时间库

// Rockchip 硬件加速库
#include "im2d.h"
#include "RgaUtils.h"
#include "rga.h"
#include "opencv2/core.hpp"
#include "opencv2/videoio.hpp" // VideoCapture
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/opencv.hpp"
#include "postprocess.h"


AntennaVisioner::AntennaVisioner(const std::string& modelPath, 
                                const std::string& labelPath, 
                                int videoDeviceIndex)
    : modelPath_(modelPath), 
      labelPath_(labelPath), 
      videoDeviceIndex_(videoDeviceIndex) 
{
    /* 成员变量已有默认初始化值 (C++11/14 特性) */
    memset(&rknnAppCtx_, 0, sizeof(rknn_app_context_t));
}

AntennaVisioner::~AntennaVisioner() 
{
    int ret;

    deinit_post_process();
    ret = release_yolov8_model(&rknnAppCtx_);
    if (ret != 0)
        printf("release_yolov8_model fail! ret=%d\n", ret);

    // if (cap_.isOpened()) cap_.release();
}

bool AntennaVisioner::init_system() 
{
    int ret;

    set_label_path(labelPath_);
    ret = init_post_process();
    if (ret != 0) {
        printf("Fail to initial post process.\n");
        return false;
    }

    ret = init_yolov8_model(modelPath_.c_str(), &rknnAppCtx_);
    if (ret != 0) {
        printf("Initial yolov8 model failed! ret = %d model_path = %s\n", ret, modelPath_);
        goto fail_to_init_model;
    }

    /* 打开摄像头 */
    // cap_.open(videoDeviceIndex_);
    // if (!cap_.isOpened()) {
    //     std::cerr << "Cannot open camera /dev/video" << videoDeviceIndex_ << std::endl;
    //     goto fail_to_open_camera;
    // }

    // /* 设置摄像头分辨率 (建议与模型输入尺寸接近，提高效率) */
    // cap_.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    // cap_.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

    return true;

fail_to_init_model:
    deinit_post_process();
    return false;

fail_to_open_camera:
    deinit_post_process();
    ret = release_yolov8_model(&rknnAppCtx_);
    if (ret != 0)
        printf("Release yolov8 model failed! ret = %d\n", ret);
    return false;
}

bool AntennaVisioner::detect_once(cv::Mat& outputFrame) 
{
    int ret;

    #if TIME_CONM_CALC
    /* C++14 使用 std::chrono 进行高精度计时 */
    auto start_time = std::chrono::high_resolution_clock::now();
    #endif
    
    cv::Mat origImg;
    image_buffer_t ibImage;

    memset(&ibImage, 0, sizeof(image_buffer_t));

    origImg = outputFrame;
    if (origImg.empty()) {
        std::cerr << "Read pic data failed." << std::endl;
        return false;
    }

    /* 格式转换: OpenCV 默认是 BGR，RKNN/YOLO 通常需要 RGB */
    cv::cvtColor(origImg, origImg, cv::COLOR_BGR2RGB);

    /* 填充 src_image 结构体信息 */
    ibImage.width = origImg.cols;
    ibImage.height = origImg.rows;
    ibImage.format = IMAGE_FORMAT_RGB888;
    ibImage.size = origImg.total() * origImg.elemSize(); // 宽*高*通道数

    /* [普通路径]: 直接使用 OpenCV 的内存指针 */
    // 注意：origImg 必须在 inference 结束前保持存活
    ibImage.virt_addr = origImg.data;
    ibImage.fd = -1; // 无 dma fd

    /* 推理与后处理 */
    object_detect_result_list odResults;
    ret = inference_yolov8_model(&rknnAppCtx_, &ibImage, &odResults);
    if (ret != 0) {
        printf("inference fail! ret=%d\n", ret);
        return false; // 跳过本帧后续处理，开始处理下一帧
    }

   /* 画框 (画在 ibImage 上，也就是画在了 origImg 的数据里 */
    // 注意：如果之前转成了 RGB，OpenCV显示时可能颜色不对，可以选择再转回 BGR 或者直接画
    for (int i = 0; i < odResults.count; i++) {
        object_detect_result *detResult = &(odResults.results[i]);
        
        /* 简单的终端打印 */
        printf("%s @ (%.2f) \n", coco_cls_to_name(detResult->cls_id), detResult->prop);

        int x1 = detResult->box.left;
        int y1 = detResult->box.top;
        int x2 = detResult->box.right;
        int y2 = detResult->box.bottom;

        /* 调用你原本的画图函数 */
        draw_rectangle(&ibImage, x1, y1, x2 - x1, y2 - y1, COLOR_BLUE, 3);
        
        char text[256];
        sprintf(text, "%s %.1f%%", coco_cls_to_name(detResult->cls_id), detResult->prop * 100);
        draw_text(&ibImage, text, x1, y1 - 20, COLOR_RED, 10);
    }

    /* 将 RGB 转回 BGR 以便正确显示颜色 */
    cv::Mat showFrame(ibImage.height, ibImage.width, CV_8UC3, ibImage.virt_addr);
    cv::cvtColor(showFrame, showFrame, cv::COLOR_RGB2BGR);

    /* 输出图像 */
    outputFrame = origImg;

    #if TIME_CONM_CALC
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    std::cout << "Inference + Draw time: " << duration.count() << "ms" << std::endl;
    #endif
    
    return true;
}
