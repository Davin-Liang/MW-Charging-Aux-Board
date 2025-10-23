// command_struct.h
#ifndef __COMMAND_STRUCT_H
#define __COMMAND_STRUCT_H

#include <stdint.h>

#define BUFFER_LEN 40

// 命令类型枚举
typedef enum {
    CMD_MOTOR_CONTROL = 0x01, // 电机控制  
    CMD_FIND_OPT_RES = 0x02, // 寻优
    MOTOR_DATA_READ = 0x03, // 电机数据读取
    CMD_OPT_RES_READ = 0x04, // 寻优结果读取
    CURRENT_VPCH_READ = 0x05,
    CMD_PASS_DATE_TIME = 0x06, // 向下位机传递数据
    CMD_RESPONSE = 0x80 // 响应命令
} CommandType_t;

// 响应状态
typedef enum {
    STATUS_SUCCESS = 0,
    STATUS_INVALID_CMD,
    STATUS_INVALID_PARAM,
    STATUS_EXECUTE_FAIL,
    STATUS_BUSY
} ResponseStatus_t;

// 命令头结构
typedef struct __attribute__((packed)) {
    uint8_t startMagic; // 起始标志 0xAA
    uint16_t cmdId; // 命令ID
    // uint16_t seqNum; // 序列号
    uint16_t dataLen; // 数据长度
    uint8_t checksum; // 头校验和
} CmdHeader_t;

// 日期时间结构
typedef struct __attribute__((packed)) {
    uint16_t year;          // 年份 2023-2100
    uint8_t month;          // 月份 1-12
    uint8_t day;            // 日期 1-31
    uint8_t hour;           // 小时 0-23
    uint8_t minute;         // 分钟 0-59
    uint8_t week_day;       // 星期 0-6 (0=周日)
} DateTime_t;

// 电机控制命令  
typedef struct __attribute__((packed)) {
    float x; // 接收天线 X 坐标[m]
    float y; // 接收天线 Y 坐标[m]
    uint16_t speed; // 接收天线移动速度——0 表示不设置速度/ >0 表示设置速度
} MotorCmd_t;

// 轨迹类型
typedef enum {
    SQU_TRAJ = 0, // 方型轨迹
    CIR_TRAJ, // 圆形轨迹
} ThajType_t;

typedef struct __attribute__((packed)) {
    float motorX;
    float motorY;
    int motorSpeed;
} MotorData_t;

typedef struct __attribute__((packed)) {
    MotorData_t motorData;
    float optimalPower;
    float optimalVs[4]; 
} OptResData_t;

typedef struct __attribute__((packed)) {
    uint8_t currentChannel;
    float currentV;
    float currentP; 
} CurrentVPCh_t;

// 寻优控制命令
typedef struct __attribute__((packed)) {
    ThajType_t whichThaj; // 哪种轨迹
    float cirTrajRad; // 圆形轨迹半径[m]
    uint8_t squThajStepLen; // 执行方形轨迹的步长[mm]
    float maxVol; // 通道可设置的最大电压[v]
    float volStepLen; // 设置电压时电压跳变的步长[v]    
} FindOptimalCmd_t;

// 响应结构
typedef struct __attribute__((packed)) {
    uint16_t originalSeq;   // 原命令序列号
    uint8_t status;          // 执行状态
    uint32_t timestamp;      // 时间戳
    uint16_t dataLen;       // 响应数据长度
    uint8_t data[];          // 响应数据(柔性数组)
} ResponseData_t;

// 完整命令帧结构
typedef struct __attribute__((packed)) {
    CmdHeader_t header;
    union __attribute__((packed)) {
        MotorCmd_t motorCmd;
        FindOptimalCmd_t findOptCmd;
        MotorData_t motorData;
        OptResData_t optResData;
        CurrentVPCh_t currentVPCh;
        DateTime_t timeData;
        ResponseData_t response;
        uint8_t rawData[BUFFER_LEN];  // 原始数据存储
    } payload;
} CommandFrame_t;

#endif
