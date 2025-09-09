#include "file_common.h"
#include "ff.h"      // FatFS头文件
#include "stdio.h"   // 用于sprintf函数
#include "FreeRTOS.h"
#include "task.h"

/**
  * @brief  将数组数据写入CSV文件
  * @param  path 文件名
  * @param  historyVoltages
  * @param  historyPowers
  * @return 写入结果
  **/
FRESULT writeArraysToCSV(const TCHAR * path, float * historyVoltages, float * historyPowers) 
{
    FRESULT res;            // FatFS操作结果
    FATFS fs;               // 文件系统对象
    FIL fil;                // 文件对象
    char buffer[64];        // 用于格式化的缓冲区
		int error;
    
//    taskENTER_CRITICAL();           //进入临界区
	
		/* 挂载文件系统 */
    res = f_mount(&fs, "0:", 1);
//    if (res != FR_OK) return res;
	
		/* 如果没有文件系统就格式化创建创建文件系统 */
		if(res == FR_NO_FILESYSTEM)
		{
			/* 格式化 */
			res=f_mkfs("0:",0,0);							
			
			if(res == FR_OK)
			{
				/* 格式化后，先取消挂载 */
				res = f_mount(NULL,"0:",1);			
				/* 重新挂载	*/			
				res = f_mount(&fs,"0:",1);
			}
		}
	
    
    /* 创建/打开文件（如果存在则覆盖） */
    res = f_open(&fil, path, FA_WRITE | FA_CREATE_ALWAYS);
    if (res != FR_OK) 
    {
        f_mount(NULL, "0:", 1); // 卸载文件系统
        return res;
    }
    
    /* 写入CSV表头 */
    f_puts("Index,Voltage,Power\n", &fil);
    
    /* 写入数据行 */
    for (int i = 0; i < 130; i ++) 
    {
        /* 格式化数据行：索引,电压,功率 */
        sprintf(buffer, "%d,%.3f,%.3f\n", i, historyVoltages[i], historyPowers[i]);
				
        
        /* 写入到文件 */
        f_puts(buffer, &fil);
			
//        if (res == FR_OK) continue;
        
//        /* 如果写入出错，关闭文件并返回错误 */
//        f_close(&fil);
//        f_mount(NULL, "0:", 1);
//        return res;
    }
    
    /* 关闭文件 */
    res = f_close(&fil);
    
    /* 卸载文件系统 */
    f_mount(NULL, "0:", 1);
		
//		taskEXIT_CRITICAL();            //退出临界区
    
    return res;
}

