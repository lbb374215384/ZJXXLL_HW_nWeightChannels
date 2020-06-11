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
 * @brief  摄像头通信超时事件
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

	 if( p_ImgErrCount > 0 ) //重发次数
	 {
		 p_ImgErrCount = 0;
		 if( ImagePollState == ImagePolling )//图像轮询过程中
		 {
		   p_PackageCount++;
		   Uart3_SendImageDataReqPackageHandle( 0, &p_PackageCount, &p_PackageNum );
		 }
	 }
	 else
	 {
		 p_ImgErrCount++;
		 RS485COM_EN(ON);  
		 HAL_UART_Transmit(&Uart3Handle, redSendBuff+1, redSendBuff[0], 5000);//重发
		 RS485COM_EN(OFF);
     Timer_Event_Set( &timer_10msDB[10], ReceCamDataTimeout, ENABLE, 0, Uart3_ReceiveTimeout_Evt );//开启超时判断		 
		 memset( redSendBuff,0x00, 25);
	 }
}

/*********************************************************************
 * @fn      void ReceiveCameraData_Handle( void )
 *
 * @brief   USART3 图像数据处理
 *          
 * @param   none        
 *
 * @return  none
 */
void ReceiveCameraData_Handle( void )// FE |lenl lenh |cmd |status|data |sn |fcs
{
	uint16_t buflen = 0;
	uint8_t *ptr = NULL;

	if( Uart3Handle.RxXferCount == 0xFFFF )//数据接收
	{
		Uart3Handle.RxXferCount = 0;
		//memcpy(tcpdata ,Uart3Handle.pRxBuffPtr ,strlen((char *)Uart3Handle.pRxBuffPtr));
		//HAL_UART_Transmit_DMA(&Uart2Handle, tcpdata, strlen((char *)tcpdata));//EC20 AT命令
		
		ptr = Uart3Handle.pRxBuffPtr;
		
		if( *ptr == 0xFE  )
		{
			 buflen = BUILD_UINT16( ptr[1], ptr[2] );
			 if( buflen <= ( U3_PackageSize + 29))
			 {
				 if( *(ptr + buflen+2) == Xor_CalcFCS( ptr,  buflen+2) )//头数据正确
				 {
					 CameraData_Handle( ptr[3],  ptr+4,  buflen-2 );//数据处理
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
 * @brief   图像数据接收处理
 *          
 * @param   cmdtype 帧类型     mpbuf 数据地址   mpbuflen    数据长度
 *
 * @return  none 
 */	
  uint32_t RevStartAdr =0, offsetAddr = 0;
	uint16_t RevImglen = 0;
	uint8_t  filename =0;
void CameraData_Handle( uint8_t cmdtype,  uint8_t *mpbuf,  uint16_t mpbuflen  )//数据接受处理
{

	switch( cmdtype )
	{
		case 0x10:   // 拍照完成    |status|utc|filename|filelen|md5
		case 0x30:   //询问图像信息 |status|utc|filename|filelen|md5	
				if( *mpbuf == 0x00 )
				{
					 memcpy( (uint8_t *)&FileSize, mpbuf+6, 4);   //得到图像总大小
					 p_PackageNum = FileSize/U3_PackageSize;      //包个数+1
					 p_RemainSize = FileSize%U3_PackageSize;      //剩余的个数
		
					 SPI_FLASH_ImageErase(NV_Image1DataAddr);//NV_Image1DataAddr
					 //SPI_FLASH_BufferWrite( mpbuf+1, NV_Image1HeadAddr, NV_ImageHeadSize);//utc|filename|filelen|md5
					 memcpy(tcpdata, mpbuf+1, NV_ImageHeadSize );
					 SPI_FLASH_BufferWrite( tcpdata, NV_Image1HeadAddr, NV_ImageHeadSize);
					 /******触发询问图像内容帧******/
					 p_PackageCount = 0;
				   ImagePollState = ImagePolling;
					 Uart3_SendImageDataReqPackageHandle( filename, &p_PackageCount, &p_PackageNum );
				}
				break;
				
	  case 0x31://图像帧回复| status | filename | startaddr | datalen | data |
				memcpy( (uint8_t *)&RevStartAdr,  mpbuf+2, 4);  //起始地址 startPtr == startAddr
				memcpy( (uint8_t *)&RevImglen,  mpbuf+6, 2);    //数据长度
		    //SPI_FLASH_ImageErase(NV_Image1DataAddr);//NV_Image1DataAddr
				if( *mpbuf == 0x00 && RevStartAdr == p_startPtr )
				{  
					 Timer_Event_Set( &timer_10msDB[10], 0, DISABLE, 0, Uart3_ReceiveTimeout_Evt );//清除超时
					 p_ImgErrCount = 0;
   				 offsetAddr = NV_Image1DataAddr + RevStartAdr;
					 SPI_FLASH_BufferWrite( mpbuf+8, offsetAddr,  RevImglen);
				 	 SPI_FLASH_BufferRead( tcpdata, offsetAddr, RevImglen ); 
					 /******触发询问图像内容下一帧******/
					 p_PackageCount++;
					 Uart3_SendImageDataReqPackageHandle(  filename, &p_PackageCount, &p_PackageNum );
				 }
				 else
				 {
						 if( p_ImgErrCount > 0 ) //重发次数
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
							 Timer_Event_Set( &timer_10msDB[10], ReceCamDataTimeout, ENABLE, 0, Uart3_ReceiveTimeout_Evt );//开启超时判断 
							 memset( redSendBuff,0x00, 25);
						 }
				 }
		   break;
				
		case 0x11:
		  Uart3_BuildAndSendHotResContr( 0x10, 0x01, 0x04 );
	  	break; //打开电热丝	
		default: break;
	}
}

/***************************************************************************************************
 * @fn      void  Uart3_SendImageDataReqPackageHandle( uint8_t filename, uint16_t *packageCount, uint16_t *packageNum )
 *
 * @brief   图像数据询问
 *
 * @param   uint8_t filename 文件名  uint16_t *packageCount, 包号  uint16_t *packageNum 包总数
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
			 Uart3_BuildAndSendImageDataReq( 0x31,  filename, p_startPtr, p_RemainSize, RS485Index++ );//最后一帧
		 }
	}		 
  else  //全部图像数据读取完
	{
		 ImagePollState = ImagePollIdle;//图像轮询完毕标志
     //StartImageUpload();  //图像准备上传状态 
	}		
}	
/***************************************************************************************************
 * @fn      void SPI_FLASH_ImageErase(uint32_t ImageAddr)
 *
 * @brief   图像文件擦除
 * @param   
 *          uint32_t ImageAddr   图像地址
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
 * @brief   图像文件信息查询
 * @param   
 *          uint8_t cmdtype   命令
 *          uint8_t filename  文件名（序号）
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
	Timer_Event_Set( &timer_10msDB[10], ReceCamDataTimeout, ENABLE, 0, Uart3_ReceiveTimeout_Evt );//开启超时判断
	RS485COM_EN(OFF); 
}

/***************************************************************************************************
 * @fn      void Uart3_BuildAndSendImageDataReq( uint8_t cmdtype,  uint8_t filename, uint32_t saddr, uint16_t datalen, uint8_t index )
 *
 * @brief   图像数据获取
 * @param   
 *          uint8_t  cmdtype   命令
 *          uint8_t  filename  文件名（序号）
 *          uint32_t saddr     图像的开始地址
 *          uint16_t datalen   一次读取的数据
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
	Timer_Event_Set( &timer_10msDB[10], ReceCamDataTimeout, ENABLE, 0, Uart3_ReceiveTimeout_Evt );//开启超时判断
	
}

/***************************************************************************************************
 * @fn     void Uart3_BuildAndSendTakephotosReq( uint8_t cmdtype,  uint32_t utct, uint8_t filename, uint8_t flashstatus, uint8_t index )
 *
 * @brief   通知摄像头模块进行拍摄
 * @param   
 *          uint8_t  cmdtype        命令
 *          uint8_t  filename       文件名（序号）
 *          uint32_t utct           拍摄时间
 *          uint8_t  flashstatus    闪光状态
 *          uint32_t index          序号
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
	Timer_Event_Set( &timer_10msDB[10], ReceCamDataTimeout*4, ENABLE, 0, Uart3_ReceiveTimeout_Evt );//开启超时判断
}


/***************************************************************************************************
 * @fn      void Uart3_BuildAndSendHotResContr( uint8_t cmdtype, uint8_t hotResState, uint8_t index )
 *
 * @brief   通知摄像头模块进行加热丝控制
 * @param   
 *          uint8_t cmdtype   命令
 *          uint8_t hotResState  加热丝状态
 *          uint32_t index   序号
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
	Timer_Event_Set( &timer_10msDB[10], ReceCamDataTimeout, ENABLE, 0, Uart3_ReceiveTimeout_Evt );//开启超时判断
}

