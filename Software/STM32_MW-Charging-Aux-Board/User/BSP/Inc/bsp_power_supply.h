#ifndef __BSP_POWER_SUPPLY_H
#define	__BSP_POWER_SUPPLY_H

#include <stdint.h>

#define POWER_SUPPLY_DEFAULT_VOLTAGE 14.0f // ��ԴĬ�ϵ�ѹ
#define TIME_OF_FINISHING_SETTING_VOL 50 // ���õ�Դ��ѹ�ɹ�����Ҫ��ʱ��
#define VOL_SENDING_TIME_INTERVAL 1000

#define PS_USART              USART2
#define PS_USART_RX_GPIO_CLK  RCC_AHB1Periph_GPIOB
#define PS_USART_TX_GPIO_CLK  RCC_AHB1Periph_GPIOB
#define PS_USART_CLK 		  RCC_APB1Periph_USART3
#define PS_USART_TX_PIN       GPIO_Pin_10
#define PS_USART_RX_PIN       GPIO_Pin_11
#define PS_USART_TX_GPIO_PORT GPIOB
#define PS_USART_RX_GPIO_PORT GPIOB
#define PS_USART_TX_SOURCE    GPIO_PinSource10
#define PS_USART_RX_SOURCE    GPIO_PinSource11
#define PS_USART_TX_AF        GPIO_AF_USART3
#define PS_USART_RX_AF        GPIO_AF_USART3
#define PS_USART_BAUDRATE     9600

#define PS_USART_IRQHandler   USART3_IRQHandler
#define PS_USART_IRQ          USART3_IRQn

#define PS_SLAVE_ADDR         0x01

/* ��Դ�Ĵ�����ַ��4 ·����� */
#define PS_REG_ADDR(index) ((uint16_t[]){0x006E, 0x00D2, 0x0136, 0x019A})[index] // ͨ��������ȡ��Ӧֵ

void usart_power_supply_init(void);
void set_power_supply_voltage(uint8_t slaveAddr, uint16_t regAddr, float voltage);

#endif
