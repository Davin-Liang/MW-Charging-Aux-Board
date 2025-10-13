#include <stm32f4xx_conf.h>
#include "bsp_power_supply.h"

static uint16_t modbus_crc16(const uint8_t *buf, uint16_t len);
static void ps_serial_send_byte(uint8_t byte);
static void ps_serial_send_mul_bytes(uint8_t *bytes, uint16_t length);
static uint16_t psRegAddr[] = {0x01FE, 0x02C6, 0x032A, 0x038E};//5��7��8��9ͨ��д���ѹ�Ĵ�����ַ

/**
  * @brief  ��Դ����(USART2) ��ʼ�� PB10 PB11
	* @param  void
	* @return void
  **/
void usart_power_supply_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
	
	/* GPIO ʱ��ʹ�� */
  RCC_AHB1PeriphClockCmd(PS_USART_RX_GPIO_CLK | PS_USART_TX_GPIO_CLK, ENABLE);

  /* ����ʱ��ʹ�� */
  RCC_APB1PeriphClockCmd(PS_USART_CLK, ENABLE);
  
  /* GPIO��ʼ�� */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  
  /* ����Tx����Ϊ���ù���  */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = PS_USART_TX_PIN;  
  GPIO_Init(PS_USART_TX_GPIO_PORT, &GPIO_InitStructure);

  /* ����Rx����Ϊ���ù��� */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = PS_USART_RX_PIN;
  GPIO_Init(PS_USART_RX_GPIO_PORT, &GPIO_InitStructure);
  
 /* ���� PXx �� USARTx_Tx*/
  GPIO_PinAFConfig(PS_USART_TX_GPIO_PORT, PS_USART_TX_SOURCE, PS_USART_TX_AF);

  /*  ���� PXx �� USARTx__Rx*/
  GPIO_PinAFConfig(PS_USART_RX_GPIO_PORT, PS_USART_RX_SOURCE, PS_USART_RX_AF);
  
  /* ���ô�DEBUG_USART ģʽ */
  /* ���������ã�DEBUG_USART_BAUDRATE */
  USART_InitStructure.USART_BaudRate = PS_USART_BAUDRATE;
  /* �ֳ�(����λ+У��λ)��8 */
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  /* ֹͣλ��1��ֹͣλ */
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  /* У��λѡ�񣺲�ʹ��У�� */
  USART_InitStructure.USART_Parity = USART_Parity_No;
  /* Ӳ�������ƣ���ʹ��Ӳ���� */
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  /* USARTģʽ���ƣ�ͬʱʹ�ܽ��պͷ��� */
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  /* ���USART��ʼ������ */
  USART_Init(PS_USART, &USART_InitStructure); 
	
  /* ʹ�ܴ��� */
  USART_Cmd(PS_USART, ENABLE);
}

/**
  * @brief  ���͵�Դ���õ�ѹ����
	* @param  slave_addr �ӻ���ַ
  * @param  reg_addr ���ͨ���Ĵ�����ַ
  * @param  voltage ���õ�ѹֵ (float, ��λ V)
	* @return void
  **/
void set_power_supply_voltage(uint8_t slaveAddr, uint16_t regAddr, float voltage)
{
  uint16_t val = (uint16_t)(voltage * 100); // ����Э��Ҫ���� 0.01V Ϊ��λ������
  uint8_t frame[8];

  frame[0] = slaveAddr; // �ӻ���ַ
  frame[1] = 0x06; // �����룺д�����Ĵ���
  frame[2] = (regAddr >> 8) & 0xFF; // �Ĵ������ֽ�
  frame[3] = regAddr & 0xFF; // �Ĵ������ֽ�
  frame[4] = (val >> 8) & 0xFF; // ���ݸ��ֽ�
  frame[5] = val & 0xFF; // ���ݵ��ֽ�

  uint16_t crc = modbus_crc16(frame, 6);
  frame[6] = crc & 0xFF; // CRC ���ֽ�
  frame[7] = (crc >> 8) & 0xFF; // CRC ���ֽ�

  ps_serial_send_mul_bytes(frame, 8);
}

/**
  * @brief  CRC16 (Modbus RTU) ���㺯��
	* @param  buf bufferָ��
  * @param  len buffer����
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
  * @brief  ����Դ���ڷ��͵��ֽ�
	* @param  byte �ֽ�
	* @return void
  **/
static void ps_serial_send_byte(uint8_t byte)
{
  USART_SendData(PS_USART, byte);
  while (USART_GetFlagStatus(PS_USART, USART_FLAG_TXE) == RESET);
}

/**
  * @brief  ����Դ���ڷ��Ͷ��ֽ�
	* @param  bytes ����
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
  * @brief  ����Դͨ�����ó�ʼ��ѹ
	* @param  voltage ��ų�ʼ��ѹֵ������
	* @return void
  **/
void set_voltage_for_power(float *voltage)
{
	for(int i = 0; i<=3; i++)
	{
		set_power_supply_voltage(0X01, psRegAddr[i], voltage[i]);
		vTaskDelay(500);//�ȴ���Դ��ѹ��Ϊ0.1
	}
}
