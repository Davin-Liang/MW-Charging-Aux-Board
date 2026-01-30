#ifndef __DATA_SUM_TASK_H
#define	__DATA_SUM_TASK_H

#include "FreeRTOS.h"
#include "queue.h"

BaseType_t create_task_for_data_sum(uint16_t size, UBaseType_t priority);
void data_sum_test1(void);

struct MotorData_t
{
    int16_t x;
    int16_t y;
};

struct Optimal_v_p_t
{
    float optimalVs[4];

    float optimalP;
};

struct CurrentV_P_Ch_t
{
    uint8_t channel;
    float currentV;
    float currentP;
};

#define MOTOR_DATA_QUEUE_LEN 10
#define OPTIMAL_V_P_DATA_QUEUE_LEN 10
#define CURRENT_V_P_CH_QUEUE_LEN 10

#endif
