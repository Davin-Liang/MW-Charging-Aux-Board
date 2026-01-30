#include "bsp_dm542.h"
#include "dm542_test.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stm32f4xx_conf.h>
#include "stm32f4xx.h"
#include "bsp_led.h"
#include "bsp_debug_usart.h"

void test1(void)
{
    dm542_pul_config(horDm542, ENABLE);
//		vTaskSuspend(NULL);;
//    dm542_dir_config(horDm542, Bit_SET);
//    dm542_ena_config(horDm542, Bit_SET);

    vTaskDelay(5000);

//    // dm542_dir_config(horDm542, Bit_RESET);

////    vTaskDelay(5000);

//    dm542_pul_config(horDm542, DISABLE);
//////    dm542_dir_config(horDm542, Bit_RESET);
//////    dm542_ena_config(horDm542, Bit_SET); 

//		vTaskDelay(5000);	
}

void dm542_test2(void)
{
//    motor_distancce_ctrl(100);
//    vTaskDelay(5000);

//    motor_distancce_ctrl(-100);
//    vTaskDelay(5000);
		vTaskDelay(1000);
		motor_status_write_from_flash();
		motor_position_ctrl(5, 0);
//    motor_position_ctrl(8, 8);
//    LED1_ON;
    vTaskDelay(1000);

//		motor_position_ctrl(0, 8);
	motor_status_write_from_flash();
    motor_position_ctrl(-5, 0);
//    LED1_OFF;	
}

void dm542_test3(void)
{
    motor_status_write_to_flash();
    LED1_ON;
    vTaskDelay(2000);
    motor_status_write_from_flash();
    LED1_OFF;
    motor_status_add();

    vTaskDelay(2000);
}

void dm542_test4(void)
{
    vTaskDelay(2000);
    motor_status_write_from_flash();
    motor_position_ctrl(200, 200);

    vTaskDelay(2000);
	motor_status_write_from_flash();
    motor_position_ctrl(0, 0);
}

void dm542_test5(void)
{
    vTaskDelay(5000);

    static Point2D trajectory[25];
    static int test = 1;

    if (test)
    {
        generate_circle_trajectory(trajectory, 25, 200);

        for (int i = 0; i < 25; i ++)
        {
            motor_position_ctrl((float)trajectory[i].x, (float)trajectory[i].y);
            mutual_printf("trajectory[%d].x = %d,trajectory[%d].y = %d", i, trajectory[i].x, i, trajectory[i].y);
            mutual_printf("circle trajectory[%d] arrived!!!", i);
					 
            vTaskDelay(500);
        }

        test = 0;
    }

    vTaskDelay(10);

// static Point2D trajectory[POINT_NUM];
// generate_circle_trajectory(trajectory, POINT_NUM, 200); // 圆形轨迹
// // generate_square_trajectory(trajectory, POINT_NUM, 400); // 正方形轨迹
// for (int i = 0; i < POINT_NUM; i ++)
// {
//     motor_position_ctrl((float)trajectory[i].x, (float)trajectory[i].y);       
//     vTaskDelay(POINT_INTERVAL_TIME);
// }

}

/*  */

