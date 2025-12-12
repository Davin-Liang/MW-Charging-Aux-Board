#ifndef __MODBUS_TCP_SERVER_TASK_H
#define	__MODBUS_TCP_SERVER_TASK_H

#include "FreeRTOS.h"
#include "command_struct.h"
#include "modbus.h"

BaseType_t create_task_for_modbus_TCP(uint16_t size, UBaseType_t priority);
struct DataCenter_t * get_data_center(void);
modbus_mapping_t * get_mbMapping(void);

#endif
