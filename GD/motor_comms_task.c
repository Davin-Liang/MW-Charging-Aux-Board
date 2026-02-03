#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "bsp_can.h"
#include "bsp_dm542.h" // 引用你之前提供的步进电机驱动头文件
#include "app_protocol.h"

/* 定义全局队列句柄 */
QueueHandle_t motorCmdQueue;

/* 引用外部变量或函数 */
extern struct ScrewMotorStatus horSM;
extern struct ScrewMotorStatus verSM;
// 假设这是你之前代码中的电机控制函数
extern int motor_position_ctrl(float horPosition, float verPosition); 

/**
 * @brief  电机通信与控制主任务
 */
void MotorCommsTask(void *pvParameters)
{
    Coordinate_t targetPos;
    
    /* 1. 初始化 CAN */
    bsp_can_config();
    
    /* 2. 创建队列 (深度为5，每个元素是一个坐标结构体) */
    motorCmdQueue = xQueueCreate(5, sizeof(Coordinate_t));

    while(1)
    {
        /* 等待接收来自 STM32 的指令 (阻塞等待) */
        if(xQueueReceive(motorCmdQueue, &targetPos, portMAX_DELAY) == pdPASS)
        {
            // 收到新坐标，开始执行运动
            // 注意：motor_position_ctrl 在你之前的代码中貌似是阻塞的(while循环等待)
            // 如果是阻塞的，这里会一直等到运动完成
            motor_position_ctrl(targetPos.x, targetPos.y);

            /* 运动完成后，读取当前实际位置 */
            // 这里假设 motor_position_ctrl 会更新 horSM 和 verSM
            float currentX = horSM.currentPosition;
            float currentY = verSM.currentPosition;

            /* 通过 CAN 发送反馈给 STM32 */
            can_send_position_feedback(currentX, currentY);
        }
    }
}