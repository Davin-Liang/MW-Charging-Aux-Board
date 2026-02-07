#include "usb-hikvisioner.h"
#include "antenna-visioner.h"
#include <iostream>
#include <chrono>


int main ()
{
    bool ret;
    std::vector<DetectionResult> results;

    USBHikvisioner uhv(0);
    AntennaVisioner anV("./yolov8_model/best.rknn", 
                        "./yolov8_model/antenna.txt", 1024, 614);

    cv::Mat& img = anV.get_rga_mat();

    anV.init_system();
    ret = uhv.camera_init();
    if (!ret)
        return 0;

    ret = uhv.camera_open();
    if (!ret)
        return 0;

    auto start_time = std::chrono::high_resolution_clock::now();    
    // ret = uhv.read_img(img, 1000);

    cv::Mat temp = cv::imread("./yolov8_model/image.jpg");
    temp.copyTo(img);

    if (!ret)
        return 0;
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    std::cout << "采集一帧图像所需要的时间: " << duration.count() << "ms." << std::endl;

    ret = anV.detect_once(img, results);
    if (!ret)
        return 0;

    for (const auto& item : results) {
        std::cout << "Detected: " << item.className 
                  << ", Prob: " << item.prop << std::endl;
    }

    if (img.empty()) {
        std::cout << "错误：图像为空，无法保存！" << std::endl;
        return 0;
    }

    // 保存图片
    bool result = cv::imwrite("output_image.jpg", img);
    if (result)
        std::cout << "图片保存成功！" << std::endl;
    else
        std::cerr << "图片保存失败。" << std::endl;

    return 0;
}