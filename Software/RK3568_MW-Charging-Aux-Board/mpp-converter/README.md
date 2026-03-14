
# MppConverter - RK3568 硬件多路视频编码器

`MppConverter` 是一个专为 Rockchip RK3568 平台设计的高性能、多路并发视频硬件编码类。它利用瑞芯微底层的 MPP (Media Process Platform) 框架，能够以“极致零拷贝”的方式将多路 NV12 图像流实时压缩为 H.265 (HEVC) 码流。

## ✨ 核心特性

* **硬件加速** ：完全调用 VPU 进行 H.265 编码，极大降低 CPU 占用率，解决纯软件录像/推流导致的卡顿问题。
* **极致零拷贝** ：直接通过 `dmaFd` 传递物理内存块，全程无需 CPU 拷贝图像数据。
* **多路并发** ：采用 Actor/Worker 模型，每添加一路视频流自动分配独立线程，互不阻塞，支持 1080p 与 720p 同时编码。
* **优雅解耦** ：采用生产者-消费者模型，通过 `ThreadSafeQueue` 阻塞式获取任务，自带 `nullptr` 毒丸安全退出机制与自动内存回收。

---

## 📦 依赖项与工程结构

本模块作为 CMake 子模块或静态库编译，依赖以下组件：

* **Rockchip MPP 库** ：需要 `rk_mpi.h`, `mpp_buffer.h` 等头文件及对应的动态库（存放在 `../3rdparty/`）。
* **USBHikvisioner** ：依赖海康相机模块提供的 `usb_hikvisioner_lib` 及 `DmaBuffer_t` 内存池结构。
* **C++ 标准** ：要求 C++14 或更高版本。

---

## 🛠️ 编译说明 (交叉编译)

本模块支持独立编译或作为 CMake 子项目嵌入。默认使用  **aarch64 交叉编译工具链** 。

1. **环境准备** ：
   确保您的 PC 上安装了 Linaro GCC 工具链，并解压至 `/opt/tool_chain/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu`（如路径不同，请修改根目录下编译脚本中的 `TOOL_CHAIN` 变量）。
2. **一键编译脚本** ：
   直接运行根目录下的编译脚本：
   **Bash**

```
   chmod +x build.sh
   ./build.sh
```

   *编译成功后，将在 `build/` 目录下生成 `libmpp_converter_lib.a` 静态库，并自动执行 `make install` 安装到本地 `install/` 目录。*

---

## 🔗 在主项目中集成 (CMake)

由于本模块在 `CMakeLists.txt` 中被标准化配置为静态库，并且通过 `PUBLIC` 关键字暴露了头文件路径与依赖，您可以非常优雅地将其作为子目录（Sub-project）无缝嵌入到主项目中。

在您**主工程**的 `CMakeLists.txt` 中添加以下配置：

**CMake**

```
# 1. 引入 MppConverter 模块所在的目录
# 假设您将本模块放在了主工程的 Software/RK3568_MW-Charging-Aux-Board/ 目录下
add_subdirectory(Software/RK3568_MW-Charging-Aux-Board/mpp-converter)

# 2. 定义您的主程序可执行文件
add_executable(your_main_application 
    src/main.cpp 
)

# 3. 链接 MppConverter 库
# 💡 提示：因为 mpp_converter_lib 已经 PUBLIC 暴露了它的 include 目录和底层 MPP 依赖，
# 所以您在主程序中无需再手动配置 include_directories，直接链接即可在 main.cpp 中 #include "mpp_converter.h"！
target_link_libraries(your_main_application
    PRIVATE
    mpp_converter_lib
)
```

---

## 🚀 快速使用指南 (C++)

以下是如何在您的主业务流中集成 `MppConverter` 的标准代码流程：

### 1. 引入头文件与实例化

**C++**

```
#include "mpp_converter.h"
#include "usb-hikvisioner.h"

// 假设您已经初始化了海康相机实例 (内部包含 dma-buffer-pool 等)
USBHikvisioner* camera = new USBHikvisioner();

// 实例化编码管理器，传入相机指针 (用于内存回收)
MppConverter encoderManager(camera);
```

### 2. 定义编码输出回调 (Callback)

当硬件压好一帧 H.265 数据后，会异步触发此回调。您可以在此对接推流或录像业务逻辑：

**C++**

```
auto on_h265_ready = [](int streamId, const uint8_t* h265Data, size_t dataSize) {
    if (streamId == 0) {
        // [业务逻辑]: 处理 1080p 录像数据
        // file.write((char*)h265Data, dataSize);
    } else if (streamId == 1) {
        // [业务逻辑]: 处理 720p 网络推流数据
        // zlmediakit.push(h265Data, dataSize);
    }
};
```

### 3. 添加并启动编码流

调用 `add_stream_task` 添加需要的视频流。您可以同时添加多个不同分辨率的流：

**C++**

```
// 假设这是您的图像任务队列 (存放从 RGA 处理完毕的 NV12 DmaBuffer_t 指针)
ThreadSafeQueue<DmaBuffer_t*> queue_720p;

// 启动一路 1280x720, 30fps, 2Mbps 的 H.265 编码管线
bool success = encoderManager.add_stream_task(
    1,                  // streamId (自定义的流标识，例如：1 代表 720p)
    1280,               // imageWidth (真实宽度)
    720,                // imageHeight (真实高度)
    30,                 // frameRate (输入/输出帧率)
    2000000,            // targetBps (目标码率: 2000000 表示 2Mbps)
    &queue_720p,        // taskQueue (绑定的输入源队列)
    on_h265_ready       // encodedCallback (绑定的输出回调)
);

if (!success) {
    std::cerr << "Failed to start 720p encoder!" << std::endl;
}
```

*💡  **工作原理** ：启动成功后，底层的消费者线程会立即阻塞等待 `queue_720p` 中的数据。前端只需不断往队列中 `push` 数据，硬件就会自动吃掉它并吐出 H.265。*

### 4. 优雅停止与销毁

当程序需要退出或停止推流时，调用停止方法。该方法会向所有阻塞的队列发送 `nullptr` 唤醒线程，并安全释放所有 MPP 硬件内存：

**C++**

```
encoderManager.stop_all_streams();
```

---

## ⚠️ 开发注意事项

1. **队列独立性** ：如果您启动了多路不同分辨率的流（如一路 1080p 录像，一路 720p 推流），**绝对不能**让它们共享同一个 `ThreadSafeQueue`。必须为每一路流准备独立的数据输入队列，否则会导致严重的抢占丢帧。
2. **16 字节对齐** ：RK3568 的 VPU 硬件要求图像宽高必须是 16 的整数倍。本类内部已通过 `MPP_ALIGN` 宏自动处理了 Stride（跨度），但请确保上游 RGA 转换时的目标尺寸也是 16 的倍数（如 1280, 720, 1920, 1080 均天然满足）。
3. **自动内存归还** ：本类仅负责读取 `dmaFd` 并进行硬件编码。为防止内存泄漏，消费者线程在使用完 `DmaBuffer_t` 后，已内置 **自动触发归还逻辑** （调用 `uhv_->pushFlowPool.release_buffer(srcBuffer)`），请勿在回调或其他外部代码中重复释放该块内存。
