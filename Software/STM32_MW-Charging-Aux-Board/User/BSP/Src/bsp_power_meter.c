#include <stm32f4xx_conf.h>
#include "bsp_power_meter.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "FreeRTOS.h"
#include "bsp_debug_usart.h"
#include "task.h"

static void NVIC_Configuration(void);
static int get_receiving_status_of_pm_info(void);

/** g_pm_usart_rx_sta:
  * @bit15: 	�Ƿ������ɱ�־
  * @bit14: 	�Ƿ��յ� 0x0D
  * @bit13-0: ���յ�����Ч�ֽ���
  **/
uint16_t g_pmUsartRxSta = 0;
uint8_t  g_pmUsartRxBuf[PM_USART_REC_LEN];

/**
  * @brief  ���ʼƴ���(USART2) ��ʼ��
	* @param  void
	* @return void
  **/
void usart_power_meter_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
	
	/* GPIO ʱ��ʹ�� */
  RCC_AHB1PeriphClockCmd(PM_USART_RX_GPIO_CLK | PM_USART_TX_GPIO_CLK, ENABLE);

  /* ����ʱ��ʹ�� */
  RCC_APB1PeriphClockCmd(PM_USART_CLK, ENABLE);
  
  /* GPIO��ʼ�� */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  
  /* ����Tx����Ϊ���ù���  */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = PM_USART_TX_PIN;  
  GPIO_Init(PM_USART_TX_GPIO_PORT, &GPIO_InitStructure);

  /* ����Rx����Ϊ���ù��� */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = PM_USART_RX_PIN;
  GPIO_Init(PM_USART_RX_GPIO_PORT, &GPIO_InitStructure);
  
 /* ���� PXx �� USARTx_Tx*/
  GPIO_PinAFConfig(PM_USART_TX_GPIO_PORT, PM_USART_TX_SOURCE, PM_USART_TX_AF);

  /*  ���� PXx �� USARTx__Rx*/
  GPIO_PinAFConfig(PM_USART_RX_GPIO_PORT, PM_USART_RX_SOURCE, PM_USART_RX_AF);
  
  /* ���ô�DEBUG_USART ģʽ */
  /* ���������ã�DEBUG_USART_BAUDRATE */
  USART_InitStructure.USART_BaudRate = PM_USART_BAUDRATE;
  /* �ֳ�(����λ+У��λ)��8 */
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  /* ֹͣλ��1��ֹͣλ */
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  /* У��λѡ�񣺲�ʹ��У�� */
  USART_InitStructure.USART_Parity = USART_Parity_No;
  /* Ӳ�������ƣ���ʹ��Ӳ���� */
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  /* USARTģʽ���ƣ�ͬʱʹ�ܽ��պͷ��� */
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  /* ���USART��ʼ������ */
  USART_Init(PM_USART, &USART_InitStructure); 
	
  /* Ƕ�������жϿ�����NVIC���� */
	NVIC_Configuration();
  
	/* ��ʧ�ܴ��ڽ����ж� */
	pm_usart_it_config(DISABLE);
	
  /* ʹ�ܴ��� */
  USART_Cmd(PM_USART, ENABLE);
}

/**
  * @brief  ����Ƕ�������жϿ�����NVIC
  * @param  ��
  * @return ��
  **/
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* ����USARTΪ�ж�Դ */
  NVIC_InitStructure.NVIC_IRQChannel = PM_USART_IRQ;
  /* �������ȼ�Ϊ1 */
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  /* �����ȼ�Ϊ1 */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  /* ʹ���ж� */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  /* ��ʼ������NVIC */
  NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  ��д USART2_IRQHandler Rx ������ɻص������ʼư� CRLF ���,�ɹ�����һ�ΰ���
						�ֶ�������һ�����
  * @param  ��
  * @return ��
  **/
void USART2_IRQHandler(void)
{
	if (USART_GetITStatus(PM_USART, USART_IT_RXNE) == SET)
	{
		uint8_t rxData = USART_ReceiveData(PM_USART);
		
		if ((g_pmUsartRxSta & 0x8000) == 0) // ��δ�������
		{
				if (g_pmUsartRxSta & 0x4000) // �Ѿ��յ� 0x0D
				{
						if (rxData != 0x0A) // �ڴ� 0x0A
								g_pmUsartRxSta = 0; // ���󣬸�λ
						else
								g_pmUsartRxSta |= 0x8000; // ���һ֡��... 0D 0A
				}
				else
				{
						if (rxData == 0x0D)
								g_pmUsartRxSta |= 0x4000; // ����յ� 0x0D
						else
						{
								g_pmUsartRxBuf[g_pmUsartRxSta & 0x3FFF] = rxData;
								g_pmUsartRxSta ++;
								if (g_pmUsartRxSta > (PM_USART_REC_LEN - 1))
										g_pmUsartRxSta = 0; // ���磬��λ
						}
				}
		}	

		USART_ClearITPendingBit(PM_USART, USART_IT_RXNE); // �����־λ
	}
}

/**
  * @brief  �����ʼƻ��壨ASCII��תΪ float������ĩβ CRLF
  * @param  ��
  * @return ��
  **/
static int get_receiving_status_of_pm_info(void)
{
	return g_pmUsartRxSta & 0x8000;
}

/**
  * @brief  �����ʼƻ��壨ASCII��תΪ float������ĩβ CRLF
  * @param  ��
  * @return ��
  **/
int parse_power_from_buf(float * outPower)
{
    int timeCount = 0;
	
		/* ��λ��׼����ȡ���´������� */
		g_pmUsartRxSta = 0;
	
		while (!get_receiving_status_of_pm_info() && timeCount != 10)
		{
			vTaskDelay(100);
			timeCount ++;
		}
		
		if (timeCount == 10)
			return 0;
		
		mutual_printf("Get Data!\r\n");
		
		
		char temp[PM_USART_REC_LEN + 1] = {0};
    uint16_t len = (g_pmUsartRxSta & 0x3FFF);
    if (len == 0 || len >= PM_USART_REC_LEN) 
			return 0;

    memcpy(temp, g_pmUsartRxBuf, len);
    temp[len] = '\0';

    /* ȥ��β�����ܵĿհ�/CRLF */
    for (int i = (int)len - 1; i >= 0; --i)
    {
        if (temp[i] == '\r' || temp[i] == '\n' || temp[i] == ' ' || temp[i] == '\t')
            temp[i] = '\0';
        else 
					break;
    }

    char *endp = NULL;
    float val = strtof(temp, &endp);
		
    if (endp == temp) 
			return 0;  /* ����ʧ�� */
		
    *outPower = val;
    return 1;
}

/**
  * @brief  ʹ��/ʧ�ܹ��ʼƴ��ڵ��ж�
  * @param  NewState ENABLE / DISABLE
  * @return ��
  **/
void pm_usart_it_config(FunctionalState NewState)
{
	USART_ITConfig(PM_USART, USART_IT_RXNE, NewState);
	USART_ClearITPendingBit(PM_USART, USART_IT_RXNE);
}
