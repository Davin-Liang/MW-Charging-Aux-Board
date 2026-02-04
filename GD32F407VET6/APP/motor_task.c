#include "motor_task.h"
#include "bsp_dm542.h"
#include "bsp_can.h"
#include "queue.h"
#include "app_protocol.h"
QueueHandle_t motorCmdQueue;

// 假设 LED 接在 PE3 (根据你的点灯代码)
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

    // 3. 任务主循环
    while(1)
    {
        // 阻塞等待指令
        if(xQueueReceive(motorCmdQueue, &targetPos, portMAX_DELAY) == pdPASS)
        {
            // 收到指令，LED 翻转一次表示正在处理
            gpio_bit_toggle(DEBUG_LED_PORT, DEBUG_LED_PIN);
            
            // 执行运动 (阻塞直到完成)
            motor_position_ctrl(targetPos.x, targetPos.y);
            
            // 发送反馈
            can_send_position_feedback(horSM.currentPosition, verSM.currentPosition);
            
            // 完成后 LED 再翻转一次
            gpio_bit_toggle(DEBUG_LED_PORT, DEBUG_LED_PIN);
        }
    }
}
