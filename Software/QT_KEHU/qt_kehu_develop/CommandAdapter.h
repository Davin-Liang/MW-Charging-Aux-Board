#ifndef COMMAND_ADAPTER_H
#define COMMAND_ADAPTER_H

#include "command_struct.h"
#include <QByteArray>

class CommandAdapter
{
public:
    CommandAdapter();

    QByteArray buildMotorCommand(float x, float y, uint16_t speed);
    QByteArray buildFindOptimalCommand(uint8_t whichThaj, float cirTrajRad,
                                       uint8_t squThajStepLen, float maxVol, float volStepLen);
    QByteArray buildReadCommand(CommandType_t readCmd);
    QByteArray buildTimeCommand();

    bool parseCommandFrame(const QByteArray &data, CommandFrame_t &frame);

private:
    uint8_t calculateChecksum(const uint8_t* data, uint16_t len);
    void setCurrentTime(DateTime_t &timeData);

    MotorCmd_t motorCmd;
    FindOptimalCmd_t findOptCmd;
    DateTime_t timeData;
};

#endif // COMMAND_ADAPTER_H
