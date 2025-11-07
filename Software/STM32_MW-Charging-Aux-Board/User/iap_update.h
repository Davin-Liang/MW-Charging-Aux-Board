// #ifndef __IAP_UPDATE_H
// #define	__IAP_UPDATE_H

// #include "stdint.h"

// typedef  void (*pFunction)(void);   //函数指针  函数指针是一个指针，指向1个函数

// /* Flash扇区信息结构体 */
// typedef struct {
//    uint32_t start_addr;  // 扇区起始地址
//    uint32_t size;        // 扇区大小（字节）
//    uint8_t sector_num;   // 扇区编号
// } FlashSectorInfo_t;

// /* 应用地址定义 */
// /* 设定APP1从扇区x开始，填写该扇区的地址：扇区1 ~ 扇区5 */
// #define FLASH_APP1_ADDR    0x08004000
// /* 假设APP2从扇区x开始，填写该扇区的地址：扇区13 ~ 扇区16 */
// #define FLASH_APP2_ADDR    0x08104000
// // #define APP1_FLAG_ADDR	   (FLASH_APP2_ADDR-4) //APP1是否有更新程序标志位 
// /* APP2是否有更新程序标志位 扇区起始地址 + 该扇区大小 - 4 */
// #define APP2_FLAG_ADDR	   (0x08110000 + (64 * 1024) - 4)
// #define APP_TOTAL_SIZE     (112 * 1024) // 应用大小为 16 + 16 + 16 + 64
// /* 缓冲区配置 */
// #define COPY_BLOCK_SIZE     4096 // 每次复制4KB数据
// /* 电压范围选择 */
// // VoltageRange_1: 1.8V ± 8% 
// // VoltageRange_2: 2.1V ± 8%  
// // VoltageRange_3: 2.7V ± 8%
// // VoltageRange_4: 3.3V ± 8%
// #define VOLTAGE_RANGE      VoltageRange_3

// void check_update_flag(void);
// void NVIC_SETVectorTable(void);
// void recv_time_out(void);

// #endif
