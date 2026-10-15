#ifndef __MODBUS_TCP_SERVER_TASK_H
#define __MODBUS_TCP_SERVER_TASK_H

#include "FreeRTOS.h"

BaseType_t create_task_for_modbus_TCP(uint16_t size, UBaseType_t priority);

#endif

