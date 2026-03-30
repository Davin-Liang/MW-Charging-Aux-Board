#include "turntable_controller.h"
#include <iostream>
#include <cstdio>
#include <limits>
#include <cmath>
#include <cstring>
#include <unistd.h>
#include <cerrno>

// 仅在内部使用的共用体
union FloatToBytes_t {
    float floatValue;
    uint16_t words[2];
};

TurntableController::TurntableController(const char* port, int slaveId) 
    : m_ctx(nullptr), m_port(port), m_slaveId(slaveId), m_isConnected(false) 
{
    std::cout << "创建转台控制器: 端口=" << port 
              << ", 从站地址=" << slaveId << std::endl;
}

TurntableController::~TurntableController() 
{
    disconnect();
}

bool TurntableController::connect(int baudrate, char parity, int dataBit, int stopBit) 
{
    if (m_isConnected) {
        std::cout << "警告：尝试连接已连接的设备，先断开现有连接" << std::endl;
        disconnect();
    }

    m_ctx = modbus_new_rtu(m_port, baudrate, parity, dataBit, stopBit);
    modbus_set_debug(m_ctx, false);
    
    if (m_ctx == nullptr) {
        std::cerr << "错误：无法创建 Modbus 上下文" << std::endl;
        return false;
    }

    modbus_set_slave(m_ctx, m_slaveId);
    modbus_set_response_timeout(m_ctx, 1, 0);
    modbus_set_byte_timeout(m_ctx, 0, 1000);

    if (modbus_connect(m_ctx) == -1) {
        std::cerr << "连接失败: " << modbus_strerror(errno) << std::endl;
        modbus_free(m_ctx);
        m_ctx = nullptr;
        m_isConnected = false;
        return false;
    }

    std::cout << "转台控制器连接成功: " << m_port 
              << " 波特率=" << baudrate << " 地址=" << m_slaveId << std::endl;
    m_isConnected = true;
    return true;
}

void TurntableController::disconnect() 
{
    if (m_ctx) {
        std::cout << "断开转台控制器连接..." << std::endl;
        modbus_close(m_ctx);
        modbus_free(m_ctx);
        m_ctx = nullptr;
        m_isConnected = false;
        std::cout << "转台控制器连接已断开" << std::endl;
    }
}

bool TurntableController::read_holding_registers(int addr, int nb, uint16_t* dest) 
{
    if (!m_ctx) return false;

    int rc = modbus_read_registers(m_ctx, addr, nb, dest);
    if (rc == -1) {
        std::cerr << "读取寄存器失败: " << modbus_strerror(errno) << std::endl;
        return false;
    }
    return true;
}

bool TurntableController::write_single_register(int addr, uint16_t value) 
{
    if (!m_ctx) return false;

    int rc = modbus_write_register(m_ctx, addr, value);
    if (rc == -1) {
        std::cerr << "写入寄存器失败: " << modbus_strerror(errno) << std::endl;
        return false;
    }
    return true;
}

bool TurntableController::write_multiple_registers(int addr, int nb, const uint16_t* data) 
{
    if (!m_ctx) return false;

    int rc = modbus_write_registers(m_ctx, addr, nb, data);
    if (rc == -1) {
        std::cerr << "写入多个寄存器失败: " << modbus_strerror(errno) << std::endl;
        return false;
    }
    return true;
}

bool TurntableController::read_coils(int addr, int nb, uint8_t* dest) 
{
    if (!m_ctx) return false;

    int rc = modbus_read_bits(m_ctx, addr, nb, dest);
    if (rc == -1) {
        std::cerr << "读取线圈失败: " << modbus_strerror(errno) << std::endl;
        return false;
    }
    return true;
}

bool TurntableController::write_single_coil(int addr, bool status) 
{
    if (!m_ctx) return false;

    int rc = modbus_write_bit(m_ctx, addr, status ? 1 : 0);
    if (rc == -1) {
        std::cerr << "写入线圈失败: " << modbus_strerror(errno) << std::endl;
        return false;
    }
    return true;
}

bool TurntableController::read_input_status(int addr, int nb, uint8_t* dest) 
{
    if (!m_ctx) return false;

    int rc = modbus_read_input_bits(m_ctx, addr, nb, dest);
    if (rc == -1) {
        std::cerr << "读取输入状态失败: " << modbus_strerror(errno) << std::endl;
        return false;
    }
    return true;
}

bool TurntableController::read_input_registers(int addr, int nb, uint16_t* dest) 
{
    if (!m_ctx) return false;

    int rc = modbus_read_input_registers(m_ctx, addr, nb, dest);
    if (rc == -1) {
        std::cerr << "读取输入寄存器失败: " << modbus_strerror(errno) << std::endl;
        return false;
    }
    return true;
}

bool TurntableController::set_manual_rotation(YawOrPitch_t axis, LeftOrRight_t dir)
{
    if (axis == YawOrPitch_t::Yaw) {
        if (dir == LeftOrRight_t::Left)
            return write_single_coil(0x001E, 0xFF00);
        return write_single_coil(0x001F, 0xFF00);        
    }
        
    if (dir == LeftOrRight_t::Left)
        return write_single_coil(0x0020, 0xFF00);
    return write_single_coil(0x0021, 0xFF00);
}

bool TurntableController::stop_manual_rotation(YawOrPitch_t axis)
{
    if (axis == YawOrPitch_t::Yaw)
        return write_single_coil(0x001E, 0x0000);
    return write_single_coil(0x0020, 0x0000);
}

bool TurntableController::reset_axis_coord(YawOrPitch_t axis)
{
    if (axis == YawOrPitch_t::Yaw)
        return write_single_coil(0x0024, 0xFF00);
    return write_single_coil(0x0025, 0xFF00);    
}

bool TurntableController::set_axis_speed(YawOrPitch_t axis, float goalSpeed)
{   
    uint16_t words[2];

    if (goalSpeed > MAX_AXIS_SPEED)
        goalSpeed = MAX_AXIS_SPEED;

    floatTo3412(goalSpeed, words);

    if (axis == YawOrPitch_t::Yaw)
        return write_multiple_registers(0x0007, 2, words);
    return write_multiple_registers(0x0009, 2, words);     
}

bool TurntableController::read_axis_speed(YawOrPitch_t axis, float* readedSpeed)
{
    uint16_t words[2];
    bool result;

    if (axis == YawOrPitch_t::Yaw)
        result = read_input_registers(0x0007, 2, words);
    else
        result = read_input_registers(0x0009, 2, words);

    if (result) {
        *readedSpeed = floatFrom3412(words);
        return true;
    }
    return false;
}

bool TurntableController::set_axis_angle(YawOrPitch_t axis, float goalAngle)
{
    uint16_t words[2];

    if (axis == YawOrPitch_t::Pitch && goalAngle > MAX_AXIS_PITCH_ANGLE)
        goalAngle = MAX_AXIS_PITCH_ANGLE;

    floatTo3412(goalAngle, words);

    if (axis == YawOrPitch_t::Yaw)
        return write_multiple_registers(0x0016, 2, words);
    return write_multiple_registers(0x0018, 2, words);
}

bool TurntableController::read_axis_angle(YawOrPitch_t axis, float* readedAngle)
{
    uint16_t words[2];
    bool result;

    if (axis == YawOrPitch_t::Yaw)
        result = read_input_registers(0x0016, 2, words);
    else
        result = read_input_registers(0x0018, 2, words);

    if (result) {
        *readedAngle = floatFrom3412(words);
        return true;
    }
    return false;
}

bool TurntableController::set_axis_relative_motion(YawOrPitch_t axis, float goalAngle)
{
    FloatToBytes_t angle;
    angle.floatValue = goalAngle;
    
    if (axis == YawOrPitch_t::Yaw)
        return write_multiple_registers(0x001C, 2, angle.words);
    return write_multiple_registers(0x001E, 2, angle.words);    
}

bool TurntableController::read_holding_registers_with_retry(int addr, int nb, uint16_t* dest, int maxRetries) 
{
    for (int attempt = 0; attempt < maxRetries; attempt++) {
        if (read_holding_registers(addr, nb, dest))
            return true;
        usleep(100000); // 延迟100ms重试
    }
    return false;
}

void TurntableController::set_slave_id(int slaveId) 
{
    m_slaveId = slaveId;
    if (m_ctx)
        modbus_set_slave(m_ctx, m_slaveId);
}

void TurntableController::floatTo3412(float value, uint16_t words[2])
{
    uint32_t float_bits;
    std::memcpy(&float_bits, &value, sizeof(float));
    
    // 提取4个字节（大端序）
    uint8_t bytes[4];
    bytes[0] = (float_bits >> 24) & 0xFF;  // 最高字节（符号位+指数高7位）
    bytes[1] = (float_bits >> 16) & 0xFF;  // 指数低1位+尾数高7位
    bytes[2] = (float_bits >> 8) & 0xFF;   // 尾数中间8位
    bytes[3] = float_bits & 0xFF;          // 尾数低8位
    
    // 3412顺序转换：B1 B2 B3 B4 -> B3 B4 B1 B2
    words[0] = (bytes[2] << 8) | bytes[3];  // B3B4
    words[1] = (bytes[0] << 8) | bytes[1];  // B1B2
}

float TurntableController::floatFrom3412(const uint16_t words[2])
{
    uint8_t bytes[4];
    uint8_t temp[4];

    // 从两个16位字提取字节
    bytes[0] = (words[0] >> 8) & 0xFF;
    bytes[1] = words[0];
    bytes[2] = (words[1] >> 8) & 0xFF;
    bytes[3] = words[1];

    // 3412字节顺序转换回原始顺序
    temp[0] = bytes[2];  // B1
    temp[1] = bytes[3];  // B2
    temp[2] = bytes[0];  // B3
    temp[3] = bytes[1];  // B4

    // 解析IEEE 754单精度浮点数（32位）
    uint32_t bits = (static_cast<uint32_t>(temp[0]) << 24) |
                    (static_cast<uint32_t>(temp[1]) << 16) |
                    (static_cast<uint32_t>(temp[2]) << 8) |
                    static_cast<uint32_t>(temp[3]);

    int sign = (bits >> 31) ? -1 : 1;
    int exponent = ((bits >> 23) & 0xFF) - 127;
    uint32_t mantissa = bits & 0x7FFFFF;

    float float_value;
    if (exponent == -127 && mantissa == 0) {
        float_value = 0.0f * sign;
    } else if (exponent == 128) {
        float_value = (mantissa == 0) ? 
                    (sign * std::numeric_limits<float>::infinity()) : 
                    std::numeric_limits<float>::quiet_NaN();
    } else {
        float_value = sign * (1.0f + static_cast<float>(mantissa) / (1 << 23)) * std::pow(2.0f, exponent);
    }

    return float_value;
}