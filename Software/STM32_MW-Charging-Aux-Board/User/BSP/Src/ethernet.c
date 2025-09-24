/**
 ****************************************************************************************************
 * @file        ethernet.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2021-10-14
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
 * V1.0 20211014
 * ��һ�η���
 *
 ****************************************************************************************************
 */

#include "ethernet.h"
//#include "lwip_comm.h"
#include "stm32f429_eth_conf.h"
#include "stm32f429_eth.h"
//#include "delay.h"
#include "malloc.h"


//ETH_HandleTypeDef g_eth_handler;            /* ��̫����� */
//ETH_DMADESCTypeDef *g_eth_dma_rx_dscr_tab;  /* ��̫��DMA�������������ݽṹ��ָ�� */
//ETH_DMADESCTypeDef *g_eth_dma_tx_dscr_tab;  /* ��̫��DMA�������������ݽṹ��ָ�� */
//uint8_t *g_eth_rx_buf;                      /* ��̫���ײ���������buffersָ�� */
//uint8_t *g_eth_tx_buf;                      /* ��̫���ײ���������buffersָ�� */


//ETH_DMADESCTypeDef g_eth_dma_rx_dscr_tab[ETH_RXBUFNB];    //��̫��DMA�������������ݽṹ��ָ��  
//ETH_DMADESCTypeDef g_eth_dma_tx_dscr_tab[ETH_TXBUFNB];    //��̫��DMA�������������ݽṹ��ָ��   
//uint8_t g_eth_rx_buf[ETH_RX_BUF_SIZE*ETH_RXBUFNB];    //��̫���ײ���������buffersָ��   
//uint8_t g_eth_tx_buf[ETH_TX_BUF_SIZE*ETH_TXBUFNB];    //��̫���ײ���������buffersָ�� 

//ETH_DMADESCTypeDef  *DMATxDescToSet;            //DMA����������׷��ָ��
//ETH_DMADESCTypeDef  *DMARxDescToGet;         //DMA����������׷��ָ�� 
//ETH_DMA_Rx_Frame_infos *DMA_RX_FRAME_infos;    //DMA�����յ���֡��Ϣָ��


ETH_InitTypeDef ETH_InitStructure;

/**
 * @brief       ��̫��оƬ��ʼ��
 * @param       ��
 * @retval      0,�ɹ�
 *              1,ʧ��
 */
uint8_t ethernet_init(void)
{
    ETH_MspInit();
	
		ETH_StructInit(&ETH_InitStructure);

    /* Fill ETH_InitStructure parametrs */
    /*------------------------   MAC   -----------------------------------*/
        /* ������������Ӧ���� */
    ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable;
    //  ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Disable; 
    //  ETH_InitStructure.ETH_Speed = ETH_Speed_10M;
    //  ETH_InitStructure.ETH_Mode = ETH_Mode_FullDuplex;   
    /* �رշ��� */
    ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;
        /* �ر��ش����� */
    ETH_InitStructure.ETH_RetryTransmission = ETH_RetryTransmission_Disable;
        /* �ر��Զ�ȥ��PDA/CRC����  */
    ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable;
        /* �رս������е�֡ */
    ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Disable;
        /* ����������й㲥֡ */
    ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable;
        /* �رջ��ģʽ�ĵ�ַ����  */
    ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;
        /* �����鲥��ַʹ��������ַ����    */
    ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;
        /* �Ե�����ַʹ��������ַ����  */
    ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;
    #ifdef CHECKSUM_BY_HARDWARE
        /* ����ipv4��TCP/UDP/ICMP��֡У���ж��   */
    ETH_InitStructure.ETH_ChecksumOffload = ETH_ChecksumOffload_Enable;
    #endif

    /*------------------------   DMA   -----------------------------------*/  

    /* When we use the Checksum offload feature, we need to enable the Store and Forward mode:
    the store and forward guarantee that a whole frame is stored in the FIFO, so the MAC can insert/verify the checksum, 
    if the checksum is OK the DMA can handle the frame otherwise the frame is dropped */
        /*������ʹ��֡У���ж�ع��ܵ�ʱ��һ��Ҫʹ�ܴ洢ת��ģʽ,�洢ת��ģʽ��Ҫ��֤����֡�洢��FIFO��,
        ����MAC�ܲ���/ʶ���֡У��ֵ,����У����ȷ��ʱ��DMA�Ϳ��Դ���֡,����Ͷ�������֡*/
        
        /* ��������TCP/IP����֡ */
    ETH_InitStructure.ETH_DropTCPIPChecksumErrorFrame = ETH_DropTCPIPChecksumErrorFrame_Enable;
        /* �����������ݵĴ洢ת��ģʽ  */
    ETH_InitStructure.ETH_ReceiveStoreForward = ETH_ReceiveStoreForward_Enable;
        /* �����������ݵĴ洢ת��ģʽ   */
    ETH_InitStructure.ETH_TransmitStoreForward = ETH_TransmitStoreForward_Enable;

        /* ��ֹת������֡ */
    ETH_InitStructure.ETH_ForwardErrorFrames = ETH_ForwardErrorFrames_Disable;
        /* ��ת����С�ĺ�֡ */
    ETH_InitStructure.ETH_ForwardUndersizedGoodFrames = ETH_ForwardUndersizedGoodFrames_Disable;
        /* �򿪴���ڶ�֡���� */
    ETH_InitStructure.ETH_SecondFrameOperate = ETH_SecondFrameOperate_Enable;
        /* ����DMA����ĵ�ַ���빦�� */
    ETH_InitStructure.ETH_AddressAlignedBeats = ETH_AddressAlignedBeats_Enable;
        /* �����̶�ͻ������ */
    ETH_InitStructure.ETH_FixedBurst = ETH_FixedBurst_Enable;
        /* DMA���͵����ͻ������Ϊ32������ */
    ETH_InitStructure.ETH_RxDMABurstLength = ETH_RxDMABurstLength_32Beat;
        /*DMA���յ����ͻ������Ϊ32������ */
    ETH_InitStructure.ETH_TxDMABurstLength = ETH_TxDMABurstLength_32Beat;
    ETH_InitStructure.ETH_DMAArbitration = ETH_DMAArbitration_RoundRobin_RxTx_2_1;

    /* Configure Ethernet */
	/* ����ETH */
		uint32_t EthStatus = ETH_ERROR;
    EthStatus = ETH_Init(&ETH_InitStructure, ETHERNET_PHY_ADDRESS);
    if (EthStatus == ETH_SUCCESS)
    {
        return 0;   /* �ɹ� */
    }
    else
    {
        return 1;  /* ʧ�� */
    }
    // uint8_t macaddress[6];

    // macaddress[0] = g_lwipdev.mac[0];
    // macaddress[1] = g_lwipdev.mac[1];
    // macaddress[2] = g_lwipdev.mac[2];
    // macaddress[3] = g_lwipdev.mac[3];
    // macaddress[4] = g_lwipdev.mac[4];
    // macaddress[5] = g_lwipdev.mac[5];

    // g_eth_handler.Instance = ETH;
    // g_eth_handler.Init.AutoNegotiation = ETH_AUTONEGOTIATION_ENABLE;    /* ʹ����Э��ģʽ */
    // g_eth_handler.Init.Speed = ETH_SPEED_100M;                          /* �ٶ�100M,�����������Э��ģʽ�������þ���Ч */
    // g_eth_handler.Init.DuplexMode = ETH_MODE_FULLDUPLEX;                /* ȫ˫��ģʽ�������������Э��ģʽ�������þ���Ч */
    // g_eth_handler.Init.PhyAddress = ETHERNET_PHY_ADDRESS;               /* ��̫��оƬ�ĵ�ַ */
    // g_eth_handler.Init.MACAddr = macaddress;                            /* MAC��ַ */
    // g_eth_handler.Init.RxMode = ETH_RXINTERRUPT_MODE;                   /* �жϽ���ģʽ */
    // g_eth_handler.Init.ChecksumMode = ETH_CHECKSUM_BY_HARDWARE;         /* Ӳ��֡У�� */
    // g_eth_handler.Init.MediaInterface = ETH_MEDIA_INTERFACE_RMII;       /* RMII�ӿ� */
}

/**
 * @brief       ETH�ײ�������ʱ��ʹ�ܣ���������
 *    @note     �˺����ᱻHAL_ETH_Init()����
 * @param       heth:��̫�����
 * @retval      ��
 */
void ETH_MspInit(void)
{
    // GPIO_InitTypeDef gpio_init_struct;

    // ETH_CLK_GPIO_CLK_ENABLE();          /* ����ETH_CLKʱ�� */
    // ETH_MDIO_GPIO_CLK_ENABLE();         /* ����ETH_MDIOʱ�� */
    // ETH_CRS_GPIO_CLK_ENABLE();          /* ����ETH_CRSʱ�� */
    // ETH_MDC_GPIO_CLK_ENABLE();          /* ����ETH_MDCʱ�� */
    // ETH_RXD0_GPIO_CLK_ENABLE();         /* ����ETH_RXD0ʱ�� */
    // ETH_RXD1_GPIO_CLK_ENABLE();         /* ����ETH_RXD1ʱ�� */
    // ETH_TX_EN_GPIO_CLK_ENABLE();        /* ����ETH_TX_ENʱ�� */
    // ETH_TXD0_GPIO_CLK_ENABLE();         /* ����ETH_TXD0ʱ�� */
    // ETH_TXD1_GPIO_CLK_ENABLE();         /* ����ETH_TXD1ʱ�� */
    // ETH_RESET_GPIO_CLK_ENABLE();        /* ����ETH_RESETʱ�� */
    // __HAL_RCC_ETH_CLK_ENABLE();         /* ����ETHʱ�� */


    // /* ������������ RMII�ӿ�
    //  * ETH_MDIO -------------------------> PA2
    //  * ETH_MDC --------------------------> PC1
    //  * ETH_RMII_REF_CLK------------------> PA1
    //  * ETH_RMII_CRS_DV ------------------> PA7
    //  * ETH_RMII_RXD0 --------------------> PC4
    //  * ETH_RMII_RXD1 --------------------> PC5
    //  * ETH_RMII_TX_EN -------------------> PG11
    //  * ETH_RMII_TXD0 --------------------> PG13
    //  * ETH_RMII_TXD1 --------------------> PG14
    //  * ETH_RESET-------------------------> PD3
    //  */

    // /* PA1,2,7 */
    // gpio_init_struct.Pin = ETH_CLK_GPIO_PIN;
    // gpio_init_struct.Mode = GPIO_MODE_AF_PP;                /* ���츴�� */
    // gpio_init_struct.Pull = GPIO_NOPULL;                    /* ���������� */
    // gpio_init_struct.Speed = GPIO_SPEED_HIGH;               /* ���� */
    // gpio_init_struct.Alternate = GPIO_AF11_ETH;             /* ����ΪETH���� */
    // HAL_GPIO_Init(ETH_CLK_GPIO_PORT, &gpio_init_struct);    /* ETH_CLK����ģʽ���� */
    
    // gpio_init_struct.Pin = ETH_MDIO_GPIO_PIN;
    // HAL_GPIO_Init(ETH_MDIO_GPIO_PORT, &gpio_init_struct);   /* ETH_MDIO����ģʽ���� */
    
    // gpio_init_struct.Pin = ETH_CRS_GPIO_PIN;    
    // HAL_GPIO_Init(ETH_CRS_GPIO_PORT, &gpio_init_struct);    /* ETH_CRS����ģʽ���� */

    // /* PC1 */
    // gpio_init_struct.Pin = ETH_MDC_GPIO_PIN;
    // HAL_GPIO_Init(ETH_MDC_GPIO_PORT, &gpio_init_struct);    /* ETH_MDC��ʼ�� */

    // /* PC4 */
    // gpio_init_struct.Pin = ETH_RXD0_GPIO_PIN;
    // HAL_GPIO_Init(ETH_RXD0_GPIO_PORT, &gpio_init_struct);   /* ETH_RXD0��ʼ�� */
    
    // /* PC5 */
    // gpio_init_struct.Pin = ETH_RXD1_GPIO_PIN;
    // HAL_GPIO_Init(ETH_RXD1_GPIO_PORT, &gpio_init_struct);   /* ETH_RXD1��ʼ�� */
    
    
    // /* PG11,13,14 */
    // gpio_init_struct.Pin = ETH_TX_EN_GPIO_PIN; 
    // HAL_GPIO_Init(ETH_TX_EN_GPIO_PORT, &gpio_init_struct);  /* ETH_TX_EN��ʼ�� */

    // gpio_init_struct.Pin = ETH_TXD0_GPIO_PIN; 
    // HAL_GPIO_Init(ETH_TXD0_GPIO_PORT, &gpio_init_struct);   /* ETH_TXD0��ʼ�� */
    
    // gpio_init_struct.Pin = ETH_TXD1_GPIO_PIN; 
    // HAL_GPIO_Init(ETH_TXD1_GPIO_PORT, &gpio_init_struct);   /* ETH_TXD1��ʼ�� */
    
    
    // /* ��λ���� */
    // gpio_init_struct.Pin = ETH_RESET_GPIO_PIN;      /* ETH_RESET��ʼ�� */
    // gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;    /* ������� */
    // gpio_init_struct.Pull = GPIO_NOPULL;            /* �������� */
    // gpio_init_struct.Speed = GPIO_SPEED_HIGH;       /* ���� */
    // HAL_GPIO_Init(ETH_RESET_GPIO_PORT, &gpio_init_struct);

    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable GPIOs clocks */
    RCC_AHB1PeriphClockCmd(ETH_MDIO_GPIO_CLK          | 
                            ETH_MDC_GPIO_CLK          |
                            ETH_RMII_REF_CLK_GPIO_CLK | 
                            ETH_RMII_CRS_DV_GPIO_CLK  |
                            ETH_RMII_RXD0_GPIO_CLK    | 
                            ETH_RMII_RXD1_GPIO_CLK    |
                            ETH_RMII_TX_EN_GPIO_CLK   | 
                            ETH_RMII_TXD0_GPIO_CLK    |
                            ETH_RMII_TXD1_GPIO_CLK |
                            ETH_RESET_GPIO_CLK, ENABLE);

    /* Enable ETHERNET clock  */
    // ʹ�� ETH ʱ��
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_ETH_MAC | 
                            RCC_AHB1Periph_ETH_MAC_Tx |
                            RCC_AHB1Periph_ETH_MAC_Rx, ENABLE);

    /* Enable SYSCFG clock */
    // ʹ����ӳ�����Ź��ܣ���Ҫʹ���ⲿ��ʱ�ӣ���ʵʹ��ʹ�ܶ�����ν
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

		SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_RMII); // //MAC��PHY֮��ʹ��RMII�ӿ�

//    /* MII/RMII Media interface selection --------------------------------------*/
//    #ifdef MII_MODE /* Mode MII with STM324xG-EVAL  */
//    #ifdef PHY_CLOCK_MCO

//    /* Output HSE clock (25MHz) on MCO pin (PA8) to clock the PHY */
//    RCC_MCO1Config(RCC_MCO1Source_HSE, RCC_MCO1Div_1);
//    #endif /* PHY_CLOCK_MCO */

//    SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_MII);
//    #elif defined RMII_MODE  /* Mode RMII with STM324xG-EVAL */

//    SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_RMII); // //MAC��PHY֮��ʹ��RMII�ӿ�
//    #endif



    /* Ethernet pins configuration ************************************************/
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
                    ETH_NRST -------------------------> PI1
                                                    */

    /* Configure ETH_MDIO */
    GPIO_InitStructure.GPIO_Pin = ETH_MDIO_PIN;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(ETH_MDIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ETH_MDIO_PORT, ETH_MDIO_SOURCE, ETH_MDIO_AF);
        
        /* Configure ETH_MDC */
    GPIO_InitStructure.GPIO_Pin = ETH_MDC_PIN;
    GPIO_Init(ETH_MDC_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ETH_MDC_PORT, ETH_MDC_SOURCE, ETH_MDC_AF);
        
        /* Configure ETH_RMII_REF_CLK */
    GPIO_InitStructure.GPIO_Pin = ETH_RMII_REF_CLK_PIN;
    GPIO_Init(ETH_RMII_REF_CLK_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ETH_RMII_REF_CLK_PORT, ETH_RMII_REF_CLK_SOURCE, ETH_RMII_REF_CLK_AF);
        
        /* Configure ETH_RMII_CRS_DV */
    GPIO_InitStructure.GPIO_Pin = ETH_RMII_CRS_DV_PIN;
    GPIO_Init(ETH_RMII_CRS_DV_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ETH_RMII_CRS_DV_PORT, ETH_RMII_CRS_DV_SOURCE, ETH_RMII_CRS_DV_AF);
        
        /* Configure ETH_RMII_RXD0 */
    GPIO_InitStructure.GPIO_Pin = ETH_RMII_RXD0_PIN;
    GPIO_Init(ETH_RMII_RXD0_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ETH_RMII_RXD0_PORT, ETH_RMII_RXD0_SOURCE, ETH_RMII_RXD0_AF);
        
        /* Configure ETH_RMII_RXD1 */
    GPIO_InitStructure.GPIO_Pin = ETH_RMII_RXD1_PIN;
    GPIO_Init(ETH_RMII_RXD1_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ETH_RMII_RXD1_PORT, ETH_RMII_RXD1_SOURCE, ETH_RMII_RXD1_AF);
        
        /* Configure ETH_RMII_TX_EN */
    GPIO_InitStructure.GPIO_Pin = ETH_RMII_TX_EN_PIN;
    GPIO_Init(ETH_RMII_TX_EN_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ETH_RMII_TX_EN_PORT, ETH_RMII_TX_EN_SOURCE, ETH_RMII_TX_EN_AF);
        
        /* Configure ETH_RMII_TXD0 */
    GPIO_InitStructure.GPIO_Pin = ETH_RMII_TXD0_PIN;
    GPIO_Init(ETH_RMII_TXD0_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ETH_RMII_TXD0_PORT, ETH_RMII_TXD0_SOURCE, ETH_RMII_TXD0_AF);
        
        /* Configure ETH_RMII_TXD1 */
    GPIO_InitStructure.GPIO_Pin = ETH_RMII_TXD1_PIN;
    GPIO_Init(ETH_RMII_TXD1_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ETH_RMII_TXD1_PORT, ETH_RMII_TXD1_SOURCE, ETH_RMII_TXD1_AF);		

    /* �������� */
    GPIO_InitStructure.GPIO_Pin = ETH_RESET_GPIO_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(ETH_RESET_GPIO_PORT, &GPIO_InitStructure);

    ETHERNET_RST(0);     /* Ӳ����λ */
    // delay_ms(50); TODO:�滻��ʱ����
    ETHERNET_RST(1);     /* ��λ���� */

//    HAL_NVIC_SetPriority(ETH_IRQn, 6, 0); /* �����ж����ȼ�Ӧ�ø�һ�� */
//    HAL_NVIC_EnableIRQ(ETH_IRQn);

		// �����ж����ȼ�
		NVIC_InitTypeDef NVIC_InitStructure;
		NVIC_InitStructure.NVIC_IRQChannel = ETH_IRQn;           // ��̫���ж�
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; // ��ռ���ȼ�
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        // �����ȼ�
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // ʹ���ж�
		NVIC_Init(&NVIC_InitStructure);

}

/**
 * @breif       ��ȡ��̫��оƬ�Ĵ���ֵ
 * @param       reg����ȡ�ļĴ�����ַ
 * @retval      ��
 */
uint32_t ethernet_read_phy(uint16_t reg)
{
    uint32_t regval;

    regval = ETH_ReadPHYRegister(ETHERNET_PHY_ADDRESS, reg);
    // HAL_ETH_ReadPHYRegister(&g_eth_handler, reg, &regval);

    return regval;
}

/**
 * @breif       ����̫��оƬָ����ַд��Ĵ���ֵ
 * @param       reg   : Ҫд��ļĴ���
 * @param       value : Ҫд��ļĴ���
 * @retval      ��
 */
void ethernet_write_phy(uint16_t reg, uint16_t value)
{
    uint32_t temp = value;
    
    ETH_WritePHYRegister(ETHERNET_PHY_ADDRESS, reg, value);
    // HAL_ETH_WritePHYRegister(&g_eth_handler, reg, temp);
}

/**
 * @breif       �������оƬ���ٶ�ģʽ
 * @param       ��
 * @retval      1:��ȡ100M�ɹ�
                0:ʧ��
 */
uint8_t ethernet_chip_get_speed(void)
{
    uint8_t speed;
    #if(PHY_TYPE == LAN8720) 
    speed = ~((ethernet_read_phy(PHY_SR) & PHY_SPEED_STATUS));     /* ��LAN8720��31�żĴ����ж�ȡ�����ٶȺ�˫��ģʽ */
    #elif(PHY_TYPE == SR8201F)
    speed = ((ethernet_read_phy(PHY_SR) & PHY_SPEED_STATUS) >> 13);    /* ��SR8201F��0�żĴ����ж�ȡ�����ٶȺ�˫��ģʽ */
    #elif(PHY_TYPE == YT8512C)
    speed = ((ethernet_read_phy(PHY_SR) & PHY_SPEED_STATUS) >> 14);    /* ��YT8512C��17�żĴ����ж�ȡ�����ٶȺ�˫��ģʽ */
    #elif(PHY_TYPE == RTL8201)
    speed = ((ethernet_read_phy(PHY_SR) & PHY_SPEED_STATUS) >> 1);     /* ��RTL8201��16�żĴ����ж�ȡ�����ٶȺ�˫��ģʽ */
    #endif

    return speed;
}

//extern void lwip_pkt_handle(void); /* ��lwip_comm.c���涨�� */

/**
 * @breif       �жϷ�����
 * @param       ��
 * @retval      ��
 */
//void ETH_IRQHandler(void)
//{
//    if (ethernet_get_eth_rx_size(g_eth_handler.RxDesc))
//    {
////        lwip_pkt_handle();      /* ������̫�����ݣ����������ύ��LWIP */
//    }

//    __HAL_ETH_DMA_CLEAR_IT(&g_eth_handler, ETH_DMA_IT_NIS);   /* ���DMA�жϱ�־λ */
//    __HAL_ETH_DMA_CLEAR_IT(&g_eth_handler, ETH_DMA_IT_R);     /* ���DMA�����жϱ�־λ */
//}

void ETH_IRQHandler(void)
{
    /* ��鲢��������жϱ�־ */
    if (ETH_GetDMAFlagStatus(ETH_DMA_FLAG_NIS) != RESET)
    {
        /* �������ж� */
        if (ETH_GetDMAFlagStatus(ETH_DMA_FLAG_R) != RESET)
        {
            if (ethernet_get_eth_rx_size(DMARxDescToGet))
            {
//                lwip_pkt_handle();      /* ������̫�����ݣ����������ύ��LWIP */
            }
            ETH_DMAClearITPendingBit(ETH_DMA_IT_R);
        }
        
        /* ��������ж��ܽ��־ */
        ETH_DMAClearITPendingBit(ETH_DMA_IT_NIS);
    }
}


/**
 * @breif       ��ȡ���յ���֡����
 * @param       dma_rx_desc : ����DMA������
 * @retval      frameLength : ���յ���֡����
 */
uint32_t  ethernet_get_eth_rx_size(ETH_DMADESCTypeDef *dma_rx_desc)
{
//    uint32_t frameLength = 0;

//    if (((dma_rx_desc->Status & ETH_DMATxDesc_OWN) == (uint32_t)RESET) &&
//        ((dma_rx_desc->Status & ETH_DMATxDesc_ES)  == (uint32_t)RESET) &&
//        ((dma_rx_desc->Status & ETH_DMATxDesc_LS)  != (uint32_t)RESET))
//    {
//        frameLength = ((dma_rx_desc->Status & ETH_DMARxDesc_FL) >> ETH_DMARXDESC_FRAME_LENGTHSHIFT);
//    }

    return ETH_GetRxPktSize(dma_rx_desc);
}

//����һ���������ݰ�
//����ֵ:�������ݰ�֡�ṹ��
FrameTypeDef ETH_Rx_Packet(void)
{ 
    uint32_t framelength = 0;
    FrameTypeDef frame = {0,0};   
		
    /* ��鵱ǰ������,�Ƿ�����ETHERNET DMA(���õ�ʱ��)/CPU(��λ��ʱ��) */
    if ((DMARxDescToGet->Status & ETH_DMATxDesc_OWN) != (uint32_t)RESET)
    {    
        frame.length = ETH_ERROR; 
			
				/* �����ջ�����������״̬ */
        if ((ETH->DMASR & ETH_DMASR_RBUS) != (uint32_t)RESET)  
        { 
            ETH->DMASR = ETH_DMASR_RBUS;//���ETH DMA��RBUSλ 
            ETH->DMARPDR = 0;//�ָ�DMA����
        }
        return frame;//����,OWNλ��������
    }  
		
    if (((DMARxDescToGet->Status & ETH_DMARxDesc_ES) == (uint32_t)RESET) && 
    ((DMARxDescToGet->Status & ETH_DMARxDesc_LS) != (uint32_t)RESET) &&  
    ((DMARxDescToGet->Status & ETH_DMARxDesc_FS) != (uint32_t)RESET))  
    {       
        framelength = ((DMARxDescToGet->Status & ETH_DMARxDesc_FL) >> ETH_DMARxDesc_FrameLengthShift) - 4; // �õ����հ�֡����(������4�ֽ�CRC)
        frame.buffer = DMARxDescToGet->Buffer1Addr; // �õ����������ڵ�λ��
    }
		else 
				framelength = ETH_ERROR; // ����  
    frame.length = framelength; 
    frame.descriptor = DMARxDescToGet; 
		
    // ����ETH DMAȫ��Rx������Ϊ��һ��Rx������
    // Ϊ��һ��buffer��ȡ������һ��DMA Rx������
    DMARxDescToGet = (ETH_DMADESCTypeDef*)(DMARxDescToGet->Buffer2NextDescAddr);   
    return frame;  
}

// ����һ���������ݰ�
// FrameLength:���ݰ�����
// ����ֵ:ETH_ERROR,����ʧ��(0)
//        ETH_SUCCESS,���ͳɹ�(1)
uint8_t ETH_Tx_Packet(uint16_t FrameLength)
{   
    // ��鵱ǰ�������Ƿ�����CPU��OWNλΪ0��ʾ����CPU��
    if ((DMATxDescToSet->Status & ETH_DMATxDesc_OWN) != (uint32_t)RESET)
        return ETH_ERROR;  // ����OWNλ�������ˣ�����DMA��
    
    // ����֡���ȵ����ƻ�������С�ֶε�TBS1λ��
    DMATxDescToSet->ControlBufferSize = FrameLength & ETH_DMATxDesc_TBS1;
    
    // �������һ���͵�һ����λ��1������������һ֡��
    DMATxDescToSet->Status |= ETH_DMATxDesc_LS | ETH_DMATxDesc_FS;
    
    // ����Tx��������OWNλ��buffer�ع�ETH DMA
    DMATxDescToSet->Status |= ETH_DMATxDesc_OWN;
    
    // ��Tx Buffer������λ(TBUS)������ʱ�����������ָ�����
    if ((ETH->DMASR & ETH_DMASR_TBUS) != (uint32_t)RESET)
    { 
        ETH->DMASR = ETH_DMASR_TBUS;  // ���ETH DMA TBUSλ 
        ETH->DMATPDR = 0;             // �ָ�DMA����
    } 
    
    // ����ETH DMAȫ��Tx������Ϊ��һ��Tx������
    if (DMATxDescToSet->Buffer2NextDescAddr != 0)
    {
        DMATxDescToSet = (ETH_DMADESCTypeDef*)(DMATxDescToSet->Buffer2NextDescAddr);
    }
    
    return ETH_SUCCESS;   
}

// �õ���ǰ��������Tx buffer��ַ
// ����ֵ:Tx buffer��ַ
uint32_t ETH_GetCurrentTxBuffer(void)
{  
  return DMATxDescToSet->Buffer1Addr;//����Tx buffer��ַ  
}

///**
// * @breif       ΪETH�ײ����������ڴ�
// * @param       ��
// * @retval      0,����
// *              1,ʧ��
// */
//uint8_t ethernet_mem_malloc(void)
//{
//    if ((g_eth_dma_rx_dscr_tab || g_eth_dma_tx_dscr_tab || g_eth_rx_buf || g_eth_tx_buf) == NULL)
//    {
//        g_eth_dma_rx_dscr_tab = mymalloc(SRAMIN, ETH_RXBUFNB * sizeof(ETH_DMADESCTypeDef));         /* �����ڴ� */
//        g_eth_dma_tx_dscr_tab = mymalloc(SRAMIN, ETH_TXBUFNB * sizeof(ETH_DMADESCTypeDef));         /* �����ڴ� */
//        g_eth_rx_buf = mymalloc(SRAMIN, ETH_RX_BUF_SIZE * ETH_RXBUFNB);                             /* �����ڴ� */
//        g_eth_tx_buf = mymalloc(SRAMIN, ETH_TX_BUF_SIZE * ETH_TXBUFNB);                             /* �����ڴ� */
//        
//        if (!(uint32_t)&g_eth_dma_rx_dscr_tab || !(uint32_t)&g_eth_dma_tx_dscr_tab || !(uint32_t)&g_eth_rx_buf || !(uint32_t)&g_eth_tx_buf)
//        {
//            ethernet_mem_free();
//            return 1;                                                                               /* ����ʧ�� */
//        }
//    }

//    return 0;       /* ����ɹ� */
//}

///**
// * @breif       �ͷ�ETH �ײ�����������ڴ�
// * @param       ��
// * @retval      ��
// */
//void ethernet_mem_free(void)
//{
//    myfree(SRAMIN, g_eth_dma_rx_dscr_tab);  /* �ͷ��ڴ� */
//    myfree(SRAMIN, g_eth_dma_tx_dscr_tab);  /* �ͷ��ڴ� */
//    myfree(SRAMIN, g_eth_rx_buf);           /* �ͷ��ڴ� */
//    myfree(SRAMIN, g_eth_tx_buf);           /* �ͷ��ڴ� */
//}
