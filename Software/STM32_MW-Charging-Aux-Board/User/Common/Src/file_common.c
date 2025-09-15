#include "file_common.h"
#include "ff.h"      // FatFSͷ�ļ�
#include "stdio.h"   // ����sprintf����
#include "FreeRTOS.h"
#include "task.h"

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

