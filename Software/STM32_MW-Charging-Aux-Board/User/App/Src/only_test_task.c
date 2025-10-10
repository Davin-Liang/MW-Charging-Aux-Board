#include "only_test_task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "dm542_test.h"
#include "bsp_dm542.h"

static void only_test_Task(void * param); /* only_test_Task����ʵ�� */

static TaskHandle_t g_onlyTestTaskHandle = NULL;/* LED������ */

/**
  * @brief  only_test_Task��������
  * @param  parameter �������   
  * @return ��
  **/
static void only_test_Task(void * param)
{	
  while (1)
  {
//        test1();
//				vTaskDelay(5000);
//       dm542_test2();
//    dm542_test3();
    // dm542_test4();
    dm542_test5();
  }
}

BaseType_t create_task_for_only_test(uint16_t size, UBaseType_t priority)
{
	
	return xTaskCreate((TaskFunction_t )only_test_Task,  /* ������ں��� */
										(const char*    )"only_test_Task",/* �������� */
										(uint16_t       )size,  /* ����ջ��С */
										(void*          )NULL,/* ������ں������� */
										(UBaseType_t    )priority, /* ��������ȼ� */
										(TaskHandle_t*  )&g_onlyTestTaskHandle); /* ������ƿ�ָ�� */ 
}
