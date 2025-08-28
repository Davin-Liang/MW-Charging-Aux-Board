#include "power_supply_task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "command.h"
#include "bsp_power_supply.h"
#include "bsp_power_meter.h"
#include "command_task.h"

static void power_supply_task(void * param);
static void detect_optimal_voltage(float currentVoltage, float currentPower);

static TaskHandle_t g_PowerSupplyTaskHandle = NULL;
static enum MWCommandDef * Command;
static float s_lastPower = -1e30f;
static float s_bestVoltage = 0.0f;
static float s_bestPower = 0.0f;

/**
  * @brief  为电源创建任务
	* @param  MWCommand 系统命令
	* @return void
  **/
void register_command_for_power_supply(enum MWCommandDef * MWCommand)
{
	*Command = *MWCommand;
}

static void power_supply_task(void * param)
{
	float currentVoltage = 0.0f;
	float currentPower = 0.0f;
	FunctionalState enableUsart = ENABLE;
	
	/* 将 g_PowerSupplyTaskHandle 插入链表 */
	insert_task_handle(g_PowerSupplyTaskHandle, "power_supply");
	
	/* 设置电源的起始电压 */
	// void set_voltage_for_power(float voltage); //TODO: 在 bsp_power.c 中实现这个函数
	// set_voltage_for_power(POWER_SUPPLY_DEFAULT_VOLTAGE); //TODO: 实现完该函数再取消注释
	
	while (1)
	{
		switch ((int)Command)
		{
			case demandOne:

				if (enableUsart == ENABLE)
				{
					pm_usart_it_config(ENABLE);
					enableUsart = DISABLE;
				}
				
				/* 发送电压 */
				// set_voltage_for_power(); //TODO: 实现完该函数再取消注释
				vTaskDelay(TIME_OF_FINISHING_SETTING_VOL);
				/* 得到发送的电压对应的功率值 */
				if (!parse_power_from_buf(&currentPower))
				{
					/* 长时间没收到串口数据 */
					*Command = demandFault; // 设置命令状态异常
					enableUsart = ENABLE;
					printf("未收到串口数据，请检查串口与功率计的连接!\r\n");
				}
					
				/* 电压、功率值记录 */
				detect_optimal_voltage(currentVoltage, currentPower);
			
				/* 设置下次的发送电压 */
				currentVoltage += VOL_STEP;
				if (currentVoltage >= MAX_VAL)
				{
					printf("Best(P,V)=(%.3f, %.2f)\r\n", s_bestPower, s_bestVoltage);
					enableUsart = ENABLE;
					*Command = noDemand; // 命令完成
				}
			
				break;
			case demandTwo:
				// TODO: 完善需求实现
				break;
			default:
				currentVoltage = 0.0f;
				currentPower = 0.0f;
				vTaskSuspend(NULL); // 完成任务将自身挂起，节约系统资源
				break;
		}
		
		vTaskDelay(5);
	}
}

/**
  * @brief  寻找最优电压
	* @param  currentVoltage 当前电压
						currentPower 当前功率值
	* @return void
  **/
static void detect_optimal_voltage(float currentVoltage, float currentPower)
{
	if (currentPower > s_lastPower)
	{
		s_bestVoltage = currentVoltage;
		s_bestPower = currentPower;
	}
	s_lastPower = currentPower;
	
	printf("P=%.3fW, V=%.2fV, Best(P,V)=(%.3f, %.2f)\r\n",
                          currentPower, currentVoltage, s_bestPower, s_bestVoltage);
	
	// TODO: 未记录当前电压和当前功率值
}

/**
  * @brief  为电源创建任务
	* @param  void
	* @return void
  **/
BaseType_t create_task_for_power_supply(uint16_t size, UBaseType_t priority)
{
	
	return xTaskCreate((TaskFunction_t )power_supply_task,  /* 任务入口函数 */
										(const char*    )"power_supply_task",/* 任务名字 */
										(uint16_t       )size,  /* 任务栈大小 */
										(void*          )NULL,/* 任务入口函数参数 */
										(UBaseType_t    )priority, /* 任务的优先级 */
										(TaskHandle_t*  )&g_PowerSupplyTaskHandle); /* 任务控制块指针 */ 
}
