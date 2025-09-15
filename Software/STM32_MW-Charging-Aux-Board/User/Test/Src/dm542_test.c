#include "bsp_dm542.h"
#include "dm542_test.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stm32f4xx_conf.h>
#include "stm32f4xx.h"
#include "bsp_led.h"

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
		motor_position_ctrl(0, 100);
//    motor_position_ctrl(8, 8);
    LED1_ON;
    vTaskDelay(2000);

//		motor_position_ctrl(0, 8);
    motor_position_ctrl(0, -100);
    LED1_OFF;
    vTaskDelay(2000);	
}
