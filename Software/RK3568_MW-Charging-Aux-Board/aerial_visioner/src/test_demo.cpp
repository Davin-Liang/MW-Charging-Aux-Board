#include "antenna-visioner.h"
#include "opencv2/core.hpp"
#include "opencv2/videoio.hpp" // VideoCapture
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include <iostream>
#include "opencv2/opencv.hpp"

int main()
{
    AntennaVisioner anV("./model/yolov5s-640-640.rknn", "./model/coco_80_labels_list.txt", 9);

    anV.init_system();
    cv::Mat img;
    if (anV.detect_once(img)) {
        /* 将图片存为文件 */
        // 检查图像是否为空
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
    } 

    return 0;
}