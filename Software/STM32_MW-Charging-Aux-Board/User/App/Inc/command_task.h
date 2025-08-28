#ifndef __COMMAND_TASK_H
#define	__COMMAND_TASK_H

#include "FreeRTOS.h"
#include "task.h"

BaseType_t create_task_for_command(uint16_t size, UBaseType_t priority);
int insert_task_handle(TaskHandle_t handle, char * taskName);
struct TaskHandleNode * find_task_node_by_name(const char* targetName);
	
#endif
