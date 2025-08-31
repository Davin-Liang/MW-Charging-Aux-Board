#include "FreeRTOS.h"
#include "task.h"
#include "test_task.h"
#include "bsp_led.h"

static void test_Task(void * param); /* Test_Task����ʵ�� */

static TaskHandle_t g_testTaskHandle = NULL;/* LED������ */

/**
  * @brief  test_Task��������
  * @param  parameter �������   
  * @return ��
  **/
static void test_Task(void * param)
{	
  while (1)
  {
    LED1_ON;
    printf("Test_Task Running,LED1_ON\r\n");
    vTaskDelay(500);   /* ��ʱ500��tick */
    
    LED1_OFF;     
    printf("Test_Task Running,LED1_OFF\r\n");
    vTaskDelay(500);   /* ��ʱ500��tick */
  }
}

BaseType_t create_task_for_test(uint16_t size, UBaseType_t priority)
{
	
	return xTaskCreate((TaskFunction_t )test_Task,  /* ������ں��� */
										(const char*    )"test_Task",/* �������� */
										(uint16_t       )size,  /* ����ջ��С */
										(void*          )NULL,/* ������ں������� */
										(UBaseType_t    )priority, /* ��������ȼ� */
										(TaskHandle_t*  )&g_testTaskHandle); /* ������ƿ�ָ�� */ 
}

