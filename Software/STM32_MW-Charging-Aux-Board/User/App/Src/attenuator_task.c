#include "attenuator_task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "command.h"
#include "command_task.h"

static void attenuator_task(void * param);

static TaskHandle_t g_attenuatorTaskHandle = NULL;
static struct CommandInfo * command;

/**
  * @brief  衰减器任务
  * @param  param 任务参数
  * @return void
  **/
static void attenuator_task(void * param)
{
  /* 将 g_PowerSupplyTaskHandle 插入链表 */
	insert_task_handle(g_attenuatorTaskHandle, "attenuator");
  
  while (1)
  {
    switch ((int)command->commandType)
    {
      case demandThree:
        attenuator_switch_config(command->attenuatorIndex, command->attNewState);
        vTaskSuspend(NULL);
        break;
    }
  }
}

BaseType_t create_task_for_attenuator(uint16_t size, UBaseType_t priority)
{
	
	return xTaskCreate((TaskFunction_t )attenuator_task,  /* 任务入口函数 */
										(const char*    )"attenuator_task",/* 任务名字 */
										(uint16_t       )size,  /* 任务栈大小 */
										(void*          )NULL,/* 任务入口函数参数 */
										(UBaseType_t    )priority, /* 任务的优先级 */
										(TaskHandle_t*  )&g_attenuatorTaskHandle); /* 任务控制块指针 */ 
}

/**
  * @brief  为衰减器任务注册命令
  * @param  MWCommand 系统命令
  * @return void
  **/
void register_command_for_attenuator(struct CommandInfo * MWCommand)
{
	command = MWCommand;
}
