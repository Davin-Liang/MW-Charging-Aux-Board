/**
 * @file turntable_controller.h
 * @brief 转台控制器头文件
 * 通过 Modbus RTU 协议控制转台设备的类
 */

#ifndef TURNTABLE_CONTROLLER_H
#define TURNTABLE_CONTROLLER_H

#include <modbus/modbus.h>  // Modbus 库头文件
#include <iostream>         // 输入输出流
#include <cstring>          // 字符串操作
#include <unistd.h>         // Unix 标准函数，如 usleep

enum ModbusRTUCommand_t {
    ManLeftYaw = 0x001E,
    ManRightYaw = 0x001F,
    ManLeftPitch,
    ManRightPitch,  

};

class TurntableController {
private:
    modbus_t* m_ctx;        ///< Modbus 上下文指针，用于所有 Modbus 操作
    const char* m_port;     ///< 串口设备路径，如 "/dev/ttyUSB0"
    int m_slaveId;          ///< 从站设备地址
    bool m_isConnected;     ///< 连接状态标志

public:
    /**
     * @brief 构造函数
     * @param port 串口设备路径，如 "/dev/ttyUSB0" 或 "/dev/ttyS0"
     * @param slaveId 从站设备地址，默认为1
     */
    TurntableController(const char* port, int slaveId = 1);

    /**
     * @brief 析构函数
     * 自动断开连接并释放资源
     */
    ~TurntableController();

    // 连接管理函数
    /**
     * @brief 连接到转台设备
     * @param baudrate 波特率，如 9600, 19200, 115200 等
     * @param parity 校验位：'N'=无校验, 'E'=偶校验, 'O'=奇校验
     * @param dataBit 数据位，通常为8
     * @param stopBit 停止位，通常为1或2
     * @return 连接成功返回 true，失败返回 false
     */
    bool connect(int baudrate = 9600, char parity = 'N', int dataBit = 8, int stopBit = 1);

    /**
     * @brief 断开与转台设备的连接并释放资源
     */
    void disconnect();

    /**
     * @brief 检查连接状态
     * @return 已连接返回true，未连接返回false
     */
    bool is_connected() const { return m_isConnected; }

    // 基本 Modbus 操作函数
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
     * @param addr 起始地址（从0开始，对应Modbus地址40001）
     * @param nb 要写入的寄存器数量
     * @param data 包含要写入数据的数组指针
     * @return 写入成功返回 true，失败返回 false
     */
    bool write_multiple_registers(int addr, int nb, const uint16_t* data);

    /**
     * @brief 读取线圈状态（数字输出）
     * @param addr 起始地址（从0开始，对应Modbus地址00001）
     * @param nb 要读取的线圈数量
     * @param dest 存储读取结果的数组指针（每个元素8位）
     * @return 读取成功返回 true，失败返回 false
     */
    bool read_coils(int addr, int nb, uint8_t* dest);

    /**
     * @brief 写入单个线圈（数字输出）
     * @param addr 线圈地址（从0开始，对应Modbus地址00001）
     * @param status 线圈状态：true=ON(1), false=OFF(0)
     * @return 写入成功返回 true，失败返回 false
     */
    bool write_single_coil(int addr, bool status);

    /**
     * @brief 读取输入状态（数字输入）
     * @param addr 起始地址（从0开始，对应Modbus地址10001）
     * @param nb 要读取的输入数量
     * @param dest 存储读取结果的数组指针
     * @return 读取成功返回 true，失败返回 false
     */
    bool read_input_status(int addr, int nb, uint8_t* dest);

    /**
     * @brief 读取输入寄存器（只读寄存器）
     * @param addr 起始地址（从0开始，对应Modbus地址30001）
     * @param nb 要读取的寄存器数量
     * @param dest 存储读取结果的数组指针
     * @return 读取成功返回 true，失败返回 false
     */
    bool read_input_registers(int addr, int nb, uint16_t* dest);

    // 转台专用控制函数
    /**
     * @brief 设置转台目标位置
     * @param position 目标位置值
     * @param positionRegAddr 位置寄存器地址
     * @return 设置成功返回true，失败返回false
     */
    bool set_target_position(uint16_t position, int positionRegAddr = 0);

    /**
     * @brief 读取转台当前位置
     * @param positionRegAddr 位置寄存器地址
     * @return 读取的位置值，读取失败返回0xFFFF
     */
    uint16_t read_current_position(int positionRegAddr = 0);

    /**
     * @brief 启动转台运动
     * @param startCoilAddr 启动线圈地址
     * @return 启动命令发送成功返回true，失败返回false
     */
    bool start_movement(int startCoilAddr = 0);

    /**
     * @brief 停止转台运动
     * @param stopCoilAddr 停止线圈地址
     * @return 停止命令发送成功返回true，失败返回false
     */
    bool stop_movement(int stopCoilAddr = 1);

    /**
     * @brief 读取转台状态
     * @param statusRegAddr 状态寄存器地址
     * @return 转台状态值，读取失败返回0xFFFF
     */
    uint16_t read_status(int statusRegAddr = 2);

    /**
     * @brief 设置转台速度
     * @param speed 速度值
     * @param speedRegAddr 速度寄存器地址
     * @return 设置成功返回true，失败返回false
     */
    bool set_speed(uint16_t speed, int speedRegAddr = 1);

    // 高级功能函数
    /**
     * @brief 带重试的读取操作
     * @param addr 寄存器地址
     * @param nb 寄存器数量
     * @param dest 目标数组
     * @param maxRetries 最大重试次数
     * @return 成功返回true，失败返回false
     */
    bool read_holding_registers_with_retry(int addr, int nb, uint16_t* dest, int maxRetries = 3);

    /**
     * @brief 设置从站地址
     * @param slaveId 新的从站地址
     */
    void set_slave_id(int slaveId);

    /**
     * @brief 获取当前从站地址
     * @return 当前从站地址
     */
    int get_slave_id() const;

    /**
     * @brief 获取连接的串口设备路径
     * @return 串口设备路径
     */
    const char* get_port() const { return m_port; }
};

#endif // TURNTABLE_CONTROLLER_H