/**************************************************************************************************
  Filename:       net_device.h
  Revised:        $Date: 2018-10-8  $
  Revision:       $Revision: V1.0  $
  User:           $User:Asong      $
  Description:    4g 通信相关

**************************************************************************************************/
#ifndef _NET_DEVICE_H
#define _NET_DEVICE_H


#ifdef __cplusplus
extern "C"
{
#endif
/*********************************************************************/
#include "stm32f0xx_hal.h"
#include "Global.h"
	
extern uint8_t *NET_DEVICE_GetListHeadBuf(void);
extern uint16_t NET_DEVICE_GetListHeadLen( void );
extern uint8_t NET_DEVICE_AddDataSendList( uint8_t connId, uint8_t *buf , uint16_t dataLen , uint8_t sendcount, uint16_t protocolsn);
extern _Bool NET_DEVICE_DeleteDataSendList(void);
extern _Bool NET_DEVICE_DeleteDataSendListHead( uint16_t  ReceiveSn );
extern uint8_t NET_DEVICE_GetNumInList( netMsgList_t *head );
	
extern void NET_DEVICE_UpdateBtsMacList(char *pbuf );
extern void NET_DEVICE_ClearBtsMacList( void );
	

extern void NET_DEVICE_StateChange_Evt( void );
extern void ReceiveATResponseData_Handle( void );
extern void NetDevSend_Readqueue_Handle( void );
extern void RevNwkRsponseTimeout_Evt( void );
extern void SendHbMsgToServer( void );
extern void RevNwkData_Handle(uint8_t connectid , uint8_t *buf,   uint16_t buflen );	


extern void NET_DEVICE_RDYRsp_Handle( void );

extern void NET_DEVICE_ClosedRsp_Handle( char *pbuf );
extern void NET_DEVICE_SmartTouch_Handle( char *wifiInfor );
extern void TCP_RevNwkData_Handle(uint8_t *buf,  uint16_t buflen);
extern void UDP_RevNwkData_Handle(uint8_t *buf,  uint16_t buflen);


extern uint8_t FrameIntervalFlag ;

extern uint8_t PingCount;
extern uint8_t ConnectCount;
extern uint8_t SendErrCount;
 
extern devStates_t devState ;


 
extern uint8_t FrameIntervalFlag;
extern uint8_t NetDevSendFlag;
 
//extern uint8_t  revConnectId ;
//extern uint16_t revDatalen;

//extern uint16_t PackageNum; 
//extern uint16_t PackageCount;
//extern uint16_t RemainSize ;
//extern uint32_t startPtr ;
//extern uint32_t endPtr ;
// 
/*********************************************************************/
#ifdef __cplusplus
}
#endif

#endif
