#ifndef __POWER_SUPPLY_TASK_H
#define	__POWER_SUPPLY_TASK_H

#include "FreeRTOS.h"
#include "command.h"

#define VOL_STEP 0.1f
#define MAX_VAL 10.f

BaseType_t create_task_for_power_supply(uint16_t size, UBaseType_t priority);
void register_command_for_power_supply(enum MWCommandDef * MWCommand);

#endif
