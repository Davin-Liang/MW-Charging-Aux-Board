#ifndef __APP_PROTOCOL_H
#define __APP_PROTOCOL_H

#include <stdint.h>

// CAN ID 定义
#define CAN_ID_CMD_RX     0x201 // GD32接收指令的ID (STM32发送的目标ID)
#define CAN_ID_FB_TX      0x202 // GD32发送反馈的ID (STM32接收的ID)

// 数据联合体（方便 float 和 byte 互转）
typedef union {
    float fVal;
    uint8_t bytes[4];
} FloatByte_u;

// 坐标结构体
typedef struct {
    float x;
    float y;
} Coordinate_t;

#endif