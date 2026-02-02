#include "usb-hikvisioner.h"
#include <iostream>
#include <chrono>

int main ()
{
    bool ret;
    cv::Mat img;

    USBHikvisioner uhv(0);
    ret = uhv.camera_init();
    if (!ret)
        return 0;

    ret = uhv.camera_open();
    if (!ret)
        return 0;

    auto start_time = std::chrono::high_resolution_clock::now();    
    ret = uhv.read_img(img, 1000);
    if (!ret)
        return 0;
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    std::cout << "Inference + Draw time: " << duration.count() << "ms" << std::endl;

    if (img.empty()) {
        std::cout << "错误：图像为空，无法保存！" << std::endl;
        return 0;
    }

    // 保存图片
    bool result = cv::imwrite("hikvision_image.jpg", img);
    if (result)
        std::cout << "图片保存成功！" << std::endl;
    else
        std::cerr << "图片保存失败。" << std::endl;

    return 0;
}