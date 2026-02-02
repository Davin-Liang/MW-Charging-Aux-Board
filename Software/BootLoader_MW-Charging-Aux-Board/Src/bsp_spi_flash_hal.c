#include "bsp_spi_flash_hal.h"

/* ================= 内部函数 ================= */
static void SPI_FLASH_Delay(void)
{
    for (volatile uint32_t i = 0; i < 100; i++);
}

/* ================= 初始化 ================= */
void SPI_FLASH_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitStruct.Pin = FLASH_CS_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(FLASH_CS_GPIO_PORT, &GPIO_InitStruct);

    SPI_FLASH_CS_HIGH();
}

/* ================= SPI 读写 ================= */
u8 SPI_FLASH_SendByte(u8 byte)
{
    u8 rx = 0;
    HAL_SPI_TransmitReceive(&hspi1, &byte, &rx, 1, HAL_MAX_DELAY);
    return rx;
}

u8 SPI_FLASH_ReadByte(void)
{
    return SPI_FLASH_SendByte(Dummy_Byte);
}

/* ================= 写使能 ================= */
void SPI_FLASH_WriteEnable(void)
{
    SPI_FLASH_CS_LOW();
    SPI_FLASH_SendByte(W25X_WriteEnable);
    SPI_FLASH_CS_HIGH();
}

/* ================= 等待写完成 ================= */
void SPI_FLASH_WaitForWriteEnd(void)
{
    u8 status;

    SPI_FLASH_CS_LOW();
    SPI_FLASH_SendByte(W25X_ReadStatusReg);

    do {
        status = SPI_FLASH_SendByte(Dummy_Byte);
    } while (status & WIP_Flag);

    SPI_FLASH_CS_HIGH();
}

/* ================= 扇区擦除 ================= */
void SPI_FLASH_SectorErase(u32 SectorAddr)
{
    SPI_FLASH_WriteEnable();

    SPI_FLASH_CS_LOW();
    SPI_FLASH_SendByte(W25X_SectorErase);
    SPI_FLASH_SendByte((SectorAddr >> 16) & 0xFF);
    SPI_FLASH_SendByte((SectorAddr >> 8) & 0xFF);
    SPI_FLASH_SendByte(SectorAddr & 0xFF);
    SPI_FLASH_CS_HIGH();

    SPI_FLASH_WaitForWriteEnd();
}

/* ================= 整片擦除 ================= */
void SPI_FLASH_BulkErase(void)
{
    SPI_FLASH_WriteEnable();

    SPI_FLASH_CS_LOW();
    SPI_FLASH_SendByte(W25X_ChipErase);
    SPI_FLASH_CS_HIGH();

    SPI_FLASH_WaitForWriteEnd();
}

/* ================= 页写 ================= */
void SPI_FLASH_PageWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
    if (NumByteToWrite > SPI_FLASH_PerWritePageSize)
        NumByteToWrite = SPI_FLASH_PerWritePageSize;

    SPI_FLASH_WriteEnable();

    SPI_FLASH_CS_LOW();
    SPI_FLASH_SendByte(W25X_PageProgram);
    SPI_FLASH_SendByte((WriteAddr >> 16) & 0xFF);
    SPI_FLASH_SendByte((WriteAddr >> 8) & 0xFF);
    SPI_FLASH_SendByte(WriteAddr & 0xFF);

    while (NumByteToWrite--)
    {
        SPI_FLASH_SendByte(*pBuffer++);
    }

    SPI_FLASH_CS_HIGH();
    SPI_FLASH_WaitForWriteEnd();
}

/* ================= 连续写 ================= */
void SPI_FLASH_BufferWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
    u16 pageRemain = SPI_FLASH_PageSize - (WriteAddr % SPI_FLASH_PageSize);

    if (NumByteToWrite <= pageRemain)
    {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
    }
    else
    {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, pageRemain);
        pBuffer += pageRemain;
        WriteAddr += pageRemain;
        NumByteToWrite -= pageRemain;

        while (NumByteToWrite >= SPI_FLASH_PageSize)
        {
            SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
            pBuffer += SPI_FLASH_PageSize;
            WriteAddr += SPI_FLASH_PageSize;
            NumByteToWrite -= SPI_FLASH_PageSize;
        }

        if (NumByteToWrite)
        {
            SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
        }
    }
}

/* ================= 读数据 ================= */
void SPI_FLASH_BufferRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)
{
    SPI_FLASH_CS_LOW();
    SPI_FLASH_SendByte(W25X_ReadData);
    SPI_FLASH_SendByte((ReadAddr >> 16) & 0xFF);
    SPI_FLASH_SendByte((ReadAddr >> 8) & 0xFF);
    SPI_FLASH_SendByte(ReadAddr & 0xFF);

    while (NumByteToRead--)
    {
        *pBuffer++ = SPI_FLASH_SendByte(Dummy_Byte);
    }

    SPI_FLASH_CS_HIGH();
}

/* ================= 读 ID ================= */
u32 SPI_FLASH_ReadID(void)
{
    u32 id = 0;

    SPI_FLASH_CS_LOW();
    SPI_FLASH_SendByte(W25X_JedecDeviceID);
    id |= SPI_FLASH_SendByte(Dummy_Byte) << 16;
    id |= SPI_FLASH_SendByte(Dummy_Byte) << 8;
    id |= SPI_FLASH_SendByte(Dummy_Byte);
    SPI_FLASH_CS_HIGH();

    return id;
}

u32 SPI_FLASH_ReadDeviceID(void)
{
    u32 id;

    SPI_FLASH_CS_LOW();
    SPI_FLASH_SendByte(W25X_DeviceID);
    SPI_FLASH_SendByte(0);
    SPI_FLASH_SendByte(0);
    SPI_FLASH_SendByte(0);
    id = SPI_FLASH_SendByte(Dummy_Byte);
    SPI_FLASH_CS_HIGH();

    return id;
}

/* ================= 掉电 / 唤醒 ================= */
void SPI_Flash_PowerDown(void)
{
    SPI_FLASH_CS_LOW();
    SPI_FLASH_SendByte(W25X_PowerDown);
    SPI_FLASH_CS_HIGH();
}

void SPI_Flash_WAKEUP(void)
{
    SPI_FLASH_CS_LOW();
    SPI_FLASH_SendByte(W25X_ReleasePowerDown);
    SPI_FLASH_CS_HIGH();
}
