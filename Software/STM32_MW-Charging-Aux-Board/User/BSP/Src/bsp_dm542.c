#include "bsp_dm542.h"
#include <stm32f4xx_conf.h>
#include "stm32f4xx.h"
#include <stdlib.h>
#include <math.h>
#include "FreeRTOSConfig.h"
#include <stdint.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"

struct ScrewMotorStatus horSM = {
    .movingCompletionStatus = finished,
		.currentPosition = 0.0f
		
};
struct ScrewMotorStatus verSM = {
    .movingCompletionStatus = finished,
		.currentPosition = 0.0f
};

/**
  * @brief  ��ʼ����DM542��ͨѶ����
  * @param  period TODO:
  * @param  prescaler
  * @param  pulse
  * @return void
  **/
void hor_dm542_init(uint32_t period, uint16_t prescaler, uint32_t pulse)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	
	/* GPIO ʱ��ʹ�� */
    RCC_AHB1PeriphClockCmd(HOR_DM542_TIMER_PUL_GPIO_CLK | HOR_DM542_DIR_GPIO_CLK, ENABLE);

    /* ��ʱ��ͨ�����Ÿ��� */
	GPIO_PinAFConfig(HOR_DM542_TIMER_PUL_GPIO_PORT, 
                    HOR_DM542_TIMER_PUL_SOURCE,
                    HOR_DM542_TIMER_PUL_AF);
  
    /* GPIO��ʼ�� */
    // ����DM542������GPIO����
	GPIO_InitStructure.GPIO_Pin = HOR_DM542_TIMER_PUL_PIN;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;    
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 
	GPIO_Init(HOR_DM542_TIMER_PUL_GPIO_PORT, &GPIO_InitStructure);  

    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Pin = HOR_DM542_DIR_PIN;  
    GPIO_Init(HOR_DM542_DIR_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;  
    GPIO_Init(GPIOA, &GPIO_InitStructure);


	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
        
    /* TIMERʱ��ʹ�� */
    RCC_APB1PeriphClockCmd(HOR_DM542_TIMER_CLK, ENABLE);

    /* �ۼ� TIM_Period�������һ�����»����ж� */		
    // ����ʱ����0������(period - 1)����Ϊ period �Σ�Ϊһ����ʱ����
    TIM_TimeBaseStructure.TIM_Period = period;       
        
    // ͨ�ÿ��ƶ�ʱ��ʱ��ԴTIMxCLK = HCLK/2=84MHz 
    // �趨��ʱ��Ƶ��Ϊ=TIMxCLK/(TIM_Prescaler+1)=100KHz
    TIM_TimeBaseStructure.TIM_Prescaler = prescaler;	
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; // ����ʱ�ӷ�Ƶ
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; // ������ʽ
    TIM_TimeBaseInit(HOR_DM542_TIM, &TIM_TimeBaseStructure);
        
    /* PWMģʽ���� */
    /* PWM1 Mode configuration: Channel1 */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; // ����ΪPWMģʽ1
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
    TIM_OCInitStructure.TIM_Pulse = pulse;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; // ����ʱ������ֵС��CCR1_ValʱΪ�ߵ�ƽ
    TIM_OC1Init(HOR_DM542_TIM, &TIM_OCInitStructure); // ʹ��ͨ��1
    
    /* ʹ��ͨ��1���� */
    TIM_OC1PreloadConfig(HOR_DM542_TIM, TIM_OCPreload_Enable);

    /* ѡ������ʱ�� */
    TIM_SelectMasterSlaveMode(HOR_DM542_TIM, TIM_MasterSlaveMode_Enable);

    /* ����TIM��ģʽ: �����¼���Ϊ�������(TRGO) */
    TIM_SelectOutputTrigger(HOR_DM542_TIM, TIM_TRGOSource_Update);

    // ʹ�ܴӶ�ʱ��ʱ��
    RCC_APB1PeriphClockCmd(HOR_DM542_SLAVE_TIMER_CLK, ENABLE);
    
    // ��ʱ����������
    TIM_TimeBaseStructure.TIM_Prescaler = 0; // ��Ԥ��Ƶ
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period = 799; // TODO:
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(HOR_DM542_SLAVE_TIM, &TIM_TimeBaseStructure);
    
    // ���ô�ģʽ: �ⲿʱ��ģʽ1, ����ԴΪITR2(TIM3)
    TIM_SelectInputTrigger(HOR_DM542_SLAVE_TIM, TIM_TS_ITR2);
    TIM_SelectSlaveMode(HOR_DM542_SLAVE_TIM, TIM_SlaveMode_External1);
    
    // ����NVIC
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
		
    // ʹ��TIM2�����ж�
    TIM_ITConfig(HOR_DM542_SLAVE_TIM, TIM_IT_Update, ENABLE);
		
    TIM_Cmd(HOR_DM542_SLAVE_TIM, ENABLE);
    TIM_Cmd(HOR_DM542_SLAVE_TIM, DISABLE);
}

void ver_dm542_init(uint32_t period, uint16_t prescaler, uint32_t pulse)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* GPIO ʱ��ʹ�� */
    RCC_AHB1PeriphClockCmd(VER_DM542_TIMER_PUL_GPIO_CLK | VER_DM542_DIR_GPIO_CLK, ENABLE);

   /* ��ʱ��ͨ�����Ÿ��� */
	GPIO_PinAFConfig(VER_DM542_TIMER_PUL_GPIO_PORT, 
                   VER_DM542_TIMER_PUL_SOURCE,
                   VER_DM542_TIMER_PUL_AF);
 
   /* GPIO��ʼ�� */
   // ����DM542������GPIO����
	GPIO_InitStructure.GPIO_Pin = VER_DM542_TIMER_PUL_PIN;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;    
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 
	GPIO_Init(VER_DM542_TIMER_PUL_GPIO_PORT, &GPIO_InitStructure);  

    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Pin = VER_DM542_DIR_PIN;  
    GPIO_Init(VER_DM542_DIR_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;  
    GPIO_Init(GPIOB, &GPIO_InitStructure);

	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
       
    /* TIMERʱ��ʹ�� */
    RCC_APB1PeriphClockCmd(VER_DM542_TIMER_CLK, ENABLE);

    /* �ۼ� TIM_Period�������һ�����»����ж� */		
    // ����ʱ����0������(period - 1)����Ϊ period �Σ�Ϊһ����ʱ����
    TIM_TimeBaseStructure.TIM_Period = period;       
        
    // ͨ�ÿ��ƶ�ʱ��ʱ��ԴTIMxCLK = HCLK/2=84MHz 
    // �趨��ʱ��Ƶ��Ϊ=TIMxCLK/(TIM_Prescaler+1)=100KHz
    TIM_TimeBaseStructure.TIM_Prescaler = prescaler;	
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; // ����ʱ�ӷ�Ƶ
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; // ������ʽ
    TIM_TimeBaseInit(VER_DM542_TIM, &TIM_TimeBaseStructure);
        
    /* PWMģʽ���� */
    /* PWM1 Mode configuration: Channel1 */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; // ����ΪPWMģʽ1
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
    TIM_OCInitStructure.TIM_Pulse = pulse;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; // ����ʱ������ֵС��CCR1_ValʱΪ�ߵ�ƽ
    TIM_OC1Init(VER_DM542_TIM, &TIM_OCInitStructure); // ʹ��ͨ��1
    
    /* ʹ��ͨ��1���� */
    TIM_OC1PreloadConfig(VER_DM542_TIM, TIM_OCPreload_Enable);

    /* ѡ������ʱ�� */
    TIM_SelectMasterSlaveMode(VER_DM542_TIM, TIM_MasterSlaveMode_Enable);

    /* ����TIM��ģʽ: �����¼���Ϊ�������(TRGO) */
    TIM_SelectOutputTrigger(VER_DM542_TIM, TIM_TRGOSource_Update);

    // ʹ�ܴӶ�ʱ��ʱ��
    RCC_APB1PeriphClockCmd(VER_DM542_SLAVE_TIMER_CLK, ENABLE);
    
    // ��ʱ����������
    TIM_TimeBaseStructure.TIM_Prescaler = 0; // ��Ԥ��Ƶ
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period = 799; // TODO:
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(VER_DM542_SLAVE_TIM, &TIM_TimeBaseStructure);
    
    // ���ô�ģʽ: �ⲿʱ��ģʽ1, ����ԴΪITR2(TIM3)
    TIM_SelectInputTrigger(VER_DM542_SLAVE_TIM, TIM_TS_ITR2);
    TIM_SelectSlaveMode(VER_DM542_SLAVE_TIM, TIM_SlaveMode_External1);
    
    // ����NVIC
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
		
    // ʹ��TIM2�����ж�
    TIM_ITConfig(VER_DM542_SLAVE_TIM, TIM_IT_Update, ENABLE);
		
    TIM_Cmd(VER_DM542_SLAVE_TIM, ENABLE);
    TIM_Cmd(VER_DM542_SLAVE_TIM, DISABLE);   
}

/**
  * @brief  DM542�������������
  * @param  whichDm542 ��һ��DM542������
  * @param  bitVal set����/reset����
  * @return void
  **/
void dm542_dir_config(enum Dm542Def whichDm542, BitAction bitVal)
{
    if (whichDm542 == horDm542)
        GPIO_WriteBit(HOR_DM542_DIR_GPIO_PORT, HOR_DM542_DIR_PIN, bitVal);
    else if (whichDm542 == verDm542)
        GPIO_WriteBit(VER_DM542_DIR_GPIO_PORT, VER_DM542_DIR_PIN, bitVal);
}

/**
  * @brief  �Ƿ���������DM542������
  * @param  whichDm542 ��һ��DM542������
  * @param  newState ENABLE��/DISABLE��
  * @return void
  **/
void dm542_pul_config(enum Dm542Def whichDm542, FunctionalState newState)
{
    if (whichDm542 == horDm542)
    {
        TIM_Cmd(HOR_DM542_SLAVE_TIM, newState);
        TIM_Cmd(HOR_DM542_TIM, newState);
    }
    else if (whichDm542 == verDm542)
    {
        TIM_Cmd(VER_DM542_SLAVE_TIM, newState);
        TIM_Cmd(VER_DM542_TIM, newState);  
    }
}

void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(HOR_DM542_SLAVE_TIM, TIM_IT_Update) != RESET)
    {
        // ����жϱ�־
        TIM_ClearITPendingBit(HOR_DM542_SLAVE_TIM, TIM_IT_Update);
        
        // ֹͣ��ʱ��
        TIM_Cmd(HOR_DM542_SLAVE_TIM, DISABLE);
        TIM_Cmd(HOR_DM542_TIM, DISABLE);
        horSM.movingCompletionStatus = finished;
        GPIO_ToggleBits(GPIOA, GPIO_Pin_8);
    }
}

void TIM5_IRQHandler(void)
{
    if (TIM_GetITStatus(VER_DM542_SLAVE_TIM, TIM_IT_Update) != RESET)
    {
        // ����жϱ�־
        TIM_ClearITPendingBit(VER_DM542_SLAVE_TIM, TIM_IT_Update);
        
        // ֹͣ��ʱ��
        TIM_Cmd(VER_DM542_SLAVE_TIM, DISABLE);
        TIM_Cmd(VER_DM542_TIM, DISABLE);
        verSM.movingCompletionStatus = finished;
        GPIO_ToggleBits(GPIOB, GPIO_Pin_5);
    }
}

/**
  * @brief  �������λ�ÿ���
  * @param  horPosition ����˿��Ŀ��λ�� 
  * @param  verPosition ����˿��Ŀ��λ��
  * @return 0λ�ò�����/1�ƶ��ɹ�
  **/
int motor_position_ctrl(float horPosition, float verPosition)
{
    float horDistance, verDistance;

    /* �������� */
    if ((fabs(horSM.currentPosition)  > MAX_DISTANCE_VAL * 1000) || 
        (fabs(verSM.currentPosition) > MAX_DISTANCE_VAL * 1000))
        return 0;
		
    horDistance = horPosition - horSM.currentPosition;
    verDistance = verPosition - verSM.currentPosition;
		
    if (horDistance != 0)
        horSM.movingCompletionStatus = unfinished;
    if (verDistance != 0)
        verSM.movingCompletionStatus = unfinished;

    /* �����¼ */
    // TODO:���Ľ�
    horSM.currentPosition = horPosition; 
    verSM.currentPosition = verPosition; 
    // TODO:д�뵽 flash


    /* ���򲽽����������� */
    if (horDistance >= 0)
        dm542_dir_config(horDm542, Bit_SET);
    else
        dm542_dir_config(horDm542, Bit_RESET);

    /* ���򲽽����������� */
    if (verDistance >= 0)
        dm542_dir_config(verDm542, Bit_SET);
    else
        dm542_dir_config(verDm542, Bit_RESET);

    /* Ŀ������������ */
    uint32_t horNeededPulse = 0;
    uint32_t verNeededPulse = 0;
    if (horDistance != 0)
        horNeededPulse = (int)(fabs(horDistance) / PULSE_EQUIVALENT);
    if (verDistance != 0)
        verNeededPulse = (int)(fabs(verDistance) / PULSE_EQUIVALENT);

    /* �����Զ�����ֵ */
    if (horDistance != 0)
    {
        TIM_SetAutoreload(HOR_DM542_SLAVE_TIM, horNeededPulse);
        dm542_pul_config(horDm542, ENABLE);
    }
    if (verDistance != 0)
    {
        TIM_SetAutoreload(VER_DM542_SLAVE_TIM, verNeededPulse);
        dm542_pul_config(verDm542, ENABLE);
    }

    /* ����ʱ������ */
    
    /* ����Ƿ񵽴�Ŀ����� */
    while (horSM.movingCompletionStatus == unfinished)
        vTaskDelay(5);
    while (verSM.movingCompletionStatus == unfinished)
        vTaskDelay(5);

    return 1;
}

/**
  * @brief  �������״̬��ʼ������flash�л�ȡ״̬
  * @param  void
  * @return void
  **/
void screw_motor_status_init(void)
{
	/* �� flash �л�ȡ���򲽽����ʵ�ʾ���ֵ */
    // TODO:
    /* �� flash �л�ȡ���򲽽����ʵ�ʾ���ֵ */
    // TODO:
}	
