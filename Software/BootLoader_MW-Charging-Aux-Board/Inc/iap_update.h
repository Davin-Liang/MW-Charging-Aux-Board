#ifndef __IAP_UPDATE_H
#define	__IAP_UPDATE_H

#include "stdint.h"
#include "stm32f4xx.h"  //包含了flash相关api文件

//#define BOOT_LOADER_FLAG 1 //定义烧录BOOT_LOADER_FLAG时，表示烧录BOOT_LOADER代码，否则烧录APP代码

//选取PF10引脚的绿色LED灯作为调试灯
#define LEDG_ON()   HAL_GPIO_WritePin(GPIOF, GPIO_PIN_10, GPIO_PIN_SET)
#define LEDG_OFF()  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_10, GPIO_PIN_RESET)

/* 内部 Flash */
#define APP_START_SECTOR_ADDR   0x08010000//FLASH_Sector_4
#define APP_END_SECTOR_ADDR     0x080DFFFF//FLASH_Sector_10
#define FLASH_FLAG_ADDR    0x080E0000//FLASH_Sector_11 // Sector 11，更新标志位

/* 外部 Flash */
#define ZONE_A_ADDR        0x000000     // 下载区（定义下载区和备份区空间都为1MB）
#define ZONE_B_ADDR        0x100000     // 备份区
#define FW_MAX_SIZE        (512*1024)   // 最大固件大小 512KB（即0.5MB）

#define OTA_FLAG_ADDR  FLASH_FLAG_ADDR

typedef  void (*pFunction)(void);   //函数指针  函数指针是一个指针，指向1个函数

/* Flash扇区信息结构体 */
typedef struct {
   uint32_t start_addr;  // 扇区起始地址
   uint32_t size;        // 扇区大小（字节）
   uint8_t sector_num;   // 扇区编号
} FlashSectorInfo_t;

/* 缓冲区配置 */
#define COPY_BLOCK_SIZE     4096 // 每次复制4KB数据
/* 电压范围选择 */
// VoltageRange_1: 1.8V ± 8% 
// VoltageRange_2: 2.1V ± 8%  
// VoltageRange_3: 2.7V ± 8%
// VoltageRange_4: 3.3V ± 8%
#define VOLTAGE_RANGE      FLASH_VOLTAGE_RANGE_3

static void ota_flag_write(uint32_t flag);
static uint32_t ota_flag_read(void);
void check_update_flag(void);
static void update_code(void);
static void rollback_code(void);
static void user_flash_app_run(void);
static HAL_StatusTypeDef erase_APP(void);
static HAL_StatusTypeDef program_flash_data(uint32_t address, uint16_t *data, uint32_t halfWordCount);
static void copy_w25q_to_flash(uint32_t w25_addr,
                               uint32_t flash_addr,
                               uint32_t size);
static void backup_app_to_w25q(uint32_t app_addr,
                               uint32_t w25_addr,
                               uint32_t size);
void IAP_load_app(uint32_t appAddr);


#endif
