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


AntennaVisioner::AntennaVisioner(const std::string& modelPath, 
                                const std::string& labelPath, int videoDeviceIndex)
    : modelPath_(modelPath), labelPath_(labelPath), videoDeviceIndex_(videoDeviceIndex) 
{
    /* 成员变量已有默认初始化值 (C++11/14 特性) */
}

AntennaVisioner::~AntennaVisioner() 
{
    if (rknnCtx_ > 0) {
        rknn_destroy(rknnCtx_);
        std::cout << "[AntennaVisioner] RKNN context destroyed." << std::endl;
    }
    if (cap_.isOpened()) cap_.release();

    deinitPostProcess();
}

bool AntennaVisioner::load_model_data(const std::string& filename, std::vector<unsigned char>& data) 
{
    /* 使用 std::ifstream (C++ 标准流) 代替 FILE* */
    std::ifstream ifs(filename, std::ios::binary | std::ios::ate); // ate: 打开即定位到末尾
    if (!ifs.is_open()) {
        std::cerr << "Failed to open model file: " << filename << std::endl;
        return false;
    }

    auto size = ifs.tellg();
    if (size <= 0) return false;

    ifs.seekg(0, std::ios::beg);
    
    data.resize(size); // vector 自动分配内存
    if (!ifs.read(reinterpret_cast<char*>(data.data()), size)) {
        std::cerr << "Failed to read model data." << std::endl;
        return false;
    }

    return true;
}

void AntennaVisioner::dump_tensor_attr(const rknn_tensor_attr& attr) 
{
    printf("  index=%d, name=%s, n_dims=%d, dims=[%d, %d, %d, %d], fmt=%s\n",
           attr.index, // 端口索引 - 通常单输入模型就是 0
           attr.name, // 节点名称
           attr.n_dims, // 维度数量 - 深度学习图像通常是 4 维
           attr.dims[0], // 一次处理几张图片
           attr.dims[1], // 图片的高度
           attr.dims[2], // 图片的宽度
           attr.dims[3], // 颜色通道数 - 3 代表 RGB 三通道
           get_format_string(attr.fmt)); // 数据排布格式 - 数据的内存排列方式
}

bool AntennaVisioner::init_system() 
{
    using namespace std::chrono; // 命名空间简化

    /* 1. 加载模型 */
    std::cout << "Loading model..." << std::endl;
    if (!load_model_data(modelPath_, modelData_))
        return false;

    /* 2. 初始化 RKNN */
    // data() 返回原生指针，size() 返回大小
    int ret = rknn_init(&rknnCtx_, modelData_.data(), modelData_.size(), 0, nullptr);
    if (ret < 0) {
        std::cerr << "rknn_init failed! ret=" << ret << std::endl;
        return false;
    }

    /* 3. 查询输入输出数量 */
    ret = rknn_query(rknnCtx_, RKNN_QUERY_IN_OUT_NUM, &ioNum_, sizeof(ioNum_));
    if (ret != RKNN_SUCC) return false;

    /* 4. 分配并查询属性 (C++14 make_unique) */
    // make_unique<T[]> 是 C++14 的重要特性，比 new T[] 更安全
    inputAttrs_ = std::make_unique<rknn_tensor_attr[]>(ioNum_.n_input);
    outputAttrs_ = std::make_unique<rknn_tensor_attr[]>(ioNum_.n_output);

    /* 4.1 查询输入属性 */
    std::cout << "Input Tensors:" << std::endl;
    for (int i = 0; i < ioNum_.n_input; ++i) {
        inputAttrs_[i].index = i;
        rknn_query(rknnCtx_, RKNN_QUERY_INPUT_ATTR, &(inputAttrs_[i]), sizeof(rknn_tensor_attr));
        dump_tensor_attr(inputAttrs_[i]);
    }

    /* 4.2 查询输出属性 */
    std::cout << "Output Tensors:" << std::endl;
    for (int i = 0; i < ioNum_.n_output; ++i) {
        outputAttrs_[i].index = i;
        rknn_query(rknnCtx_, RKNN_QUERY_OUTPUT_ATTR, &(outputAttrs_[i]), sizeof(rknn_tensor_attr));
        dump_tensor_attr(outputAttrs_[i]);
    }

    /* 5. 解析模型尺寸 */
    auto& attr0 = inputAttrs_[0]; 
    if (attr0.fmt == RKNN_TENSOR_NCHW) {
        modelChannel_ = attr0.dims[1]; // 得到输入图片的通道数
        modelWidth_   = attr0.dims[2]; // 得到输入图片的高
        modelHeight_  = attr0.dims[3]; // 得到输入图片的宽
    } else {
        modelWidth_   = attr0.dims[1];
        modelHeight_  = attr0.dims[2];
        modelChannel_ = attr0.dims[3];
    }
    std::cout << "Model Shape (W,H,C): " << modelWidth_ << "," << modelHeight_ << "," << modelChannel_ << std::endl;

    /* 6. 预分配 RGA 缓冲区 */
    resizeBuf_.resize(modelWidth_ * modelHeight_ * modelChannel_); // 其实就是一张输入图片的大小

    /* 7. 打开摄像头 */
    cap_.open(videoDeviceIndex_);
    if (!cap_.isOpened()) {
        std::cerr << "Cannot open camera /dev/video" << videoDeviceIndex_ << std::endl;
        return false;
    }

    set_label_path(labelPath_);

    return true;
}

bool AntennaVisioner::detect_once(cv::Mat& outputFrame) 
{
    int ret;
    
    #if TIME_CONM_CALC
    /* C++14 使用 std::chrono 进行高精度计时 */
    auto start_time = std::chrono::high_resolution_clock::now();
    #endif

    // if (!cap_.isOpened()) return false;

    cv::Mat origImg;
    if (!cap_.read(origImg) || origImg.empty()) {
        std::cerr << "Read pic data failed." << std::endl;
        return false;
    }

    /* --- RGA 硬件预处理 --- */
    // 获取原生指针进行 C API 调用
    int imgWidth = origImg.cols;
    int imgHeight = origImg.rows;

    rga_buffer_t src = wrapbuffer_virtualaddr(origImg.data, imgWidth, imgHeight, RK_FORMAT_BGR_888);
    rga_buffer_t dst = wrapbuffer_virtualaddr(resizeBuf_.data(), modelWidth_, modelHeight_, RK_FORMAT_RGB_888);
    
    /* 初始化矩形区域 (C++11/14 列表初始化) */
    im_rect srcRect = {0, 0, imgWidth, imgHeight};
    im_rect dstRect = {0, 0, modelWidth_, modelHeight_};

    /* 检查传入的参数是否符合 RGA 硬件的胃口 */
    if (imcheck(src, dst, srcRect, dstRect) != IM_STATUS_NOERROR) {
        std::cerr << "IMCheck failed" << std::endl;
        return false;
    }

    /* 硬件裁剪图片 */
    // 不管你摄像头的原始分辨率是 1920x1080 (1080p) 还是 1280x720 (720p)，
    // 在喂给 AI 之前，必须强行把图片缩放（Resize）到这个尺寸
    imresize(src, dst);

    /* --- RKNN 推理 --- */
    rknn_input inputs[1]; // 模型为单输入模型 
    memset(inputs, 0, sizeof(inputs)); // C API 结构体用 memset 清零
    inputs[0].index = 0;
    inputs[0].type = RKNN_TENSOR_UINT8;
    inputs[0].size = resizeBuf_.size();
    inputs[0].fmt = RKNN_TENSOR_NHWC;
    inputs[0].buf = resizeBuf_.data();

    rknn_inputs_set(rknnCtx_, ioNum_.n_input, inputs);

    ret = rknn_run(rknnCtx_, nullptr);
    if (ret < 0) {
        std::cerr << "rknn_run error: " << ret << std::endl;
        return false;
    }

    /* --- 获取输出 --- */
    std::vector<rknn_output> outputs(ioNum_.n_output);
    memset(outputs.data(), 0, sizeof(rknn_output) * ioNum_.n_output);
    
    // C++14 Lambda 表达式：用于自动释放 output 的 Scope Guard
    // 无论函数如何返回，析构时都会调用 release
    // 注意：这里仅作展示，实际 logic 比较简单，手动 release 也可以
    /* auto output_guard = [&](rknn_output* ptr) {
        rknn_outputs_release(rknnCtx_, ioNum_.n_output, ptr);
    };
    */

    ret = rknn_outputs_get(rknnCtx_, ioNum_.n_output, outputs.data(), nullptr);
    if (ret < 0) return false;

    /* --- 后处理 --- */
    /* 背景：
        摄像头进来的图片很大（例如 imgWidth = 1920）。
        模型需要的图片很小（例如 modelWidth_ = 640）。
        在推理前，我们把大图“压缩”成了小图喂给了 AI */
    // 计算缩放比例 (Coordinate Mapping)
    float scaleW = (float)modelWidth_ / imgWidth;
    float scaleH = (float)modelHeight_ / imgHeight;

    /* 准备反量化参数 */
    // ZP (Zero Point / 零点) 和 Scale (缩放因子) 是模型训练和转换时确定的“密码本”
    std::vector<float> outScales;
    std::vector<int32_t> outZps;
    outScales.reserve(ioNum_.n_output); // 预分配优化性能
    outZps.reserve(ioNum_.n_output);

    // 把每个输出层的“密码本”收集起来，交给后面的函数，
        // 让它能把 NPU 输出的 0~255 的整数翻译回真实的坐标和概率
    for (int i = 0; i < ioNum_.n_output; ++i) {
        outScales.push_back(outputAttrs_[i].scale);
        outZps.push_back(outputAttrs_[i].zp);
    }

    /* 执行核心后处理算法 */
    detect_result_group_t detectGroup;
    post_process((int8_t*)outputs[0].buf, (int8_t*)outputs[1].buf, (int8_t*)outputs[2].buf, // NPU 吐出来的原始数据
                 modelHeight_, modelWidth_, // 模型网格的大小
                 BOX_CONF_THRESHOLD, // 置信度阈值
                 NMS_THRESHOLD,  // 非极大值抑制 (NMS) 阈值
                 scaleW, scaleH, // 刚才算出来的比例，用于把框的坐标拉伸回原图大小
                 outZps, outScales, 
                 &detectGroup); // detectGroup 里面就装好了最终的识别列表（比如：检测到 2 个人，坐标在哪里，概率是多少）

    /* --- 绘制结果 --- */
    for (int i = 0; i < detectGroup.count; ++i) {
        // 使用 auto& 引用，避免拷贝
        auto& res = detectGroup.results[i];
        
        /* 获取目标框左上角、右下角坐标 */
        int x1 = res.box.left;
        int y1 = res.box.top;
        int x2 = res.box.right;
        int y2 = res.box.bottom;

        /* C++ string 格式化 */
        std::string label = std::string(res.name) + " " + std::to_string((int)(res.prop * 100)) + "%";

        cv::rectangle(origImg, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 255, 0), 2);
        cv::putText(origImg, label, cv::Point(x1, y1 - 5), 
                    cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 0, 255), 2);
    }

    /* 释放 RKNN 内部申请的输出 buffer */
    rknn_outputs_release(rknnCtx_, ioNum_.n_output, outputs.data());

    /* 输出图像 */
    outputFrame = origImg;

    #if TIME_CONM_CALC
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    std::cout << "Inference + Draw time: " << duration.count() << "ms" << std::endl;
    #endif

    return true;
}
