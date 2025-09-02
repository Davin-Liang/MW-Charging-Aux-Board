#ifndef __ATTENUATOR_TASK_H
#define	__ATTENUATOR_TASK_H

#include "FreeRTOS.h"
#include "command.h"

BaseType_t create_task_for_attenuator(uint16_t size, UBaseType_t priority);
void register_command_for_attenuator(struct CommandInfo * MWCommand);

#endif
