#ifndef __BSP_DM542_H
#define	__BSP_DM542_H

#include "FreeRTOS.h"

#define DM542_SUBDIVISION 16 // DM542ϸ����
#define SCREW_LEAD 10.f // ˿�˵���
#define MOTOR_STEP_ANGLE 1.8f // �����
#define MOTOR_PRR (360.f / MOTOR_STEP_ANGLE) // ���ÿת�����������
#define PULSE_EQUIVALENT (SCREW_LEAD / (MOTOR_PRR * DM542_SUBDIVISION)) // ���嵱��

/*
 * �᷽�򲽽����
 * PUL+ <����> PA6 <����> TIM3_CH4
 * DIR+ <����> PA7
 * ENA+ <����> PA8  
 */
#define HOR_DM542_TIMER_PUL_GPIO_CLK    RCC_AHB1Periph_GPIOA
#define HOR_DM542_TIMER_CLK 	        RCC_APB1Periph_TIM3
#define HOR_DM542_TIMER_PUL_PIN         GPIO_Pin_6
#define HOR_DM542_TIMER_PUL_GPIO_PORT   GPIOA
#define HOR_DM542_TIMER_PUL_SOURCE      GPIO_PinSource6
#define HOR_DM542_TIMER_PUL_AF          GPIO_AF_TIM3
#define HOR_DM542_TIM                   TIM3

#define HOR_DM542_DIR_GPIO_CLK          RCC_AHB1Periph_GPIOA
#define HOR_DM542_DIR_PIN               GPIO_Pin_7
#define HOR_DM542_DIR_GPIO_PORT         GPIOA

#define HOR_DM542_ENA_GPIO_CLK          RCC_AHB1Periph_GPIOA
#define HOR_DM542_ENA_PIN               GPIO_Pin_8
#define HOR_DM542_ENA_GPIO_PORT         GPIOA

/*
 * �ݷ��򲽽����
 * PUL+ <����> PB6 <����> TIM4_CH4
 * DIR+ <����> PB7
 * ENA+ <����> PB5
 */
#define VER_DM542_TIMER_PUL_GPIO_CLK    RCC_AHB1Periph_GPIOB
#define VER_DM542_TIMER_CLK 	        RCC_APB1Periph_TIM4
#define VER_DM542_TIMER_PUL_PIN         GPIO_Pin_6
#define VER_DM542_TIMER_PUL_GPIO_PORT   GPIOB
#define VER_DM542_TIMER_PUL_SOURCE      GPIO_PinSource6
#define VER_DM542_TIMER_PUL_AF          GPIO_AF_TIM4
#define VER_DM542_TIM                   TIM4

#define VER_DM542_DIR_GPIO_CLK          RCC_AHB1Periph_GPIOB
#define VER_DM542_DIR_PIN               GPIO_Pin_7
#define VER_DM542_DIR_GPIO_PORT         GPIOB

#define VER_DM542_ENA_GPIO_CLK          RCC_AHB1Periph_GPIOB
#define VER_DM542_ENA_PIN               GPIO_Pin_5
#define VER_DM542_ENA_GPIO_PORT         GPIOB

enum Dm542Def{
    horDm542, // ����DM542������
    verDm542, // ����DM542������
};

void dm542_init(uint32_t period, uint16_t prescaler, uint32_t pulse);
void dm542_dir_config(enum Dm542Def whichDm542, BitAction bitVal);
void dm542_ena_config(enum Dm542Def whichDm542, BitAction bitVal);

#endif
