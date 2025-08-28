#include "FreeRTOS.h"
#include "task.h"
#include "test_task.h"
#include "bsp_led.h"

static void test_Task(void * param); /* Test_Task任务实现 */

static TaskHandle_t g_testTaskHandle = NULL;/* LED任务句柄 */

/**
  * @brief  test_Task任务主体
  * @param  parameter 任务参数   
  * @return 无
  **/
static void test_Task(void * param)
{	
  while (1)
  {
    LED1_ON;
    printf("Test_Task Running,LED1_ON\r\n");
    vTaskDelay(500);   /* 延时500个tick */
    
    LED1_OFF;     
    printf("Test_Task Running,LED1_OFF\r\n");
    vTaskDelay(500);   /* 延时500个tick */
  }
}

BaseType_t create_task_for_test(uint16_t size, UBaseType_t priority)
{
	
	return xTaskCreate((TaskFunction_t )test_Task,  /* 任务入口函数 */
										(const char*    )"test_Task",/* 任务名字 */
										(uint16_t       )size,  /* 任务栈大小 */
										(void*          )NULL,/* 任务入口函数参数 */
										(UBaseType_t    )priority, /* 任务的优先级 */
										(TaskHandle_t*  )&g_testTaskHandle); /* 任务控制块指针 */ 
}

