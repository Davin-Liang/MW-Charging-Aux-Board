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
  * @brief  Ϊ��Դ��������
	* @param  MWCommand ϵͳ����
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
	
	/* �� g_PowerSupplyTaskHandle �������� */
	insert_task_handle(g_PowerSupplyTaskHandle, "power_supply");
	
	/* ���õ�Դ����ʼ��ѹ */
	// void set_voltage_for_power(float voltage); //TODO: �� bsp_power.c ��ʵ���������
	// set_voltage_for_power(POWER_SUPPLY_DEFAULT_VOLTAGE); //TODO: ʵ����ú�����ȡ��ע��
	
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
				
				/* ���͵�ѹ */
				// set_voltage_for_power(); //TODO: ʵ����ú�����ȡ��ע��
				vTaskDelay(TIME_OF_FINISHING_SETTING_VOL);
				/* �õ����͵ĵ�ѹ��Ӧ�Ĺ���ֵ */
				if (!parse_power_from_buf(&currentPower))
				{
					/* ��ʱ��û�յ��������� */
					*Command = demandFault; // ��������״̬�쳣
					enableUsart = ENABLE;
					printf("δ�յ��������ݣ����鴮���빦�ʼƵ�����!\r\n");
				}
					
				/* ��ѹ������ֵ��¼ */
				detect_optimal_voltage(currentVoltage, currentPower);
			
				/* �����´εķ��͵�ѹ */
				currentVoltage += VOL_STEP;
				if (currentVoltage >= MAX_VAL)
				{
					printf("Best(P,V)=(%.3f, %.2f)\r\n", s_bestPower, s_bestVoltage);
					enableUsart = ENABLE;
					*Command = noDemand; // �������
				}
			
				break;
			case demandTwo:
				// TODO: ��������ʵ��
				break;
			default:
				currentVoltage = 0.0f;
				currentPower = 0.0f;
				vTaskSuspend(NULL); // �������������𣬽�Լϵͳ��Դ
				break;
		}
		
		vTaskDelay(5);
	}
}

/**
  * @brief  Ѱ�����ŵ�ѹ
	* @param  currentVoltage ��ǰ��ѹ
						currentPower ��ǰ����ֵ
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
	
	// TODO: δ��¼��ǰ��ѹ�͵�ǰ����ֵ
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
