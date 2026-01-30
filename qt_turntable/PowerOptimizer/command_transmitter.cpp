// #include "command_struct.h"
// #include "command_transmitter.h"

// // C++ 标准库头文件
// #include <cstring>
// #include <ctime>
// #include <cstdint>
// #include <fstream>
// #include <sstream>
// #include <iomanip>
// #include <iostream>

// // JSON 库
// #include "thirdparty/nlohmann/json.hpp"

// // Qt 头文件
// #include <QDebug>
// #include <QHostAddress>

// using json = nlohmann::json;

// /**
//   * @brief  构造函数：初始化TCP服务器、私有结构体、从json文件中读取默认参数
//   * @param  parent
//   * @return void
//   **/
// CommandTransmitter::CommandTransmitter(QObject * parent)
//     : QObject(parent)
//     , m_tcpServer(nullptr)
//     , m_clientSocket(nullptr)
//     , m_serverPort(0)
//     , m_isServerRunning(false)
// {
//     /* 初始化 TCP 服务器 */
//     m_tcpServer = new QTcpServer(this);
//     connect(m_tcpServer, &QTcpServer::newConnection, this, &CommandTransmitter::on_new_connection);

//     /* 初始化结构体 */
//     memset(&motorCmd, 0, sizeof(motorCmd));
//     memset(&findOptCmd, 0, sizeof(findOptCmd));
//     memset(&motorData, 0, sizeof(motorData));
//     memset(&timeData, 0, sizeof(timeData));
//     memset(&optResData, 0, sizeof(optResData));

//     /* 从json 配置文件中读取结构体数据 */
//     param_initialize("config.json");
// }

// CommandTransmitter::~CommandTransmitter()
// {
//     /* 清理资源 */
//     stop_server();
    
//     /* 如果手动分配了其他资源，在这里释放 */
//     if (m_tcpServer) 
//     {
//         m_tcpServer->deleteLater();
//         m_tcpServer = nullptr;
//     }
// }




// void CommandTransmitter::on_ready_read(void)
// {
//     if (!m_clientSocket) 
//         return; // 如果没和客户端建立连接的话，直接返回
    
//     QByteArray data = m_clientSocket->readAll();
//     qDebug() << "接收到下位机数据，长度:" << data.length();
    
//     /* 解析下位机的响应或状态数据 */
//     qDebug() << "sizeof(CommandFrame_t):" << sizeof(CommandFrame_t);
//     if (data.length() <= sizeof(CommandFrame_t)) 
//     {
//         CommandFrame_t cmd;
//         int parseResult = parse_command_frame(reinterpret_cast<const uint8_t*>(data.constData()), 
//                                data.length(), &cmd);
//         qDebug() << "parseResult:" << parseResult;
//         if (parseResult == 0) 
//         {
//             // 成功解析命令帧
//             qDebug() << "解析到命令帧，类型:" << cmd.header.cmdId;
//             execute_command(&cmd);
//         }
//     }
// }


// /**
//   * @brief 从JSON文件初始化参数
//   * @param filename JSON文件名
//   * @return true 成功 \ false 失败
//   **/
// bool CommandTransmitter::param_initialize(const std::string & filename) 
// {
//     try 
//     {
//         /* 打开并解析JSON文件 */
//         std::ifstream file(filename);
//         if (!file.is_open()) 
//         {
//             std::cout << "错误: 无法打开文件 " << filename << std::endl;
//             return false;
//         }
        
//         json config;
//         file >> config;
//         file.close();

//         /* 初始化 MotorCmd_t 参数 */
//         if (config.contains("motorCmd")) 
//         {
//             json motorConfig = config["motorCmd"];
//             motorCmd.x = motorConfig["x"];
//             motorCmd.y = motorConfig["y"];
//             motorCmd.speed = motorConfig["speed"];
//         } 
//         else 
//         {
//             std::cout << "错误: JSON中缺少 motorCmd 部分" << std::endl;
//             return false;
//         }

//         /* 初始化 FindOptimalCmd_t 参数 */
//         if (config.contains("findOptCmd")) 
//         {
//             json optConfig = config["findOptCmd"];
//             findOptCmd.whichThaj = optConfig["whichThaj"];
//             findOptCmd.cirTrajRad = optConfig["cirTrajRad"];
//             findOptCmd.squThajStepLen = optConfig["squThajStepLen"];
//             findOptCmd.maxVol = optConfig["maxVol"];
//             findOptCmd.volStepLen = optConfig["volStepLen"];
            
//         } 
//         else 
//         {
//             std::cerr << "错误: JSON中缺少 findOptCmd 部分" << std::endl;
//             return false;
//         }

//         std::cout << "参数初始化成功!" << std::endl;
//         return true;

//     } 
//     catch (const std::exception& e) 
//     {
//         std::cerr << "参数初始化异常: " << e.what() << std::endl;
//         return false;
//     }
// }

// bool CommandTransmitter::param_record(const std::string & filename) 
// {
//     // TODO:
// }

// /**
//   * @brief  构建命令帧
//   * @param  data 数据地址
//   * @param  len 数据长度
//   * @return -1 数据长度超出限制 | [有效数据长度]
//   **/
// int CommandTransmitter::build_command_frame(uint8_t* buffer, CommandType_t cmdType, const void* data, uint16_t dataLen) 
// {
//     if (dataLen > BUFFER_LEN) return -1;
    
//     CommandFrame_t * frame = (CommandFrame_t*)buffer;
    
//     /* 填充命令头 */
//     frame->header.startMagic = 0xAA;
//     frame->header.cmdId = cmdType;
//     // frame->header.seqNum = seqNum;
//     frame->header.dataLen = dataLen;
//     std::cout << "dataLen = " << dataLen << std::endl;
    
//     /* 拷贝数据 */
//     if (dataLen > 0 && data != NULL)
//         std::memcpy(frame->payload.rawData, data, dataLen);
    
//     /* 计算校验和（只计算头部） */
//     frame->header.checksum = calculate_checksum((uint8_t*)&frame->header, 
//                                                sizeof(CmdHeader_t) - 1);
    
//     return sizeof(CmdHeader_t) + dataLen;
// }

// /**
//   * @brief  计算校验和
//   * @param  data 数据地址
//   * @param  len 数据长度
//   * @return void
//   **/
// uint8_t CommandTransmitter::calculate_checksum(const uint8_t* data, uint16_t len) 
// {
//     uint8_t sum = 0;
//     for (uint16_t i = 0; i < len; i++) 
//     {
//         sum += data[i];
//     }
//     return ~sum + 1; // 补码
// }

// /**
//   * @brief  解析命令帧
//   * @param  buffer 数据地址
//   * @param  len 数据长度
//   * @param  cmd 命令结构体
//   * @return -1 数据长度过于小 | -2 命令帧帧头不对 | -3 命令帧校验和不对 | -4 数据长度不对
//   **/
// int CommandTransmitter::parse_command_frame(const uint8_t * buffer, uint16_t len, CommandFrame_t * cmd) 
// {
//     if(len < sizeof(CmdHeader_t)) return -1;
    
//     std::memcpy(cmd, buffer, sizeof(CmdHeader_t));
    
//     /* 验证起始标志 */
//     if(cmd->header.startMagic != 0xAA) return -2;
    
//     /* 验证校验和 */
//     uint8_t calc_checksum = calculate_checksum(buffer, sizeof(CmdHeader_t) - 1);
//     if(calc_checksum != cmd->header.checksum) return -3;
    
//     /* 验证数据长度 */
//     if(len < sizeof(CmdHeader_t) + cmd->header.dataLen) return -4;
    
//     /* 拷贝数据部分 */
//     if(cmd->header.dataLen > 0)
//         std::memcpy(cmd->payload.rawData, buffer + sizeof(CmdHeader_t), cmd->header.dataLen);
    
//     return 0;
// }

// /** @brief  执行命令并放回响应数据
//   * @param  buffer 数据地址
//   * @param  len 数据长度
//   * @param  cmd 命令结构体
//   * @return -1 数据长度过于小 | -2 命令帧帧头不对 | -3 命令帧校验和不对 | -4 数据长度不对
//   **/
// void CommandTransmitter::execute_command(const CommandFrame_t* cmd) 
// {
//     switch(cmd->header.cmdId) 
//     {     
//         case MOTOR_DATA_READ:
//             if(cmd->header.dataLen == sizeof(MotorData_t)) 
//             {
//                 motorData.motorX = cmd->payload.motorData.motorX;
//                 motorData.motorY = cmd->payload.motorData.motorY;
//                 motorData.motorSpeed = cmd->payload.motorData.motorSpeed;

//                 // 发射电机数据信号
//                 emit motorDataReceived(motorData);

//                 std::cout << "motorData.motorX" << motorData.motorX
//                         << "motorData.motorY" << motorData.motorY
//                         << "motorData.motorSpeed" << motorData.motorSpeed << std::endl;
//             }
            
//             break;

//         case CMD_OPT_RES_READ:
//             if(cmd->header.dataLen == sizeof(OptResData_t)) 
//             {
//                 optResData.motorData.motorX = cmd->payload.optResData.motorData.motorX;
//                 optResData.motorData.motorY = cmd->payload.optResData.motorData.motorY;
//                 optResData.optimalPower = cmd->payload.optResData.optimalPower;
//                 for (int i = 0; i < 4; i ++)
//                     optResData.optimalVs[i] = cmd->payload.optResData.optimalVs[i];

//                 // 发射优化结果信号
//                 emit optResDataReceived(optResData);

//                 std::cout << "optResData.motorData.motorX" << optResData.motorData.motorX
//                         << "optResData.motorData.motorY" << optResData.motorData.motorY
//                         << "optResData.optimalPower" << optResData.optimalPower
//                         << "optResData.optimalVs[0]" << optResData.optimalVs[0]
//                         << "optResData.optimalVs[1]" << optResData.optimalVs[1]
//                         << "optResData.optimalVs[2]" << optResData.optimalVs[2]
//                         << "optResData.optimalVs[3]" << optResData.optimalVs[3]
//                          << std::endl;

//                 std::string resultFileName = generate_file_name("optimal_result.csv", &timeData);

//                 /* 将数据记录到文件中 */
//                 write_opt_res_to_csv(resultFileName, &optResData);
//             }
            
//             break;

//         case CURRENT_VPCH_READ:
//             if(cmd->header.dataLen == sizeof(CurrentVPCh_t)) 
//             {
//                 currentVPCh.currentChannel = cmd->payload.currentVPCh.currentChannel;
//                 // std::cout << "cmd->payload.rawData[5] = " << cmd->payload.rawData[5] << std::endl;
//                 std::cout << (currentVPCh.currentChannel == 2) << std::endl;
//                 currentVPCh.currentV = cmd->payload.currentVPCh.currentV;
//                 currentVPCh.currentP = cmd->payload.currentVPCh.currentP;

//                 // 发射通道数据信号
//                 emit channelDataReceived(currentVPCh);

//                 std::cout 
//                         << "currentVPCh.currentV" << currentVPCh.currentV
//                         << "currentVPCh.currentP" << currentVPCh.currentP 
//                         << "currentVPCh.currentChannel" << currentVPCh.currentChannel
//                         << std::endl;

//                 std::string resultFileName = generate_file_name("channel_info.csv", &timeData);

//                 /* 将数据记录到文件中 */
//                 write_cur_channel_info_to_csv(resultFileName, &currentVPCh);
//             }
            
//             break;

//         // case MOTOR_DATA_READ:
//         //     if(cmd->header.dataLen == sizeof(MotorData_t)) 
//         //     {
//         //         motorData.motorX = cmd->motorData.motorX;
//         //         motorData.motorY = cmd->motorData.motorY;
//         //         motorData.motorSpeed = cmd->motorData.motorSpeed;
//         //     }
            
//         //     break;
            
//         case CMD_RESPONSE:
//             if(cmd->header.dataLen == sizeof(ResponseData_t))
//                 ;// system_control(&cmd->payload.response);
            
//             break;
            
//         default:
//             break;
//     }
    
//     // 构建响应数据（如果有需要返回的数据）
//     // *respLen = 0;
//     // 可以在这里填充response_data
    
//     // return status;
// }

// /**
//   * @brief  发送控制电机命令帧（不使用类私有变量）
//   * @param  x 目标电机位置 X 坐标
//   * @param  y 目标电机位置 Y 坐标
//   * @param  speed 目标电机速度
//   * @return 发送的字节数
//   **/
// int CommandTransmitter::send_motor_command(float x, float y, uint16_t speed) 
// {
//     uint8_t buffer[BUFFER_LEN];
//     motorCmd.x = x;
//     motorCmd.y = y;
//     if (speed != 0)
//         motorCmd.speed = speed;
    
//     int frameLen = build_command_frame(buffer, CMD_MOTOR_CONTROL, &motorCmd, sizeof(MotorCmd_t));

//     if (frameLen > 0) 
//     {
//         /* 使用 QTcpSocket 的 write 函数发送数据 */
//         qint64 bytesWritten = m_clientSocket->write(reinterpret_cast<const char*>(buffer), frameLen);
        
//         if (bytesWritten == frameLen) 
//         {
//            /*  立即刷新发送缓冲区 */
//             if (m_clientSocket->flush()) 
//             {
//                 qDebug() << "电机命令发送成功，长度:" << frameLen;
//                 return 0;
//             } 
//             else 
//             {
//                 qDebug() << "电机命令发送但刷新缓冲区失败";
//                 return -3;
//             }
//         } 
//         else if (bytesWritten == -1) 
//         {
//             qDebug() << "电机命令发送失败:" << m_clientSocket->errorString();
//             return -2;
//         } 
//         else 
//         {
//             qDebug() << "电机命令发送不完整，预期:" << frameLen << "实际:" << bytesWritten;
//             return -4;
//         }
//     }

//     return -1;
// }

// /**
//   * @brief  发送控制电机命令帧（使用类私有变量）
//   * @param  void 
//   * @return void
//   **/
// int CommandTransmitter::send_motor_command(void) 
// {
//     uint8_t buffer[BUFFER_LEN];
    
//     int frameLen = build_command_frame(buffer, CMD_MOTOR_CONTROL, &motorCmd, sizeof(MotorCmd_t));

//     if (frameLen > 0) 
//     {
//         /* 使用 QTcpSocket 的 write 函数发送数据 */
//         qint64 bytesWritten = m_clientSocket->write(reinterpret_cast<const char*>(buffer), frameLen);
        
//         if (bytesWritten == frameLen) 
//         {
//            /*  立即刷新发送缓冲区 */
//             if (m_clientSocket->flush()) 
//             {
//                 qDebug() << "电机命令发送成功，长度:" << frameLen;
//                 return 0;
//             } 
//             else 
//             {
//                 qDebug() << "电机命令发送但刷新缓冲区失败";
//                 return -3;
//             }
//         } 
//         else if (bytesWritten == -1) 
//         {
//             qDebug() << "电机命令发送失败:" << m_clientSocket->errorString();
//             return -2;
//         } 
//         else 
//         {
//             qDebug() << "电机命令发送不完整，预期:" << frameLen << "实际:" << bytesWritten;
//             return -4;
//         }
//     }

//     return -1;
// }

// /**
//   * @brief  发送寻优控制命令帧（不使用类私有变量）
//   * @param  whichThaj 哪种轨迹- CIR_TRAJ 圆形轨迹 \ SQU_TRAJ 方型轨迹
//   * @param  cirTrajRad 如果使用圆形轨迹，将设置圆形轨迹半径
//   * @param  squThajStepLen 如果使用方形轨迹，将设置执行方形轨迹的步长
//   * @param  maxVol 通道最大电压
//   * @param  volStepLen 电压步长
//   * @return 发送的字节数
//   **/
// int CommandTransmitter::send_find_opt_command(ThajType_t whichThaj,
//                                                 float cirTrajRad, float squThajLen, uint8_t squThajStepLen,
//                                                 float maxVol, float volStepLen,float initialVol)
// {
//     uint8_t buffer[BUFFER_LEN];
//     findOptCmd.whichThaj = (uint8_t)whichThaj;
//     findOptCmd.cirTrajRad = cirTrajRad;
//     findOptCmd.squThajLen = squThajLen;
//     findOptCmd.squThajStepLen = squThajStepLen;
//     findOptCmd.maxVol = maxVol;
//     findOptCmd.volStepLen = volStepLen;
//     findOptCmd.initialVol = initialVol;
//     // printf("sizeof(ThajType_t) = %d\n", sizeof(ThajType_t));
//     std::cout << "sizeof(ThajType_t) = " << sizeof(ThajType_t) << std::endl;
    
//     int frameLen = build_command_frame(buffer, CMD_FIND_OPT_RES, &findOptCmd, sizeof(FindOptimalCmd_t));

//     if (frameLen > 0) 
//     {
//         /* 使用 QTcpSocket 的 write 函数发送数据 */
//         qint64 bytesWritten = m_clientSocket->write(reinterpret_cast<const char*>(buffer), frameLen);
        
//         if (bytesWritten == frameLen) 
//         {
//            /*  立即刷新发送缓冲区 */
//             if (m_clientSocket->flush()) 
//             {
//                 qDebug() << "电机命令发送成功，长度:" << frameLen;
//                 return 0;
//             } 
//             else 
//             {
//                 qDebug() << "电机命令发送但刷新缓冲区失败";
//                 return -3;
//             }
//         } 
//         else if (bytesWritten == -1) 
//         {
//             qDebug() << "电机命令发送失败:" << m_clientSocket->errorString();
//             return -2;
//         } 
//         else 
//         {
//             qDebug() << "电机命令发送不完整，预期:" << frameLen << "实际:" << bytesWritten;
//             return -4;
//         }
//     }

//     return -1;
// }

// /**
//   * @brief  发送寻优控制命令帧（使用类私有变量）
//   * @param  void
//   * @return 发送的字节数
//   **/
// int CommandTransmitter::send_find_opt_command(void) 
// {
//     uint8_t buffer[BUFFER_LEN];

//     std::cout << "sizeof(CommandType_t) = " << sizeof(CommandType_t) << std::endl;
//     std::cout << "sizeof(ThajType_t) = " << sizeof(ThajType_t) << std::endl;
    
//     int frameLen = build_command_frame(buffer, CMD_FIND_OPT_RES, &findOptCmd, sizeof(FindOptimalCmd_t));

//     if (frameLen > 0) 
//     {
//         /* 使用 QTcpSocket 的 write 函数发送数据 */
//         qint64 bytesWritten = m_clientSocket->write(reinterpret_cast<const char*>(buffer), frameLen);
        
//         if (bytesWritten == frameLen) 
//         {
//            /*  立即刷新发送缓冲区 */
//             if (m_clientSocket->flush()) 
//             {
//                 qDebug() << "电机命令发送成功，长度:" << frameLen;
//                 return 0;
//             } 
//             else 
//             {
//                 qDebug() << "电机命令发送但刷新缓冲区失败";
//                 return -3;
//             }
//         } 
//         else if (bytesWritten == -1) 
//         {
//             qDebug() << "电机命令发送失败:" << m_clientSocket->errorString();
//             return -2;
//         } 
//         else 
//         {
//             qDebug() << "电机命令发送不完整，预期:" << frameLen << "实际:" << bytesWritten;
//             return -4;
//         }
//     }

//     return -1;
// }

// /**
//   * @brief  发送当前时间命令帧（使用类私有变量）
//   * @param  void
//   * @return 发送的字节数
//   **/
// int CommandTransmitter::send_time_command(void) 
// {
//     uint8_t buffer[BUFFER_LEN];
    
//     set_current_time(&timeData);

//     int frameLen = build_command_frame(buffer, CMD_PASS_DATE_TIME, &timeData, sizeof(DateTime_t));

//     if (frameLen > 0) 
//     {
//         /* 使用 QTcpSocket 的 write 函数发送数据 */
//         qint64 bytesWritten = m_clientSocket->write(reinterpret_cast<const char*>(buffer), frameLen);
        
//         if (bytesWritten == frameLen) 
//         {
//            /*  立即刷新发送缓冲区 */
//             if (m_clientSocket->flush()) 
//             {
//                 qDebug() << "电机命令发送成功，长度:" << frameLen;
//                 return 0;
//             } 
//             else 
//             {
//                 qDebug() << "电机命令发送但刷新缓冲区失败";
//                 return -3;
//             }
//         } 
//         else if (bytesWritten == -1) 
//         {
//             qDebug() << "电机命令发送失败:" << m_clientSocket->errorString();
//             return -2;
//         } 
//         else 
//         {
//             qDebug() << "电机命令发送不完整，预期:" << frameLen << "实际:" << bytesWritten;
//             return -4;
//         }
//     }

//     return -1;
// }

// /**
//   * @brief  为私有变量设置时间
//   * @param  dt 时间
//   * @return void
//   **/
// void CommandTransmitter::set_current_time(DateTime_t * dt) 
// {
//     if (dt == nullptr)
//         return; // 安全检查
    
//     /* 获取当前时间 */
//     std::time_t currentTime = std::time(nullptr);
//     std::tm* localTime = std::localtime(&currentTime);
    
//     /* 填充结构体 */
//     dt->year = localTime->tm_year + 1900;      // tm_year 是从1900开始的年数
//     dt->month = localTime->tm_mon + 1;         // tm_mon 范围是 0-11
//     dt->day = localTime->tm_mday;              // tm_mday 范围是 1-31
//     dt->hour = localTime->tm_hour;             // tm_hour 范围是 0-23
//     dt->minute = localTime->tm_min;            // tm_min 范围是 0-59
//     dt->week_day = localTime->tm_wday;         // tm_wday 范围是 0-6 (0=周日)
// }

// /**
//   * @brief  生成带有时间信息的文件名（含星期信息）
//   * @param  fileSuffix 文件名后半部分
//   * @param  datetime 时间信息
//   * @return 带有时间信息的文件名
//   **/
// std::string CommandTransmitter::generate_file_name(const std::string& fileSuffix, const DateTime_t* datetime) 
// {
//     if (!datetime)
//         return "default_" + fileSuffix;
    
//     const char* weekDays[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    
//     std::ostringstream filename;
    
//     /* 添加时间戳部分: YYYYMMDD_HHMM_Week */
//     filename << std::setw(4) << std::setfill('0') << datetime->year
//                 << std::setw(2) << std::setfill('0') << static_cast<int>(datetime->month)
//                 << std::setw(2) << std::setfill('0') << static_cast<int>(datetime->day)
//                 << "_"
//                 << std::setw(2) << std::setfill('0') << static_cast<int>(datetime->hour)
//                 << std::setw(2) << std::setfill('0') << static_cast<int>(datetime->minute)
//                 << "_"
//                 << weekDays[datetime->week_day % 7];
    
//     /* 添加文件后缀 */
//     filename << "_" << fileSuffix;
    
//     return filename.str();
// }

// /**
//   * @brief  写入优化结果到CSV文件
//   * @param  filename 文件名
//   * @param  optData 寻优结果
//   * @return true 写入文件成功 \ false 打开文件失败
//   **/
// bool CommandTransmitter::write_opt_res_to_csv(const std::string& filename, const OptResData_t * optData) 
// {
//     /* 打开文件，如果文件不存在则创建，如果存在则追加写入 */
//     std::ofstream file(filename, std::ios::app);
    
//     if (!file.is_open())
//         return false; // 文件打开失败
    
//     /* 检查文件是否为空，如果为空则写入表头 */
//     file.seekp(0, std::ios::end);
//     if (file.tellp() == 0)
//         file << "X,Y,P,V1,V2,V3,V4\n";
    
//     /* 设置浮点数输出精度 */
//     file << std::fixed << std::setprecision(6);
    
//     /* 写入数据 */
//     file << optData->motorData.motorX << ","
//             << optData->motorData.motorY << ","
//             << optData->optimalPower << ","
//             << optData->optimalVs[0] << ","
//             << optData->optimalVs[1] << ","
//             << optData->optimalVs[2] << ","
//             << optData->optimalVs[3] << "\n";
    
//     file.close();
//     return true;
// }

// /**
//   * @brief  写入当前通道信息到CSV文件
//   * @param  filename 文件名
//   * @param  channelData 当前通道信息
//   * @return true 写入文件成功 \ false 打开文件失败
//   **/
// bool CommandTransmitter::write_cur_channel_info_to_csv(const std::string& filename, const CurrentVPCh_t * channelData) 
// {
//     /* 打开文件，如果文件不存在则创建，如果存在则追加写入 */
//     std::ofstream file(filename, std::ios::app);
//     static uint32_t index = 0;
    
//     if (!file.is_open())
//         return false; // 文件打开失败
    
//     /* 检查文件是否为空，如果为空则写入表头 */
//     file.seekp(0, std::ios::end);
//     if (file.tellp() == 0)
//         file << "Index,Voltage,Power,Channel\n";
//     else
//         index = 0;
    
//     // 设置浮点数输出精度
//     file << std::fixed << std::setprecision(6);
    
//     // 写入数据
//     file << index << ","  // 自动生成索引
//             << channelData->currentV << ","
//             << channelData->currentP << ","
//             << static_cast<int>(channelData->currentChannel) << "\n";
    
//     file.close();
//     return true;
// }
