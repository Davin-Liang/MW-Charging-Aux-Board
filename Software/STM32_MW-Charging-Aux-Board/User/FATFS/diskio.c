/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2014        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
#include "ff.h"
#include "bsp_sdio_sd.h"
#include "string.h"

/* Ϊÿ���豸����һ�������� */
#define ATA			           0     // SD��
#define SPI_FLASH		       1     // Ԥ���ⲿSPI Flashʹ��

#define SD_BLOCKSIZE     512 

extern  SD_CardInfo SDCardInfo;

/**
  * @brief  ��ȡ�豸״̬
  * @param  pdrv ������   
  * @return DSTATUS �豸״̬
  **/
DSTATUS disk_status(BYTE pdrv)
{
	DSTATUS status = STA_NOINIT;
	
	switch (pdrv) 
	{
		case ATA:	/* SD CARD */
			status &= ~STA_NOINIT;
			break;
    
		case SPI_FLASH:        /* SPI Flash */   
			break;

		default:
			status = STA_NOINIT;
	}
	return status;
}

/**
  * @brief  �豸��ʼ��
  * @param  pdrv ������   
  * @return DSTATUS �豸״̬
  **/
DSTATUS disk_initialize(BYTE pdrv)
{
	DSTATUS status = STA_NOINIT;	
	switch (pdrv) 
	{
		case ATA:	         /* SD CARD */
			if(SD_Init() == SD_OK) // ������ó�ʼ������
			{
				status &= ~STA_NOINIT;
			}
			else 
			{
				status = STA_NOINIT;
			}
		
			break;
    
		case SPI_FLASH:    /* SPI Flash */ 
			break;
      
		default:
			status = STA_NOINIT;
	}
	return status;
}

/**
  * @brief  ����������ȡ�������ݵ�ָ���洢��
  * @param  pdrv ������   
  * @param  buff ���ݻ�����
  * @param  sector �����׵�ַ
  * @param  count ��������(1..128)
  * @return DRESULT ��ȡ״̬
  **/
DRESULT disk_read(BYTE pdrv, BYTE *buff, DWORD sector,	UINT count)
{
	DRESULT status = RES_PARERR;
	SD_Error SD_state = SD_OK;
	
	switch (pdrv) 
	{
		case ATA:	/* SD CARD */
			/*
			 * ����ַ�������λ�Ƿ�Ϊ�㣬��������Ϊ�㣬
			 * ˵����ַ�������λ������һλ��1��
			 * ����ζ�Ÿõ�ַ����4�ı�����������4�ֽڶ���ģ�
			 */						
		  	if((DWORD)buff & 3)
			{
				DRESULT res = RES_OK;
				/* ����Ϊ DWORD ���Ϳ���ʹ�����Զ� 4 �ֽڶ��� */
				DWORD scratch[SD_BLOCKSIZE / 4]; // ������һ������128��DWORD�����飬�ܴ�С������512�ֽ�

				while (count--) 
				{
					res = disk_read(ATA, (void *)scratch, sector++, 1);

					if (res != RES_OK) 
					{
						break;
					}
					memcpy(buff, scratch, SD_BLOCKSIZE); // �����ݴӶ���� scratch ���Ƶ��û�ָ���ġ����ܲ������ buff ��
					buff += SD_BLOCKSIZE;
		    	}

		    	return res;
			}
			
			SD_state = SD_ReadMultiBlocks(buff,sector*SD_BLOCKSIZE,SD_BLOCKSIZE,count);
		  	if (SD_state == SD_OK)
			{
				/* Check if the Transfer is finished */
				SD_state = SD_WaitReadOperation();
				while (SD_GetStatus() != SD_TRANSFER_OK);
			}
			if (SD_state != SD_OK)
				status = RES_PARERR;
		  	else
			  status = RES_OK;	
			break;   
			
		case SPI_FLASH:
		break;
    
		default:
			status = RES_PARERR;
	}
	return status;
}

#if _USE_WRITE
/**
  * @brief  д��������ָ���洢���ϵ�����д��ָ������
  * @param  pdrv ������   
  * @param  buff ��д�����ݵĻ�����
  * @param  sector �����׵�ַ
  * @param  count ��������(1..128)
  * @return DRESULT ��ȡ״̬
  **/
DRESULT disk_write(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count)
{
	DRESULT status = RES_PARERR;
	SD_Error SD_state = SD_OK;
	
	if (!count) 
	{
		return RES_PARERR;		/* Check parameter */
	}

	switch (pdrv) 
	{
		case ATA:	/* SD CARD */  
			if ((DWORD)buff & 3)
			{
				DRESULT res = RES_OK;
				DWORD scratch[SD_BLOCKSIZE / 4];

				while (count--) 
				{
					memcpy(scratch, buff, SD_BLOCKSIZE);
					res = disk_write(ATA, (void *)scratch, sector++, 1);
					if (res != RES_OK) 
					{
						break;
					}					
					buff += SD_BLOCKSIZE;
				}
				return res;
			}		
		
			SD_state = SD_WriteMultiBlocks((uint8_t *)buff, sector * SD_BLOCKSIZE, SD_BLOCKSIZE, count);
			if (SD_state == SD_OK)
			{
				/* Check if the Transfer is finished */
				SD_state = SD_WaitWriteOperation();

				/* Wait until end of DMA transfer */
				while (SD_GetStatus() != SD_TRANSFER_OK);			
			}
			if (SD_state != SD_OK)
				status = RES_PARERR;
		  else
			  status = RES_OK;	
		break;

		case SPI_FLASH:
		break;
    
		default:
			status = RES_PARERR;
	}
	return status;
}

#endif

#if _USE_IOCTL
/**
  * @brief  ��������
  * @param  pdrv ������   
  * @param  cmd ����ָ��
  * @param  buff д����߶�ȡ���ݵ�ַָ��
  * @return DRESULT ����״̬
  **/
DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buff)
{
	DRESULT status = RES_PARERR;
	switch (pdrv) 
	{
		case ATA:	/* SD CARD */
			switch (cmd) 
			{
				// Get R/W sector size (WORD) 
				case GET_SECTOR_SIZE:    
					*(WORD * )buff = SD_BLOCKSIZE;
				break;
				// Get erase block size in unit of sector (DWORD)
				case GET_BLOCK_SIZE:      
					*(DWORD * )buff = 1; // SDCardInfo.CardBlockSize;
				break;

				case GET_SECTOR_COUNT:
					*(DWORD * )buff = SDCardInfo.CardCapacity / SDCardInfo.CardBlockSize;
					break;
				case CTRL_SYNC :
				break;
			}
			status = RES_OK;
			break;
    
		case SPI_FLASH:		      
		break;
    
		default:
			status = RES_PARERR;
	}
	return status;
}

#endif

__weak DWORD get_fattime(void) {
	/* ���ص�ǰʱ��� */
	return	  ((DWORD)(2015 - 1980) << 25)	/* Year 2015 */
			| ((DWORD)1 << 21)				/* Month 1 */
			| ((DWORD)1 << 16)				/* Mday 1 */
			| ((DWORD)0 << 11)				/* Hour 0 */
			| ((DWORD)0 << 5)				  /* Min 0 */
			| ((DWORD)0 >> 1);				/* Sec 0 */
}
