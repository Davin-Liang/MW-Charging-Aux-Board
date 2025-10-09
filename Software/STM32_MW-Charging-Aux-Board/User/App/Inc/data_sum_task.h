#ifndef __DATA_SUM_TASK_H
#define	__DATA_SUM_TASK_H

#include "FreeRTOS.h"
#include "queue.h"

BaseType_t create_task_for_data_sum(uint16_t size, UBaseType_t priority);

struct MotorData_t
{
    float x;
    float y;
};

struct Optimal_v_p_t
{
    float optimalV1;
    float optimalV2;
    float optimalV3;
    float optimalV4;

    float optimalP;
};

#define MOTOR_DATA_QUEUE_LEN 10
#define OPTIMAL_V_P_DATA_QUEUE_LEN 10

extern QueueHandle_t g_motorDataQueue;
extern QueueHandle_t g_optimalVPDataQueue;

#endif
