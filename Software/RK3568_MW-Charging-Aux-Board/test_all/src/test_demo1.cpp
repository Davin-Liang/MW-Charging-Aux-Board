#include "usb-hikvisioner.h"
#include "antenna-visioner.h"
#include <iostream>

int main() {
    // 1. 初始化底层的海康相机与 DMA 流水线
    USBHikvisioner camera(0);
    camera.camera_init();
    camera.open_grab_callback(); // 开启 DMA 回调模式
    camera.camera_open();

    // 2. 实例化 AntennaVisioner，传入相机指针进行绑定
    // 参数: 模型路径, 标签路径, 输入宽, 输入高, 相机对象指针
    AntennaVisioner visioner("./yolov8_model/yolov8n.rknn", 
                             "./yolov8_model/coco_80_labels_list.txt", 
                             640, 640, &camera);

    if (!visioner.init_system()) return -1;

    // 3. 循环推理
    cv::Mat outputFrame;
    std::vector<DetectionResult> results;
    
    // while (true) {
        // 极速单次检测！直接从 DMA 池拿数据，NPU 推理，并在物理内存上画框
    if (visioner.detect_once(outputFrame, results)) {
        // 终端打印识别结果
        for (const auto& res : results) {
            std::cout << "Detected: " << res.className 
                        << " Prob: " << res.prop * 100 << "%" << std::endl;
        }
        // outputFrame 是已经画好框的 BGR 图像，可以直接给 Qt 或 imshow 显示

        bool result = cv::imwrite("demo1_output_image.jpg", outputFrame);
        if (result)
            std::cout << "图片保存成功！" << std::endl;
        else
            std::cerr << "图片保存失败。" << std::endl;
    }
    // }

    if (camera.camera_close())
        std::cout << "成功关闭相机." << std::endl;
    else
        std::cout << "无法关闭相机." << std::endl;

    return 0;
}
