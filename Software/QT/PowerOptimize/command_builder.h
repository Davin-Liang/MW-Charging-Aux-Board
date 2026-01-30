// command_builder.h
#ifndef COMMAND_BUILDER_H
#define COMMAND_BUILDER_H

#include "command_struct.h"
#include <QByteArray>
#include <ctime>

class CommandBuilder
{
public:
    CommandBuilder();

    // 构建各种命令帧
    QByteArray buildMotorCommand(float x, float y, uint16_t speed);
    QByteArray buildFindOptimalCommand(uint8_t whichThaj, float cirTrajRad,
                                       uint8_t squThajStepLen, float maxVol, float volStepLen);
    QByteArray buildReadCommand(CommandType_t readCmd);
    QByteArray buildTimeCommand();

    // 解析接收到的命令帧
    bool parseCommandFrame(const QByteArray &data, CommandFrame_t &frame);

    // 获取默认参数
    MotorCmd_t getDefaultMotorCmd() const { return defaultMotorCmd; }
    FindOptimalCmd_t getDefaultFindOptCmd() const { return defaultFindOptCmd; }

private:
    MotorCmd_t defaultMotorCmd;
    FindOptimalCmd_t defaultFindOptCmd;

    uint8_t calculateChecksum(const uint8_t* data, uint16_t len);
    void setCurrentTime(DateTime_t &timeData);
};

#endif // COMMAND_BUILDER_H
