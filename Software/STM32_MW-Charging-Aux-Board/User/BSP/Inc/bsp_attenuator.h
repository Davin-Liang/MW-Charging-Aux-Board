#ifndef __BSP_ATTENUATOR_H
#define	__BSP_ATTENUATOR_H

#include "stm32f4xx.h"

#define ATT_GPIO_CLK RCC_AHB1Periph_GPIOE
#define ATT_GPIO_PORT GPIOE
#define ATT1_GPIO_PIN GPIO_Pin_0
#define ATT2_GPIO_PIN GPIO_Pin_1
#define ATT3_GPIO_PIN GPIO_Pin_2
#define ATT4_GPIO_PIN GPIO_Pin_3
#define ATT5_GPIO_PIN GPIO_Pin_4
#define ATT6_GPIO_PIN GPIO_Pin_5

enum AttenuatorIndex_t {
    attenuator1,
    attenuator2,
    attenuator3,
    attenuator4,
    attenuator5,
    attenuator6,
};

void attenuators_init(void);
void attenuator_switch_config(enum AttenuatorIndex_t attenuatorIndex, FunctionalState NewState);

#endif
