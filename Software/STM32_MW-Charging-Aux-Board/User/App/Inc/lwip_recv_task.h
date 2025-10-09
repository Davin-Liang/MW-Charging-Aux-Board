#ifndef __LWIP_RECV_TASK_H
#define	__LWIP_RECV_TASK_H

#include "FreeRTOS.h"

#define LWIP_DEMO_PORT          8080 // ���ض˿ں�
#define IP_ADDR                 "192.168.1.100" // Զ�� IP ��ַ
#define LWIP_RX_BUFSIZE    20 // ���������ݳ���

BaseType_t create_task_for_lwip_recv(uint16_t size, UBaseType_t priority);

#endif
