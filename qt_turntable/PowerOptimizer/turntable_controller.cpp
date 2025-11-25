#include "turntable_controller.h"

/**
 * @brief 构造函数
 * @param port 串口设备路径，如 "/dev/ttyUSB0" 或 "/dev/ttyS0"
 * @param slaveId 从站设备地址，默认为1
 */
TurntableController::TurntableController(const char* port, int slaveId) 
    : m_port(port), m_slaveId(slaveId), m_ctx(nullptr), m_isConnected(false) 
{
    std::cout << "创建转台控制器: 端口=" << port 
              << ", 从站地址=" << slaveId << std::endl;
}

/**
 * @brief 析构函数
 * 自动断开连接并释放资源
 */
TurntableController::~TurntableController() 
{
    disconnect();  // 确保连接被正确关闭
}

/**
 * @brief 连接到转台设备
 * @param baudrate 波特率，如 9600, 19200, 115200 等
 * @param parity 校验位：'N'=无校验, 'E'=偶校验, 'O'=奇校验
 * @param dataBit 数据位，通常为8
 * @param stopBit 停止位，通常为1或2
 * @return 连接成功返回 true，失败返回 false
 */
bool TurntableController::connect(int baudrate, char parity, int dataBit, int stopBit) 
{
    /* 创建 RTU 模式的 Modbus 上下文 */
    m_ctx = modbus_new_rtu(m_port, baudrate, parity, dataBit, stopBit);
    modbus_set_debug(m_ctx, TRUE);
    
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

/**
 * @brief 断开与转台设备的连接并释放资源
 */
void TurntableController::disconnect(void) 
{
    if (m_ctx) {
        std::cout << "断开转台控制器连接..." << std::endl;
        modbus_close(m_ctx);
        modbus_free(m_ctx);
        m_ctx = nullptr;
        m_isConnected = false;
    }
}

/**
 * @brief 读取保持寄存器
 * @param addr 起始地址（从0开始，对应Modbus地址40001）
 * @param nb 要读取的寄存器数量
 * @param dest 存储读取结果的数组指针
 * @return 读取成功返回 true，失败返回 false
 */
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

/**
 * @brief 写入单个保持寄存器
 * @param addr 寄存器地址（从0开始，对应Modbus地址40001）
 * @param value 要写入的16位值
 * @return 写入成功返回 true，失败返回 false
 */
bool TurntableController::write_single_register(int addr, uint16_t value) 
{
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

/**
 * @brief 写入多个保持寄存器
 * @param addr 起始地址（从0开始，对应Modbus地址40001）
 * @param nb 要写入的寄存器数量
 * @param data 包含要写入数据的数组指针
 * @return 写入成功返回 true，失败返回 false
 */
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

/**
 * @brief 读取线圈状态（数字输出）
 * @param addr 起始地址（从0开始，对应Modbus地址00001）
 * @param nb 要读取的线圈数量
 * @param dest 存储读取结果的数组指针（每个元素8位）
 * @return 读取成功返回 true，失败返回 false
 */
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

/**
 * @brief 写入单个线圈（数字输出）
 * @param addr 线圈地址（从0开始，对应Modbus地址00001）
 * @param status 线圈状态：true=ON(1), false=OFF(0)
 * @return 写入成功返回 true，失败返回 false
 */
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

/**
 * @brief 读取输入状态（数字输入）
 * @param addr 起始地址（从0开始，对应Modbus地址10001）
 * @param nb 要读取的输入数量
 * @param dest 存储读取结果的数组指针
 * @return 读取成功返回 true，失败返回 false
 */
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

/**
 * @brief 读取输入寄存器（只读寄存器）
 * @param addr 起始地址（从0开始，对应Modbus地址30001）
 * @param nb 要读取的寄存器数量
 * @param dest 存储读取结果的数组指针
 * @return 读取成功返回 true，失败返回 false
 */
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

/**
 * @brief 设置轴的手动操作
 * @param axis 要转动哪个轴
 * @param dir 左转还是右转
 * @return 操作成功返回 true，失败返回 false
 */
bool TurntableController::set_manual_rotation(enum YawOrPitch_t axis, enum LeftOrRight_t dir)
{
    if (Yaw == axis){
        if (Left == dir)
            return write_single_coil(0x001E, 0xFF00);
        return write_single_coil(0x001F, 0xFF00);        
    }
        
    if (Left == dir)
        return write_single_coil(0x0020, 0xFF00);
    return write_single_coil(0x0021, 0xFF00);
}

/**
 * @brief 设置轴的手动操作
 * @param axis 要设置哪个轴
 * @return 操作成功返回 true，失败返回 false
 */
bool TurntableController::stop_manual_rotation(enum YawOrPitch_t axis)
{
    if (Yaw == axis)
        return write_single_coil(0x001E, 0x0000);
    return write_single_coil(0x001F, 0x0000);
}

/**
 * @brief 轴坐标清零
 * @param axis 要设置哪个轴
 * @return 操作成功返回 true，失败返回 false
 */
bool TurntableController::reset_axis_coord(enum YawOrPitch_t axis)
{
    if (Yaw == axis)
        return write_single_coil(0x0024, 0xFF00);
    return write_single_coil(0x0025, 0xFF00);    
}

/**
 * @brief 设置轴的速度[°/sec]
 * @param axis 要设置哪个轴
 * @param goalSpeed 目标速度
 * @return 操作成功返回 true，失败返回 false
 */
bool TurntableController::set_axis_speed(enum YawOrPitch_t axis, float goalSpeed)
{
    union FloatToBytes_t speed;

    if (goalSpeed > MAX_AXIS_SPEED)
        speed.floatValue = MAX_AXIS_SPEED;
    else
        speed.floatValue = goalSpeed;

    /* 交换高两字节和低两字节 */
    // uint16_t temp = speed.words[0];
    // speed.words[0] = speed.words[1];
    // speed.words[1] = temp;
    
    if (Yaw == axis)
        return write_multiple_registers(0x0007, 2, speed.words);
    return write_multiple_registers(0x0009, 2, speed.words);     
}

/**
 * @brief 读取轴的速度[°/sec]
 * @param axis 哪个轴
 * @param speed 要读取的速度
 * @return 操作成功返回 true，失败返回 false
 */
bool TurntableController::read_axis_speed(enum YawOrPitch_t axis, float * readedSpeed)
{
    union FloatToBytes_t speed;
    bool result;
    
    if (Yaw == axis)
        result = read_input_registers(0x0007, 2, speed.words);
    result = read_input_registers(0x0009, 2, speed.words);

    if (result) {
        /* 交换高两字节和低两字节 */
        uint16_t temp   = speed.words[0];
        speed.words[0]  = speed.words[1];
        speed.words[1]  = temp;   
        *readedSpeed    = speed.floatValue;

        return true;
    }

    return false;
}

/**
 * @brief 设置轴的角度[°]
 * @param axis 哪个轴
 * @param goalAngle 目标角度
 * @return 操作成功返回 true，失败返回 false
 */
bool TurntableController::set_axis_angle(enum YawOrPitch_t axis, float goalAngle)
{
    union FloatToBytes_t angle;

    /* Pitch 轴才有限位 */
    if (Pitch == axis)
        if (goalAngle > MAX_AXIS_PITCH_ANGLE)
            angle.floatValue = MAX_AXIS_PITCH_ANGLE;
        else 
            angle.floatValue = goalAngle;

    /* 交换高两字节和低两字节 */
    // uint16_t temp = angle.words[0];
    // angle.words[0] = angle.words[1];
    // angle.words[1] = temp;
    
    if (Yaw == axis)
        return write_multiple_registers(0x0016, 2, angle.words);
    return write_multiple_registers(0x0018, 2, angle.words);     
}

/**
 * @brief 读取轴的角度[°]
 * @param axis 哪个轴
 * @param angle 要读取的角度
 * @return 操作成功返回 true，失败返回 false
 */
bool TurntableController::read_axis_angle(enum YawOrPitch_t axis, float * readedAngle)
{
    union FloatToBytes_t angle;
    bool result;
    
    if (Yaw == axis)
        result = read_input_registers(0x0016, 2, angle.words);
    result = read_input_registers(0x0018, 2, angle.words);

    if (result) {
        /* 交换高两字节和低两字节 */
        uint16_t temp   = angle.words[0];
        angle.words[0]  = angle.words[1];
        angle.words[1]  = temp;   
        *readedAngle    = angle.floatValue;

        return true;
    }

    return false;
}

/**
 * @brief 设置轴的相对运动[°]
 * @param axis 哪个轴
 * @param angle 要运动的相对角度
 * @return 操作成功返回 true，失败返回 false
 */
bool TurntableController::set_axis_relative_motion(enum YawOrPitch_t axis, float goalAngle)
{
    union FloatToBytes_t angle;

    angle.floatValue = goalAngle;

    /* 交换高两字节和低两字节 */
    // uint16_t temp = angle.words[0];
    // angle.words[0] = angle.words[1];
    // angle.words[1] = temp;
    
    if (Yaw == axis)
        return write_multiple_registers(0x001c, 2, angle.words);
    return write_multiple_registers(0x001e, 2, angle.words);     
}

/**
 * @brief 带重试的读取操作
 * @param addr 寄存器地址
 * @param nb 寄存器数量
 * @param dest 目标数组
 * @param maxRetries 最大重试次数
 * @return 成功返回true，失败返回false
 */
bool TurntableController::read_holding_registers_with_retry(int addr, int nb, uint16_t* dest, int maxRetries) 
{
    for (int attempt = 0; attempt < maxRetries; attempt++) {
        if (read_holding_registers(addr, nb, dest))
            return true;

        std::cout << "读取失败，第 " << (attempt + 1) << "/" << maxRetries << " 次重试" << std::endl;
        usleep(100000); // 等待100ms后重试
    }
    return false;
}

/**
 * @brief 设置从站地址
 * @param slaveId 新的从站地址
 */
void TurntableController::set_slave_id(int slaveId) 
{
    m_slaveId = slaveId;
    if (m_ctx)
        modbus_set_slave(m_ctx, m_slaveId);
}
