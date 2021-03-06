/**************************************************************************************************
  Filename:       Global.h
  Revised:        $Date: 2015-9-28  $
  Revision:       $Revision: V1.1  $
  User:           $User:Asong      $
  Description:    相关信息全局头文件

**************************************************************************************************/
#ifndef GLOBAL_H
#define GLOBAL_H

#ifdef __cplusplus
extern "C"
{
#endif
 
#include "stm32f0xx.h"
#include "stm32f0xx_hal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hal_led.h"
#include "hal_defs.h"
#include "iwdg.h"

  
#define st(x)      do { x } while (__LINE__ == -1)

#define NET_MallocBuffer	malloc
#define NET_FreeBuffer		free


#define ON                    1
#define OFF                   0


#define Num_10MsBase             3

#define ReadScalesDataQueueTask  0
#define Led_Task                 2


	#define HAL_UART_ISR_RX_MAX   128
	#define HAL_UART_ISR_TX_MAX   128
#define BufSize               128 
#define ScaleFrameLen         12
#define TimeOutCount          5


#define SUCCESS                   0x00
#define FAILURE                   0x01

#define LED_ON                 1
#define LED_OFF                0

//#define hw_ver     "V1.0"
//#define soft_ver   "V1.0"

/*********************定时任务结构体*********************/
typedef struct
{
		uint32_t   Time;
		uint8_t    Enable;
		uint8_t    Flag;
		void    (*TimersHandle) (void);
}Event_Timer;




/**************************定时任务**************************/
/*********************串口数据结构体*********************/
  typedef struct
  {
    uint8_t rxBuf[HAL_UART_ISR_RX_MAX];
#if HAL_UART_ISR_RX_MAX < 256
    uint8_t rxHead;
    volatile uint8_t rxTail;
#else
    uint16_t rxHead;
    volatile uint16_t rxTail;
#endif
    uint8_t rxTimeOut;
		uint8_t rxBusy;
		
    uint8_t txBuf[HAL_UART_ISR_TX_MAX];
#if HAL_UART_ISR_TX_MAX < 256
    volatile uint8_t txHead;
    uint8_t txTail;
#else
    volatile uint16_t txHead;
    uint16_t txTail;
#endif
   uint8_t txMT;

	 uint16_t frameIdleCount;
		
  }uartISRCfg_t;
	
	
	
	 typedef struct
  {
    uint8_t rxBuf[HAL_UART_ISR_RX_MAX];
    uint16_t rxHead;
    volatile uint16_t rxTail;
    uint8_t rxTimeOut;
		uint8_t rxBusy;
		
    uint8_t txBuf[2048UL];
    volatile uint16_t txHead;
    uint16_t txTail;
   uint8_t txMT;
	 uint16_t frameIdleCount;
		
  }mainISRCfg_t;

	
	
typedef struct _uartMsgList_t {
	
		struct _uartMsgList_t *next;
	  uint16_t bufLen;			    //数据长度
    uint8_t  *buf;				    //数据指针
		uint8_t  reSendCount;     //重发次数
	  uint8_t  flag;            //重发次数
} uartMsgList_t;
	

typedef struct
{
    /*************************发送队列*************************/
    uartMsgList_t *head;
    uartMsgList_t	*end;

}scaleDataList_t;
	
/*********************串口数据结构体*********************/
typedef struct
{
	uint8_t  rxBuf[BufSize];//20
	uint8_t  txBuf[BufSize];//20
} uartBuf_t;







/******************************串口通信数据*******************************/
/**********************************/
#define DataLenMax 50
/**************串口收到的消息数据*************/
typedef struct
{
	uint8_t Length;
  uint8_t Cmd0;
	uint8_t Cmd1;
  uint8_t Data[DataLenMax];
	
}uartRecev_t;




typedef union  
{
	uint8_t  allBits;
  struct
   {
      uint8_t  Addr     :4;
      uint8_t  Symbol   :4;
   }OneBit;
}devInforlist_t;



extern scaleDataList_t ScaleDataList;

extern void Error_Handler(void);

extern uint16_t CRC16( uint8_t *Data, uint8_t Data_length );

extern  void U3_printf( uint8_t *buf, uint16_t buflen, uint8_t type );
#ifdef __cplusplus
}
#endif

#endif
