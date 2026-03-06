# AntennaVisioner: 边缘端硬件加速目标检测与视觉感知模块

## 📖 简介

`AntennaVisioner` 是一个专为嵌入式异构计算平台（如 Rockchip RK3568）打造的高性能目标检测与视觉感知核心模块。

本模块将底层的  **RKNN (NPU 推理)** 、 **RGA (2D 图形加速)** 、**DMA (零拷贝内存)** 以及 **OpenCV** 进行了深度有机结合。它向下兼容普通 USB 免驱摄像头与海康威视工业相机，向上提供极简的面向对象接口，实现了从“视频流获取 -> NPU 零拷贝推理 -> YOLO 后处理与画框”的全链路硬件加速闭环。

## ✨ 核心特性

* **⚡ 极致的 ZERO_COPY 架构** ：内部原生定义了 `ZERO_COPY` 编译宏，在海康相机模式下，图像数据直接在物理内存中流转（相机 -> DMA -> RGA -> RKNN），彻底绕过 CPU 内存拷贝，算力损耗降至最低。
* **📷 智能双引擎 (Dual-Mode)** ：
* **高性能模式** ：无缝对接 `USBHikvisioner`，享受 DMA 内存池与 RGA 异步调度带来的满血性能。
* **通用兼容模式** ：支持直接传入 `/dev/videoX` 索引，通过 OpenCV 原生读取任意普通 USB 摄像头。
* **🎯 一站式 YOLOv8 支持** ：内置完整的图像预处理（Letterbox 等比例缩放）与后处理（NMS 非极大值抑制、置信度过滤、BBox 实时绘制）。
* **🛠️ 完善的工程化部署** ：CMake 脚本已妥善处理 RPATH 运行库寻址、OpenCV 动态库分离打包以及 YOLOv8 模型文件的自动装配。

## 🧰 环境依赖与交叉编译

* **C++ 标准** : C++14
* **工具链** : Linaro GCC 6.3.1 (`aarch64-linux-gnu`)
* **核心依赖** : OpenCV, RKNN API (`librknnrt`), RGA API (`librga`), DMA Allocator。

### 一键构建与部署

项目根目录提供了自动化交叉编译脚本，执行以下命令即可完成编译与安装目录重组：

**Bash**

```
chmod +x build.sh
./build.sh
```

 **编译产物 (`install/` 目录)** ：

* `antenna_visioner_demo`: 测试验证用的可执行程序。
* `lib/`: 自动打包的 OpenCV 及其他依赖动态库。
* `yolov8_model/`: 自动拷贝的 RKNN 模型与标签文件。

---

## 🚀 业务代码接入指引

### 模式 A：满血版零拷贝模式（配合海康威视工业相机）

这是在板端部署的 **推荐模式** 。需要先实例化并启动 `USBHikvisioner` 底层基础设施。

**C++**

```
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
    AntennaVisioner visioner("./yolov8_model/yolov8.rknn", 
                             "./yolov8_model/coco_80_labels_list.txt", 
                             640, 640, &camera);

    if (!visioner.init_system()) return -1;

    // 3. 循环推理
    cv::Mat outputFrame;
    std::vector<DetectionResult> results;
  
    while (true) {
        // 极速单次检测！直接从 DMA 池拿数据，NPU 推理，并在物理内存上画框
        if (visioner.detect_once(outputFrame, results)) {
            // 终端打印识别结果
            for (const auto& res : results) {
                std::cout << "Detected: " << res.className 
                          << " Prob: " << res.prop * 100 << "%" << std::endl;
            }
            // outputFrame 是已经画好框的 BGR 图像，可以直接给 Qt 或 imshow 显示
        }
    }

    return 0;
}
```

### 模式 B：通用开发与调试模式（使用普通 USB 摄像头）

如果没有海康相机，也可以用几十块钱的免驱 USB 摄像头进行快速算法验证，只需改变构造函数即可。

**C++**

```
#include "antenna-visioner.h"

int main() {
    // 使用仅带 3 个参数的构造函数: 模型路径, 标签路径, /dev/video 索引号
    AntennaVisioner visioner("./yolov8_model/yolov8.rknn", 
                             "./yolov8_model/coco_80_labels_list.txt", 
                             0); // 对应 /dev/video0

    if (!visioner.init_system()) return -1;

    cv::Mat outputFrame;
    std::vector<DetectionResult> results;
  
    while (true) {
        // OpenCV 传统链路：抓图 -> CPU 转色 -> NPU 推理 -> CPU 画框
        if (visioner.detect_once(outputFrame, results)) {
            // ... 处理结果 ...
        }
    }
    return 0;
}
```

## ⚙️ CMake 嵌套集成 (Sub-Project)

当您的超级工程需要链接本视觉模块时，请注意 **本模块编译出的静态库名称为 `aerial_visioner_lib`** 。在顶层 `CMakeLists.txt` 中添加：

**CMake**

```
# 引入视觉模块子目录
add_subdirectory(path/to/antenna_visioner)

# 链接到您的主程序
add_executable(my_main_app main.cpp)

# 注意链接的库名
target_link_libraries(my_main_app PUBLIC aerial_visioner_lib)
```
