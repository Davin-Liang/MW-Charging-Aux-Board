#define USE_SIMULATOR_TASK     0   // 开启模拟任务总开关
#define USE_CAN_LOOPBACK_TEST  0  // 开启 CAN 回环测试 (如果不定义，则使用队列直写测试)

#include "gd32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "motor_task.h" 
#include "app_protocol.h"

#include "bsp_debug_usart.h"

// 只有在 CAN 模式下才需要包含 CAN 头文件
#ifdef USE_CAN_LOOPBACK_TEST
    #include "bsp_can.h" 
#endif
extern QueueHandle_t motorCmdQueue;

// LED 初始化 (保持不变)
#define TEST_LED_PORT   GPIOB
#define TEST_LED_PIN    GPIO_PIN_4
#define TEST_LED_CLK    RCU_GPIOB

void LED_Init(void) {
    rcu_periph_clock_enable(TEST_LED_CLK);
    gpio_mode_set(TEST_LED_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, TEST_LED_PIN);
    gpio_output_options_set(TEST_LED_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, TEST_LED_PIN);
    gpio_bit_set(TEST_LED_PORT, TEST_LED_PIN); // 默认灭
}
/* 3. 模拟任务实现 (根据宏切换逻辑) */
#ifdef USE_SIMULATOR_TASK 

void Task_Simulator(void *pvParameters) {
    Coordinate_t testCmd;
    printf("[Sim] Simulator Task Started!\r\n"); 
    vTaskDelay(3000); 

    while(1) {
        /* --- 动作 1 --- */
        #ifdef USE_CAN_LOOPBACK_TEST
            // 模式 A: CAN 回环发送
            can_test_send_cmd(100.0f, 50.0f);
        #else
            // 模式 B: 队列直接发送
            testCmd.x = 100.0f; testCmd.y = 50.0f;
            printf("[Sim] Sending CMD (Queue): Move to (%.1f, %.1f)\r\n", testCmd.x, testCmd.y);
            if(motorCmdQueue) xQueueSend(motorCmdQueue, &testCmd, 0);
        #endif
        
        vTaskDelay(5000);

        /* --- 动作 2 --- */
        #ifdef USE_CAN_LOOPBACK_TEST
            can_test_send_cmd(0.0f, 0.0f);
        #else
            testCmd.x = 0.0f; testCmd.y = 0.0f;
            printf("[Sim] Sending CMD (Queue): Return Home\r\n");
            if(motorCmdQueue) xQueueSend(motorCmdQueue, &testCmd, 0);
        #endif
        
        vTaskDelay(5000); 
    }
}
#endif // USE_SIMULATOR_TASK

int main(void){
		SystemInit();
    SystemCoreClockUpdate(); // 别忘了这个
    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);
		debug_usart_init(); // 初始化串口，一定要放在最前面！
    printf("\r\n===================================\r\n");
    printf(" GD32F407 Motor Control System Boot\r\n");
    printf(" Build Date: %s %s\r\n", __DATE__, __TIME__);
    printf("===================================\r\n");
    LED_Init();
	

    // 1. LED任务
    // xTaskCreate(Task_Blink, "Blinky", 128, NULL, 2, NULL); 
    // 2. 创建电机任务
    xTaskCreate(MotorTask, "Motor", 512, NULL, 3, NULL);

    // 3. 创建模拟任务
		#ifdef USE_SIMULATOR_TASK
    xTaskCreate(Task_Simulator, "TestSim", 128, NULL, 1, NULL);
		#endif
		printf("[Main] Starting Scheduler...\r\n");
    vTaskStartScheduler();
    while(1);
}


