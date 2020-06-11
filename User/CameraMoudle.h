/**************************************************************************************************
  Filename:       cameramoudle.h
  Revised:        $Date: 2018-10-8  $
  Revision:       $Revision: V1.0   $
  User:           $User:Asong       $
  Description:    485接口摄像头相关

**************************************************************************************************/
#ifndef _CAMERAMOUDLE_H
#define _CAMERAMOUDLE_H

#ifdef __cplusplus
extern "C"
{
#endif
  
/*********************************************************************/
#include "stm32f0xx_hal.h"

	
extern void ReceiveCameraData_Handle( void );
extern void CameraData_Handle( uint8_t cmdtype,  uint8_t *mpbuf,  uint16_t mpbuflen  );//数据接受处理
extern void SPI_FLASH_ImageErase(uint32_t ImageAddr);
extern void  Uart3_SendImageDataReqPackageHandle( uint8_t filename, uint16_t *packageCount, uint16_t *packageNum );
extern void Uart3_BuildAndSendImageinforReq( uint8_t cmdtype, uint8_t filename , uint8_t index );
extern void Uart3_BuildAndSendImageDataReq( uint8_t cmdtype,  uint8_t filename, uint32_t saddr, uint16_t datalen, uint8_t index );
extern void Uart3_BuildAndSendTakephotosReq( uint8_t cmdtype,  uint32_t utct, uint8_t filename, uint8_t flashstatus, uint8_t index );
extern void Uart3_BuildAndSendHotResContr( uint8_t cmdtype, uint8_t hotResState, uint8_t index );
	
extern uint32_t FileSize;
extern uint8_t  RS485Index;
extern uint8_t  p_ImgErrCount;
//extern imagePollState_t  ImagePollState;

extern uint16_t p_PackageNum; 
extern uint16_t p_PackageCount;
extern uint16_t p_RemainSize;
extern uint32_t p_startPtr ,f_RevAdr;
extern uint32_t p_endPtr ;

extern uint8_t redSendBuff[25];
/*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif
