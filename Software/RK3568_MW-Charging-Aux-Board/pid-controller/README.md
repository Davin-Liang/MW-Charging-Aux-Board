# PIDController: 工业级云台闭环追踪控制模块

## 📖 简介

`PIDController` 是专为 `TurntableController` 硬件驱动层量身打造的  **上层算法闭环控制模块** 。

在实际的自动化追踪（如视觉锁定、雷达引导）场景中，仅靠开环下发目标角度往往无法应对机械惯性、负载变化和外部干扰。本模块通过引入经典的 **比例-积分-微分 (PID)** 控制算法，实时读取云台当前物理反馈角度，动态计算并下发补偿速度与方向，从而实现高速、平滑且精准的动态位置追踪。

## ✨ 核心特性

* **高精度闭环位置追踪** ：
  * 彻底接管底层的角度控制，将生硬的“点到点”绝对运动转化为平滑的“速度-时间”动态逼近，有效消除机械稳态误差。
* **独立单轴实例化设计** ：
  * 采用“一轴一实例”的解耦架构。你可以为 Yaw（偏航）和 Pitch（俯仰）轴分别实例化独立的 PID 控制器，并针对两轴不同的机械负载特性（如俯仰轴受重力影响更大）配置完全不同的 PID 增益参数。
* **多线程与运行时安全** ：
  * 核心状态机与增益参数读写均受 `std::mutex` 保护。支持在主控线程运行 `controlLoop` 的同时，由另一个 UI 线程或网络线程实时动态修改 `kp, ki, kd` 参数，无需停机。
* **内置输出限幅防抖** ：
  * 算法末端内置了速度输出安全限幅（`MAX_SPEED`），防止在目标突变产生巨大 Error 时下发超限速度，保护减速器和电机免受剧烈冲击。

## 🛠️ 环境依赖

* **C++ 标准** : C++11 或更高 (`CMAKE_CXX_STANDARD 14` 推荐)
* **核心库依赖** :
  * `turntable_controller` (本项目的底层硬件驱动模块)
* **系统要求** : 支持标准 `<mutex>` 和 `<cmath>` 库。

## ⚙️ 作为子工程引入 (CMake Integration)

PID 模块强依赖于底层云台硬件模块。在你的主工程 `CMakeLists.txt` 中，请确保  **先引入转台控制器，再引入 PID 控制器** ：

**CMake**

**CMake**

```
# 1. 引入底层云台驱动 (含 libmodbus)
add_subdirectory(path/to/turntable_controller)

# 2. 引入 PID 算法模块
add_subdirectory(path/to/pid_controller)

# 3. 将你的应用链接到 PID 模块 (CMake会自动将 turntable_controller 一并链接)
add_executable(your_tracking_app main.cpp)
target_link_libraries(your_tracking_app PUBLIC 
    pid_controller_lib
)
```

## 🧰 核心类方法 API 详解

### 1. 实例化与绑定

* `explicit PIDController(YawOrPitch_t axis, TurntableController *controller = nullptr)`
  * **说明** : 构造函数。指明该 PID 实例负责接管哪个轴（Yaw 或 Pitch），并传入底层转台的实例化指针。
* `void setController(TurntableController *controller)`
  * **说明** : 运行时动态重绑底层硬件控制器句柄。

### 2. 参数动态调节

* `void setGains(const Gains &g)`
  * **说明** : 设置 PID 增益结构体（包含 `kp`, `ki`, `kd`）。线程安全，支持运行时动态调参。
* `Gains getGains() const`
  * **说明** : 获取当前正在使用的增益参数，常用于 UI 界面数据回显。
* `void reset()`
  * **说明** : 清空历史积分器 (`integral`) 和上次误差 (`lastError`)。在每次切换追踪新目标、或因卡顿导致积分饱和时，建议调用此函数以防系统“猛窜”。

### 3. 核心控制环

* `bool controlLoop(double target, double threshold = 0.01, double controlPeriodSec = 0.05)`
  * **说明** :  **核心调度引擎（需在外部循环中持续调用）** 。
  * **行为** : 自动读取当前反馈角度 **$\rightarrow$** 计算误差 **$\rightarrow$** 执行 PID 运算 **$\rightarrow$** 将计算结果转化为方向和速度下发给云台。
  * **返回值** : 当且仅当 `|当前角度 - target| <= threshold` 时，判定为到达目标并返回 `true`，同时自动下发停止指令；否则返回 `false`。

## 🚀 业务代码接入指引

### 标准工作流：双轴协同闭环追踪

因为 `PIDController` 是单轴管理的，所以在实际控制双轴转台时，我们需要实例化两个 PID 对象，并将它们放入一个高频循环中执行。

**C++**

**C++**

```
#include "turntable_controller.h"
#include "PID_Controller.h"
#include <iostream>
#include <unistd.h>

int main() {
    // 1. 初始化底层硬件控制器
    TurntableController gimbal("/dev/ttyUSB0", 0xcc);
    if (!gimbal.connect(115200)) {
        std::cerr << "云台硬件连接失败！" << std::endl;
        return -1;
    }

    // 2. 实例化双轴 PID 控制器，并注入硬件依赖
    PIDController pidYaw(YawOrPitch_t::Yaw, &gimbal);
    PIDController pidPitch(YawOrPitch_t::Pitch, &gimbal);

    // 3. 配置 PID 参数 (注意：不同轴的机械特性不同，参数往往需要分开整定)
    PIDController::Gains yawGains   = { 1.5, 0.01, 0.1 }; // 示例参数
    PIDController::Gains pitchGains = { 2.0, 0.05, 0.2 }; 
    pidYaw.setGains(yawGains);
    pidPitch.setGains(pitchGains);

    // 4. 设定追踪目标
    double targetYaw = 45.0;
    double targetPitch = 15.0;
  
    std::cout << "开始闭环追踪目标: Yaw=" << targetYaw << "°, Pitch=" << targetPitch << "°" << std::endl;

    bool yawReached = false;
    bool pitchReached = false;

    // 5. 开启高频控制环 (通常放在单独的线程中运行)
    while (!(yawReached && pitchReached)) {
      
        // 分别计算并下发两轴的控制指令 (容差设为 0.5°)
        if (!yawReached) {
            yawReached = pidYaw.controlLoop(targetYaw, 0.5, 0.05);
        }
      
        if (!pitchReached) {
            pitchReached = pidPitch.controlLoop(targetPitch, 0.5, 0.05);
        }

        // 模拟控制周期 (50ms = 0.05s)，与 controlPeriodSec 对应
        usleep(50000); 
    }

    std::cout << "目标已锁定！" << std::endl;

    // 6. 退出释放
    gimbal.disconnect();
    return 0;
}
```

## 🧠 核心架构说明 (Data Flow)

本模块是一个典型的时间离散型反馈控制系统：

1. **Sense (感知)** ：`controlLoop` 入口第一时间调用底层的 `read_axis_angle`，通过 Modbus 总线获取云台电机的实时编码器角度反馈。
2. **Compute (计算)** ：

* **P (比例)** ：当前误差直接乘以 **$K_p$**，提供主要的纠偏动力。
* **I (积分)** ：误差随时间累加并乘以 **$K_i$**，用于消除系统静态阻力（如线缆拉力）导致的稳态误差。
* **D (微分)** ：误差的变化率乘以 **$K_d$**，提前感知“超调”趋势，起到阻尼减震的作用。

1. **Limit & Decode (限幅与解码)** ：对算出的输出总量进行 `MAX_SPEED` 上下限幅，随后根据正负号，将其拆解为下发给底层的 `速度 (Speed)` 和 `点动方向 (Left/Right)`。
2. **Act (执行)** ：调用底层的 `set_axis_speed` 与 `set_manual_rotation` 让电机按计算出的姿态运行，并在下一周期重复以上步骤，直到误差收敛至死区（Threshold）内。
