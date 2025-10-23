// command_struct.h
#ifndef __COMMAND_STRUCT_H
#define __COMMAND_STRUCT_H

#include <stdint.h>

// ��������ö��
typedef enum {
    CMD_MOTOR_CONTROL = 0x01, // �������  
    CMD_FIND_OPT_RES = 0x02, // Ѱ��
    MOTOR_DATA_READ = 0x03, // ������ݶ�ȡ
    CMD_OPT_RES_READ = 0x04, // Ѱ�Ž����ȡ
    CURRENT_VPCH_READ = 0x05,
    CMD_PASS_DATE_TIME = 0x06, // ����λ����������
    CMD_RESPONSE = 0x80 // ��Ӧ����
} CommandType_t;

// ��Ӧ״̬
typedef enum {
    STATUS_SUCCESS = 0,
    STATUS_INVALID_CMD,
    STATUS_INVALID_PARAM,
    STATUS_EXECUTE_FAIL,
    STATUS_BUSY
} ResponseStatus_t;

// ����ͷ�ṹ
typedef struct __attribute__((packed)) {
    uint8_t startMagic; // ��ʼ��־ 0xAA
    uint16_t cmdId; // ����ID
    uint16_t seqNum; // ���к�
    uint16_t dataLen; // ���ݳ���
    uint8_t checksum; // ͷУ���
} CmdHeader_t;

// ����ʱ��ṹ
typedef struct __attribute__((packed)) {
    uint16_t year;          // ��� 2023-2100
    uint8_t month;          // �·� 1-12
    uint8_t day;            // ���� 1-31
    uint8_t hour;           // Сʱ 0-23
    uint8_t minute;         // ���� 0-59
    uint8_t week_day;       // ���� 0-6 (0=����)
} DateTime_t;

// �����������  
typedef struct __attribute__((packed)) {
    float x; // �������� X ����[m]
    float y; // �������� Y ����[m]
    uint16_t speed; // ���������ƶ��ٶȡ���0 ��ʾ�������ٶ�/ >0 ��ʾ�����ٶ�
} MotorCmd_t;

// �켣����
typedef enum {
    SQU_TRAJ = 0, // ���͹켣
    CIR_TRAJ, // Բ�ι켣
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

// Ѱ�ſ�������
typedef struct __attribute__((packed)) {
    ThajType_t whichThaj; // ���ֹ켣
    float cirTrajRad; // Բ�ι켣�뾶[m]
    uint8_t squThajStepLen; // ִ�з��ι켣�Ĳ���[mm]
    float maxVol; // ͨ�������õ�����ѹ[v]
    float volStepLen; // ���õ�ѹʱ��ѹ����Ĳ���[v]    
} FindOptimalCmd_t;

// ��Ӧ�ṹ
typedef struct __attribute__((packed)) {
    uint16_t originalSeq;   // ԭ�������к�
    uint8_t status;          // ִ��״̬
    uint32_t timestamp;      // ʱ���
    uint16_t dataLen;       // ��Ӧ���ݳ���
    uint8_t data[];          // ��Ӧ����(��������)
} ResponseData_t;

// ��������֡�ṹ
typedef struct __attribute__((packed)) {
    CmdHeader_t header;
    union __attribute__((packed)) {
        MotorCmd_t motorCmd;
        FindOptimalCmd_t findOptCmd;
        MotorData_t motorData;
        CurrentVPCh_t currentVPCh;
        DateTime_t timeData;
        ResponseData_t response;
        uint8_t rawData[128];  // ԭʼ���ݴ洢
    } payload;
} CommandFrame_t;

// ��������
uint8_t calculate_checksum(const uint8_t* data, uint16_t len);
int build_command_frame(uint8_t* buffer, CommandType_t cmdType, const void* data, uint16_t dataLen);
int parse_command_frame(const uint8_t* buffer, uint16_t len, CommandFrame_t* cmd);
ResponseStatus_t execute_command(const CommandFrame_t* cmd, uint8_t* responseData, uint16_t* respLen);

#endif
