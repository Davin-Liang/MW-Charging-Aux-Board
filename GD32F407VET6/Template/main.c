#include "gd32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "motor_task.h" // 包含新头文件

// --- 原有的 LED 定义 ---
#define TEST_LED_PORT   GPIOB
#define TEST_LED_PIN    GPIO_PIN_4
#define TEST_LED_CLK    RCU_GPIOB

void LED_Init(void) {
	rcu_periph_clock_enable(TEST_LED_CLK);
	gpio_mode_set(TEST_LED_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, TEST_LED_PIN);
	gpio_output_options_set(TEST_LED_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, TEST_LED_PIN);
	gpio_bit_set(TEST_LED_PORT, TEST_LED_PIN); // 默认灭
}

// 独立的点灯任务 
void Task_Blink(void *pvParameters) {
	while(1) {
			gpio_bit_toggle(TEST_LED_PORT, TEST_LED_PIN);
			vTaskDelay(500); // 500ms 闪烁
	}
}

int main(void) {
	// 2. 中断优先级分组
	nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);
	
	// 3. 硬件初始化
	LED_Init();
	

	// 4. 创建任务
	// 任务1: LED 心跳 (优先级 2)
	xTaskCreate(Task_Blink, "Blinky", 128, NULL, 2, NULL);
	
	// 任务2: 电机控制 (优先级 3)
	xTaskCreate(MotorTask, "Motor", 512, NULL, 3, NULL);

	// 5. 启动调度
	vTaskStartScheduler();

	while(1);
}
//#include "gd32f4xx.h"
//#include "FreeRTOS.h"
//#include "task.h"


//#define TEST_LED_PORT  GPIOB
//#define TEST_LED_PIN   GPIO_PIN_4
//#define TEST_LED_CLK   RCU_GPIOB

//void LED_Init(void) {
//    rcu_periph_clock_enable(TEST_LED_CLK);
//    gpio_mode_set(TEST_LED_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, TEST_LED_PIN);
//    gpio_output_options_set(TEST_LED_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, TEST_LED_PIN);
//}

//void Task_Blink(void *pvParameters) {
//    while(1) {
//        gpio_bit_toggle(TEST_LED_PORT, TEST_LED_PIN);
//        vTaskDelay(500); // 50ms 翻转一次
//    }
//}

//int main(void) {
//    // 1. 中断分组配置 
//    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);
//    
//    // 2. 初始化硬件
//    LED_Init();

//    // 3. 创建测试任务
//    xTaskCreate(Task_Blink, "Blinky", 128, NULL, 2, NULL);

//    // 4. 开启调度
//    vTaskStartScheduler();

//    while(1);
//}
