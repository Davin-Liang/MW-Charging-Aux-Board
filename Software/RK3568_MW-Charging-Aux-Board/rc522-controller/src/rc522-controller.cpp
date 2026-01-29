#include "rc522-controller.h"
#include <iostream>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <thread>
#include <chrono>
#include <algorithm> // for std::copy

RC522Controller::RC522Controller(const std::string& devicePath) : fd_(-1) 
{
    fd_ = open(devicePath.c_str(), O_RDWR);
    if (fd_ < 0)
        perror("RC522Controller: Open device failed");
}

RC522Controller::~RC522Controller() 
{
    if (fd_ >= 0) {
        close(fd_);
        std::cout << "RC522Controller: Device closed." << std::endl;
    }
}

bool RC522Controller::is_valid() const 
{
    return fd_ >= 0;
}

// ==========================================
// 私有底层硬件操作 (Private Low-Level)
// ==========================================

uint8_t RC522Controller::read_raw_rc_(uint8_t address) const 
{
    uint8_t buf[1];
    buf[0] = address;
    
    if (read(fd_, buf, 1) != 1) {
        std::cout << "Failed to read a single byte on the address" << std::endl;
        return 0;
    }
    return buf[0];
}

void RC522Controller::write_raw_rc_(uint8_t address, uint8_t value) const 
{
    uint8_t buf[2];
    buf[0] = address;
    buf[1] = value;
    write(fd_, buf, 2);
}

void RC522Controller::set_bit_mask_(uint8_t reg, uint8_t mask) const 
{
    uint8_t temp = read_raw_rc_(reg);
    write_raw_rc_(reg, temp | mask);
}

void RC522Controller::clear_bit_mask_(uint8_t reg, uint8_t mask) const 
{
    uint8_t temp = read_raw_rc_(reg);
    write_raw_rc_(reg, temp & (~mask));
}

void RC522Controller::pcd_antenna_on_() const 
{
    uint8_t temp = read_raw_rc_(TxControlReg);
    if (!(temp & 0x03))
        set_bit_mask_(TxControlReg, 0x03);
}

void RC522Controller::calculate_crc_(const std::vector<uint8_t>& inData, std::vector<uint8_t>& outData) const 
{
    uint8_t i, n;
    
    clear_bit_mask_(DivIrqReg, 0x04);      // 清除 CRC 中断标志
    write_raw_rc_(CommandReg, PCD_IDLE);   // 取消当前命令
    set_bit_mask_(FIFOLevelReg, 0x80);     // 清空 FIFO 指针

    /* 将数据写入 FIFO */
    for (uint8_t byte : inData)
        write_raw_rc_(FIFODataReg, byte);

    write_raw_rc_(CommandReg, PCD_CALCCRC); // 开始计算 CRC

    /* 等待 CRC 计算完成 */
    i = 0xFF;
    do {
        n = read_raw_rc_(DivIrqReg);
        i--;
    } while ((i != 0) && !(n & 0x04));

    /* 读取结果 */
    outData.clear();
    outData.push_back(read_raw_rc_(CRCResultRegL));
    outData.push_back(read_raw_rc_(CRCResultRegM));
}

// ==========================================
// 核心通讯逻辑 (Core Logic)
// ==========================================

char RC522Controller::pcd_com_mf522_(uint8_t command, 
                                     const std::vector<uint8_t>& inData, 
                                     std::vector<uint8_t>& outData, 
                                     uint32_t& outLenBit) 
{
    char status = MI_ERR;
    uint8_t irqEn = 0x00;
    uint8_t waitFor = 0x00;
    uint8_t lastBits;
    uint8_t n;
    uint32_t i;

    /* 根据命令设置中断等待标志 */
    switch (command) {
        case PCD_AUTHENT:
            irqEn = 0x12;
            waitFor = 0x10;
            break;
        case PCD_TRANSCEIVE:
            irqEn = 0x77;
            waitFor = 0x30;
            break;
        default:
            break;
    }

    write_raw_rc_(ComIEnReg, irqEn | 0x80);  // 开启中断
    clear_bit_mask_(ComIrqReg, 0x80);        // 清除中断请求位
    write_raw_rc_(CommandReg, PCD_IDLE);     // 停止当前命令
    set_bit_mask_(FIFOLevelReg, 0x80);       // 清除 FIFO

    /* 写入 FIFO */
    for (uint8_t byte : inData)
        write_raw_rc_(FIFODataReg, byte);

    write_raw_rc_(CommandReg, command);      // 执行命令

    if (command == PCD_TRANSCEIVE)
        set_bit_mask_(BitFramingReg, 0x80);  // 启动发送

    /* 等待命令完成或超时 */
    i = 1000; // 根据时钟频率调整
    do {
        n = read_raw_rc_(ComIrqReg);
        i--;
    } while ((i != 0) && !(n & 0x01) && !(n & waitFor));

    clear_bit_mask_(BitFramingReg, 0x80);    // 停止发送

    if (i != 0) {
        // 检查是否有错误寄存器标志
        if (!(read_raw_rc_(ErrorReg) & 0x1B)) {
            status = MI_OK;
            if (n & irqEn & 0x01)
                status = MI_NOTAGERR;

            if (command == PCD_TRANSCEIVE) {
                n = read_raw_rc_(FIFOLevelReg);           // 读取 FIFO 中的字节数
                lastBits = read_raw_rc_(ControlReg) & 0x07; // 最后接收字节的有效位数
                
                if (lastBits)
                    outLenBit = (n - 1) * 8 + lastBits;
                else
                    outLenBit = n * 8;

                if (n == 0) n = 1;
                if (n > maxRLen_) n = maxRLen_;

                /* 读取数据到输出 Vector */
                outData.clear();
                outData.reserve(n);
                for (uint32_t k = 0; k < n; k++)
                    outData.push_back(read_raw_rc_(FIFODataReg));
            }
        } else
            status = MI_ERR;
    }

    set_bit_mask_(ControlReg, 0x80);     // 停止定时器
    write_raw_rc_(CommandReg, PCD_IDLE); // 恢复空闲
    return status;
}

// ==========================================
// 公有接口实现 (Public API)
// ==========================================

int RC522Controller::pcd_reset() 
{
    if (!is_valid()) return -1;

    /* 硬件复位 */
    write_raw_rc_(CommandReg, 0x0f);
    while (read_raw_rc_(CommandReg) & 0x10); // 等待复位成功
    std::cout << "Hardware reset sucessfully." << std::endl;

    /* 初始化寄存器 */
    write_raw_rc_(ModeReg, 0x3D); // 设置 CRC 预设值
    write_raw_rc_(TReloadRegL, 30); // 设定等待卡片回应的窗口时间
    write_raw_rc_(TReloadRegH, 0);
    write_raw_rc_(TModeReg, 0x8D); // 设置自动启动定时器
    write_raw_rc_(TPrescalerReg, 0x3E); // 设置定时器频率的分频系数
    write_raw_rc_(TxAutoReg, 0x40); // 强制使用 100% ASK 调制
    std::cout << "Initial registers sucessfully." << std::endl;
    
    return 0;
}

char RC522Controller::config_iso_type(uint8_t type) 
{
    if (type == 'A') {
        /* 配置 RC522 模块以支持 ISO 14443A 协议 */
        clear_bit_mask_(Status2Reg, 0x08); // 关闭内部的 Mifare 加密单元
        write_raw_rc_(ModeReg, 0x3D);
        write_raw_rc_(RxSelReg, 0x86); // 接收增益设置为 23dB
        write_raw_rc_(RFCfgReg, 0x7F); // 配置解调器的电压阈值，确保能从载波中正确解调出微弱的卡片信号
        write_raw_rc_(TReloadRegL, 30); // 
        write_raw_rc_(TReloadRegH, 0);
        write_raw_rc_(TModeReg, 0x8D);
        write_raw_rc_(TPrescalerReg, 0x3E);
        
        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // 给振荡器和射频电路一点启动稳定的时间
        pcd_antenna_on_();
    } else
        return MI_ERR;

    return MI_OK;
}

char RC522Controller::pcd_request(uint8_t reqCode, std::vector<uint8_t>& tagType) 
{
    char status;
    uint32_t len;
    std::vector<uint8_t> buffer; // 发送缓冲
    std::vector<uint8_t> recvBuffer; // 接收缓冲

    clear_bit_mask_(Status2Reg, 0x08);
    write_raw_rc_(BitFramingReg, 0x07);
    set_bit_mask_(TxControlReg, 0x03);

    buffer.push_back(reqCode);

    status = pcd_com_mf522_(PCD_TRANSCEIVE, buffer, recvBuffer, len);

    if ((status == MI_OK) && (len == 0x10))
        // 请求成功，返回的数据通常是2字节的卡类型 (ATQA)
        tagType = recvBuffer;
    else
        status = MI_ERR;

    return status;
}

char RC522Controller::pcd_anticoll(std::vector<uint8_t>& snr) {
    char status;
    uint8_t snrCheck = 0;
    uint32_t len;
    std::vector<uint8_t> buffer;
    std::vector<uint8_t> recvBuffer;

    clear_bit_mask_(Status2Reg, 0x08);
    write_raw_rc_(BitFramingReg, 0x00);
    clear_bit_mask_(CollReg, 0x80);

    // 防冲突命令 0x93 0x20
    buffer.push_back(0x93);
    buffer.push_back(0x20);

    status = pcd_com_mf522_(PCD_TRANSCEIVE, buffer, recvBuffer, len);

    if (status == MI_OK) {
        // 期望收到 5 个字节 (4字节 UID + 1字节校验)
        if (recvBuffer.size() >= 5) {
            snr.clear();
            for (int i = 0; i < 4; i++) {
                snr.push_back(recvBuffer[i]);
                snrCheck ^= recvBuffer[i];
            }
            // 校验异或和
            if (snrCheck != recvBuffer[4]) {
                status = MI_ERR;
            }
        } else {
            status = MI_ERR;
        }
    }
    set_bit_mask_(CollReg, 0x80);
    return status;
}

char RC522Controller::pcd_select(const std::vector<uint8_t>& snr) 
{
    if (snr.size() < 4) return MI_ERR;

    char status;
    uint32_t len;
    std::vector<uint8_t> buffer;
    std::vector<uint8_t> recvBuffer;
    std::vector<uint8_t> crcOut;

    // 构造选卡命令
    buffer.push_back(PICC_ANTICOLL1);
    buffer.push_back(0x70);
    
    // 放入UID (前4字节)
    for (int i = 0; i < 4; i++) {
        buffer.push_back(snr[i]);
    }
    // BCC (Block Check Character) 计算：UID的异或
    uint8_t bcc = 0;
    for (int i = 0; i < 4; i++) bcc ^= snr[i];
    buffer.push_back(bcc);

    // 计算 buffer 的 CRC 并追加
    calculate_crc_(buffer, crcOut);
    buffer.insert(buffer.end(), crcOut.begin(), crcOut.end());

    clear_bit_mask_(Status2Reg, 0x08);

    status = pcd_com_mf522_(PCD_TRANSCEIVE, buffer, recvBuffer, len);

    if ((status == MI_OK) && (len == 0x18)) {
        return MI_OK;
    }
    return MI_ERR;
}

char RC522Controller::pcd_auth_state(uint8_t authMode, uint8_t addr, 
                                     const std::vector<uint8_t>& key, 
                                     const std::vector<uint8_t>& snr) 
{
    if (key.size() < 6 || snr.size() < 4) return MI_ERR;

    char status;
    uint32_t len;
    std::vector<uint8_t> buffer;
    std::vector<uint8_t> recvBuffer; // auth 没有实际的数据返回，但需要占位

    // 构造认证数据包: Mode + BlockAddr + Key(6) + UID(4)
    buffer.push_back(authMode);
    buffer.push_back(addr);
    buffer.insert(buffer.end(), key.begin(), key.begin() + 6);
    buffer.insert(buffer.end(), snr.begin(), snr.begin() + 4);

    // 发送认证命令
    status = pcd_com_mf522_(PCD_AUTHENT, buffer, recvBuffer, len);

    // 检查硬件标志位 Status2Reg 的第3位
    if ((status != MI_OK) || (!(read_raw_rc_(Status2Reg) & 0x08))) {
        status = MI_ERR;
    }
    return status;
}

char RC522Controller::pcd_write(uint8_t addr, const std::vector<uint8_t>& data) 
{
    if (data.size() < 16) return MI_ERR;

    char status;
    uint32_t len;
    std::vector<uint8_t> buffer;
    std::vector<uint8_t> recvBuffer;
    std::vector<uint8_t> crcOut;

    // --- 步骤 1: 发送写命令和地址 ---
    buffer.push_back(PICC_WRITE);
    buffer.push_back(addr);
    calculate_crc_(buffer, crcOut);
    buffer.insert(buffer.end(), crcOut.begin(), crcOut.end());

    status = pcd_com_mf522_(PCD_TRANSCEIVE, buffer, recvBuffer, len);

    // 检查响应 (低4位是否为 0x0A - ACK)
    if ((status != MI_OK) || (len != 4) || ((recvBuffer[0] & 0x0F) != 0x0A)) {
        return MI_ERR;
    }

    // --- 步骤 2: 发送数据 ---
    buffer.clear();
    // 复制前16个字节数据
    buffer.insert(buffer.end(), data.begin(), data.begin() + 16);
    calculate_crc_(buffer, crcOut);
    buffer.insert(buffer.end(), crcOut.begin(), crcOut.end());

    status = pcd_com_mf522_(PCD_TRANSCEIVE, buffer, recvBuffer, len);

    // 再次检查响应 ACK
    if ((status != MI_OK) || (len != 4) || ((recvBuffer[0] & 0x0F) != 0x0A)) {
        return MI_ERR;
    }

    return MI_OK;
}

char RC522Controller::pcd_read(uint8_t addr, std::vector<uint8_t>& data) 
{
    char status;
    uint32_t len;
    std::vector<uint8_t> buffer;
    std::vector<uint8_t> crcOut;

    // 发送读命令
    buffer.push_back(PICC_READ);
    buffer.push_back(addr);
    calculate_crc_(buffer, crcOut);
    buffer.insert(buffer.end(), crcOut.begin(), crcOut.end());

    // data 将作为接收缓冲区被 pcd_com_mf522_ 填充
    status = pcd_com_mf522_(PCD_TRANSCEIVE, buffer, data, len);

    // 读卡成功通常返回 16字节数据 + 2字节CRC (共18字节，即 0x90 bits)
    if ((status == MI_OK) && (len == 0x90)) {
        // 移除末尾的2字节 CRC，只给用户返回16字节数据
        if (data.size() >= 16) {
            data.resize(16);
        }
    } else {
        status = MI_ERR;
    }
    return status;
}

char RC522Controller::pcd_halt() 
{
    uint32_t len;
    std::vector<uint8_t> buffer;
    std::vector<uint8_t> recvBuffer;
    std::vector<uint8_t> crcOut;

    buffer.push_back(PICC_HALT);
    buffer.push_back(0);
    calculate_crc_(buffer, crcOut);
    buffer.insert(buffer.end(), crcOut.begin(), crcOut.end());

    pcd_com_mf522_(PCD_TRANSCEIVE, buffer, recvBuffer, len);
    return MI_OK;
}

void RC522Controller::print_hex_data(const char* title, const std::vector<uint8_t>& data) const 
{
    printf("%s", title);
    for (uint8_t byte : data) {
        printf(" 0x%02X", byte);
    }
    printf("\n");
}

// ==========================================
// 测试逻辑 (Test Loop)
// ==========================================

void RC522Controller::run_test_loop() {
    if (pcd_reset() < 0) {
        std::cerr << "PCD Reset Failed" << std::endl;
        return;
    }
    
    if (config_iso_type('A') == MI_ERR) {
        std::cerr << "Config ISO Type Failed" << std::endl;
    } else {
        std::cout << "RC522 Initialized. Scanning..." << std::endl;
    }

    // 默认密钥 (全F)
    std::vector<uint8_t> defaultKey = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    // 测试写入数据 (全0xAA)
    std::vector<uint8_t> writeBuf(16, 0xAA);
    std::vector<uint8_t> readBuf;
    std::vector<uint8_t> cardId; // UID
    
    uint8_t sector = 1;      // 操作第1扇区
    uint8_t blockAddr = sector * 4 + 3; // 该扇区的块地址

    while (true) {
        // 1. 寻卡
        if (pcd_request(PICC_REQIDL, cardId) == MI_OK) {
            
            // 2. 防冲突 (获取完整UID)
            if (pcd_anticoll(cardId) == MI_OK) {
                
                // 3. 选卡
                pcd_select(cardId);
                
                // 4. 验证密码
                char authStatus = pcd_auth_state(KEYA, blockAddr, defaultKey, cardId);
                
                if (authStatus != MI_OK) {
                    printf("Found Card UID: %02X%02X%02X%02X - Auth Error\n", 
                           cardId[0], cardId[1], cardId[2], cardId[3]);
                } else {
                    printf("Found Card UID: %02X%02X%02X%02X - Auth Success\n", 
                           cardId[0], cardId[1], cardId[2], cardId[3]);

                    // 5. 写测试
                    if (pcd_write(blockAddr, writeBuf) == MI_OK) {
                        std::cout << " > Write Block Success" << std::endl;
                    } else {
                        std::cout << " > Write Block Failed" << std::endl;
                    }

                    // 6. 读测试
                    if (pcd_read(blockAddr, readBuf) == MI_OK) {
                        print_hex_data(" > Read Block Data:", readBuf);
                    } else {
                        std::cout << " > Read Block Failed" << std::endl;
                    }
                }
            } else {
                // 防冲突失败，停止卡片
                pcd_halt();
            }
        }
        
        // 降低CPU占用
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}