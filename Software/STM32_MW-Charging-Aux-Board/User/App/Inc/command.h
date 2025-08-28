#ifndef __COMMAND_H
#define	__COMMAND_H

#include "FreeRTOS.h"
#include "task.h"

enum MWCommandDef{
	demandOne, // ����һ
	demandTwo, // �����
	demandFault,
	noDemand
};

struct TaskHandleNode{
	TaskHandle_t taskHandle;
	char * taskName;
	
	struct TaskHandleNode * next;
};

#endif
