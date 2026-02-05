#include "motor_task.h"
#include "bsp_dm542.h"
#include "bsp_can.h"
#include "queue.h"
#include "app_protocol.h"
#include "bsp_debug_usart.h"
QueueHandle_t motorCmdQueue;

// LED 
#define DEBUG_LED_PORT  GPIOB
#define DEBUG_LED_PIN   GPIO_PIN_4

void MotorTask(void *pvParameters)
{
    Coordinate_t targetPos;
    
    // 1. 初始化队列
    motorCmdQueue = xQueueCreate(5, sizeof(Coordinate_t));

    // 2. 初始化底层硬件
    bsp_can_config();
    
    // 初始化电机 (周期和占空比基于宏计算)
    hor_dm542_init(NEEDED_ARR - 1, NEEDED_PSC - 1, NEEDED_CCR);
    ver_dm542_init(NEEDED_ARR - 1, NEEDED_PSC - 1, NEEDED_CCR);

		printf("[Motor] Hardware Init Done. Waiting for commands...\r\n");
    // 3. 任务主循环
    while(1)
    {
        // 阻塞等待指令
        if(xQueueReceive(motorCmdQueue, &targetPos, portMAX_DELAY) == pdPASS)
        {
            // 收到指令，LED 翻转一次表示正在处理
            gpio_bit_toggle(DEBUG_LED_PORT, DEBUG_LED_PIN);
            // 收到指令打印
            printf("[Motor] Received CMD: X=%.2f, Y=%.2f. Moving...\r\n", targetPos.x, targetPos.y);
            // 执行运动 (阻塞直到完成)
            motor_position_ctrl(targetPos.x, targetPos.y);
            // 运动完成打印
            printf("[Motor] Position Reached! Feedback sent.\r\n");
            // 发送反馈
            can_send_position_feedback(horSM.currentPosition, verSM.currentPosition);
            
            // 完成后 LED 再翻转一次
            gpio_bit_toggle(DEBUG_LED_PORT, DEBUG_LED_PIN);
        }
    }
}
