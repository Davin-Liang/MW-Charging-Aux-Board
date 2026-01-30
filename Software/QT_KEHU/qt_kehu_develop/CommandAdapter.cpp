#include "Commandadapter.h"
#include <QDebug>
#include <cstring>

CommandAdapter::CommandAdapter()
{
    // 使用CommandTransmitter中的默认初始化逻辑
    memset(&motorCmd, 0, sizeof(motorCmd));
    memset(&findOptCmd, 0, sizeof(findOptCmd));
    memset(&timeData, 0, sizeof(timeData));

    // 设置默认值（与CommandTransmitter保持一致）
    motorCmd.x = 0.0f;
    motorCmd.y = 0.0f;
    motorCmd.speed = 100;

    findOptCmd.whichThaj = SQU_TRAJ;
    findOptCmd.cirTrajRad = 1.0f;
    findOptCmd.squThajStepLen = 10;
    findOptCmd.maxVol = 5.0f;
    findOptCmd.volStepLen = 0.1f;
}

QByteArray CommandAdapter::buildMotorCommand(float x, float y, uint16_t speed)
{
    // 完全复用CommandTransmitter中的逻辑
    motorCmd.x = x;
    motorCmd.y = y;
    if (speed != 0)
        motorCmd.speed = speed;

    CommandFrame_t frame;
    frame.header.startMagic = 0xAA;
    frame.header.cmdId = CMD_MOTOR_CONTROL;
    frame.header.dataLen = sizeof(MotorCmd_t);
    std::memcpy(&frame.payload.motorCmd, &motorCmd, sizeof(MotorCmd_t));
    frame.header.checksum = calculateChecksum(reinterpret_cast<uint8_t*>(&frame.header),
                                              sizeof(CmdHeader_t) - 1);

    return QByteArray(reinterpret_cast<char*>(&frame), sizeof(CmdHeader_t) + sizeof(MotorCmd_t));
}

QByteArray CommandAdapter::buildFindOptimalCommand(uint8_t whichThaj, float cirTrajRad,
                                                   uint8_t squThajStepLen, float maxVol, float volStepLen)
{
    // 完全复用CommandTransmitter中的逻辑
    findOptCmd.whichThaj = whichThaj;
    findOptCmd.cirTrajRad = cirTrajRad;
    findOptCmd.squThajStepLen = squThajStepLen;
    findOptCmd.maxVol = maxVol;
    findOptCmd.volStepLen = volStepLen;

    CommandFrame_t frame;
    frame.header.startMagic = 0xAA;
    frame.header.cmdId = CMD_FIND_OPT_RES;
    frame.header.dataLen = sizeof(FindOptimalCmd_t);
    std::memcpy(&frame.payload.findOptCmd, &findOptCmd, sizeof(FindOptimalCmd_t));
    frame.header.checksum = calculateChecksum(reinterpret_cast<uint8_t*>(&frame.header),
                                              sizeof(CmdHeader_t) - 1);

    return QByteArray(reinterpret_cast<char*>(&frame), sizeof(CmdHeader_t) + sizeof(FindOptimalCmd_t));
}

QByteArray CommandAdapter::buildReadCommand(CommandType_t readCmd)
{
    CommandFrame_t frame;
    frame.header.startMagic = 0xAA;
    frame.header.cmdId = readCmd;
    frame.header.dataLen = 0;
    frame.header.checksum = calculateChecksum(reinterpret_cast<uint8_t*>(&frame.header),
                                              sizeof(CmdHeader_t) - 1);

    return QByteArray(reinterpret_cast<char*>(&frame), sizeof(CmdHeader_t));
}

QByteArray CommandAdapter::buildTimeCommand()
{
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

bool CommandAdapter::parseCommandFrame(const QByteArray &data, CommandFrame_t &frame)
{
    if (data.size() < static_cast<int>(sizeof(CmdHeader_t))) {
        return false;
    }

    // 复用CommandTransmitter的解析逻辑
    std::memcpy(&frame, data.constData(), sizeof(CmdHeader_t));

    if (frame.header.startMagic != 0xAA) {
        return false;
    }

    if (data.size() < static_cast<int>(sizeof(CmdHeader_t) + frame.header.dataLen)) {
        return false;
    }

    if (frame.header.dataLen > 0) {
        std::memcpy(&frame.payload, data.constData() + sizeof(CmdHeader_t), frame.header.dataLen);
    }

    return true;
}

uint8_t CommandAdapter::calculateChecksum(const uint8_t* data, uint16_t len)
{
    // 完全复用CommandTransmitter中的算法
    uint8_t sum = 0;
    for (uint16_t i = 0; i < len; i++) {
        sum += data[i];
    }
    return ~sum + 1;
}

void CommandAdapter::setCurrentTime(DateTime_t &timeData)
{
    // 完全复用CommandTransmitter中的逻辑
    std::time_t currentTime = std::time(nullptr);
    std::tm* localTime = std::localtime(&currentTime);

    timeData.year = localTime->tm_year + 1900;
    timeData.month = localTime->tm_mon + 1;
    timeData.day = localTime->tm_mday;
    timeData.hour = localTime->tm_hour;
    timeData.minute = localTime->tm_min;
    timeData.week_day = localTime->tm_wday;
}
