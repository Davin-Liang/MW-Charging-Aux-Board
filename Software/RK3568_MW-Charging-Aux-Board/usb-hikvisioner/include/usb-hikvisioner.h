#ifndef USB_HIKVISIONER_H
#define USB_HIKVISIONER_H

#include <stdio.h>
#include <string.h>
#include "opencv2/core/core_c.h"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "MvCameraControl.h"
#include <memory>

enum CameraStatus {
    WAIT_FOR_INIT = 0,
    INITED,
    OPENED,
    CLOSED,
};

class USBHikvisioner {
public:
    USBHikvisioner(int videoDeviceIndex);
    ~USBHikvisioner();

    bool camera_init(void);
    bool camera_open(void);
    bool camera_close(void);
    bool read_img(cv::Mat& img, unsigned int nMsec);
    bool convert_to_mat(MV_FRAME_OUT_INFO_EX& stImageInfo, 
                                        std::unique_ptr<uint8_t[]>& pData,
                                        cv::Mat& dstImg);

private:
    void print_device_info_(MV_CC_DEVICE_INFO& stMVDevInfo);

    int cameraDeviceIndex_;
    void * deviceHandle_;
    std::unique_ptr<uint8_t[]> pData_;
    unsigned int nPayloadSize_;
    MV_FRAME_OUT_INFO_EX stImageInfo_;
    CameraStatus cameraStatus_;
};

#endif
