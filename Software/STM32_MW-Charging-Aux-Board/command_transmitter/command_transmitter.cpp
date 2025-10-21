#include "command_struct.h"
#include "command_transmitter.h"
#include <cstring>
#include <ctime>
#include <cstdint>
#include <fstream>
#include <nlohmann/json.hpp>
#include <QDebug>
#include <QHostAddress>

using json = nlohmann::json;

CommandTransmitter::CommandTransmitter(QObject * parent)
    : QObject(parent)
    , m_tcpServer(nullptr)
    , m_clientSocket(nullptr)
    , m_serverPort(0)
    , m_isServerRunning(false)
{
    /* 初始化 TCP 服务器 */
    m_tcpServer = new QTcpServer(this);
    connect(m_tcpServer, &QTcpServer::newConnection, this, &CommandTransmitter::on_new_connection);

    /* 初始化结构体 */
    memset(&motorCmd, 0, sizeof(motorCmd));
    memset(&findOptCmd, 0, sizeof(findOptCmd));
    memset(&motorData, 0, sizeof(motorData));
    memset(&timeData, 0, sizeof(timeData));
    memset(&optResData, 0, sizeof(optResData));

    /* 从json 配置文件中读取结构体数据 */
    param_initialize("config.json");
}

bool CommandTransmitter::start_server(quint16 port)
{
    if (m_isServerRunning) 
        stop_server();
    
    /* 开始监听端口 */
    if (!m_tcpServer->listen(QHostAddress::Any, port)) 
    {
        qDebug() << "无法启动服务器:" << m_tcpServer->errorString();
        return false;
    }
    
    m_serverPort = port;
    m_isServerRunning = true;
    qDebug() << "命令传输服务器已启动，监听端口:" << port;

    return true;
}

void CommandTransmitter::stop_server(void)
{
    if (m_clientSocket) 
    {
        m_clientSocket->close();
        m_clientSocket->deleteLater();
        m_clientSocket = nullptr;
    }
    
    if (m_tcpServer)
        m_tcpServer->close();
    
    m_isServerRunning = false;
    qDebug() << "服务器已停止";
}

void CommandTransmitter::on_new_connection(void)
{
    if (m_clientSocket) 
    {
        /* 如果已有连接，拒绝新连接 */
        QTcpSocket *newSocket = m_tcpServer->nextPendingConnection();
        qDebug() << "拒绝新连接，已有客户端连接:" << newSocket->peerAddress().toString();
        newSocket->close();
        newSocket->deleteLater();

        return;
    }
    
    m_clientSocket = m_tcpServer->nextPendingConnection();
    if (m_clientSocket) 
    {
        connect(m_clientSocket, &QTcpSocket::readyRead, this, &CommandTransmitter::onReadyRead);
        connect(m_clientSocket, &QTcpSocket::disconnected, this, &CommandTransmitter::onDisconnected);
        connect(m_clientSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),
                this, &CommandTransmitter::onErrorOccurred);
        
        qDebug() << "新的下位机连接:" << m_clientSocket->peerAddress().toString() 
                 << ":" << m_clientSocket->peerPort();
    }
}

void CommandTransmitter::on_ready_read(void)
{
    if (!m_clientSocket) 
        return; // 如果没和客户端建立连接的话，直接返回
    
    QByteArray data = m_clientSocket->readAll();
    qDebug() << "接收到下位机数据，长度:" << data.length();
    
    /* 解析下位机的响应或状态数据 */
    if (data.length() >= sizeof(CommandFrame_t)) 
    {
        CommandFrame_t cmd;
        if (parse_command_frame(reinterpret_cast<const uint8_t*>(data.constData()), 
                               data.length(), &cmd) == 0) 
        {
            // 成功解析命令帧
            qDebug() << "解析到命令帧，类型:" << cmd.cmdType << "序列号:" << cmd.seqNum;
        }
    }
}

void CommandTransmitter::on_disconnected()
{
    if (m_clientSocket) 
    {
        qDebug() << "下位机断开连接:" << m_clientSocket->peerAddress().toString();
        m_clientSocket->deleteLater();
        m_clientSocket = nullptr;
    }
}

void CommandTransmitter::on_error_occurred(QAbstractSocket::SocketError error)
{
    if (m_clientSocket)
        qDebug() << "Socket错误:" << m_clientSocket->errorString();
}

/**
  * @brief 从JSON文件初始化参数
  * @param filename JSON文件名
  * @return true-成功, false-失败
  **/
bool CommandTransmitter::param_initialize(const std::string & filename) 
{
    try 
    {
        /* 打开并解析JSON文件 */
        std::ifstream file(filename);
        if (!file.is_open()) 
        {
            std::cerr << "错误: 无法打开文件 " << filename << std::endl;
            return false;
        }
        
        json config;
        file >> config;
        file.close();

        /* 初始化 MotorCmd_t 参数 */
        if (config.contains("motorCmd")) 
        {
            json motorConfig = config["motorCmd"];
            motorCmd.x = motorConfig["x"];
            motorCmd.y = motorConfig["y"];
            motorCmd.speed = motorConfig["speed"];
        } 
        else 
        {
            std::cerr << "错误: JSON中缺少 motorCmd 部分" << std::endl;
            return false;
        }

        /* 初始化 FindOptimalCmd_t 参数 */
        if (config.contains("findOptCmd")) 
        {
            json optConfig = config["findOptCmd"];
            findOptCmd.whichThaj = static_cast<ThajType_t>(optConfig["whichThaj"]);
            findOptCmd.cirTrajRad = optConfig["cirTrajRad"];
            findOptCmd.squThajStepLen = optConfig["squThajStepLen"];
            findOptCmd.maxVol = optConfig["maxVol"];
            findOptCmd.volStepLen = optConfig["volStepLen"];
            
        } 
        else 
        {
            std::cerr << "错误: JSON中缺少 findOptCmd 部分" << std::endl;
            return false;
        }

        std::cout << "参数初始化成功!" << std::endl;
        return true;

    } 
    catch (const std::exception& e) 
    {
        std::cerr << "参数初始化异常: " << e.what() << std::endl;
        return false;
    }
}

bool CommandTransmitter::param_record(const std::string & filename) 
{

}

/**
  * @brief  构建命令帧
  * @param  data 数据地址
  * @param  len 数据长度
  * @return -1 数据长度超出限制 | [有效数据长度]
  **/
int CommandTransmitter::build_command_frame(uint8_t* buffer, CommandType_t cmdType, const void* data, uint16_t dataLen) 
{
    if (dataLen > 128) return -1;
    
    CommandFrame_t * frame = (CommandFrame_t*)buffer;
    
    /* 填充命令头 */
    frame->header.startMagic = 0xAA;
    frame->header.cmdId = cmdType;
    frame->header.seqNum = seqNum;
    frame->header.dataLen = dataLen;
    
    /* 拷贝数据 */
    if (dataLen > 0 && data != NULL)
        std::memcpy(frame->payload.rawData, data, dataLen);
    
    /* 计算校验和（只计算头部） */
    frame->header.checksum = calculate_checksum((uint8_t*)&frame->header, 
                                               sizeof(CmdHeader_t) - 1);
    
    return sizeof(CmdHeader_t) + dataLen;
}

/**
  * @brief  计算校验和
  * @param  data 数据地址
  * @param  len 数据长度
  * @return void
  **/
uint8_t CommandTransmitter::calculate_checksum(const uint8_t* data, uint16_t len) 
{
    uint8_t sum = 0;
    for (uint16_t i = 0; i < len; i++) 
    {
        sum += data[i];
    }
    return ~sum + 1; // 补码
}

/**
  * @brief  解析命令帧
  * @param  buffer 数据地址
  * @param  len 数据长度
  * @param  cmd 命令结构体
  * @return -1 数据长度过于小 | -2 命令帧帧头不对 | -3 命令帧校验和不对 | -4 数据长度不对
  **/
int CommandTransmitter::parse_command_frame(const uint8_t * buffer, uint16_t len, CommandFrame_t * cmd) 
{
    if(len < sizeof(CmdHeader_t)) return -1;
    
    std::memcpy(cmd, buffer, sizeof(CmdHeader_t));
    
    /* 验证起始标志 */
    if(cmd->header.startMagic != 0xAA) return -2;
    
    /* 验证校验和 */
    uint8_t calc_checksum = calculate_checksum(buffer, sizeof(CmdHeader_t) - 1);
    if(calc_checksum != cmd->header.checksum) return -3;
    
    /* 验证数据长度 */
    if(len < sizeof(CmdHeader_t) + cmd->header.dataLen) return -4;
    
    /* 拷贝数据部分 */
    if(cmd->header.dataLen > 0)
        std::memcpy(cmd->payload.rawData, buffer + sizeof(CmdHeader_t), cmd->header.dataLen);
    
    return 0;
}

/** @brief  执行命令并放回响应数据
  * @param  buffer 数据地址
  * @param  len 数据长度
  * @param  cmd 命令结构体
  * @return -1 数据长度过于小 | -2 命令帧帧头不对 | -3 命令帧校验和不对 | -4 数据长度不对
  **/
void execute_command(const CommandFrame_t* cmd) 
{
    switch(cmd->header.cmdId) 
    {     
        case MOTOR_DATA_READ:
            if(cmd->header.dataLen == sizeof(MotorData_t)) 
            {
                motorData.motorX = cmd->motorData.motorX;
                motorData.motorY = cmd->motorData.motorY;
                motorData.motorSpeed = cmd->motorData.motorSpeed;
            }
            
            break;

        case CMD_OPT_RES_READ:
            if(cmd->header.dataLen == sizeof(OptResData_t)) 
            {
                optResData.motorData.motorX = cmd->optResData.motorData.motorX;
                optResData.motorData.motorY = cmd->optResData.motorData.motorY;
                optResData.optimalPower = cmd->optResData.optimalPower;
                for (int i = 0; i < 0; i ++)
                    optResData.optimalVs[i] = cmd->optResData.optimalVs[i];
            }
            
            break;

        case CURRENT_VPCH_READ:
            if(cmd->header.dataLen == sizeof(CurrentVPCh_t)) 
            {
                currentVPCh.currentChannel = cmd->currentVPCh.currentChannel;
                currentVPCh.currentV = cmd->currentVPCh.currentV;
                currentVPCh.currentP = cmd->currentVPCh.currentP;
            }
            
            break;

        case MOTOR_DATA_READ:
            if(cmd->header.dataLen == sizeof(MotorData_t)) 
            {
                motorData.motorX = cmd->motorData.motorX;
                motorData.motorY = cmd->motorData.motorY;
                motorData.motorSpeed = cmd->motorData.motorSpeed;
            }
            
            break;
            
        case CMD_RESPONSE:
            if(cmd->header.dataLen == sizeof(ResponseData_t))
                ;// system_control(&cmd->payload.response);
            
            break;
            
        default:
            break;
    }

    #endif
    
    // 构建响应数据（如果有需要返回的数据）
    *respLen = 0;
    // 可以在这里填充response_data
    
    return status;
}

/**
  * @brief  发送电机命令帧
  * @param  sock socket 编号
  * @param  x 目标电机位置 X 坐标
  * @param  y 目标电机位置 Y 坐标
  * @param  speed 目标电机速度
  * @return y 目标电机位置 Y 坐标
  **/
int CommandTransmitter::send_motor_command(int sock, float x, float y, uint16_t speed = 0) 
{
    uint8_t buffer[128];
    motorCmd.x = x;
    motorCmd.y = y;
    if (speed != 0)
        motorCmd.speed = speed;
    
    int frameLen = build_command_frame(buffer, CMD_MOTOR_CONTROL, &motorCmd, sizeof(MotorCmd_t));
    
    // if(frameLen > 0) 
    //     return send(sock, buffer, frameLen, 0); // TODO: 可能要改成 QT socket 的发送函数，这里用的是 socket 的标准版本的发送函数

    if (frameLen > 0) 
    {
        /* 使用 QTcpSocket 的 write 函数发送数据 */
        qint64 bytesWritten = m_clientSocket->write(reinterpret_cast<const char*>(buffer), frameLen);
    }

    return -1;
}

int CommandTransmitter::send_motor_command(void) 
{
    uint8_t buffer[128];
    
    int frameLen = build_command_frame(buffer, CMD_MOTOR_CONTROL, &motorCmd, sizeof(MotorCmd_t));
    
    // if(frameLen > 0) 
    //     return send(sock, buffer, frameLen, 0); // TODO: 可能要改成 QT socket 的发送函数，这里用的是 socket 的标准版本的发送函数

    if (frameLen > 0) 
    {
        /* 使用 QTcpSocket 的 write 函数发送数据 */
        qint64 bytesWritten = m_clientSocket->write(reinterpret_cast<const char*>(buffer), frameLen);
    }

    return -1;
}

int CommandTransmitter::send_find_opt_command(int sock, ThajType_t whichThaj, 
                                                float cirTrajRad, uint8_t squThajStepLen, 
                                                float maxVolfloat volStepLen) 
{
    uint8_t buffer[128];
    findOptCmd.whichThaj = whichThaj;
    findOptCmd.cirTrajRad = cirTrajRad;
    findOptCmd.squThajStepLen = squThajStepLen;
    findOptCmd.volStepLen = volStepLen;
    
    int frameLen = build_command_frame(buffer, CMD_FIND_OPT_RES, &findOptCmd, sizeof(FindOptimalCmd_t));
    
    // if(frameLen > 0) 
    //     return send(sock, buffer, frameLen, 0); // TODO: 可能要改成 QT socket 的发送函数，这里用的是 socket 的标准版本的发送函数

    if (frameLen > 0) 
    {
        /* 使用 QTcpSocket 的 write 函数发送数据 */
        qint64 bytesWritten = m_clientSocket->write(reinterpret_cast<const char*>(buffer), frameLen);
        
        // if (bytesWritten == frameLen) 
        // {
        //     // 可选：立即刷新发送缓冲区
        //     if (m_clientSocket->flush()) {
        //         qDebug() << "电机命令发送成功，长度:" << frameLen;
        //         return 0;
        //     } else {
        //         qDebug() << "电机命令发送但刷新缓冲区失败";
        //         return -3;
        //     }
        // } else if (bytesWritten == -1) {
        //     qDebug() << "电机命令发送失败:" << m_clientSocket->errorString();
        //     return -2;
        // } else {
        //     qDebug() << "电机命令发送不完整，预期:" << frameLen << "实际:" << bytesWritten;
        //     return -4;
        // }
    }

    return -1;
}

int CommandTransmitter::send_find_opt_command(void) 
{
    uint8_t buffer[128];
    
    int frameLen = build_command_frame(buffer, CMD_FIND_OPT_RES, &findOptCmd, sizeof(FindOptimalCmd_t));
    
    // if(frameLen > 0) 
    //     return send(sock, buffer, frameLen, 0); // TODO: 可能要改成 QT socket 的发送函数，这里用的是 socket 的标准版本的发送函数

    if (frameLen > 0) 
    {
        /* 使用 QTcpSocket 的 write 函数发送数据 */
        qint64 bytesWritten = m_clientSocket->write(reinterpret_cast<const char*>(buffer), frameLen);
    }

    return -1;
}

int CommandTransmitter::send_time_command(void) 
{
    uint8_t buffer[128];
    
    set_current_time(&timeData);

    int frameLen = build_command_frame(buffer, CMD_PASS_DATE_TIME, &timeData, sizeof(DateTime_t));
    
    // if (frameLen > 0) 
    //     return send(sock, buffer, frameLen, 0); // TODO: 可能要改成 QT socket 的发送函数，这里用的是 socket 的标准版本的发送函数

    if (frameLen > 0) 
    {
        /* 使用 QTcpSocket 的 write 函数发送数据 */
        qint64 bytesWritten = m_clientSocket->write(reinterpret_cast<const char*>(buffer), frameLen);
    }

    return -1;
}

void CommandTransmitter::set_current_time(DateTime_t * dt) 
{
    if (dt == nullptr)
        return;  // 安全检查
    
    /* 获取当前时间 */
    std::time_t currentTime = std::time(nullptr);
    std::tm* localTime = std::localtime(&currentTime);
    
    /* 填充结构体 */
    dt->year = localTime->tm_year + 1900;      // tm_year 是从1900开始的年数
    dt->month = localTime->tm_mon + 1;         // tm_mon 范围是 0-11
    dt->day = localTime->tm_mday;              // tm_mday 范围是 1-31
    dt->hour = localTime->tm_hour;             // tm_hour 范围是 0-23
    dt->minute = localTime->tm_min;            // tm_min 范围是 0-59
    dt->week_day = localTime->tm_wday;         // tm_wday 范围是 0-6 (0=周日)
}
