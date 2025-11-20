#include "only_test_task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "dm542_test.h"
#include "bsp_dm542.h"
#include "data_sum_task.h"

static void only_test_Task(void * param); /* only_test_Task任务实现 */

static TaskHandle_t g_onlyTestTaskHandle = NULL;/* LED任务句柄 */

/**
  * @brief  only_test_Task任务主体
  * @param  parameter 任务参数   
  * @return 无
  **/
static void only_test_Task(void * param)
{	
  vTaskDelay(10000);
  while (1)
  {
//        test1();
//				vTaskDelay(5000);
//       dm542_test2();
//    dm542_test3();
    // dm542_test4();
//    dm542_test5();
    data_sum_test1();
  }
}

BaseType_t create_task_for_only_test(uint16_t size, UBaseType_t priority)
{
	
	return xTaskCreate((TaskFunction_t )only_test_Task,  /* 任务入口函数 */
										(const char*    )"only_test_Task",/* 任务名字 */
										(uint16_t       )size,  /* 任务栈大小 */
										(void*          )NULL,/* 任务入口函数参数 */
										(UBaseType_t    )priority, /* 任务的优先级 */
										(TaskHandle_t*  )&g_onlyTestTaskHandle); /* 任务控制块指针 */ 
}
