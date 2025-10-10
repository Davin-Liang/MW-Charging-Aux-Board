#include "power_supply_task.h"
#include "FreeRTOS.h"
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

static void power_supply_task(void * param);
static void detect_optimal_voltage(float currentVoltage, float currentPower, int mode, int channel);
static void reset_ps_data(void);

static TaskHandle_t g_PowerSupplyTaskHandle = NULL;
static struct CommandInfo * command;
// static float s_lastPower = -1e30f;
static float s_bestVoltage = 0.0f;
static float s_bestPower = -1e30f;
static float s_bestVoltages[4] = {0.0f};
static float s_bestPowers[4] = {0.0f};
static float historyVoltages[140] = {0.0f};
static float historyPowers[140] = {0.0f};
static uint16_t psRegAddr[] = {0x006E, 0x00D2, 0x0136, 0x019A};

/**
  * @brief  ��Դ����
  * @param  param �������
  * @return void
  **/
static void power_supply_task(void * param)
{
	float currentVoltage = 0.0f;
	float currentPower = 0.0f;
	FunctionalState enableUsart = ENABLE;
	FRESULT res;
	
	/* �� g_PowerSupplyTaskHandle �������� */
	insert_task_handle(g_PowerSupplyTaskHandle, "power_supply");
	
	/* ���õ�Դ����ʼ��ѹ */
	// void set_voltage_for_power(float voltage); //TODO: �� bsp_power.c ��ʵ���������
	// set_voltage_for_power(POWER_SUPPLY_DEFAULT_VOLTAGE); //TODO: ʵ����ú�����ȡ��ע��
	
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
				if (enableUsart == ENABLE)
				{
					pm_usart_it_config(enableUsart);
					enableUsart = DISABLE;
				}

				for (int i = 0; i < 4; i ++)
				{
					/* ���͵�ѹ */
					set_power_supply_voltage(PS_SLAVE_ADDR, PS_REG_ADDR(i), currentVoltage);
					vTaskDelay(TIME_OF_FINISHING_SETTING_VOL);
					/* �õ����͵ĵ�ѹ��Ӧ�Ĺ���ֵ */
					if (!parse_power_from_buf(&currentPower))
					{
						/* ��ʱ��û�յ��������� */
						command->commandType = demandFault; // ��������״̬�쳣
						enableUsart = ENABLE;
						mutual_printf("No serial port data received. Please check the connection between the serial port and the power meter!\r\n");
					}
						
					/* ��ѹ������ֵ��¼ */
					detect_optimal_voltage(currentVoltage, currentPower, MULTI_CHANNELS_SCANNING, i);
				}

				/* �����´εķ��͵�ѹ */
				currentVoltage += VOL_STEP;
				if (currentVoltage >= MAX_VAL)
				{
					mutual_printf("Best(P,V)=(%.3f, %.2f)\r\n", s_bestPower, s_bestVoltage);
					enableUsart = ENABLE;
					command->commandType = noDemand; // �������
				}

				break;
			default:
				reset_ps_data();
				pm_usart_it_config(DISABLE);
				mutual_printf("Power supply closed!");
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
