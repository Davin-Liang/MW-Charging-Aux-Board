#include "modbus-tcp-server-task.h"
#include "lwip/ip4_addr.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "ethernetif.h"
#include "lwip/sockets.h"
/* FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "event_groups.h"
#include <stdio.h>
#include "type_cast_common.h"
/* Peripherals */
// #include "enet.h"
/* libmodbus */
#include "modbus.h"
#include "command_struct.h"
#include "command.h"
#include "queues-create-task.h"
#include "data_sum_task.h"
#include "main.h"

#define SERVER_IP   "192.168.1.30"
#define SERVER_PORT 8080

static TaskHandle_t g_modbusTCPServerTaskHandle = NULL;
static struct DataCenter_t * g_dataCenter;
static struct CommandInfo command;
static struct SystemQueues_t * queues;
static  modbus_mapping_t * mbMapping;

static void modbus_tcp_server_task(void* param)
{
    xEventGroupWaitBits(xSystemEventGroup, BIT_WAKE_MODBUS_TCP, pdTRUE, pdTRUE, portMAX_DELAY);
    
    g_dataCenter = malloc_data_center();
    queues = get_queues();
    
    /* 创建 Modbus TCP 从机 */
    modbus_t *ctx = modbus_new_tcp(SERVER_IP, SERVER_PORT);
    if (ctx == NULL) {
        printf("Failed to create context\n");
        vTaskDelete(NULL);
    }
		
    modbus_set_debug(ctx, 0);
    modbus_set_slave(ctx, 1);
    modbus_set_byte_timeout(ctx, 0, 500000);
    modbus_set_response_timeout(ctx, 0, 500000);
    modbus_flush(ctx);

    /* 创建寄存器映射 */
    mbMapping = modbus_mapping_new(3, 0, 20, 22);
    if (mbMapping == NULL) {
        printf("Failed to create mapping\n");
        modbus_free(ctx);
        vTaskDelete(NULL);
    }

    /* 打开 TCP 监听 */
    int serverSocket = modbus_tcp_listen(ctx, 1);
    if (serverSocket < 0) {
        printf("Listen failed\n");
        modbus_mapping_free(mbMapping);
        modbus_free(ctx);
        vTaskDelete(NULL);
    }

    printf("Modbus TCP slave started at %s:%d\n", SERVER_IP, SERVER_PORT);

    while (1) {
        /* 等待客户端连接 */
        int clientSocket = modbus_tcp_accept(ctx, &serverSocket); // 连不上会一直堵塞
        if (clientSocket < 0) {
            printf("Accept failed\n");
            continue;
        }

        printf("Client connected.\n");

        while (1) {
            uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];
            int rc = modbus_receive(ctx, query);
            
            if (rc > 0) {
                modbus_reply(ctx, query, rc, mbMapping);
                /* 获取从上位机发送的保持寄存器的值 */
                g_dataCenter->dataUpdateFlag = mbMapping->tab_registers[0x0011];
                if (check_data_update_flag(get_data_update_flag(g_dataCenter), 0)) {
                    printf("Get motor cmd.\n");
                    g_dataCenter->motorCmd->x     = (int16_t)mbMapping->tab_registers[0x0001];
										g_dataCenter->motorCmd->y     = (int16_t)mbMapping->tab_registers[0x0002];
                    g_dataCenter->motorCmd->speed = mbMapping->tab_registers[0x0003];

                    g_dataCenter->dataUpdateFlag = 0;
                    mbMapping->tab_registers[0x0011] &= ~(1 << 0);
                }
                if (check_data_update_flag(get_data_update_flag(g_dataCenter), 1)) {
                    printf("Get find optimal cmd.\n");
                    g_dataCenter->findOptCmd->whichThaj      = mbMapping->tab_registers[0x0004];
                    g_dataCenter->findOptCmd->cirTrajRad     = mbMapping->tab_registers[0x0005];
                    g_dataCenter->findOptCmd->squThajLen     = mbMapping->tab_registers[0x0006];
                    g_dataCenter->findOptCmd->squThajStepLen = mbMapping->tab_registers[0x0007];
                    g_dataCenter->findOptCmd->maxVol     = uint16_to_float(&mbMapping->tab_registers[0x0008]);
                    g_dataCenter->findOptCmd->volStepLen = uint16_to_float(&mbMapping->tab_registers[0x000A]);
                    g_dataCenter->findOptCmd->initialVol = uint16_to_float(&mbMapping->tab_registers[0x000C]);

                    g_dataCenter->dataUpdateFlag = 0;
                    mbMapping->tab_registers[0x0011] &= ~(1 << 1);
                }
                if (check_data_update_flag(get_data_update_flag(g_dataCenter), 2)) {
                    printf("Get time.\n");
                    g_dataCenter->timeData->year   = mbMapping->tab_registers[0x000E];
                    g_dataCenter->timeData->month  = (uint8_t)mbMapping->tab_registers[0x000F];
                    g_dataCenter->timeData->day    = (uint8_t)(mbMapping->tab_registers[0x000F] >> 8);
                    g_dataCenter->timeData->hour   = (uint8_t)mbMapping->tab_registers[0x0010];
                    g_dataCenter->timeData->minute = (uint8_t)(mbMapping->tab_registers[0x0010] >> 8);

                    g_dataCenter->dataUpdateFlag = 0;
                    mbMapping->tab_registers[0x0011] &= ~(1 << 2);
                }

                /* 获取从上位机发送的线圈的值 */
                if (mbMapping->tab_bits[0x0001] != 0) {
                    printf("Received find opt res command!");
                    uint8_t cmd = CMD_FIND_OPT_RES;
                    xQueueSend(queues->cmdQueue, &cmd, 10);
                    command.commandType = demandMutiFindOpt;
                    xQueueSend(queues->commandQueue, &command, 10);                   

                    mbMapping->tab_bits[0x0001] = 0;
                }
                if (mbMapping->tab_bits[0x0002] != 0) {
                    printf("Received motor control!");
                    uint8_t cmd = CMD_MOTOR_CONTROL;
                    xQueueSend(queues->cmdQueue, &cmd, 10);
                    command.commandType = demandMotorControl;
                    xQueueSend(queues->commandQueue, &command, 10);    
                        
                    mbMapping->tab_bits[0x0002] = 0;
                }
                if (mbMapping->tab_bits[0x0003] != 0) {
                    printf("Received pass date command!");
                        
                    mbMapping->tab_bits[0x0003] = 0;
                }
                
            }
            else {
                /* 客户端断开 */
                printf("Client disconnected.\n");
                lwip_close(clientSocket);   // 防止句柄泄漏
                break;                       // 等待下一个客户端
            }
        }
    }

    /* 永不到达，但写着规范 */
    lwip_close(serverSocket);
    modbus_mapping_free(mbMapping);
    modbus_free(ctx);
    vPortFree(g_dataCenter);

    vTaskDelete(NULL);
}

BaseType_t create_task_for_modbus_TCP(uint16_t size, UBaseType_t priority)
{
	return xTaskCreate((TaskFunction_t )modbus_tcp_server_task,  /* 任务入口函数 */
                        (const char*    )"modbus_tcp_server_task",/* 任务名字 */
                        (uint16_t       )size,  /* 任务栈大小 */
                        (void*          )NULL,/* 任务入口函数参数 */
                        (UBaseType_t    )priority, /* 任务的优先级 */
                        (TaskHandle_t*  )&g_modbusTCPServerTaskHandle); /* 任务控制块指针 */ 
}

struct DataCenter_t * get_data_center(void)
{
    return g_dataCenter;
}

modbus_mapping_t * get_mbMapping(void)
{
    return mbMapping;
}
