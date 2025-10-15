#ifndef __LWIP_RECV_TASK_H
#define	__LWIP_RECV_TASK_H

#include "FreeRTOS.h"

#define LWIP_DEMO_PORT          8080 // 本地端口号
#define IP_ADDR                 "192.168.1.100" // 远程 IP 地址
#define LWIP_RX_BUFSIZE    20 // 最大接收数据长度

BaseType_t create_task_for_lwip_recv(uint16_t size, UBaseType_t priority);

#endif
