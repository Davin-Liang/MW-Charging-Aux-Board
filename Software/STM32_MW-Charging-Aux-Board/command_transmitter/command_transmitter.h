#ifndef __COMMAND_TRANSMITTER_H
#define __COMMAND_TRANSMITTER_H

#include <string>
#include <vector>
#include "command_struct.h"

class CommandTransmitter {
public:
    /* 构造函数和析构函数 */
    CommandTransmitter();
    virtual ~CommandTransmitter();

    int build_command_frame(uint8_t* buffer, CommandType_t cmdType, 
                       uint16_t seqNum, const void* data, uint16_t dataLen);
    int parse_command_frame(const uint8_t * buffer, uint16_t len, CommandFrame_t * cmd);
    int send_motor_command(int sock, float x, float y, uint16_t speed = 0);
    int send_motor_command(void);
    int send_find_opt_command(int sock, ThajType_t whichThaj, 
                                float cirTrajRad, uint8_t squThajStepLen, 
                                float maxVolfloat volStepLen);
    int send_find_opt_command(void);

private:
    uint8_t calculate_checksum(const uint8_t* data, uint16_t len);

    /* 私有成员变量 */
    MotorCmd_t motorCmd;
    FindOptimalCmd_t findOptCmd;
    MotorData_t motorData;
    DateTime_t timeData;
};

#endif // __COMMAND_TRANSMITTER_H
