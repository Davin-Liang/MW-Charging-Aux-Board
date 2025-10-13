#include <stm32f4xx_conf.h>
#include "bsp_power_meter.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "FreeRTOS.h"
#include "bsp_debug_usart.h"
#include "task.h"
#include "bsp_led.h"

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
		uint16_t rxData = USART_ReceiveData(PM_USART);
		
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
						if((g_pmUsartRxSta & 0x3FFF) == 0)
						{
						   if (rxData== '$')  /* �յ�$��ͷ */
                {
                    g_pmUsartRxBuf[0]= rxData;
                    g_pmUsartRxSta = 1;    /* ��ʼ������Ч���� */
                }
                /* �������$��ͷ������rx_staΪ0���ȴ���ȷ��ͷ */ 
						
						}
						else  /* �Ѿ���ʼ�������� */
					{
					  if (rxData == 0x0D)
						{
								g_pmUsartRxSta |= 0x4000;
						} // ����յ� 0x0D
						else
						{
								g_pmUsartRxBuf[g_pmUsartRxSta & 0x3FFF] = rxData;
								g_pmUsartRxSta ++;
								if (g_pmUsartRxSta > (PM_USART_REC_LEN - 1))
										g_pmUsartRxSta = 0; // ���磬��λ
						}
				}
			}
		}	
		LED2_ON;
		
		USART_ClearITPendingBit(PM_USART, USART_IT_RXNE); // �����־λ
	}
}



/**
  * @brief  �ж�һ֡�����Ƿ��Ѿ��������
  * @param  ��
  * @return �������ݼĴ������λֵ
  **/
static int get_receiving_status_of_pm_info(void)
{
	return g_pmUsartRxSta & 0x8000;
}

/**
  * @brief  �����յ������ݽ��д���ֻ��������ֵ��С���ݣ�float�����֣�
  * @param  outPower���������ݲ��������֮��õ��Ĺ���ֵ���
  * @return 1��0�ֱ��ʾ�ɹ��������ݺͽ���ʧ���������
  **/
int parse_power_from_buf(float *outPower)
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
		
//		mutual_printf("Get Data!\r\n");
		
		
		char temp[PM_USART_REC_LEN + 1] = {0};
    uint16_t len = (g_pmUsartRxSta & 0x1FFF);
    if (len == 0 || len >= PM_USART_REC_LEN) 
			return 0;

    memcpy(temp, g_pmUsartRxBuf + 1, len - 1);
    //temp[len] = '\0';

    /* ȥ��ĩβ��CRLF */
    for (int i = (int)len - 1; i >= 0; --i)
    {
        if (temp[i] == '\r' || temp[i] == '\n')
            temp[i] = '\0';
        else 
            break;
    }

    /* ����"dBm"�ַ��� */
    char *dbm_ptr = strstr(temp, "m");//ָ�벻��*ʱ��ʾ���ʸ�ָ��ָ��ĵ�ַ
    if (dbm_ptr == NULL)
    {
        printf("δ�ҵ�dBm\n");  // �������
        return 0;
    }
    
//    printf("�ҵ�dBmλ��: %s\n", dbm_ptr);  // �������
    
    /* ��dBm֮��������ÿ� */
//		memset(&dbm_ptr+1, 'A', (temp+ len - 1 - dbm_ptr) * sizeof(char));
    *dbm_ptr = '\0';
//    printf("�ضϺ�: %s\n", temp);  // �������
    
    /* ���ڴӽضϺ���ַ����в������һ������ */
    char *comma_ptr = strrchr(temp, ',');
    if (comma_ptr == NULL)
    {
        printf("δ�ҵ�����\n");  // �������
        return 0;
    }
    
//    printf("�ҵ�����λ��: %s\n", comma_ptr);  // �������
    
    /* ��ȡ���ź����ֵ���� */
    char *num_start = comma_ptr + 1;
    char *num_end = dbm_ptr;
    
//    printf("num_start: %s\n", num_start);  // �������
//    printf("num_end: %s\n", num_end);  // �������
    
    /* ȥ�����ܵ�ǰ��ո� */
    while (*num_start == ' ') num_start++;
    
//    printf("ȥ���ո��num_start: %s\n", num_start);  // �������
    
    /* �����ֵ�����Ƿ���Ч */
    if (num_start >= num_end)
    {
        printf("����: num_start >= num_end (%p >= %p)\n", num_start, num_end);  // �������
        printf("����ԭ��: ���ź�dBm֮��û�����ݻ�ֻ�пո�\n");
        return 0;
    }
    
    /* ��ȡ��ֵ�ַ��� */
    char num_buf[100] = {0};
    int num_len = num_end - num_start;
    if (num_len <= 0 || num_len >= sizeof(num_buf))
    {
        printf("��ֵ���ȴ���: %d\n", num_len);  // �������
        return 0;
    }
    
    memcpy(num_buf, num_start, num_len);
    num_buf[num_len] = '\0';
    
//    printf("��ȡ����ֵ�ַ���: '%s'\n", num_buf);  // �������
    
    /* ת��Ϊ������ */
    char *endp = NULL;
    float val = strtof(num_buf, &endp);
    
    if (endp == num_buf) 
    {
        printf("ת��ʧ��: '%s'\n", num_buf);  // �������
        return 0;
    }
    
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


