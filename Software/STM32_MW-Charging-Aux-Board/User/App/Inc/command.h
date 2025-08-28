#ifndef __COMMAND_H
#define	__COMMAND_H

#include "FreeRTOS.h"
#include "task.h"

enum MWCommandDef{
	demandOne, // 需求一
	demandTwo, // 需求二
	demandFault,
	noDemand
};

struct TaskHandleNode{
	TaskHandle_t taskHandle;
	char * taskName;
	
	struct TaskHandleNode * next;
};

#endif
