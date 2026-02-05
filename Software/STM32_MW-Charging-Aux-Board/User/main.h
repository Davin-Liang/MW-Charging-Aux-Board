#ifndef __MAIN_H
#define	__MAIN_H

#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"

#define SD_NOTE 0 // 是否使用 SD 记录
#define USE_DM542 1 // 是否使用 DM542 控制的步进电机
#define USE_ZHENGDIAN 1 // 是否在使用正点原子的板子
#define RESET_MOTOR_DATA 1 // 是否要复位 flash 中电机的数据
#define ONLY_TEST 0
#define USE_POWER_METER 1 // 是否使用功率计
#define USE_POWER_SUPPLY 1 // 是否使用电源
#define USE_COMMAND_CONTROL 1 // 是否使用命令控制
#define USE_DATA_SUM 1 

#define BIT_WAKE_MODBUS_TCP ( 1 << 0 )
#define BIT_WAKE_COMMAND_TASK ( 1 << 1 )
#define BIT_WAKE_POWER_SUPPLY_TASK ( 1 << 2 )
#define BIT_WAKE_DATA_SUM_TASK ( 1 << 3 )
#define BIT_WAKE_DM542_TASK ( 1 << 4 )
#define BIT_WAKE_ALL (BIT_WAKE_MODBUS_TCP | BIT_WAKE_COMMAND_TASK | BIT_WAKE_POWER_SUPPLY_TASK | BIT_WAKE_DATA_SUM_TASK | BIT_WAKE_DM542_TASK)
extern EventGroupHandle_t xSystemEventGroup;

#endif
