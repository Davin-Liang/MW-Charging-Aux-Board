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

static void power_supply_task(void * param);
static void detect_optimal_voltage(float currentVoltage, float currentPower, int mode, int channel);
static void reset_ps_data(void);

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
static float Power_Supply_Default_Voltage[] = {0.1f, 0.1f, 0.1f, 0.1f};
static uint8_t Seek_Max_Power_Flag = 1;
extern SemaphoreHandle_t dm542_USART3_Mutex;

/**
  * @brief  ��Դ����
  * @param  param �������
  * @return void
  **/
static void power_supply_task(void * param)
{
	float currentVoltage = 0.1f;
	float currentPower = 0.0f;
	FunctionalState enableUsart = ENABLE;
	FRESULT res;
	struct Optimal_v_p_t currentOptimalVP;
	struct CurrentV_P_Ch_t currentVPCh;
	
	/* �� g_PowerSupplyTaskHandle �������� */
	insert_task_handle(g_PowerSupplyTaskHandle, "power_supply");
	
	/* ���õ�Դ����ʼ��ѹ */
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
				
				/* ���͵�ѹ */
				set_power_supply_voltage(PS_SLAVE_ADDR, psRegAddr[(int)(command->psChannel)], currentVoltage);
				vTaskDelay(TIME_OF_FINISHING_SETTING_VOL);
				/* �õ����͵ĵ�ѹ��Ӧ�Ĺ���ֵ */
				#if !SD_NOTE
				if (!parse_power_from_buf(&currentPower))
				{
					/* ��ʱ��û�յ��������� */
					command->commandType = demandFault; // ��������״̬�쳣
					enableUsart = ENABLE;
					mutual_printf("No serial port data received. Please check the connection between the serial port and the power meter!\r\n");
					
					break;
				}
				#endif
					
				/* ��ѹ������ֵ��¼ */
				detect_optimal_voltage(currentVoltage, currentPower, SINGLE_CHANNEL_SCANNING, 0);
			
				/* �����´εķ��͵�ѹ */
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

					command->commandType = noDemand; // �������
				}
				
				vTaskDelay(VOL_SENDING_TIME_INTERVAL);
			
				break;
			case demandTwo:
				xSemaphoreTake(dm542_USART3_Mutex, portMAX_DELAY);
				Seek_Max_Power_Flag = 1;
				for (int i = 0; (i < 4) && (Seek_Max_Power_Flag == 1); i ++)
				{
					while(currentVoltage < MAX_VAL)
					{
						/* ���͵�ѹ */
						set_power_supply_voltage(PS_SLAVE_ADDR, PS_REG_ADDR(i), currentVoltage);
						vTaskDelay(TIME_OF_FINISHING_SETTING_VOL);
						currentVPCh.channel = i + 1;
						currentVPCh.currentV = currentVoltage;
						pm_usart_it_config(ENABLE);//���úõ�ѹ���������жϽ�������
						if (parse_power_from_buf(&currentPower))
						{
							pm_usart_it_config(DISABLE);//������ɺ�رմ���2
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
						
						/* δ�õ����͵ĵ�ѹ��Ӧ�Ĺ���ֵ */
						else if (!parse_power_from_buf(&currentPower))
						{
							/* ��ʱ��û�յ��������� */
							command->commandType = demandFault; // ��������״̬�쳣
							enableUsart = ENABLE;
							mutual_printf("No serial port data received. Please check the connection between the serial port and the power meter!\r\n");
						}
						/* �����´εķ��͵�ѹ */
						currentVoltage += VOL_STEP;
					}
					set_power_supply_voltage(PS_SLAVE_ADDR, PS_REG_ADDR(i), s_bestVoltages[i]);
					vTaskDelay(VOL_SENDING_TIME_INTERVAL);
					currentVoltage = 0.1f;
						
//					/* ��ѹ������ֵ��¼ */
//					detect_optimal_voltage(currentVoltage, currentPower, MULTI_CHANNELS_SCANNING, i);
				}
				// mutual_printf("V1 = %.2fV, V2 = %.2fV, V3 = %.2fV, V4 = %.2fV, P = %.3fdBm",
				// 			s_bestVoltages[0], s_bestVoltages[1], s_bestVoltages[2], s_bestVoltages[3], s_bestPowers[3]);
				Seek_Max_Power_Flag = 0;//ֻɨ��һ��
				LED2_TOGGLE;//ɨ����ɺ��̵���˸
				currentOptimalVP.optimalVs[0] = s_bestVoltages[0];
				currentOptimalVP.optimalVs[1] = s_bestVoltages[1];
				currentOptimalVP.optimalVs[2] = s_bestVoltages[2];
				currentOptimalVP.optimalVs[3] = s_bestVoltages[3];
				currentOptimalVP.optimalP = s_bestPowers[3];
				xQueueSend(g_optimalVPDataQueue, &currentOptimalVP, 10);
				command->commandType = noDemand; // �������
				xSemaphoreGive(dm542_USART3_Mutex);
				vTaskDelay(1000);
				break;
			default:
				reset_ps_data();
				pm_usart_it_config(DISABLE);
				mutual_printf("Power supply closed!\r\n");
				vTaskSuspend(NULL); // �������������𣬽�Լϵͳ��Դ
				break;
		}
		
		vTaskDelay(5);
	}
}

/**
  * @brief  Ϊ��Դע������
  * @param  MWCommand ϵͳ����
  * @return void
  **/
void register_command_for_power_supply(struct CommandInfo * MWCommand)
{
	command = MWCommand;
}

/**
  * @brief  Ѱ�����ŵ�ѹ
  * @param  currentVoltage ��ǰ��ѹ
  * @param  currentPower ��ǰ����ֵ
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
	
	// TODO: δ��¼��ǰ��ѹ�͵�ǰ����ֵ
}


static void reset_ps_data(void)
{
	s_bestVoltage = 0.0f;
	s_bestPower = 0.0f;
	memset(s_bestVoltages, 0, sizeof(s_bestVoltages));
	memset(s_bestPowers, 0, sizeof(s_bestPowers));
}

/**
  * @brief  Ϊ��Դ��������
  * @param  void
  * @return void
  **/
BaseType_t create_task_for_power_supply(uint16_t size, UBaseType_t priority)
{
	
	return xTaskCreate((TaskFunction_t )power_supply_task,  /* ������ں��� */
					   (const char*    )"power_supply_task",/* �������� */
					   (uint16_t       )size,  /* ����ջ��С */
					   (void*          )NULL,/* ������ں������� */
					   (UBaseType_t    )priority, /* ��������ȼ� */
					   (TaskHandle_t*  )&g_PowerSupplyTaskHandle); /* ������ƿ�ָ�� */ 
}
