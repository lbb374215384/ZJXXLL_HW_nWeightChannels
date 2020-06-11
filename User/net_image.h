/**************************************************************************************************
  Filename:       net_image.h
  Revised:        $Date: 2015-9-28  $
  Revision:       $Revision: V1.1  $
  User:           $User:Asong      $
  Description:    图像照片处理相关

**************************************************************************************************/
#ifndef NET_IMAGE_H
#define NET_IMAGE_H

#ifdef __cplusplus
extern "C"
{
#endif
#include "Global.h"
	
	
#define ImageFrameHeadSize   4
	
/*********************************************************************/
extern void RequestImageServerIPandPort( uint8_t connectid );
extern void RequestConnectToImageServer(uint8_t connectid, uint8_t * terminalsn, uint8_t tlen, uint8_t *joincode, uint8_t jlen );
extern void RequestUploadImageMsg( uint8_t connectid, uint16_t *serial, uint32_t utcTime, uint8_t filename, uint32_t filelen, uint8_t *md5 );
extern void SendImageMsgToImageServerFromW25X16(uint8_t connectid, uint16_t *serial, uint32_t utcTime, uint8_t filename, uint32_t start, uint32_t end, uint32_t f_imageAddr );
extern void SendImageMsgToImageServer(uint8_t connectid, uint16_t *serial, uint32_t utcTime, uint8_t filename, uint32_t start, uint32_t end, uint8_t *md5,  uint8_t *picdata );
extern uint8_t *BulidImageMsgHead( uint8_t pv, flag_t fg, uint16_t len, uint8_t cmd, uint8_t *msgptr );

//extern void  SendImagePackageHandle( uint8_t *imagePtr , uint16_t *packageCount, uint16_t *packageNum );
extern void  SendImagePackageHandle( uint32_t imageAddr , uint16_t *packageCount, uint16_t *packageNum );

extern void StartImageUpload( void );
	
extern uint16_t ImgProtocolIndex;
/*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif
