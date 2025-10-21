#ifndef __LWIP_RECV_TASK_H
#define	__LWIP_RECV_TASK_H

#include "FreeRTOS.h"
#include "queue.h"

#define LWIP_DEMO_PORT          8080 // 本地端口号
#define IP_ADDR                 "192.168.1.100" // 远程 IP 地址
#define LWIP_RX_BUFSIZE    128 // 最大接收数据长度
#define MOTOR_CMD_QUEUE_LEN 10
#define COMMAND_QUEUE_LEN 10
#define OPT_RES_QUEUE_LEN 10
#define TIME_DATA_QUEUE_LEN 10

extern QueueHandle_t g_motorCmdQueue;
extern QueueHandle_t g_commandQueue;
extern QueueHandle_t g_findOptCmdQueue;
extern QueueHandle_t g_timeDataQueue;

BaseType_t create_task_for_lwip_recv(uint16_t size, UBaseType_t priority);

#endif
