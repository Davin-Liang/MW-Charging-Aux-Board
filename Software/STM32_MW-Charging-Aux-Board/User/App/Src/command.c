// command_handler.c
#include "command_struct.h"
#include "stdio.h"
#include <string.h> 

#define  PC_OR_32 0 // 1 ���� ���Զ� | 0 ����32��

/**
  * @brief  ����У���
  * @param  data ���ݵ�ַ
  * @param  len ���ݳ���
  * @return void
  **/
uint8_t calculate_checksum(const uint8_t* data, uint16_t len) 
{
    uint8_t sum = 0;
    for(uint16_t i = 0; i < len; i++) 
    {
        sum += data[i];
    }
    return ~sum + 1;  // ����
}

/**
  * @brief  ��������֡
  * @param  data ���ݵ�ַ
  * @param  len ���ݳ���
  * @return -1 ���ݳ��ȳ������� | [��Ч���ݳ���]
  **/
int build_command_frame(uint8_t* buffer, CommandType_t cmdType, const void* data, uint16_t dataLen) 
{
    if (dataLen > 128) return -1;
    
    CommandFrame_t * frame = (CommandFrame_t*)buffer;
    
    // �������ͷ
    frame->header.startMagic = 0xAA;
    frame->header.cmdId = cmdType;
    // frame->header.seqNum = seqNum;
    frame->header.dataLen = dataLen;
    
    // ��������
    if (dataLen > 0 && data != NULL)
        memcpy(frame->payload.rawData, data, dataLen);
    
    // ����У��ͣ�ֻ����ͷ����
    frame->header.checksum = calculate_checksum((uint8_t*)&frame->header, 
                                               sizeof(CmdHeader_t) - 1);
    
    return sizeof(CmdHeader_t) + dataLen;
}

/**
  * @brief  ��������֡
  * @param  buffer ���ݵ�ַ
  * @param  len ���ݳ���
  * @param  cmd ����ṹ��
  * @return -1 ���ݳ��ȹ���С | -2 ����֡֡ͷ���� | -3 ����֡У��Ͳ��� | -4 ���ݳ��Ȳ���
  **/
int parse_command_frame(const uint8_t* buffer, uint16_t len, CommandFrame_t* cmd) 
{
    if(len < sizeof(CmdHeader_t)) return -1;
    
    memcpy(cmd, buffer, sizeof(CmdHeader_t));
    
    // ��֤��ʼ��־
    if(cmd->header.startMagic != 0xAA) return -2;
    
    // ��֤У���
    uint8_t calc_checksum = calculate_checksum(buffer, sizeof(CmdHeader_t) - 1);
    if(calc_checksum != cmd->header.checksum) return -3;
    
    // ��֤���ݳ���
    if(len < sizeof(CmdHeader_t) + cmd->header.dataLen) return -4;
    
    // �������ݲ���
    if(cmd->header.dataLen > 0)
        memcpy(cmd->payload.rawData, buffer + sizeof(CmdHeader_t), cmd->header.dataLen);
    
    return 0;
}

/**
  * @brief  ִ������Ż���Ӧ����
  * @param  buffer ���ݵ�ַ
  * @param  len ���ݳ���
  * @param  cmd ����ṹ��
  * @return -1 ���ݳ��ȹ���С | -2 ����֡֡ͷ���� | -3 ����֡У��Ͳ��� | -4 ���ݳ��Ȳ���
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
    
    // ������Ӧ���ݣ��������Ҫ���ص����ݣ�
    *respLen = 0;
    // �������������response_data
    
    return status;
}
