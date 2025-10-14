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
#include "command_struct.h"
#include "command.h"

int g_sock = -1;
/* 接收数据缓冲区 */
uint8_t g_lwipDemoRecvBuf[LWIP_RX_BUFSIZE]; 
uint8_t g_lwipDemoRespBuf[LWIP_RX_BUFSIZE];

static TaskHandle_t g_lwipRecvTaskHandle = NULL;/* LED任务句柄 */
static struct CommandInfo command;

QueueHandle_t g_motorCmdQueue = NULL;
QueueHandle_t g_commandQueue = NULL;
QueueHandle_t g_optResDataQueue = NULL;
QueueHandle_t g_timeDataQueue = NULL;

static void lwip_recv_task(void * param)
{    
    struct sockaddr_in lwipClientAddr;
    err_t err;
    char tBuf[40];
    int recvDataLen;
    vTaskDelay(5000);

    g_motorCmdQueue = xQueueCreate(MOTOR_CMD_QUEUE_LEN, sizeof(MotorCmd_t));
    g_commandQueue = xQueueCreate(COMMAND_QUEUE_LEN, sizeof(struct CommandInfo));
    g_optResDataQueue = xQueueCreate(OPT_RES_QUEUE_LEN, sizeof(struct CommandInfo));
    g_timeDataQueue = xQueueCreate(TIME_DATA_QUEUE_LEN, sizeof(DateTime_t));

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
        recvDataLen = recv(g_sock, g_lwipDemoRecvBuf, LWIP_RX_BUFSIZE, 0);
        // mutual_printf("Receive sucess: %.*s\r\n", 20, g_lwipDemoRecvbuf);
        
        if (recvDataLen > 0)
        {
            CommandFrame_t receivedCmd;
            
            /* 解析命令 */
            int parse_result = parse_command_frame(g_lwipDemoRecvBuf, recvDataLen, &receivedCmd);
            if(parse_result == 0) 
            {
                // uint16_t respDataLen = 0;
                // uint8_t respData[LWIP_RX_BUFSIZE];
                
                /* 执行命令 */
                // ResponseStatus_t status = execute_command(&receivedCmd, respData, &respDataLen);
                ResponseStatus_t status = STATUS_SUCCESS;
                switch(receivedCmd.header.cmdId) 
                {
                    case CMD_MOTOR_CONTROL:
                        if(receivedCmd.header.dataLen == sizeof(MotorCmd_t))
                        {
                            xQueueSend(g_motorCmdQueue, &receivedCmd.payload.motorCmd, 10);
                            command.commandType = demandMotorControl;
                            xQueueSend(g_commandQueue, &command, 10);                          
                        }

                        else 
                            status = STATUS_INVALID_PARAM;
                        
                        break;
                        
                    case CMD_FIND_OPT_RES:
                        if(receivedCmd.header.dataLen == sizeof(OptResData_t))     
                        {
                            xQueueSend(g_optResDataQueue, &receivedCmd.payload.findOptCmd, 10);
                            // TODO:完善 g_optResDataQueue 队列的接收部分
                            command.commandType = demandTwo;
                             xQueueSend(g_commandQueue, &command, 10);
                        }
                        else
                            status = STATUS_INVALID_PARAM;
                        
                        break;

                    case CMD_PASS_DATE_TIME:
                        if(receivedCmd.header.dataLen == sizeof(DateTime_t))     
                        {
                            xQueueSend(g_timeDataQueue, &receivedCmd.payload.timeData, 10);
                        }
                        else
                            status = STATUS_INVALID_PARAM;
                        
                        break;   
                        
                    default:
                        status = STATUS_INVALID_CMD;
                        break;
    

                // TODO:构建响应帧
                }
            }
        }
        else if (recvDataLen == 0)
        {
            mutual_printf("Connection break!\r\n");
            break;
        }
        else
        {
            printf("Reception Error!\r\n");
            break;
        }


        vTaskDelay(10);
    }

    closesocket(g_sock);
    g_sock = -1;
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
