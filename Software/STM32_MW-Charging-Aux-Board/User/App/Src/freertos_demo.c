#include "freertos_demo.h"

#include "FreeRTOS.h"
#include "task.h"
#include "lwip_comm.h"
#include "modbus-tcp-server-task.h"
#include <stdio.h>

#define NETWORK_INIT_TASK_PRIORITY  5
#define NETWORK_INIT_TASK_STACK     512
#define MODBUS_TASK_PRIORITY        6
#define MODBUS_TASK_STACK           512

static void network_init_task(void *param)
{
    (void)param;

    while (lwip_comm_init() != 0)
    {
        printf("LwIP initialization failed; retrying\r\n");
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    if (create_task_for_modbus_TCP(MODBUS_TASK_STACK, MODBUS_TASK_PRIORITY) != pdPASS)
    {
        printf("Modbus task creation failed\r\n");
    }

    vTaskDelete(NULL);
}


void freertos_demo(void)
{
    xTaskCreate(network_init_task,
                "network_init_task",
                NETWORK_INIT_TASK_STACK,
                NULL,
                NETWORK_INIT_TASK_PRIORITY,
                NULL);
}
