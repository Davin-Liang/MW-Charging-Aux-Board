#include "bsp_can.h"
#include "app_protocol.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "bsp_debug_usart.h"
// 引用外部定义的队列（在 motor_task.c 中定义）
extern QueueHandle_t motorCmdQueue;

void bsp_can_config(void)
{
		can_parameter_struct can_parameter;
    can_filter_parameter_struct can_filter;
    /* 1. 开启时钟 (CAN0, GPIOA) */
    rcu_periph_clock_enable(RCU_CAN0);
    rcu_periph_clock_enable(RCU_GPIOB);

    /* 2. GPIO 配置 (PA11-RX, PA12-TX) */
		gpio_af_set(GPIOB, GPIO_AF_9, GPIO_PIN_8);
    gpio_af_set(GPIOB, GPIO_AF_9, GPIO_PIN_9);
	
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_8);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
	
	
		gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_9);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);

    /* 3. CAN 参数配置 (500Kbps) */
//    can_parameter_struct can_parameter;
    can_struct_para_init(CAN_INIT_STRUCT, &can_parameter);
    can_deinit(CAN0);

    can_parameter.time_triggered = DISABLE;
    can_parameter.auto_bus_off_recovery = ENABLE;
    can_parameter.auto_retrans = ENABLE;
		
    can_parameter.working_mode = CAN_NORMAL_MODE;   // 正常模式(需要接线)
    //can_parameter.working_mode = CAN_LOOPBACK_MODE; // 回环模式(自发自收)
    
    // 波特率 500k (APB1=42M): 42M / 6 / (1+9+4) = 500k
    can_parameter.prescaler = 6;
    can_parameter.resync_jump_width = CAN_BT_SJW_1TQ;
    can_parameter.time_segment_1 = CAN_BT_BS1_9TQ;
    can_parameter.time_segment_2 = CAN_BT_BS2_4TQ;
    can_init(CAN0, &can_parameter);

    /* 4. 过滤器配置 */
//    can_filter_parameter_struct can_filter;
    can_filter.filter_number = 0;
    can_filter.filter_mode = CAN_FILTERMODE_MASK;
    can_filter.filter_bits = CAN_FILTERBITS_32BIT;
    can_filter.filter_list_high = 0x0000;
    can_filter.filter_list_low = 0x0000;
    can_filter.filter_mask_high = 0x0000;
    can_filter.filter_mask_low = 0x0000; // 接收所有ID
    can_filter.filter_fifo_number = CAN_FIFO0;
    can_filter.filter_enable = ENABLE;
    can_filter_init(&can_filter);

    /* 5. 中断配置 */
    nvic_irq_enable(CAN0_RX0_IRQn, 6, 0); 
    can_interrupt_enable(CAN0, CAN_INT_RFNE0);
}
/* 发送反馈 (GD32 -> STM32, ID: 0x202) */
void can_send_position_feedback(float x, float y)
{
    can_trasnmit_message_struct tx_message;
    FloatByte_u cvtX, cvtY;
    cvtX.fVal = x; cvtY.fVal = y;

    tx_message.tx_sfid = CAN_ID_FB_TX; // 0x202
    tx_message.tx_efid = 0x00;
    tx_message.tx_ft = CAN_FT_DATA;
    tx_message.tx_ff = CAN_FF_STANDARD;
    tx_message.tx_dlen = 8;
    
    tx_message.tx_data[0] = cvtX.bytes[0]; tx_message.tx_data[1] = cvtX.bytes[1];
    tx_message.tx_data[2] = cvtX.bytes[2]; tx_message.tx_data[3] = cvtX.bytes[3];
    tx_message.tx_data[4] = cvtY.bytes[0]; tx_message.tx_data[5] = cvtY.bytes[1];
    tx_message.tx_data[6] = cvtY.bytes[2]; tx_message.tx_data[7] = cvtY.bytes[3];

    can_message_transmit(CAN0, &tx_message);
	// 调试打印
    printf("[CAN TX] Feedback Sent: ID=0x%X\r\n", CAN_ID_FB_TX);
}

/* 模拟 STM32 发送指令 (ID: 0x201) */
void can_test_send_cmd(float x, float y)
{
    can_trasnmit_message_struct tx_message;
    FloatByte_u cvtX, cvtY;
    cvtX.fVal = x; cvtY.fVal = y;

    tx_message.tx_sfid = CAN_ID_CMD_RX; // 0x201 (模拟 STM32 发过来的 ID)
    tx_message.tx_efid = 0x00;
    tx_message.tx_ft = CAN_FT_DATA;
    tx_message.tx_ff = CAN_FF_STANDARD;
    tx_message.tx_dlen = 8;
    
    tx_message.tx_data[0] = cvtX.bytes[0]; tx_message.tx_data[1] = cvtX.bytes[1];
    tx_message.tx_data[2] = cvtX.bytes[2]; tx_message.tx_data[3] = cvtX.bytes[3];
    tx_message.tx_data[4] = cvtY.bytes[0]; tx_message.tx_data[5] = cvtY.bytes[1];
    tx_message.tx_data[6] = cvtY.bytes[2]; tx_message.tx_data[7] = cvtY.bytes[3];

    can_message_transmit(CAN0, &tx_message);
    
    // 调试打印
    printf("[CAN TEST] Simulating STM32 CMD: ID=0x%X (Move to %.1f, %.1f)\r\n", CAN_ID_CMD_RX, x, y);
}

/* CAN 接收中断处理 在这里给 FreeRTOS 队列发消息 */
void CAN0_RX0_IRQHandler(void)
{
    can_receive_message_struct rx_message;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    Coordinate_t receivedCmd;
    FloatByte_u cvtX, cvtY;

    if(can_interrupt_flag_get(CAN0, CAN_INT_FLAG_RFL0) != RESET){
        can_message_receive(CAN0, CAN_FIFO0, &rx_message);
				// 如果收到了 ID 为 0x201 的数据
        if(rx_message.rx_sfid == CAN_ID_CMD_RX && rx_message.rx_dlen == 8)
        {
            cvtX.bytes[0] = rx_message.rx_data[0]; cvtX.bytes[1] = rx_message.rx_data[1];
            cvtX.bytes[2] = rx_message.rx_data[2]; cvtX.bytes[3] = rx_message.rx_data[3];
            cvtY.bytes[0] = rx_message.rx_data[4]; cvtY.bytes[1] = rx_message.rx_data[5];
            cvtY.bytes[2] = rx_message.rx_data[6]; cvtY.bytes[3] = rx_message.rx_data[7];
						//小端序的数据
            receivedCmd.x = cvtX.fVal;
            receivedCmd.y = cvtY.fVal;

            /* 发送到队列 */
            xQueueSendFromISR(motorCmdQueue, &receivedCmd, &xHigherPriorityTaskWoken);
        }
    }
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
