/**
 * @file turntable_controller.h
 * @brief 转台控制器头文件
 * @details 通过 Modbus RTU 协议控制转台设备的类
 */

#pragma once

#include <modbus.h>
#include <cstdint>

// 使用 constexpr 替代宏定义，提供类型安全
constexpr float MAX_AXIS_SPEED = 30.0f;       // 单位[°/sec]
constexpr float MAX_AXIS_PITCH_ANGLE = 60.0f; // 单位[°]

// 使用强类型枚举 (enum class) 提升类型安全，避免命名冲突
enum class LeftOrRight_t {
    Left,
    Right
};

enum class YawOrPitch_t {
    Yaw,
    Pitch
};

class TurntableController {
public:
    /**
     * @brief 构造函数
     * @param port 串口设备路径，如 "/dev/ttyUSB0" 或 "/dev/ttyS0"
     * @param slaveId 从站设备地址，默认为 0xcc
     */
    TurntableController(const char* port, int slaveId = 0xcc);

    /**
     * @brief 析构函数，自动断开连接并释放资源
     */
    ~TurntableController();

    /**
     * @brief 连接到转台设备
     * @param baudrate 波特率，如 9600, 19200, 115200 等
     * @param parity 校验位：'N'=无校验, 'E'=偶校验, 'O'=奇校验
     * @param dataBit 数据位，通常为8
     * @param stopBit 停止位，通常为1或2
     * @return 连接成功返回 true，失败返回 false
     */
    bool connect(int baudrate = 115200, char parity = 'N', int dataBit = 8, int stopBit = 1);

    /**
     * @brief 断开与转台设备的连接并释放资源
     */
    void disconnect();

    /**
     * @brief 检查连接状态
     * @return 已连接返回true，未连接返回false
     */
    bool is_connected() const { return m_isConnected; }

    /**
     * @brief 读取保持寄存器
     * @param addr 起始地址（从0开始，对应Modbus地址40001）
     * @param nb 要读取的寄存器数量
     * @param dest 存储读取结果的数组指针
     * @return 读取成功返回 true，失败返回 false
     */
    bool read_holding_registers(int addr, int nb, uint16_t* dest);

    /**
     * @brief 写入单个保持寄存器
     * @param addr 寄存器地址（从0开始，对应Modbus地址40001）
     * @param value 要写入的16位值
     * @return 写入成功返回 true，失败返回 false
     */
    bool write_single_register(int addr, uint16_t value);

    /**
     * @brief 写入多个保持寄存器
     * @param addr 起始地址
     * @param nb 要写入的寄存器数量
     * @param data 包含要写入数据的数组指针
     * @return 写入成功返回 true，失败返回 false
     */
    bool write_multiple_registers(int addr, int nb, const uint16_t* data);

    /**
     * @brief 读取线圈状态（数字输出）
     * @param addr 起始地址（从0开始，对应Modbus地址00001）
     * @param nb 要读取的线圈数量
     * @param dest 存储读取结果的数组指针
     * @return 读取成功返回 true，失败返回 false
     */
    bool read_coils(int addr, int nb, uint8_t* dest);

    /**
     * @brief 写入单个线圈（数字输出）
     * @param addr 线圈地址
     * @param status 线圈状态：true=ON(1), false=OFF(0)
     * @return 写入成功返回 true
     */
    bool write_single_coil(int addr, bool status);

    /**
     * @brief 读取输入状态（数字输入）
     * @param addr 起始地址
     * @param nb 要读取的输入数量
     * @param dest 存储读取结果的数组指针
     * @return 读取成功返回 true
     */
    bool read_input_status(int addr, int nb, uint8_t* dest);

    /**
     * @brief 读取输入寄存器（只读寄存器）
     * @param addr 起始地址
     * @param nb 要读取的寄存器数量
     * @param dest 存储读取结果的数组指针
     * @return 读取成功返回 true
     */
    bool read_input_registers(int addr, int nb, uint16_t* dest);

    /**
     * @brief 设置轴的手动操作
     * @param axis 要转动哪个轴
     * @param dir 左转还是右转
     * @return 操作成功返回 true
     */
    bool set_manual_rotation(YawOrPitch_t axis, LeftOrRight_t dir);

    /**
     * @brief 停止轴的手动操作
     * @param axis 要停止哪个轴
     * @return 操作成功返回 true
     */
    bool stop_manual_rotation(YawOrPitch_t axis);

    /**
     * @brief 轴坐标清零
     * @param axis 要设置哪个轴
     * @return 操作成功返回 true
     */
    bool reset_axis_coord(YawOrPitch_t axis);

    /**
     * @brief 设置轴的速度[°/sec]
     * @param axis 轴类型
     * @param goalSpeed 目标速度
     * @return 操作成功返回 true
     */
    bool set_axis_speed(YawOrPitch_t axis, float goalSpeed);

    /**
     * @brief 读取轴的速度[°/sec]
     * @param axis 轴类型
     * @param readedSpeed 获取到的速度存储指针
     * @return 操作成功返回 true
     */
    bool read_axis_speed(YawOrPitch_t axis, float* readedSpeed);

    /**
     * @brief 设置轴的角度[°]
     * @param axis 轴类型
     * @param goalAngle 目标角度
     * @return 操作成功返回 true
     */
    bool set_axis_angle(YawOrPitch_t axis, float goalAngle);

    /**
     * @brief 读取轴的角度[°]
     * @param axis 轴类型
     * @param readedAngle 获取到的角度存储指针
     * @return 操作成功返回 true
     */
    bool read_axis_angle(YawOrPitch_t axis, float* readedAngle);

    /**
     * @brief 设置轴的相对运动[°]
     * @param axis 轴类型
     * @param goalAngle 要运动的相对角度
     * @return 操作成功返回 true
     */
    bool set_axis_relative_motion(YawOrPitch_t axis, float goalAngle);

    /**
     * @brief 带重试的读取操作
     * @param addr 寄存器地址
     * @param nb 寄存器数量
     * @param dest 目标数组
     * @param maxRetries 最大重试次数
     * @return 成功返回true
     */
    bool read_holding_registers_with_retry(int addr, int nb, uint16_t* dest, int maxRetries = 3);

    /**
     * @brief 设置从站地址
     * @param slaveId 新的从站地址
     */
    void set_slave_id(int slaveId);

    int get_slave_id() const { return m_slaveId; }
    const char* get_port() const { return m_port; }
    bool get_connect_status() const { return m_isConnected; }

private:
    modbus_t* m_ctx;        ///< Modbus 上下文指针
    const char* m_port;     ///< 串口设备路径
    int m_slaveId;          ///< 从站设备地址
    bool m_isConnected;     ///< 连接状态标志

    // 内部数据转换辅助函数
    void floatTo3412(float value, uint16_t words[2]);
    float floatFrom3412(const uint16_t words[2]);
};
