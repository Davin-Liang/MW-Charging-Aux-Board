#include "file_common.h"
#include "ff.h"      // FatFSͷ�ļ�
#include "stdio.h"   // ����sprintf����
#include "FreeRTOS.h"
#include "task.h"

/**
  * @brief  ����������д��CSV�ļ�
  * @param  path �ļ���
  * @param  historyVoltages
  * @param  historyPowers
  * @return д����
  **/
FRESULT writeArraysToCSV(const TCHAR * path, float * historyVoltages, float * historyPowers) 
{
    FRESULT res;            // FatFS�������
    FATFS fs;               // �ļ�ϵͳ����
    FIL fil;                // �ļ�����
    char buffer[64];        // ���ڸ�ʽ���Ļ�����
		int error;
    
//    taskENTER_CRITICAL();           //�����ٽ���
	
		/* �����ļ�ϵͳ */
    res = f_mount(&fs, "0:", 1);
//    if (res != FR_OK) return res;
	
		/* ���û���ļ�ϵͳ�͸�ʽ�����������ļ�ϵͳ */
		if(res == FR_NO_FILESYSTEM)
		{
			/* ��ʽ�� */
			res=f_mkfs("0:",0,0);							
			
			if(res == FR_OK)
			{
				/* ��ʽ������ȡ������ */
				res = f_mount(NULL,"0:",1);			
				/* ���¹���	*/			
				res = f_mount(&fs,"0:",1);
			}
		}
	
    
    /* ����/���ļ�����������򸲸ǣ� */
    res = f_open(&fil, path, FA_WRITE | FA_CREATE_ALWAYS);
    if (res != FR_OK) 
    {
        f_mount(NULL, "0:", 1); // ж���ļ�ϵͳ
        return res;
    }
    
    /* д��CSV��ͷ */
    f_puts("Index,Voltage,Power\n", &fil);
    
    /* д�������� */
    for (int i = 0; i < 130; i ++) 
    {
        /* ��ʽ�������У�����,��ѹ,���� */
        sprintf(buffer, "%d,%.3f,%.3f\n", i, historyVoltages[i], historyPowers[i]);
				
        
        /* д�뵽�ļ� */
        f_puts(buffer, &fil);
			
//        if (res == FR_OK) continue;
        
//        /* ���д������ر��ļ������ش��� */
//        f_close(&fil);
//        f_mount(NULL, "0:", 1);
//        return res;
    }
    
    /* �ر��ļ� */
    res = f_close(&fil);
    
    /* ж���ļ�ϵͳ */
    f_mount(NULL, "0:", 1);
		
//		taskEXIT_CRITICAL();            //�˳��ٽ���
    
    return res;
}

