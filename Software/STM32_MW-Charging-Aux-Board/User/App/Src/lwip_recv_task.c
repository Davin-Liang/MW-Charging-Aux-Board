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
#include "command_task.h"

int g_sock = -1;
/* 接收数据缓冲区 */
uint8_t g_lwipDemoRecvBuf[LWIP_RX_BUFSIZE]; 
uint8_t g_lwipDemoRespBuf[LWIP_RX_BUFSIZE];

//CommandFrame_t receivedCmd;

static TaskHandle_t g_lwipRecvTaskHandle = NULL;/* LED任务句柄 */
static struct CommandInfo command;
static int datetime_to_filename(const DateTime_t *dt, const char *filename, 
                        char *output, size_t output_size);

QueueHandle_t g_motorCmdQueue = NULL;
QueueHandle_t g_commandQueue = NULL;
QueueHandle_t g_findOptCmdQueue = NULL;
QueueHandle_t g_timeDataQueue = NULL;

static void lwip_recv_task(void * param)
{    
    struct sockaddr_in lwipClientAddr;
    err_t err;
    char tBuf[40];
    int recvDataLen;
    vTaskDelay(5000); // TODO:待优化

    g_motorCmdQueue = xQueueCreate(MOTOR_CMD_QUEUE_LEN, sizeof(MotorCmd_t));
    g_commandQueue = xQueueCreate(COMMAND_QUEUE_LEN, sizeof(struct CommandInfo));
    g_findOptCmdQueue = xQueueCreate(OPT_RES_QUEUE_LEN, sizeof(FindOptimalCmd_t));
    g_timeDataQueue = xQueueCreate(TIME_DATA_QUEUE_LEN, sizeof(DateTime_t));

lwip_start:     
    lwipClientAddr.sin_family = AF_INET; // 表示IPV4协议
    lwipClientAddr.sin_port = htons(LWIP_DEMO_PORT); // 端口号
    lwipClientAddr.sin_addr.s_addr = inet_addr(IP_ADDR); // 设置远程 IP 地址
    g_sock = socket(AF_INET, SOCK_STREAM, 0); // TCP 协议
    memset(&(lwipClientAddr.sin_zero), 0, sizeof(lwipClientAddr.sin_zero));
	
    sprintf((char *)tBuf, "Port:%d", LWIP_DEMO_PORT); // 客户端端口号
    
    /* 连接远程 IP 地址 */
    err = connect(g_sock, (struct sockaddr *)&lwipClientAddr, sizeof(struct sockaddr));

    if (err == -1) {
        printf("Connection failed.\r\n");
        g_sock = -1;
        closesocket(g_sock);
        vTaskDelay(1000);
        goto lwip_start;
    }
	
	printf("Connection succeeded.\r\n");
    vTaskResume(find_task_node_by_name("data_sum")->taskHandle);
    
    while (1) {
        recvDataLen = recv(g_sock, g_lwipDemoRecvBuf, LWIP_RX_BUFSIZE, 0);
        
        if (recvDataLen > 0) {
            CommandFrame_t receivedCmd;
            
            /* 解析命令 */
            int parse_result = parse_command_frame(g_lwipDemoRecvBuf, recvDataLen, &receivedCmd);
            if (parse_result == 0) {            
                /* 执行命令 */
                ResponseStatus_t status = STATUS_SUCCESS;
                switch (receivedCmd.header.cmdId) {
                    case CMD_MOTOR_CONTROL:
                        if (receivedCmd.header.dataLen == sizeof(MotorCmd_t)) {
                            printf("Received motor control!");
                            xQueueSend(g_motorCmdQueue, &receivedCmd.payload.motorCmd, 10);
                            command.commandType = demandMotorControl;
                            xQueueSend(g_commandQueue, &command, 10);                          
                        } else 
                            status = STATUS_INVALID_PARAM;
                        break;
                        
                    case CMD_FIND_OPT_RES:
                        if (receivedCmd.header.dataLen == sizeof(FindOptimalCmd_t)) {
                           printf("Received find opt res command!");
                           xQueueSend(g_findOptCmdQueue, &receivedCmd.payload.findOptCmd, 10);
                           command.commandType = demandMutiFindOpt;
                           xQueueSend(g_commandQueue, &command, 10);
                        } else
                            status = STATUS_INVALID_PARAM;
                        break;

                    case CMD_PASS_DATE_TIME:
                        if (receivedCmd.header.dataLen == sizeof(DateTime_t)) {
                            printf("Received pass date command!");
                            char result[20];
                            int len = 30;
                            datetime_to_filename(&receivedCmd.payload.timeData, "test.csv", result, 30);
                            printf("%s\n", result);
                           xQueueSend(g_timeDataQueue, &receivedCmd.payload.timeData, 10);
                        } else
                            status = STATUS_INVALID_PARAM;
                        break;   
                        
                    default:
                        status = STATUS_INVALID_CMD;
                        break;

                // TODO:构建响应帧
                }
            }
        }
        else if (recvDataLen == 0) {
            mutual_printf("Connection break!\r\n");
            break;
        } else {
            printf("Reception Error!\r\n");
            break;
        }

        vTaskDelay(10);
    }

    closesocket(g_sock);
    g_sock = -1;
    goto lwip_start;
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


static int datetime_to_filename(const DateTime_t *dt, const char *filename, 
                        char *output, size_t output_size) 
{
    if (dt == NULL || filename == NULL || output == NULL)
        return -1; // 参数错误
    
//    /* 验证日期时间有效性 */
//    return validate_date_time(dt);
    
    /* 查找文件名中的扩展名 */
    const char *dot = strrchr(filename, '.');
    const char *name_part = filename;
    const char *ext_part = "";
    
    if (dot != NULL) 
    {
        /* 分离文件名和扩展名 */
        size_t name_len = dot - filename;
        ext_part = dot; // 包含点号的扩展名
    }
    
    /* 构建新文件名: 文件名_YYYYMMDD_HHMMSS.扩展名 */
    int result = snprintf(output, output_size, "%.*s_%04u%02u%02u_%02u%02u%s",
                         (int)(dot ? (dot - filename) : strlen(filename)),
                         name_part,
                         dt->year, dt->month, dt->day,
                         dt->hour, dt->minute,
                         ext_part);
    
    if (result < 0 || (size_t)result >= output_size)
        return -3; // 缓冲区不足或格式化错误
    
    return 0; // 成功
}

