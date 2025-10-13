#ifndef __DM542_TASK_H
#define	__DM542_TASK_H

#include "FreeRTOS.h"
#include "command.h"

BaseType_t create_task_for_dm542(uint16_t size, UBaseType_t priority);
void register_command_for_dm542(struct CommandInfo * MWCommand);

#endif
