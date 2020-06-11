/**************************************************************************************************
  Filename:       wifimoudle.h
  Revised:        $Date: 2018-10-8  $
  Revision:       $Revision: V1.0  $
  User:           $User:Asong      $
  Description:    wifimoudle œ‡πÿ

**************************************************************************************************/
#ifndef _WIFIMOUDLE_H
#define _WIFIMOUDLE_H

#ifdef __cplusplus
extern "C"
{
#endif
  
/*********************************************************************/
#include "stm32f0xx_hal.h"
#include "global.h"

extern void WIFI_DEVICE_Reset(void);

extern _Bool WIFI_DEVICE_SendCmd( char *Command, char *Response, uint32_t Timeout, uint8_t Retry );

extern _Bool WIFI_DEVICE_SendData(uint8_t connectid, uint8_t *data, uint16_t len);
extern _Bool WIFI_DEVICE_GetMac( char *Command, uint8_t *Response, uint32_t Timeout, uint8_t Retry );
extern _Bool WIFI_DEVICE_ScanAPhocList( char *Command, nearMac_t *nearmac, uint32_t Timeout, uint8_t Retry );
extern _Bool WIFI_DEVICE_SendPing( char *Command, uint32_t Timeout, uint8_t Retry );
extern void MacStringToHex( char *str, uint8_t *outMac);

extern void WifiMoudle_Init(void);

extern void ReceiveWifiATResponseData_Handle(void);


/*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif
