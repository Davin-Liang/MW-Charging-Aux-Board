#ifndef __SPI_FLASH_H
#define __SPI_FLASH_H

#include "stm32f4xx_hal.h"
#include <stdio.h>

/* ================= Flash ÐÍºÅ ================= */
#define sFLASH_ID      0xEF4018    // W25Q128

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;

/* ================= Flash ²ÎÊý ================= */
#define SPI_FLASH_PageSize             256
#define SPI_FLASH_PerWritePageSize     256

/* ================= W25Q ÃüÁî ================= */
#define W25X_WriteEnable           0x06
#define W25X_WriteDisable          0x04
#define W25X_ReadStatusReg         0x05
#define W25X_WriteStatusReg        0x01
#define W25X_ReadData              0x03
#define W25X_FastReadData          0x0B
#define W25X_PageProgram           0x02
#define W25X_SectorErase           0x20
#define W25X_ChipErase             0xC7
#define W25X_PowerDown             0xB9
#define W25X_ReleasePowerDown      0xAB
#define W25X_DeviceID              0xAB
#define W25X_JedecDeviceID         0x9F

#define WIP_Flag                   0x01
#define Dummy_Byte                 0xFF

/* ================= CS Òý½ÅÅäÖÃ ================= */
#define FLASH_CS_GPIO_PORT         GPIOB
#define FLASH_CS_PIN               GPIO_PIN_14

#define SPI_FLASH_CS_LOW()         HAL_GPIO_WritePin(FLASH_CS_GPIO_PORT, FLASH_CS_PIN, GPIO_PIN_RESET)
#define SPI_FLASH_CS_HIGH()        HAL_GPIO_WritePin(FLASH_CS_GPIO_PORT, FLASH_CS_PIN, GPIO_PIN_SET)

/* ================= SPI ¾ä±ú ================= */
extern SPI_HandleTypeDef hspi1;

/* ================= API ================= */
void SPI_FLASH_Init(void);
void SPI_FLASH_SectorErase(u32 SectorAddr);
void SPI_FLASH_BulkErase(void);
void SPI_FLASH_PageWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);
void SPI_FLASH_BufferWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);
void SPI_FLASH_BufferRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead);
u32  SPI_FLASH_ReadID(void);
u32  SPI_FLASH_ReadDeviceID(void);
void SPI_FLASH_StartReadSequence(u32 ReadAddr);
void SPI_Flash_PowerDown(void);
void SPI_Flash_WAKEUP(void);

u8   SPI_FLASH_ReadByte(void);
u8   SPI_FLASH_SendByte(u8 byte);
void SPI_FLASH_WriteEnable(void);
void SPI_FLASH_WaitForWriteEnd(void);

#endif
