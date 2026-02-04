#include "bsp_dm542.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include <math.h>

struct ScrewMotorStatus horSM = { 0.0f, finished }; // 顺序要跟结构体定义一致
struct ScrewMotorStatus verSM = { 0.0f, finished };

/* 通用初始化函数 (内部使用) */
static void dm542_base_init(uint32_t master_periph, uint32_t slave_periph, 
                            uint32_t period, uint16_t prescaler, uint32_t pulse, 
                            uint32_t slave_trig_src, uint8_t slave_irq)
{
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

    /* 1. 主定时器 (PWM) 配置 */
    timer_deinit(master_periph);
    timer_initpara.prescaler         = prescaler;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = period;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_init(master_periph, &timer_initpara);

    /* PWM 配置 (CH0) */
    timer_channel_output_struct_para_init(&timer_ocintpara); // 必须初始化结构体
    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.ocidlestate       = TIMER_OC_IDLE_STATE_LOW;
    timer_channel_output_config(master_periph, TIMER_CH_0, &timer_ocintpara);
    
    timer_channel_output_pulse_value_config(master_periph, TIMER_CH_0, pulse);
    timer_channel_output_mode_config(master_periph, TIMER_CH_0, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(master_periph, TIMER_CH_0, TIMER_OC_SHADOW_ENABLE);

    /* 主模式: 更新事件作为 TRGO */
    timer_master_slave_mode_config(master_periph, TIMER_MASTER_SLAVE_MODE_ENABLE);
    timer_master_output_trigger_source_select(master_periph, TIMER_TRI_OUT_SRC_UPDATE);

    /* 2. 从定时器 (计数) 配置 */
    timer_deinit(slave_periph);
    timer_initpara.prescaler         = 0;
    timer_initpara.period            = 0xFFFF; 
    timer_init(slave_periph, &timer_initpara);

    /* 从模式: 外部时钟模式1 */
    timer_slave_mode_select(slave_periph, TIMER_SLAVE_MODE_EXTERNAL0);
    /* 关键: 设置触发源 (ITR2/ITR3) */
    timer_input_trigger_source_select(slave_periph, slave_trig_src);

    /* 中断配置 */
    nvic_irq_enable((uint8_t)slave_irq, 5, 0); 
    timer_interrupt_enable(slave_periph, TIMER_INT_UP);
    
    /* 初始关闭 */
    timer_disable(slave_periph);
    timer_disable(master_periph);
}

void hor_dm542_init(uint32_t period, uint16_t prescaler, uint32_t pulse)
{
    /* 1. 时钟使能 */
    rcu_periph_clock_enable(HOR_PUL_CLK);
    rcu_periph_clock_enable(HOR_DIR_CLK);
    rcu_periph_clock_enable(HOR_TIM_CLK);
    rcu_periph_clock_enable(HOR_SLAVE_TIM_CLK); // RCU_TIMER1

    /* 2. GPIO 配置 */
    gpio_mode_set(HOR_PUL_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, HOR_PUL_PIN);
    gpio_output_options_set(HOR_PUL_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, HOR_PUL_PIN);
    gpio_af_set(HOR_PUL_PORT, HOR_PUL_AF, HOR_PUL_PIN);

    gpio_mode_set(HOR_DIR_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, HOR_DIR_PIN);
    gpio_output_options_set(HOR_DIR_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, HOR_DIR_PIN);

    /* 3. 定时器配置 (传入修正后的 ITR 宏) */
    dm542_base_init(HOR_TIM, HOR_SLAVE_TIM, period, prescaler, pulse, 
                    HOR_SLAVE_TRIG_SRC, HOR_SLAVE_IRQn);
}

void ver_dm542_init(uint32_t period, uint16_t prescaler, uint32_t pulse)
{
    /* 1. 时钟使能 */
    rcu_periph_clock_enable(VER_PUL_CLK);
    rcu_periph_clock_enable(VER_DIR_CLK);
    rcu_periph_clock_enable(VER_TIM_CLK);
    rcu_periph_clock_enable(VER_SLAVE_TIM_CLK); // RCU_TIMER4

    /* 2. GPIO 配置 */
    gpio_mode_set(VER_PUL_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, VER_PUL_PIN);
    gpio_output_options_set(VER_PUL_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, VER_PUL_PIN);
    gpio_af_set(VER_PUL_PORT, VER_PUL_AF, VER_PUL_PIN);

    gpio_mode_set(VER_DIR_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, VER_DIR_PIN);
    gpio_output_options_set(VER_DIR_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, VER_DIR_PIN);

    /* 3. 定时器配置 */
    dm542_base_init(VER_TIM, VER_SLAVE_TIM, period, prescaler, pulse, 
                    VER_SLAVE_TRIG_SRC, VER_SLAVE_IRQn);
}

void dm542_dir_config(Dm542Def whichDm542, bit_status bitVal)
{
    if (whichDm542 == horDm542)
        gpio_bit_write(HOR_DIR_PORT, HOR_DIR_PIN, bitVal);
    else
        gpio_bit_write(VER_DIR_PORT, VER_DIR_PIN, bitVal);
}

void dm542_pul_config(Dm542Def whichDm542, ControlStatus newState)
{
    uint32_t master, slave;
    if (whichDm542 == horDm542) { master = HOR_TIM; slave = HOR_SLAVE_TIM; }
    else                        { master = VER_TIM; slave = VER_SLAVE_TIM; }

    if (newState == ENABLE) {
        timer_enable(slave);
        timer_enable(master);
    } else {
        timer_disable(master);
        timer_disable(slave);
    }
}

int motor_position_ctrl(float horPosition, float verPosition)
{
    float horDistance = horPosition - horSM.currentPosition;
    float verDistance = verPosition - verSM.currentPosition;
		uint32_t horPulse = 0; // 提前声明并初始化
    uint32_t verPulse = 0;
    
    // 简单的距离判断
    if (fabs(horDistance) < 0.001f && fabs(verDistance) < 0.001f) return 1;

    // 1. 设置状态
    if (fabs(horDistance) >= 0.001f) horSM.movingCompletionStatus = unfinished;
    if (fabs(verDistance) >= 0.001f) verSM.movingCompletionStatus = unfinished;

    // 2. 设置方向
    dm542_dir_config(horDm542, (horDistance >= 0) ? SET : RESET);
    dm542_dir_config(verDm542, (verDistance >= 0) ? SET : RESET);

    // 3. 计算脉冲
    horPulse = (uint32_t)(fabs(horDistance) / PULSE_EQUIVALENT);
    verPulse = (uint32_t)(fabs(verDistance) / PULSE_EQUIVALENT);

    // 4. 设置脉冲数并启动 (注意 -1)
    if (horPulse > 0) {
        timer_autoreload_value_config(HOR_SLAVE_TIM, horPulse - 1);
        timer_event_software_generate(HOR_SLAVE_TIM, TIMER_EVENT_SRC_UPG);
        timer_flag_clear(HOR_SLAVE_TIM, TIMER_FLAG_UP);
        dm542_pul_config(horDm542, ENABLE);
    }
    if (verPulse > 0) {
        timer_autoreload_value_config(VER_SLAVE_TIM, verPulse - 1);
        timer_event_software_generate(VER_SLAVE_TIM, TIMER_EVENT_SRC_UPG);
        timer_flag_clear(VER_SLAVE_TIM, TIMER_FLAG_UP);
        dm542_pul_config(verDm542, ENABLE);
    }

    // 5. 等待完成
    while (horSM.movingCompletionStatus == unfinished || verSM.movingCompletionStatus == unfinished) {
        vTaskDelay(5);
    }

    horSM.currentPosition = horPosition;
    verSM.currentPosition = verPosition;
    

    return 1;
}

// 横向从定时器中断 (TIMER1)
void TIMER1_IRQHandler(void)
{
    if (timer_interrupt_flag_get(HOR_SLAVE_TIM, TIMER_INT_FLAG_UP) != RESET) {
        timer_interrupt_flag_clear(HOR_SLAVE_TIM, TIMER_INT_FLAG_UP);
        dm542_pul_config(horDm542, DISABLE);
        horSM.movingCompletionStatus = finished;
    }
}

// 纵向从定时器中断 (TIMER4)
void TIMER4_IRQHandler(void)
{
    if (timer_interrupt_flag_get(VER_SLAVE_TIM, TIMER_INT_FLAG_UP) != RESET) {
        timer_interrupt_flag_clear(VER_SLAVE_TIM, TIMER_INT_FLAG_UP);
        dm542_pul_config(verDm542, DISABLE);
        verSM.movingCompletionStatus = finished;
    }
}
