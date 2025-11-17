#include "power_supply_task.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "queue.h"
#include "command.h"
#include "bsp_power_supply.h"
#include "bsp_power_meter.h"
#include "command_task.h"
#include <string.h>
#include "ff.h"
#include "file_common.h"
#include "main.h"
#include "bsp_led.h"
#include "dm542_task.h"
#include "bsp_dm542.h"
#include "data_sum_task.h"
#include "command_struct.h"
#include "lwip_recv_task.h"

static void power_supply_task(void * param);
static void detect_optimal_voltage(float currentVoltage, float currentPower, int mode, int channel);
static void reset_ps_data(float * bestPowers);

static TaskHandle_t g_PowerSupplyTaskHandle = NULL;
static struct CommandInfo * command;
// static float s_lastPower = -1e30f;
static float s_bestVoltage = 0.0f;
static float s_bestPower = -1e30f;
static float s_bestVoltages[4] = {0.1f, 0.1f, 0.1f, 0.1f};
static float s_bestPowers[4] = {-1e30f, -1e30f, -1e30f, -1e30f};
static float historyVoltages[140] = {0.0f};
static float historyPowers[140] = {0.0f};
static uint16_t psRegAddr[] = {0x01FE, 0x02C6, 0x032A, 0x038E};
static float Power_Supply_Default_Voltage[] = {0.2f, 0.2f, 0.2f, 0.2f};
static uint8_t Seek_Max_Power_Flag = 1;
extern SemaphoreHandle_t dm542_USART3_Mutex;

static FindOptimalCmd_t findOptCmd;

/**
  * @brief  电源任务
  * @param  param 任务参数
  * @return void
  **/
static void power_supply_task(void * param)
{
	float currentVoltage = 0.2f;
	float currentPower = 0.0f;
	FunctionalState enableUsart = ENABLE;
	FRESULT res;
	struct Optimal_v_p_t currentOptimalVP;
	struct CurrentV_P_Ch_t currentVPCh;
	
	/* 将 g_PowerSupplyTaskHandle 插入链表 */
	insert_task_handle(g_PowerSupplyTaskHandle, "power_supply");
	
	/* 设置电源的起始电压 */
	set_voltage_for_power(Power_Supply_Default_Voltage);
	
	while (1)
	{
		switch ((int)command->commandType)
		{
			case demandOne:
//				mutual_printf("ps_demandone\r\n");
//				command->commandType = noDemand;
				if (enableUsart == ENABLE)
				{
					pm_usart_it_config(enableUsart);
					enableUsart = DISABLE;
				}
				
				/* 发送电压 */
				set_power_supply_voltage(PS_SLAVE_ADDR, psRegAddr[(int)(command->psChannel)], currentVoltage);
				vTaskDelay(TIME_OF_FINISHING_SETTING_VOL);
				/* 得到发送的电压对应的功率值 */
				#if !SD_NOTE
				if (!parse_power_from_buf(&currentPower))
				{
					/* 长时间没收到串口数据 */
					command->commandType = demandFault; // 设置命令状态异常
					enableUsart = ENABLE;
					mutual_printf("No serial port data received. Please check the connection between the serial port and the power meter!\r\n");
					
					break;
				}
				#endif
					
				/* 电压、功率值记录 */
				detect_optimal_voltage(currentVoltage, currentPower, SINGLE_CHANNEL_SCANNING, 0);
			
				/* 设置下次的发送电压 */
				currentVoltage += VOL_STEP;
				if (currentVoltage >= MAX_VAL)
				{
					mutual_printf("Best(P,V)=(%.3f, %.2f)\r\n", s_bestPower, s_bestVoltage);
					enableUsart = ENABLE;
					
					#if SD_NOTE
					currentVoltage = 0.f;
					mutual_printf("Start writing!\r\n");
					res = write_arrays_to_CSV("0:test1.csv", historyVoltages, historyPowers);
					if (res == FR_OK)
						mutual_printf("FR_OK");
					else if (res == FR_DISK_ERR)
						mutual_printf("FR_DISK_ERR");
					mutual_printf("error type: %d", (int)res);
					mutual_printf("Successfully written to SD card!\r\n");

					#endif

					command->commandType = noDemand; // 命令完成
				}
				
				vTaskDelay(VOL_SENDING_TIME_INTERVAL);
			
				break;
			case demandTwo:
				if (pdPASS != xQueueReceive(g_findOptCmdQueue, &findOptCmd, 0))
					break;
				
				xSemaphoreTake(dm542_USART3_Mutex, portMAX_DELAY);
				Seek_Max_Power_Flag = 1;
				for (int i = 0; (i < 4) && (Seek_Max_Power_Flag == 1); i ++)
				{
					// while(currentVoltage < MAX_VAL)
					while (currentVoltage < findOptCmd.maxVol)
					{
						/* 发送电压 */
						set_power_supply_voltage(PS_SLAVE_ADDR, PS_REG_ADDR(i), currentVoltage);
						vTaskDelay(TIME_OF_FINISHING_SETTING_VOL);
						currentVPCh.channel = i + 1;
						currentVPCh.currentV = currentVoltage;
						pm_usart_it_config(ENABLE);//设置好电压后开启串口中断接收数据
						if (parse_power_from_buf(&currentPower))
						{
							pm_usart_it_config(DISABLE);//解析完成后关闭串口2
							currentVPCh.currentP = currentPower;
							xQueueSend(g_currentVPChQueue, &currentVPCh, 10);
							if (currentPower > s_bestPowers[i])
							{
								s_bestVoltages[i] = currentVoltage;
								s_bestPowers[i] = currentPower;
							}
//							mutual_printf("channel=%d, P=%.3fW, V=%.2fV, Best(P,V)=(%.3f, %.2f)\r\n",
//													i, currentPower, currentVoltage, s_bestPowers[i], s_bestVoltages[i]);	
						}
						
						/* 未得到发送的电压对应的功率值 */
						else if (!parse_power_from_buf(&currentPower))
						{
							/* 长时间没收到串口数据 */
							command->commandType = demandFault; // 设置命令状态异常
							enableUsart = ENABLE;
							mutual_printf("No serial port data received. Please check the connection between the serial port and the power meter!\r\n");
						}
						/* 设置下次的发送电压 */
						currentVoltage += findOptCmd.volStepLen;
					}
					set_power_supply_voltage(PS_SLAVE_ADDR, PS_REG_ADDR(i), s_bestVoltages[i]);
					vTaskDelay(VOL_SENDING_TIME_INTERVAL);
					
					// currentVoltage = 0.1f;
					currentVoltage = findOptCmd.initialVol;
						
//					/* 电压、功率值记录 */
//					detect_optimal_voltage(currentVoltage, currentPower, MULTI_CHANNELS_SCANNING, i);
				}
				// mutual_printf("V1 = %.2fV, V2 = %.2fV, V3 = %.2fV, V4 = %.2fV, P = %.3fdBm",
				// 			s_bestVoltages[0], s_bestVoltages[1], s_bestVoltages[2], s_bestVoltages[3], s_bestPowers[3]);
				Seek_Max_Power_Flag = 0;//只扫描一次
				LED2_TOGGLE;//扫描完成后绿灯闪烁
				currentOptimalVP.optimalVs[0] = s_bestVoltages[0];
				currentOptimalVP.optimalVs[1] = s_bestVoltages[1];
				currentOptimalVP.optimalVs[2] = s_bestVoltages[2];
				currentOptimalVP.optimalVs[3] = s_bestVoltages[3];
				currentOptimalVP.optimalP = s_bestPowers[3];
				xQueueSend(g_optimalVPDataQueue, &currentOptimalVP, 10);
				command->commandType = noDemand; // 命令完成
				reset_ps_data(s_bestPowers);
				float chanVols[] = {findOptCmd.initialVol, findOptCmd.initialVol, findOptCmd.initialVol, findOptCmd.initialVol};
				set_voltage_for_power(chanVols);
				xSemaphoreGive(dm542_USART3_Mutex);
				vTaskDelay(1000);
				break;
				
			case demandScan:
				
				break;
			default:
//				reset_ps_data();
				pm_usart_it_config(DISABLE);
				mutual_printf("Power supply closed!\r\n");
				vTaskSuspend(NULL); // 完成任务将自身挂起，节约系统资源
				break;
		}
		
		vTaskDelay(5);
	}
}

/**
  * @brief  为电源注册命令
  * @param  MWCommand 系统命令
  * @return void
  **/
void register_command_for_power_supply(struct CommandInfo * MWCommand)
{
	command = MWCommand;
}

/**
  * @brief  寻找最优电压
  * @param  currentVoltage 当前电压
  * @param  currentPower 当前功率值
  * @return void
  **/
static void detect_optimal_voltage(float currentVoltage, float currentPower, int mode, int channel)
{
	if (mode == SINGLE_CHANNEL_SCANNING)
	{
		// historyVoltages
		if (currentPower > s_bestPower)
		{
			s_bestVoltage = currentVoltage;
			s_bestPower = currentPower;
		}
		mutual_printf("P=%.3fW, V=%.2fV, Best(P,V)=(%.3f, %.2f)\r\n",
								currentPower, currentVoltage, s_bestPower, s_bestVoltage);
	}
	else if (mode == MULTI_CHANNELS_SCANNING)
	{
		if (currentPower > s_bestPowers[channel])
		{
			s_bestVoltages[channel] = currentVoltage;
			s_bestPowers[channel] = currentPower;
		}
		mutual_printf("channel=%d, P=%.3fW, V=%.2fV, Best(P,V)=(%.3f, %.2f)\r\n",
								channel, currentPower, currentVoltage, s_bestPower, s_bestVoltage);	
	}
	
	// TODO: 未记录当前电压和当前功率值
}


static void reset_ps_data(float * bestPowers)
{
		for (int i = 0; i < 4; i ++)
				bestPowers[i] = -1e30f;
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
