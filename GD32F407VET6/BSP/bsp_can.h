#ifndef __BSP_CAN_H
#define __BSP_CAN_H
#include "gd32f4xx.h"
void bsp_can_config(void);
void can_send_position_feedback(float x, float y);
#endif
