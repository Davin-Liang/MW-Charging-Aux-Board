#include "file_common.h"
#include "ff.h"      // FatFS头文件
#include "stdio.h"   // 用于sprintf函数
#include "FreeRTOS.h"
#include "file_common.h"
#include "task.h"
#include <string.h>

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

/**
  * @brief  对于每一个通道，将电压和当前功率数据写入CSV文件
  * @param  path 文件名
  * @param  voltage 当前电压
  * @param  power 功率
  * @param  channel 通道
  * @return 写入结果
  **/
FRESULT write_v_p_to_csv(const TCHAR * path, float voltage, float power, int channel)
{
    FRESULT res;
    FIL fil;
    char buffer[64];
    static int index = 0;
    
    // 尝试以追加方式打开现有文件
    res = f_open(&fil, path, FA_WRITE | FA_OPEN_EXISTING);
    
    if (res == FR_OK) 
    {
        // 文件存在，移动到末尾
        f_lseek(&fil, f_size(&fil));
    } 
    else if (res == FR_NO_FILE) 
    {
        // 文件不存在，创建新文件
        res = f_open(&fil, path, FA_WRITE | FA_CREATE_NEW);
        index = 0;
        if (res == FR_OK) 
        {
            // 写入表头
            f_puts("Index,Voltage,Power,Channel\n", &fil);
        }
    }
    
    if (res != FR_OK) 
    {
        return res;
    }
    
    // 写入数据
    sprintf(buffer, "%d,%.3f,%.3f,%d\n", index, voltage, power, channel);
    index ++;
    f_puts(buffer, &fil);
    
    res = f_close(&fil);
    return res;
}

/**
  * @brief  将每一次寻优结果写入 csv 文件
  * @param  path 文件名
  * @param  x 当前接收天线 x 坐标
  * @param  y 当前接收天线 y 坐标
  * @param  optimalP 最优功率
  * @return 写入结果
  **/
FRESULT write_x_y_v_p_to_csv(const TCHAR * path, float x, float y, float * optimalVs, float optimalP)
{
    FRESULT res;
    FIL fil;
    char buffer[64];
    
    // 尝试以追加方式打开现有文件
    res = f_open(&fil, path, FA_WRITE | FA_OPEN_EXISTING);
    
    if (res == FR_OK) 
    {
        // 文件存在，移动到末尾
        f_lseek(&fil, f_size(&fil));
    } 
    else if (res == FR_NO_FILE) 
    {
        // 文件不存在，创建新文件
        res = f_open(&fil, path, FA_WRITE | FA_CREATE_NEW);
        if (res == FR_OK) 
        {
            // 写入表头
            f_puts("X,Y,V1,V2,V3,V4,P\n", &fil);
        }
    }
    
    if (res != FR_OK) 
    {
        return res;
    }
    
    // 写入数据
    sprintf(buffer, "%.3f,%.3f,%.2f,%.2f,%.2f,%.2f,%.3f\n", x, y, optimalVs[0], optimalVs[1], optimalVs[2], optimalVs[3], optimalP);
    f_puts(buffer, &fil);
		
		f_lseek(&fil, 0);  // 偏移量设为0
		
//		printf("size = %d", f_size(&fil));
//		
//    printf("文件内容:\r\n");
//    printf("--------------------------------\r\n");
    
//    // 循环读取文件内容
//		UINT br;     // 读取的字节数
//   
//        // 读取文件内容到缓冲区
//        f_read(&fil, buffer, sizeof(buffer) - 1, &br);
//		
//				printf("br = %d", (int)br);
//        
//        // 在缓冲区末尾添加字符串结束符
//        buffer[br] = '\0';
//        
//        // 使用printf打印内容
//        printf("%s", buffer);
//    
//    
//    printf("\r\n--------------------------------\r\n");
//    printf("文件读取完成\r\n");
    
    res = f_close(&fil);
    return res;
}

