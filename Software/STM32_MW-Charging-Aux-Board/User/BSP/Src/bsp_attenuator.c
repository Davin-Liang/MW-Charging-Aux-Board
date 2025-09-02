#include "bsp_attenuator.h"
#include <stm32f4xx_conf.h>
#include "stm32f4xx.h"

static void attenuator_gpio_init(uint32_t RCC_AHB1Periph, GPIO_TypeDef* gpioPort, uint32_t gpioPin);

/**
  * @brief  �����е�˥����GPIO���г�ʼ��
  * @param  void
  * @return void
  * @note   PE0-PE5
  **/
void attenuators_init(void)
{
    attenuator_gpio_init(RCC_AHB1Periph_GPIOE, ATT_GPIO_PORT, ATT1_GPIO_PIN);
    attenuator_gpio_init(RCC_AHB1Periph_GPIOE, ATT_GPIO_PORT, ATT2_GPIO_PIN);
    attenuator_gpio_init(RCC_AHB1Periph_GPIOE, ATT_GPIO_PORT, ATT3_GPIO_PIN);
    attenuator_gpio_init(RCC_AHB1Periph_GPIOE, ATT_GPIO_PORT, ATT4_GPIO_PIN);
    attenuator_gpio_init(RCC_AHB1Periph_GPIOE, ATT_GPIO_PORT, ATT5_GPIO_PIN);
    attenuator_gpio_init(RCC_AHB1Periph_GPIOE, ATT_GPIO_PORT, ATT6_GPIO_PIN);
}

/**
  * @brief  ˥������ʼ����
  * @param  AttenuatorIndex_t ��һ��˥����
  * @param  NewState ˥����״̬ENABLE/DISABLE
  * @return void
  **/
void attenuator_switch_config(enum AttenuatorIndex_t attenuatorIndex, FunctionalState NewState)
{
    switch((int)attenuatorIndex)
    {
        case attenuator1:
            if (NewState)
                GPIO_WriteBit(ATT_GPIO_PORT, ATT1_GPIO_PIN, Bit_SET);
            else
                GPIO_WriteBit(ATT_GPIO_PORT, ATT1_GPIO_PIN, Bit_RESET);
            break;
        case attenuator2:
            if (NewState)
                GPIO_WriteBit(ATT_GPIO_PORT, ATT2_GPIO_PIN, Bit_SET);
            else
                GPIO_WriteBit(ATT_GPIO_PORT, ATT2_GPIO_PIN, Bit_RESET);
            break;
        case attenuator3:
            if (NewState)
                GPIO_WriteBit(ATT_GPIO_PORT, ATT3_GPIO_PIN, Bit_SET);
            else
                GPIO_WriteBit(ATT_GPIO_PORT, ATT3_GPIO_PIN, Bit_RESET);
            break;
        case attenuator4:
            if (NewState)
                GPIO_WriteBit(ATT_GPIO_PORT, ATT4_GPIO_PIN, Bit_SET);
            else
                GPIO_WriteBit(ATT_GPIO_PORT, ATT4_GPIO_PIN, Bit_RESET);
            break;
        case attenuator5:
            if (NewState)
                GPIO_WriteBit(ATT_GPIO_PORT, ATT5_GPIO_PIN, Bit_SET);
            else
                GPIO_WriteBit(ATT_GPIO_PORT, ATT5_GPIO_PIN, Bit_RESET);
            break;
        case attenuator6:
            if (NewState)
                GPIO_WriteBit(ATT_GPIO_PORT, ATT6_GPIO_PIN, Bit_SET);
            else
                GPIO_WriteBit(ATT_GPIO_PORT, ATT6_GPIO_PIN, Bit_RESET);
            break;
    }
    
}

/**
  * @brief  ˥����GPIO��ʼ��
  * @param  RCC_AHB1Periph 
  * @param  gpioPort GPIO�˿�
  * @param  gpioPin GPIO����
  * @return void
  * @note   PE0-PE5
  **/
static void attenuator_gpio_init(uint32_t RCC_AHB1Periph, GPIO_TypeDef* gpioPort, uint32_t gpioPin)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	
	/* GPIO ʱ��ʹ�� */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph, ENABLE);
  
    /* GPIO��ʼ�� */
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Pin = gpioPin;  
    GPIO_Init(gpioPort, &GPIO_InitStructure);
}
