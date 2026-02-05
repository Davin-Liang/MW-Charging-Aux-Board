#ifndef __BSP_DM542_H
#define	__BSP_DM542_H

#include "FreeRTOS.h"
#include "semphr.h"

#define DM542_SUBDIVISION   5 // DM542细分数
#define SCREW_LEAD          10.f // 丝杆导程
#define MOTOR_STEP_ANGLE    1.8f // 步距角
#define MOTOR_PRR           ((360.f / MOTOR_STEP_ANGLE) * DM542_SUBDIVISION) // 电机每转所需的脉冲数
#define PULSE_EQUIVALENT    (SCREW_LEAD / MOTOR_PRR) // 脉冲当量
#define MOTOR_ANGULAR_VEL   70.f // 电机角速度
#define MOTOR_RPM           (MOTOR_ANGULAR_VEL / (2 * 3.14)) // 电机转速
#define NEEDED_FPULSE       (MOTOR_PRR * MOTOR_RPM) // 脉冲频率
#define NEEDED_PSC          128
#define NEEDED_CK_CNT       (168000000 / NEEDED_PSC)
#define NEEDED_ARR          (int)(NEEDED_CK_CNT / NEEDED_FPULSE)
#define NEEDED_CCR          (int)(NEEDED_ARR / 2)

#define MAX_DISTANCE_VAL    0.4f // 单位[m]

#define DATA_FLAG           0XCD
#define MOTOR_DATA_LEN         9


enum Dm542Def{
    horDm542, // 横向DM542驱动器
    verDm542, // 纵向DM542驱动器
};

enum MovingCompletionStatus{
    finished,
    unfinished,
};


struct ScrewMotorStatus{
    float currentPosition;
    enum MovingCompletionStatus movingCompletionStatus;
};

/* 轨迹点结构体 */
typedef struct {
    int x;
    int y;
} Point2D;

extern SemaphoreHandle_t dm542_USART3_Mutex;

void hor_dm542_init(uint32_t period, uint16_t prescaler, uint32_t pulse);
void ver_dm542_init(uint32_t period, uint16_t prescaler, uint32_t pulse);
int motor_position_ctrl(float horPosition, float verPosition);
void screw_motor_status_init(void);
void screw_motor_status_reset(void);
int motor_status_write_to_flash(void);
int motor_status_write_from_flash(void);
int generate_circle_trajectory(Point2D *points, int num_points, int radius);
int generate_square_trajectory(Point2D *points, int num_points, int side_length);
void set_motor_speed(enum Dm542Def whichDm542, float motorAngularVel);
void motor_status_add(void);

#endif
