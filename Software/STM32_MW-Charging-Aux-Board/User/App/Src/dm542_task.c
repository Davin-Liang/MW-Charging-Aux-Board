#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "dm542_task.h"
#include "bsp_dm542.h"
#include "command.h"
#include "command_task.h"
#include "data_sum_task.h"
#include "lwip_recv_task.h"
#include "command_struct.h"

static void dm542_Task(void * param); /* Test_Task任务实现 */

static TaskHandle_t g_dm542TaskHandle = NULL;/* LED任务句柄 */
static struct CommandInfo * command;
static MotorCmd_t motorCmd;
static FindOptimalCmd_t g_findOptCmd;

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
  
  while (1) {
    switch ((int)command->commandType) {
        case demandMutiFindOpt:
          if (pdPASS == xQueueReceive(g_findOptCmdQueue, &g_findOptCmd, portMAX_DELAY)) {
            if (g_findOptCmd.whichThaj == SQU_TRAJ) {
              for (float x = (-(g_findOptCmd.squThajLen)/2); x <= (g_findOptCmd.squThajLen)/2; x = x + g_findOptCmd.squThajStepLen) {
                for (float y = (-(g_findOptCmd.squThajLen)/2); y <= (g_findOptCmd.squThajLen)/2; y = y + g_findOptCmd.squThajStepLen) {
                  xSemaphoreTake(dm542_USART3_Mutex, portMAX_DELAY);
                  /* 每次移动位置执行一次寻优 */
                  xQueueSend(g_findOptCmdQueue, &g_findOptCmd, 10);
                  command->commandType = demandTwo;
                  /* 唤醒功率计任务 */
                  vTaskResume(find_task_node_by_name("power_supply")->taskHandle);
                  /* 电机开始移动 */
                  motor_position_ctrl(x, y);
                  /* 上报数据 */
                  currentMotorData.x = x;
                  currentMotorData.y = y;
                  xQueueSend(g_motorDataQueue, &currentMotorData, 10);
                  xSemaphoreGive(dm542_USART3_Mutex);
                  vTaskDelay(5000);
                }
              }
            } else if (g_findOptCmd.whichThaj == CIR_TRAJ) {
              // TODO: 
            }

            /* 完成寻优任务 */
            command->commandType = noDemand;
          }

          break;
        case demandMotorControl:
          if (pdPASS == xQueueReceive(g_motorCmdQueue, &motorCmd, portMAX_DELAY))
          {
            motor_position_ctrl(motorCmd.x, motorCmd.y);
            command->commandType = noDemand;
          }

          break;
        case noDemand:
          vTaskSuspend(NULL);
    }

    vTaskDelay(10);
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

