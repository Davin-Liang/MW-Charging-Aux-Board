#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "dm542_task.h"
#include "bsp_dm542.h"
#include "command.h"
#include "command_task.h"
#include "data_sum_task.h"

static void dm542_Task(void * param); /* Test_Task任务实现 */

static TaskHandle_t g_dm542TaskHandle = NULL;/* LED任务句柄 */
static struct CommandInfo * command;

extern SemaphoreHandle_t dm542_USART3_Mutex;

/**
  * @brief  test_Task任务主体
  * @param  parameter 任务参数   
  * @return 无
  **/
static void dm542_Task(void * param)
{	
	/* 将 g_PowerSupplyTaskHandle 插入链表 */
	insert_task_handle(g_dm542TaskHandle, "dm542");
  struct MotorData_t currentMotorData;
  
    while (1)
  {
    // vTaskDelay(5000);
    switch ((int)command->commandType)
    {
        case demandTwo:
            for (float x = -30.0f; x < 30.0f; x = x + 10)
            {
                for (float y = -30.0f; y < 30.0f; y = y + 10)
                {
                    xSemaphoreTake(dm542_USART3_Mutex, portMAX_DELAY);
                    motor_position_ctrl(x, y);
                    currentMotorData.x = x;
                    currentMotorData.y = y;
                    xQueueSend(g_motorDataQueue, &currentMotorData, 10);
                    //mutual_printf("Position:(%.2f,%.2f)\r\n", x, y);
                    xSemaphoreGive(dm542_USART3_Mutex);
                    vTaskDelay(5000);
                }
            }
//						command->commandType = noDemand; // 命令完成

            break;
    }


  }
}

void register_command_for_dm542(struct CommandInfo * MWCommand)
{
	command = MWCommand;
}

BaseType_t create_task_for_dm542(uint16_t size, UBaseType_t priority)
{
	
	return xTaskCreate((TaskFunction_t )dm542_Task,  /* 任务入口函数 */
										(const char*    )"dm542_Task",/* 任务名字 */
										(uint16_t       )size,  /* 任务栈大小 */
										(void*          )NULL,/* 任务入口函数参数 */
										(UBaseType_t    )priority, /* 任务的优先级 */
										(TaskHandle_t*  )&g_dm542TaskHandle); /* 任务控制块指针 */ 
}

