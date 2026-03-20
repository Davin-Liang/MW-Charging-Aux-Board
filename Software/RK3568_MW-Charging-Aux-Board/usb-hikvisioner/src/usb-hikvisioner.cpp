#include "usb-hikvisioner.h"
#include <unistd.h>
#include "dma-buffer-pool.h"
#include <thread>
#include "im2d.h"
#include "drmrga.h"
#include <algorithm>
#include <chrono>
// #include "image_utils.h"

USBHikvisioner::USBHikvisioner(int cameraDeviceIndex) 
:cameraDeviceIndex_(cameraDeviceIndex),
 deviceHandle_(nullptr), // 规范：NULL -> nullptr
 nPayloadSize_(0),
 cameraStatus_(CameraStatus::WAIT_FOR_INIT),
 openGrabCallback_(false) {}

USBHikvisioner::~USBHikvisioner()
{
    camera_close();

    /* 销毁句柄 */
    if (deviceHandle_) {
        MV_CC_DestroyHandle(deviceHandle_);
        deviceHandle_ = nullptr;
    }
} 

bool USBHikvisioner::camera_init()
{
    int ret = MV_OK;

    if (cameraStatus_ != CameraStatus::WAIT_FOR_INIT) {
        printf("Camera has been initialled.\n");
        return false;
    }

    /* 设备枚举 */
    MV_CC_DEVICE_INFO_LIST stDeviceList;
    memset(&stDeviceList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));
    ret = MV_CC_EnumDevices(MV_USB_DEVICE, &stDeviceList);
    if (MV_OK != ret) {
        printf("Enum Devices fail! ret: [0x%x].\n", ret);
        return false;
    }
    
    /* 显示设备信息 */
    if (stDeviceList.nDeviceNum > 0) {
        for (int i = 0; i < stDeviceList.nDeviceNum; i++) { // 遍历每个device
            printf("[device %d]:\n", i);
            MV_CC_DEVICE_INFO* pDeviceInfo = stDeviceList.pDeviceInfo[i];
            if (nullptr == pDeviceInfo) {
                printf("Fail to get current device info.\n");
                return false;
            }
            // print_device_info_(*pDeviceInfo);
        }
    } else {
        printf("Find No Devices!\n");
        return false;
    }    

    /* 检查设备是否可访问 */
    if (cameraDeviceIndex_ >= 0 && cameraDeviceIndex_ < stDeviceList.nDeviceNum) {
        if (false == MV_CC_IsDeviceAccessible(stDeviceList.pDeviceInfo[cameraDeviceIndex_], 
                                                MV_ACCESS_Exclusive)) {
            printf("Can't connect! Please check the camera index value entered during instantiation.\n"); 
            return false;
        }
    } else {
        printf("The camera index is invalid.\n");
        return false;
    }

    /* 创建设备句柄 */
    ret = MV_CC_CreateHandle(&deviceHandle_, stDeviceList.pDeviceInfo[cameraDeviceIndex_]);
    if (MV_OK != ret) {
        printf("Create Handle fail! ret: [0x%x]\n.", ret);
        return false;
    }

    cameraStatus_ = CameraStatus::INITED;
    return true;
}

bool USBHikvisioner::camera_open()
{
    int ret = MV_OK;

    if (cameraStatus_ == CameraStatus::OPENED) {
        printf("Camera has been opened. No need to open again.\n");
        return true;
    }

    if (cameraStatus_ == CameraStatus::WAIT_FOR_INIT) {
        printf("Camera needs to be initialized first.\n");
        return false;
    }
    
    /* 打开设备 */
    ret = MV_CC_OpenDevice(deviceHandle_);
    if (MV_OK != ret) {
        printf("Open Device fail! ret: [0x%x]\n.", ret);
        return false;
    }
    
    // ---------------------------------------------------------
    // 【关键新增】：强制设置相机的输出像素格式为 YUV422 (YUYV)
    // ---------------------------------------------------------
    // PixelType_Gvsp_YUV422_Packed 的宏定义在 PixelType.h 中
    ret = MV_CC_SetEnumValue(deviceHandle_, "PixelFormat", PixelType_Gvsp_YUV422_YUYV_Packed);
    if (MV_OK != ret) {
        // 有些海康相机型号叫做 YUYV_Packed
        // ret = MV_CC_SetEnumValue(deviceHandle_, "PixelFormat", PixelType_Gvsp_YUYV_Packed);
        // if (MV_OK != ret) {
        //      printf("[Warning] Failed to force PixelFormat to YUV422! Current format: 0x%x\n", stImageInfo_.enPixelType);
        //      printf("[Warning] This will likely cause RGA color distortion (Green/Pink screen).\n");
        // } else {
        //      printf("[Config] Set PixelFormat to YUYV_Packed Success.\n");
        // }
    } else {
        printf("[Config] Set PixelFormat to YUV422_Packed Success.\n");
    }

    /* 设置自动增益 (Auto Gain) -> Continuous (2) */
    ret = MV_CC_SetEnumValue(deviceHandle_, "GainAuto", 2);
    if (MV_OK != ret) {
        printf("[Warning] Set Auto Gain Fail, trying manual gain 10dB...\n");
        MV_CC_SetEnumValue(deviceHandle_, "GainAuto", 0); // Off
        MV_CC_SetFloatValue(deviceHandle_, "Gain", 10.0f);
    } else {
        printf("[Config] Set Auto Gain Success.\n");
    }

    /* 设置自动曝光 (Auto Exposure) -> Continuous (2) */
    ret = MV_CC_SetEnumValue(deviceHandle_, "ExposureAuto", 2);
    if (MV_OK != ret) {
        printf("[Warning] Set Auto Exposure Fail, trying manual exposure 50ms...\n");
        MV_CC_SetEnumValue(deviceHandle_, "ExposureAuto", 0); // Off
        MV_CC_SetFloatValue(deviceHandle_, "ExposureTime", 50000.0f); 
    } else {
        printf("[Config] Set Auto Exposure Success.\n");
    }

    // 3. 稍微等待一下让自动算法收敛（变亮）
    usleep(500000); // 等待 0.5 秒

    /* 关闭触发模式 */
    ret = MV_CC_SetEnumValue(deviceHandle_, "TriggerMode", 0);
    if (MV_OK != ret) {
        printf("Set Trigger Mode fail! ret: [0x%x]\n.", ret);
        return false;
    } 
    
    /* --- 设置 Binning (降低分辨率，提高帧率) --- */
    ret = MV_CC_SetEnumValue(deviceHandle_, "BinningHorizontal", 2);
    if (MV_OK != ret) {
        printf("Set BinningH fail! ret: [0x%x]\n", ret);
        return false;
    }

    ret = MV_CC_SetEnumValue(deviceHandle_, "BinningVertical", 2);
    if (MV_OK != ret) {
        printf("Set BinningV fail! ret: [0x%x]\n", ret);
        return false;
    }

    /* 获取图像大小 */
    MVCC_INTVALUE stParam;
    memset(&stParam, 0, sizeof(MVCC_INTVALUE));
    ret = MV_CC_GetIntValue(deviceHandle_, "PayloadSize", &stParam);
    if (MV_OK != ret) {
        printf("Get PayloadSize fail! ret: [0x%x]\n.", ret);
        return false;
    }
    nPayloadSize_ = stParam.nCurValue;
    printf("PayloadSize = %d.\n", nPayloadSize_);

    // 【新增】：动态获取相机当前的真实宽、高
    MVCC_INTVALUE stParamWidth;
    memset(&stParamWidth, 0, sizeof(MVCC_INTVALUE));
    MV_CC_GetIntValue(deviceHandle_, "Width", &stParamWidth);
    int realWidth = stParamWidth.nCurValue;

    MVCC_INTVALUE stParamHeight;
    memset(&stParamHeight, 0, sizeof(MVCC_INTVALUE));
    MV_CC_GetIntValue(deviceHandle_, "Height", &stParamHeight);
    int realHeight = stParamHeight.nCurValue;
    
    printf("Real Camera Output: Width = %d, Height = %d, Payload = %d\n", 
            realWidth, realHeight, nPayloadSize_);

    /* 初始化图像信息 */
    memset(&stImageInfo_, 0, sizeof(MV_FRAME_OUT_INFO_EX));
    pData_ = std::make_unique<uint8_t[]>(nPayloadSize_); // 默认置零
    if (nullptr == pData_) {
        printf("Allocate memory failed.\n");
        return false;
    }

    /* 注册抓图回调 */
    if (openGrabCallback_) {
        if (!sourcePool.alloc_pool(4, realWidth, realHeight, BufferFormat::YUV422)) return false;
        if (!yoloPool.alloc_pool(4, 640, 640, BufferFormat::RGB888)) return false;
        if (!pushFlowPool.alloc_pool(4, 1280, 720, BufferFormat::NV12)) return false;
        if (!v1080pPool.alloc_pool(4, 1920, 1088, BufferFormat::NV12)) return false;

        start_rga_thread();

        ret = MV_CC_RegisterImageCallBackEx2(deviceHandle_, image_callback_ex2, 
                                            this, true);
        if (MV_OK != ret) {
            printf("MV_CC_RegisterImageCallBackEx fail! nRet [%x]\n", ret);
            stop_rga_thread();
            sourcePool.destroy_pool();
            yoloPool.destroy_pool();
            pushFlowPool.destroy_pool();
            v1080pPool.destroy_pool();
            MV_CC_CloseDevice(deviceHandle_);
            return false; 
        }
    }

    /* 【新增修复】：强制限制相机的采集帧率，防止撑爆 USB 总线带宽 */
    // MV_CC_SetBoolValue(deviceHandle_, "AcquisitionFrameRateEnable", true);
    // MV_CC_SetFloatValue(deviceHandle_, "AcquisitionFrameRate", 10.0f);  

    /* 开始取流 */
    ret = MV_CC_StartGrabbing(deviceHandle_);
    if (MV_OK != ret) {
        printf("Start Grabbing fail! ret: [0x%x].\n", ret);
        if (openGrabCallback_) {    
            stop_rga_thread();
            sourcePool.destroy_pool();
            yoloPool.destroy_pool();
            pushFlowPool.destroy_pool();
            v1080pPool.destroy_pool();
            MV_CC_CloseDevice(deviceHandle_);
        }
        return false;
    }

    cameraStatus_ = CameraStatus::OPENED;
    return true;
}

bool USBHikvisioner::camera_close()
{
    int ret;

    if (cameraStatus_ == CameraStatus::CLOSED) {
        printf("Camera has been closed. No need to close again.\n");
        return true;
    }

    if (cameraStatus_ == CameraStatus::WAIT_FOR_INIT || cameraStatus_ == CameraStatus::INITED) {
        printf("Camera hasn't been opened.\n");
        return false;
    }
    
    /* 停止取流 */
    ret = MV_CC_StopGrabbing(deviceHandle_);
    if (MV_OK != ret) {
        printf("Stop Grabbing fail! ret: [0x%x]\n.", ret);
        return false;
    }

    /* 关闭设备 */
    ret = MV_CC_CloseDevice(deviceHandle_);
    if (MV_OK != ret) {
        printf("ClosDevice fail! ret: [0x%x]\n.", ret);
        return false;
    }    

    if (openGrabCallback_) {
        /* 先让 RGA 消费者线程安全停止 */
        stop_rga_thread();

        /* 清空队列中残留的指针，防止下次启动时串线 */
        {
            std::lock_guard<std::mutex> lock(queueMutex_);
            while(!rgaTaskQueue_.empty()) rgaTaskQueue_.pop();
        }

        /* 线程彻底死透了，现在可以安全地拆除物理内存池了 */
        sourcePool.destroy_pool();
        yoloPool.destroy_pool();
        pushFlowPool.destroy_pool();
        v1080pPool.destroy_pool();
    }

    cameraStatus_ = CameraStatus::CLOSED;
    return true;
}

bool USBHikvisioner::read_img(cv::Mat& img, unsigned int nMsec)
{
    int ret;

    if (cameraStatus_ != CameraStatus::OPENED) {
        printf("Camera needs to been opened.\n");
        return false;
    }
    
    /* 获取一帧图像，超时时间1000ms */
    ret = MV_CC_GetOneFrameTimeout(deviceHandle_, pData_.get(), nPayloadSize_, &stImageInfo_, nMsec);
    if (MV_OK != ret) {
        printf("Get Frame fail! ret: [0x%x]\n.", ret);
        return false;
    }
    printf("Get One Frame: Width[%d], Height[%d], FrameNum[%d]\n",
    stImageInfo_.nWidth, stImageInfo_.nHeight, stImageInfo_.nFrameNum);

    /* 格式转换 */
    return convert_to_mat(stImageInfo_, pData_, img);
}

bool USBHikvisioner::convert_to_mat(MV_FRAME_OUT_INFO_EX& stImageInfo, 
                                    std::unique_ptr<uint8_t[]>& pData,
                                    cv::Mat& dstImg)
{
    if (!pData) {
        printf("Data is null.\n");
        return false;
    }
    
    unsigned int nRGBSize = stImageInfo.nWidth * stImageInfo.nHeight * 3;
    
    MV_CC_PIXEL_CONVERT_PARAM stConvertParam = {0};
    stConvertParam.nWidth = stImageInfo.nWidth;
    stConvertParam.nHeight = stImageInfo.nHeight;
    stConvertParam.pSrcData = pData.get();                  
    stConvertParam.nSrcDataLen = stImageInfo.nFrameLen;     
    stConvertParam.enSrcPixelType = stImageInfo.enPixelType;
    
    stConvertParam.enDstPixelType = PixelType_Gvsp_RGB8_Packed; 
    
    stConvertParam.pDstBuffer = dstImg.data;                
    stConvertParam.nDstBufferSize = nRGBSize;

    int ret = MV_CC_ConvertPixelType(deviceHandle_, &stConvertParam);
    if (MV_OK != ret) {
        printf("Convert Pixel Type fail! ret: [0x%x], Input Format: [0x%x]\n", ret, stImageInfo.enPixelType);
        return false;
    }

    return true;
}

void __stdcall USBHikvisioner::image_callback_ex2(MV_FRAME_OUT* pstFrame, void *pUser, bool bAutoFree) 
{
    USBHikvisioner* pThis = static_cast<USBHikvisioner*>(pUser); 

    DmaBuffer_t* buffer = pThis->sourcePool.get_buffer();
    if (buffer == nullptr) return;

    auto start_time = std::chrono::high_resolution_clock::now();
    memcpy(buffer->virtAddr, pstFrame->pBufAddr, pstFrame->stFrameInfo.nFrameLen);
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    std::cout << "[time]: 拷贝海康一帧原始数据的时间为: " << duration.count() << "ms." << std::endl;

    {
        std::lock_guard<std::mutex> lock(pThis->queueMutex_);
        pThis->rgaTaskQueue_.push(buffer);
    }
    pThis->queueCv_.notify_one(); 
}

void USBHikvisioner::open_grab_callback()
{
    openGrabCallback_ = true;
}

void USBHikvisioner::rga_dispatch_thread_func()
{
    while (isRunning_) {
        DmaBuffer_t* sourceBuffer = nullptr;

        /* --- 核心阻塞区：等待海康回调投递任务 --- */
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            
            queueCv_.wait(lock, [this]{ 
                return !rgaTaskQueue_.empty() || !isRunning_; 
            });
            
            if (!isRunning_ && rgaTaskQueue_.empty()) break; 

            sourceBuffer = rgaTaskQueue_.front();
            rgaTaskQueue_.pop();
        }

        /* --- 开始干活：调用 RGA 硬件进行分发 --- */
        if (sourceBuffer != nullptr) {
            DmaBuffer_t* yoloBuffer = yoloPool.get_buffer();
            
            if (yoloBuffer != nullptr) {
                /* 只有获取到 yolo 内存，且 RGA 转换成功时，才入队 */
                auto start_time = std::chrono::high_resolution_clock::now();
                if (rga_process_to_rgb(sourceBuffer, yoloBuffer)) {
                    std::lock_guard<std::mutex> lock(queueMutex_);
                    yoloTaskQueue.push(yoloBuffer);
                } else {
                    /* 如果 RGA 转换失败，要把 yoloBuffer 还回去，防止内存流失 */
                    yoloPool.release_buffer(yoloBuffer);
                }
                auto end_time = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
                std::cout << "[time]: YUV422转到RGB888所需要的时间为: " << duration.count() << "ms." << std::endl;
            } else {
                printf("[Warning] YOLO Buffer Pool is empty! Drop 1 frame.\n");
            }

            DmaBuffer_t* pushFlowBuffer = pushFlowPool.get_buffer();

            if (pushFlowBuffer != nullptr) {
                /* 只有获取到 yolo 内存，且 RGA 转换成功时，才入队 */
                auto start_time = std::chrono::high_resolution_clock::now();
                if (rga_process_to_nv12(sourceBuffer, pushFlowBuffer)) {
                    std::lock_guard<std::mutex> lock(queueMutex_);
                    pushFlowTaskQueue.push(pushFlowBuffer);
                } else {
                    /* 如果 RGA 转换失败，要把 yoloBuffer 还回去，防止内存流失 */
                    pushFlowPool.release_buffer(pushFlowBuffer);
                }
                auto end_time = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
                std::cout << "[time]: YUV422转到NV12 1280*720 所需要的时间为: " << duration.count() << "ms." << std::endl;
            } else {
                printf("[Warning] Push-Flow Buffer Pool is empty! Drop 1 frame.\n");
            }

            DmaBuffer_t* v1080pBuffer = v1080pPool.get_buffer();

            if (v1080pBuffer != nullptr) {
                /* 只有获取到1080p内存，且 RGA 转换成功时，才入队 */
                auto start_time = std::chrono::high_resolution_clock::now();
                if (rga_process_to_nv12(sourceBuffer, v1080pBuffer)) {
                    std::lock_guard<std::mutex> lock(queueMutex_); // hereTODO:
                    v1080pTaskQueue.push(v1080pBuffer);
                } else {
                    /* 如果 RGA 转换失败，要把 yoloBuffer 还回去，防止内存流失 */
                    v1080pPool.release_buffer(v1080pBuffer);
                }
                auto end_time = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
                std::cout << "[time]: YUV422转到NV12 1920*1080 所需要的时间为: " << duration.count() << "ms." << std::endl;
            } else {
                printf("[Warning] 1080p Buffer Pool is empty! Drop 1 frame.\n");
            }

            /* 无论如何，源头 Buffer 用完了必须还回去 */
            sourcePool.release_buffer(sourceBuffer);
        }
    }
}

bool USBHikvisioner::start_rga_thread()
{
    if (isRunning_) {
        printf("RGA 线程已经在运行了！\n");
        return true; 
    }

    isRunning_ = true;

    rgaThread_ = std::thread(&USBHikvisioner::rga_dispatch_thread_func, this);
    
    printf("RGA 调度线程启动成功！\n");
    return true;
}

void USBHikvisioner::stop_rga_thread()
{
    if (!isRunning_) return;

    isRunning_ = false;

    queueCv_.notify_all(); 

    if (rgaThread_.joinable())
        rgaThread_.join(); 

    printf("RGA 调度线程已安全停止并回收。\n");
}

bool USBHikvisioner::rga_process_to_rgb(DmaBuffer_t* srcBuf, DmaBuffer_t* dstBuf) 
{
    if (!srcBuf || srcBuf->dmaFd <= 0 || !dstBuf || dstBuf->dmaFd <= 0) {
        std::cerr << "[RGA Error] Invalid DMA fd!" << std::endl;
        return false;
    }

    bool ret = true;
    IM_STATUS ret_rga = IM_STATUS_NOERROR;
    
    /* RGA 句柄 */
    rga_buffer_handle_t rga_handle_src = 0;
    rga_buffer_handle_t rga_handle_dst = 0;

    /* 格式定义：源头是海康的 YUYV，目标是 YOLO 的 RGB888 */
    int srcFmt = RK_FORMAT_YUYV_422; 
    int dstFmt = RK_FORMAT_RGB_888;

    /* 1. 导入 DMA Fd 生成 RGA Handle */
    im_handle_param_t in_param = { srcBuf->width, srcBuf->height, srcFmt };
    rga_handle_src = importbuffer_fd(srcBuf->dmaFd, &in_param);
    if (rga_handle_src <= 0) {
        std::cerr << "[RGA Error] Failed to import src fd!" << std::endl;
        return false;
    }

    im_handle_param_t dst_param = { dstBuf->width, dstBuf->height, dstFmt };
    rga_handle_dst = importbuffer_fd(dstBuf->dmaFd, &dst_param);
    if (rga_handle_dst <= 0) {
        std::cerr << "[RGA Error] Failed to import dst fd!" << std::endl;
        releasebuffer_handle(rga_handle_src); // 错误处理：释放已申请的源句柄
        return false;
    }

    /* 2. 包装 RGA Buffer */
    rga_buffer_t rga_buf_src = wrapbuffer_handle(rga_handle_src, srcBuf->width, srcBuf->height, 
                                                srcFmt, srcBuf->width, srcBuf->height);
    rga_buffer_t rga_buf_dst = wrapbuffer_handle(rga_handle_dst, dstBuf->width, dstBuf->height,
                                                dstFmt, dstBuf->width, dstBuf->height);

    /* 3. 计算 Letterbox (等比例缩放) 参数 */
    float scale = std::min((float)dstBuf->width / srcBuf->width, 
                            (float)dstBuf->height / srcBuf->height);
    int scaled_w = srcBuf->width * scale;
    int scaled_h = srcBuf->height * scale;
    int offset_x = (dstBuf->width - scaled_w) / 2;
    int offset_y = (dstBuf->height - scaled_h) / 2;

    im_rect srect = {0, 0, srcBuf->width, srcBuf->height};
    im_rect drect = {offset_x, offset_y, scaled_w, scaled_h};

    /* 4. 背景填充 (Padding) */
    if (scaled_w != dstBuf->width || scaled_h != dstBuf->height) {
        im_rect dst_whole_rect = {0, 0, dstBuf->width, dstBuf->height};
        ret_rga = imfill(rga_buf_dst, dst_whole_rect, 0xFF727272); 
        if (ret_rga <= 0 && dstBuf->virtAddr != nullptr)
            memset(dstBuf->virtAddr, 114, dstBuf->bufferSize);
    }

    /* 5. RGA 终极处理：瞬间完成 格式转换 + 等比例缩放 + 偏移写入 */
    rga_buffer_t pat; memset(&pat, 0, sizeof(rga_buffer_t));
    im_rect prect; memset(&prect, 0, sizeof(im_rect));
    
    ret_rga = improcess(rga_buf_src, rga_buf_dst, pat, srect, drect, prect, 0);
    if (ret_rga <= 0) {
        std::cerr << "[RGA Error] improcess failed: " << imStrError((IM_STATUS)ret_rga) << std::endl;
        ret = false;
    }

    /* 6. 释放 RGA 句柄 (必须执行，否则内核内存泄漏！) */
    releasebuffer_handle(rga_handle_src);
    releasebuffer_handle(rga_handle_dst);

    return ret;
}

bool USBHikvisioner::rga_process_to_nv12(DmaBuffer_t* srcBuf, DmaBuffer_t* dstBuf) 
{
    if (!srcBuf || srcBuf->dmaFd <= 0 || !dstBuf || dstBuf->dmaFd <= 0) {
        std::cerr << "[RGA Error] Invalid DMA fd!" << std::endl;
        return false;
    }

    bool ret = true;
    IM_STATUS ret_rga = IM_STATUS_NOERROR;
    
    /* RGA 句柄 */
    rga_buffer_handle_t rga_handle_src = 0;
    rga_buffer_handle_t rga_handle_dst = 0;

    /* 格式定义：源头是海康的 YUYV，目标是 NV12 (YUV420SP) */
    int srcFmt = RK_FORMAT_YUYV_422; 
    int dstFmt = RK_FORMAT_YCbCr_420_SP; // RGA 中的 NV12 格式宏

    /* 1. 导入 DMA Fd 生成 RGA Handle */
    im_handle_param_t in_param = { srcBuf->width, srcBuf->height, srcFmt };
    rga_handle_src = importbuffer_fd(srcBuf->dmaFd, &in_param);
    if (rga_handle_src <= 0) {
        std::cerr << "[RGA Error] Failed to import src fd!" << std::endl;
        return false;
    }

    im_handle_param_t dst_param = { dstBuf->width, dstBuf->height, dstFmt };
    rga_handle_dst = importbuffer_fd(dstBuf->dmaFd, &dst_param);
    if (rga_handle_dst <= 0) {
        std::cerr << "[RGA Error] Failed to import dst fd!" << std::endl;
        releasebuffer_handle(rga_handle_src); // 错误处理：释放已申请的源句柄
        return false;
    }

    /* 2. 包装 RGA Buffer */
    rga_buffer_t rga_buf_src = wrapbuffer_handle(rga_handle_src, srcBuf->width, srcBuf->height, 
                                                srcFmt, srcBuf->width, srcBuf->height);
    rga_buffer_t rga_buf_dst = wrapbuffer_handle(rga_handle_dst, dstBuf->width, dstBuf->height,
                                                dstFmt, dstBuf->width, dstBuf->height);

    /* 3. 计算 Letterbox (等比例缩放) 参数 */
    float scale = std::min((float)dstBuf->width / srcBuf->width, 
                            (float)dstBuf->height / srcBuf->height);
    
    // 【修改点1】：NV12格式要求宽、高以及X、Y的偏移量必须是偶数 (2的倍数)，使用 & (~1) 强制清零最低位
    int scaled_w = ((int)(srcBuf->width * scale)) & (~1);
    int scaled_h = ((int)(srcBuf->height * scale)) & (~1);
    int offset_x = ((dstBuf->width - scaled_w) / 2) & (~1);
    int offset_y = ((dstBuf->height - scaled_h) / 2) & (~1);

    im_rect srect = {0, 0, srcBuf->width, srcBuf->height};
    im_rect drect = {offset_x, offset_y, scaled_w, scaled_h};

    /* 4. 背景填充 (Padding) */
    if (scaled_w != dstBuf->width || scaled_h != dstBuf->height) {
        im_rect dst_whole_rect = {0, 0, dstBuf->width, dstBuf->height};
        
        // 【修改点2】：NV12 填充灰色。YUV 的灰色大约是 Y=114(0x72), U=128(0x80), V=128(0x80)
        ret_rga = imfill(rga_buf_dst, dst_whole_rect, 0x00808072); 
        
        // CPU 回退策略 (如果 RGA 硬件填充失败)
        if (ret_rga <= 0 && dstBuf->virtAddr != nullptr) {
            int y_size = dstBuf->width * dstBuf->height;
            int uv_size = y_size / 2;
            
            // NV12 是半平面格式，Y 占据前 width*height，紧接着是交错的 UV 分量
            uint8_t* y_ptr = static_cast<uint8_t*>(dstBuf->virtAddr);
            uint8_t* uv_ptr = y_ptr + y_size;
            
            memset(y_ptr, 114, y_size);   // Y 亮度通道：114 (中灰色)
            memset(uv_ptr, 128, uv_size); // UV 色度通道：128 (无色差)
        }
    }

    /* 5. RGA 终极处理：瞬间完成 格式转换 + 等比例缩放 + 偏移写入 */
    rga_buffer_t pat; memset(&pat, 0, sizeof(rga_buffer_t));
    im_rect prect; memset(&prect, 0, sizeof(im_rect));
    
    ret_rga = improcess(rga_buf_src, rga_buf_dst, pat, srect, drect, prect, 0);
    if (ret_rga <= 0) {
        std::cerr << "[RGA Error] improcess failed: " << imStrError((IM_STATUS)ret_rga) << std::endl;
        ret = false;
    }

    /* 6. 释放 RGA 句柄 (必须执行，否则内核内存泄漏！) */
    releasebuffer_handle(rga_handle_src);
    releasebuffer_handle(rga_handle_dst);

    return ret;
}