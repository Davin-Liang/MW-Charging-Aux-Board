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
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;  
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
       GPIO_ToggleBits(GPIOB, GPIO_Pin_8);
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
		
    /* �����¼ */
    // TODO:���Ľ�
    horSM.currentPosition = horPosition; 
    verSM.currentPosition = verPosition; 
    // д�뵽 flash
    motor_status_write_to_flash();

    return 1;
}

/**
  * @brief  �������״̬��ʼ������flash�л�ȡ״̬
  * @param  void
  * @return void
  **/
void screw_motor_status_init(void)
{
    motor_status_write_from_flash();
}	

/**
  * @brief  �������״̬��λ
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
  * @brief  ���������״̬������д�� flash
  * @param  void
  * @return -1 û�к�Flash����ͨѶ / 1 д�����ݳɹ�
  **/
int motor_status_write_to_flash(void)
{
    __IO uint32_t flashID = 0;
    uint8_t dataFlag = DATA_FLAG;
    uint8_t motorStatusData[MOTOR_DATA_LEN] = {0};
    // uint8_t i = 0;

	/* ��ȡ SPI Flash ID */
	flashID = SPI_FLASH_ReadID();

    if (flashID != sFLASH_ID)
    {
        mutual_printf("[error]: No get flash ID!!!\r\n");
        return -1;
    }
        

    /* �������� */
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
    
    /* �������� */
    SPI_FLASH_SectorErase(0);

    /* д����״̬���ݵ���0ҳ */
    SPI_FLASH_BufferWrite((void*)motorStatusData, (SPI_FLASH_PageSize * 0), MOTOR_DATA_LEN);   
    mutual_printf("[write]: The data to be written --> horSM.currentposition = %f, verSM.currentposition = %f\r\n", horSM.currentPosition, verSM.currentPosition);

    return 1;
}

/**
  * @brief  �� flash ��ȡ�������״̬������
  * @param  void
  * @return -1 û�к�Flash����ͨѶ / 0 flash ��û�е������ / 1 ��ȡ���ݳɹ�
  **/
int motor_status_write_from_flash(void)
{
    __IO uint32_t flashID = 0;
    uint8_t dataFlag = 0;
    uint8_t motorStatusData[MOTOR_DATA_LEN] = {0};
    uint32_t horPostionData = 0;
    uint32_t verPostionData = 0;

	/* ��ȡ SPI Flash ID */
	flashID = SPI_FLASH_ReadID();

    if (flashID != sFLASH_ID)
    {
        mutual_printf("[error]: No get flash ID!!!\r\n");
        return -1;
    }
    /* ��ȡ���ݱ�־λ */
    SPI_FLASH_BufferRead((void*)&dataFlag, (SPI_FLASH_PageSize * 0), 1);

    if (dataFlag != DATA_FLAG)
        return 0;

    /* ��ȡ���� */
    SPI_FLASH_BufferRead((void*)motorStatusData, (SPI_FLASH_PageSize * 0), MOTOR_DATA_LEN);

    /* ������ȡ */
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
  * @brief  ����Բ�ι켣
  * @param  points    ��Ź켣������飨��Ҫ���ⲿ����ÿռ䣩
  * @param  num_points �켣�����
  * @param  radius     Բ�İ뾶����λ mm������С�� 400��
  * @retval 0 ��ʾ�ɹ���-1 ��ʾʧ�ܣ��뾶������Χ��
  **/
int generate_circle_trajectory(Point2D *points, int num_points, int radius)
{
    if (radius <= 0 || radius >= (int)(MAX_DISTANCE_VAL * 1000))
        return -1;  // �뾶���Ϸ�


    for (int i = 0; i < num_points; i++) 
    {
        double theta = 2.0 * 3.14 * i / num_points;  // ��ǰ�Ƕ�

        /* ����Բ�ϵ������ */
        double x = radius * cos(theta);
        double y = radius * sin(theta);

        /* ת��Ϊ int �� */
        points[i].x = (int)lround(x);
        points[i].y = (int)lround(y);
			mutual_printf("points[%d].x = %d,points[%d].y = %d \r\n", i, points[i].x,i, points[i].y);
    }

    return 0; // �ɹ�
}

/**
 * @brief  ���������ι켣
 * @param  points      ��Ź켣������飨��Ҫ���ⲿ����ÿռ䣩
 * @param  num_points  �켣�����
 * @param  side_length �����α߳�����λ mm������С�� 800��
 * @retval 0 ��ʾ�ɹ���-1 ��ʾʧ�ܣ��߳�������Χ��
 */
int generate_square_trajectory(Point2D *points, int num_points, int side_length)
{
    if (side_length <= 0 || side_length >= 800)
        return -1;  // �߳����Ϸ�

    if (num_points < 4)
        return -1;  // ����Ҫ 4 ��������γ�������


    int half = side_length / 2;  // �����ΰ�߳�
    int points_per_side = num_points / 4;  // ÿ���߷���ĵ���������������

    int idx = 0;

    // 1. �±� (�����µ�����)
    for (int i = 0; i < points_per_side; i++) 
    {
        double t = (double)i / (points_per_side - 1);
        points[idx].x = (int)lround(-half + t * side_length);
        points[idx].y = -half;
        idx++;
    }

    // 2. �ұ� (�����µ�����)
    for (int i = 0; i < points_per_side; i++) 
    {
        double t = (double)i / (points_per_side - 1);
        points[idx].x = half;
        points[idx].y = (int)lround(-half + t * side_length);
        idx++;
    }

    // 3. �ϱ� (�����ϵ�����)
    for (int i = 0; i < points_per_side; i++) 
    {
        double t = (double)i / (points_per_side - 1);
        points[idx].x = (int)lround(half - t * side_length);
        points[idx].y = half;
        idx++;
    }

    // 4. ��� (�����ϵ�����)
    for (int i = 0; i < points_per_side; i++) 
    {
        double t = (double)i / (points_per_side - 1);
        points[idx].x = -half;
        points[idx].y = (int)lround(half - t * side_length);
        idx++;
    }

    // ����������� 4 �ı�����������ʣ��һЩ�㣬�����Ƿֲ�����㸽��
    while (idx < num_points) 
    {
        points[idx].x = -half;
        points[idx].y = -half;
        idx++;
    }

    return 0; // �ɹ�
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
