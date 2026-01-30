// command_builder.cpp
#include "command_builder.h"
#include <QDebug>
#include <cstring>

CommandBuilder::CommandBuilder()
{
    // 初始化默认参数
    defaultMotorCmd.x = 0.0f;
    defaultMotorCmd.y = 0.0f;
    defaultMotorCmd.speed = 100;

    defaultFindOptCmd.whichThaj = SQU_TRAJ;
    defaultFindOptCmd.cirTrajRad = 1.0f;
    defaultFindOptCmd.squThajStepLen = 10;
    defaultFindOptCmd.maxVol = 5.0f;
    defaultFindOptCmd.volStepLen = 0.1f;
}

QByteArray CommandBuilder::buildMotorCommand(float x, float y, uint16_t speed)
{
    MotorCmd_t cmd;
    cmd.x = x;
    cmd.y = y;
    cmd.speed = speed;

    CommandFrame_t frame;
    frame.header.startMagic = 0xAA;
    frame.header.cmdId = CMD_MOTOR_CONTROL;
    frame.header.dataLen = sizeof(MotorCmd_t);
    std::memcpy(&frame.payload.motorCmd, &cmd, sizeof(MotorCmd_t));
    frame.header.checksum = calculateChecksum(reinterpret_cast<uint8_t*>(&frame.header),
                                              sizeof(CmdHeader_t) - 1);

    return QByteArray(reinterpret_cast<char*>(&frame), sizeof(CmdHeader_t) + sizeof(MotorCmd_t));
}

QByteArray CommandBuilder::buildFindOptimalCommand(uint8_t whichThaj, float cirTrajRad,
                                                   uint8_t squThajStepLen, float maxVol, float volStepLen)
{
    FindOptimalCmd_t cmd;
    cmd.whichThaj = whichThaj;
    cmd.cirTrajRad = cirTrajRad;
    cmd.squThajStepLen = squThajStepLen;
    cmd.maxVol = maxVol;
    cmd.volStepLen = volStepLen;

    CommandFrame_t frame;
    frame.header.startMagic = 0xAA;
    frame.header.cmdId = CMD_FIND_OPT_RES;
    frame.header.dataLen = sizeof(FindOptimalCmd_t);
    std::memcpy(&frame.payload.findOptCmd, &cmd, sizeof(FindOptimalCmd_t));
    frame.header.checksum = calculateChecksum(reinterpret_cast<uint8_t*>(&frame.header),
                                              sizeof(CmdHeader_t) - 1);

    return QByteArray(reinterpret_cast<char*>(&frame), sizeof(CmdHeader_t) + sizeof(FindOptimalCmd_t));
}

QByteArray CommandBuilder::buildReadCommand(CommandType_t readCmd)
{
    CommandFrame_t frame;
    frame.header.startMagic = 0xAA;
    frame.header.cmdId = readCmd;
    frame.header.dataLen = 0;
    frame.header.checksum = calculateChecksum(reinterpret_cast<uint8_t*>(&frame.header),
                                              sizeof(CmdHeader_t) - 1);

    return QByteArray(reinterpret_cast<char*>(&frame), sizeof(CmdHeader_t));
}

QByteArray CommandBuilder::buildTimeCommand()
{
    DateTime_t timeData;
    setCurrentTime(timeData);

    CommandFrame_t frame;
    frame.header.startMagic = 0xAA;
    frame.header.cmdId = CMD_PASS_DATE_TIME;
    frame.header.dataLen = sizeof(DateTime_t);
    std::memcpy(&frame.payload.timeData, &timeData, sizeof(DateTime_t));
    frame.header.checksum = calculateChecksum(reinterpret_cast<uint8_t*>(&frame.header),
                                              sizeof(CmdHeader_t) - 1);

    return QByteArray(reinterpret_cast<char*>(&frame), sizeof(CmdHeader_t) + sizeof(DateTime_t));
}

bool CommandBuilder::parseCommandFrame(const QByteArray &data, CommandFrame_t &frame)
{
    if (data.size() < static_cast<int>(sizeof(CmdHeader_t))) {
        return false;
    }

    // 拷贝数据到结构体
    std::memcpy(&frame, data.constData(), sizeof(CmdHeader_t));

    // 验证起始标志
    if (frame.header.startMagic != 0xAA) {
        return false;
    }

    // 验证数据长度
    if (data.size() < static_cast<int>(sizeof(CmdHeader_t) + frame.header.dataLen)) {
        return false;
    }

    // 拷贝数据部分
    if (frame.header.dataLen > 0) {
        std::memcpy(&frame.payload, data.constData() + sizeof(CmdHeader_t), frame.header.dataLen);
    }

    return true;
}

uint8_t CommandBuilder::calculateChecksum(const uint8_t* data, uint16_t len)
{
    uint8_t sum = 0;
    for (uint16_t i = 0; i < len; i++) {
        sum += data[i];
    }
    return ~sum + 1;
}

void CommandBuilder::setCurrentTime(DateTime_t &timeData)
{
    std::time_t currentTime = std::time(nullptr);
    std::tm* localTime = std::localtime(&currentTime);

    timeData.year = localTime->tm_year + 1900;
    timeData.month = localTime->tm_mon + 1;
    timeData.day = localTime->tm_mday;
    timeData.hour = localTime->tm_hour;
    timeData.minute = localTime->tm_min;
    timeData.week_day = localTime->tm_wday;
}
