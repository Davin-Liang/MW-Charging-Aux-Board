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
  * @bit15: 	是否接收完成标志
  * @bit14: 	是否收到 0x0D
  * @bit13-0: 接收到的有效字节数
  **/
uint16_t g_pmUsartRxSta = 0;
uint8_t  g_pmUsartRxBuf[PM_USART_REC_LEN];

/**
  * @brief  功率计串口(USART2) 初始化
	* @param  void
	* @return void
  **/
void usart_power_meter_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
	
	/* GPIO 时钟使能 */
  RCC_AHB1PeriphClockCmd(PM_USART_RX_GPIO_CLK | PM_USART_TX_GPIO_CLK, ENABLE);

  /* 串口时钟使能 */
  RCC_APB1PeriphClockCmd(PM_USART_CLK, ENABLE);
  
  /* GPIO初始化 */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  
  /* 配置Tx引脚为复用功能  */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = PM_USART_TX_PIN;  
  GPIO_Init(PM_USART_TX_GPIO_PORT, &GPIO_InitStructure);

  /* 配置Rx引脚为复用功能 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = PM_USART_RX_PIN;
  GPIO_Init(PM_USART_RX_GPIO_PORT, &GPIO_InitStructure);
  
 /* 连接 PXx 到 USARTx_Tx*/
  GPIO_PinAFConfig(PM_USART_TX_GPIO_PORT, PM_USART_TX_SOURCE, PM_USART_TX_AF);

  /*  连接 PXx 到 USARTx__Rx*/
  GPIO_PinAFConfig(PM_USART_RX_GPIO_PORT, PM_USART_RX_SOURCE, PM_USART_RX_AF);
  
  /* 配置串DEBUG_USART 模式 */
  /* 波特率设置：DEBUG_USART_BAUDRATE */
  USART_InitStructure.USART_BaudRate = PM_USART_BAUDRATE;
  /* 字长(数据位+校验位)：8 */
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  /* 停止位：1个停止位 */
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  /* 校验位选择：不使用校验 */
  USART_InitStructure.USART_Parity = USART_Parity_No;
  /* 硬件流控制：不使用硬件流 */
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  /* USART模式控制：同时使能接收和发送 */
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  /* 完成USART初始化配置 */
  USART_Init(PM_USART, &USART_InitStructure); 
	
  /* 嵌套向量中断控制器NVIC配置 */
	NVIC_Configuration();
  
	/* 先失能串口接收中断 */
	pm_usart_it_config(ENABLE);
	
  /* 使能串口 */
  USART_Cmd(PM_USART, ENABLE);
}

/**
  * @brief  配置嵌套向量中断控制器NVIC
  * @param  无
  * @return 无
  **/
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* 配置USART为中断源 */
  NVIC_InitStructure.NVIC_IRQChannel = PM_USART_IRQ;
  /* 抢断优先级为1 */
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  /* 子优先级为1 */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  /* 使能中断 */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  /* 初始化配置NVIC */
  NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  重写 USART2_IRQHandler Rx 传输完成回调：功率计按 CRLF 组包,成功组完一次包后，
						手动发起下一次组包
  * @param  无
  * @return 无
  **/
void USART2_IRQHandler(void)
{
	if (USART_GetITStatus(PM_USART, USART_IT_RXNE) == SET)
	{
		uint16_t rxData = USART_ReceiveData(PM_USART);
		
		if ((g_pmUsartRxSta & 0x8000) == 0) // 尚未接收完成
		{
				if (g_pmUsartRxSta & 0x4000) // 已经收到 0x0D
				{
						if (rxData != 0x0A) // 期待 0x0A
								g_pmUsartRxSta = 0; // 错误，复位
						else
								g_pmUsartRxSta |= 0x8000; // 完成一帧：... 0D 0A
				}
				else
				{
						if((g_pmUsartRxSta & 0x3FFF) == 0)
						{
						   if (rxData== '$')  /* 收到$开头 */
                {
                    g_pmUsartRxBuf[0]= rxData;
                    g_pmUsartRxSta = 1;    /* 开始接收有效数据 */
                }
                /* 如果不是$开头，保持rx_sta为0，等待正确开头 */ 
						
						}
						else  /* 已经开始接收数据 */
					{
					  if (rxData == 0x0D)
						{
								g_pmUsartRxSta |= 0x4000;
						} // 标记收到 0x0D
						else
						{
								g_pmUsartRxBuf[g_pmUsartRxSta & 0x3FFF] = rxData;
								g_pmUsartRxSta ++;
								if (g_pmUsartRxSta > (PM_USART_REC_LEN - 1))
										g_pmUsartRxSta = 0; // 超界，复位
						}
				}
			}
		}	
		LED2_ON;
		
		USART_ClearITPendingBit(PM_USART, USART_IT_RXNE); // 清除标志位
	}
}



/**
  * @brief  判断一帧数据是否已经接收完成
  * @param  无
  * @return 接收数据寄存器最高位值
  **/
static int get_receiving_status_of_pm_info(void)
{
	return g_pmUsartRxSta & 0x8000;
}

/**
  * @brief  将接收到的数据进行处理，只保留功率值大小数据（float型数字）
  * @param  outPower：接收数据并解析完成之后得到的功率值结果
  * @return 1或0分别表示成功解析数据和解析失败两种情况
  **/
int parse_power_from_buf(float *outPower)
{
    int timeCount = 0;
	
		/* 复位，准备获取最新串口数据 */
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

    /* 去除末尾的CRLF */
    for (int i = (int)len - 1; i >= 0; --i)
    {
        if (temp[i] == '\r' || temp[i] == '\n')
            temp[i] = '\0';
        else 
            break;
    }

    /* 查找"dBm"字符串 */
    char *dbm_ptr = strstr(temp, "m");//指针不带*时表示访问该指针指向的地址
    if (dbm_ptr == NULL)
    {
        printf("未找到dBm\n");  // 调试输出
        return 0;
    }
    
//    printf("找到dBm位置: %s\n", dbm_ptr);  // 调试输出
    
    /* 将dBm之后的内容置空 */
//		memset(&dbm_ptr+1, 'A', (temp+ len - 1 - dbm_ptr) * sizeof(char));
    *dbm_ptr = '\0';
//    printf("截断后: %s\n", temp);  // 调试输出
    
    /* 现在从截断后的字符串中查找最后一个逗号 */
    char *comma_ptr = strrchr(temp, ',');
    if (comma_ptr == NULL)
    {
        printf("未找到逗号\n");  // 调试输出
        return 0;
    }
    
//    printf("找到逗号位置: %s\n", comma_ptr);  // 调试输出
    
    /* 提取逗号后的数值部分 */
    char *num_start = comma_ptr + 1;
    char *num_end = dbm_ptr;
    
//    printf("num_start: %s\n", num_start);  // 调试输出
//    printf("num_end: %s\n", num_end);  // 调试输出
    
    /* 去除可能的前后空格 */
    while (*num_start == ' ') num_start++;
    
//    printf("去除空格后num_start: %s\n", num_start);  // 调试输出
    
    /* 检查数值部分是否有效 */
    if (num_start >= num_end)
    {
        printf("错误: num_start >= num_end (%p >= %p)\n", num_start, num_end);  // 调试输出
        printf("可能原因: 逗号和dBm之间没有内容或只有空格\n");
        return 0;
    }
    
    /* 截取数值字符串 */
    char num_buf[100] = {0};
    int num_len = num_end - num_start;
    if (num_len <= 0 || num_len >= sizeof(num_buf))
    {
        printf("数值长度错误: %d\n", num_len);  // 调试输出
        return 0;
    }
    
    memcpy(num_buf, num_start, num_len);
    num_buf[num_len] = '\0';
    
//    printf("提取的数值字符串: '%s'\n", num_buf);  // 调试输出
    
    /* 转换为浮点数 */
    char *endp = NULL;
    float val = strtof(num_buf, &endp);
    
    if (endp == num_buf) 
    {
        printf("转换失败: '%s'\n", num_buf);  // 调试输出
        return 0;
    }
    
    *outPower = val;
    return 1;
}

/**
  * @brief  使能/失能功率计串口的中断
  * @param  NewState ENABLE / DISABLE
  * @return 无
  **/
void pm_usart_it_config(FunctionalState NewState)
{
	USART_ITConfig(PM_USART, USART_IT_RXNE, NewState);
	USART_ClearITPendingBit(PM_USART, USART_IT_RXNE);
}


