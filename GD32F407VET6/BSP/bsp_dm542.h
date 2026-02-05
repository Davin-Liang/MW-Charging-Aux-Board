#ifndef __BSP_DM542_H
#define __BSP_DM542_H

#include "gd32f4xx.h"
#include "FreeRTOS.h"
#include "semphr.h"

// --- 电机物理参数 ---
#define DM542_SUBDIVISION       5
#define SCREW_LEAD              10.0f
#define MOTOR_STEP_ANGLE        1.8f
#define MOTOR_PRR               ((360.0f / MOTOR_STEP_ANGLE) * DM542_SUBDIVISION)
#define PULSE_EQUIVALENT        (SCREW_LEAD / MOTOR_PRR)

// --- 定时器频率计算 (基于 GD32 APB1 = 84MHz) ---
#define MOTOR_ANGULAR_VEL       70.0f
#define MOTOR_RPM               (MOTOR_ANGULAR_VEL / (2.0f * 3.14159f))
#define NEEDED_FPULSE           (MOTOR_PRR * MOTOR_RPM)
#define NEEDED_PSC              128
// APB1 定时器时钟为 84MHz
#define NEEDED_CK_CNT           (84000000 / NEEDED_PSC) 
#define NEEDED_ARR              ((int)(NEEDED_CK_CNT / NEEDED_FPULSE))
#define NEEDED_CCR              ((int)(NEEDED_ARR / 2))

#define MAX_DISTANCE_VAL        0.4f // [m]
#define DATA_FLAG               0XCD
#define MOTOR_DATA_LEN          9

// --- 横向电机 (Horizontal) [PA6/PA8] ---
// PWM输出: PA6 -> AF2 -> TIMER2_CH0
#define HOR_TIM                 TIMER2
#define HOR_TIM_CLK             RCU_TIMER2
#define HOR_PUL_PORT            GPIOA
#define HOR_PUL_PIN             GPIO_PIN_6
#define HOR_PUL_CLK             RCU_GPIOA
#define HOR_PUL_AF              GPIO_AF_2
#define HOR_TIM_CHANNEL         TIMER_CH_0

// 脉冲计数: 使用 TIMER1 (32位) 作为 TIMER2 的从机
#define HOR_SLAVE_TIM           TIMER1
#define HOR_SLAVE_TIM_CLK       RCU_TIMER1
#define HOR_SLAVE_IRQn          TIMER1_IRQn
#define HOR_SLAVE_TRIG_SRC      TIMER_SMCFG_TRGSEL_ITI2

// 方向控制: PA8
#define HOR_DIR_PORT            GPIOA
#define HOR_DIR_PIN             GPIO_PIN_8
#define HOR_DIR_CLK             RCU_GPIOA

// --- 纵向电机 (Vertical) [PB6/PB7] ---
// PWM输出: PB6 -> AF2 -> TIMER3_CH0
#define VER_TIM                 TIMER3
#define VER_TIM_CLK             RCU_TIMER3
#define VER_PUL_PORT            GPIOB
#define VER_PUL_PIN             GPIO_PIN_6
#define VER_PUL_CLK             RCU_GPIOB
#define VER_PUL_AF              GPIO_AF_2
#define VER_TIM_CHANNEL         TIMER_CH_0

// 脉冲计数: 使用 TIMER4 (32位) 作为 TIMER3 的从机
// 连接关系: TIMER3_TRGO -> TIMER4_ITR2
#define VER_SLAVE_TIM           TIMER4
#define VER_SLAVE_TIM_CLK       RCU_TIMER4
#define VER_SLAVE_IRQn          TIMER4_IRQn
#define VER_SLAVE_TRIG_SRC      TIMER_SMCFG_TRGSEL_ITI2

// 方向控制: PB7
#define VER_DIR_PORT            GPIOB
#define VER_DIR_PIN             GPIO_PIN_7
#define VER_DIR_CLK             RCU_GPIOB


// --- 类型定义 ---

typedef enum {
    horDm542,
    verDm542
} Dm542Def;

typedef enum {
    unfinished,
    finished
} MoveStatus;

struct ScrewMotorStatus {
    MoveStatus movingCompletionStatus;
    float currentPosition;
};

/* 轨迹点结构体 */
typedef struct {
    int x;
    int y;
} Point2D;

// --- 全局变量声明 ---
extern SemaphoreHandle_t dm542_USART3_Mutex;
extern struct ScrewMotorStatus horSM;
extern struct ScrewMotorStatus verSM;

// --- 函数声明 ---
void hor_dm542_init(uint32_t period, uint16_t prescaler, uint32_t pulse);
void ver_dm542_init(uint32_t period, uint16_t prescaler, uint32_t pulse);

void dm542_dir_config(Dm542Def whichDm542, bit_status bitVal);
void dm542_pul_config(Dm542Def whichDm542, ControlStatus newState);

int motor_position_ctrl(float horPosition, float verPosition);

// 横向从定时器中断 (TIMER1)
void TIMER1_IRQHandler(void);
// 纵向从定时器中断 (TIMER4)
void TIMER4_IRQHandler(void);

#endif /* __BSP_DM542_H */

