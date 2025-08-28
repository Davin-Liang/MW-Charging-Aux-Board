#ifndef __BSP_POWER_METER_H
#define	__BSP_POWER_METER_H

#define PM_USART              USART2
#define PM_USART_RX_GPIO_CLK  RCC_AHB1Periph_GPIOA
#define PM_USART_TX_GPIO_CLK  RCC_AHB1Periph_GPIOA
#define PM_USART_CLK 		      RCC_APB1Periph_USART2
#define PM_USART_TX_PIN       GPIO_Pin_2
#define PM_USART_RX_PIN       GPIO_Pin_1
#define PM_USART_TX_GPIO_PORT GPIOA
#define PM_USART_RX_GPIO_PORT GPIOA
#define PM_USART_TX_SOURCE    GPIO_PinSource2
#define PM_USART_RX_SOURCE    GPIO_PinSource1
#define PM_USART_TX_AF        GPIO_AF_USART2
#define PM_USART_RX_AF        GPIO_AF_USART2
#define PM_USART_BAUDRATE     9600

#define PM_USART_IRQHandler   USART2_IRQHandler
#define PM_USART_IRQ          USART2_IRQn

#define RX_BUFFER_SIZE        1
#define PM_USART_REC_LEN      200  /* ���ʼ��������ֽ��� */


void usart_power_meter_init(void);
int parse_power_from_buf(float * outPower);
void pm_usart_it_config(FunctionalState NewState);

#endif
