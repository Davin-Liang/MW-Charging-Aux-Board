#ifndef __POWER_METER_TASK_H
#define	__POWER_METER_TASK_H

#include "FreeRTOS.h"
#include "queue.h"

#define PM_QUEUE_LENGTH 10

BaseType_t create_task_for_pm(uint16_t size, UBaseType_t priority);

//extern QueueHandle_t g_pmQueueHandle;

#endif
