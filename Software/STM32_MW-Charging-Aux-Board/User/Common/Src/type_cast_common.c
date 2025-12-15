#include "type_cast_common.h"
#include <stdint.h>
#include <stdio.h>

void float_to_uint16(float data, uint16_t * uint16)
{
    uint32_t temp = 0;

    memcpy(&temp, &data, sizeof(float));

    /* 组合两个16位成一个32位（Modbus 默认大端，高字在前） */
    uint16[0] = (temp << 16);
    uint16[1] = temp;
}

float uint16_to_float(const uint16_t *data)
{
    uint32_t temp = 0;

    /* 组合两个16位成一个32位（Modbus 默认大端，高字在前） */
    temp |= ((uint32_t)data[0] << 16);
    temp |= ((uint32_t)data[1]);

    float result;
    memcpy(&result, &temp, sizeof(float));
    return result;
}
