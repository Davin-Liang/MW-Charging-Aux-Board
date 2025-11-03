#ifndef __COMMAND_TRANSMITTER_H
#define __COMMAND_TRANSMITTER_H

#include <string>
#include <vector>
#include "command_struct.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QObject>
#include <QList>
#include <QByteArray>
#include <cstdint>
#include <string>

class CommandTransmitter : public QObject {
    Q_OBJECT
public:
    /* 构造函数和析构函数 */
    CommandTransmitter(QObject *parent = nullptr);
    virtual ~CommandTransmitter();

    bool param_initialize(const std::string & filename);
    bool param_record(const std::string & filename);

    bool start_server(quint16 port);
    void stop_server();

    int build_command_frame(uint8_t* buffer, CommandType_t cmdType, const void* data, uint16_t dataLen);
    int parse_command_frame(const uint8_t * buffer, uint16_t len, CommandFrame_t * cmd);
    void execute_command(const CommandFrame_t* cmd);
    int send_motor_command(float x, float y, uint16_t speed = 0);
    int send_motor_command(void);
    int send_find_opt_command(ThajType_t whichThaj, 
                                float cirTrajRad, uint8_t squThajStepLen, 
                                float maxVol, float volStepLen);
    int send_find_opt_command(void);
    int send_time_command(void);
    void set_current_time(DateTime_t * dt);
    std::string generate_file_name(const std::string& fileSuffix, const DateTime_t* datetime);
    bool write_opt_res_to_csv(const std::string& filename, const OptResData_t * optData);
    bool write_cur_channel_info_to_csv(const std::string& filename, const CurrentVPCh_t * channelData);

    QTcpSocket * m_clientSocket;        // 当前连接的客户端（假设单连接）
public:
    // 获取配置参数的接口
    MotorCmd_t getMotorCmd() const { return motorCmd; }
    FindOptimalCmd_t getFindOptCmd() const { return findOptCmd; }

    // 更新配置参数的接口
    void setMotorCmd(const MotorCmd_t &cmd) { motorCmd = cmd; }
    void setFindOptCmd(const FindOptimalCmd_t &cmd) { findOptCmd = cmd; }

signals:
    // 新增信号：用于通知UI更新数据
    void motorDataReceived(const MotorData_t &data);
    void channelDataReceived(const CurrentVPCh_t &data);
    void optResDataReceived(const OptResData_t &data);

private slots:
    void on_new_connection(void);
    void on_ready_read(void);
    void on_disconnected(void);
    void on_error_occurred(QAbstractSocket::SocketError error);

private:
    uint8_t calculate_checksum(const uint8_t* data, uint16_t len);

    /* 私有成员变量 */
    MotorCmd_t motorCmd;
    FindOptimalCmd_t findOptCmd;
    MotorData_t motorData;
    DateTime_t timeData;
    OptResData_t optResData;
    CurrentVPCh_t currentVPCh;

    /* TCP 服务器相关私有变量 */
    QTcpServer * m_tcpServer;           // TCP 服务器实例
    quint16 m_serverPort;              // 服务器端口
    bool m_isServerRunning;            // 服务器运行状态
};

#endif // __COMMAND_TRANSMITTER_H
