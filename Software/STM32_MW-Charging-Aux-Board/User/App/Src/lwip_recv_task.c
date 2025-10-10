#include "lwip_recv_task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include <stdint.h>
#include <stdio.h>
#include <lwip/sockets.h>
#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/api.h"
#include "bsp_debug_usart.h"

int g_sock = -1;
/* 接收数据缓冲区 */
uint8_t g_lwipDemoRecvbuf[LWIP_RX_BUFSIZE]; 
static TaskHandle_t g_lwipRecvTaskHandle = NULL;/* LED任务句柄 */

static void lwip_recv_task(void * param)
{    
		struct sockaddr_in lwipClientAddr;
    err_t err;
    char tBuf[40];
    int recvDataLen;
		vTaskDelay(5000);
lwip_start:     
    lwipClientAddr.sin_family = AF_INET; // 表示IPV4协议
    lwipClientAddr.sin_port = htons(LWIP_DEMO_PORT); // 端口号
    lwipClientAddr.sin_addr.s_addr = inet_addr(IP_ADDR); // 设置远程 IP 地址
    g_sock = socket(AF_INET, SOCK_STREAM, 0); // TCP 协议
    memset(&(lwipClientAddr.sin_zero), 0, sizeof(lwipClientAddr.sin_zero));
	
    // tBuf = mymalloc(SRAMIN, 200);
    sprintf((char *)tBuf, "Port:%d", LWIP_DEMO_PORT); // 客户端端口号
    
    /* 连接远程 IP 地址 */
    err = connect(g_sock, (struct sockaddr *)&lwipClientAddr, sizeof(struct sockaddr));

    if (err == -1)
    {
        printf("Connection failed.\r\n");
        g_sock = -1;
        closesocket(g_sock);
        vTaskDelay(1000);
				goto lwip_start;
    }
    
    while (1)
    {
        recvDataLen = recv(g_sock, g_lwipDemoRecvbuf, LWIP_RX_BUFSIZE, 0);
        mutual_printf("Receive sucess: %.*s\r\n", 20, g_lwipDemoRecvbuf);
        
        if (recvDataLen <= 0 )
        {
            closesocket(g_sock);
            g_sock = -1;
        }

        vTaskDelay(10);
    }
}

BaseType_t create_task_for_lwip_recv(uint16_t size, UBaseType_t priority)
{
	return xTaskCreate((TaskFunction_t )lwip_recv_task, /* 任务入口函数 */
                        (const char*    )"lwip_recv_task", /* 任务名字 */
                        (uint16_t       )size, /* 任务栈大小 */
                        (void*          )NULL, /* 任务入口函数参数 */
                        (UBaseType_t    )priority, /* 任务的优先级 */
                        (TaskHandle_t*  )&g_lwipRecvTaskHandle); /* 任务控制块指针 */ 
}
