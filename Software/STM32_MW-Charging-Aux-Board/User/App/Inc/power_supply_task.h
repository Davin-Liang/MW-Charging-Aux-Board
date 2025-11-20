#ifndef __POWER_SUPPLY_TASK_H
#define	__POWER_SUPPLY_TASK_H

#include "FreeRTOS.h"
#include "command.h"

#define VOL_STEP 0.2f
#define MAX_VAL 14.f
#define SINGLE_CHANNEL_SCANNING 1
#define MULTI_CHANNELS_SCANNING 2

BaseType_t create_task_for_power_supply(uint16_t size, UBaseType_t priority);
void register_command_for_power_supply(struct CommandInfo * MWCommand);

#endif
