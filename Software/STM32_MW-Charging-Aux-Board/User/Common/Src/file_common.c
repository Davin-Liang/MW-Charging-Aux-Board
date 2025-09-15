#include "file_common.h"
#include "ff.h"      // FatFS头文件
#include "stdio.h"   // 用于sprintf函数
#include "FreeRTOS.h"
#include "task.h"

//UBaseType_t tackGet0;
UBaseType_t tackGet;
UBaseType_t tackGet1;
UBaseType_t tackGet2;


/**
  * @brief  将数组数据写入CSV文件
  * @param  path 文件名
  * @param  historyVoltages
  * @param  historyPowers
  * @return 写入结果
  **/
FRESULT write_arrays_to_CSV(const TCHAR * path, float * historyVoltages, float * historyPowers) 
{
    FRESULT res;            // FatFS操作结果
    FIL fil;                // 文件对象
    char buffer[64];        // 用于格式化的缓冲区
	  
    /* 创建/打开文件（如果存在则覆盖） */
    res = f_open(&fil, path, FA_WRITE | FA_CREATE_ALWAYS);
//    if (res != FR_OK) 
//    {
//        f_mount(NULL, "0:", 1); // 卸载文件系统
//        return res;
//    }
    
    /* 写入CSV表头 */
    f_puts("Index,Voltage,Power\n", &fil);
    
    /* 写入数据行 */
    for (int i = 0; i < 50; i ++) 
    {
        /* 格式化数据行：索引,电压,功率 */
        sprintf(buffer, "%d,%.3f,%.3f\n", i, historyVoltages[i], historyPowers[i]);		
        
        /* 写入到文件 */
        f_puts(buffer, &fil);
    }
    
    /* 关闭文件 */
    res = f_close(&fil);
    
    return res;
}

