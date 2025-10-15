#include "command_struct.h"
#include "command_transmitter.h"
#include <cstring>

CommandTransmitter::CommandTransmitter()
{
    std::cout << "CommandTransmitter constructor called" << std::endl;
}

/**
  * @brief  构建命令帧
  * @param  data 数据地址
  * @param  len 数据长度
  * @return -1 数据长度超出限制 | [有效数据长度]
  **/
int CommandTransmitter::build_command_frame(uint8_t* buffer, CommandType_t cmdType, 
                       uint16_t seqNum, const void* data, uint16_t dataLen) 
{
    if (dataLen > 128) return -1;
    
    CommandFrame_t * frame = (CommandFrame_t*)buffer;
    
    /* 填充命令头 */
    frame->header.startMagic = 0xAA;
    frame->header.cmdId = cmdType;
    frame->header.seqNum = seqNum;
    frame->header.dataLen = dataLen;
    
    /* 拷贝数据 */
    if (dataLen > 0 && data != NULL)
        std::memcpy(frame->payload.rawData, data, dataLen);
    
    // 计算校验和（只计算头部）
    frame->header.checksum = calculate_checksum((uint8_t*)&frame->header, 
                                               sizeof(CmdHeader_t) - 1);
    
    return sizeof(CmdHeader_t) + dataLen;
}

/**
  * @brief  计算校验和
  * @param  data 数据地址
  * @param  len 数据长度
  * @return void
  **/
uint8_t CommandTransmitter::calculate_checksum(const uint8_t* data, uint16_t len) 
{
    uint8_t sum = 0;
    for (uint16_t i = 0; i < len; i++) 
    {
        sum += data[i];
    }
    return ~sum + 1; // 补码
}

/**
  * @brief  解析命令帧
  * @param  buffer 数据地址
  * @param  len 数据长度
  * @param  cmd 命令结构体
  * @return -1 数据长度过于小 | -2 命令帧帧头不对 | -3 命令帧校验和不对 | -4 数据长度不对
  **/
int CommandTransmitter::parse_command_frame(const uint8_t * buffer, uint16_t len, CommandFrame_t * cmd) 
{
    if(len < sizeof(CmdHeader_t)) return -1;
    
    std::memcpy(cmd, buffer, sizeof(CmdHeader_t));
    
    /* 验证起始标志 */
    if(cmd->header.startMagic != 0xAA) return -2;
    
    /* 验证校验和 */
    uint8_t calc_checksum = calculate_checksum(buffer, sizeof(CmdHeader_t) - 1);
    if(calc_checksum != cmd->header.checksum) return -3;
    
    /* 验证数据长度 */
    if(len < sizeof(CmdHeader_t) + cmd->header.dataLen) return -4;
    
    /* 拷贝数据部分 */
    if(cmd->header.dataLen > 0)
        std::memcpy(cmd->payload.rawData, buffer + sizeof(CmdHeader_t), cmd->header.dataLen);
    
    return 0;
}

/**
  * @brief  发送电机命令帧
  * @param  sock socket 编号
  * @param  x 目标电机位置 X 坐标
  * @param  y 目标电机位置 Y 坐标
  * @param  speed 目标电机速度
  * @return y 目标电机位置 Y 坐标
  **/
int CommandTransmitter::send_motor_command(int sock, float x, float y, uint16_t speed = 0) 
{
    uint8_t buffer[128];
    motorCmd.x = x;
    motorCmd.y = y;
    if (speed != 0)
        motorCmd.speed = speed;
    
    int frameLen = build_command_frame(buffer, CMD_MOTOR_CONTROL, &motorCmd, sizeof(MotorCmd_t));
    
    if(frameLen > 0) 
        return send(sock, buffer, frameLen, 0); // TODO: 可能要改成 QT socket 的发送函数，这里用的是 socket 的标准版本的发送函数

    return -1;
}

int CommandTransmitter::send_motor_command(void) 
{
    uint8_t buffer[128];
    
    int frameLen = build_command_frame(buffer, CMD_MOTOR_CONTROL, &motorCmd, sizeof(MotorCmd_t));
    
    if(frameLen > 0) 
        return send(sock, buffer, frameLen, 0); // TODO: 可能要改成 QT socket 的发送函数，这里用的是 socket 的标准版本的发送函数

    return -1;
}

int CommandTransmitter::send_find_opt_command(int sock, ThajType_t whichThaj, 
                                                float cirTrajRad, uint8_t squThajStepLen, 
                                                float maxVolfloat volStepLen) 
{
    uint8_t buffer[128];
    findOptCmd.whichThaj = whichThaj;
    findOptCmd.cirTrajRad = cirTrajRad;
    findOptCmd.squThajStepLen = squThajStepLen;
    findOptCmd.volStepLen = volStepLen;
    
    int frameLen = build_command_frame(buffer, CMD_FIND_OPT_RES, &findOptCmd, sizeof(FindOptimalCmd_t));
    
    if(frameLen > 0) 
        return send(sock, buffer, frameLen, 0); // TODO: 可能要改成 QT socket 的发送函数，这里用的是 socket 的标准版本的发送函数

    return -1;
}

int CommandTransmitter::send_find_opt_command(void) 
{
    uint8_t buffer[128];
    
    int frameLen = build_command_frame(buffer, CMD_FIND_OPT_RES, &findOptCmd, sizeof(FindOptimalCmd_t));
    
    if(frameLen > 0) 
        return send(sock, buffer, frameLen, 0); // TODO: 可能要改成 QT socket 的发送函数，这里用的是 socket 的标准版本的发送函数

    return -1;
}
