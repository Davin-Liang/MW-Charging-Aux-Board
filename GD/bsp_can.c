#include "bsp_can.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "app_protocol.h"

/* 定义接收队列，用于将中断数据传给任务 */
extern QueueHandle_t motorCmdQueue; 

void bsp_can_config(void)
{
    /* 1. 开启时钟 (假设使用 CAN0, 引脚 PA11/PA12) */
    rcu_periph_clock_enable(RCU_CAN0);
    rcu_periph_clock_enable(RCU_GPIOA);

    /* 2. 配置 GPIO (PA11-RX, PA12-TX) */
    /* 复用功能设置 */
    gpio_af_set(GPIOA, GPIO_AF_9, GPIO_PIN_11);
    gpio_af_set(GPIOA, GPIO_AF_9, GPIO_PIN_12);
    
    /* 模式配置: 复用推挽 */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_11);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_11);
    
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_12);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12);

    /* 3. CAN 参数配置 */
    can_parameter_struct can_parameter;
    can_struct_para_init(CAN_INIT_STRUCT, &can_parameter);
    can_deinit(CAN0);

    can_parameter.time_triggered = DISABLE;
    can_parameter.auto_bus_off_recovery = ENABLE;
    can_parameter.auto_wake_up = DISABLE;
    can_parameter.auto_retrans = ENABLE;
    can_parameter.rec_fifo_overwrite = DISABLE;
    can_parameter.trans_fifo_order = DISABLE;
    can_parameter.working_mode = CAN_WORKING_MODE_NORMAL;

    /* 4. 波特率计算: 500Kbps */
    /* 注意: GD32F4 CAN 在 APB1, 假设 APB1=42MHz (System=168MHz) 
       Baud = PCLK / (Prescaler * (1 + BS1 + BS2))
       42M / 6 / (1 + 9 + 4) = 42M / 6 / 14 = 500K */
    can_parameter.prescaler = 6;
    can_parameter.resynch_jump_width = CAN_BT_SJW_1TQ;
    can_parameter.time_segment_1 = CAN_BT_BS1_9TQ;
    can_parameter.time_segment_2 = CAN_BT_BS2_4TQ;
    can_init(CAN0, &can_parameter);

    /* 5. 配置过滤器 (允许所有ID或指定ID) */
    can_filter_parameter_struct can_filter;
    can_filter.filter_number = 0;
    can_filter.filter_mode = CAN_FILTERMODE_MASK;
    can_filter.filter_bits = CAN_FILTERBITS_32BIT;
    can_filter.filter_list_high = 0x0000;
    can_filter.filter_list_low = 0x0000;
    can_filter.filter_mask_high = 0x0000;
    can_filter.filter_mask_low = 0x0000; // 掩码为0，接收所有
    can_filter.filter_fifo_number = CAN_FIFO0;
    can_filter.filter_enable = ENABLE;
    can_filter_init(&can_filter);

    /* 6. 配置中断 (接收 FIFO0) */
    nvic_irq_enable(CAN0_RX0_IRQn, 6, 0); // FreeRTOS 中优先级不要太高
    can_interrupt_enable(CAN0, CAN_INT_RFNE0);
}

/* CAN 发送函数 */
void can_send_position_feedback(float x, float y)
{
    can_trasnmit_message_struct tx_message;
    FloatByte_u cvtX, cvtY;

    cvtX.fVal = x;
    cvtY.fVal = y;

    tx_message.tx_sfid = CAN_ID_FB_TX; // ID: 0x202
    tx_message.tx_efid = 0x00;
    tx_message.tx_ft = CAN_FT_DATA;
    tx_message.tx_ff = CAN_FF_STANDARD;
    tx_message.tx_dlen = 8;
    
    /* 填充数据 */
    tx_message.tx_data[0] = cvtX.bytes[0];
    tx_message.tx_data[1] = cvtX.bytes[1];
    tx_message.tx_data[2] = cvtX.bytes[2];
    tx_message.tx_data[3] = cvtX.bytes[3];
    
    tx_message.tx_data[4] = cvtY.bytes[0];
    tx_message.tx_data[5] = cvtY.bytes[1];
    tx_message.tx_data[6] = cvtY.bytes[2];
    tx_message.tx_data[7] = cvtY.bytes[3];

    can_message_transmit(CAN0, &tx_message);
}

/* CAN 接收中断服务函数 */
void CAN0_RX0_IRQHandler(void)
{
    can_receive_message_struct rx_message;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    Coordinate_t receivedCmd;
    FloatByte_u cvtX, cvtY;

    if(can_interrupt_flag_get(CAN0, CAN_INT_FLAG_RFL0) != RESET){
        can_message_receive(CAN0, CAN_FIFO0, &rx_message);

        /* 解析数据 */
        if(rx_message.rx_sfid == CAN_ID_CMD_RX && rx_message.rx_dlen == 8)
        {
            cvtX.bytes[0] = rx_message.rx_data[0];
            cvtX.bytes[1] = rx_message.rx_data[1];
            cvtX.bytes[2] = rx_message.rx_data[2];
            cvtX.bytes[3] = rx_message.rx_data[3];
            
            cvtY.bytes[0] = rx_message.rx_data[4];
            cvtY.bytes[1] = rx_message.rx_data[5];
            cvtY.bytes[2] = rx_message.rx_data[6];
            cvtY.bytes[3] = rx_message.rx_data[7];

            receivedCmd.x = cvtX.fVal;
            receivedCmd.y = cvtY.fVal;

            /* 发送到队列 */
            xQueueSendFromISR(motorCmdQueue, &receivedCmd, &xHigherPriorityTaskWoken);
        }
    }
    
    /* 如果唤醒了高优先级任务，进行上下文切换 */
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}