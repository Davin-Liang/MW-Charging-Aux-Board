/**
 ****************************************************************************************************
 * @file        ethernet.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2021-10-14
 * @brief       ETHERNET 驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 探索者 F407开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20211014
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include "ethernet.h"
//#include "lwip_comm.h"
#include "stm32f429_eth_conf.h"
#include "stm32f429_eth.h"
//#include "delay.h"
#include "malloc.h"


//ETH_HandleTypeDef g_eth_handler;            /* 以太网句柄 */
//ETH_DMADESCTypeDef *g_eth_dma_rx_dscr_tab;  /* 以太网DMA接收描述符数据结构体指针 */
//ETH_DMADESCTypeDef *g_eth_dma_tx_dscr_tab;  /* 以太网DMA发送描述符数据结构体指针 */
//uint8_t *g_eth_rx_buf;                      /* 以太网底层驱动接收buffers指针 */
//uint8_t *g_eth_tx_buf;                      /* 以太网底层驱动发送buffers指针 */


//ETH_DMADESCTypeDef g_eth_dma_rx_dscr_tab[ETH_RXBUFNB];    //以太网DMA接收描述符数据结构体指针  
//ETH_DMADESCTypeDef g_eth_dma_tx_dscr_tab[ETH_TXBUFNB];    //以太网DMA发送描述符数据结构体指针   
//uint8_t g_eth_rx_buf[ETH_RX_BUF_SIZE*ETH_RXBUFNB];    //以太网底层驱动接收buffers指针   
//uint8_t g_eth_tx_buf[ETH_TX_BUF_SIZE*ETH_TXBUFNB];    //以太网底层驱动发送buffers指针 

//ETH_DMADESCTypeDef  *DMATxDescToSet;            //DMA发送描述符追踪指针
//ETH_DMADESCTypeDef  *DMARxDescToGet;         //DMA接收描述符追踪指针 
//ETH_DMA_Rx_Frame_infos *DMA_RX_FRAME_infos;    //DMA最后接收到的帧信息指针


ETH_InitTypeDef ETH_InitStructure;

/**
 * @brief       以太网芯片初始化
 * @param       无
 * @retval      0,成功
 *              1,失败
 */
uint8_t ethernet_init(void)
{
    ETH_MspInit();
	
		ETH_StructInit(&ETH_InitStructure);

    /* Fill ETH_InitStructure parametrs */
    /*------------------------   MAC   -----------------------------------*/
        /* 开启网络自适应功能 */
    ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable;
    //  ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Disable; 
    //  ETH_InitStructure.ETH_Speed = ETH_Speed_10M;
    //  ETH_InitStructure.ETH_Mode = ETH_Mode_FullDuplex;   
    /* 关闭反馈 */
    ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;
        /* 关闭重传功能 */
    ETH_InitStructure.ETH_RetryTransmission = ETH_RetryTransmission_Disable;
        /* 关闭自动去除PDA/CRC功能  */
    ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable;
        /* 关闭接收所有的帧 */
    ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Disable;
        /* 允许接收所有广播帧 */
    ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable;
        /* 关闭混合模式的地址过滤  */
    ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;
        /* 对于组播地址使用完美地址过滤    */
    ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;
        /* 对单播地址使用完美地址过滤  */
    ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;
    #ifdef CHECKSUM_BY_HARDWARE
        /* 开启ipv4和TCP/UDP/ICMP的帧校验和卸载   */
    ETH_InitStructure.ETH_ChecksumOffload = ETH_ChecksumOffload_Enable;
    #endif

    /*------------------------   DMA   -----------------------------------*/  

    /* When we use the Checksum offload feature, we need to enable the Store and Forward mode:
    the store and forward guarantee that a whole frame is stored in the FIFO, so the MAC can insert/verify the checksum, 
    if the checksum is OK the DMA can handle the frame otherwise the frame is dropped */
        /*当我们使用帧校验和卸载功能的时候，一定要使能存储转发模式,存储转发模式中要保证整个帧存储在FIFO中,
        这样MAC能插入/识别出帧校验值,当真校验正确的时候DMA就可以处理帧,否则就丢弃掉该帧*/
        
        /* 开启丢弃TCP/IP错误帧 */
    ETH_InitStructure.ETH_DropTCPIPChecksumErrorFrame = ETH_DropTCPIPChecksumErrorFrame_Enable;
        /* 开启接收数据的存储转发模式  */
    ETH_InitStructure.ETH_ReceiveStoreForward = ETH_ReceiveStoreForward_Enable;
        /* 开启发送数据的存储转发模式   */
    ETH_InitStructure.ETH_TransmitStoreForward = ETH_TransmitStoreForward_Enable;

        /* 禁止转发错误帧 */
    ETH_InitStructure.ETH_ForwardErrorFrames = ETH_ForwardErrorFrames_Disable;
        /* 不转发过小的好帧 */
    ETH_InitStructure.ETH_ForwardUndersizedGoodFrames = ETH_ForwardUndersizedGoodFrames_Disable;
        /* 打开处理第二帧功能 */
    ETH_InitStructure.ETH_SecondFrameOperate = ETH_SecondFrameOperate_Enable;
        /* 开启DMA传输的地址对齐功能 */
    ETH_InitStructure.ETH_AddressAlignedBeats = ETH_AddressAlignedBeats_Enable;
        /* 开启固定突发功能 */
    ETH_InitStructure.ETH_FixedBurst = ETH_FixedBurst_Enable;
        /* DMA发送的最大突发长度为32个节拍 */
    ETH_InitStructure.ETH_RxDMABurstLength = ETH_RxDMABurstLength_32Beat;
        /*DMA接收的最大突发长度为32个节拍 */
    ETH_InitStructure.ETH_TxDMABurstLength = ETH_TxDMABurstLength_32Beat;
    ETH_InitStructure.ETH_DMAArbitration = ETH_DMAArbitration_RoundRobin_RxTx_2_1;

    /* Configure Ethernet */
	/* 配置ETH */
		uint32_t EthStatus = ETH_ERROR;
    EthStatus = ETH_Init(&ETH_InitStructure, ETHERNET_PHY_ADDRESS);
    if (EthStatus == ETH_SUCCESS)
    {
        return 0;   /* 成功 */
    }
    else
    {
        return 1;  /* 失败 */
    }
    // uint8_t macaddress[6];

    // macaddress[0] = g_lwipdev.mac[0];
    // macaddress[1] = g_lwipdev.mac[1];
    // macaddress[2] = g_lwipdev.mac[2];
    // macaddress[3] = g_lwipdev.mac[3];
    // macaddress[4] = g_lwipdev.mac[4];
    // macaddress[5] = g_lwipdev.mac[5];

    // g_eth_handler.Instance = ETH;
    // g_eth_handler.Init.AutoNegotiation = ETH_AUTONEGOTIATION_ENABLE;    /* 使能自协商模式 */
    // g_eth_handler.Init.Speed = ETH_SPEED_100M;                          /* 速度100M,如果开启了自协商模式，此配置就无效 */
    // g_eth_handler.Init.DuplexMode = ETH_MODE_FULLDUPLEX;                /* 全双工模式，如果开启了自协商模式，此配置就无效 */
    // g_eth_handler.Init.PhyAddress = ETHERNET_PHY_ADDRESS;               /* 以太网芯片的地址 */
    // g_eth_handler.Init.MACAddr = macaddress;                            /* MAC地址 */
    // g_eth_handler.Init.RxMode = ETH_RXINTERRUPT_MODE;                   /* 中断接收模式 */
    // g_eth_handler.Init.ChecksumMode = ETH_CHECKSUM_BY_HARDWARE;         /* 硬件帧校验 */
    // g_eth_handler.Init.MediaInterface = ETH_MEDIA_INTERFACE_RMII;       /* RMII接口 */
}

/**
 * @brief       ETH底层驱动，时钟使能，引脚配置
 *    @note     此函数会被HAL_ETH_Init()调用
 * @param       heth:以太网句柄
 * @retval      无
 */
void ETH_MspInit(void)
{
    // GPIO_InitTypeDef gpio_init_struct;

    // ETH_CLK_GPIO_CLK_ENABLE();          /* 开启ETH_CLK时钟 */
    // ETH_MDIO_GPIO_CLK_ENABLE();         /* 开启ETH_MDIO时钟 */
    // ETH_CRS_GPIO_CLK_ENABLE();          /* 开启ETH_CRS时钟 */
    // ETH_MDC_GPIO_CLK_ENABLE();          /* 开启ETH_MDC时钟 */
    // ETH_RXD0_GPIO_CLK_ENABLE();         /* 开启ETH_RXD0时钟 */
    // ETH_RXD1_GPIO_CLK_ENABLE();         /* 开启ETH_RXD1时钟 */
    // ETH_TX_EN_GPIO_CLK_ENABLE();        /* 开启ETH_TX_EN时钟 */
    // ETH_TXD0_GPIO_CLK_ENABLE();         /* 开启ETH_TXD0时钟 */
    // ETH_TXD1_GPIO_CLK_ENABLE();         /* 开启ETH_TXD1时钟 */
    // ETH_RESET_GPIO_CLK_ENABLE();        /* 开启ETH_RESET时钟 */
    // __HAL_RCC_ETH_CLK_ENABLE();         /* 开启ETH时钟 */


    // /* 网络引脚设置 RMII接口
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
    // gpio_init_struct.Mode = GPIO_MODE_AF_PP;                /* 推挽复用 */
    // gpio_init_struct.Pull = GPIO_NOPULL;                    /* 不带上下拉 */
    // gpio_init_struct.Speed = GPIO_SPEED_HIGH;               /* 高速 */
    // gpio_init_struct.Alternate = GPIO_AF11_ETH;             /* 复用为ETH功能 */
    // HAL_GPIO_Init(ETH_CLK_GPIO_PORT, &gpio_init_struct);    /* ETH_CLK引脚模式设置 */
    
    // gpio_init_struct.Pin = ETH_MDIO_GPIO_PIN;
    // HAL_GPIO_Init(ETH_MDIO_GPIO_PORT, &gpio_init_struct);   /* ETH_MDIO引脚模式设置 */
    
    // gpio_init_struct.Pin = ETH_CRS_GPIO_PIN;    
    // HAL_GPIO_Init(ETH_CRS_GPIO_PORT, &gpio_init_struct);    /* ETH_CRS引脚模式设置 */

    // /* PC1 */
    // gpio_init_struct.Pin = ETH_MDC_GPIO_PIN;
    // HAL_GPIO_Init(ETH_MDC_GPIO_PORT, &gpio_init_struct);    /* ETH_MDC初始化 */

    // /* PC4 */
    // gpio_init_struct.Pin = ETH_RXD0_GPIO_PIN;
    // HAL_GPIO_Init(ETH_RXD0_GPIO_PORT, &gpio_init_struct);   /* ETH_RXD0初始化 */
    
    // /* PC5 */
    // gpio_init_struct.Pin = ETH_RXD1_GPIO_PIN;
    // HAL_GPIO_Init(ETH_RXD1_GPIO_PORT, &gpio_init_struct);   /* ETH_RXD1初始化 */
    
    
    // /* PG11,13,14 */
    // gpio_init_struct.Pin = ETH_TX_EN_GPIO_PIN; 
    // HAL_GPIO_Init(ETH_TX_EN_GPIO_PORT, &gpio_init_struct);  /* ETH_TX_EN初始化 */

    // gpio_init_struct.Pin = ETH_TXD0_GPIO_PIN; 
    // HAL_GPIO_Init(ETH_TXD0_GPIO_PORT, &gpio_init_struct);   /* ETH_TXD0初始化 */
    
    // gpio_init_struct.Pin = ETH_TXD1_GPIO_PIN; 
    // HAL_GPIO_Init(ETH_TXD1_GPIO_PORT, &gpio_init_struct);   /* ETH_TXD1初始化 */
    
    
    // /* 复位引脚 */
    // gpio_init_struct.Pin = ETH_RESET_GPIO_PIN;      /* ETH_RESET初始化 */
    // gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;    /* 推挽输出 */
    // gpio_init_struct.Pull = GPIO_NOPULL;            /* 无上下拉 */
    // gpio_init_struct.Speed = GPIO_SPEED_HIGH;       /* 高速 */
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
    // 使能 ETH 时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_ETH_MAC | 
                            RCC_AHB1Periph_ETH_MAC_Tx |
                            RCC_AHB1Periph_ETH_MAC_Rx, ENABLE);

    /* Enable SYSCFG clock */
    // 使用重映射引脚功能，需要使能这部分时钟，其实使不使能都无所谓
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

		SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_RMII); // //MAC和PHY之间使用RMII接口

//    /* MII/RMII Media interface selection --------------------------------------*/
//    #ifdef MII_MODE /* Mode MII with STM324xG-EVAL  */
//    #ifdef PHY_CLOCK_MCO

//    /* Output HSE clock (25MHz) on MCO pin (PA8) to clock the PHY */
//    RCC_MCO1Config(RCC_MCO1Source_HSE, RCC_MCO1Div_1);
//    #endif /* PHY_CLOCK_MCO */

//    SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_MII);
//    #elif defined RMII_MODE  /* Mode RMII with STM324xG-EVAL */

//    SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_RMII); // //MAC和PHY之间使用RMII接口
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

    /* 复用引脚 */
    GPIO_InitStructure.GPIO_Pin = ETH_RESET_GPIO_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(ETH_RESET_GPIO_PORT, &GPIO_InitStructure);

    ETHERNET_RST(0);     /* 硬件复位 */
    // delay_ms(50); TODO:替换延时函数
    ETHERNET_RST(1);     /* 复位结束 */

//    HAL_NVIC_SetPriority(ETH_IRQn, 6, 0); /* 网络中断优先级应该高一点 */
//    HAL_NVIC_EnableIRQ(ETH_IRQn);

		// 设置中断优先级
		NVIC_InitTypeDef NVIC_InitStructure;
		NVIC_InitStructure.NVIC_IRQChannel = ETH_IRQn;           // 以太网中断
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; // 抢占优先级
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        // 子优先级
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // 使能中断
		NVIC_Init(&NVIC_InitStructure);

}

/**
 * @breif       读取以太网芯片寄存器值
 * @param       reg：读取的寄存器地址
 * @retval      无
 */
uint32_t ethernet_read_phy(uint16_t reg)
{
    uint32_t regval;

    regval = ETH_ReadPHYRegister(ETHERNET_PHY_ADDRESS, reg);
    // HAL_ETH_ReadPHYRegister(&g_eth_handler, reg, &regval);

    return regval;
}

/**
 * @breif       向以太网芯片指定地址写入寄存器值
 * @param       reg   : 要写入的寄存器
 * @param       value : 要写入的寄存器
 * @retval      无
 */
void ethernet_write_phy(uint16_t reg, uint16_t value)
{
    uint32_t temp = value;
    
    ETH_WritePHYRegister(ETHERNET_PHY_ADDRESS, reg, value);
    // HAL_ETH_WritePHYRegister(&g_eth_handler, reg, temp);
}

/**
 * @breif       获得网络芯片的速度模式
 * @param       无
 * @retval      1:获取100M成功
                0:失败
 */
uint8_t ethernet_chip_get_speed(void)
{
    uint8_t speed;
    #if(PHY_TYPE == LAN8720) 
    speed = ~((ethernet_read_phy(PHY_SR) & PHY_SPEED_STATUS));     /* 从LAN8720的31号寄存器中读取网络速度和双工模式 */
    #elif(PHY_TYPE == SR8201F)
    speed = ((ethernet_read_phy(PHY_SR) & PHY_SPEED_STATUS) >> 13);    /* 从SR8201F的0号寄存器中读取网络速度和双工模式 */
    #elif(PHY_TYPE == YT8512C)
    speed = ((ethernet_read_phy(PHY_SR) & PHY_SPEED_STATUS) >> 14);    /* 从YT8512C的17号寄存器中读取网络速度和双工模式 */
    #elif(PHY_TYPE == RTL8201)
    speed = ((ethernet_read_phy(PHY_SR) & PHY_SPEED_STATUS) >> 1);     /* 从RTL8201的16号寄存器中读取网络速度和双工模式 */
    #endif

    return speed;
}

//extern void lwip_pkt_handle(void); /* 在lwip_comm.c里面定义 */

/**
 * @breif       中断服务函数
 * @param       无
 * @retval      无
 */
//void ETH_IRQHandler(void)
//{
//    if (ethernet_get_eth_rx_size(g_eth_handler.RxDesc))
//    {
////        lwip_pkt_handle();      /* 处理以太网数据，即将数据提交给LWIP */
//    }

//    __HAL_ETH_DMA_CLEAR_IT(&g_eth_handler, ETH_DMA_IT_NIS);   /* 清除DMA中断标志位 */
//    __HAL_ETH_DMA_CLEAR_IT(&g_eth_handler, ETH_DMA_IT_R);     /* 清除DMA接收中断标志位 */
//}

void ETH_IRQHandler(void)
{
    /* 检查并清除所有中断标志 */
    if (ETH_GetDMAFlagStatus(ETH_DMA_FLAG_NIS) != RESET)
    {
        /* 检查接收中断 */
        if (ETH_GetDMAFlagStatus(ETH_DMA_FLAG_R) != RESET)
        {
            if (ethernet_get_eth_rx_size(DMARxDescToGet))
            {
//                lwip_pkt_handle();      /* 处理以太网数据，即将数据提交给LWIP */
            }
            ETH_DMAClearITPendingBit(ETH_DMA_IT_R);
        }
        
        /* 清除正常中断总结标志 */
        ETH_DMAClearITPendingBit(ETH_DMA_IT_NIS);
    }
}


/**
 * @breif       获取接收到的帧长度
 * @param       dma_rx_desc : 接收DMA描述符
 * @retval      frameLength : 接收到的帧长度
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

//接收一个网卡数据包
//返回值:网络数据包帧结构体
FrameTypeDef ETH_Rx_Packet(void)
{ 
    uint32_t framelength = 0;
    FrameTypeDef frame = {0,0};   
		
    /* 检查当前描述符,是否属于ETHERNET DMA(设置的时候)/CPU(复位的时候) */
    if ((DMARxDescToGet->Status & ETH_DMATxDesc_OWN) != (uint32_t)RESET)
    {    
        frame.length = ETH_ERROR; 
			
				/* 检查接收缓冲区不可用状态 */
        if ((ETH->DMASR & ETH_DMASR_RBUS) != (uint32_t)RESET)  
        { 
            ETH->DMASR = ETH_DMASR_RBUS;//清除ETH DMA的RBUS位 
            ETH->DMARPDR = 0;//恢复DMA接收
        }
        return frame;//错误,OWN位被设置了
    }  
		
    if (((DMARxDescToGet->Status & ETH_DMARxDesc_ES) == (uint32_t)RESET) && 
    ((DMARxDescToGet->Status & ETH_DMARxDesc_LS) != (uint32_t)RESET) &&  
    ((DMARxDescToGet->Status & ETH_DMARxDesc_FS) != (uint32_t)RESET))  
    {       
        framelength = ((DMARxDescToGet->Status & ETH_DMARxDesc_FL) >> ETH_DMARxDesc_FrameLengthShift) - 4; // 得到接收包帧长度(不包含4字节CRC)
        frame.buffer = DMARxDescToGet->Buffer1Addr; // 得到包数据所在的位置
    }
		else 
				framelength = ETH_ERROR; // 错误  
    frame.length = framelength; 
    frame.descriptor = DMARxDescToGet; 
		
    // 更新ETH DMA全局Rx描述符为下一个Rx描述符
    // 为下一次buffer读取设置下一个DMA Rx描述符
    DMARxDescToGet = (ETH_DMADESCTypeDef*)(DMARxDescToGet->Buffer2NextDescAddr);   
    return frame;  
}

// 发送一个网卡数据包
// FrameLength:数据包长度
// 返回值:ETH_ERROR,发送失败(0)
//        ETH_SUCCESS,发送成功(1)
uint8_t ETH_Tx_Packet(uint16_t FrameLength)
{   
    // 检查当前描述符是否属于CPU（OWN位为0表示属于CPU）
    if ((DMATxDescToSet->Status & ETH_DMATxDesc_OWN) != (uint32_t)RESET)
        return ETH_ERROR;  // 错误，OWN位被设置了（属于DMA）
    
    // 设置帧长度到控制缓冲区大小字段的TBS1位域
    DMATxDescToSet->ControlBufferSize = FrameLength & ETH_DMATxDesc_TBS1;
    
    // 设置最后一个和第一个段位（1个描述符传输一帧）
    DMATxDescToSet->Status |= ETH_DMATxDesc_LS | ETH_DMATxDesc_FS;
    
    // 设置Tx描述符的OWN位，buffer重归ETH DMA
    DMATxDescToSet->Status |= ETH_DMATxDesc_OWN;
    
    // 当Tx Buffer不可用位(TBUS)被设置时，重置它并恢复传输
    if ((ETH->DMASR & ETH_DMASR_TBUS) != (uint32_t)RESET)
    { 
        ETH->DMASR = ETH_DMASR_TBUS;  // 清除ETH DMA TBUS位 
        ETH->DMATPDR = 0;             // 恢复DMA发送
    } 
    
    // 更新ETH DMA全局Tx描述符为下一个Tx描述符
    if (DMATxDescToSet->Buffer2NextDescAddr != 0)
    {
        DMATxDescToSet = (ETH_DMADESCTypeDef*)(DMATxDescToSet->Buffer2NextDescAddr);
    }
    
    return ETH_SUCCESS;   
}

// 得到当前描述符的Tx buffer地址
// 返回值:Tx buffer地址
uint32_t ETH_GetCurrentTxBuffer(void)
{  
  return DMATxDescToSet->Buffer1Addr;//返回Tx buffer地址  
}

///**
// * @breif       为ETH底层驱动申请内存
// * @param       无
// * @retval      0,正常
// *              1,失败
// */
//uint8_t ethernet_mem_malloc(void)
//{
//    if ((g_eth_dma_rx_dscr_tab || g_eth_dma_tx_dscr_tab || g_eth_rx_buf || g_eth_tx_buf) == NULL)
//    {
//        g_eth_dma_rx_dscr_tab = mymalloc(SRAMIN, ETH_RXBUFNB * sizeof(ETH_DMADESCTypeDef));         /* 申请内存 */
//        g_eth_dma_tx_dscr_tab = mymalloc(SRAMIN, ETH_TXBUFNB * sizeof(ETH_DMADESCTypeDef));         /* 申请内存 */
//        g_eth_rx_buf = mymalloc(SRAMIN, ETH_RX_BUF_SIZE * ETH_RXBUFNB);                             /* 申请内存 */
//        g_eth_tx_buf = mymalloc(SRAMIN, ETH_TX_BUF_SIZE * ETH_TXBUFNB);                             /* 申请内存 */
//        
//        if (!(uint32_t)&g_eth_dma_rx_dscr_tab || !(uint32_t)&g_eth_dma_tx_dscr_tab || !(uint32_t)&g_eth_rx_buf || !(uint32_t)&g_eth_tx_buf)
//        {
//            ethernet_mem_free();
//            return 1;                                                                               /* 申请失败 */
//        }
//    }

//    return 0;       /* 申请成功 */
//}

///**
// * @breif       释放ETH 底层驱动申请的内存
// * @param       无
// * @retval      无
// */
//void ethernet_mem_free(void)
//{
//    myfree(SRAMIN, g_eth_dma_rx_dscr_tab);  /* 释放内存 */
//    myfree(SRAMIN, g_eth_dma_tx_dscr_tab);  /* 释放内存 */
//    myfree(SRAMIN, g_eth_rx_buf);           /* 释放内存 */
//    myfree(SRAMIN, g_eth_tx_buf);           /* 释放内存 */
//}
