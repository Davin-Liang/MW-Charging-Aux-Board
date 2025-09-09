#include "bsp_dm542.h"
#include <stm32f4xx_conf.h>
#include "stm32f4xx.h"

/**
  * @brief  ��ʼ����DM542��ͨѶ����
  * @param  period TODO:
  * @param  prescaler
  * @param  pulse
  * @return void
  **/
void dm542_init(uint32_t period, uint16_t prescaler, uint32_t pulse)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	
	/* GPIO ʱ��ʹ�� */
    RCC_AHB1PeriphClockCmd(HOR_DM542_TIMER_PUL_GPIO_CLK | HOR_DM542_DIR_GPIO_CLK | HOR_DM542_ENA_GPIO_CLK, ENABLE);
    RCC_AHB1PeriphClockCmd(VER_DM542_TIMER_PUL_GPIO_CLK | VER_DM542_DIR_GPIO_CLK | VER_DM542_ENA_GPIO_CLK, ENABLE);

    /* ��ʱ��ͨ�����Ÿ��� */
	GPIO_PinAFConfig(HOR_DM542_TIMER_PUL_GPIO_PORT, 
                    HOR_DM542_TIMER_PUL_SOURCE,
                    HOR_DM542_TIMER_PUL_AF);
	GPIO_PinAFConfig(HOR_DM542_TIMER_PUL_GPIO_PORT, 
                    VER_DM542_TIMER_PUL_SOURCE,
                    VER_DM542_TIMER_PUL_AF);
  
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
    GPIO_InitStructure.GPIO_Pin = HOR_DM542_ENA_PIN;  
    GPIO_Init(HOR_DM542_ENA_GPIO_PORT, &GPIO_InitStructure);

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
    GPIO_InitStructure.GPIO_Pin = VER_DM542_ENA_PIN;  
    GPIO_Init(VER_DM542_ENA_GPIO_PORT, &GPIO_InitStructure);

	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
        
    /* TIMERʱ��ʹ�� */
    RCC_APB1PeriphClockCmd(HOR_DM542_TIMER_CLK, ENABLE);
    RCC_APB1PeriphClockCmd(VER_DM542_TIMER_CLK, ENABLE);

    /* �ۼ� TIM_Period�������һ�����»����ж� */		
    // ����ʱ����0������(period - 1)����Ϊ period �Σ�Ϊһ����ʱ����
    TIM_TimeBaseStructure.TIM_Period = period - 1;       
        
    // ͨ�ÿ��ƶ�ʱ��ʱ��ԴTIMxCLK = HCLK/2=84MHz 
    // �趨��ʱ��Ƶ��Ϊ=TIMxCLK/(TIM_Prescaler+1)=100KHz
    TIM_TimeBaseStructure.TIM_Prescaler = prescaler - 1;	
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; // ����ʱ�ӷ�Ƶ
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; // ������ʽ
    TIM_TimeBaseInit(HOR_DM542_TIM, &TIM_TimeBaseStructure);
    TIM_TimeBaseInit(VER_DM542_TIM, &TIM_TimeBaseStructure);
        
    /* PWMģʽ���� */
    /* PWM1 Mode configuration: Channel1 */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; // ����ΪPWMģʽ1
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
    TIM_OCInitStructure.TIM_Pulse = pulse - 1;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; // ����ʱ������ֵС��CCR1_ValʱΪ�ߵ�ƽ
    TIM_OC1Init(HOR_DM542_TIM, &TIM_OCInitStructure); // ʹ��ͨ��1
    TIM_OC1Init(VER_DM542_TIM, &TIM_OCInitStructure); // ʹ��ͨ��1
    
    /* ʹ��ͨ��1���� */
    TIM_OC1PreloadConfig(HOR_DM542_TIM, TIM_OCPreload_Enable);
    TIM_OC1PreloadConfig(VER_DM542_TIM, TIM_OCPreload_Enable);
        
    // /* ʹ�ܶ�ʱ�� */
    // TIM_Cmd(DM542_TIM, ENABLE);	
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
  * @brief  DM542������ʹ��/ʧ�ܿ���
  * @param  whichDm542 ��һ��DM542������
  * @param  bitVal setʹ��/resetʧ��
  * @return void
  **/
void dm542_ena_config(enum Dm542Def whichDm542, BitAction bitVal)
{
    if (whichDm542 == horDm542)
        GPIO_WriteBit(HOR_DM542_ENA_GPIO_PORT, HOR_DM542_ENA_PIN, bitVal);
    else if (whichDm542 == verDm542)
        GPIO_WriteBit(VER_DM542_ENA_GPIO_PORT, VER_DM542_ENA_PIN, bitVal);
}
