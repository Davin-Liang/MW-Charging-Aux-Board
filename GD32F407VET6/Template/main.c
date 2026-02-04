#include "gd32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"


#define TEST_LED_PORT  GPIOB
#define TEST_LED_PIN   GPIO_PIN_4
#define TEST_LED_CLK   RCU_GPIOE

void LED_Init(void) {
    rcu_periph_clock_enable(TEST_LED_CLK);
    gpio_mode_set(TEST_LED_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, TEST_LED_PIN);
    gpio_output_options_set(TEST_LED_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, TEST_LED_PIN);
}

void Task_Blink(void *pvParameters) {
    while(1) {
        gpio_bit_toggle(TEST_LED_PORT, TEST_LED_PIN);
        vTaskDelay(500); // 500ms 翻转一次
    }
}

int main(void) {
    // 1. 中断分组配置 (必须)
    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);
    
    // 2. 初始化硬件
    LED_Init();

    // 3. 创建测试任务
    xTaskCreate(Task_Blink, "Blinky", 128, NULL, 2, NULL);

    // 4. 开启调度
    vTaskStartScheduler();

    while(1);
}
