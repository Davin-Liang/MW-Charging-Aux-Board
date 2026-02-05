#ifndef __BSP_CAN_H
#define __BSP_CAN_H
#include "gd32f4xx.h"
void bsp_can_config(void);
void can_send_position_feedback(float x, float y);

//专门用于模拟 STM32 发送指令的测试函数
void can_test_send_cmd(float x, float y);

#endif
