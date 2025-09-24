/**
 ****************************************************************************************************
 * @file        ethernet.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2021-12-02
 * @brief       ETHERNET ��������
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� ̽���� F407������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 * �޸�˵��
 * V1.0 20211202
 * ��һ�η���
 *
 ****************************************************************************************************
 */
 
#ifndef __ETHERNET_H
#define __ETHERNET_H
#include "sys.h"
// #include "stm32f4xx_hal_conf.h"
#include <stm32f4xx_conf.h>
#include "stm32f4xx.h"
#include "stm32f429_eth.h"

/******************************************************************************************/
/* ���� ���� */

// #define ETH_CLK_GPIO_PORT               GPIOA
// #define ETH_CLK_GPIO_PIN                GPIO_PIN_1
// #define ETH_CLK_GPIO_CLK_ENABLE()       do{ __HAL_RCC_GPIOA_CLK_ENABLE();}while(0)                  /* ����IO��ʱ��ʹ�� */

// #define ETH_MDIO_GPIO_PORT              GPIOA
// #define ETH_MDIO_GPIO_PIN               GPIO_PIN_2
// #define ETH_MDIO_GPIO_CLK_ENABLE()       do{ __HAL_RCC_GPIOA_CLK_ENABLE();}while(0)                 /* ����IO��ʱ��ʹ�� */

// #define ETH_CRS_GPIO_PORT               GPIOA
// #define ETH_CRS_GPIO_PIN                GPIO_PIN_7
// #define ETH_CRS_GPIO_CLK_ENABLE()       do{ __HAL_RCC_GPIOA_CLK_ENABLE();}while(0)                  /* ����IO��ʱ��ʹ�� */

// #define ETH_MDC_GPIO_PORT               GPIOC
// #define ETH_MDC_GPIO_PIN                GPIO_PIN_1
// #define ETH_MDC_GPIO_CLK_ENABLE()       do{ __HAL_RCC_GPIOC_CLK_ENABLE();}while(0)                  /* ����IO��ʱ��ʹ�� */

// #define ETH_RXD0_GPIO_PORT              GPIOC
// #define ETH_RXD0_GPIO_PIN               GPIO_PIN_4
// #define ETH_RXD0_GPIO_CLK_ENABLE()      do{ __HAL_RCC_GPIOC_CLK_ENABLE();}while(0)                  /* ����IO��ʱ��ʹ�� */

// #define ETH_RXD1_GPIO_PORT              GPIOC
// #define ETH_RXD1_GPIO_PIN               GPIO_PIN_5
// #define ETH_RXD1_GPIO_CLK_ENABLE()      do{ __HAL_RCC_GPIOC_CLK_ENABLE();}while(0)                  /* ����IO��ʱ��ʹ�� */

// #define ETH_TX_EN_GPIO_PORT             GPIOG
// #define ETH_TX_EN_GPIO_PIN              GPIO_PIN_11
// #define ETH_TX_EN_GPIO_CLK_ENABLE()     do{ __HAL_RCC_GPIOG_CLK_ENABLE();}while(0)                  /* ����IO��ʱ��ʹ�� */

// #define ETH_TXD0_GPIO_PORT              GPIOG
// #define ETH_TXD0_GPIO_PIN               GPIO_PIN_13
// #define ETH_TXD0_GPIO_CLK_ENABLE()      do{ __HAL_RCC_GPIOG_CLK_ENABLE();}while(0)                  /* ����IO��ʱ��ʹ�� */

// #define ETH_TXD1_GPIO_PORT              GPIOG
// #define ETH_TXD1_GPIO_PIN               GPIO_PIN_14
// #define ETH_TXD1_GPIO_CLK_ENABLE()      do{ __HAL_RCC_GPIOG_CLK_ENABLE();}while(0)                  /* ����IO��ʱ��ʹ�� */

// #define ETH_RESET_GPIO_PORT             GPIOD
// #define ETH_RESET_GPIO_PIN              GPIO_PIN_3
// #define ETH_RESET_GPIO_CLK_ENABLE()     do{ __HAL_RCC_GPIOD_CLK_ENABLE();}while(0)                  /* ����IO��ʱ��ʹ�� */

/* ����PHY��ַ���� */
#define ETHERNET_PHY_ADDRESS              0x00

/* ѡ��PHYоƬ */
#define LAN8720                          0
#define SR8201F                          1
#define YT8512C                          2
#define RTL8201                          3
#define PHY_TYPE                         YT8512C


#if(PHY_TYPE == LAN8720) 
#define PHY_SR                           ((uint16_t)0x1F)                       /*!< tranceiver status register */
#define PHY_SPEED_STATUS                 ((uint16_t)0x0004)                     /*!< configured information of speed: 100Mbit/s */
#define PHY_DUPLEX_STATUS                ((uint16_t)0x0010)                     /*!< configured information of duplex: full-duplex */
#elif(PHY_TYPE == SR8201F)
#define PHY_SR                           ((uint16_t)0x00)                       /*!< tranceiver status register */
#define PHY_SPEED_STATUS                 ((uint16_t)0x2020)                     /*!< configured information of speed: 100Mbit/s */
#define PHY_DUPLEX_STATUS                ((uint16_t)0x0100)                     /*!< configured information of duplex: full-duplex */
#elif(PHY_TYPE == YT8512C)
#define PHY_SR                           ((uint16_t)0x11)                       /*!< tranceiver status register */
#define PHY_SPEED_STATUS                 ((uint16_t)0x4010)                     /*!< configured information of speed: 100Mbit/s */
#define PHY_DUPLEX_STATUS                ((uint16_t)0x2000)                     /*!< configured information of duplex: full-duplex */
#elif(PHY_TYPE == RTL8201)
#define PHY_SR                           ((uint16_t)0x10)                       /*!< tranceiver status register */
#define PHY_SPEED_STATUS                 ((uint16_t)0x0022)                     /*!< configured information of speed: 100Mbit/s */
#define PHY_DUPLEX_STATUS                ((uint16_t)0x0004)                     /*!< configured information of duplex: full-duplex */

#endif /* PHY_TYPE */
/*
	ETH_MDIO -------------------------> PA2
	ETH_MDC --------------------------> PC1
	ETH_MII_RX_CLK/ETH_RMII_REF_CLK---> PA1
	ETH_MII_RX_DV/ETH_RMII_CRS_DV ----> PA7
	ETH_MII_RXD0/ETH_RMII_RXD0 -------> PC4
	ETH_MII_RXD1/ETH_RMII_RXD1 -------> PC5
	ETH_MII_TX_EN/ETH_RMII_TX_EN -----> PG11
	ETH_MII_TXD0/ETH_RMII_TXD0 -------> PG13
	ETH_MII_TXD1/ETH_RMII_TXD1 -------> PG14
																						*/
/* ETH_MDIO */
#define ETH_MDIO_GPIO_CLK               RCC_AHB1Periph_GPIOA
#define ETH_MDIO_PORT                   GPIOA
#define ETH_MDIO_PIN                    GPIO_Pin_2
#define ETH_MDIO_AF                     GPIO_AF_ETH
#define ETH_MDIO_SOURCE                 GPIO_PinSource2

/* ETH_MDC */
#define ETH_MDC_GPIO_CLK                RCC_AHB1Periph_GPIOC
#define ETH_MDC_PORT                    GPIOC
#define ETH_MDC_PIN                     GPIO_Pin_1
#define ETH_MDC_AF                      GPIO_AF_ETH
#define ETH_MDC_SOURCE                  GPIO_PinSource1

/* ETH_RMII_REF_CLK */
#define ETH_RMII_REF_CLK_GPIO_CLK       RCC_AHB1Periph_GPIOA
#define ETH_RMII_REF_CLK_PORT           GPIOA
#define ETH_RMII_REF_CLK_PIN            GPIO_Pin_1
#define ETH_RMII_REF_CLK_AF             GPIO_AF_ETH
#define ETH_RMII_REF_CLK_SOURCE         GPIO_PinSource1

/* ETH_RMII_CRS_DV */
#define ETH_RMII_CRS_DV_GPIO_CLK        RCC_AHB1Periph_GPIOA
#define ETH_RMII_CRS_DV_PORT            GPIOA
#define ETH_RMII_CRS_DV_PIN             GPIO_Pin_7
#define ETH_RMII_CRS_DV_AF              GPIO_AF_ETH
#define ETH_RMII_CRS_DV_SOURCE          GPIO_PinSource7

/* ETH_RMII_RXD0 */
#define ETH_RMII_RXD0_GPIO_CLK          RCC_AHB1Periph_GPIOC
#define ETH_RMII_RXD0_PORT              GPIOC
#define ETH_RMII_RXD0_PIN               GPIO_Pin_4
#define ETH_RMII_RXD0_AF                GPIO_AF_ETH
#define ETH_RMII_RXD0_SOURCE            GPIO_PinSource4

/* ETH_RMII_RXD1 */
#define ETH_RMII_RXD1_GPIO_CLK          RCC_AHB1Periph_GPIOC
#define ETH_RMII_RXD1_PORT              GPIOC
#define ETH_RMII_RXD1_PIN               GPIO_Pin_5
#define ETH_RMII_RXD1_AF                GPIO_AF_ETH
#define ETH_RMII_RXD1_SOURCE            GPIO_PinSource5

/* ETH_RMII_TX_EN */
#define ETH_RMII_TX_EN_GPIO_CLK         RCC_AHB1Periph_GPIOG
#define ETH_RMII_TX_EN_PORT             GPIOG
#define ETH_RMII_TX_EN_PIN              GPIO_Pin_11
#define ETH_RMII_TX_EN_AF               GPIO_AF_ETH
#define ETH_RMII_TX_EN_SOURCE           GPIO_PinSource11

/* ETH_RMII_TXD0 */
#define ETH_RMII_TXD0_GPIO_CLK          RCC_AHB1Periph_GPIOG
#define ETH_RMII_TXD0_PORT              GPIOG
#define ETH_RMII_TXD0_PIN               GPIO_Pin_13
#define ETH_RMII_TXD0_AF                GPIO_AF_ETH
#define ETH_RMII_TXD0_SOURCE            GPIO_PinSource13

/* ETH_RMII_TXD1 */
#define ETH_RMII_TXD1_GPIO_CLK          RCC_AHB1Periph_GPIOG
#define ETH_RMII_TXD1_PORT              GPIOG
#define ETH_RMII_TXD1_PIN               GPIO_Pin_14
#define ETH_RMII_TXD1_AF                GPIO_AF_ETH
#define ETH_RMII_TXD1_SOURCE            GPIO_PinSource14

/* ��λ���� */
#define ETH_RESET_GPIO_CLK              RCC_AHB1Periph_GPIOD
#define ETH_RESET_GPIO_PORT             GPIOD
#define ETH_RESET_GPIO_PIN              GPIO_Pin_3

/******************************************************************************************/

/* ETH�˿ڶ��� */
#define ETHERNET_RST(x)  do{ x ? \
                            GPIO_WriteBit(GPIOD, GPIO_Pin_3, Bit_SET) : \
                            GPIO_WriteBit(GPIOD, GPIO_Pin_3, Bit_RESET); \
                        }while(0)



//extern ETH_HandleTypeDef g_eth_handler;                                 /* ��̫����� */
//extern ETH_DMADESCTypeDef *g_eth_dma_rx_dscr_tab;                       /* ��̫��DMA�������������ݽṹ��ָ�� */
//extern ETH_DMADESCTypeDef *g_eth_dma_tx_dscr_tab;                       /* ��̫��DMA�������������ݽṹ��ָ�� */
//extern uint8_t *g_eth_rx_buf;                                           /* ��̫���ײ���������buffersָ�� */
//extern uint8_t *g_eth_tx_buf;                                           /* ��̫���ײ���������buffersָ�� */
//extern ETH_DMADESCTypeDef g_eth_dma_rx_dscr_tab[ETH_RXBUFNB];    //��̫��DMA�������������ݽṹ��ָ��  
//extern ETH_DMADESCTypeDef g_eth_dma_tx_dscr_tab[ETH_TXBUFNB];    //��̫��DMA�������������ݽṹ��ָ��   
//extern uint8_t g_eth_rx_buf[ETH_RX_BUF_SIZE*ETH_RXBUFNB];    //��̫���ײ���������buffersָ��   
//extern uint8_t g_eth_tx_buf[ETH_TX_BUF_SIZE*ETH_TXBUFNB];    //��̫���ײ���������buffersָ�� 

//extern ETH_DMADESCTypeDef  *DMATxDescToSet;            //DMA����������׷��ָ��
//extern ETH_DMADESCTypeDef  *DMARxDescToGet;         //DMA����������׷��ָ�� 
//extern ETH_DMA_Rx_Frame_infos *DMA_RX_FRAME_infos;    //DMA�����յ���֡��Ϣָ��																			

uint8_t ethernet_init(void);                                            /* ��̫��оƬ��ʼ�� */
void ETH_MspInit(void);
uint32_t ethernet_read_phy(uint16_t reg);                               /* ��ȡ��̫��оƬ�Ĵ���ֵ */
void ethernet_write_phy(uint16_t reg, uint16_t value);                  /* ����̫��оƬָ����ַд��Ĵ���ֵ */
uint8_t ethernet_chip_get_speed(void);                                  /* �����̫��оƬ���ٶ�ģʽ */
uint32_t  ethernet_get_eth_rx_size(ETH_DMADESCTypeDef *dma_rx_desc);    /* ��ȡ���յ���֡���� */

FrameTypeDef ETH_Rx_Packet(void);
uint8_t ETH_Tx_Packet(uint16_t FrameLength);
uint32_t ETH_GetCurrentTxBuffer(void);

//uint8_t ethernet_mem_malloc(void);                                           /* ΪETH�ײ����������ڴ� */
//void ethernet_mem_free(void);                                                /* �ͷ�ETH �ײ�����������ڴ� */
#endif

