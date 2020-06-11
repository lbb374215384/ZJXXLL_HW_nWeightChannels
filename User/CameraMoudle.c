/* Includes ------------------------------------------------------------------*/
#include "Ec20.h"
#include "AppTask.h"
#include "Variable.h"
#include "TimeEVent.h"
#include "usart123.h"
#include "gpio.h"
#include "wifimoudle.h"
#include "Global.h"
#include "net_device.h"
#include "net_image.h"
#include "CameraMoudle.h"
#include "spi_flash.h"

uint32_t FileSize = 0;
uint8_t  RS485Index = 0;
uint8_t  p_ImgErrCount = 0;
imagePollState_t  ImagePollState;

uint16_t p_PackageNum = 0; 
uint16_t p_PackageCount = 0;
uint16_t p_RemainSize = 0;
uint32_t p_startPtr = 0,f_RevAdr = 0;
uint32_t p_endPtr = 0;

uint8_t redSendBuff[25];
/*********************************************************************
 * @fn     void Uart3_ReceiveTimeout_Evt( void )
 *
 * @brief  ����ͷͨ�ų�ʱ�¼�
 *          
 * @param   none          
 *
 * @return  none
 */
void Uart3_ReceiveTimeout_Evt( void )
{  
	 timer_10msDB[10].Flag = 0;
	 timer_10msDB[10].Enable = DISABLE;
	 timer_10msDB[10].Time = 0;

	 if( p_ImgErrCount > 0 ) //�ط�����
	 {
		 p_ImgErrCount = 0;
		 if( ImagePollState == ImagePolling )//ͼ����ѯ������
		 {
		   p_PackageCount++;
		   Uart3_SendImageDataReqPackageHandle( 0, &p_PackageCount, &p_PackageNum );
		 }
	 }
	 else
	 {
		 p_ImgErrCount++;
		 RS485COM_EN(ON);  
		 HAL_UART_Transmit(&Uart3Handle, redSendBuff+1, redSendBuff[0], 5000);//�ط�
		 RS485COM_EN(OFF);
     Timer_Event_Set( &timer_10msDB[10], ReceCamDataTimeout, ENABLE, 0, Uart3_ReceiveTimeout_Evt );//������ʱ�ж�		 
		 memset( redSendBuff,0x00, 25);
	 }
}

/*********************************************************************
 * @fn      void ReceiveCameraData_Handle( void )
 *
 * @brief   USART3 ͼ�����ݴ���
 *          
 * @param   none        
 *
 * @return  none
 */
void ReceiveCameraData_Handle( void )// FE |lenl lenh |cmd |status|data |sn |fcs
{
	uint16_t buflen = 0;
	uint8_t *ptr = NULL;

	if( Uart3Handle.RxXferCount == 0xFFFF )//���ݽ���
	{
		Uart3Handle.RxXferCount = 0;
		//memcpy(tcpdata ,Uart3Handle.pRxBuffPtr ,strlen((char *)Uart3Handle.pRxBuffPtr));
		//HAL_UART_Transmit_DMA(&Uart2Handle, tcpdata, strlen((char *)tcpdata));//EC20 AT����
		
		ptr = Uart3Handle.pRxBuffPtr;
		
		if( *ptr == 0xFE  )
		{
			 buflen = BUILD_UINT16( ptr[1], ptr[2] );
			 if( buflen <= ( U3_PackageSize + 29))
			 {
				 if( *(ptr + buflen+2) == Xor_CalcFCS( ptr,  buflen+2) )//ͷ������ȷ
				 {
					 CameraData_Handle( ptr[3],  ptr+4,  buflen-2 );//���ݴ���
				 }	 
			 }
		}
		
		memset(Uart3Handle.pRxBuffPtr, 0x00, Uart3Handle.RxXferSize );
		HAL_UART_Receive_DMA(&Uart3Handle, Uart3Handle.pRxBuffPtr,  Uart3Handle.RxXferSize);
	}
}
	
/*********************************************************************
 * @fn      void RevNwkData_Handle
 *
 * @brief   ͼ�����ݽ��մ���
 *          
 * @param   cmdtype ֡����     mpbuf ���ݵ�ַ   mpbuflen    ���ݳ���
 *
 * @return  none 
 */	
  uint32_t RevStartAdr =0, offsetAddr = 0;
	uint16_t RevImglen = 0;
	uint8_t  filename =0;
void CameraData_Handle( uint8_t cmdtype,  uint8_t *mpbuf,  uint16_t mpbuflen  )//���ݽ��ܴ���
{

	switch( cmdtype )
	{
		case 0x10:   // �������    |status|utc|filename|filelen|md5
		case 0x30:   //ѯ��ͼ����Ϣ |status|utc|filename|filelen|md5	
				if( *mpbuf == 0x00 )
				{
					 memcpy( (uint8_t *)&FileSize, mpbuf+6, 4);   //�õ�ͼ���ܴ�С
					 p_PackageNum = FileSize/U3_PackageSize;      //������+1
					 p_RemainSize = FileSize%U3_PackageSize;      //ʣ��ĸ���
		
					 SPI_FLASH_ImageErase(NV_Image1DataAddr);//NV_Image1DataAddr
					 //SPI_FLASH_BufferWrite( mpbuf+1, NV_Image1HeadAddr, NV_ImageHeadSize);//utc|filename|filelen|md5
					 memcpy(tcpdata, mpbuf+1, NV_ImageHeadSize );
					 SPI_FLASH_BufferWrite( tcpdata, NV_Image1HeadAddr, NV_ImageHeadSize);
					 /******����ѯ��ͼ������֡******/
					 p_PackageCount = 0;
				   ImagePollState = ImagePolling;
					 Uart3_SendImageDataReqPackageHandle( filename, &p_PackageCount, &p_PackageNum );
				}
				break;
				
	  case 0x31://ͼ��֡�ظ�| status | filename | startaddr | datalen | data |
				memcpy( (uint8_t *)&RevStartAdr,  mpbuf+2, 4);  //��ʼ��ַ startPtr == startAddr
				memcpy( (uint8_t *)&RevImglen,  mpbuf+6, 2);    //���ݳ���
		    //SPI_FLASH_ImageErase(NV_Image1DataAddr);//NV_Image1DataAddr
				if( *mpbuf == 0x00 && RevStartAdr == p_startPtr )
				{  
					 Timer_Event_Set( &timer_10msDB[10], 0, DISABLE, 0, Uart3_ReceiveTimeout_Evt );//�����ʱ
					 p_ImgErrCount = 0;
   				 offsetAddr = NV_Image1DataAddr + RevStartAdr;
					 SPI_FLASH_BufferWrite( mpbuf+8, offsetAddr,  RevImglen);
				 	 SPI_FLASH_BufferRead( tcpdata, offsetAddr, RevImglen ); 
					 /******����ѯ��ͼ��������һ֡******/
					 p_PackageCount++;
					 Uart3_SendImageDataReqPackageHandle(  filename, &p_PackageCount, &p_PackageNum );
				 }
				 else
				 {
						 if( p_ImgErrCount > 0 ) //�ط�����
						 {
							 p_ImgErrCount = 0;
							 p_PackageCount++;
							 Uart3_SendImageDataReqPackageHandle(  filename, &p_PackageCount, &p_PackageNum );
						 }
						 else
						 {
							 p_ImgErrCount++;
							 RS485COM_EN(ON);  
							 HAL_UART_Transmit(&Uart3Handle, redSendBuff+1, redSendBuff[0], 5000);
							 RS485COM_EN(OFF); 
							 Timer_Event_Set( &timer_10msDB[10], ReceCamDataTimeout, ENABLE, 0, Uart3_ReceiveTimeout_Evt );//������ʱ�ж� 
							 memset( redSendBuff,0x00, 25);
						 }
				 }
		   break;
				
		case 0x11:
		  Uart3_BuildAndSendHotResContr( 0x10, 0x01, 0x04 );
	  	break; //�򿪵���˿	
		default: break;
	}
}

/***************************************************************************************************
 * @fn      void  Uart3_SendImageDataReqPackageHandle( uint8_t filename, uint16_t *packageCount, uint16_t *packageNum )
 *
 * @brief   ͼ������ѯ��
 *
 * @param   uint8_t filename �ļ���  uint16_t *packageCount, ����  uint16_t *packageNum ������
 *
 * @return  NULL
 ***************************************************************************************************/
void  Uart3_SendImageDataReqPackageHandle( uint8_t filename, uint16_t *packageCount, uint16_t *packageNum )
{
	if( *packageCount <= *packageNum )//RemainSize
	{
		 p_startPtr = U3_PackageSize*p_PackageCount;
		 if( *packageCount != *packageNum )//RemainSize
		 {
			 Uart3_BuildAndSendImageDataReq( 0x31,  filename, p_startPtr, U3_PackageSize, RS485Index++ );
		 }
		 else
		 {
			 Uart3_BuildAndSendImageDataReq( 0x31,  filename, p_startPtr, p_RemainSize, RS485Index++ );//���һ֡
		 }
	}		 
  else  //ȫ��ͼ�����ݶ�ȡ��
	{
		 ImagePollState = ImagePollIdle;//ͼ����ѯ��ϱ�־
     //StartImageUpload();  //ͼ��׼���ϴ�״̬ 
	}		
}	
/***************************************************************************************************
 * @fn      void SPI_FLASH_ImageErase(uint32_t ImageAddr)
 *
 * @brief   ͼ���ļ�����
 * @param   
 *          uint32_t ImageAddr   ͼ���ַ
 *
 * @return  NULL
 ***************************************************************************************************/
void SPI_FLASH_ImageErase(uint32_t ImageAddr)//NV_Image1DataAddr
{

 ImageAddr -= 0x100;
 SPI_FLASH_BlockErase( ImageAddr );
 SPI_FLASH_BlockErase(ImageAddr+ SPI_FLASH_BlockSize );
 SPI_FLASH_BlockErase(ImageAddr+ 2*SPI_FLASH_BlockSize );
 SPI_FLASH_BlockErase(ImageAddr+ 3*SPI_FLASH_BlockSize );
	
}
	
/***************************************************************************************************
 * @fn      void Uart3_BuildAndSendImageinforReq( uint8_t cmdtype, uint8_t filename )
 *
 * @brief   ͼ���ļ���Ϣ��ѯ
 * @param   
 *          uint8_t cmdtype   ����
 *          uint8_t filename  �ļ�������ţ�
 *
 * @return  NULL
 ***************************************************************************************************/
void Uart3_BuildAndSendImageinforReq( uint8_t cmdtype, uint8_t filename , uint8_t index )
{
	//ADDR| LEN2|CMD| STATUS| FIMENAME|SN|FCS|
	uint8_t buflen = 0;
	
	memset(Uart3Handle.pTxBuffPtr,0x00,Uart3Handle.TxXferSize );
	Uart3Handle.pTxBuffPtr[0] = 0xFE;
	Uart3Handle.pTxBuffPtr[1] = 0x05;
	Uart3Handle.pTxBuffPtr[2] = 0x00;	
	buflen = Uart3Handle.pTxBuffPtr[1]+3;
	Uart3Handle.pTxBuffPtr[3] = cmdtype;
	Uart3Handle.pTxBuffPtr[4] = 0x00;
	Uart3Handle.pTxBuffPtr[5] = filename;
	Uart3Handle.pTxBuffPtr[6] = index;
	Uart3Handle.pTxBuffPtr[7] = Xor_CalcFCS( Uart3Handle.pTxBuffPtr, buflen-1 );
	
	RS485COM_EN(ON);  
	HAL_UART_Transmit(&Uart3Handle, Uart3Handle.pTxBuffPtr, buflen, 5000);
	
	memset( redSendBuff,0x00, 25);
	redSendBuff[0]= buflen;
	memcpy( redSendBuff+1, Uart3Handle.pTxBuffPtr, buflen);
	Timer_Event_Set( &timer_10msDB[10], ReceCamDataTimeout, ENABLE, 0, Uart3_ReceiveTimeout_Evt );//������ʱ�ж�
	RS485COM_EN(OFF); 
}

/***************************************************************************************************
 * @fn      void Uart3_BuildAndSendImageDataReq( uint8_t cmdtype,  uint8_t filename, uint32_t saddr, uint16_t datalen, uint8_t index )
 *
 * @brief   ͼ�����ݻ�ȡ
 * @param   
 *          uint8_t  cmdtype   ����
 *          uint8_t  filename  �ļ�������ţ�
 *          uint32_t saddr     ͼ��Ŀ�ʼ��ַ
 *          uint16_t datalen   һ�ζ�ȡ������
 *          uint32_t index
 * @return  NULL
 ***************************************************************************************************/
void Uart3_BuildAndSendImageDataReq( uint8_t cmdtype,  uint8_t filename, uint32_t saddr, uint16_t datalen, uint8_t index )
{
	//ADDR| LEN2|CMD| STATUS| FIMENAME|s4|e4|SN|FCS|
	uint8_t buflen = 0;
	
	memset(Uart3Handle.pTxBuffPtr,0x00,Uart3Handle.TxXferSize );

	Uart3Handle.pTxBuffPtr[0] = 0xFE;
	Uart3Handle.pTxBuffPtr[1] = 0x0B;
	Uart3Handle.pTxBuffPtr[2] = 0x00;	
	buflen = Uart3Handle.pTxBuffPtr[1]+3;
	Uart3Handle.pTxBuffPtr[3] = cmdtype;
	Uart3Handle.pTxBuffPtr[4] = 0x00;
	Uart3Handle.pTxBuffPtr[5] = filename;
	memcpy(Uart3Handle.pTxBuffPtr+6, (uint8_t*)&saddr, 4);
	memcpy(Uart3Handle.pTxBuffPtr+10,(uint8_t*)&datalen, 2);
  Uart3Handle.pTxBuffPtr[12] = index;
	Uart3Handle.pTxBuffPtr[13] = Xor_CalcFCS( Uart3Handle.pTxBuffPtr, buflen-1 );
	
	RS485COM_EN(ON);  
	HAL_UART_Transmit(&Uart3Handle, Uart3Handle.pTxBuffPtr, buflen, 5000);
	RS485COM_EN(OFF);  
	memset( redSendBuff,0x00, 25);
	redSendBuff[0]= buflen;
	memcpy( redSendBuff+1, Uart3Handle.pTxBuffPtr, buflen);
	Timer_Event_Set( &timer_10msDB[10], ReceCamDataTimeout, ENABLE, 0, Uart3_ReceiveTimeout_Evt );//������ʱ�ж�
	
}

/***************************************************************************************************
 * @fn     void Uart3_BuildAndSendTakephotosReq( uint8_t cmdtype,  uint32_t utct, uint8_t filename, uint8_t flashstatus, uint8_t index )
 *
 * @brief   ֪ͨ����ͷģ���������
 * @param   
 *          uint8_t  cmdtype        ����
 *          uint8_t  filename       �ļ�������ţ�
 *          uint32_t utct           ����ʱ��
 *          uint8_t  flashstatus    ����״̬
 *          uint32_t index          ���
 * @return  NULL
 ***************************************************************************************************/
void Uart3_BuildAndSendTakephotosReq( uint8_t cmdtype,  uint32_t utct, uint8_t filename, uint8_t flashstatus, uint8_t index )
{
	//ADDR| LEN2|CMD| STATUS| FIMENAME|flashstatus|SN|FCS|

	uint8_t buflen = 0;
	
	memset(Uart3Handle.pTxBuffPtr,0x00,Uart3Handle.TxXferSize );

	Uart3Handle.pTxBuffPtr[0] = 0xFE;
	Uart3Handle.pTxBuffPtr[1] = 0x0A;
	Uart3Handle.pTxBuffPtr[2] = 0x00;	
	buflen = Uart3Handle.pTxBuffPtr[1]+3;
	Uart3Handle.pTxBuffPtr[3] = cmdtype;
	Uart3Handle.pTxBuffPtr[4] = 0x00;
	utct = BSWAP_32(utct);
	memcpy(Uart3Handle.pTxBuffPtr+5, (uint8_t*)&utct, 4);
	
	Uart3Handle.pTxBuffPtr[9] = filename;
	Uart3Handle.pTxBuffPtr[10] = flashstatus;
  Uart3Handle.pTxBuffPtr[11] = index;
	Uart3Handle.pTxBuffPtr[12] = Xor_CalcFCS( Uart3Handle.pTxBuffPtr, buflen-1 );
	
	RS485COM_EN(ON);  
	HAL_UART_Transmit(&Uart3Handle, Uart3Handle.pTxBuffPtr, buflen, 5000);
	RS485COM_EN(OFF); 
	memset( redSendBuff,0x00, 25);
	redSendBuff[0]= buflen;
	memcpy( redSendBuff+1, Uart3Handle.pTxBuffPtr, buflen);
	Timer_Event_Set( &timer_10msDB[10], ReceCamDataTimeout*4, ENABLE, 0, Uart3_ReceiveTimeout_Evt );//������ʱ�ж�
}


/***************************************************************************************************
 * @fn      void Uart3_BuildAndSendHotResContr( uint8_t cmdtype, uint8_t hotResState, uint8_t index )
 *
 * @brief   ֪ͨ����ͷģ����м���˿����
 * @param   
 *          uint8_t cmdtype   ����
 *          uint8_t hotResState  ����˿״̬
 *          uint32_t index   ���
 * @return  NULL
 ***************************************************************************************************/
void Uart3_BuildAndSendHotResContr( uint8_t cmdtype, uint8_t hotResState, uint8_t index )
{
	//ADDR| LEN2|CMD| STATUS| FIMENAME|s4|e4|SN|FCS|

	uint8_t buflen = 0;
	
	memset(Uart3Handle.pTxBuffPtr,0x00,Uart3Handle.TxXferSize );
	Uart3Handle.pTxBuffPtr[0] = 0xFE;
	Uart3Handle.pTxBuffPtr[1] = 0x05;
	Uart3Handle.pTxBuffPtr[2] = 0x00;	
	buflen = Uart3Handle.pTxBuffPtr[1]+3;
	Uart3Handle.pTxBuffPtr[3] = cmdtype;
	Uart3Handle.pTxBuffPtr[4] = 0x00;
	Uart3Handle.pTxBuffPtr[5] = hotResState;

  Uart3Handle.pTxBuffPtr[6] = index;
	Uart3Handle.pTxBuffPtr[7] = Xor_CalcFCS( Uart3Handle.pTxBuffPtr, buflen-1 );
	
	RS485COM_EN(ON);  
	HAL_UART_Transmit(&Uart3Handle, Uart3Handle.pTxBuffPtr, buflen, 5000);
	RS485COM_EN(OFF); 
	memset( redSendBuff,0x00, 25);
	redSendBuff[0]= buflen;
	memcpy( redSendBuff+1, Uart3Handle.pTxBuffPtr, buflen);
	Timer_Event_Set( &timer_10msDB[10], ReceCamDataTimeout, ENABLE, 0, Uart3_ReceiveTimeout_Evt );//������ʱ�ж�
}

