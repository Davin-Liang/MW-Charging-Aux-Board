#include "command_task.h"
#include "command.h"

#include "FreeRTOS.h"
#include "task.h"
#include "test_task.h"
#include "bsp_led.h"
#include "bsp_power_supply.h"
#include <string.h>
#include <stdlib.h>
#include "power_supply_task.h"
#include "attenuator_task.h"
#include "queue.h"

#define DEBUG 1

static void command_task(void * param);

/* command_task������ */
static TaskHandle_t g_commandTaskHandle = NULL;
/* ��Ŀ���� */
static struct CommandInfo command;
/* ������������� */
static struct TaskHandleNode *g_taskNode = NULL;
QueueHandle_t g_commandQueueHandle;

/**
  * @brief  command_task��������
  * @param  param �������   
  * @return ��
  **/
static void command_task(void * param)
{	
  /* ע��command��ʹ�������command_task.c��ʹ�ø�command */
  register_command_for_power_supply(&command);
  register_command_for_attenuator(&command);
	insert_task_handle(g_commandTaskHandle, "command");
  g_commandQueueHandle = xQueueCreate(COMMAND_QUEUE_LENGTH, sizeof(struct CommandInfo));

  #ifdef DEBUG

  /* �ü���Ϣ���Թ��� */
  struct CommandInfo fuckCommand = {
    .commandType = demandOne,
    .psChannel = 1,
    .attenuatorIndex = attenuator1,
    .attNewState = ENABLE
  };

  xQueueSend(g_commandQueueHandle, &fuckCommand, 10);

  #endif
	
	while (1)
  {
		if (pdPASS == xQueueReceive(g_commandQueueHandle, &command, COMMAND_QUEUE_LEN))
    {
      switch ((int)(command.commandType))
      {
        case demandOne:
        case demandTwo:
          vTaskResume(find_task_node_by_name("power_supply")->taskHandle);
          break;
        case demandThree:
        
          break;
        case demandFault:
          
          break;
        case noDemand:
          
          break;
		  }      
    }
		
		vTaskDelay(10);
  }
}

BaseType_t create_task_for_command(uint16_t size, UBaseType_t priority)
{
	
	return xTaskCreate((TaskFunction_t )command_task,  /* ������ں��� */
										(const char*    )"command_task",/* �������� */
										(uint16_t       )size,  /* ����ջ��С */
										(void*          )NULL,/* ������ں������� */
										(UBaseType_t    )priority, /* ��������ȼ� */
										(TaskHandle_t*  )&g_commandTaskHandle); /* ������ƿ�ָ�� */ 
}

/**
	* @brief  ��TaskHandle��������ͷ��
	* @param  handle Ҫ�����TaskHandle_t����
	* @return �ɹ�����1��ʧ�ܷ���0
	**/
int insert_task_handle(TaskHandle_t handle, char * taskName) 
{
    /* �����½ڵ� */ 
    struct TaskHandleNode *newNode =  (struct TaskHandleNode*)pvPortMalloc(sizeof(struct TaskHandleNode));
    if (newNode == NULL)
        return 0;  // �ڴ����ʧ��
    
    /* ��ʼ���½ڵ� */ 
    newNode->taskHandle = handle;
    /* �����ַ������� */ 
    if (taskName != NULL) 
		{
        /*�����ڴ棨+1 ��Ϊ�˴洢�ַ��������� '\0'�� */ 
        newNode->taskName = (char*)pvPortMalloc(strlen(taskName) + 1);
        if (newNode->taskName == NULL) 
				{
            vPortFree(newNode);  // �ڴ����ʧ�ܣ��ͷŽڵ�
            return 0;
        }
        /* �����ַ��� */ 
        strcpy(newNode->taskName, taskName);
    } else {
        newNode->taskName = NULL;
    }
    newNode->next = NULL;
    
    /* ������������ʵ�ֵ���ͷ�巨�� */ 
    if (g_taskNode == NULL) {
        /* ����Ϊ��ʱ���½ڵ��Ϊͷ�ڵ� */ 
        g_taskNode = newNode;
    } else {
        /* ����ǿ�ʱ���½ڵ���뵽ͷ�� */ 
        newNode->next = g_taskNode;
        g_taskNode = newNode;
    }
    
    return 1;  // ����ɹ�
}

/**
  * @brief  ͨ��taskName���������е�Ԫ��
  * @param  targetName Ҫ���ҵ���������
  * @return �ҵ����ض�Ӧ�ڵ�ָ�룬δ�ҵ�����NULL
  **/
struct TaskHandleNode * find_task_node_by_name(const char* targetName) 
{
    /* ����������Ŀ��Ϊ�գ�ֱ�ӷ���NULL */ 
    if (g_taskNode == NULL || targetName == NULL)
        return NULL;
    
    /* �����������ƥ��Ľڵ� */ 
    struct TaskHandleNode * current = g_taskNode;
    while (current != NULL) 
		{
        /* �Ƚ��������ƣ�ע���жϽڵ��taskName�Ƿ�ΪNULL�� */ 
        if (current->taskName != NULL && strcmp(current->taskName, targetName) == 0)
            return current;  // �ҵ�ƥ��ڵ㣬���ظýڵ�

        current = current->next;  // �ƶ�����һ���ڵ�
    }
    
    // ������������δ�ҵ�
    return NULL;
}

