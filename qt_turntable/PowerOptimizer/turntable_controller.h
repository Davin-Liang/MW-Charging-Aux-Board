/**
 * @file turntable_controller.h
 * @brief 转台控制器头文件
 * 通过 Modbus RTU 协议控制转台设备的类
 */

#ifndef TURNTABLE_CONTROLLER_H
#define TURNTABLE_CONTROLLER_H

#include <modbus/modbus.h>
#include <iostream>         // 输入输出流
#include <cstring>          // 字符串操作
#include <unistd.h>         // Unix 标准函数，如 usleep

#define MAX_AXIS_SPEED 30.f // 单位[°/sec]
#define MAX_AXIS_PITCH_ANGLE 60.f // 单位[°]

enum LeftOrRight_t {
    Left,
    Right
};

enum YawOrPitch_t {
    Yaw,
    Pitch
};

union FloatToBytes_t {
    float floatValue;
    uint16_t words[2];
};

class TurntableController {
private:
    modbus_t* m_ctx;        ///< Modbus 上下文指针，用于所有 Modbus 操作
    const char* m_port;     ///< 串口设备路径，如 "/dev/ttyUSB0"
    int m_slaveId;          ///< 从站设备地址
    bool m_isConnected;     ///< 连接状态标志
    // 浮点数3412字节顺序转换
    void floatTo3412(float value, uint16_t words[2]);
    float floatFrom3412(const uint16_t words[2]);

public:
    TurntableController(const char* port, int slaveId = 0xcc);
    ~TurntableController();

    // 连接管理函数
    bool connect(int baudrate = 9600, char parity = 'N', int dataBit = 8, int stopBit = 1);
    void disconnect(void);

    /**
     * @brief 检查连接状态
     * @return 已连接返回true，未连接返回false
     */
    bool is_connected() const { return m_isConnected; }

    // 基本 Modbus 操作函数
    bool read_holding_registers(int addr, int nb, uint16_t* dest);
    bool write_single_register(int addr, uint16_t value);
    bool write_multiple_registers(int addr, int nb, const uint16_t* data);
    bool read_coils(int addr, int nb, uint8_t* dest);
    bool write_single_coil(int addr, bool status);
    bool read_input_status(int addr, int nb, uint8_t* dest);
    bool read_input_registers(int addr, int nb, uint16_t* dest);

    // 控制转台函数
    bool set_manual_rotation(enum YawOrPitch_t axis, enum LeftOrRight_t dir);
    bool stop_manual_rotation(enum YawOrPitch_t axis);
    bool reset_axis_coord(enum YawOrPitch_t axis);
    bool set_axis_speed(enum YawOrPitch_t axis, float goalSpeed);
    bool read_axis_speed(enum YawOrPitch_t axis, float * readedSpeed);
    bool set_axis_angle(enum YawOrPitch_t axis, float goalAngle);
    bool read_axis_angle(enum YawOrPitch_t axis, float * readedAngle);
    bool set_axis_relative_motion(enum YawOrPitch_t axis, float goalAngle);

    // 高级功能函数
    bool read_holding_registers_with_retry(int addr, int nb, uint16_t* dest, int maxRetries = 3);

    void set_slave_id(int slaveId);

    /**
     * @brief 获取当前从站地址
     * @return 当前从站地址
     */
    int get_slave_id() const { return m_slaveId; }
    const char* get_port() const { return m_port; }
    const bool get_connect_status() const { return m_isConnected; }
};

#endif // TURNTABLE_CONTROLLER_H
