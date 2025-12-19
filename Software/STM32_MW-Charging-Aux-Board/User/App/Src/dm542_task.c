#include "main.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "event_groups.h"

#include "dm542_task.h"
#include "command_task.h"
#include "data_sum_task.h"
#include "lwip_recv_task.h"
#include "queues-create-task.h"
#include "modbus-tcp-server-task.h"

#include "bsp_dm542.h"
#include "command.h"
#include "command_struct.h"

static void dm542_task(void * param); /* Test_Task任务实现 */

static TaskHandle_t g_dm542TaskHandle = NULL;/* LED任务句柄 */
static struct CommandInfo * command;
static struct SystemQueues_t * queues;
extern SemaphoreHandle_t dm542_USART3_Mutex;

/**
  * @brief  test_Task任务主体
  * @param  parameter 任务参数   
  * @return 无
  **/
static void dm542_task(void * param)
{	
	xEventGroupWaitBits(xSystemEventGroup, BIT_WAKE_DM542_TASK, pdTRUE, pdTRUE, portMAX_DELAY);
  
  struct MotorData_t currentMotorData;
  
  queues = get_queues();
  
  /* 将 g_PowerSupplyTaskHandle 插入链表 */
	insert_task_handle(g_dm542TaskHandle, "dm542");
  
  while (1) {
    uint8_t cmd;
    switch ((int)command->commandType) {
        case demandMutiFindOpt: 
        {
          FindOptimalCmd_t * findOptCmd = get_find_optimal_cmd(get_data_center());
          if (pdPASS == xQueueReceive(queues->cmdQueue, &cmd, portMAX_DELAY)) {
            if (findOptCmd->whichThaj == SQU_TRAJ) {
              for (float x = (-(findOptCmd->squThajLen)/2); x <= (findOptCmd->squThajLen)/2; x = x + findOptCmd->squThajStepLen) {
                for (float y = (-(findOptCmd->squThajLen)/2); y <= (findOptCmd->squThajLen)/2; y = y + findOptCmd->squThajStepLen) {
                  xSemaphoreTake(dm542_USART3_Mutex, portMAX_DELAY);
                  /* 每次移动位置执行一次寻优 */
                  // xQueueSend(g_findOptCmdQueue, &g_findOptCmd, 10);
                  command->commandType = demandTwo;
                  /* 唤醒功率计任务 */
                  vTaskResume(find_task_node_by_name("power_supply")->taskHandle);
                  /* 电机开始移动 */
                  motor_position_ctrl(x, y);
                  /* 上报数据 */
                  currentMotorData.x = x;
                  currentMotorData.y = y;
                  xQueueSend(queues->motorDataQueue, &currentMotorData, 10);
                  xSemaphoreGive(dm542_USART3_Mutex);
                  vTaskDelay(5000);
                }
              }
            } else if (findOptCmd->whichThaj == CIR_TRAJ) {
              // TODO: 
            }

            /* 完成寻优任务 */
            command->commandType = noDemand;
          }

          break;
        }
        case demandMotorControl:
        {
					MotorCmd_t * motorCmd = get_motor_cmd(get_data_center());
          if (pdPASS == xQueueReceive(queues->cmdQueue, &cmd, portMAX_DELAY)) {
            if (cmd == CMD_MOTOR_CONTROL) {
              motor_position_ctrl(motorCmd->x, motorCmd->y);
              /* 完成电机任务 */
              command->commandType = noDemand;
            }
          }

          break;
        }
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
	
	return xTaskCreate((TaskFunction_t )dm542_task,  /* 任务入口函数 */
										(const char*    )"dm542_task",/* 任务名字 */
										(uint16_t       )size,  /* 任务栈大小 */
										(void*          )NULL,/* 任务入口函数参数 */
										(UBaseType_t    )priority, /* 任务的优先级 */
										(TaskHandle_t*  )&g_dm542TaskHandle); /* 任务控制块指针 */ 
}

