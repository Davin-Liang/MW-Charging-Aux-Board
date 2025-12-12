#include "queues-create-task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "command.h"
#include "command_struct.h"
#include "data_sum_task.h"
#include "main.h"
#include "event_groups.h"

static TaskHandle_t g_queuesCreateTaskHandle = NULL;/* LED任务句柄 */

static struct SystemQueues_t * queues;

/**
  * @brief  test_Task任务主体
  * @param  parameter 任务参数   
  * @return 无
  **/
static void queues_create_task(void * param)
{	
    queues = pvPortMalloc(sizeof(struct SystemQueues_t));
	
		queues->cmdQueue = xQueueCreate(10, sizeof(uint8_t));
    queues->commandQueue = xQueueCreate(10, sizeof(struct CommandInfo));
    queues->motorDataQueue = xQueueCreate(10, sizeof(struct MotorData_t));
    queues->currentVPChQueue = xQueueCreate(10, sizeof(struct CurrentV_P_Ch_t));
    queues->optimalVPDataQueue = xQueueCreate(10, sizeof(struct Optimal_v_p_t));
    xEventGroupSetBits(xSystemEventGroup, BIT_WAKE_ALL);

    vTaskDelete(NULL);
}

BaseType_t create_task_for_queues_create(uint16_t size, UBaseType_t priority)
{
	
	return xTaskCreate((TaskFunction_t )queues_create_task,  /* 任务入口函数 */
						          (const char*    )"queues-create-task",/* 任务名字 */
                      (uint16_t       )size,  /* 任务栈大小 */
                      (void*          )NULL,/* 任务入口函数参数 */
                      (UBaseType_t    )priority, /* 任务的优先级 */
                      (TaskHandle_t*  )&g_queuesCreateTaskHandle); /* 任务控制块指针 */ 
}

struct SystemQueues_t * get_queues(void)
{
  return queues;
}

