/**************************************************************************************************
  Filename:       Ec20.h
  Revised:        $Date: 2018-10-8  $
  Revision:       $Revision: V1.0  $
  User:           $User:Asong      $
  Description:    EC20 config

**************************************************************************************************/
#ifndef _EC20_H
#define _EC20_H

#ifdef __cplusplus
extern "C"
{
#endif
  
/*********************************************************************/
#include "stm32f0xx_hal.h"
extern void NET_DEVICE_Reset(void);
extern void NET_DEVICE_SET_BaudRate(void);
extern void NET_DEVICE_GetDevAndNwkInfor(void);
extern _Bool NET_DEVICE_Close( uint8_t connectid );
extern _Bool NET_DEVICE_TCPIPLink(uint8_t connectid , uint8_t *ip,  uint16_t *port );//"bgm.sanquan.com"  43571, 42.228.5.151
extern _Bool NET_DEVICE_SendCmd( char *Command, char *Response, uint32_t Timeout, uint8_t Retry );
extern _Bool NET_DEVICE_DetectNwkCreg( char *Command, uint32_t Timeout, uint8_t Retry );
extern _Bool NET_DEVICE_SendData(uint8_t connectid, uint8_t *data, uint16_t len);
extern void  EC20_Init(void);
extern void Uart2_BuildAndSendMsgToEC20( uint8_t connectid,uint8_t flag ,uint8_t cmdtype, uint8_t *pData, uint8_t pDataLen ,uint8_t sn );
extern _Bool NET_DEVICE_GetSN_IMSI_UTCTime_CSQ( char *Command, uint8_t *Response, uint32_t Timeout, uint8_t Retry );
extern _Bool NET_DEVICE_GetLac_Cellid( char *Command, uint8_t *lac, uint8_t *cellid, uint32_t Timeout, uint8_t Retry );
/*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif
