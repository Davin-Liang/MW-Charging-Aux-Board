#ifndef __BSP_POWER_SUPPLY_H
#define	__BSP_POWER_SUPPLY_H

#include <stdint.h>

#define POWER_SUPPLY_DEFAULT_VOLTAGE 14.0f // 电源默认电压
#define TIME_OF_FINISHING_SETTING_VOL 200 // 设置电源电压成功所需要的时间
#define VOL_SENDING_TIME_INTERVAL 3000

#define PS_USART              USART3
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

/* 电源寄存器地址（4 路输出） */
#define PS_REG_ADDR(index) ((uint16_t[]){0x01FE, 0x02C6, 0x032A, 0x038E})[index] // 通过索引获取对应值

void usart_power_supply_init(void);
void set_power_supply_voltage(uint8_t slaveAddr, uint16_t regAddr, float voltage);
void set_voltage_for_power(float *voltage);

#endif
