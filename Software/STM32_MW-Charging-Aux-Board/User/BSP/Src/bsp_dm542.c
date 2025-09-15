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
  * @brief  初始化和DM542的通讯设置
  * @param  period TODO:
  * @param  prescaler
  * @param  pulse
  * @return void
  **/
void hor_dm542_init(uint32_t period, uint16_t prescaler, uint32_t pulse)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	
	/* GPIO 时钟使能 */
    RCC_AHB1PeriphClockCmd(HOR_DM542_TIMER_PUL_GPIO_CLK | HOR_DM542_DIR_GPIO_CLK, ENABLE);

    /* 定时器通道引脚复用 */
	GPIO_PinAFConfig(HOR_DM542_TIMER_PUL_GPIO_PORT, 
                    HOR_DM542_TIMER_PUL_SOURCE,
                    HOR_DM542_TIMER_PUL_AF);
  
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
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;  
    GPIO_Init(GPIOA, &GPIO_InitStructure);


	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
        
    /* TIMER时钟使能 */
    RCC_APB1PeriphClockCmd(HOR_DM542_TIMER_CLK, ENABLE);

    /* 累计 TIM_Period个后产生一个更新或者中断 */		
    // 当定时器从0计数到(period - 1)，即为 period 次，为一个定时周期
    TIM_TimeBaseStructure.TIM_Period = period;       
        
    // 通用控制定时器时钟源TIMxCLK = HCLK/2=84MHz 
    // 设定定时器频率为=TIMxCLK/(TIM_Prescaler+1)=100KHz
    TIM_TimeBaseStructure.TIM_Prescaler = prescaler;	
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; // 采样时钟分频
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; // 计数方式
    TIM_TimeBaseInit(HOR_DM542_TIM, &TIM_TimeBaseStructure);
        
    /* PWM模式配置 */
    /* PWM1 Mode configuration: Channel1 */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; // 配置为PWM模式1
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
    TIM_OCInitStructure.TIM_Pulse = pulse;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; // 当定时器计数值小于CCR1_Val时为高电平
    TIM_OC1Init(HOR_DM542_TIM, &TIM_OCInitStructure); // 使能通道1
    
    /* 使能通道1重载 */
    TIM_OC1PreloadConfig(HOR_DM542_TIM, TIM_OCPreload_Enable);

    /* 选定主定时器 */
    TIM_SelectMasterSlaveMode(HOR_DM542_TIM, TIM_MasterSlaveMode_Enable);

    /* 配置TIM主模式: 更新事件作为触发输出(TRGO) */
    TIM_SelectOutputTrigger(HOR_DM542_TIM, TIM_TRGOSource_Update);

    // 使能从定时器时钟
    RCC_APB1PeriphClockCmd(HOR_DM542_SLAVE_TIMER_CLK, ENABLE);
    
    // 定时器基础设置
    TIM_TimeBaseStructure.TIM_Prescaler = 0; // 无预分频
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period = 799; // TODO:
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(HOR_DM542_SLAVE_TIM, &TIM_TimeBaseStructure);
    
    // 配置从模式: 外部时钟模式1, 触发源为ITR2(TIM3)
    TIM_SelectInputTrigger(HOR_DM542_SLAVE_TIM, TIM_TS_ITR2);
    TIM_SelectSlaveMode(HOR_DM542_SLAVE_TIM, TIM_SlaveMode_External1);
    
    // 配置NVIC
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
		
    // 使能TIM2更新中断
    TIM_ITConfig(HOR_DM542_SLAVE_TIM, TIM_IT_Update, ENABLE);
		
    TIM_Cmd(HOR_DM542_SLAVE_TIM, ENABLE);
    TIM_Cmd(HOR_DM542_SLAVE_TIM, DISABLE);
}

void ver_dm542_init(uint32_t period, uint16_t prescaler, uint32_t pulse)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* GPIO 时钟使能 */
    RCC_AHB1PeriphClockCmd(VER_DM542_TIMER_PUL_GPIO_CLK | VER_DM542_DIR_GPIO_CLK, ENABLE);

   /* 定时器通道引脚复用 */
	GPIO_PinAFConfig(VER_DM542_TIMER_PUL_GPIO_PORT, 
                   VER_DM542_TIMER_PUL_SOURCE,
                   VER_DM542_TIMER_PUL_AF);
 
   /* GPIO初始化 */
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
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;  
    GPIO_Init(GPIOB, &GPIO_InitStructure);

	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
       
    /* TIMER时钟使能 */
    RCC_APB1PeriphClockCmd(VER_DM542_TIMER_CLK, ENABLE);

    /* 累计 TIM_Period个后产生一个更新或者中断 */		
    // 当定时器从0计数到(period - 1)，即为 period 次，为一个定时周期
    TIM_TimeBaseStructure.TIM_Period = period;       
        
    // 通用控制定时器时钟源TIMxCLK = HCLK/2=84MHz 
    // 设定定时器频率为=TIMxCLK/(TIM_Prescaler+1)=100KHz
    TIM_TimeBaseStructure.TIM_Prescaler = prescaler;	
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; // 采样时钟分频
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; // 计数方式
    TIM_TimeBaseInit(VER_DM542_TIM, &TIM_TimeBaseStructure);
        
    /* PWM模式配置 */
    /* PWM1 Mode configuration: Channel1 */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; // 配置为PWM模式1
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
    TIM_OCInitStructure.TIM_Pulse = pulse;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; // 当定时器计数值小于CCR1_Val时为高电平
    TIM_OC1Init(VER_DM542_TIM, &TIM_OCInitStructure); // 使能通道1
    
    /* 使能通道1重载 */
    TIM_OC1PreloadConfig(VER_DM542_TIM, TIM_OCPreload_Enable);

    /* 选定主定时器 */
    TIM_SelectMasterSlaveMode(VER_DM542_TIM, TIM_MasterSlaveMode_Enable);

    /* 配置TIM主模式: 更新事件作为触发输出(TRGO) */
    TIM_SelectOutputTrigger(VER_DM542_TIM, TIM_TRGOSource_Update);

    // 使能从定时器时钟
    RCC_APB1PeriphClockCmd(VER_DM542_SLAVE_TIMER_CLK, ENABLE);
    
    // 定时器基础设置
    TIM_TimeBaseStructure.TIM_Prescaler = 0; // 无预分频
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period = 799; // TODO:
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(VER_DM542_SLAVE_TIM, &TIM_TimeBaseStructure);
    
    // 配置从模式: 外部时钟模式1, 触发源为ITR2(TIM3)
    TIM_SelectInputTrigger(VER_DM542_SLAVE_TIM, TIM_TS_ITR2);
    TIM_SelectSlaveMode(VER_DM542_SLAVE_TIM, TIM_SlaveMode_External1);
    
    // 配置NVIC
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
		
    // 使能TIM2更新中断
    TIM_ITConfig(VER_DM542_SLAVE_TIM, TIM_IT_Update, ENABLE);
		
    TIM_Cmd(VER_DM542_SLAVE_TIM, ENABLE);
    TIM_Cmd(VER_DM542_SLAVE_TIM, DISABLE);   
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
  * @brief  是否输出脉冲给DM542驱动器
  * @param  whichDm542 哪一个DM542驱动器
  * @param  newState ENABLE是/DISABLE否
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
        // 清除中断标志
        TIM_ClearITPendingBit(HOR_DM542_SLAVE_TIM, TIM_IT_Update);
        
        // 停止定时器
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
        // 清除中断标志
        TIM_ClearITPendingBit(VER_DM542_SLAVE_TIM, TIM_IT_Update);
        
        // 停止定时器
        TIM_Cmd(VER_DM542_SLAVE_TIM, DISABLE);
        TIM_Cmd(VER_DM542_TIM, DISABLE);
        verSM.movingCompletionStatus = finished;
        GPIO_ToggleBits(GPIOB, GPIO_Pin_5);
    }
}

/**
  * @brief  步进电机位置控制
  * @param  horPosition 横向丝杆目标位置 
  * @param  verPosition 纵向丝杆目标位置
  * @return 0位置不合理/1移动成功
  **/
int motor_position_ctrl(float horPosition, float verPosition)
{
    float horDistance, verDistance;

    /* 距离限制 */
    if ((fabs(horSM.currentPosition)  > MAX_DISTANCE_VAL * 1000) || 
        (fabs(verSM.currentPosition) > MAX_DISTANCE_VAL * 1000))
        return 0;
		
    horDistance = horPosition - horSM.currentPosition;
    verDistance = verPosition - verSM.currentPosition;
		
    if (horDistance != 0)
        horSM.movingCompletionStatus = unfinished;
    if (verDistance != 0)
        verSM.movingCompletionStatus = unfinished;

    /* 距离记录 */
    // TODO:待改进
    horSM.currentPosition = horPosition; 
    verSM.currentPosition = verPosition; 
    // TODO:写入到 flash


    /* 横向步进电机方向控制 */
    if (horDistance >= 0)
        dm542_dir_config(horDm542, Bit_SET);
    else
        dm542_dir_config(horDm542, Bit_RESET);

    /* 纵向步进电机方向控制 */
    if (verDistance >= 0)
        dm542_dir_config(verDm542, Bit_SET);
    else
        dm542_dir_config(verDm542, Bit_RESET);

    /* 目标脉冲数计算 */
    uint32_t horNeededPulse = 0;
    uint32_t verNeededPulse = 0;
    if (horDistance != 0)
        horNeededPulse = (int)(fabs(horDistance) / PULSE_EQUIVALENT);
    if (verDistance != 0)
        verNeededPulse = (int)(fabs(verDistance) / PULSE_EQUIVALENT);

    /* 设置自动重载值 */
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

    /* 运行时间限制 */
    
    /* 检查是否到达目标距离 */
    while (horSM.movingCompletionStatus == unfinished)
        vTaskDelay(5);
    while (verSM.movingCompletionStatus == unfinished)
        vTaskDelay(5);

    return 1;
}

/**
  * @brief  步进电机状态初始化，从flash中获取状态
  * @param  void
  * @return void
  **/
void screw_motor_status_init(void)
{
	/* 从 flash 中获取横向步进电机实际距离值 */
    // TODO:
    /* 从 flash 中获取纵向步进电机实际距离值 */
    // TODO:
}	
