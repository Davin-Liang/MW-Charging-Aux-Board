#ifndef __BSP_CAN_H
#define __BSP_CAN_H

#include "stm32f4xx.h"

void bsp_can_init(void);
uint8_t can_send_motor_cmd(float x, float y);

#endif