#include "bsp_dm542.h"
#include "dm542_test.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stm32f4xx_conf.h>
#include "stm32f4xx.h"

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
    motor_distancce_ctrl(1000);
    vTaskDelay(15000);

    motor_distancce_ctrl(-1000);
    vTaskDelay(15000);
}
