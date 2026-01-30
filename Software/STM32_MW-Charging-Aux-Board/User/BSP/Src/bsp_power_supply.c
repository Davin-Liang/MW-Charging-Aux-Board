#include <stm32f4xx_conf.h>
#include "bsp_power_supply.h"

static uint16_t modbus_crc16(const uint8_t *buf, uint16_t len);
static void ps_serial_send_byte(uint8_t byte);
static void ps_serial_send_mul_bytes(uint8_t *bytes, uint16_t length);
static uint16_t psRegAddr[] = {0x01FE, 0x02C6, 0x032A, 0x038E};//5、7、8、9通道写入电压寄存器地址

/**
  * @brief  电源串口(USART2) 初始化 PB10 PB11
	* @param  void
	* @return void
  **/
void usart_power_supply_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
	
	/* GPIO 时钟使能 */
  RCC_AHB1PeriphClockCmd(PS_USART_RX_GPIO_CLK | PS_USART_TX_GPIO_CLK, ENABLE);

  /* 串口时钟使能 */
  RCC_APB1PeriphClockCmd(PS_USART_CLK, ENABLE);
  
  /* GPIO初始化 */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  
  /* 配置Tx引脚为复用功能  */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = PS_USART_TX_PIN;  
  GPIO_Init(PS_USART_TX_GPIO_PORT, &GPIO_InitStructure);

  /* 配置Rx引脚为复用功能 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = PS_USART_RX_PIN;
  GPIO_Init(PS_USART_RX_GPIO_PORT, &GPIO_InitStructure);
  
 /* 连接 PXx 到 USARTx_Tx*/
  GPIO_PinAFConfig(PS_USART_TX_GPIO_PORT, PS_USART_TX_SOURCE, PS_USART_TX_AF);

  /*  连接 PXx 到 USARTx__Rx*/
  GPIO_PinAFConfig(PS_USART_RX_GPIO_PORT, PS_USART_RX_SOURCE, PS_USART_RX_AF);
  
  /* 配置串DEBUG_USART 模式 */
  /* 波特率设置：DEBUG_USART_BAUDRATE */
  USART_InitStructure.USART_BaudRate = PS_USART_BAUDRATE;
  /* 字长(数据位+校验位)：8 */
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  /* 停止位：1个停止位 */
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  /* 校验位选择：不使用校验 */
  USART_InitStructure.USART_Parity = USART_Parity_No;
  /* 硬件流控制：不使用硬件流 */
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  /* USART模式控制：同时使能接收和发送 */
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  /* 完成USART初始化配置 */
  USART_Init(PS_USART, &USART_InitStructure); 
	
  /* 使能串口 */
  USART_Cmd(PS_USART, ENABLE);
}

/**
  * @brief  发送电源设置电压命令
	* @param  slave_addr 从机地址
  * @param  reg_addr 输出通道寄存器地址
  * @param  voltage 设置电压值 (float, 单位 V)
	* @return void
  **/
void set_power_supply_voltage(uint8_t slaveAddr, uint16_t regAddr, float voltage)
{
  uint16_t val = (uint16_t)(voltage * 100); // 假设协议要求以 0.01V 为单位的整数
  uint8_t frame[8];

  frame[0] = slaveAddr; // 从机地址
  frame[1] = 0x06; // 功能码：写单个寄存器
  frame[2] = (regAddr >> 8) & 0xFF; // 寄存器高字节
  frame[3] = regAddr & 0xFF; // 寄存器低字节
  frame[4] = (val >> 8) & 0xFF; // 数据高字节
  frame[5] = val & 0xFF; // 数据低字节

  uint16_t crc = modbus_crc16(frame, 6);
  frame[6] = crc & 0xFF; // CRC 低字节
  frame[7] = (crc >> 8) & 0xFF; // CRC 高字节

  ps_serial_send_mul_bytes(frame, 8);
}

/**
  * @brief  CRC16 (Modbus RTU) 计算函数
	* @param  buf buffer指针
  * @param  len buffer长度
	* @return crc 
  **/
static uint16_t modbus_crc16(const uint8_t *buf, uint16_t len)
{
    uint16_t crc = 0xFFFF;
    for (uint16_t i = 0; i < len; i++)
    {
        crc ^= buf[i];
        for (uint8_t j = 0; j < 8; j++)
        {
            if (crc & 0x0001)
                crc = (crc >> 1) ^ 0xA001;
            else
                crc >>= 1;
        }
    }
    return crc;
}

/**
  * @brief  给电源串口发送单字节
	* @param  byte 字节
	* @return void
  **/
static void ps_serial_send_byte(uint8_t byte)
{
  USART_SendData(PS_USART, byte);
  while (USART_GetFlagStatus(PS_USART, USART_FLAG_TXE) == RESET);
}

/**
  * @brief  给电源串口发送多字节
	* @param  bytes 数组
	* @return void
  **/
static void ps_serial_send_mul_bytes(uint8_t *bytes, uint16_t length)
{
  uint16_t i;
  for (i = 0; i < length; i ++)
  {
    ps_serial_send_byte(bytes[i]);
  }
}

/**
  * @brief  给电源通道设置初始电压
	* @param  voltage 存放初始电压值的数组
	* @return void
  **/
void set_voltage_for_power(float *voltage)
{
	for(int i = 0; i<=3; i++)
	{
		set_power_supply_voltage(0X01, psRegAddr[i], voltage[i]);
		vTaskDelay(2000);//等待电源电压变为0.1
	}
}
