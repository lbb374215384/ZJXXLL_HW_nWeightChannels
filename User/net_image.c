/* Includes ------------------------------------------------------------------*/
//#include <stdio.h>
//#include <stdlib.h>
#include <math.h>
#include "stm32f0xx.h"
#include "usart123.h"
#include "Global.h"
#include "usart123.h"
#include "Variable.h"
#include "gpio.h"
#include "Ec20.h"
#include "rtc.h"
#include "net_device.h"
#include "net_image.h"
#include "TimeEvent.h"
#include "md5.h"
#include "spi_flash.h"

//    | PV| FLAG| LEN |cmd| serial| Date |FileName| start | end | MD5 | Data |
//    | 1 |  1  |  2  | 1 |    2  |  4   |   1    | 4     |4    |  16 |  N   | 

//    | PV| FLAG| LEN |cmd| serial|   (status ?)£ ????   | 
//    | 1 |  1  |  2  | 1 |    2  |     5               |
/*********************************************************************
 * MACROS
 */
/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

flag_t Flag ;
uint16_t ImgProtocolIndex = 0;
uint8_t *BulidImageMsgHead( uint8_t pv, flag_t fg, uint16_t len, uint8_t cmd, uint8_t *msgptr );

/***************************************************************************************************
 * @fn      void RequestImageServerIPandPort( void )
 *
 * @brief   »ñÈ¡¹¤×÷·þÎñÆ÷
 *
 * @param   void
 *
 * @return  void
 ***************************************************************************************************/
void RequestImageServerIPandPort( uint8_t connectid )
{
//    | PV| FLAG| LEN |cmd| DATA |
//    | 1 |  1  |  2  | 1 |  1   |    
	uint8_t *msg_ptr = NULL ,*pBuf = NULL;
  uint16_t msglen = 0,len;
	
	Flag.Byte = 0;
	Flag.Bits.SR = Req;
	
	len = 2;
	msglen = ImageFrameHeadSize + len;

	pBuf = (uint8_t *)malloc( msglen );
	
	if( pBuf != NULL )
  {
		/***************head***************/
    msg_ptr = pBuf;
		pBuf = BulidImageMsgHead(IMGPV, Flag, len, CMD_GetServerInfor , pBuf );
		/***************data***************/
		*pBuf = 0x02;
		
    NET_DEVICE_AddDataSendList(connectid, msg_ptr, msglen , 1, 0x7FFF );
	}
}

/***************************************************************************************************
 * @fn      void RequestWorkServerIPandPort( void )
 *
 * @brief   ÇëÇó½ÓÈëÁ´½Ó
 *
 * @param   void
 *
 * @return  void
 ***************************************************************************************************/
void RequestConnectToImageServer(uint8_t connectid, uint8_t * terminalsn, uint8_t tlen, uint8_t *joincode, uint8_t jlen )
{
	
//    | PV| FLAG| LEN |cmd| TLen | TerminalSN | JLen | JoinCode |
//    | 1 |  1  |  2  | 1 |  1   |      n     |    1 |     m    |
	uint8_t *msg_ptr = NULL ,*pBuf = NULL;
  uint16_t msglen = 0,len;
	
	Flag.Byte = 0;
	Flag.Bits.SR = Req;
	
	len = 1+tlen +jlen+2;
	msglen = ImageFrameHeadSize + len;

	pBuf = (uint8_t *)malloc( msglen );
	
	if( pBuf != NULL )
  {
		/***************head***************/
    msg_ptr = pBuf;
		pBuf = BulidImageMsgHead(IMGPV, Flag, len, CMD_ConnectServer , pBuf );
		/***************data***************/
		 *pBuf++ = tlen;
		 memcpy(pBuf, terminalsn, tlen);
		 pBuf += tlen;
		
		*pBuf++ = jlen;
		 memcpy(pBuf, joincode, jlen);
		
     NET_DEVICE_AddDataSendList(connectid, msg_ptr, msglen ,1, 0x7FFF );
	}
}

/***************************************************************************************************
 * @fn      void RequestUploadImageMsg( uint16_t serial, uint32_t utcTime, uint8_t filename, uint32_t filelen, uint8_t *md5 )
 *
 * @brief   ÇëÇóÉÏ´«Í¼ÏñÎÄ¼þ
 *
 * @param   void
 *
 * @return  void
 ***************************************************************************************************/
void RequestUploadImageMsg( uint8_t connectid, uint16_t *serial, uint32_t utcTime, uint8_t filename, uint32_t filelen, uint8_t *md5 )
{
//      B1   00   001C  03  0000    5C0769BB   1E       00009842   15D99A7903F47DED613D7DAA976507D7"
//    | PV| FLAG| LEN |cmd| Serial |  Date  | FileName | FileLen |    MD5         |
//    | 1 |  1  |  2  | 1 |    2   |   4    |     1    |    4    |     16         |
  
	uint8_t *msg_ptr = NULL ,*pBuf = NULL;
  uint16_t msglen = 0,len;
	
	Flag.Byte = 0;
	Flag.Bits.SR = Req;
	
	len = 1+27;
	msglen = ImageFrameHeadSize + len;

	pBuf = (uint8_t *)malloc( msglen );
	
	if( pBuf )
  {
		/***************head***************/
    msg_ptr = pBuf;
		pBuf = BulidImageMsgHead(IMGPV, Flag, len, 0x03 , pBuf );
		/***************data***************/
    *pBuf++ = HI_UINT16(*serial);
		*pBuf++ = LO_UINT16(*serial);//sn
		*pBuf++ = BREAK_UINT32( utcTime, 3 );
		*pBuf++ = BREAK_UINT32( utcTime, 2 );
		*pBuf++ = BREAK_UINT32( utcTime, 1 );
		*pBuf++ = BREAK_UINT32( utcTime, 0 );//utc
		*pBuf++ = filename; //FileName
		
	  *pBuf++ = BREAK_UINT32( filelen, 3 );
		*pBuf++ = BREAK_UINT32( filelen, 2 );
		*pBuf++ = BREAK_UINT32( filelen, 1 );
		*pBuf++ = BREAK_UINT32( filelen, 0 );//FileLen
		
		 memcpy( pBuf, md5, md5len );//md5
     NET_DEVICE_AddDataSendList(connectid, msg_ptr, msglen , 1,*serial );
		 *serial  = (*serial +1)% 32766U;//7FFE;
	}
}

/***************************************************************************************************
 * @fn      void SendImageMsgToImageServer( uint16_t serial, uint32_t utcTime, uint8_t filename, uint32_t filelen, uint8_t *md5 )
 *
 * @brief   ÉÏ´«Í¼ÏñÊý¾Ý
 *
 * @param   void
 *
 * @return  void
 ***************************************************************************************************/
void SendImageMsgToImageServerFromW25X16(uint8_t connectid, uint16_t *serial, uint32_t utcTime, uint8_t filename, uint32_t start, uint32_t end, uint32_t f_imageAddr )
{
//    | PV| FLAG| LEN |cmd| serial| Date |FileName| start | end | MD5 | Data |1424( 1280 000-4FF 500-9FF)
//    | 1 |  1  |  2  | 1 |    2  |  4   |   1    | 4     |4    |  16 |  N   | 
	
	uint8_t *msg_ptr = NULL ,*pBuf = NULL;
  uint16_t msglen = 0,len, imglen;
	
	Flag.Byte = 0;
	Flag.Bits.SR = Req;
	
	len = 1+31+end-start+1;
	msglen = ImageFrameHeadSize + len;

	pBuf = (uint8_t *)malloc( msglen );
	
	if( pBuf )
  {
		/***************head***************/
    msg_ptr = pBuf;
		pBuf = BulidImageMsgHead(IMGPV, Flag, len, 0x04 , pBuf );
		/***************data***************/
    *pBuf++ = LO_UINT16(*serial);//·þÎñÆ÷bug½ÓÊÕµÄ·½Ê½ÊÇÐ¡¶Ë£¬·µ»ØµÄ×´Ì¬ÊÇ´ó¶Ë
		*pBuf++ = HI_UINT16(*serial);//sn
		*pBuf++ = BREAK_UINT32( utcTime, 3 );
		*pBuf++ = BREAK_UINT32( utcTime, 2 );
		*pBuf++ = BREAK_UINT32( utcTime, 1 );
		*pBuf++ = BREAK_UINT32( utcTime, 0 );//utc
		*pBuf++ = filename; //FileName
		
		*pBuf++ = BREAK_UINT32( start, 3 );
		*pBuf++ = BREAK_UINT32( start, 2 );
		*pBuf++ = BREAK_UINT32( start, 1 );
		*pBuf++ = BREAK_UINT32( start, 0 );
		
	  *pBuf++ = BREAK_UINT32( end, 3 );
		*pBuf++ = BREAK_UINT32( end, 2 );
		*pBuf++ = BREAK_UINT32( end, 1 );
	  *pBuf++ = BREAK_UINT32( end, 0 );

		 /*********w25x16flashÖÐ¶ÁÈ¡*********/
		 imglen = end-start+1;
		 SPI_FLASH_BufferRead( pBuf+md5len, f_imageAddr, imglen ); 
		 Create_MD5_CheckSum( pBuf+md5len,  imglen, pBuf);
		
     NET_DEVICE_AddDataSendList(connectid, msg_ptr, msglen , 1,*serial );
		 *serial  = (*serial +1)% 32766U;//7FFE;
	}
}

/***************************************************************************************************
 * @fn      void SendImageMsgToImageServer( uint16_t serial, uint32_t utcTime, uint8_t filename, uint32_t filelen, uint8_t *md5 )
 *
 * @brief   ÉÏ´«Í¼ÏñÊý¾Ý
 *
 * @param   void
 *
 * @return  void
 ***************************************************************************************************/
void SendImageMsgToImageServer(uint8_t connectid, uint16_t *serial, uint32_t utcTime, uint8_t filename, uint32_t start, uint32_t end, uint8_t *md5,  uint8_t *picdata )
{
//    | PV| FLAG| LEN |cmd| serial| Date |FileName| start | end | MD5 | Data |1424( 1280 000-4FF 500-9FF)
//    | 1 |  1  |  2  | 1 |    2  |  4   |   1    | 4     |4    |  16 |  N   | 
	
	uint8_t *msg_ptr = NULL ,*pBuf = NULL;
  uint16_t msglen = 0,len, imglen;
	
	Flag.Byte = 0;
	Flag.Bits.SR = Req;
	
	len = 1+31+end-start+1;
	msglen = ImageFrameHeadSize + len;

	pBuf = (uint8_t *)malloc( msglen );
	
	if( pBuf )
  {
		/***************head***************/
    msg_ptr = pBuf;
		pBuf = BulidImageMsgHead(IMGPV, Flag, len, 0x04 , pBuf );
		/***************data***************/
    *pBuf++ = LO_UINT16(*serial);//·þÎñÆ÷bug½ÓÊÕµÄ·½Ê½ÊÇÐ¡¶Ë£¬·µ»ØµÄ×´Ì¬ÊÇ´ó¶Ë
		*pBuf++ = HI_UINT16(*serial);//sn
		*pBuf++ = BREAK_UINT32( utcTime, 3 );
		*pBuf++ = BREAK_UINT32( utcTime, 2 );
		*pBuf++ = BREAK_UINT32( utcTime, 1 );
		*pBuf++ = BREAK_UINT32( utcTime, 0 );//utc
		*pBuf++ = filename; //FileName
		
		*pBuf++ = BREAK_UINT32( start, 3 );
		*pBuf++ = BREAK_UINT32( start, 2 );
		*pBuf++ = BREAK_UINT32( start, 1 );
		*pBuf++ = BREAK_UINT32( start, 0 );
		
	  *pBuf++ = BREAK_UINT32( end, 3 );
		*pBuf++ = BREAK_UINT32( end, 2 );
		*pBuf++ = BREAK_UINT32( end, 1 );
	  *pBuf++ = BREAK_UINT32( end, 0 );
     
		 memcpy(pBuf, md5, md5len );
		 pBuf += md5len;
		 imglen = end-start+1;
     memcpy(pBuf, picdata, imglen);

		
     NET_DEVICE_AddDataSendList(connectid, msg_ptr, msglen , 1,*serial );
		 *serial  = (*serial +1)% 32766U;//7FFE;
	}
}

/***************************************************************************************************
 * @fn      uint8_t *BulidImageMsgHead( uint8_t pv, flag_t fg, uint16_t len , uint8_t *msgptr )
 *
 * @brief   Í¼Ïñ¹¤×÷Êý¾ÝÍ·´ò°ü
 *
 * @param   void
 *
 * @return  void
 ***************************************************************************************************/
uint8_t *BulidImageMsgHead( uint8_t pv, flag_t fg, uint16_t len, uint8_t cmd, uint8_t *msgptr )
{
/**
 | PV| FLAG| LEN |cmd| 
**/
	/***************head***************/
	*msgptr++ = pv;  
	*msgptr++ = fg.Byte;
	*msgptr++ = HI_UINT16(len);//len
	*msgptr++ = LO_UINT16(len);
	*msgptr++ = cmd;

	 return msgptr;
}

/***************************************************************************************************
 * @fn      uint8_t *BulidImageMsgHead( uint8_t pv, flag_t fg, uint16_t len , uint8_t *msgptr )
 *
 * @brief   Í¼Ïñ¹¤×÷Êý¾ÝÍ·´ò°ü
 *
 * @param   void
 *
 * @return  void
 ***************************************************************************************************/
void  SendImagePackageHandle( uint32_t imageAddr , uint16_t *packageCount, uint16_t *packageNum )
{
	if( *packageCount <= *packageNum )
	{ 
		startPtr = Net_PackageSize*(*packageCount);//(000-4FF  500 - 9FF  A00 - EFF  F00 - 14ff  )
		if( *packageCount != *packageNum )//(10 0-9 )
		{
			 endPtr = startPtr + Net_PackageSize-1;
		}
		else 
		{
			 endPtr = startPtr + RemainSize-1;
		}
#ifdef U3print_Debug
		memset(Uart3Handle.pTxBuffPtr,0x00,Uart3Handle.TxXferSize );
		sprintf((char *)Uart3Handle.pTxBuffPtr, "--PackageCount: %d \r\n",(*packageCount));
		U3_printf(Uart3Handle.pTxBuffPtr, 0, 0);
#endif		
		SendImageMsgToImageServerFromW25X16(CONNID_IMAGE, &ImgProtocolIndex, DeviceInfor.Secs , FileName, startPtr, endPtr, imageAddr+startPtr);
	}
	else
	{
		NET_DEVICE_Close( CONNID_IMAGE );
		GetIpType = 0; //¹¤×÷·þÎñÆ÷ÀàÐÍ
		imageState = IMG_HOLD;//Í¼ÏñÊý¾Ý´«ÊäÍê³É
		
	  SendHbMsgToServer( );
    Timer_Event_Set( &timer_10msDB[7],  DeviceInfor.Moulde.HbIntervals *1000,  ENABLE,  0,  SendHbMsgToServer);
		
#ifdef U3print_Debug
		U3_printf((uint8_t *)"\r\n------------  upload jpeg  Over !!!! --------------\r\n", 0, 0);
		U3_printf((uint8_t *)"\r\n------------  upload jpeg  Over !!!! --------------\r\n", 0, 0);
		U3_printf((uint8_t *)"\r\n------------  upload jpeg  Over !!!! --------------\r\n", 0, 0);
		U3_printf((uint8_t *)"\r\n------------  upload jpeg  Over !!!! --------------\r\n", 0, 0);
		U3_printf((uint8_t *)"\r\n------------  upload jpeg  Over !!!! --------------\r\n", 0, 0);
		U3_printf((uint8_t *)"\r\n------------  upload jpeg  Over !!!! --------------\r\n", 0, 0);
#endif
	}		
}	

/***************************************************************************************************
 * @fn      void StartImageUpload( void )
 *
 * @brief   ¿ªÆôÍ¼ÏñÉÏ´«
 *
 * @param   void
 *
 * @return  void
 ***************************************************************************************************/
void StartImageUpload( void )
{
	  if( imageState == IMG_HOLD )
		{
			 Timer_Event_Set( &timer_10msDB[5],  100,  ENABLE,  0, NET_DEVICE_StateChange_Evt );//200msºó×´Ì¬Á´½ÓÊÂ¼þ
			 devState = DEV_NWK_CONNECTING;
			 connectId = CONNID_GETIP;
			 GetIpType = 1; 
			 imageState = IMG_READY;//Í¼Ïñ×¼±¸×´Ì¬ 
		}
}
 



