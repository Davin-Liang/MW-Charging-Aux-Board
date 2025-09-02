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

/* command_task任务句柄 */
static TaskHandle_t g_commandTaskHandle = NULL;
/* 项目命令 */
static struct CommandInfo command;
/* 所有任务的链表 */
static struct TaskHandleNode *g_taskNode = NULL;
QueueHandle_t g_commandQueueHandle;

/**
  * @brief  command_task任务主体
  * @param  param 任务参数   
  * @return 无
  **/
static void command_task(void * param)
{	
  /* 注册command，使其可以在command_task.c中使用该command */
  register_command_for_power_supply(&command);
  register_command_for_attenuator(&command);
	insert_task_handle(g_commandTaskHandle, "command");
  g_commandQueueHandle = xQueueCreate(COMMAND_QUEUE_LENGTH, sizeof(struct CommandInfo));

  #ifdef DEBUG

  /* 用假消息测试功能 */
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
	
	return xTaskCreate((TaskFunction_t )command_task,  /* 任务入口函数 */
										(const char*    )"command_task",/* 任务名字 */
										(uint16_t       )size,  /* 任务栈大小 */
										(void*          )NULL,/* 任务入口函数参数 */
										(UBaseType_t    )priority, /* 任务的优先级 */
										(TaskHandle_t*  )&g_commandTaskHandle); /* 任务控制块指针 */ 
}

/**
	* @brief  将TaskHandle插入链表头部
	* @param  handle 要插入的TaskHandle_t变量
	* @return 成功返回1，失败返回0
	**/
int insert_task_handle(TaskHandle_t handle, char * taskName) 
{
    /* 创建新节点 */ 
    struct TaskHandleNode *newNode =  (struct TaskHandleNode*)pvPortMalloc(sizeof(struct TaskHandleNode));
    if (newNode == NULL)
        return 0;  // 内存分配失败
    
    /* 初始化新节点 */ 
    newNode->taskHandle = handle;
    /* 处理字符串复制 */ 
    if (taskName != NULL) 
		{
        /*分配内存（+1 是为了存储字符串结束符 '\0'） */ 
        newNode->taskName = (char*)pvPortMalloc(strlen(taskName) + 1);
        if (newNode->taskName == NULL) 
				{
            vPortFree(newNode);  // 内存分配失败，释放节点
            return 0;
        }
        /* 复制字符串 */ 
        strcpy(newNode->taskName, taskName);
    } else {
        newNode->taskName = NULL;
    }
    newNode->next = NULL;
    
    /* 插入链表（这里实现的是头插法） */ 
    if (g_taskNode == NULL) {
        /* 链表为空时，新节点成为头节点 */ 
        g_taskNode = newNode;
    } else {
        /* 链表非空时，新节点插入到头部 */ 
        newNode->next = g_taskNode;
        g_taskNode = newNode;
    }
    
    return 1;  // 插入成功
}

/**
  * @brief  通过taskName查找链表中的元素
  * @param  targetName 要查找的任务名称
  * @return 找到返回对应节点指针，未找到返回NULL
  **/
struct TaskHandleNode * find_task_node_by_name(const char* targetName) 
{
    /* 空链表或查找目标为空，直接返回NULL */ 
    if (g_taskNode == NULL || targetName == NULL)
        return NULL;
    
    /* 遍历链表查找匹配的节点 */ 
    struct TaskHandleNode * current = g_taskNode;
    while (current != NULL) 
		{
        /* 比较任务名称（注意判断节点的taskName是否为NULL） */ 
        if (current->taskName != NULL && strcmp(current->taskName, targetName) == 0)
            return current;  // 找到匹配节点，返回该节点

        current = current->next;  // 移动到下一个节点
    }
    
    // 遍历完链表仍未找到
    return NULL;
}

