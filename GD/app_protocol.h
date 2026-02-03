#ifndef __APP_PROTOCOL_H
#define __APP_PROTOCOL_H

#include <stdint.h>

/* CAN ID 定义 */
#define CAN_ID_CMD_RX     0x201 // 接收控制指令的ID
#define CAN_ID_FB_TX      0x202 // 发送位置反馈的ID

/* 数据转换联合体 */
typedef union {
    float fVal;
    uint8_t bytes[4];
} FloatByte_u;

/* 位置数据结构体 */
typedef struct {
    float x;
    float y;
} Coordinate_t;

#endif