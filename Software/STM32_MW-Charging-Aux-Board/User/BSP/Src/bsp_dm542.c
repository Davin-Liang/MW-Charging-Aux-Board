#include "bsp_dm542.h"
#include <stm32f4xx_conf.h>
#include "stm32f4xx.h"

/**
  * @brief  初始化和DM542的通讯设置
  * @param  period TODO:
  * @param  prescaler
  * @param  pulse
  * @return void
  **/
void dm542_init(uint32_t period, uint16_t prescaler, uint32_t pulse)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	
	/* GPIO 时钟使能 */
    RCC_AHB1PeriphClockCmd(HOR_DM542_TIMER_PUL_GPIO_CLK | HOR_DM542_DIR_GPIO_CLK | HOR_DM542_ENA_GPIO_CLK, ENABLE);
    RCC_AHB1PeriphClockCmd(VER_DM542_TIMER_PUL_GPIO_CLK | VER_DM542_DIR_GPIO_CLK | VER_DM542_ENA_GPIO_CLK, ENABLE);

    /* 定时器通道引脚复用 */
	GPIO_PinAFConfig(HOR_DM542_TIMER_PUL_GPIO_PORT, 
                    HOR_DM542_TIMER_PUL_SOURCE,
                    HOR_DM542_TIMER_PUL_AF);
	GPIO_PinAFConfig(HOR_DM542_TIMER_PUL_GPIO_PORT, 
                    VER_DM542_TIMER_PUL_SOURCE,
                    VER_DM542_TIMER_PUL_AF);
  
    /* GPIO初始化 */
    // 横向DM542驱动器GPIO设置
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

    // 纵向DM542驱动器GPIO设置
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
        
    /* TIMER时钟使能 */
    RCC_APB1PeriphClockCmd(HOR_DM542_TIMER_CLK, ENABLE);
    RCC_APB1PeriphClockCmd(VER_DM542_TIMER_CLK, ENABLE);

    /* 累计 TIM_Period个后产生一个更新或者中断 */		
    // 当定时器从0计数到(period - 1)，即为 period 次，为一个定时周期
    TIM_TimeBaseStructure.TIM_Period = period - 1;       
        
    // 通用控制定时器时钟源TIMxCLK = HCLK/2=84MHz 
    // 设定定时器频率为=TIMxCLK/(TIM_Prescaler+1)=100KHz
    TIM_TimeBaseStructure.TIM_Prescaler = prescaler - 1;	
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; // 采样时钟分频
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; // 计数方式
    TIM_TimeBaseInit(HOR_DM542_TIM, &TIM_TimeBaseStructure);
    TIM_TimeBaseInit(VER_DM542_TIM, &TIM_TimeBaseStructure);
        
    /* PWM模式配置 */
    /* PWM1 Mode configuration: Channel1 */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; // 配置为PWM模式1
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
    TIM_OCInitStructure.TIM_Pulse = pulse - 1;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; // 当定时器计数值小于CCR1_Val时为高电平
    TIM_OC1Init(HOR_DM542_TIM, &TIM_OCInitStructure); // 使能通道1
    TIM_OC1Init(VER_DM542_TIM, &TIM_OCInitStructure); // 使能通道1
    
    /* 使能通道1重载 */
    TIM_OC1PreloadConfig(HOR_DM542_TIM, TIM_OCPreload_Enable);
    TIM_OC1PreloadConfig(VER_DM542_TIM, TIM_OCPreload_Enable);
        
    // /* 使能定时器 */
    // TIM_Cmd(DM542_TIM, ENABLE);	
}

/**
  * @brief  DM542驱动器方向控制
  * @param  whichDm542 哪一个DM542驱动器
  * @param  bitVal set正向/reset反向
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
  * @brief  DM542驱动器使能/失能控制
  * @param  whichDm542 哪一个DM542驱动器
  * @param  bitVal set使能/reset失能
  * @return void
  **/
void dm542_ena_config(enum Dm542Def whichDm542, BitAction bitVal)
{
    if (whichDm542 == horDm542)
        GPIO_WriteBit(HOR_DM542_ENA_GPIO_PORT, HOR_DM542_ENA_PIN, bitVal);
    else if (whichDm542 == verDm542)
        GPIO_WriteBit(VER_DM542_ENA_GPIO_PORT, VER_DM542_ENA_PIN, bitVal);
}
