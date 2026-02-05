#include "iap_update.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include "stm32f4xx_hal.h"
#include "bsp_spi_flash_hal.h"  // HAL SPI 驱动的 W25Q128 Flash 函数

typedef enum {
    FLAG_IDLE           = 0x7FFFFFFF,
    UPDATE_AVAILABLE    = 0x7FFFFFFE,
    REBOOT_TO_UPDATE    = 0x7FFFFFFC,
    TRIAL_RUNNING       = 0x7FFFFFF8,
    UPDATE_SUCCESS      = 0x7FFFFFF0
} ota_flag_t;  // Flash 只能从 1 变 0，全1表示空闲状态

/* OTA 状态 */
static ota_flag_t otaFlag = FLAG_IDLE;

/* 串口接收缓冲区 */
uint8_t recvBuff[256] = {0};
uint32_t recvNum = 0;
uint32_t addr = ZONE_A_ADDR;
uint8_t recvTime = 0;
uint8_t recvOver = 0;

/* Flash缓冲区 */
static uint16_t readBuff[512];  // 512*2 = 1KB

/* -------------------- 函数声明 -------------------- */
static void ota_flag_write(uint32_t flag);
static uint32_t ota_flag_read(void);
static void update_code(void);
static void rollback_code(void);
static void copy_w25q_to_flash(uint32_t w25_addr, uint32_t flash_addr, uint32_t size);
static void backup_app_to_w25q(uint32_t app_addr, uint32_t w25_addr, uint32_t size);
static HAL_StatusTypeDef erase_APP(void);
static HAL_StatusTypeDef program_flash_data(uint32_t address, uint16_t *data, uint32_t halfWordCount);
static void user_flash_app_run(void);
static void recv_over_fun(void);

/* ---------------------- 标志位操作 ---------------------- */
static void ota_flag_write(uint32_t flag)
{
    HAL_FLASH_Unlock();
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR |
                           FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR |
                           FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, OTA_FLAG_ADDR, flag) != HAL_OK)
    {
//        printf("[IAP]: 写 OTA 标志失败!\r\n");
    }

    HAL_FLASH_Lock();
}

static uint32_t ota_flag_read(void)
{
    return *(volatile uint32_t*)OTA_FLAG_ADDR;
}

/* ---------------------- OTA 流程 ---------------------- */
void check_update_flag(void)
{
    otaFlag = (ota_flag_t)ota_flag_read();

    if(otaFlag == UPDATE_AVAILABLE || otaFlag == REBOOT_TO_UPDATE)
    {
//        printf("[IAP]: 检测到新固件，开始升级...\r\n");
        update_code();
    }
    else
    {
//        printf("[IAP]: 没有新固件，执行原有 APP...\r\n");
        user_flash_app_run();
//        LEDG_OFF();
    }
}

/* ---------------------- 升级固件 ---------------------- */
static void update_code(void)
{
//    printf("[IAP]: 开始从 W25Q128 ZONE_A 升级到内部 Flash...\r\n");

    HAL_FLASH_Unlock();
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR |
                           FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR |
                           FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

    /* 备份当前 APP 到 W25Q128 ZONE_B */
//    printf("[IAP]: 备份当前固件到 W25Q128 ZONE_B...\r\n");
    backup_app_to_w25q(APP_START_SECTOR_ADDR, ZONE_B_ADDR, FW_MAX_SIZE);

    /* 擦除 APP 区域 */
    if (erase_APP() != HAL_OK)
    {
//        printf("[IAP]: 擦除 APP 区失败!\r\n");
        HAL_FLASH_Lock();
        return;
    }

    /* 从 ZONE_A 拷贝到内部 Flash */
    copy_w25q_to_flash(ZONE_A_ADDR, APP_START_SECTOR_ADDR, FW_MAX_SIZE);

    HAL_FLASH_Lock();
//    printf("[IAP]: 升级完成!\r\n");

    /* 更新 OTA 标志 */
    otaFlag = UPDATE_SUCCESS;
    ota_flag_write(otaFlag);

    /* 跳转 APP */
    user_flash_app_run();
}

/* ---------------------- 回滚机制 ---------------------- */
static void rollback_code(void)
{
//    printf("[IAP]: 检测 APP 自检失败，开始回滚...\r\n");

    HAL_FLASH_Unlock();
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR |
                           FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR |
                           FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

    if (erase_APP() != HAL_OK)
    {
//        printf("[IAP]: 擦除 APP 区失败!\r\n");
        HAL_FLASH_Lock();
        return;
    }

    /* 从 ZONE_B 恢复 APP */
    copy_w25q_to_flash(ZONE_B_ADDR, APP_START_SECTOR_ADDR, FW_MAX_SIZE);

    HAL_FLASH_Lock();

    otaFlag = FLAG_IDLE;
    ota_flag_write(otaFlag);

//    printf("[IAP]: 回滚完成，重新执行旧固件...\r\n");
    user_flash_app_run();
}

/* ---------------------- W25Q128 到内部 Flash ---------------------- */
static void copy_w25q_to_flash(uint32_t w25_addr, uint32_t flash_addr, uint32_t size)
{
    uint32_t copied = 0;
    while(copied < size)
    {
        uint32_t chunk = sizeof(readBuff);
        if(copied + chunk > size) chunk = size - copied;

        SPI_FLASH_BufferRead((uint8_t*)readBuff, w25_addr + copied, chunk);
        program_flash_data(flash_addr + copied, readBuff, chunk / 2);

        copied += chunk;
    }
}

/* ---------------------- 内部 Flash 到 W25Q128 ---------------------- */
static void backup_app_to_w25q(uint32_t app_addr, uint32_t w25_addr, uint32_t size)
{
    uint8_t buffer[256];
    uint32_t offset = 0;

    while (offset < size)
    {
        uint32_t len = 256;
        if (offset + len > size)
            len = size - offset;

        memcpy(buffer, (uint8_t *)(app_addr + offset), len);
        SPI_FLASH_PageWrite(buffer, w25_addr + offset, len);

        offset += len;
    }
}

/* ---------------------- 擦除 APP 区 ---------------------- */
static HAL_StatusTypeDef erase_APP(void)
{
    HAL_StatusTypeDef status = HAL_OK;
    FLASH_EraseInitTypeDef eraseInit;
    uint32_t sectorError = 0;

    /* 1. 解锁 Flash */
    HAL_FLASH_Unlock();

    /* 2. 清除 Flash 标志位 */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP  | FLASH_FLAG_OPERR |
                           FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR |
                           FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

    /* 3. 配置擦除参数（公共部分） */
    eraseInit.TypeErase    = FLASH_TYPEERASE_SECTORS;
    eraseInit.VoltageRange = FLASH_VOLTAGE_RANGE_3;
    eraseInit.NbSectors    = 1;

    /* 4. 逐扇区擦除 APP 区 */
    for (uint32_t sector = FLASH_SECTOR_4; sector <= FLASH_SECTOR_10; sector++)
    {
        eraseInit.Sector = sector;

        status = HAL_FLASHEx_Erase(&eraseInit, &sectorError);
        if (status != HAL_OK)
        {
            /* 擦除失败，立即停止 */
            break;
        }
    }

    /* 5. 上锁 Flash */
    HAL_FLASH_Lock();

    return status;
}


/* ---------------------- 内部 Flash 编程 ---------------------- */
static HAL_StatusTypeDef program_flash_data(uint32_t address, uint16_t *data, uint32_t halfWordCount)
{
    for (uint32_t i = 0; i < halfWordCount; i++)
    {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, address + i*2, data[i]) != HAL_OK)
            return HAL_ERROR;
    }
    return HAL_OK;
}

/* ---------------------- 跳转 APP ---------------------- */
static void user_flash_app_run(void)
{
//    printf("[IAP]: 跳转 APP...\r\n");
    if(((*(volatile uint32_t*)(APP_START_SECTOR_ADDR + 4)) & 0xFF000000) == 0x08000000)
    {
        IAP_load_app(APP_START_SECTOR_ADDR);
    }
    else
    {
//        printf("[IAP]: APP 加载失败，尝试回滚...\r\n");
        rollback_code();
    }
}

void IAP_load_app(uint32_t appxAddr)
{
    if(((*(__IO uint32_t*)appxAddr) & 0x2FFE0000) == 0x20000000)
    {
        //禁用所有中断（防止 Bootloader 中断干扰 App）
        __disable_irq();
        // 关闭 SysTick、清除挂起的中断（干净的执行环境）
        SysTick->CTRL = 0;
        SysTick->LOAD = 0;
        SysTick->VAL  = 0;
        for (uint32_t i = 0; i < 8; i++) {
            NVIC->ICER[i] = 0xFFFFFFFF;  // 禁用所有中断
            NVIC->ICPR[i] = 0xFFFFFFFF;  // 清除挂起标志
        }
		// 将中断向量表重定向到应用程序地址
        SCB->VTOR = appxAddr;
        pFunction jumpToApplication = (pFunction)*(uint32_t*)(appxAddr + 4);
        __set_MSP(*(__IO uint32_t*)appxAddr);
        // 重新使能中断
		__enable_irq();
        jumpToApplication();
    }
}

/* ---------------------- OTA 接收完成处理 ---------------------- */
static void recv_over_fun(void)
{
    if(recvOver)
    {
//        printf("[OTA]: 下载完成 %lu 字节\r\n", recvNum);

        recvNum = 0;
        recvOver = 0;
        recvTime = 0;
        addr = ZONE_A_ADDR;

        otaFlag = REBOOT_TO_UPDATE;
        ota_flag_write(otaFlag);

//        printf("[OTA]: 固件已写入 W25Q128，请重启升级\n");
    }
}

/* ---------------------- OTA 接收超时 ---------------------- */
void recv_time_out(void)
{
    if(recvTime)
    {
        recvTime++;
        if(recvTime >= 100)
        {
            recvOver = 1;
            recvTime = 0;
        }
    }

    recv_over_fun();
}
