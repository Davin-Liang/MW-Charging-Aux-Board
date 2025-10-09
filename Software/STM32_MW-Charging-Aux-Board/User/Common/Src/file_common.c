#include "file_common.h"
#include "ff.h"      // FatFSͷ�ļ�
#include "stdio.h"   // ����sprintf����
#include "FreeRTOS.h"
#include "file_common.h"
#include "task.h"
#include <string.h>

//UBaseType_t tackGet0;
UBaseType_t tackGet;
UBaseType_t tackGet1;
UBaseType_t tackGet2;


/**
  * @brief  ����������д��CSV�ļ�
  * @param  path �ļ���
  * @param  historyVoltages
  * @param  historyPowers
  * @return д����
  **/
FRESULT write_arrays_to_CSV(const TCHAR * path, float * historyVoltages, float * historyPowers) 
{
    FRESULT res;            // FatFS�������
    FIL fil;                // �ļ�����
    char buffer[64];        // ���ڸ�ʽ���Ļ�����
	  
    /* ����/���ļ�����������򸲸ǣ� */
    res = f_open(&fil, path, FA_WRITE | FA_CREATE_ALWAYS);
//    if (res != FR_OK) 
//    {
//        f_mount(NULL, "0:", 1); // ж���ļ�ϵͳ
//        return res;
//    }
    
    /* д��CSV��ͷ */
    f_puts("Index,Voltage,Power\n", &fil);
    
    /* д�������� */
    for (int i = 0; i < 50; i ++) 
    {
        /* ��ʽ�������У�����,��ѹ,���� */
        sprintf(buffer, "%d,%.3f,%.3f\n", i, historyVoltages[i], historyPowers[i]);		
        
        /* д�뵽�ļ� */
        f_puts(buffer, &fil);
    }
    
    /* �ر��ļ� */
    res = f_close(&fil);
    
    return res;
}

/**
  * @brief  ����ÿһ��ͨ��������ѹ�͵�ǰ��������д��CSV�ļ�
  * @param  path �ļ���
  * @param  voltage ��ǰ��ѹ
  * @param  power ����
  * @param  index ����
  * @param  channel ͨ��
  * @return д����
  **/
FRESULT write_v_p_to_csv(const TCHAR * path, float voltage, float power, int index, int channel)
{
    FRESULT res;
    FIL fil;
    char buffer[64];
    
    // ������׷�ӷ�ʽ�������ļ�
    res = f_open(&fil, path, FA_WRITE | FA_OPEN_EXISTING);
    
    if (res == FR_OK) 
    {
        // �ļ����ڣ��ƶ���ĩβ
        f_lseek(&fil, f_size(&fil));
    } 
    else if (res == FR_NO_FILE) 
    {
        // �ļ������ڣ��������ļ�
        res = f_open(&fil, path, FA_WRITE | FA_CREATE_NEW);
        if (res == FR_OK) 
        {
            // д���ͷ
            f_puts("Index,Voltage,Power,Channel\n", &fil);
        }
    }
    
    if (res != FR_OK) 
    {
        return res;
    }
    
    // д������
    sprintf(buffer, "%d,%.3f,%.3f,%d\n", index, voltage, power, channel);
    f_puts(buffer, &fil);
    
    res = f_close(&fil);
    return res;
}

/**
  * @brief  ��ÿһ��Ѱ�Ž��д�� csv �ļ�
  * @param  path �ļ���
  * @param  x ��ǰ�������� x ����
  * @param  y ��ǰ�������� y ����
  * @param  optimalP ���Ź���
  * @return д����
  **/
FRESULT write_x_y_v_p_to_csv(const TCHAR * path, float x, float y, float * optimalVs, float optimalP)
{
    FRESULT res;
    FIL fil;
    char buffer[64];
    
    // ������׷�ӷ�ʽ�������ļ�
    res = f_open(&fil, path, FA_WRITE | FA_OPEN_EXISTING);
    
    if (res == FR_OK) 
    {
        // �ļ����ڣ��ƶ���ĩβ
        f_lseek(&fil, f_size(&fil));
    } 
    else if (res == FR_NO_FILE) 
    {
        // �ļ������ڣ��������ļ�
        res = f_open(&fil, path, FA_WRITE | FA_CREATE_NEW);
        if (res == FR_OK) 
        {
            // д���ͷ
            f_puts("X,Y,P\n", &fil);
        }
    }
    
    if (res != FR_OK) 
    {
        return res;
    }
    
    // д������
    sprintf(buffer, "%.3f,%.3f,%.3f\n", x, y, optimalP);
    f_puts(buffer, &fil);
    
    res = f_close(&fil);
    return res;
}

