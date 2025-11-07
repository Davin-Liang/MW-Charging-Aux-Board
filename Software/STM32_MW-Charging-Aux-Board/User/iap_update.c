// #include "iap_update.h"
// #include "stdint.h"
// #include "stdio.h"
// #include "stm32f4xx.h"

// static FLASH_Status erase_APP1_by_sectors(void);
// static FLASH_Status erase_APP2_by_sectors(void);
// static FLASH_Status program_flash_data(uint32_t address, uint16_t *data, uint32_t half_word_count);
// static const FlashSectorInfo_t* find_sector_info(uint32_t addr);
// static void update_code(void);
// static void user_flash_app_run(void);
// static void IAP_load_app(u32 appxAddr);
// static void recv_over_fun(void);

// uint8_t recvBuff[2] = {0};   //存放准备写入APP2的数据，在串口中断中调用，没收到2个字节，写入一次
// uint32_t recvNum = 0;   //接收的数量  标记升级文件的大小
// uint32_t addr = FLASH_APP2_ADDR;   //写入APP2的地址   最开始是APP2区域的起始地址
// uint8_t recvTime = 0;  //用来判断是否接收完成
// uint8_t recvOver = 0;   //升级文件接收完成  1接收完成
// uint16_t app2FlagBuff[2] = {0xAAAA, 0xAAAA};  //APP2区域是否有升级文件的标志  获取升级文件完成之后，写入APP2区域有升级文件的标记

// /* 读取的缓冲区 */
// static uint16_t readBuff[COPY_BLOCK_SIZE / 2] = {0};

// /**
//  * @brief  检查是否有更新标志：
//            1. 有更新标志————将APP2区域的拷贝到APP1区域，并且跳转到APP1区域执行
//            2. 没有更新标志————执行原有APP1区域的代码
//            3. APP1和APP2区域都没有代码的话，不跳转,执行bootloader
//  * @param  void
//  * @return void
//  * @note   放在 Bootloader 程序的 main 函数里边执行
//  **/
// void check_update_flag(void)
// {
// 	uint16_t app2FlagBuff[2] = {0};

//    /* 1. 直接从Flash读取APP2区域的标志位 */
//    /* 使用memcpy或直接指针访问读取Flash数据 */
//    memcpy(app2FlagBuff, (void*)APP2_FLAG_ADDR, sizeof(app2FlagBuff));

// 	if (app2FlagBuff[0] == 0xAAAA && app2FlagBuff[1] == 0xAAAA) 
//    {
// 		/* APP2区域有更新程序 */
// 		printf("[IAP]: 有更新程序,正在执行代码升级... ...\r\n");
// 		update_code(); // 2.将程序从APP2搬运到APP1

//         return;
// 	}

//     /* APP2区域没有新的程序 */
//     printf("[IAP]: 没有新的程序,执行原有APP.\r\n");
//     user_flash_app_run(); //3.没有新的APP2程序，执行原有的APP1
// }

// /* Flash扇区映射表（根据您的芯片型号调整） */
// const FlashSectorInfo_t g_flashSectorMap[] = { // TODO:
//    {0x08000000, 16*1024,  FLASH_Sector_0},
//    {0x08004000, 16*1024,  FLASH_Sector_1},
//    {0x08008000, 16*1024,  FLASH_Sector_2},
//    {0x0800C000, 16*1024,  FLASH_Sector_3},
//    {0x08010000, 64*1024,  FLASH_Sector_4},
//    {0x08020000, 128*1024, FLASH_Sector_5},
//    {0x08040000, 128*1024, FLASH_Sector_6},
//    {0x08060000, 128*1024, FLASH_Sector_7},
//    {0x08080000, 128*1024, FLASH_Sector_8},
//    {0x080A0000, 128*1024, FLASH_Sector_9},
//    {0x080C0000, 128*1024, FLASH_Sector_10},
//    {0x080E0000, 128*1024, FLASH_Sector_11},
//    {0x08100000, 16*1024,  FLASH_Sector_12},
//    {0x08104000, 16*1024,  FLASH_Sector_13},
//    {0x08108000, 16*1024,  FLASH_Sector_14},
//    {0x0810C000, 16*1024,  FLASH_Sector_15},
//    {0x08110000, 64*1024,  FLASH_Sector_16},
//    {0x08120000, 128*1024, FLASH_Sector_17},
// 	{0x08120000, 128*1024, FLASH_Sector_18},
// 	{0x08120000, 128*1024, FLASH_Sector_19},
// 	{0x08120000, 128*1024, FLASH_Sector_20},
// 	{0x08120000, 128*1024, FLASH_Sector_21},
// 	{0x08120000, 128*1024, FLASH_Sector_22},
// 	{0x08120000, 128*1024, FLASH_Sector_23},
//    // ... 根据您的实际芯片添加更多扇区
// };
// /* 扇区数量大小 */
// #define FLASH_SECTOR_COUNT (sizeof(g_flashSectorMap) / sizeof(FlashSectorInfo_t))

// /**
//  * @brief  固件更新函数
//  * @param  void
//  * @return void
//  * @note   放在 Bootloader 程序里边执行
//  **/
// static void update_code(void)
// {  
//    printf("[IAP]: 开始更新固件... ...\r\n");
   
//    /* 1. 解锁Flash */
//    FLASH_Unlock();
//    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
//                   FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
   
//    /* 2. 擦除APP1区域 */
//    if (erase_APP1_by_sectors() != FLASH_COMPLETE) 
// 	{
//        printf("[IAP]: APP1擦除失败!\r\n");
//        FLASH_Lock();
//        return;
//    }
   
//    /* 3. 复制数据从APP2到APP1 */
//    uint32_t currentApp2Addr = FLASH_APP2_ADDR;
//    uint32_t currentApp1Addr = FLASH_APP1_ADDR;
//    uint32_t totalSizeCopied = 0;
   
//    while (totalSizeCopied < APP_TOTAL_SIZE) 
// 	{
//        /* 查找当前APP地址对应的扇区信息 */
//        const FlashSectorInfo_t* srcSector = find_sector_info(currentApp2Addr);
//        const FlashSectorInfo_t* destSector = find_sector_info(currentApp1Addr);
       
//        if (!srcSector || !destSector) 
// 		{
//            printf("[IAP]: 地址超出范围: APP2=0x%08lX, APP1=0x%08lX\r\n", currentApp2Addr, currentApp1Addr);
//            break;
//        }
       
//        /* 计算本次可复制的数据量（取两个扇区剩余空间的最小值） */
// 		// 确保每次复制的数据不会跨越两个扇区
//        uint32_t srcRemaining = srcSector->start_addr + srcSector->size - currentApp2Addr;
//        uint32_t destRemaining = destSector->start_addr + destSector->size - currentApp1Addr;
//        uint32_t copySize = (srcRemaining < destRemaining) ? srcRemaining : destRemaining;
       
//        /* 限制单次复制大小（避免缓冲区溢出） */
//        if (copySize > sizeof(readBuff))
//            copySize = sizeof(readBuff);
       
//        /* 确保不会超过总大小 */
//        if (totalSizeCopied + copySize > APP_TOTAL_SIZE)
//            copySize = APP_TOTAL_SIZE - totalSizeCopied;
       
//        printf("[IAP]: 复制: 从0x%08lX到0x%08lX, 大小:%lu字节\r\n", currentApp2Addr, currentApp1Addr, copySize);
       
//        /* 从APP2读取数据 */
// 		// memcpy函数可以直接读取STM32F4系列的Flash内存
//        memcpy(readBuff, (void*)currentApp2Addr, copySize);
       
//        /* 写入到APP1 */
//        if (program_flash_data(currentApp1Addr, readBuff, copySize / 2) != FLASH_COMPLETE) 
// 		{
//            printf("[IAP]: 编程失败在地址: 0x%08lX\r\n", currentApp1Addr);
//            FLASH_Lock();
//            return;
//        }
       
//        /* 更新地址和计数器 */
//        currentApp2Addr += copySize;
//        currentApp1Addr += copySize;
//        totalSizeCopied += copySize;
       
//        printf("[IAP]: 进度: %lu/%lu 字节 (%.1f%%)\r\n", 
//               totalSizeCopied, APP_TOTAL_SIZE, 
//               (totalSizeCopied * 100.0) / APP_TOTAL_SIZE);
//    }
   
//    /* 4. 锁定Flash */
//    FLASH_Lock();
   
//    printf("[IAP]: 固件更新完成! 总共复制%lu字节\r\n", totalSizeCopied);
   
//    /* 5. 擦除APP2区域 */
//    erase_APP2_by_sectors();
   
//    /* 6. 跳转到APP1执行 */
//    user_flash_app_run();
// }

// /**
//  * @brief  该函数通过遍历Flash扇区映射表，根据给定的地址查找对应的Flash扇区信息，
//            返回找到的扇区信息指针，若未找到则返回NULL。
//  * @param  addr: 要查找的地址
//  * @return 扇区信息指针，NULL表示未找到
//  */
// static const FlashSectorInfo_t* find_sector_info(uint32_t addr)
// {
//    for (int i = 0; i < FLASH_SECTOR_COUNT; i ++) 
// 	{
//        if (addr >= g_flashSectorMap[i].start_addr && 
//            addr < (g_flashSectorMap[i].start_addr + g_flashSectorMap[i].size)) 
// 		{
//            return &g_flashSectorMap[i];
//        }
//    }
//    return NULL;
// }

// /**
//  * @brief 按扇区擦除APP1区域
//  * @param void
//  * @return Flash擦除状态
//  **/
// static FLASH_Status erase_APP1_by_sectors(void)
// {
//    FLASH_Status status = FLASH_COMPLETE;
//    uint32_t addr = FLASH_APP1_ADDR;
   
//    while (addr < (FLASH_APP1_ADDR + APP_TOTAL_SIZE)) 
// 	{
//        const FlashSectorInfo_t* sector = find_sector_info(addr);
//        if (!sector) break;
       
//        printf("[IAP]: 擦除APP1扇区%d...\r\n", sector->sector_num);
//        status = FLASH_EraseSector(sector->sector_num, VOLTAGE_RANGE);
//        if (status != FLASH_COMPLETE) break;
       
//        addr = sector->start_addr + sector->size; // 移动到下一个扇区
//    }
   
//    return status;
// }

// /**
//  * @brief 按扇区擦除APP2区域
//  * @param void
//  * @return Flash擦除状态
//  **/
// static FLASH_Status erase_APP2_by_sectors(void)
// {
//    FLASH_Status status = FLASH_COMPLETE;
//    uint32_t addr = FLASH_APP2_ADDR;
   
//    while (addr < (FLASH_APP2_ADDR + APP_TOTAL_SIZE)) 
// 	{
//        const FlashSectorInfo_t* sector = find_sector_info(addr);
//        if (!sector) break;
       
//        printf("[IAP]: 擦除APP2扇区%d...\r\n", sector->sector_num);
//        status = FLASH_EraseSector(sector->sector_num, VOLTAGE_RANGE);
//        if (status != FLASH_COMPLETE) break;
       
//        addr = sector->start_addr + sector->size;
//    }
   
//    return status;
// }

// /**
//  * @brief  Flash编程函数，支持任意大小数据
//  * @param	address 		要写入的地址
//  * @param	data			数据
//  * @param	halfWordCount 半字的数量
//  * @return	Flash写入状态
//  */
// static FLASH_Status program_flash_data(uint32_t address, uint16_t *data, uint32_t halfWordCount)
// {
//    FLASH_Status status = FLASH_COMPLETE;
   
//    for (uint32_t i = 0; i < halfWordCount; i++) 
// 	{
//        status = FLASH_ProgramHalfWord(address + i * 2, data[i]);
//        if (status != FLASH_COMPLETE) 
//            break;
//    }
   
//    return status;
// }

// /**
//  * @brief  跳转到Flash中用户代码执行
//  * @param  void
//  * @return void
//  **/
// static void user_flash_app_run(void)
// {
// 	printf("[IAP]: 开始执行FLASH用户代码!!\r\n");  

// 	/* 判断是否为0X08XXXXXX.  如果不是表示地址不合法 */	
// 	// FLASH_APP1_ADDR 为应用程序起始地址，(FLASH_APP1_ADDR + 4) 为中断向量表起始地址
// 	if (((*(vu32*)(FLASH_APP1_ADDR + 4)) & 0xFF000000) == 0x08000000)
// 	{	 
// 		printf("[IAP]: 成功跳转APP1区域执行\r\n");
// 		/* 执行FLASH APP1代码 */
// 		IAP_load_app(FLASH_APP1_ADDR);
// 	}else 
// 		printf("[IAP]: APP程序加载失败!\r\n");   							 
// }

// pFunction jumpToApplication;
// /**
//  * @brief  跳转到应用程序段
//  * @param  appxaddr 用户代码起始地址.
//  * @return void
//  **/
// static void IAP_load_app(u32 appxAddr)
// {
// 	/* 栈顶指针的高15位（位31-17）必须等于 0x2000 */
// 	// 确保栈在RAM中：STM32的RAM通常从 0x20000000 开始
// 	// 粗略的范围检查：不是精确检查，而是确保地址在合理的RAM范围内
// 	if (((*(__IO uint32_t*)appxAddr) & 0x2FFE0000) == 0x20000000)
// 	{ 
// 		jumpToApplication = (pFunction)*(uint32_t*)(appxAddr + 4); // 用户代码区第二个字为程序开始地址(复位地址)	
// 		/* 设置主堆栈指针(MSP)为应用程序的栈顶值 */	
// 		// 初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)   在core_cm3.c  28行
// 		__set_MSP(*(__IO uint32_t*)appxAddr); 
// 		jumpToApplication(); // 跳转到APP.
// 	}	
// }

// //生成二进制文件
// //D:\MDK5\ARM\ARMCC\bin\fromelf.exe --bin -o .\Objects\Demo.bin .\Objects\Demo.axf
// /**
//  * @brief  判断从串口发送的升级文件是否发送完成
//  * 		如何确定  最后一个字节收到之后，计时会溢出
//  * @param  void
//  * @return void
//  **/
// static void recv_over_fun(void)
// {
// 	if (recvOver == 1) 
// 	{
// 		printf("APP数据接收完成:%d\r\n", recvNum);
// 		recvNum = 0;
// 		recvOver = 0;
// 		recvTime = 0;
// 		addr = FLASH_APP2_ADDR;
		
// 		FLASH_ProgramHalfWord(APP2_FLAG_ADDR, app2FlagBuff[0]);    //写APP2区域有升级文件的标志
// 		FLASH_ProgramHalfWord(APP2_FLAG_ADDR+2, app2FlagBuff[0]);
// 		printf("核对数据无误后，请按下复位按键进行数据更新\r\n");   //也可以选择调用复位函数   看门狗复位   NVIC_SystemReset();

// 		/* 关闭空闲中断 */
// 		// TODO:
// 	}
// }

// /**
//  * @brief  修改中断向量表的地址偏移
//  * @param  void
//  * @return void
//  * @note   在 app1 的 main 函数中运行
//  **/
// void NVIC_SETVectorTable(void)
// {
// 	// Boot 程序大小为 12k=0x3000
// 	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x3000);
// }

// /**
//  * @brief  超时判断，定时执行recv_time_out，开启串口的空闲中断，
//  * @param  void
//  * @return void
//  * @note   在定时器的定时中断中运行
//  **/
// void recv_time_out(void) // 1ms一次
// {
// 	if (recvTime) 
// 	{
// 		recvTime ++;
// 		if (recvTime >= 100) 
// 		{
// 			recvOver = 1;
// 			recvTime = 0;
// 		}
// 	}

// 	recv_over_fun();
// }
