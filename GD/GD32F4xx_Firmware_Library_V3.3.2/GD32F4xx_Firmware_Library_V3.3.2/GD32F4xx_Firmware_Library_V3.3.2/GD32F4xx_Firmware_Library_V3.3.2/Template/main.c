#include "gd32f4xx.h"
#include "systick.h"

/* 假设 LED 接在 PE3 */
#define LED_PIN     GPIO_PIN_3
#define LED_PORT    GPIOE
#define LED_CLK     RCU_GPIOE

int main(void)
{
    /* 1. 配置系统时钟 (默认通常是 168MHz) */
    systick_config(); 

    /* 2. 开启 GPIO 时钟 */
    rcu_periph_clock_enable(LED_CLK);

    /* 3. 配置 GPIO 为推挽输出模式 */
    /* 参数: 端口, 模式, 上下拉, 引脚 */
    gpio_mode_set(LED_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_PIN);
    /* 参数: 端口, 输出类型, 速度, 引脚 */
    gpio_output_options_set(LED_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, LED_PIN);

    /* 4. 初始状态：点亮 (假设低电平点亮) */
    gpio_bit_reset(LED_PORT, LED_PIN);

    while(1)
    {
        /* 5. 翻转 LED 状态 */
        gpio_bit_toggle(LED_PORT, LED_PIN);
        
        /* 延时 500ms */
        delay_1ms(500);
    }
}
