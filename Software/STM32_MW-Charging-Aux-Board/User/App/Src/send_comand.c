#include "command_struct.h"
#include <lwip/sockets.h>
#include "send_command.h"

/**
  * @brief  发送电机数据命令帧
  * @param  sock socket 编号
  * @param  x 当前电机位置 X 坐标
  * @param  y 当前电机位置 Y 坐标
  * @param  speed 当前电机速度
  * @return 发送的字节数
  **/
int send_motor_data_command(int sock, float x, float y, uint16_t speed) 
{
    uint8_t buffer[128];

    MotorData_t currentMotorData;
    currentMotorData.motorX = x;
    currentMotorData.motorY = y;
    currentMotorData.motorSpeed = speed;

    
    int frameLen = build_command_frame(buffer, MOTOR_DATA_READ, &currentMotorData, sizeof(MotorData_t));
    
    if(frameLen > 0) 
        return send(sock, buffer, frameLen, 0);

    return -1;
}

/**
  * @brief  发送寻优结果命令帧
  * @param  sock socket 编号
  * @param  x 当前电机位置 X 坐标
  * @param  y 当前电机位置 Y 坐标
  * @param  optimalPower 最优功率
  * @param  optimalVs 最优电压
  * @return 发送的字节数
  **/
int send_opt_res_data_command(int sock, float x, float y, float optimalPower, float * optimalVs) 
{
    uint8_t buffer[128];

    OptResData_t OptResData;
    OptResData.motorData.motorX = x;
    OptResData.motorData.motorY = y;
    OptResData.optimalPower = optimalPower;
    for (int i = 0; i < 4; i ++)
        OptResData.optimalVs[i] = optimalVs[i];
    
    int frameLen = build_command_frame(buffer, CMD_OPT_RES_READ, &OptResData, sizeof(OptResData_t));
    
    if(frameLen > 0) 
        return send(sock, buffer, frameLen, 0);

    return -1;
}

/**
  * @brief  发送寻优结果命令帧
  * @param  sock socket 编号
  * @param  channel 当前测试通道
  * @param  v 当前发送电压
  * @param  p 当前功率
  * @return 发送的字节数
  **/
int send_current_vpch_command(int sock, uint8_t channel, float v, float p) 
{
    uint8_t buffer[128];

    CurrentVPCh_t currentVPCh;
    currentVPCh.currentChannel = channel;
    currentVPCh.currentV = v;
    currentVPCh.currentP = p;
    
    int frameLen = build_command_frame(buffer, CURRENT_VPCH_READ, &currentVPCh, sizeof(CurrentVPCh_t));
    
    if(frameLen > 0) 
        return send(sock, buffer, frameLen, 0);

    return -1;
}