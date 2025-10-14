// command_handler.c
#include "command_struct.h"
#include "stdio.h"
#include <string.h> 

#define  PC_OR_32 0 // 1 代表 电脑端 | 0 代表32端

/**
  * @brief  计算校验和
  * @param  data 数据地址
  * @param  len 数据长度
  * @return void
  **/
uint8_t calculate_checksum(const uint8_t* data, uint16_t len) 
{
    uint8_t sum = 0;
    for(uint16_t i = 0; i < len; i++) 
    {
        sum += data[i];
    }
    return ~sum + 1;  // 补码
}

/**
  * @brief  构建命令帧
  * @param  data 数据地址
  * @param  len 数据长度
  * @return -1 数据长度超出限制 | [有效数据长度]
  **/
int build_command_frame(uint8_t* buffer, CommandType_t cmdType, 
                       uint16_t seqNum, const void* data, uint16_t dataLen) 
{
    if (dataLen > 128) return -1;
    
    CommandFrame_t * frame = (CommandFrame_t*)buffer;
    
    // 填充命令头
    frame->header.startMagic = 0xAA;
    frame->header.cmdId = cmdType;
    frame->header.seqNum = seqNum;
    frame->header.dataLen = dataLen;
    
    // 拷贝数据
    if (dataLen > 0 && data != NULL)
        memcpy(frame->payload.rawData, data, dataLen);
    
    // 计算校验和（只计算头部）
    frame->header.checksum = calculate_checksum((uint8_t*)&frame->header, 
                                               sizeof(CmdHeader_t) - 1);
    
    return sizeof(CmdHeader_t) + dataLen;
}

/**
  * @brief  解析命令帧
  * @param  buffer 数据地址
  * @param  len 数据长度
  * @param  cmd 命令结构体
  * @return -1 数据长度过于小 | -2 命令帧帧头不对 | -3 命令帧校验和不对 | -4 数据长度不对
  **/
int parse_command_frame(const uint8_t* buffer, uint16_t len, CommandFrame_t* cmd) 
{
    if(len < sizeof(CmdHeader_t)) return -1;
    
    memcpy(cmd, buffer, sizeof(CmdHeader_t));
    
    // 验证起始标志
    if(cmd->header.startMagic != 0xAA) return -2;
    
    // 验证校验和
    uint8_t calc_checksum = calculate_checksum(buffer, sizeof(CmdHeader_t) - 1);
    if(calc_checksum != cmd->header.checksum) return -3;
    
    // 验证数据长度
    if(len < sizeof(CmdHeader_t) + cmd->header.dataLen) return -4;
    
    // 拷贝数据部分
    if(cmd->header.dataLen > 0)
        memcpy(cmd->payload.rawData, buffer + sizeof(CmdHeader_t), cmd->header.dataLen);
    
    return 0;
}

/**
  * @brief  执行命令并放回响应数据
  * @param  buffer 数据地址
  * @param  len 数据长度
  * @param  cmd 命令结构体
  * @return -1 数据长度过于小 | -2 命令帧帧头不对 | -3 命令帧校验和不对 | -4 数据长度不对
  **/
ResponseStatus_t execute_command(const CommandFrame_t* cmd, uint8_t* responseData, uint16_t* respLen) 
{
    ResponseStatus_t status = STATUS_SUCCESS;
    #if !PC_OR_32
    switch(cmd->header.cmdId) 
    {
        case CMD_MOTOR_CONTROL:
            if(cmd->header.dataLen == sizeof(MotorCmd_t))
                ;// (&cmd->payload.motorCmd);
            else 
                status = STATUS_INVALID_PARAM;
            
            break;
            
        case CMD_FIND_OPT_RES:
            if(cmd->header.dataLen == sizeof(OptResData_t)) 
                ; // (&cmd->payload.findOptCmd);
            else
                status = STATUS_INVALID_PARAM;
            
            break;
            
        default:
            status = STATUS_INVALID_CMD;
            break;
    }
    #else 
    switch(cmd->header.cmdId) 
    {     
        case MOTOR_DATA_READ:
            if(cmd->header.dataLen == sizeof(MotorData_t)) 
                ; // task_config_control(&cmd->payload.motorData);
            else 
                status = STATUS_INVALID_PARAM;
            
            break;
            
        case CMD_RESPONSE:
            if(cmd->header.dataLen == sizeof(ResponseData_t))
                ;// system_control(&cmd->payload.response);
            else
                status = STATUS_INVALID_PARAM;
            
            break;
            
        default:
            status = STATUS_INVALID_CMD;
            break;
    }

    #endif
    
    // 构建响应数据（如果有需要返回的数据）
    *respLen = 0;
    // 可以在这里填充response_data
    
    return status;
}
