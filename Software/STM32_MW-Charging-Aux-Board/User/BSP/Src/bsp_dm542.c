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
#include "bsp_spi_flash.h"
#include "bsp_debug_usart.h"
#include <math.h>
#include <stdint.h>

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
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;  
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
       GPIO_ToggleBits(GPIOB, GPIO_Pin_8);
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
		
    /* 距离记录 */
    // TODO:待改进
    horSM.currentPosition = horPosition; 
    verSM.currentPosition = verPosition; 
    // 写入到 flash
    motor_status_write_to_flash();

    return 1;
}

/**
  * @brief  步进电机状态初始化，从flash中获取状态
  * @param  void
  * @return void
  **/
void screw_motor_status_init(void)
{
    motor_status_write_from_flash();
}	

/**
  * @brief  步进电机状态复位
  * @param  void
  * @return void
  **/
void screw_motor_status_reset(void)
{
    horSM.currentPosition = 0;
    verSM.currentPosition = 0;
    motor_status_write_to_flash();
}	

/**
  * @brief  将步进电机状态的数据写进 flash
  * @param  void
  * @return -1 没有和Flash建立通讯 / 1 写入数据成功
  **/
int motor_status_write_to_flash(void)
{
    __IO uint32_t flashID = 0;
    uint8_t dataFlag = DATA_FLAG;
    uint8_t motorStatusData[MOTOR_DATA_LEN] = {0};
    // uint8_t i = 0;

	/* 获取 SPI Flash ID */
	flashID = SPI_FLASH_ReadID();

    if (flashID != sFLASH_ID)
    {
        mutual_printf("[error]: No get flash ID!!!\r\n");
        return -1;
    }
        

    /* 整合数据 */
    motorStatusData[0] = dataFlag;
    uint32_t horPostionData = *(uint32_t*)&(horSM.currentPosition);
    motorStatusData[1] = horPostionData >> 24;
    motorStatusData[2] = horPostionData >> 16;
    motorStatusData[3] = horPostionData >>  8;
    motorStatusData[4] = horPostionData >>  0;

    uint32_t verPostionData = *(uint32_t*)&(verSM.currentPosition);
    motorStatusData[5] = verPostionData >> 24;
    motorStatusData[6] = verPostionData >> 16;
    motorStatusData[7] = verPostionData >>  8;
    motorStatusData[8] = verPostionData >>  0;
    
    /* 擦除扇区 */
    SPI_FLASH_SectorErase(0);

    /* 写入电机状态数据到第0页 */
    SPI_FLASH_BufferWrite((void*)motorStatusData, (SPI_FLASH_PageSize * 0), MOTOR_DATA_LEN);   
    mutual_printf("[write]: The data to be written --> horSM.currentposition = %f, verSM.currentposition = %f\r\n", horSM.currentPosition, verSM.currentPosition);

    return 1;
}

/**
  * @brief  从 flash 读取步进电机状态的数据
  * @param  void
  * @return -1 没有和Flash建立通讯 / 0 flash 中没有电机数据 / 1 读取数据成功
  **/
int motor_status_write_from_flash(void)
{
    __IO uint32_t flashID = 0;
    uint8_t dataFlag = 0;
    uint8_t motorStatusData[MOTOR_DATA_LEN] = {0};
    uint32_t horPostionData = 0;
    uint32_t verPostionData = 0;

	/* 获取 SPI Flash ID */
	flashID = SPI_FLASH_ReadID();

    if (flashID != sFLASH_ID)
    {
        mutual_printf("[error]: No get flash ID!!!\r\n");
        return -1;
    }
    /* 读取数据标志位 */
    SPI_FLASH_BufferRead((void*)&dataFlag, (SPI_FLASH_PageSize * 0), 1);

    if (dataFlag != DATA_FLAG)
        return 0;

    /* 读取数据 */
    SPI_FLASH_BufferRead((void*)motorStatusData, (SPI_FLASH_PageSize * 0), MOTOR_DATA_LEN);

    /* 数据提取 */
    horPostionData = motorStatusData[1] << 24 |
                    motorStatusData[2] << 16 |
                    motorStatusData[3] << 8 |
                    motorStatusData[4] << 0;
    verPostionData = motorStatusData[5] << 24 |
                    motorStatusData[6] << 16 |
                    motorStatusData[7] << 8 |
                    motorStatusData[8] << 0;
		mutual_printf("[read]: motorStatusData[1] = %d\r\n", motorStatusData[1]);
		mutual_printf("[read]: motorStatusData[2] = %d\r\n", motorStatusData[2]);
		mutual_printf("[read]: motorStatusData[3] = %d\r\n", motorStatusData[3]);
		mutual_printf("[read]: motorStatusData[4] = %d\r\n", motorStatusData[4]);
		mutual_printf("[read]: motorStatusData[5] = %d\r\n", motorStatusData[5]);
		mutual_printf("[read]: motorStatusData[6] = %d\r\n", motorStatusData[6]);
		mutual_printf("[read]: motorStatusData[7] = %d\r\n", motorStatusData[7]);
		mutual_printf("[read]: motorStatusData[8] = %d\r\n", motorStatusData[8]);

    horSM.currentPosition = *(float*)&horPostionData;
    verSM.currentPosition = *(float*)&verPostionData;

    mutual_printf("[read]: The data readed --> horSM.currentposition = %f, verSM.currentposition = %f\r\n", horSM.currentPosition, verSM.currentPosition);

    return 1;
}

/**
  * @brief  生成圆形轨迹
  * @param  points    存放轨迹点的数组（需要在外部分配好空间）
  * @param  num_points 轨迹点个数
  * @param  radius     圆的半径（单位 mm，必须小于 400）
  * @retval 0 表示成功，-1 表示失败（半径超出范围）
  **/
int generate_circle_trajectory(Point2D *points, int num_points, int radius)
{
    if (radius <= 0 || radius >= (int)(MAX_DISTANCE_VAL * 1000))
        return -1;  // 半径不合法


    for (int i = 0; i < num_points; i++) 
    {
        double theta = 2.0 * 3.14 * i / num_points;  // 当前角度

        /* 计算圆上点的坐标 */
        double x = radius * cos(theta);
        double y = radius * sin(theta);

        /* 转换为 int 型 */
        points[i].x = (int)lround(x);
        points[i].y = (int)lround(y);
			mutual_printf("points[%d].x = %d,points[%d].y = %d \r\n", i, points[i].x,i, points[i].y);
    }

    return 0; // 成功
}

/**
 * @brief  生成正方形轨迹
 * @param  points      存放轨迹点的数组（需要在外部分配好空间）
 * @param  num_points  轨迹点个数
 * @param  side_length 正方形边长（单位 mm，必须小于 800）
 * @retval 0 表示成功，-1 表示失败（边长超出范围）
 */
int generate_square_trajectory(Point2D *points, int num_points, int side_length)
{
    if (side_length <= 0 || side_length >= 800)
        return -1;  // 边长不合法

    if (num_points < 4)
        return -1;  // 至少要 4 个点才能形成正方形


    int half = side_length / 2;  // 正方形半边长
    int points_per_side = num_points / 4;  // 每条边分配的点数（不算余数）

    int idx = 0;

    // 1. 下边 (从左下到右下)
    for (int i = 0; i < points_per_side; i++) 
    {
        double t = (double)i / (points_per_side - 1);
        points[idx].x = (int)lround(-half + t * side_length);
        points[idx].y = -half;
        idx++;
    }

    // 2. 右边 (从右下到右上)
    for (int i = 0; i < points_per_side; i++) 
    {
        double t = (double)i / (points_per_side - 1);
        points[idx].x = half;
        points[idx].y = (int)lround(-half + t * side_length);
        idx++;
    }

    // 3. 上边 (从右上到左上)
    for (int i = 0; i < points_per_side; i++) 
    {
        double t = (double)i / (points_per_side - 1);
        points[idx].x = (int)lround(half - t * side_length);
        points[idx].y = half;
        idx++;
    }

    // 4. 左边 (从左上到左下)
    for (int i = 0; i < points_per_side; i++) 
    {
        double t = (double)i / (points_per_side - 1);
        points[idx].x = -half;
        points[idx].y = (int)lround(half - t * side_length);
        idx++;
    }

    // 如果点数不是 4 的倍数，还可能剩余一些点，把它们分布在起点附近
    while (idx < num_points) 
    {
        points[idx].x = -half;
        points[idx].y = -half;
        idx++;
    }

    return 0; // 成功
}

void set_motor_speed(enum Dm542Def whichDm542, float motorAngularVel)
{
    float motorRpm;
    uint32_t neededFpulse;
    uint32_t neededArr, neededCcr;

    motorRpm = motorAngularVel / (2 * 3.14);
    neededFpulse = MOTOR_PRR * motorRpm;
    neededArr = (int)(NEEDED_CK_CNT / neededFpulse);
    neededCcr = (int)(neededArr / 2);

    if (whichDm542 == horDm542)
    {
        dm542_pul_config(whichDm542, DISABLE);
        
        TIM_SetAutoreload(HOR_DM542_TIM, neededArr);
        TIM_PrescalerConfig(HOR_DM542_TIM, NEEDED_PSC, TIM_PSCReloadMode_Immediate);
        TIM_SetCompare1(HOR_DM542_TIM, neededCcr);
    }
    if (whichDm542 == verDm542)
    {
        dm542_pul_config(whichDm542, DISABLE);
        
        TIM_SetAutoreload(VER_DM542_TIM, neededArr);
        TIM_PrescalerConfig(VER_DM542_TIM, NEEDED_PSC, TIM_PSCReloadMode_Immediate);
        TIM_SetCompare1(VER_DM542_TIM, neededCcr);
    } 
}



void motor_status_add(void)
{
    horSM.currentPosition += 2;
    verSM.currentPosition += 2;
}
