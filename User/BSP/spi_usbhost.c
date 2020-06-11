/*
*********************************************************************************************************
*	                                  
*	ģ������ : CH374����ģ��
*	�ļ����� : bsp_CH374.c
*	��    �� : V1.0
*	˵    �� : CH374���������ṩ��CH374HFM.lib�ļ�ϵͳ���ļ����õĵײ�I/O�ӿں����ͷ����û����õ�
*				�����ӿ�
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		v1.0    2011-01-11 armfly  ST�̼���汾ΪV3.4.0�汾��
*
*	Copyright (C), 2010-2011, ���������� www.armfly.com
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
	������STM32F103ZE-EK ������(V2)ʹ�õ�USB HOSTоƬΪWCH��˾��CH374TоƬ��
	��оƬ����8bit���߽ӿڣ�ֱ�����ӵ� STM32��FSMC�����ϡ�	
	
	CH374T��TFT LCD��DM9000AEͬ����һ��BANK������STM32��FSMC_NE4Ƭѡ�ռ䡣
	Ϊ����LCD��DM9000AE��FSMC���ñ��ּ��ݣ�������FSMC_NE4��Ӧ��BANK����Ϊ16bitģʽ��
	
	�������ϵĵ�ַ������������CH374T�Ķ˿ڵ�ַ�ռ䡣
	FSMC_NE4 = 0; FSMC_A20 = 1; FSMC_A19 = 0ʱ��ѡ��CH374T��
	������16bitģʽ��CPU��FSMC_A20��FSMC_A19����ʵ�ʶ�Ӧ�ĵ�ַ��ΪA21��A20��
	
    FSMC_A2 �� CH374T��A0���ӣ�����ѡ��������˿ڻ������ݶ˿ڣ�ͬ��FSMC_A2ʵ�ʶ�Ӧ��ַ��A3

	CH374���ļ�����Ҫ����һ��ȫ�ֵĽṹ���������ļ������������ķ�װ�Բ�ǿ�����ǽ�
	���ֺ������������·�װ��
*/
//#define CH374_BASE_ADDR	0x6C200000	
//#define CH374_IDX_PORT	(*((volatile uint16_t *) (CH374_BASE_ADDR + 0x08)))	/* CH374�����˿ڵ�I/O��ַ */
//#define CH374_DAT_PORT	(*((volatile uint16_t *) (CH374_BASE_ADDR )))		/* CH374���ݶ˿ڵ�I/O��ַ */

/*
*********************************************************************************************************
*	���¶������ϸ˵���뿴CH374HFM.H�ļ�
*********************************************************************************************************
*/

/* CH374��INT#�������ӷ�ʽ, 0Ϊ"��ѯ��ʽ",1Ϊ"�жϷ�ʽ" */
#define LIB_CFG_INT_EN			0	

/* ֻʹ�õ�Ƭ�����õ�1KB�ⲿRAM: 0000H-01FFH Ϊ���̶�д������, 
���ֽ�Ϊ��λ��д�ļ�����Ҫ�ļ����ݶ�д������FILE_DATA_BUF */
/* �ⲿRAM�Ĵ������ݻ���������ʼ��ַ, �Ӹõ�Ԫ��ʼ�Ļ���������ΪSECTOR_SIZE */
//#define	DISK_BASE_BUF_ADDR 0x0000	

/* P3.2, INT0, CH374���ж���INT#����,����CH374��INT#����,���ڲ�ѯ�ж�״̬ */
//#define CH374_INT_WIRE			GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_2)	
/* ���δ����CH374���ж�����,��ôӦ��ȥ����������,�Զ�ʹ�üĴ�����ѯ��ʽ */

#define NO_DEFAULT_CH374_F_ENUM		1		/* δ����CH374FileEnumer����ʽ�ֹ�Խ�Լ���� */
#define NO_DEFAULT_CH374_F_QUERY	1		/* δ����CH374FileQuery����ʽ�ֹ�Խ�Լ���� */
#define NO_DEFAULT_CH374_RESET		1		/* δ����CH374Reset����ʽ�ֹ�Խ�Լ���� */

#include "CH374HFM.H"		/* ����CORTEX-M3�Ŀ�ͷ�ļ� */
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
*	�� �� ��: ch374_DetectOk
*	����˵��: ���CH374оƬID
*	��    �Σ���
*	�� �� ֵ: 1��ʾ���OK; 0��ʾ���ʧ��
*********************************************************************************************************
*/
uint8_t ch374_DetectOk(void)
{
	uint8_t id8;
	
	/* ��CH374оƬID */
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
*	�� �� ��: ch374_OpenFile
*	����˵��: ��ȡU����������λ���ֽ�
*	��    �Σ�_uiRetSizee : ��Ŵ��������ı����ĵ�ַ��ָ�룩
*	�� �� ֵ: ������롣���������CH374HFM.H�ļ��ж��塣
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
*	�� �� ��: ch374_OpenFile
*	����˵��: ��U���ļ�
*	��    �Σ�_pFileName : �ļ���(��·��)
*	�� �� ֵ: ������롣���������CH374HFM.H�ļ��ж��塣
*				ERR_MISS_DIR : ָ��·����ĳ����Ŀ¼û���ҵ�,������Ŀ¼���ƴ���
*				ERR_MISS_FILE: ָ��·�����ļ�û���ҵ�,�������ļ����ƴ���
*********************************************************************************************************
*/
uint8_t ch374_OpenFile(char *_pFileName)
{
	/* CH374���ļ�����Ҫ���� mCmdParam ȫ�ֱ������в��������������½����˷�װ�� */
	strcpy((char *)mCmdParam.Open.mPathName, _pFileName);
	return CH374FileOpen();
}

/*
*********************************************************************************************************
*	�� �� ��: ch374_CloseFile
*	����˵��: �رյ�ǰ�򿪵��ļ�
*	��    �Σ���
*	�� �� ֵ: ������롣���������CH374HFM.H�ļ��ж��塣
*********************************************************************************************************
*/
uint8_t ch374_CloseFile(void)
{
	return CH374FileClose();
}

/*
*********************************************************************************************************
*	�� �� ��: ch374_CreateFile
*	����˵��: �½�һ���ļ�
*	��    �Σ�_pFileName : �ļ���(��·��)
*	�� �� ֵ: ������롣���������CH374HFM.H�ļ��ж��塣
*********************************************************************************************************
*/
uint8_t ch374_CreateFile(char *_pFileName)
{
	strcpy((char *)mCmdParam.Create.mPathName, _pFileName);  /* ���ļ��� */
	return CH374FileCreate();  /* �½��ļ�����,����ļ��Ѿ���������ɾ�������½� */
}

/*
*********************************************************************************************************
*	�� �� ��: ch374_SeekFile
*	����˵��: ��λ��ǰ�򿪵��ļ����ļ�ָ��
*	��    �Σ�_uiPos ���ļ����ֽ�λ�ã�0xffffffff ��ʾ�Ƶ�β��
*	�� �� ֵ: 
*********************************************************************************************************
*/
uint8_t ch374_SeekFile(uint32_t _uiPos)
{
	mCmdParam.ByteLocate.mByteOffset = _uiPos;
	return CH374ByteLocate();
}

/*
*********************************************************************************************************
*	�� �� ��: ch374_ReadFile
*	����˵��: ��ȡ��ǰ���ļ������ݣ��ļ�ָ���Զ�����
*	��    ��: 	_pReadBuf    ��Ŀ�껺����
*			 	 _iByteCount : ��Ҫ��ȡ���ֽ���
*				_piReadCount : �洢ʵ�ʶ�ȡ���ֽ����ı������ɵ����߶���ñ���
*	�� �� ֵ: ������롣���������CH374HFM.H�ļ��ж��塣
*********************************************************************************************************
*/
uint8_t ch374_ReadFile(uint8_t *_pReadBuf, int32_t _iByteCount, int32_t *_piReadCount)
{
	int32_t iRemCount;		/* ʣ���ȡ���ֽ��� */ 
	int32_t iReadCount;		/* �Ѿ���ȡ���ֽ��� */
	uint8_t c;		
	uint8_t ucErr = ERR_SUCCESS;		
	
	iRemCount = _iByteCount;
	iReadCount = 0;
	
	while (iRemCount)
	{
		if (iRemCount > MAX_BYTE_IO )
		{
			/* 
				MAX_BYTE_IO ȱʡ��30�ֽڣ��û��Լ����Ե������255�ֽ�
				ʣ�����ݽ϶�,���Ƶ��ζ�д�ĳ��Ȳ��ܳ��� MAX_BYTE_IO
			*/
			c = MAX_BYTE_IO;  
		}
		else
		{
			c = iRemCount;  	/* ���ʣ����ֽ��� */
		}
		
		mCmdParam.ByteRead.mByteCount = c;  /* ����������ֽ��� */
		ucErr = CH374ByteRead();  			/* ���ֽ�Ϊ��λ��ȡ���ݿ�,���ζ�д�ĳ��Ȳ��ܳ���MAX_BYTE_IO */
		if (ucErr != ERR_SUCCESS) 
		{
			break;  /* ����ʧ�� */
		}
		
		memcpy(_pReadBuf, mCmdParam.ByteRead.mByteBuffer, mCmdParam.ByteRead.mByteCount);
		_pReadBuf += mCmdParam.ByteRead.mByteCount;
		
		iReadCount += mCmdParam.ByteRead.mByteCount;
		iRemCount -= mCmdParam.ByteRead.mByteCount;  /* ����,��ȥ��ǰʵ���Ѿ��������ַ��� */

		if (mCmdParam.ByteRead.mByteCount < c)
		{  
			/* ʵ�ʶ������ַ�������Ҫ��������ַ���,˵���Ѿ����ļ��Ľ�β */
			break;
		}		
	}
	*_piReadCount = iReadCount;
	return 	ucErr;
}

/*
*********************************************************************************************************
*	�� �� ��: ch374_WriteFile
*	����˵��: ������д�뵱ǰ���ļ����ļ�ָ���Զ�����
*	��    ��: 	_pDataBuf    ������Դ������
*			 	_iByteCount : ��Ҫд����ֽ���
*	�� �� ֵ: ������롣���������CH374HFM.H�ļ��ж��塣
*********************************************************************************************************
*/
uint8_t ch374_WriteFile(uint8_t *_pDataBuf, int32_t _iByteCount)
{
	int32_t iRemCount;		/* ʣ���ȡ���ֽ��� */ 
	int32_t iWriteCount;	/* �Ѿ�д����ֽ��� */
	uint8_t c;		
	uint8_t ucErr;		
	
	iRemCount = _iByteCount;
	iWriteCount = 0;
	
	while (iRemCount)
	{
		if (iRemCount > MAX_BYTE_IO )
		{
			/* 
				MAX_BYTE_IO ȱʡ��30�ֽڣ��û��Լ����Ե������255�ֽ�
				ʣ�����ݽ϶�,���Ƶ��ζ�д�ĳ��Ȳ��ܳ��� MAX_BYTE_IO
			*/
			c = MAX_BYTE_IO;  
		}
		else
		{
			c = iRemCount;  	/* ���ʣ����ֽ��� */
		}
		
		memcpy(mCmdParam.ByteRead.mByteBuffer, _pDataBuf, c);
		mCmdParam.ByteRead.mByteCount = c;  /* ����д����ֽ����� */
		ucErr = CH374ByteWrite();  			/* ���ֽ�Ϊ��λ��ȡ���ݿ�,���ζ�д�ĳ��Ȳ��ܳ���MAX_BYTE_IO */
		if (ucErr != ERR_SUCCESS) 
		{
			break;  /* ����ʧ�� */
		}

		_pDataBuf += c;
		iWriteCount += c;
		iRemCount -= c;  /* ����,��ȥ��ǰʵ��д����ַ��� */		
	}
	
	/* 
		д��0�ֽڵ�����,�����Զ������ļ��ĳ���
		�����������,��ôִ��CH374FileCloseʱҲ���Զ������ļ�����
	*/
	mCmdParam.ByteWrite.mByteCount = 0;  
	ucErr = CH374ByteWrite();   
	return ucErr;
}

/*
*********************************************************************************************************
*	�� �� ��: ch374_FindFile
*	����˵��: ����ָ��Ŀ¼�µ��ļ�
*	��    ��: _pPath     : ָ��������·����ͨ���*
*			  _iNo       : ö����ţ���0��2147483647��
*			  _pFileName : ����ѵ������ļ����洢��_pFileName
*	�� �� ֵ: ������롣���������CH374HFM.H�ļ��ж��塣
*			ERR_MISS_FILE ����Ҳ��������ƥ����ļ�,�Ѿ�û��ƥ����ļ���
*			ERR_FOUND_NAME : ��������ͨ�����ƥ����ļ���
*********************************************************************************************************
*/
uint8_t ch374_FindFile(uint8_t *_pPath, uint32_t _iNo, uint8_t *_pFileName)
{
	uint8_t ucLen;
	uint8_t ucErr;
	
	/* CH374���ļ�����Ҫ���� mCmdParam ȫ�ֱ������в��������������½����˷�װ�� */
	
	/* �����ļ���, *Ϊͨ���,�����������ļ�������Ŀ¼ */
	strcpy((char *)mCmdParam.Open.mPathName, (char *)_pPath);  
	ucLen = strlen((char *) mCmdParam.Open.mPathName);
	
	mCmdParam.Open.mPathName[ucLen] = 0xFF;
	CH374vFileSize = _iNo;	/* ö�ٺ� */
	
	/* ���ļ�, ����ļ����к���ͨ���*,��Ϊ�����ļ������� */
	ucErr = CH374FileOpen();  
	if (ucErr == ERR_FOUND_NAME )
	{  
		strcpy((char *)_pFileName, (char *)mCmdParam.Open.mPathName);
	}
	return ucErr;
}

/*
*********************************************************************************************************
*	�� �� ��: ch374_ModifyFileDate
*	����˵��: �޸��ļ��������ڡ�����֮ǰ��Ҫ�ȴ��ļ���
*	��    ��: _usYear     : ��, ��2010
*			  _ucMonth    : �£���1
*			  uint8_ucDay : �գ���8
*	�� �� ֵ: ������롣���������CH374HFM.H�ļ��ж��塣
*			ERR_MISS_FILE ����Ҳ��������ƥ����ļ�,�Ѿ�û��ƥ����ļ���
*********************************************************************************************************
*/
uint8_t ch374_ModifyFileDate(uint16_t _usYear, uint8_t _ucMonth, uint8_t _ucDay)
{
	mCmdParam.Modify.mFileAttr = 0xff;		/* �������: �µ��ļ�����, Ϊ0FFH���޸� */
	mCmdParam.Modify.mFileTime = 0xffff;	/* �������: �µ��ļ�����ʱ��,Ϊ0FFFFH���޸� */
	mCmdParam.Modify.mFileDate = MAKE_FILE_DATE(_usYear, _ucMonth, _ucDay);  /* �������: �µ��ļ�����ʱ�� */
	/* �������: �µ��ļ�����, ���ֽ�Ϊ��λд�ļ�Ӧ���ɳ����ر��ļ�ʱ�Զ����³���,���Դ˴����޸� */	
	mCmdParam.Modify.mFileSize = 0xffffffff; 
	return CH374FileModify();	/* �޸ĵ�ǰ�ļ�����Ϣ,�޸����� */
}

/*
*********************************************************************************************************
*	�� �� ��: ch374_DeleteFile
*	����˵��: ɾ���ļ���
*	��    ��: _pFileName : �ļ���(��·��)
*	�� �� ֵ: ������롣���������CH374HFM.H�ļ��ж��塣
*********************************************************************************************************
*/
uint8_t ch374_DeleteFile(char *_pFileName)
{
	/* CH374���ļ�����Ҫ���� mCmdParam ȫ�ֱ������в��������������½����˷�װ�� */
	strcpy((char *)mCmdParam.Open.mPathName, _pFileName);
	return CH374FileErase();  /* ɾ���ļ����ر� */
}

/*
*********************************************************************************************************
*			��CH374HFM.LIB���õĵײ�ӿں���
*********************************************************************************************************
*/

/* ��ָ���Ĵ�����ȡ���� */
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

/* ��ָ���Ĵ���д������ */
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

/* �޸�ָ���Ĵ���������,�����ٻ�,��Write374Byte��Read374ByteЧ�ʸ� */
void Modify374Byte( UINT8 mAddr, UINT8 mAndData, UINT8 mOrData)
{
//	Write374Index( mAddr );
//	Write374Data( Read374Data0( ) & mAndData | mOrData);
}

/* ��ָ����ʼ��ַ�������ݿ� */
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

/* ��ָ����ʼ��ַд�����ݿ� */
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

/* �ⲿ����ı�CH374�������õ��ӳ���,��˫����������64�ֽڵ����ݿ�,���ص�ǰ��ַ */
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

/* �ⲿ����ı�CH374�������õ��ӳ���,��˫������д��64�ֽڵ����ݿ�,���ص�ǰ��ַ */
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

/* �ⲿ����ı�CH374�������õ��ӳ���,��RAM_HOST_TRANд�볣�������ݿ� */
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
