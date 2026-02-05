#include "bsp_can.h"
#include "app_protocol.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "bsp_dm542.h" 
#include "task.h"

// 引用外部信号量 (在 bsp_dm542.c 中定义)
extern SemaphoreHandle_t xMotorMoveDoneSem; 
extern struct ScrewMotorStatus horSM;
extern struct ScrewMotorStatus verSM;

/**
 * @brief  CAN1 初始化 (PA11/PA12)
 * 波特率 500Kbps (APB1=42MHz)
 */
void bsp_can_init(void)
{
    GPIO_InitTypeDef       GPIO_InitStructure;
    CAN_InitTypeDef        CAN_InitStructure;
    CAN_FilterInitTypeDef  CAN_FilterInitStructure;
    NVIC_InitTypeDef       NVIC_InitStructure;

    //开启时钟 (CAN1 和 GPIOA) 
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

    // GPIO 配置 (PA11-RX, PA12-TX) 
    // 复用配置
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_CAN1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_CAN1);

    // 引脚参数初始化
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;       // 复用模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;     // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;       // 上拉
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //CAN 寄存器初始化 
    CAN_DeInit(CAN1);
    CAN_StructInit(&CAN_InitStructure);

    CAN_InitStructure.CAN_TTCM = DISABLE;
    CAN_InitStructure.CAN_ABOM = ENABLE;  // 自动离线管理 
    CAN_InitStructure.CAN_AWUM = DISABLE;
    CAN_InitStructure.CAN_NART = DISABLE; // 自动重传
                                          
    CAN_InitStructure.CAN_RFLM = DISABLE;
    CAN_InitStructure.CAN_TXFP = DISABLE;
    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal; // 正常模式

    /* 波特率计算 500Kbps */
    // APB1 = 42MHz
    // 42M / 6 / (1 + 9 + 4) = 500K
    CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
    CAN_InitStructure.CAN_BS1 = CAN_BS1_9tq;
    CAN_InitStructure.CAN_BS2 = CAN_BS2_4tq;
    CAN_InitStructure.CAN_Prescaler = 6; 
    CAN_Init(CAN1, &CAN_InitStructure);

    // 过滤器配置 (只接收 GD32 反馈的 ID: 0x202) 
    CAN_FilterInitStructure.CAN_FilterNumber = 0;
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;

    // ID 左移 3 位以对齐寄存器 (STID[10:0] 在 bit31-21)
    // 0x202 = 0010 0000 0010
    uint32_t filter_id = (CAN_ID_FB_TX << 21);
    
    CAN_FilterInitStructure.CAN_FilterIdHigh = (filter_id >> 16) & 0xFFFF;
    CAN_FilterInitStructure.CAN_FilterIdLow  = filter_id & 0xFFFF;
    // FFFE取出 ID 位，最后一位 RTR/IDE 也可以匹配
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0xFFFF; 
    CAN_FilterInitStructure.CAN_FilterMaskIdLow  = 0xFFFE;

    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0;
    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&CAN_FilterInitStructure);

    // 中断配置 (FIFO0 消息挂起中断) 
    NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6; // 根据FreeRTOS 需求调整
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
}
/**
 * @brief  发送电机指令 (ID: 0x201)
 * @return 0:成功, 1:失败
 */
uint8_t can_send_motor_cmd(float x, float y)
{
    CanTxMsg TxMessage;
    FloatByte_u cvtX, cvtY;
    uint8_t mailbox;
    uint16_t timeout = 0;

    cvtX.fVal = x; 
    cvtY.fVal = y;

    TxMessage.StdId = CAN_ID_CMD_RX; // 0x201
    TxMessage.ExtId = 0x00;
    TxMessage.RTR = CAN_RTR_Data;
    TxMessage.IDE = CAN_Id_Standard;
    TxMessage.DLC = 8;
    
    TxMessage.Data[0] = cvtX.bytes[0]; TxMessage.Data[1] = cvtX.bytes[1];
    TxMessage.Data[2] = cvtX.bytes[2]; TxMessage.Data[3] = cvtX.bytes[3];
    TxMessage.Data[4] = cvtY.bytes[0]; TxMessage.Data[5] = cvtY.bytes[1];
    TxMessage.Data[6] = cvtY.bytes[2]; TxMessage.Data[7] = cvtY.bytes[3];

    mailbox = CAN_Transmit(CAN1, &TxMessage);

    // 简单的等待发送完成，防止发太快堵塞 
    while((CAN_TransmitStatus(CAN1, mailbox) == CAN_TxStatus_Failed) && (timeout < 0xFFFF))
    {
        timeout++;
    }

    if(timeout >= 0xFFFF) return 1;
    return 0;
}

/**
 * @brief  CAN1 RX0 中断服务函数
 */
void CAN1_RX0_IRQHandler(void)
{
    CanRxMsg RxMessage;
    FloatByte_u cvtX, cvtY;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (CAN_GetITStatus(CAN1, CAN_IT_FMP0) != RESET)
    {
        CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);

        // 检查是否是 GD32 的反馈 (ID: 0x202)
        if (RxMessage.StdId == CAN_ID_FB_TX && RxMessage.DLC == 8)
        {
            cvtX.bytes[0] = RxMessage.Data[0]; cvtX.bytes[1] = RxMessage.Data[1];
            cvtX.bytes[2] = RxMessage.Data[2]; cvtX.bytes[3] = RxMessage.Data[3];
            cvtY.bytes[0] = RxMessage.Data[4]; cvtY.bytes[1] = RxMessage.Data[5];
            cvtY.bytes[2] = RxMessage.Data[6]; cvtY.bytes[3] = RxMessage.Data[7];

            // 更新电机状态
            horSM.currentPosition = cvtX.fVal;
            verSM.currentPosition = cvtY.fVal;

            // 释放信号量，通知任务
            if(xMotorMoveDoneSem != NULL)
            {
                xSemaphoreGiveFromISR(xMotorMoveDoneSem, &xHigherPriorityTaskWoken);
            }
        }
    }
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

