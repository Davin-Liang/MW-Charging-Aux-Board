#include "turntable_controller.h"

/* 构造函数 */
TurntableController::TurntableController(const char* port, int slaveId) 
    : m_port(port), m_slaveId(slaveId), m_ctx(nullptr), m_isConnected(false) 
{
    std::cout << "创建转台控制器: 端口=" << port 
              << ", 从站地址=" << slaveId << std::endl;
}

/* 析构函数 */
TurntableController::~TurntableController() 
{
    disconnect();  // 确保连接被正确关闭
}

/* 连接到转台设备 */
bool TurntableController::connect(int baudrate, char parity, int dataBit, int stopBit) 
{
    /* 创建 RTU 模式的 Modbus 上下文 */
    m_ctx = modbus_new_rtu(m_port, baudrate, parity, dataBit, stopBit);
    
    /* 检查上下文是否创建成功 */
    if (m_ctx == nullptr) {
        std::cerr << "错误：无法创建 Modbus 上下文" << std::endl;
        return false;
    }

    /* 设置从站地址（设备地址） */
    modbus_set_slave(m_ctx, m_slaveId);

    /* 设置响应超时时间 */
    modbus_set_response_timeout(m_ctx, 1, 0);

    /* 设置字节超时时间 */
    modbus_set_byte_timeout(m_ctx, 0, 100000);

    /* 建立实际连接 */
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

/* 断开与转台设备的连接并释放资源 */
void TurntableController::disconnect() 
{
    if (m_ctx) {
        std::cout << "断开转台控制器连接..." << std::endl;
        modbus_close(m_ctx);
        modbus_free(m_ctx);
        m_ctx = nullptr;
        m_isConnected = false;
    }
}

/* 读取保持寄存器 */
bool TurntableController::read_holding_registers(int addr, int nb, uint16_t* dest) 
{
    if (!m_ctx) {
        std::cerr << "错误：Modbus 上下文未初始化" << std::endl;
        return false;
    }

    std::cout << "读取保持寄存器: 起始地址=" << addr 
              << " (4000" << (addr + 1) << "), 数量=" << nb << std::endl;

    /* 读取保持寄存器 */
    int rc = modbus_read_registers(m_ctx, addr, nb, dest);
    
    if (rc == -1) {
        std::cerr << "读取寄存器失败: " << modbus_strerror(errno) << std::endl;
        return false;
    }

    std::cout << "成功读取 " << rc << " 个寄存器" << std::endl;
    return true;
}

/* 写入单个保持寄存器 */
bool TurntableController::write_single_register(int addr, uint16_t value) {
    if (!m_ctx) {
        std::cerr << "错误：Modbus 上下文未初始化" << std::endl;
        return false;
    }

    std::cout << "写入单个寄存器: 地址=" << addr 
              << " (4000" << (addr + 1) << "), 值=" << value << std::endl;

    /* 写入单个寄存器 */
    int rc = modbus_write_register(m_ctx, addr, value);
    
    if (rc == -1) {
        std::cerr << "写入寄存器失败: " << modbus_strerror(errno) << std::endl;
        return false;
    }

    std::cout << "写入寄存器成功" << std::endl;
    return true;
}

/* 写入多个保持寄存器 */
bool TurntableController::write_multiple_registers(int addr, int nb, const uint16_t* data) 
{
    if (!m_ctx) {
        std::cerr << "错误：Modbus 上下文未初始化" << std::endl;
        return false;
    }

    std::cout << "写入多个寄存器: 起始地址=" << addr 
              << " (4000" << (addr + 1) << "), 数量=" << nb << std::endl;

    /* 写入多个寄存器 */
    int rc = modbus_write_registers(m_ctx, addr, nb, data);
    
    if (rc == -1) {
        std::cerr << "写入多个寄存器失败: " << modbus_strerror(errno) << std::endl;
        return false;
    }

    std::cout << "写入多个寄存器成功" << std::endl;
    return true;
}

/* 读取线圈状态 */
bool TurntableController::read_coils(int addr, int nb, uint8_t* dest) 
{
    if (!m_ctx) {
        std::cerr << "错误：Modbus 上下文未初始化" << std::endl;
        return false;
    }

    std::cout << "读取线圈: 起始地址=" << addr 
              << " (0000" << (addr + 1) << "), 数量=" << nb << std::endl;

    // 读取线圈状态
    int rc = modbus_read_bits(m_ctx, addr, nb, dest);
    
    if (rc == -1) {
        std::cerr << "读取线圈失败: " << modbus_strerror(errno) << std::endl;
        return false;
    }

    std::cout << "成功读取 " << rc << " 个线圈状态" << std::endl;
    return true;
}

/* 写入单个线圈 */
bool TurntableController::write_single_coil(int addr, bool status) 
{
    if (!m_ctx) {
        std::cerr << "错误：Modbus 上下文未初始化" << std::endl;
        return false;
    }

    std::cout << "写入单个线圈: 地址=" << addr 
              << " (0000" << (addr + 1) << "), 状态=" 
              << (status ? "ON" : "OFF") << std::endl;

    /* 写入单个线圈 */
    int rc = modbus_write_bit(m_ctx, addr, status ? 1 : 0);
    
    if (rc == -1) {
        std::cerr << "写入线圈失败: " << modbus_strerror(errno) << std::endl;
        return false;
    }

    std::cout << "写入线圈成功" << std::endl;
    return true;
}

/* 读取输入状态 */
bool TurntableController::read_input_status(int addr, int nb, uint8_t* dest) 
{
    if (!m_ctx) {
        std::cerr << "错误：Modbus 上下文未初始化" << std::endl;
        return false;
    }

    std::cout << "读取输入状态: 起始地址=" << addr 
              << " (1000" << (addr + 1) << "), 数量=" << nb << std::endl;

    // 读取输入状态
    int rc = modbus_read_input_bits(m_ctx, addr, nb, dest);
    
    if (rc == -1) {
        std::cerr << "读取输入状态失败: " << modbus_strerror(errno) << std::endl;
        return false;
    }

    std::cout << "成功读取 " << rc << " 个输入状态" << std::endl;
    return true;
}

/* 读取输入寄存器 */
bool TurntableController::read_input_registers(int addr, int nb, uint16_t* dest) 
{
    if (!m_ctx) {
        std::cerr << "错误：Modbus 上下文未初始化" << std::endl;
        return false;
    }

    std::cout << "读取输入寄存器: 起始地址=" << addr 
              << " (3000" << (addr + 1) << "), 数量=" << nb << std::endl;

    /* 读取输入寄存器 */
    int rc = modbus_read_input_registers(m_ctx, addr, nb, dest);
    
    if (rc == -1) {
        std::cerr << "读取输入寄存器失败: " << modbus_strerror(errno) << std::endl;
        return false;
    }

    std::cout << "成功读取 " << rc << " 个输入寄存器" << std::endl;
    return true;
}

// 设置转台目标位置
bool TurntableController::set_target_position(uint16_t position, int positionRegAddr) {
    std::cout << "设置转台目标位置: " << position << " (寄存器地址: " << positionRegAddr << ")" << std::endl;
    return write_single_register(positionRegAddr, position);
}

// 读取转台当前位置
uint16_t TurntableController::read_current_position(int positionRegAddr) {
    uint16_t position = 0;
    if (read_holding_registers(positionRegAddr, 1, &position)) {
        std::cout << "读取转台当前位置: " << position << std::endl;
        return position;
    } else {
        std::cerr << "读取转台当前位置失败" << std::endl;
        return 0xFFFF; // 返回错误值
    }
}

// 启动转台运动
bool TurntableController::start_movement(int startCoilAddr) {
    std::cout << "启动转台运动 (线圈地址: " << startCoilAddr << ")" << std::endl;
    return write_single_coil(startCoilAddr, true);
}

// 停止转台运动
bool TurntableController::stop_movement(int stopCoilAddr) {
    std::cout << "停止转台运动 (线圈地址: " << stopCoilAddr << ")" << std::endl;
    return write_single_coil(stopCoilAddr, true);
}

/**
 * @brief 读取转台状态
 * @param statusRegAddr 状态寄存器地址
 * @return 转台状态值，读取失败返回0xFFFF
 */
uint16_t TurntableController::read_status(int statusRegAddr) {
    uint16_t status = 0;
    if (read_holding_registers(statusRegAddr, 1, &status)) {
        std::cout << "读取转台状态: " << status << std::endl;
        return status;
    } else {
        std::cerr << "读取转台状态失败" << std::endl;
        return 0xFFFF; // 返回错误值
    }
}

// 设置转台速度
bool TurntableController::set_speed(uint16_t speed, int speedRegAddr) {
    std::cout << "设置转台速度: " << speed << " (寄存器地址: " << speedRegAddr << ")" << std::endl;
    return write_single_register(speedRegAddr, speed);
}

// 带重试的读取操作
bool TurntableController::read_holding_registers_with_retry(int addr, int nb, uint16_t* dest, int maxRetries) {
    for (int attempt = 0; attempt < maxRetries; attempt++) {
        if (read_holding_registers(addr, nb, dest)) {
            return true;
        }
        std::cout << "读取失败，第 " << (attempt + 1) << "/" << maxRetries << " 次重试" << std::endl;
        usleep(100000); // 等待100ms后重试
    }
    return false;
}

// 设置从站地址
void TurntableController::set_slave_id(int slaveId) {
    m_slaveId = slaveId;
    if (m_ctx) {
        modbus_set_slave(m_ctx, m_slaveId);
    }
}

// 获取当前从站地址
int TurntableController::get_slave_id() const {
    return m_slaveId;
}