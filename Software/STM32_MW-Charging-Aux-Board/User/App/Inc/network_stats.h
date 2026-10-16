#ifndef __NETWORK_STATS_H
#define __NETWORK_STATS_H

#include <stdint.h>
#include "FreeRTOS.h"

typedef struct
{
    /* ================= Ethernet RX ================= */

    volatile uint32_t rx_irq_count;       // RX 中断次数：DMA 产生接收完成中断的次数
    volatile uint32_t rx_wakeup_count;    // RX 任务唤醒通知次数：中断中成功投递信号量的次数

    volatile uint32_t rx_desc_error;      // RX 描述符错误次数：帧错误或不满足当前单描述符收帧条件
    volatile uint32_t rx_rbus_count;      // RX Buffer Unavailable 次数：检测到 DMA 接收缓冲区不可用的次数
    volatile uint32_t rx_pbuf_fail;       // pbuf 分配失败次数：lwIP 接收内存池无法提供缓冲区

    volatile uint32_t rx_packet_count;    // RX 数据帧复制成功次数：DMA Buffer 数据成功复制到 pbuf
    volatile uint32_t rx_input_ok;        // RX 提交成功次数：netif->input() 返回 ERR_OK
    volatile uint32_t rx_input_error;     // RX 提交失败次数：netif->input() 返回错误

    /* ================= Ethernet TX ================= */

    volatile uint32_t tx_packet_count;    // TX 提交成功次数：描述符 OWN 置位并提交给 DMA
    volatile uint32_t tx_desc_busy;       // TX 描述符忙次数：发送时发现 OWN=1，描述符仍归 DMA
    volatile uint32_t tx_output_error;    // lwIP 底层发送失败次数：low_level_output() 返回发送错误

    /* ================= Modbus TCP ================= */

    volatile uint32_t tcp_connect_count;     // TCP 客户端接入成功次数：modbus_tcp_accept() 成功
    volatile uint32_t tcp_disconnect_count;  // 客户端连接结束次数：进入当前客户端连接清理路径
    volatile uint32_t tcp_accept_error;      // TCP accept 失败次数

    volatile uint32_t modbus_req_count;       // Modbus 请求接收成功次数：modbus_receive() 返回正值
    volatile uint32_t modbus_recv_end_count;  // Modbus 接收结束/失败次数：modbus_receive() 返回非正值
    volatile uint32_t modbus_reply_error;     // Modbus 响应失败次数：modbus_reply() 返回负值

} NetworkStats_t;

extern NetworkStats_t g_networkStats;

BaseType_t create_task_for_network_stats(uint16_t size, UBaseType_t priority);

#endif
