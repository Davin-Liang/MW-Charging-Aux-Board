#include "network_stats.h"

#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "lwipopts.h"
#include "lwip/stats.h"

void print_lwip_pool_stats(void)
{
#if LWIP_STATS && MEMP_STATS
    printf("[lwIP PBUF_POOL]\r\n");
    printf(" Used : %u\r\n",
           (unsigned int)lwip_stats.memp[MEMP_PBUF_POOL]->used);
    printf(" Max  : %u\r\n",
           (unsigned int)lwip_stats.memp[MEMP_PBUF_POOL]->max);
    printf(" Err  : %u\r\n",
           (unsigned int)lwip_stats.memp[MEMP_PBUF_POOL]->err);
#endif
}


NetworkStats_t g_networkStats = {0};

#define NETWORK_STATS_TASK_PERIOD_MS    5000U

#define PRINT_STAT(name, current, previous)                                      \
    printf(" %-18s %10lu %+10ld\r\n",                                           \
           name,                                                                  \
           (unsigned long)(current),                                              \
           (long)((current) - (previous)))

static TaskHandle_t g_networkStatsTaskHandle = NULL;
static NetworkStats_t g_networkStatsSnapshot = {0};

static void network_stats_task(void *param)
{
    TickType_t last_wake_time;
    NetworkStats_t current_stats;

    (void)param;
    last_wake_time = xTaskGetTickCount();

    for (;;)
    {
        vTaskDelayUntil(&last_wake_time,
                        pdMS_TO_TICKS(NETWORK_STATS_TASK_PERIOD_MS));

        current_stats = g_networkStats;

        printf("\r\n================ NET STATS [%lus] ================\r\n",
               (unsigned long)(xTaskGetTickCount() / configTICK_RATE_HZ));
        printf("                    TOTAL      DELTA(5s)\r\n");
        printf("[Ethernet RX]\r\n");
        PRINT_STAT("IRQ", current_stats.rx_irq_count, g_networkStatsSnapshot.rx_irq_count);
        PRINT_STAT("Wakeup", current_stats.rx_wakeup_count, g_networkStatsSnapshot.rx_wakeup_count);
        PRINT_STAT("Desc Error", current_stats.rx_desc_error, g_networkStatsSnapshot.rx_desc_error);
        PRINT_STAT("RBUS", current_stats.rx_rbus_count, g_networkStatsSnapshot.rx_rbus_count);
        PRINT_STAT("Pbuf Fail", current_stats.rx_pbuf_fail, g_networkStatsSnapshot.rx_pbuf_fail);
        PRINT_STAT("RX Packet", current_stats.rx_packet_count, g_networkStatsSnapshot.rx_packet_count);
        PRINT_STAT("Input OK", current_stats.rx_input_ok, g_networkStatsSnapshot.rx_input_ok);
        PRINT_STAT("Input Error", current_stats.rx_input_error, g_networkStatsSnapshot.rx_input_error);

        printf("\r\n[Ethernet TX]\r\n");
        PRINT_STAT("TX Packet", current_stats.tx_packet_count, g_networkStatsSnapshot.tx_packet_count);
        PRINT_STAT("Desc Busy", current_stats.tx_desc_busy, g_networkStatsSnapshot.tx_desc_busy);
        PRINT_STAT("Output Error", current_stats.tx_output_error, g_networkStatsSnapshot.tx_output_error);

        printf("\r\n[Modbus TCP]\r\n");
        PRINT_STAT("Connect", current_stats.tcp_connect_count, g_networkStatsSnapshot.tcp_connect_count);
        PRINT_STAT("Disconnect", current_stats.tcp_disconnect_count, g_networkStatsSnapshot.tcp_disconnect_count);
        PRINT_STAT("Accept Error", current_stats.tcp_accept_error, g_networkStatsSnapshot.tcp_accept_error);
        PRINT_STAT("Request", current_stats.modbus_req_count, g_networkStatsSnapshot.modbus_req_count);
        PRINT_STAT("Recv End", current_stats.modbus_recv_end_count, g_networkStatsSnapshot.modbus_recv_end_count);
        PRINT_STAT("Reply Error", current_stats.modbus_reply_error, g_networkStatsSnapshot.modbus_reply_error);
        PRINT_STAT("TCPIP Msg", current_stats.tcpip_msg_count, g_networkStatsSnapshot.tcpip_msg_count);

       printf("In Port   : %lu\r\n",
       (unsigned long)g_networkStats.tcp_recvmbox_in_port);

       printf("Fail Port : %lu\r\n",
              (unsigned long)g_networkStats.tcp_recvmbox_fail_port);
        print_lwip_pool_stats();

       printf("[TCP Recv Mailbox]\r\n");
       printf(" In   : %lu\r\n",
              (unsigned long)g_networkStats.tcp_recvmbox_in);
       printf(" Out  : %lu\r\n",
              (unsigned long)g_networkStats.tcp_recvmbox_out);
       printf(" Fail : %lu\r\n",
              (unsigned long)g_networkStats.tcp_recvmbox_fail);
        printf("==================================================\r\n");

        g_networkStatsSnapshot = current_stats;
    }
}

BaseType_t create_task_for_network_stats(uint16_t size, UBaseType_t priority)
{
    return xTaskCreate(network_stats_task,
                       "network_stats_task",
                       size,
                       NULL,
                       priority,
                       &g_networkStatsTaskHandle);
}
