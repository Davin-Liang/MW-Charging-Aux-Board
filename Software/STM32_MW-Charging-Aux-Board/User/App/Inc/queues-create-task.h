#ifndef __QUEUES_CREATE_TASK_H
#define	__QUEUES_CREATE_TASK_H

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

struct SystemQueues_t{
    QueueHandle_t cmdQueue;
    QueueHandle_t commandQueue;
    QueueHandle_t motorDataQueue;
    QueueHandle_t currentVPChQueue;
    QueueHandle_t optimalVPDataQueue;
};

BaseType_t create_task_for_queues_create(uint16_t size, UBaseType_t priority);
struct SystemQueues_t * get_queues(void);

#endif
