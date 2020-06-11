/**************************************************************************************************
  Filename:       UartTask.h
  Revised:        $Date: 2015-9-28  $
  Revision:       $Revision: V1.1  $
  User:           $User:Asong      $
  Description:    串口通信任务相关函数

**************************************************************************************************/
#ifndef _UARTTASK_H_
#define _UARTTASK_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************/
#include "Global.h"


# define    CRC_INIT        0xFFFF
# define    FALSE           0

#define Crcs(__CRCVAL,__NEWCHAR){(__CRCVAL) = ((uint16_t)(__CRCVAL) >> 8)^wCRC_Table[((uint16_t)(__CRCVAL) ^ (uint16_t)(__NEWCHAR)) & 0x00ff];}


extern _Bool ScaleData_CheckListHead( void );
extern uint8_t *ScaleData_GetListHeadBuf(void);
extern uint16_t ScaleData_GetListHeadLen( void );
extern uint8_t ScaleData_AddDataSendList( uint8_t *buf , uint16_t dataLen );
extern _Bool ScaleData_DeleteDataSendList(void);
extern _Bool ScaleData_DeleteAllDataList(void);
extern uint16_t ScaleData_GetNumInList( uartMsgList_t *head );


extern uint8_t Xor_CalcFCS( uint8_t *msg_ptr, uint8_t len );
extern uint8_t Sum_CalcFCS( uint8_t *msg_ptr, uint8_t len );
extern uint16_t Get_CRC16_Check_Sum(uint8_t *pchMessage,uint32_t dwLength,uint16_t wCRC);
extern uint16_t Verify_CRC16_Check_Sum( uint8_t *pchMessage, uint32_t dwLength);
extern uint8_t* Append_CRC16_Check_Sum(uint8_t* pchMessage,uint32_t dwLength);


extern void UartScalesData_Handle( void );
extern void ReadScalesDataQueue_Evt( void );
void Uart_ScaleBuildAndSendMsg( uint8_t MouID, uint8_t *pData, uint8_t msglen);
/********************
*   全局变量引用区  *
********************/

extern const uint16_t wCRC_Table[256];

/*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif
