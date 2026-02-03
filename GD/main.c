#include "gd32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"

// 声明任务函数
extern void MotorCommsTask(void *pvParameters);

int main(void)
{
    /* 配置系统时钟与中断分组 (GD32F4 标准库方式) */
    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);

    /* 初始化其他的硬件 (如 DM542 相关的定时器和GPIO) */
    // screw_motor_status_init(); // 之前的代码
    // hor_dm542_init(...); 
    // ver_dm542_init(...);

    /* 创建任务 */
    xTaskCreate(MotorCommsTask, 
                "MotorTask", 
                512, 
                NULL, 
                3, // 优先级适中
                NULL);

    /* 启动调度器 */
    vTaskStartScheduler();

    while(1);
}