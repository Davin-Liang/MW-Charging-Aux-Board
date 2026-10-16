#include "modbus-tcp-server-task.h"

#include "FreeRTOS.h"
#include "task.h"
#include "lwip/sockets.h"
#include "modbus.h"
#include <stdio.h>

#include "network_stats.h"

#define SERVER_IP       "192.168.1.30"
#define SERVER_PORT     8080
#define COIL_COUNT      16
#define REGISTER_COUNT  16

static TaskHandle_t g_modbusTCPServerTaskHandle = NULL;

static void modbus_tcp_server_task(void *param) {
    modbus_t *ctx;
    modbus_mapping_t *mapping;
    int server_socket;

    (void)param;

    ctx = modbus_new_tcp(SERVER_IP, SERVER_PORT);
    if (ctx == NULL) {
        printf("Modbus context creation failed\r\n");
        vTaskDelete(NULL);
        return;
    }

    mapping = modbus_mapping_new(COIL_COUNT, COIL_COUNT,
                                 REGISTER_COUNT, REGISTER_COUNT);
    if (mapping == NULL) {
        printf("Modbus mapping creation failed\r\n");
        modbus_free(ctx);
        vTaskDelete(NULL);
        return;
    }

    mapping->tab_registers[0] = 0x1234;
    mapping->tab_input_registers[0] = 0x5678;

    server_socket = modbus_tcp_listen(ctx, 1);
    if (server_socket < 0) {
        printf("Modbus listen failed\r\n");
        modbus_mapping_free(mapping);
        modbus_free(ctx);
        vTaskDelete(NULL);
        return;
    }

    printf("Modbus TCP demo started at %s:%d\r\n", SERVER_IP, SERVER_PORT);

    for (;;) {
        uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];
        int client_socket = modbus_tcp_accept(ctx, &server_socket);

        if (client_socket < 0) {
            g_networkStats.tcp_accept_error++;  // 接受客户端连接失败

            continue;
        }

        g_networkStats.tcp_connect_count++;     // 客户端接入成功

        printf("Modbus client connected\r\n");
        for (;;) {
            int rc = modbus_receive(ctx, query);

            if (rc <= 0) {
                g_networkStats.modbus_recv_end_count++; // 接收结束或失败

                break;
            }

            g_networkStats.modbus_req_count++;  // 收到一条完整 Modbus 请求

            int ret = modbus_reply(ctx, query, rc, mapping);

            if (ret < 0)
                g_networkStats.modbus_reply_error++; // Modbus 响应失败
        }

        lwip_close(client_socket);

        g_networkStats.tcp_disconnect_count++;  // 当前客户端连接处理结束

        printf("Modbus client disconnected\r\n");
    }
}


BaseType_t create_task_for_modbus_TCP(uint16_t size, UBaseType_t priority) {
    return xTaskCreate(modbus_tcp_server_task,
                       "modbus_tcp_server_task",
                       size,
                       NULL,
                       priority,
                       &g_modbusTCPServerTaskHandle);
}
