#include "usb-hikvisioner.h"
#include <unistd.h>


/**
 * @brief 构造函数
 * @param cameraDeviceIndex 海康威视摄像头索引，通常为0
 */
USBHikvisioner::USBHikvisioner(int cameraDeviceIndex = 0) 
:cameraDeviceIndex_(cameraDeviceIndex),
 deviceHandle_(NULL),
 nPayloadSize_(0),
 cameraStatus_(WAIT_FOR_INIT)
{

}

/**
 * @brief 析构函数——释放资源
 */
USBHikvisioner::~USBHikvisioner()
{
    camera_close();
    
    /* 销毁句柄 */
    if (deviceHandle_) {
        MV_CC_DestroyHandle(deviceHandle_);
        deviceHandle_ = NULL;
    }
} 

/**
 * @brief 摄像头初始化
 * @return true 初始化成功 | false 初始化失败
 */
bool USBHikvisioner::camera_init(void)
{
    int ret = MV_OK;

    if (cameraStatus_ != WAIT_FOR_INIT) {
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
            if (NULL == pDeviceInfo) {
                printf("Fail to get current device info.\n");
                return false;
            }
            print_device_info_(*pDeviceInfo);
        }
        // return true;
    } else {
        printf("Find No Devices!\n");
        return false;
    }    

    /* 检查设备是否可访问 */
    if (cameraDeviceIndex_ >= 0 && cameraDeviceIndex_ < stDeviceList.nDeviceNum) {
        if (false == MV_CC_IsDeviceAccessible(stDeviceList.pDeviceInfo[cameraDeviceIndex_], 
                                                MV_ACCESS_Exclusive)) {
            printf("Can't connect! Please check the camera index value entered during instantiation.\n"); // 设备不可连接，请检查实例化时输入的摄像头索引值
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

    cameraStatus_ = INITED;
    return true;
}

/**
 * @brief 打开摄像头
 */
bool USBHikvisioner::camera_open(void)
{
    int ret = MV_OK;

    if (cameraStatus_ == OPENED) {
        printf("Camera has been opened. No need to open again.\n");
        return true;
    }

    if (cameraStatus_ == WAIT_FOR_INIT) {
        printf("Camera needs to be initialized first.\n");
        return false;
    }
    
    /* 打开设备 */
    ret = MV_CC_OpenDevice(deviceHandle_);
    if (MV_OK != ret) {
        printf("Open Device fail! ret: [0x%x]\n.", ret);
        return false;
    }   

    /* 设置自动增益 (Auto Gain) -> Continuous (2) */
    // 如果不支持自动，手动设置一个较高值
    ret = MV_CC_SetEnumValue(deviceHandle_, "GainAuto", 2);
    if (MV_OK != ret) {
        printf("[Warning] Set Auto Gain Fail, trying manual gain 10dB...\n");
        MV_CC_SetEnumValue(deviceHandle_, "GainAuto", 0); // Off
        MV_CC_SetFloatValue(deviceHandle_, "Gain", 10.0f);
    } else {
        printf("[Config] Set Auto Gain Success.\n");
    }

    /* 设置自动曝光 (Auto Exposure) -> Continuous (2) */
    // 如果不支持自动，手动设置一个较长曝光时间 (50ms = 50000us)
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
    // BinningHorizontal = 4, BinningVertical = 4 表示宽高各缩小一半
    ret = MV_CC_SetEnumValue(deviceHandle_, "BinningHorizontal", 4);
    if (MV_OK != ret) {
        printf("Set BinningH fail! ret: [0x%x]\n", ret);
        return false;
    }

    ret = MV_CC_SetEnumValue(deviceHandle_, "BinningVertical", 4);
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

    /* 初始化图像信息 */
    memset(&stImageInfo_, 0, sizeof(MV_FRAME_OUT_INFO_EX));
    pData_ = std::make_unique<uint8_t[]>(nPayloadSize_); // 默认置零
    if (nullptr == pData_) {
        printf("Allocate memory failed.\n");
        return false;
    }

    /* 开始取流 */
    ret = MV_CC_StartGrabbing(deviceHandle_);
    if (MV_OK != ret) {
        printf("Start Grabbing fail! ret: [0x%x].\n", ret);
        return false;
    }

    cameraStatus_ = OPENED;
    return true;
}

/**
 * @brief 关闭摄像头
 */
bool USBHikvisioner::camera_close(void)
{
    int ret;

    if (cameraStatus_ == CLOSED) {
        printf("Camera has been closed. No need to close again.\n");
        return true;
    }

    if (cameraStatus_ == WAIT_FOR_INIT or cameraStatus_ == INITED) {
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

    cameraStatus_ = CLOSED;
    return true;
}

/**
 * @brief 读取图像
 * @param img 要读取的图像
 * @param nMsec 读取超时时间
 * @return true 读取成功 | false 读取失败
 */
bool USBHikvisioner::read_img(cv::Mat& img, unsigned int nMsec)
{
    int ret;

    if (cameraStatus_ != OPENED) {
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

/**
 * @brief 图像转成opencv格式
 * @param stImageInfo 图像信息
 * @param pData 图像数据
 * @param dstImg 要转换成的图像
 * @return true 转换成功 | false 转换失败
 */
bool USBHikvisioner::convert_to_mat(MV_FRAME_OUT_INFO_EX& stImageInfo, 
                                    std::unique_ptr<uint8_t[]>& pData,
                                    cv::Mat& dstImg)
{
    if (!pData) {
        printf("Data is null.\n");
        return false;
    }

    // /* 1. 如果本身就是 Mono8 (灰度)，直接引用内存构建 Mat，无需转换 */
    // if (stImageInfo.enPixelType == PixelType_Gvsp_Mono8) {
    //     dstImg = cv::Mat(stImageInfo.nHeight, stImageInfo.nWidth, CV_8UC1, pData.get()).clone();
    //     return true;
    // }

    // 2. 如果是彩色格式 (Bayer, RGB, YUV 等)，统一让 SDK 转换成 BGR8
    // BGR8 是 OpenCV 默认的彩色格式 (Blue, Green, Red)
    
    // 计算目标需要的内存大小 (宽 * 高 * 3通道)
    unsigned int nRGBSize = stImageInfo.nWidth * stImageInfo.nHeight * 3;
    
    // 准备转换参数
    MV_CC_PIXEL_CONVERT_PARAM stConvertParam = {0};
    stConvertParam.nWidth = stImageInfo.nWidth;
    stConvertParam.nHeight = stImageInfo.nHeight;
    stConvertParam.pSrcData = pData.get();                  // 输入数据
    stConvertParam.nSrcDataLen = stImageInfo.nFrameLen;     // 输入长度
    stConvertParam.enSrcPixelType = stImageInfo.enPixelType;// 输入格式 (自动识别)
    
    // 目标格式：PixelType_Gvsp_BGR8_Packed (这是 OpenCV 最喜欢的格式)
    stConvertParam.enDstPixelType = PixelType_Gvsp_RGB8_Packed; 
    
    // 创建输出 Mat (分配内存)
    // dstImg = cv::Mat(stImageInfo.nHeight, stImageInfo.nWidth, CV_8UC3);
    stConvertParam.pDstBuffer = dstImg.data;                // 将转换结果直接写入 Mat 的内存
    stConvertParam.nDstBufferSize = nRGBSize;

    // 调用 SDK 进行转换
    int ret = MV_CC_ConvertPixelType(deviceHandle_, &stConvertParam);
    if (MV_OK != ret) {
        printf("Convert Pixel Type fail! ret: [0x%x], Input Format: [0x%x]\n", ret, stImageInfo.enPixelType);
        return false;
    }

    return true;
}

/**
 * @brief 打印设备信息
 * @param pstMVDevInfo 设备信息
 */
void USBHikvisioner::print_device_info_(MV_CC_DEVICE_INFO& stMVDevInfo)
{
    /* 获取图像数据帧 */
    if (MV_USB_DEVICE == stMVDevInfo.nTLayerType) {
        printf("    UserDefinedName: %s\n", stMVDevInfo.SpecialInfo.stUsb3VInfo.chUserDefinedName);
		printf("    Device Model Name: %s\n\n", stMVDevInfo.SpecialInfo.stUsb3VInfo.chModelName);
        return;
    }
    printf("    Not support.\n\n");  
}
