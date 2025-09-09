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

/* 为每个设备定义一个物理编号 */
#define ATA			           0     // SD卡
#define SPI_FLASH		       1     // 预留外部SPI Flash使用

#define SD_BLOCKSIZE     512 

extern  SD_CardInfo SDCardInfo;

/**
  * @brief  获取设备状态
  * @param  pdrv 物理编号   
  * @return DSTATUS 设备状态
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
  * @brief  设备初始化
  * @param  pdrv 物理编号   
  * @return DSTATUS 设备状态
  **/
DSTATUS disk_initialize(BYTE pdrv)
{
	DSTATUS status = STA_NOINIT;	
	switch (pdrv) 
	{
		case ATA:	         /* SD CARD */
			if(SD_Init() == SD_OK) // 这里调用初始化函数
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
  * @brief  读扇区：读取扇区内容到指定存储区
  * @param  pdrv 物理编号   
  * @param  buff 数据缓存区
  * @param  sector 扇区首地址
  * @param  count 扇区个数(1..128)
  * @return DRESULT 读取状态
  **/
DRESULT disk_read(BYTE pdrv, BYTE *buff, DWORD sector,	UINT count)
{
	DRESULT status = RES_PARERR;
	SD_Error SD_state = SD_OK;
	
	switch (pdrv) 
	{
		case ATA:	/* SD CARD */
			/*
			 * 检查地址的最低两位是否为零，如果结果不为零，
			 * 说明地址的最低两位至少有一位是1，
			 * 这意味着该地址不是4的倍数（即不是4字节对齐的）
			 */						
		  	if((DWORD)buff & 3)
			{
				DRESULT res = RES_OK;
				/* 定义为 DWORD 类型可以使得其自动 4 字节对齐 */
				DWORD scratch[SD_BLOCKSIZE / 4]; // 定义了一个包含128个DWORD的数组，总大小正好是512字节

				while (count--) 
				{
					res = disk_read(ATA, (void *)scratch, sector++, 1);

					if (res != RES_OK) 
					{
						break;
					}
					memcpy(buff, scratch, SD_BLOCKSIZE); // 将数据从对齐的 scratch 复制到用户指定的、可能不对齐的 buff 中
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
  * @brief  写扇区：将指定存储区上的内容写到指定扇区
  * @param  pdrv 物理编号   
  * @param  buff 欲写入数据的缓存区
  * @param  sector 扇区首地址
  * @param  count 扇区个数(1..128)
  * @return DRESULT 读取状态
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
  * @brief  其它控制
  * @param  pdrv 物理编号   
  * @param  cmd 控制指令
  * @param  buff 写入或者读取数据地址指针
  * @return DRESULT 控制状态
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
	/* 返回当前时间戳 */
	return	  ((DWORD)(2015 - 1980) << 25)	/* Year 2015 */
			| ((DWORD)1 << 21)				/* Month 1 */
			| ((DWORD)1 << 16)				/* Mday 1 */
			| ((DWORD)0 << 11)				/* Hour 0 */
			| ((DWORD)0 << 5)				  /* Min 0 */
			| ((DWORD)0 >> 1);				/* Sec 0 */
}
