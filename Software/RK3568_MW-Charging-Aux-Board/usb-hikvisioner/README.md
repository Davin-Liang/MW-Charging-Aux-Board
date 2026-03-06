# USBHikvisioner: 硬件加速的工业相机图像分发模块

## 📖 简介

`USBHikvisioner` 是一个专为基于 RK3568 等异构计算平台设计的海康威视 (Hikvision) USB 工业相机控制模块。

本模块不仅封装了基础的相机生命周期管理（枚举、初始化、开流、配置），更重要的是，它 **深度集成了 Rockchip RGA 硬件加速模块与自定义 DMA 零拷贝内存池 (`DmaBufferPool`)** 。通过底层的“生产者-消费者”多线程架构，实现了从相机原始视频流 (YUYV) 到下游 AI 推理 (如 YOLO 所需的 640x640 RGB888) 的 **全硬件、零 CPU 拷贝转换** 。

## ✨ 核心特性

* **双工作模式支持** ：
* **传统轮询模式** ：提供 `read_img` 接口，支持阻塞获取 OpenCV `cv::Mat` 图像。
* **高性能异步回调模式** ：使能 `open_grab_callback()`，彻底解放主线程。
* **RGA 硬件级图像处理** ：内置独立后台调度线程 (`rgaThread_`)，自动调用 RGA 硬件完成分辨率缩放 (Letterbox) 与色彩空间转换 (YUYV422 -> RGB888)。
* **工业级多线程与内存安全** ：
* 严格遵循 C++ 面向对象规范（Rule of Five），防范句柄重复释放。
* 采用 `ThreadSafeQueue` 和 `std::condition_variable` 实现无锁等待与极速唤醒。
* **无缝 CMake 集成** ：原生支持 Standalone（独立编译）和 Sub-Project（作为子模块嵌套），并自动处理了复杂的 RPATH 依赖和 OpenCV 运行库打包。

## 🛠️ 环境依赖

* **C++ 标准** : C++14 (`CMAKE_CXX_STANDARD 14`)
* **核心库依赖** :
* `OpenCV` (包含 core, videoio, imgproc 等模块)
* `MvCameraControl` (海康机器视觉 SDK)
* `librga` (瑞芯微 2D 图形加速库)
* `dma_buffer_pool_lib` (本项目内的 DMA 内存池子模块)
* **交叉编译器** : Linaro GCC 6.3.1 (aarch64-linux-gnu)

## 📦 编译与部署

本项目提供了一键式交叉编译脚本。该脚本会自动配置目标板的 GCC/G++ 环境变量，执行 CMake 交叉构建，并将所有依赖项聚合到 `install` 目录。

### 1. 执行编译

在当前目录下运行一键编译脚本：

**Bash**

```
chmod +x build.sh
./build.sh
```

### 2. 编译产物 (`install/` 目录)

编译完成后，生成的可执行文件及依赖库会自动存放在本模块的 `install` 目录下：

* `usb_hikvisioner_demo`：测试 Demo 可执行程序。
* `lib/`：自动拷贝过来的 OpenCV 动态链接库 (`.so`)，利用 RPATH 机制，板端运行 `demo` 时会自动从此目录加载 OpenCV， **无需在板子上额外配置全局环境** 。
* `libusb_hikvisioner_lib.a`：供主工程链接的静态库。

## ⚙️ 作为子工程引入 (CMake Integration)

如果你需要在超级工程（如 `AntennaVisioner`）中引入相机模块，可以直接在顶层 `CMakeLists.txt` 中添加：

**CMake**

```
# 引入 USBHikvisioner 子目录 (内部已做好了防重复加载处理)
add_subdirectory(path/to/usb-hikvisioner)

# 将你的应用链接到本模块的静态库
add_executable(your_app main.cpp)
target_link_libraries(your_app PUBLIC usb_hikvisioner_lib)
```

## 🚀 业务代码接入指引

### 模式 A：高性能异步流水线 (推荐 YOLO 接入使用)

此模式下，CPU 不参与图像搬运，应用层只需从对外公开的 `yoloTaskQueue` 中拿取处理好的 RGB DMA 内存。

**C++**

```
#include "usb-hikvisioner.h"
#include <iostream>

int main() {
    // 1. 实例化相机 (设备索引 0)
    USBHikvisioner camera(0);

    // 2. 初始化设备
    if (!camera.camera_init()) return -1;

    // 3. 开启高性能异步回调与 RGA 硬件加速链路
    camera.open_grab_callback(); 
  
    // 4. 打开相机开始取流
    if (!camera.camera_open()) return -1;

    // 5. 消费者：在你的业务线程中循环获取处理好的数据
    while (true) {
        // 从线程安全队列中弹出 RGA 转换好的 YOLO 格式 Buffer
        DmaBuffer_t* yoloBuf = camera.yoloTaskQueue.pop();
      
        if (yoloBuf != nullptr) {
            // ... 将 yoloBuf 送入 NPU/RKNN 进行推理 ...
          
            // 用完后，务必归还给 yoloPool 内存池
            camera.yoloPool.release_buffer(yoloBuf);
        }
    }

    // 6. 安全退出
    camera.camera_close();
    return 0;
}
```

### 模式 B：传统阻塞读取模式 (适合调试与简单应用)

**C++**

```
#include "usb-hikvisioner.h"

int main() {
    USBHikvisioner camera(0);
    camera.camera_init();
    camera.camera_open(); // 注意：这里不要调用 open_grab_callback()

    cv::Mat frame;
    // 阻塞等待 1000ms 读取一帧并由 CPU 转换为 BGR8 的 cv::Mat
    if (camera.read_img(frame, 1000)) {
        // ... 使用 OpenCV 处理 frame ...
    }

    camera.camera_close();
    return 0;
}
```

## 🧠 核心架构说明 (Data Flow)

本模块的异步数据流严格遵循解耦设计：

1. **Producer** : `image_callback_ex2` (海康 SDK 线程) 接收到物理网卡/USB数据，写入 `sourcePool`，将句柄推入私有队列 `rgaTaskQueue_`。
2. **Dispatcher** : `rga_dispatch_thread_func` 线程被唤醒，控制 RGA 硬件将源图缩放并转换颜色，写入 `yoloPool`。
3. **Consumer** : 将处理好的 Buffer 推入公开的 `yoloTaskQueue`，供上层 AI 应用极速消费。
