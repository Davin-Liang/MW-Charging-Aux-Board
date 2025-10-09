#include "data_sum_task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

static void data_sum_Task(void * param); /* Test_Task任务实现 */
QueueHandle_t g_motorDataQueue = NULL;
QueueHandle_t g_optimalVPDataQueue = NULL;

static TaskHandle_t g_dataSumTaskHandle = NULL;/* LED任务句柄 */

/**
  * @brief  test_Task任务主体
  * @param  parameter 任务参数   
  * @return 无
  **/
static void data_sum_Task(void * param)
{	
    g_motorDataQueue = xQueueCreate(MOTOR_DATA_QUEUE_LEN, sizeof(struct MotorData_t));
    g_optimalVPDataQueue = xQueueCreate(OPTIMAL_V_P_DATA_QUEUE_LEN, sizeof(struct Optimal_v_p_t));
  
    while (1)
    {

    }
}

BaseType_t create_task_for_data_sum(uint16_t size, UBaseType_t priority)
{
	
	return xTaskCreate((TaskFunction_t )data_sum_Task,  /* 任务入口函数 */
						(const char*    )"data_sum_Task",/* 任务名字 */
						(uint16_t       )size,  /* 任务栈大小 */
                        (void*          )NULL,/* 任务入口函数参数 */
                        (UBaseType_t    )priority, /* 任务的优先级 */
                        (TaskHandle_t*  )&g_dataSumTaskHandle); /* 任务控制块指针 */ 
}


