# TurntableController: 工业级双轴云台 Modbus 控制模块

## 📖 简介

`TurntableController` 是一个专为嵌入式平台（如 RK3568 等自动化控制主板）设计的高可靠双轴（Yaw 偏航轴 / Pitch 俯仰轴）云台控制模块。

本模块不仅封装了基础的串口通信与设备生命周期管理，更重要的是，它 **深度屏蔽了底层 Modbus RTU 协议的复杂性与特殊的硬件字节序要求** 。通过面向对象的 C++11 API 设计，实现了从应用层直观的物理单位（角度/速度）到云台底层寄存器状态的 **全自动转换与安全校验** ，非常适合应用于激光追踪、雷达指引等自动化指向场景。

## ✨ 核心特性

* **全维度的运动控制接口** ：
  * **精准定位** ：提供绝对角度移动 (`set_axis_angle`) 与相对角度步进 (`set_axis_relative_motion`) 模式。
  * **柔性调速** ：支持动态配置独立轴运行速度 (`set_axis_speed`)。
  * **手动点动** ：提供方向控制的持续转动及停止接口，便于对接摇杆或上位机调试。
* **底层协议透明化处理** ：
  * 内置 IEEE 754 浮点数解析引擎，自动完成应用层浮点数与云台 Modbus 特殊的 **3412 字节顺序 (B3 B4 B1 B2)** 的双向无损转换。
* **工业级软件限位与容错安全** ：
  * **硬件保护** ：源码级内置安全阈值拦截，严格限制 Pitch 轴最大仰角不超过 60°，全局最高速度不超过 30°/sec，防止由于上层算法异常导致的机械损坏。
  * **通信抗干扰** ：核心读取操作原生提供带延时重试机制 (`read_holding_registers_with_retry`)，有效应对工业现场的电磁干扰。
* **现代化 C++ 规范集成** ：
  * 采用强类型枚举 (`enum class`) 与 `constexpr`，实现编译期类型安全，杜绝魔术数字与宏定义污染。

## 🛠️ 环境依赖

* **C++ 标准** : C++11 或更高 (`CMAKE_CXX_STANDARD 14` 推荐)
* **核心库依赖** :
  * `libmodbus` (开源的高质量 Modbus C库，建议作为 3rdparty 源码同编)
* **操作系统** : Linux (要求运行程序的用户具备 `/dev/ttyUSB*` 或 `/dev/ttyS*` 等串口设备的读写权限)

## 📦 编译与部署

本项目推荐采用 CMake 原生构建。如果你已经将 `libmodbus` 配置为本地静态库（`libmodbus.a`），直接将其与本模块一并编译即可。

 **编译产物** ：

* 编译完成后，将生成 `libturntable_controller_lib.so` (动态库) 或 `.a` (静态库)，供你的主业务工程直接链接。

## ⚙️ 作为子工程引入 (CMake Integration)

如果你的主工程（如激光追踪系统）需要引入此云台模块，可以直接在顶层 `CMakeLists.txt` 中添加：

**CMake**

**CMake**

```
# 1. 引入 libmodbus 源码子目录 (确保底层依赖就绪)
add_subdirectory(path/to/3rdparty/libmodbus)

# 2. 引入 TurntableController 子目录
add_subdirectory(path/to/turntable_controller)

# 3. 将你的应用链接到云台模块
add_executable(your_tracking_app main.cpp)
target_link_libraries(your_tracking_app PUBLIC 
    turntable_controller_lib
)
```

## 🧰 核心类方法 API 详解

本类使用强类型枚举来区分轴和方向，调用前请注意使用 `YawOrPitch_t::Yaw` / `Pitch` 以及 `LeftOrRight_t::Left` / `Right`。

### 1. 生命周期与连接管理

* `TurntableController(const char* port, int slaveId = 0xcc)`
  * **说明** : 构造函数。`port` 传入设备节点（如 `"/dev/ttyS3"`），`slaveId` 为云台的 Modbus 从站地址（默认 204）。
* `bool connect(int baudrate = 115200, char parity = 'N', int dataBit = 8, int stopBit = 1)`
  * **说明** : 初始化底层的 Modbus RTU 上下文并打通串口。如果已有连接会先自动断开。
* `void disconnect()`
  * **说明** : 安全释放 `modbus_t` 句柄，关闭串口。析构时会自动调用。
* `bool is_connected() const`
  * **说明** : 检查当前软件层面是否处于已连接状态。

### 2. 运动控制 (Motion Control)

* `bool set_axis_angle(YawOrPitch_t axis, float goalAngle)`
  * **说明** :  **绝对定位** 。驱动指定轴运动到基于零点的绝对角度。
  * **注意** : 当 `axis == Pitch` 时，`goalAngle` 会被强行限制在 `MAX_AXIS_PITCH_ANGLE` (60°) 以内。
* `bool set_axis_relative_motion(YawOrPitch_t axis, float goalAngle)`
  * **说明** :  **相对步进** 。以云台当前物理位置为起点，增量移动 `goalAngle` 角度（正数增加，负数减少）。

### 3. 速度配置与状态读取 (Speed & Status)

* `bool set_axis_speed(YawOrPitch_t axis, float goalSpeed)`
  * **说明** : 设定目标轴的运行速度（单位：°/sec）。内部限制最高速度为 30 °/sec。
* `bool read_axis_speed(YawOrPitch_t axis, float* readedSpeed)`
  * **说明** : 通过指针带出当前轴的真实设定速度。
* `bool read_axis_angle(YawOrPitch_t axis, float* readedAngle)`
  * **说明** : 通过指针带出云台当前所处的真实物理角度，常用于闭环反馈与追踪算法。

### 4. 手动控制与校准 (Manual & Calibration)

* `bool set_manual_rotation(YawOrPitch_t axis, LeftOrRight_t dir)`
  * **说明** : 开启手动点动模式，云台将持续向指定方向转动，通常用于上位机 UI 的方向按键按下事件。
* `bool stop_manual_rotation(YawOrPitch_t axis)`
  * **说明** : 停止对应轴的手动转动，通常用于上位机 UI 的方向按键抬起事件。
* `bool reset_axis_coord(YawOrPitch_t axis)`
  * **说明** :  **坐标清零** 。将当前物理位置的坐标强制标定为 0° 机械零点。

## 🚀 业务代码接入指引

### 标准工作流：连接、配置、运动控制

本模块的调用非常直观，上层算法线程只需下发目标角度，无需关心寄存器地址或字节拼接。

**C++**

**C++**

```
#include "turntable_controller.h"
#include <iostream>
#include <unistd.h>

int main() {
    // 1. 实例化云台设备 (指定串口节点与从站ID 0xcc)
    TurntableController gimbal("/dev/ttyUSB0", 0xcc);

    // 2. 初始化底层串口并建立 Modbus 连接 (115200波特率)
    if (!gimbal.connect(115200, 'N', 8, 1)) {
        std::cerr << "云台连接失败，请检查串口权限及接线！" << std::endl;
        return -1;
    }

    // 3. 基础参数配置：设置双轴运行速度为 15.0 °/sec
    gimbal.set_axis_speed(YawOrPitch_t::Yaw, 15.0f);
    gimbal.set_axis_speed(YawOrPitch_t::Pitch, 15.0f);

    // 4. 绝对运动控制：驱动云台转动到指定角度
    std::cout << "下发运动指令..." << std::endl;
    gimbal.set_axis_angle(YawOrPitch_t::Yaw, 45.0f);   // 偏航轴转至 45°
    gimbal.set_axis_angle(YawOrPitch_t::Pitch, 30.0f); // 俯仰轴转至 30°

    // 5. 状态监测：循环读取当前角度
    float currentYaw = 0.0f;
    for (int i = 0; i < 5; ++i) {
        sleep(1);
        if (gimbal.read_axis_angle(YawOrPitch_t::Yaw, &currentYaw)) {
            std::cout << "当前 Yaw 轴反馈角度: " << currentYaw << " °" << std::endl;
        }
    }

    // 6. 相对步进控制：当前位置继续微调偏移 5°
    gimbal.set_axis_relative_motion(YawOrPitch_t::Yaw, 5.0f);

    // 7. 安全退出释放句柄
    gimbal.disconnect();
    return 0;
}
```

## 🧠 核心架构说明 (Data Flow)

本控制模块的数据流向设计如下：

1. **Upper Layer (App/Algorithm)** : 下发直观的物理量指令（如 `float goalAngle = 45.0f`）。
2. **Data Transformer (Internal)** : 内部引擎调用 `floatTo3412()`，通过内存拷贝 `std::memcpy` 提取浮点数的 IEEE 754 原始大端字节，并按 `B3 B4 B1 B2` 重新拼装为两个 `uint16_t` 寄存器字。
3. **Modbus Context (libmodbus)** : 将拼接好的字数组通过 `modbus_write_multiple_registers` 封包，加上 CRC 校验后通过 Linux TTY 驱动经 RS485/RS232 下发至硬件。
4. **Safety Filter** : 在整个数据下行链路中，指令会被预先卡在 `MAX_AXIS_PITCH_ANGLE` 和 `MAX_AXIS_SPEED` 的过滤网中，杜绝非法越界指令下发至下位机。
