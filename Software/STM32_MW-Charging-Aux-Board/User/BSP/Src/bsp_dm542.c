#include "bsp_dm542.h"
#include <stm32f4xx_conf.h>
#include "stm32f4xx.h"
#include <stdlib.h>
#include <math.h>
#include "FreeRTOSConfig.h"
#include <stdio.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "bsp_spi_flash.h"
#include "bsp_debug_usart.h"
#include "bsp_can.h"
struct ScrewMotorStatus horSM = {
    .movingCompletionStatus = finished,
		.currentPosition = 0.0f
		
};
struct ScrewMotorStatus verSM = {
    .movingCompletionStatus = finished,
		.currentPosition = 0.0f
};
// 互斥量
SemaphoreHandle_t dm542_USART3_Mutex;
//同步信号量：用于等待 GD32 反馈 "运动完成"
SemaphoreHandle_t xMotorMoveDoneSem = NULL;
/**
  * @brief  初始化 (现在只需初始化 CAN 和 信号量)
  * 参数 period, prescaler, pulse 不再使用
  **/
void hor_dm542_init(uint32_t period, uint16_t prescaler, uint32_t pulse)
{
		// 初始化 RTOS 对象
    if(dm542_USART3_Mutex == NULL)
        dm542_USART3_Mutex = xSemaphoreCreateMutex();
        
    if(xMotorMoveDoneSem == NULL)
        xMotorMoveDoneSem = xSemaphoreCreateBinary();

    // 初始化 CAN 通信 (替代原有的 GPIO/TIM 初始化) 
    bsp_can_init();
    
    //初始状态恢复 (读 Flash) 
    screw_motor_status_init();
	
}	
	
/**
  * @brief  纵向初始化 (保留接口兼容性，实际共用 CAN)
  **/
void ver_dm542_init(uint32_t period, uint16_t prescaler, uint32_t pulse)
{
}

/**
  * @brief  步进电机位置控制
  * @param  horPosition 横向丝杆目标位置 
  * @param  verPosition 纵向丝杆目标位置
  * @return 0位置不合理/1移动成功
  **/
int motor_position_ctrl(float horPosition, float verPosition)
{
	   
    //距离限制检查 (保留原有逻辑) 
    if ((fabs(horSM.currentPosition) > MAX_DISTANCE_VAL * 1000) || 
        (fabs(verSM.currentPosition) > MAX_DISTANCE_VAL * 1000))
        return 0;
        
    float horDistance = horPosition - horSM.currentPosition;
    float verDistance = verPosition - verSM.currentPosition;
    
    // 如果没有位移，直接返回
    if (fabs(horDistance) < 0.001f && fabs(verDistance) < 0.001f)
        return 1;

    // 设置状态为未完成
    horSM.movingCompletionStatus = unfinished;
    verSM.movingCompletionStatus = unfinished;

    //  清空信号量，防止旧数据干扰
    xSemaphoreTake(xMotorMoveDoneSem, 0);

    // 通过 CAN 发送目标位置给 GD32
    can_send_motor_cmd(horPosition, verPosition);

    // 阻塞等待 GD32 反馈 (带超时保护，例如 20秒)
    if (xSemaphoreTake(xMotorMoveDoneSem, pdMS_TO_TICKS(20000)) == pdTRUE)
    {
        // 收到 GD32 反馈后，horSM.currentPosition 已在 ISR 中更新
        horSM.movingCompletionStatus = finished;
        verSM.movingCompletionStatus = finished;
        
        // 在运动完成后记录当前值到Flash 
        motor_status_write_to_flash();
        
        return 1;
    }
    else
    {
        // 超时：GD32 没有回应
        printf("Error: Motor Timeout!\r\n");
        return 0;
    }
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
  * @brief              生成圆形轨迹
  * @param  points      存放轨迹点的数组（需要在外部分配好空间）
  * @param  num_points  轨迹点个数
  * @param  radius      圆的半径（单位 mm，必须小于 400）
  * @retval             0 表示成功，-1 表示失败（半径超出范围）
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
  * @brief               生成正方形轨迹
  * @param  points       存放轨迹点的数组（需要在外部分配好空间）
  * @param  num_points   轨迹点个数
  * @param  side_length  正方形边长（单位 mm，必须小于 800）
  * @retval              0 表示成功，-1 表示失败（边长超出范围）
  **/
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



void motor_status_add(void)
{
    horSM.currentPosition += 2;
    verSM.currentPosition += 2;
}
