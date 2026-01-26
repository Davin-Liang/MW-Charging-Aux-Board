#include "usb-hikvisioner.h"

/**
 * @brief 构造函数
 * @param cameraDeviceIndex 海康威视摄像头索引，通常为0
 */
USBHikvisioner::USBHikvisioner(int cameraDeviceIndex = 0) 
:cameraDeviceIndex_(cameraDeviceIndex),
 deviceHandle_(NULL),
 nPayloadSize_(0),
 cameraStatus_(WAIT_FOR_INIT):
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
        return true;
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

    int (cameraStatus_ == OPENED) {
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

    /* 关闭触发模式 */
    ret = MV_CC_SetEnumValue(deviceHandle_, "TriggerMode", 0);
    if (MV_OK != ret) {
        printf("Set Trigger Mode fail! ret: [0x%x]\n.", ret);
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
    if (cameraStatus_ != OPENED) {
        printf("Camera needs to been opened.\n");
        return false;
    }
    
    /* 获取一帧图像，超时时间1000ms */
    ret = MV_CC_GetOneFrameTimeout(deviceHandle_, pData_, nPayloadSize, &stImageInfo_, nMsec);
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
    cv::Mat srcImage;

    if (!pData) {
        printf("Data is null.\n");
        return false;
    }

    if (PixelType_Gvsp_Mono8 == stImageInfo.enPixelType) // Mono8类型
        srcImage = cv::Mat(stImageInfo.nHeight, stImageInfo.nWidth, CV_8UC1, pData.get());
    else if ( PixelType_Gvsp_RGB8_Packed == stImageInfo.enPixelType ) { // RGB8类型
        /* 构造临时的 RGB Mat */
        srcImage = cv::Mat(stImageInfo.nHeight, stImageInfo.nWidth, CV_8UC3, pData.get());
        /* 转换 RGB -> BGR (OpenCV 默认显示格式) 并存入 dstImg */
        cv::cvtColor(srcImage, dstImg, cv::COLOR_RGB2BGR);
    } else {
        printf("Unsupported pixel format\n");
        return false;
    }

    /* 检查转换结果 */
    if (dstImg.empty()) {
        printf("Convert Mat failed.\n");
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
