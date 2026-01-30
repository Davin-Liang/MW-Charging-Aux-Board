#include "power_meter_task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

static void power_meter_task(void * param);

static TaskHandle_t g_PmTaskHandle = NULL;

//QueueHandle_t g_pmQueueHandle;

static void power_meter_task(void * param)
{
//	g_pmQueueHandle = xQueueCreate(PM_QUEUE_LENGTH, 10);
	
	while (1)
	{

	}
}

BaseType_t create_task_for_pm(uint16_t size, UBaseType_t priority)
{
	
	return xTaskCreate((TaskFunction_t )power_meter_task,  /* 任务入口函数 */
										(const char*    )"power_meter_task",/* 任务名字 */
										(uint16_t       )size,  /* 任务栈大小 */
										(void*          )NULL,/* 任务入口函数参数 */
										(UBaseType_t    )priority, /* 任务的优先级 */
										(TaskHandle_t*  )&g_PmTaskHandle); /* 任务控制块指针 */ 
}
