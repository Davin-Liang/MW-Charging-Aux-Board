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
  * @brief  ˥��������
  * @param  param �������
  * @return void
  **/
static void attenuator_task(void * param)
{
  /* �� g_PowerSupplyTaskHandle �������� */
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
	
	return xTaskCreate((TaskFunction_t )attenuator_task,  /* ������ں��� */
										(const char*    )"attenuator_task",/* �������� */
										(uint16_t       )size,  /* ����ջ��С */
										(void*          )NULL,/* ������ں������� */
										(UBaseType_t    )priority, /* ��������ȼ� */
										(TaskHandle_t*  )&g_attenuatorTaskHandle); /* ������ƿ�ָ�� */ 
}

/**
  * @brief  Ϊ˥��������ע������
  * @param  MWCommand ϵͳ����
  * @return void
  **/
void register_command_for_attenuator(struct CommandInfo * MWCommand)
{
	command = MWCommand;
}
