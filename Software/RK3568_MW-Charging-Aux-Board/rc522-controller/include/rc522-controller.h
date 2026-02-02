#ifndef RC522_CONTROLLER_H
#define RC522_CONTROLLER_H

#include <string>
#include <cstdint>
#include <vector>

extern "C" {
    #include "rc522.h"
}

class RC522Controller {
public:
    explicit RC522Controller(const std::string& devicePath);
    ~RC522Controller();
    bool is_valid() const;

    int pcd_reset();
    char config_iso_type(uint8_t type);

    /**
     * @brief 寻卡
     * @param reqCode 模式
     * @param tagType [输出] 返回卡片类型 (会被 resize 为 2 字节)
     */
    char pcd_request(uint8_t reqCode, std::vector<uint8_t>& tagType);

    /**
     * @brief 防冲突
     * @param snr [输出] 返回 4 字节序列号
     */
    char pcd_anticoll(std::vector<uint8_t>& snr);

    /**
     * @brief 选卡
     * @param snr [输入] 卡片序列号 (需要传入 vector)
     */
    char pcd_select(const std::vector<uint8_t>& snr);

    /**
     * @brief 验证密码
     * @param key [输入] 6字节密钥
     * @param snr [输入] 4字节序列号
     */
    char pcd_auth_state(uint8_t authMode, uint8_t addr, 
                        const std::vector<uint8_t>& key, 
                        const std::vector<uint8_t>& snr);

    /**
     * @brief 写块
     * @param data [输入] 16字节数据
     */
    char pcd_write(uint8_t addr, const std::vector<uint8_t>& data);

    /**
     * @brief 读块
     * @param data [输出] 返回 16 字节数据
     */
    char pcd_read(uint8_t addr, std::vector<uint8_t>& data);

    char pcd_halt();

    // 辅助函数也改为引用
    void print_hex_data(const char* title, const std::vector<uint8_t>& data) const;
    
    void run_test_loop();

private:
    // ==========================================
    // 私有成员变量 (Naming: xxxXxx_)
    // ==========================================
    int fd_;                ///< 设备文件描述符 (SPI/I2C/UART 句柄)
    const int maxRLen_ = 18; ///< 接收缓冲区的最大字节长度

    // ==========================================
    // 私有底层硬件操作函数 (Naming: xxx_xxx_xxx_)
    // ==========================================

    /**
     * @brief [底层] 读取 RC522 寄存器的一个字节
     * @details 通过文件描述符读取指定地址的寄存器值
     * @param address 寄存器地址
     * @return uint8_t 读取到的寄存器值
     */
    uint8_t read_raw_rc_(uint8_t address) const;

    /**
     * @brief [底层] 向 RC522 寄存器写入一个字节
     * @details 通过文件描述符向指定地址写入数值
     * @param address 寄存器地址
     * @param value 要写入的数值
     */
    void write_raw_rc_(uint8_t address, uint8_t value) const;

    /**
     * @brief [位操作] 置位寄存器位 (Set Bit)
     * @details 读-改-写 操作：将寄存器指定位置 1，其他位保持不变
     * 逻辑：Reg = Reg | mask
     * @param reg 寄存器地址
     * @param mask 掩码 (要置 1 的位为 1)
     */
    void set_bit_mask_(uint8_t reg, uint8_t mask) const;

    /**
     * @brief [位操作] 清除寄存器位 (Clear Bit)
     * @details 读-改-写 操作：将寄存器指定位置 0，其他位保持不变
     * 逻辑：Reg = Reg & (~mask)
     * @param reg 寄存器地址
     * @param mask 掩码 (要清除的位为 1)
     */
    void clear_bit_mask_(uint8_t reg, uint8_t mask) const;

    /**
     * @brief [控制] 开启天线
     * @details 设置 TxControlReg 寄存器，发射 13.56MHz 载波
     * 每次复位或初始化后通常需要调用此函数，否则无法与卡片通讯
     */
    void pcd_antenna_on_() const;

    /**
     * @brief [计算] 使用 RC522 内部协处理器计算 CRC16
     * @details 将数据写入 FIFO，发出 PCD_CALCCRC 命令，等待计算完成
     * @param inData [输入] 需要计算 CRC 的数据
     * @param outData [输出] 计算结果 (低位在前，高位在后)，通常会 append 到 vector 中
     */
    void calculate_crc_(const std::vector<uint8_t>& inData, std::vector<uint8_t>& outData) const;

    /**
     * @brief [核心] RC522 通讯核心处理函数
     * @details 处理所有的硬件交互流程：
     * 1. 配置中断位
     * 2. 清空 FIFO 并写入发送数据
     * 3. 发送命令 (如 TRANSCEIVE 或 AUTHENT)
     * 4. 启动发送 (BitFraming)
     * 5. 循环等待中断或超时 (Timeout)
     * 6. 读取状态寄存器判断是否出错
     * 7. 从 FIFO 读取返回的数据
     * * @param command RC522 命令字 (如 PCD_TRANSCEIVE, PCD_AUTHENT)
     * @param inData [输入] 发送给卡片的数据
     * @param outData [输出] 接收到的卡片回复数据
     * @param outLenBit [输出] 接收到的实际有效位数 (Bit 数量)
     * @return char 状态码 (MI_OK, MI_ERR, MI_NOTAGERR 等)
     */
    char pcd_com_mf522_(uint8_t command, 
                        const std::vector<uint8_t>& inData, 
                        std::vector<uint8_t>& outData, 
                        uint32_t& outLenBit);
};

#endif
