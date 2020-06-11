/*
*********************************************************************************************************
*	                                  
*	模块名称 : CH374驱动模块
*	文件名称 : bsp_CH374.c
*	版    本 : V1.0
*	说    明 : CH374驱动程序。提供被CH374HFM.lib文件系统库文件调用的底层I/O接口函数和方便用户调用的
*				函数接口
*	修改记录 :
*		版本号  日期       作者    说明
*		v1.0    2011-01-11 armfly  ST固件库版本为V3.4.0版本。
*
*	Copyright (C), 2010-2011, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "stm32f0xx_hal.h"
#include "Global.h"
#include "string.h"
#include "spi_flash.h"
#include "main.h"

#include "bsp_periph.h"


/* 
	安富莱STM32F103ZE-EK 开发板(V2)使用的USB HOST芯片为WCH公司的CH374T芯片。
	该芯片具有8bit总线接口，直接连接到 STM32的FSMC总线上。	
	
	CH374T和TFT LCD、DM9000AE同处于一个BANK，共享STM32的FSMC_NE4片选空间。
	为了与LCD、DM9000AE的FSMC配置保持兼容，本程序将FSMC_NE4对应的BANK配置为16bit模式。
	
	开发板上的地址译码器决定了CH374T的端口地址空间。
	FSMC_NE4 = 0; FSMC_A20 = 1; FSMC_A19 = 0时，选中CH374T。
	由于是16bit模式，CPU的FSMC_A20、FSMC_A19口线实际对应的地址线为A21、A20。
	
    FSMC_A2 和 CH374T的A0连接，用于选择是命令端口还是数据端口，同理，FSMC_A2实际对应地址线A3

	CH374的文件库需要借助一个全局的结构变量进行文件操作，函数的封装性不强。我们将
	部分函数进行了重新封装。
*/
//#define CH374_BASE_ADDR	0x6C200000	
//#define CH374_IDX_PORT	(*((volatile uint16_t *) (CH374_BASE_ADDR + 0x08)))	/* CH374索引端口的I/O地址 */
//#define CH374_DAT_PORT	(*((volatile uint16_t *) (CH374_BASE_ADDR )))		/* CH374数据端口的I/O地址 */

/*
*********************************************************************************************************
*	以下定义的详细说明请看CH374HFM.H文件
*********************************************************************************************************
*/

/* CH374的INT#引脚连接方式, 0为"查询方式",1为"中断方式" */
#define LIB_CFG_INT_EN			0	

/* 只使用单片机内置的1KB外部RAM: 0000H-01FFH 为磁盘读写缓冲区, 
以字节为单位读写文件不需要文件数据读写缓冲区FILE_DATA_BUF */
/* 外部RAM的磁盘数据缓冲区的起始地址, 从该单元开始的缓冲区长度为SECTOR_SIZE */
//#define	DISK_BASE_BUF_ADDR 0x0000	

/* P3.2, INT0, CH374的中断线INT#引脚,连接CH374的INT#引脚,用于查询中断状态 */
//#define CH374_INT_WIRE			GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_2)	
/* 如果未连接CH374的中断引脚,那么应该去掉上述定义,自动使用寄存器查询方式 */

#define NO_DEFAULT_CH374_F_ENUM		1		/* 未调用CH374FileEnumer程序故禁止以节约代码 */
#define NO_DEFAULT_CH374_F_QUERY	1		/* 未调用CH374FileQuery程序故禁止以节约代码 */
#define NO_DEFAULT_CH374_RESET		1		/* 未调用CH374Reset程序故禁止以节约代码 */

#include "CH374HFM.H"		/* 包含CORTEX-M3的库头文件 */
/*********************************************************************
 * @fn     uint8_t BSP_GPIO_Init( void )
 *
 * @brief  
 *          
 * @param  none
 *         
 * @return none
 *                        
 */
void SPI_USBhost_CH374T_Init(void)
{

  SPI2Handle.Instance               = SPI2;
	SPI2Handle.Init.Mode              = SPI_MODE_MASTER;
	SPI2Handle.Init.Direction         = SPI_DIRECTION_2LINES;
	SPI2Handle.Init.DataSize          = SPI_DATASIZE_8BIT;
	SPI2Handle.Init.CLKPolarity       = SPI_POLARITY_HIGH;
	SPI2Handle.Init.CLKPhase          = SPI_PHASE_2EDGE;
	SPI2Handle.Init.NSS               = SPI_NSS_SOFT;
  SPI2Handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  SPI2Handle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
  SPI2Handle.Init.TIMode            = SPI_TIMODE_DISABLE;
  SPI2Handle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
 // SPI2Handle.Init.CRCPolynomial     = 7;
  SPI2Handle.Init.NSSPMode          = SPI_NSS_PULSE_DISABLE;
 // SPI2Handle.Init.CRCLength         = SPI_CRC_LENGTH_8BIT;

	
  if(HAL_SPI_Init(&SPI2Handle) != HAL_OK)
  {
    printf("spi_usbhost err \r\n");
  }

}


/*
*********************************************************************************************************
*	函 数 名: ch374_DetectOk
*	功能说明: 检测CH374芯片ID
*	形    参：无
*	返 回 值: 1表示检测OK; 0表示检测失败
*********************************************************************************************************
*/
uint8_t ch374_DetectOk(void)
{
	uint8_t id8;
	
	/* 读CH374芯片ID */
	id8 = CH374_READ_REGISTER(0x04);
	if ((id8 & 0x03) == 0x01)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}	

/*
*********************************************************************************************************
*	函 数 名: ch374_OpenFile
*	功能说明: 获取U盘容量，单位：字节
*	形    参：_uiRetSizee : 存放磁盘容量的变量的地址（指针）
*	返 回 值: 错误代码。错误代码在CH374HFM.H文件中定义。
*********************************************************************************************************
*/
uint8_t ch374_DiskSize(uint32_t *_uiRetSize)
{
	uint8_t ucErr;

	ucErr = CH374DiskSize();	
	*_uiRetSize = mCmdParam.DiskSize.mDiskSizeSec;
	return ucErr;
}

/*
*********************************************************************************************************
*	函 数 名: ch374_OpenFile
*	功能说明: 打开U盘文件
*	形    参：_pFileName : 文件名(含路径)
*	返 回 值: 错误代码。错误代码在CH374HFM.H文件中定义。
*				ERR_MISS_DIR : 指定路径的某个子目录没有找到,可能是目录名称错误
*				ERR_MISS_FILE: 指定路径的文件没有找到,可能是文件名称错误
*********************************************************************************************************
*/
uint8_t ch374_OpenFile(char *_pFileName)
{
	/* CH374的文件库需要借助 mCmdParam 全局变量进行操作。安富莱重新进行了封装。 */
	strcpy((char *)mCmdParam.Open.mPathName, _pFileName);
	return CH374FileOpen();
}

/*
*********************************************************************************************************
*	函 数 名: ch374_CloseFile
*	功能说明: 关闭当前打开的文件
*	形    参：无
*	返 回 值: 错误代码。错误代码在CH374HFM.H文件中定义。
*********************************************************************************************************
*/
uint8_t ch374_CloseFile(void)
{
	return CH374FileClose();
}

/*
*********************************************************************************************************
*	函 数 名: ch374_CreateFile
*	功能说明: 新建一个文件
*	形    参：_pFileName : 文件名(含路径)
*	返 回 值: 错误代码。错误代码在CH374HFM.H文件中定义。
*********************************************************************************************************
*/
uint8_t ch374_CreateFile(char *_pFileName)
{
	strcpy((char *)mCmdParam.Create.mPathName, _pFileName);  /* 新文件名 */
	return CH374FileCreate();  /* 新建文件并打开,如果文件已经存在则先删除后再新建 */
}

/*
*********************************************************************************************************
*	函 数 名: ch374_SeekFile
*	功能说明: 定位当前打开的文件的文件指针
*	形    参：_uiPos ：文件的字节位置，0xffffffff 表示移到尾部
*	返 回 值: 
*********************************************************************************************************
*/
uint8_t ch374_SeekFile(uint32_t _uiPos)
{
	mCmdParam.ByteLocate.mByteOffset = _uiPos;
	return CH374ByteLocate();
}

/*
*********************************************************************************************************
*	函 数 名: ch374_ReadFile
*	功能说明: 读取当前打开文件的数据，文件指针自动递增
*	形    参: 	_pReadBuf    ：目标缓冲区
*			 	 _iByteCount : 将要读取的字节数
*				_piReadCount : 存储实际读取的字节数的变量，由调用者定义该变量
*	返 回 值: 错误代码。错误代码在CH374HFM.H文件中定义。
*********************************************************************************************************
*/
uint8_t ch374_ReadFile(uint8_t *_pReadBuf, int32_t _iByteCount, int32_t *_piReadCount)
{
	int32_t iRemCount;		/* 剩余读取的字节数 */ 
	int32_t iReadCount;		/* 已经读取的字节数 */
	uint8_t c;		
	uint8_t ucErr = ERR_SUCCESS;		
	
	iRemCount = _iByteCount;
	iReadCount = 0;
	
	while (iRemCount)
	{
		if (iRemCount > MAX_BYTE_IO )
		{
			/* 
				MAX_BYTE_IO 缺省是30字节，用户自己可以调大，最大255字节
				剩余数据较多,限制单次读写的长度不能超过 MAX_BYTE_IO
			*/
			c = MAX_BYTE_IO;  
		}
		else
		{
			c = iRemCount;  	/* 最后剩余的字节数 */
		}
		
		mCmdParam.ByteRead.mByteCount = c;  /* 请求读出的字节数 */
		ucErr = CH374ByteRead();  			/* 以字节为单位读取数据块,单次读写的长度不能超过MAX_BYTE_IO */
		if (ucErr != ERR_SUCCESS) 
		{
			break;  /* 操作失败 */
		}
		
		memcpy(_pReadBuf, mCmdParam.ByteRead.mByteBuffer, mCmdParam.ByteRead.mByteCount);
		_pReadBuf += mCmdParam.ByteRead.mByteCount;
		
		iReadCount += mCmdParam.ByteRead.mByteCount;
		iRemCount -= mCmdParam.ByteRead.mByteCount;  /* 计数,减去当前实际已经读出的字符数 */

		if (mCmdParam.ByteRead.mByteCount < c)
		{  
			/* 实际读出的字符数少于要求读出的字符数,说明已经到文件的结尾 */
			break;
		}		
	}
	*_piReadCount = iReadCount;
	return 	ucErr;
}

/*
*********************************************************************************************************
*	函 数 名: ch374_WriteFile
*	功能说明: 将数据写入当前打开文件，文件指针自动递增
*	形    参: 	_pDataBuf    ：数据源缓冲区
*			 	_iByteCount : 将要写入的字节数
*	返 回 值: 错误代码。错误代码在CH374HFM.H文件中定义。
*********************************************************************************************************
*/
uint8_t ch374_WriteFile(uint8_t *_pDataBuf, int32_t _iByteCount)
{
	int32_t iRemCount;		/* 剩余读取的字节数 */ 
	int32_t iWriteCount;	/* 已经写入的字节数 */
	uint8_t c;		
	uint8_t ucErr;		
	
	iRemCount = _iByteCount;
	iWriteCount = 0;
	
	while (iRemCount)
	{
		if (iRemCount > MAX_BYTE_IO )
		{
			/* 
				MAX_BYTE_IO 缺省是30字节，用户自己可以调大，最大255字节
				剩余数据较多,限制单次读写的长度不能超过 MAX_BYTE_IO
			*/
			c = MAX_BYTE_IO;  
		}
		else
		{
			c = iRemCount;  	/* 最后剩余的字节数 */
		}
		
		memcpy(mCmdParam.ByteRead.mByteBuffer, _pDataBuf, c);
		mCmdParam.ByteRead.mByteCount = c;  /* 请求写入的字节数据 */
		ucErr = CH374ByteWrite();  			/* 以字节为单位读取数据块,单次读写的长度不能超过MAX_BYTE_IO */
		if (ucErr != ERR_SUCCESS) 
		{
			break;  /* 操作失败 */
		}

		_pDataBuf += c;
		iWriteCount += c;
		iRemCount -= c;  /* 计数,减去当前实际写入的字符数 */		
	}
	
	/* 
		写入0字节的数据,用于自动更新文件的长度
		如果不这样做,那么执行CH374FileClose时也会自动更新文件长度
	*/
	mCmdParam.ByteWrite.mByteCount = 0;  
	ucErr = CH374ByteWrite();   
	return ucErr;
}

/*
*********************************************************************************************************
*	函 数 名: ch374_FindFile
*	功能说明: 搜索指定目录下的文件
*	形    参: _pPath     : 指定搜索的路径，通配符*
*			  _iNo       : 枚举序号，从0到2147483647。
*			  _pFileName : 如果搜到，则将文件名存储在_pFileName
*	返 回 值: 错误代码。错误代码在CH374HFM.H文件中定义。
*			ERR_MISS_FILE ：再也搜索不到匹配的文件,已经没有匹配的文件名
*			ERR_FOUND_NAME : 搜索到与通配符相匹配的文件名
*********************************************************************************************************
*/
uint8_t ch374_FindFile(uint8_t *_pPath, uint32_t _iNo, uint8_t *_pFileName)
{
	uint8_t ucLen;
	uint8_t ucErr;
	
	/* CH374的文件库需要借助 mCmdParam 全局变量进行操作。安富莱重新进行了封装。 */
	
	/* 搜索文件名, *为通配符,适用于所有文件或者子目录 */
	strcpy((char *)mCmdParam.Open.mPathName, (char *)_pPath);  
	ucLen = strlen((char *) mCmdParam.Open.mPathName);
	
	mCmdParam.Open.mPathName[ucLen] = 0xFF;
	CH374vFileSize = _iNo;	/* 枚举号 */
	
	/* 打开文件, 如果文件名中含有通配符*,则为搜索文件而不打开 */
	ucErr = CH374FileOpen();  
	if (ucErr == ERR_FOUND_NAME )
	{  
		strcpy((char *)_pFileName, (char *)mCmdParam.Open.mPathName);
	}
	return ucErr;
}

/*
*********************************************************************************************************
*	函 数 名: ch374_ModifyFileDate
*	功能说明: 修改文件创建日期。调用之前需要先打开文件。
*	形    参: _usYear     : 年, 如2010
*			  _ucMonth    : 月，如1
*			  uint8_ucDay : 日，如8
*	返 回 值: 错误代码。错误代码在CH374HFM.H文件中定义。
*			ERR_MISS_FILE ：再也搜索不到匹配的文件,已经没有匹配的文件名
*********************************************************************************************************
*/
uint8_t ch374_ModifyFileDate(uint16_t _usYear, uint8_t _ucMonth, uint8_t _ucDay)
{
	mCmdParam.Modify.mFileAttr = 0xff;		/* 输入参数: 新的文件属性, 为0FFH则不修改 */
	mCmdParam.Modify.mFileTime = 0xffff;	/* 输入参数: 新的文件创建时间,为0FFFFH则不修改 */
	mCmdParam.Modify.mFileDate = MAKE_FILE_DATE(_usYear, _ucMonth, _ucDay);  /* 输入参数: 新的文件创建时间 */
	/* 输入参数: 新的文件长度, 以字节为单位写文件应该由程序库关闭文件时自动更新长度,所以此处不修改 */	
	mCmdParam.Modify.mFileSize = 0xffffffff; 
	return CH374FileModify();	/* 修改当前文件的信息,修改日期 */
}

/*
*********************************************************************************************************
*	函 数 名: ch374_DeleteFile
*	功能说明: 删除文件。
*	形    参: _pFileName : 文件名(含路径)
*	返 回 值: 错误代码。错误代码在CH374HFM.H文件中定义。
*********************************************************************************************************
*/
uint8_t ch374_DeleteFile(char *_pFileName)
{
	/* CH374的文件库需要借助 mCmdParam 全局变量进行操作。安富莱重新进行了封装。 */
	strcpy((char *)mCmdParam.Open.mPathName, _pFileName);
	return CH374FileErase();  /* 删除文件并关闭 */
}

/*
*********************************************************************************************************
*			供CH374HFM.LIB调用的底层接口函数
*********************************************************************************************************
*/

/* 从指定寄存器读取数据 */
UINT8 CH374_READ_REGISTER(UINT8 mAddr)
{ 
  UINT8	returnData, adrCmd[2];
	
	adrCmd[0] = mAddr;
	adrCmd[1] = CMD_SPI_374READ;
	
	SPI_USB_CS_EN(0);  
	HAL_SPI_Transmit( &SPI2Handle, adrCmd, 2, 0xFFFFFFFFU);
	HAL_SPI_Receive(&SPI2Handle, &returnData, 1, 0xFFFFFFFFU );

	SPI_USB_CS_EN(1);  
	
	return( returnData ); 
}

/* 向指定寄存器写入数据 */
void CH374_WRITE_REGISTER(UINT8 mAddr, UINT8 mData)
{
	UINT8	adrCmd[3];
	
	adrCmd[0] = mAddr;
	adrCmd[1] = CMD_SPI_374WRITE;
	adrCmd[2] = mData;
	
	SPI_USB_CS_EN(0);  
	HAL_SPI_Transmit( &SPI2Handle, adrCmd, 3, 0xFFFFFFFFU);

	SPI_USB_CS_EN(1);  
}

/* 修改指定寄存器的数据,先与再或,比Write374Byte再Read374Byte效率高 */
void Modify374Byte( UINT8 mAddr, UINT8 mAndData, UINT8 mOrData)
{
//	Write374Index( mAddr );
//	Write374Data( Read374Data0( ) & mAndData | mOrData);
}

/* 从指定起始地址读出数据块 */
void CH374_READ_BLOCK(UINT8 mAddr, UINT8 mLen, PUINT8 mBuf)
{
	UINT8	adrCmd[2];
	
	adrCmd[0] = mAddr;
	adrCmd[1] = CMD_SPI_374READ;
	
	SPI_USB_CS_EN(0);  
	HAL_SPI_Transmit( &SPI2Handle, adrCmd, 2, 0xFFFFFFFFU);
	HAL_SPI_Receive(&SPI2Handle, mBuf, mLen, 0xFFFFFFFFU );

	SPI_USB_CS_EN(1); 
}

/* 向指定起始地址写入数据块 */
void CH374_WRITE_BLOCK(UINT8 mAddr, UINT8 mLen, PUINT8 mBuf)
{
	UINT8 adrCmd[2];
	
	adrCmd[0] = mAddr;
	adrCmd[1] = CMD_SPI_374WRITE;

	SPI_USB_CS_EN(0);  
	
	HAL_SPI_Transmit( &SPI2Handle, adrCmd, 2, 0xFFFFFFFFU);
  HAL_SPI_Transmit( &SPI2Handle, mBuf, mLen, 0xFFFFFFFFU);
	
	SPI_USB_CS_EN(1);  
}

/* 外部定义的被CH374程序库调用的子程序,从双缓冲区读出64字节的数据块,返回当前地址 */
PUINT8 CH374_READ_BLOCK64( UINT8 mAddr, PUINT8 mBuf)  
{
	
	UINT8	adrCmd[2];
	
	adrCmd[0] = mAddr;
	adrCmd[1] = CMD_SPI_374READ;
	
	SPI_USB_CS_EN(0);
  
	HAL_SPI_Transmit( &SPI2Handle, adrCmd, 2, 0xFFFFFFFFU);
	HAL_SPI_Receive(&SPI2Handle, mBuf, CH374_BLOCK_SIZE, 0xFFFFFFFFU );

	SPI_USB_CS_EN(1); 	

	return( mBuf+CH374_BLOCK_SIZE);	
}

/* 外部定义的被CH374程序库调用的子程序,向双缓冲区写入64字节的数据块,返回当前地址 */
PUINT8 CH374_WRITE_BLOCK64(UINT8 mAddr, PUINT8 mBuf)
{
	UINT8 adrCmd[2];
	
	adrCmd[0] = mAddr;
	adrCmd[1] = CMD_SPI_374WRITE;

	SPI_USB_CS_EN(0);  
	
	HAL_SPI_Transmit( &SPI2Handle, adrCmd, 2, 0xFFFFFFFFU);
  HAL_SPI_Transmit( &SPI2Handle, mBuf, CH374_BLOCK_SIZE, 0xFFFFFFFFU);
	
	SPI_USB_CS_EN(1); 
  return( mBuf+CH374_BLOCK_SIZE);	
}

/* 外部定义的被CH374程序库调用的子程序,向RAM_HOST_TRAN写入常量型数据块 */
void CH374_WRITE_BLOCK_C(UINT8 mLen, PUINT8C mBuf)  
{
	UINT8 adrCmd[2];
	
	adrCmd[0] = RAM_HOST_TRAN;
	adrCmd[1] = CMD_SPI_374WRITE;

	SPI_USB_CS_EN(0);  
	
	HAL_SPI_Transmit( &SPI2Handle, adrCmd, 2, 0xFFFFFFFFU);
  HAL_SPI_Transmit( &SPI2Handle, (UINT8 *)mBuf, mLen, 0xFFFFFFFFU);
	
	SPI_USB_CS_EN(1); 
}
