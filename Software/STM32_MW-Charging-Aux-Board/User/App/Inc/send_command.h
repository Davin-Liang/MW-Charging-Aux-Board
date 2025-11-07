#ifndef __SEND_COMMAND_H
#define	__SEND_COMMAND_H

#include "FreeRTOS.h"

int send_motor_data_command(int sock, float x, float y, uint16_t speed);
int send_opt_res_data_command(int sock, float x, float y, float optimalPower, float * optimalVs);
int send_current_vpch_command(int sock, uint8_t channel, float v, float p);

#endif
