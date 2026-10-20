
#include "modbus-tcp-server-task.h"

#include "FreeRTOS.h"
#include "task.h"
#include "lwip/sockets.h"
#include "lwip/opt.h"       // 新增：lwIP Socket 相关配置宏
#include "modbus.h"
#include <stdio.h>

#include "network_stats.h"

#define SERVER_IP       "192.168.1.30"
#define SERVER_PORT     8080
#define COIL_COUNT      16
#define REGISTER_COUNT  16

// 新增：限制不完整 Modbus 请求导致的 recv 阻塞时间
#define MODBUS_RECV_TIMEOUT_MS 1000

static TaskHandle_t g_modbusTCPServerTaskHandle = NULL;

static void modbus_tcp_server_task(void *param) {
    modbus_t *ctx;
    modbus_mapping_t *mapping;
    int server_socket;

    // 新增：记录当前唯一允许处理 Modbus 请求的客户端 Socket
    // -1 表示当前没有活动客户端
    int active_socket = -1;

    uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];

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

    server_socket = modbus_tcp_listen(ctx, 2);
    if (server_socket < 0) {
        printf("Modbus listen failed\r\n");
        modbus_mapping_free(mapping);
        modbus_free(ctx);
        vTaskDelete(NULL);
        return;
    }

    printf("Modbus TCP demo started at %s:%d\r\n",
           SERVER_IP, SERVER_PORT);

    for (;;) {
        fd_set readfds;
        struct timeval timeout;
        int maxfd = server_socket;
        int ret;

        // 新增：构造 select() 监听集合
        // 同时监听服务端 Socket 和当前活动客户端 Socket
        FD_ZERO(&readfds);
        FD_SET(server_socket, &readfds);

        if (active_socket >= 0) {
            FD_SET(active_socket, &readfds);

            if (active_socket > maxfd) maxfd = active_socket;
        }

        // 新增：select 最多阻塞 200ms
        // 没有网络事件时返回，避免任务一直阻塞在 select
        timeout.tv_sec = 0;
        timeout.tv_usec = 200000;

        ret = lwip_select(maxfd + 1, &readfds, NULL, NULL, &timeout);

        if (ret < 0) {
            printf("Modbus select error\r\n");
            vTaskDelay(pdMS_TO_TICKS(10));
            continue;
        }

        if (ret == 0) continue;

        // 新增：监听 Socket 可读，表示有连接等待 accept
        if (FD_ISSET(server_socket, &readfds)) {
            int new_socket = lwip_accept(server_socket, NULL, NULL);

            if (new_socket < 0) {
                g_networkStats.tcp_accept_error++;
            }
            else if (active_socket < 0) {
                // 当前没有客户端：允许新连接成为活动客户端
                active_socket = new_socket;

                // 告诉 libmodbus 后续收发使用哪个 Socket
                modbus_set_socket(ctx, active_socket);

#if LWIP_SO_RCVTIMEO
                // 新增：设置 Socket 接收超时
                // 防止客户端只发送部分 Modbus 报文，导致任务长时间阻塞
#if LWIP_SO_SNDRCVTIMEO_NONSTANDARD
                int recv_timeout = MODBUS_RECV_TIMEOUT_MS;
#else
                struct timeval recv_timeout = {
                    MODBUS_RECV_TIMEOUT_MS / 1000,
                    (MODBUS_RECV_TIMEOUT_MS % 1000) * 1000
                };
#endif
                if (lwip_setsockopt(active_socket, SOL_SOCKET,
                                    SO_RCVTIMEO, &recv_timeout,
                                    sizeof(recv_timeout)) < 0) {
                    printf("Set recv timeout failed\r\n");
                }
#endif

                g_networkStats.tcp_connect_count++;
                printf("Modbus client connected\r\n");
            } else {
                // 核心优化：已有活动客户端 A 时，及时 accept 并关闭 B
                // 避免 B 长期滞留在等待队列中，导致 pbuf 接收资源积压
                lwip_close(new_socket);
                printf("Extra Modbus client rejected\r\n");
            }
        }

        // 新增：活动客户端可读时，才调用 modbus_receive()
        // 避免长期阻塞接收 A，导致无法处理额外客户端 B
        if (active_socket >= 0 &&
            FD_ISSET(active_socket, &readfds)) {

            modbus_set_socket(ctx, active_socket);

            int rc = modbus_receive(ctx, query);

            if (rc > 0) {
                g_networkStats.modbus_req_count++;

                int reply_ret = modbus_reply(ctx, query, rc, mapping);

                if (reply_ret < 0)
                    g_networkStats.modbus_reply_error++;
            }
            else {
                // A 断开或接收异常：清理 Socket，允许下一个客户端接入
                g_networkStats.modbus_recv_end_count++;

                lwip_close(active_socket);
                active_socket = -1;

                // 同步清除 libmodbus 保存的旧 Socket
                modbus_set_socket(ctx, -1);

                g_networkStats.tcp_disconnect_count++;
                printf("Modbus client disconnected\r\n");
            }
        }
    }
}

BaseType_t create_task_for_modbus_TCP(uint16_t size,
                                      UBaseType_t priority) {
    return xTaskCreate(modbus_tcp_server_task,
                       "modbus_tcp_server_task",
                       size,
                       NULL,
                       priority,
                       &g_modbusTCPServerTaskHandle);
}


// #include "modbus-tcp-server-task.h"

// #include "FreeRTOS.h"
// #include "task.h"
// #include "lwip/sockets.h"
// #include "modbus.h"
// #include <stdio.h>

// #include "network_stats.h"

// #define SERVER_IP       "192.168.1.30"
// #define SERVER_PORT     8080
// #define COIL_COUNT      16
// #define REGISTER_COUNT  16

// static TaskHandle_t g_modbusTCPServerTaskHandle = NULL;

// static void modbus_tcp_server_task(void *param) {
//     modbus_t *ctx;
//     modbus_mapping_t *mapping;
//     int server_socket;

//     (void)param;

//     ctx = modbus_new_tcp(SERVER_IP, SERVER_PORT);
//     if (ctx == NULL) {
//         printf("Modbus context creation failed\r\n");
//         vTaskDelete(NULL);
//         return;
//     }

//     mapping = modbus_mapping_new(COIL_COUNT, COIL_COUNT,
//                                  REGISTER_COUNT, REGISTER_COUNT);
//     if (mapping == NULL) {
//         printf("Modbus mapping creation failed\r\n");
//         modbus_free(ctx);
//         vTaskDelete(NULL);
//         return;
//     }

//     mapping->tab_registers[0] = 0x1234;
//     mapping->tab_input_registers[0] = 0x5678;

//     server_socket = modbus_tcp_listen(ctx, 2);
//     if (server_socket < 0) {
//         printf("Modbus listen failed\r\n");
//         modbus_mapping_free(mapping);
//         modbus_free(ctx);
//         vTaskDelete(NULL);
//         return;
//     }

//     printf("Modbus TCP demo started at %s:%d\r\n", SERVER_IP, SERVER_PORT);

//     for (;;) {
//         uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];
//         int client_socket = modbus_tcp_accept(ctx, &server_socket);

//         if (client_socket < 0) {
//             g_networkStats.tcp_accept_error++;  // 接受客户端连接失败

//             continue;
//         }

//         g_networkStats.tcp_connect_count++;     // 客户端接入成功

//         printf("Modbus client connected\r\n");
//         for (;;) {
//             int rc = modbus_receive(ctx, query);

//             if (rc <= 0) {
//                 g_networkStats.modbus_recv_end_count++; // 接收结束或失败

//                 break;
//             }

//             g_networkStats.modbus_req_count++;  // 收到一条完整 Modbus 请求

//             int ret = modbus_reply(ctx, query, rc, mapping);

//             if (ret < 0)
//                 g_networkStats.modbus_reply_error++; // Modbus 响应失败
//         }

//         lwip_close(client_socket);

//         g_networkStats.tcp_disconnect_count++;  // 当前客户端连接处理结束

//         printf("Modbus client disconnected\r\n");
//     }
// }


// BaseType_t create_task_for_modbus_TCP(uint16_t size, UBaseType_t priority) {
//     return xTaskCreate(modbus_tcp_server_task,
//                        "modbus_tcp_server_task",
//                        size,
//                        NULL,
//                        priority,
//                        &g_modbusTCPServerTaskHandle);
// }
