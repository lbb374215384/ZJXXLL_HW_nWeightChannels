/* Includes ------------------------------------------------------------------*/
//#include <math.h>
#include "stm32f0xx.h"
#include "Global.h"
#include "AppTask.h"
#include "UartTask.h"
#include "Variable.h"
#include "bsp_periph.h"



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
 
#define PreambleLen  2
const uint8_t Preamble[PreambleLen]={0xAA,0x55};
const uint16_t wCRC_Table[256]=
{
  0x0000,0xC0C1,0xC181,0x0140,0xC301,0x03C0,0x0280,0xC241,0xC601,0x06C0,
  0x0780,0xC741,0x0500,0xC5C1,0xC481,0x0440,0xCC01,0x0CC0,0x0D80,0xCD41,
  0x0F00,0xCFC1,0xCE81,0x0E40,0x0A00,0xCAC1,0xCB81,0x0B40,0xC901,0x09C0,
  0x0880,0xC841,0xD801,0x18C0,0x1980,0xD941,0x1B00,0xDBC1,0xDA81,0x1A40,
  0x1E00,0xDEC1,0xDF81,0x1F40,0xDD01,0x1DC0,0x1C80,0xDC41,0x1400,0xD4C1,
  0xD581,0x1540,0xD701,0x17C0,0x1680,0xD641,0xD201,0x12C0,0x1380,0xD341,
  0x1100,0xD1C1,0xD081,0x1040,0xF001,0x30C0,0x3180,0xF141,0x3300,0xF3C1,
  0xF281,0x3240,0x3600,0xF6C1,0xF781,0x3740,0xF501,0x35C0,0x3480,0xF441,
  0x3C00,0xFCC1,0xFD81,0x3D40,0xFF01,0x3FC0,0x3E80,0xFE41,0xFA01,0x3AC0,
  0x3B80,0xFB41,0x3900,0xF9C1,0xF881,0x3840,0x2800,0xE8C1,0xE981,0x2940,
  0xEB01,0x2BC0,0x2A80,0xEA41,0xEE01,0x2EC0,0x2F80,0xEF41,0x2D00,0xEDC1,
  0xEC81,0x2C40,0xE401,0x24C0,0x2580,0xE541,0x2700,0xE7C1,0xE681,0x2640,
  0x2200,0xE2C1,0xE381,0x2340,0xE101,0x21C0,0x2080,0xE041,0xA001,0x60C0,
  0x6180,0xA141,0x6300,0xA3C1,0xA281,0x6240,0x6600,0xA6C1,0xA781,0x6740,
  0xA501,0x65C0,0x6480,0xA441,0x6C00,0xACC1,0xAD81,0x6D40,0xAF01,0x6FC0,
  0x6E80,0xAE41,0xAA01,0x6AC0,0x6B80,0xAB41,0x6900,0xA9C1,0xA881,0x6840,
  0x7800,0xB8C1,0xB981,0x7940,0xBB01,0x7BC0,0x7A80,0xBA41,0xBE01,0x7EC0,
  0x7F80,0xBF41,0x7D00,0xBDC1,0xBC81,0x7C40,0xB401,0x74C0,0x7580,0xB541,
  0x7700,0xB7C1,0xB681,0x7640,0x7200,0xB2C1,0xB381,0x7340,0xB101,0x71C0,
  0x7080,0xB041,0x5000,0x90C1,0x9181,0x5140,0x9301,0x53C0,0x5280,0x9241,
  0x9601,0x56C0,0x5780,0x9741,0x5500,0x95C1,0x9481,0x5440,0x9C01,0x5CC0,
  0x5D80,0x9D41,0x5F00,0x9FC1,0x9E81,0x5E40,0x5A00,0x9AC1,0x9B81,0x5B40,
  0x9901,0x59C0,0x5880,0x9841,0x8801,0x48C0,0x4980,0x8941,0x4B00,0x8BC1,
  0x8A81,0x4A40,0x4E00,0x8EC1,0x8F81,0x4F40,0x8D01,0x4DC0,0x4C80,0x8C41,
  0x4400,0x84C1,0x8581,0x4540,0x8701,0x47C0,0x4680,0x8641,0x8201,0x42C0,
  0x4380,0x8341,0x4100,0x81C1,0x8081,0x4040};
/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */
 
/*********************************************************************
 * LOCAL VARIABLES
 */

scaleDataList_t ScaleDataList;
	
/***************************************************************************************************
 * @fn      _Bool ScaleData_CheckListHead(void)
 *
 * @brief   检查发送链表头是否为空
 *
 * @param   NULL
 *
 * @return  0-空	1-不为空
 ***************************************************************************************************/
_Bool ScaleData_CheckListHead( void )
{
	if( ScaleDataList.head == NULL )
			return 0;
	else
			return 1;
}

/***************************************************************************************************
 * @fn      uint8_t *ScaleData_GetListHeadBuf(void)
 *
 * @brief   获取链表里需要发送的数据指针
 *
 * @param   NULL
 *
 * @return  获取链表里需要发送的数据指针
 ***************************************************************************************************/
uint8_t *ScaleData_GetListHeadBuf(void)
{
  return ScaleDataList.head->buf;
}

/***************************************************************************************************
 * @fn      uint16_t NET_DEVICE_GetListHeadLen( void )
 *
 * @brief   获取链表里需要发送的数据长度
 *
 * @param   NULL
 *
 * @return  获取链表里需要发送的数据长度
 ***************************************************************************************************/
uint16_t ScaleData_GetListHeadLen( void )
{
    return ScaleDataList.head->bufLen;
}
/***************************************************************************************************
 * @fn      uint8_t NET_DEVICE_AddDataSendList( uint8_t *buf , uint16_t dataLen )
 *
 * @brief   在发送链表尾新增一个发送链表
 *
 * @param   connId:链接号  buf：需要发送的数据 dataLen：数据长度
 *
 * @return  0-成功	其他-失败
 ***************************************************************************************************/
uint8_t ScaleData_AddDataSendList( uint8_t *buf , uint16_t dataLen )
{
   uartMsgList_t *current = NULL;
	
	 current = (uartMsgList_t *)malloc(sizeof(uartMsgList_t));//分配内存
	
   if( current == NULL )
	 {
		 free(buf); 
		 return 1;
   }
     
	 
   if( ScaleDataList.head == NULL )							//如果head为NULL
	 { 
     ScaleDataList.head = current;							//head指向当前分配的内存区
	 }
   else										//如果head不为NULL
     ScaleDataList.end->next = current;				//则end指向当前分配的内存区

		current->buf = buf;//复制数据
    current->bufLen = dataLen;
    current->next = NULL;								//下一段为NULL
	 
    current->reSendCount = 1;
	  current->flag = 0;//0：回复帧   1：主动上发帧
	 
    ScaleDataList.end = current;        //end指向当前分配的内存区
	 
    return 0;
}

/***************************************************************************************************
 * @fn      _Bool NET_DEVICE_DeleteDataSendList(void)
 *
 * @brief   从链表头删除一个链表
 *
 * @param   NULL
 *
 * @return  NULL
 ***************************************************************************************************/
_Bool ScaleData_DeleteDataSendList(void)
{
    uartMsgList_t  *pNewhead = ScaleDataList.head->next;	//保存链表头的下一段数据地址
	
    ScaleDataList.head->bufLen = 0;
	  ScaleDataList.head->reSendCount = 0;
	  ScaleDataList.head->flag = 0;
    ScaleDataList.head->next = NULL;
    free(ScaleDataList.head->buf);			//释放内存
    free(ScaleDataList.head);				    //释放内存
    ScaleDataList.head = pNewhead;			//链表头指向下一段数据
    return 0;
}

/***************************************************************************************************
 * @fn      _Bool ScaleData_DeleteAllDataList(void)
 *
 * @brief   从链表头删除整个链表
 *
 * @param   NULL
 *
 * @return  NULL
 ***************************************************************************************************/
_Bool ScaleData_DeleteAllDataList(void)
{
	 uartMsgList_t *phead = ScaleDataList.head->next;//保存链表头的下一段数据地址

	 while( ScaleDataList.head != NULL )
	 {
		 ScaleDataList.head->bufLen = 0;
		 ScaleDataList.head->reSendCount = 0;
		 ScaleDataList.head->flag = 0;
		 ScaleDataList.head->next = NULL;
		 free(ScaleDataList.head->buf);			  //释放内存
		 free(ScaleDataList.head);				    //释放内存
		 
		 ScaleDataList.head = phead;					//链表头指向下一段数据
		 phead = phead->next;
	 }
	 
	 return 0;
}


/*********************************************************************
 * @fn   uint8_t ScaleData_GetNumInList( uartMsgList_t *head )
 *
 * @brief 统计该列表的元素个数
 *          
 * @param            
 *
 * @return  none
 */

uint16_t ScaleData_GetNumInList( uartMsgList_t *head )
{    
	 uartMsgList_t *q = head;
   uint16_t num = 0;

		while(q->next != NULL)
		{
			q = q->next;
			num++;
		}
		
		return num;
}

/*********************************************************************
 * @fn      void UartScalesData_Handle( void )
 *
 * @brief   接收秤盘数据处理
 *          
 * @param            
 *
 * @return  none

		+23455\r\n
		-   10\r\n
		+  120\r\n
*/

uint8_t Channel = 0;
uint8_t RevDatalen = 0;
uint8_t rxBUF[100], txBUF[100], datalen ;
devInforlist_t devInfor;

uint8_t  deviceId = 0;
uint8_t sn = 0;
void UartScalesData_Handle( void )
{
	uint8_t *ptr = NULL;
	uint8_t *pBuf = NULL, pBuflen=0;
	char *start = NULL;
	char *end = NULL;
	uint8_t i=0;
	
	
	for(Channel=0; Channel<5;Channel++)
	{
		if( isrCfg[Channel].rxTimeOut == 1 )
	  {
		  isrCfg[Channel].rxTimeOut = 0;
			RevDatalen = HalUARTRxAvailISR( Channel );
		  if( RevDatalen )//存在数据
		  {
				ptr = (uint8_t *)&rxBUF;
				start = (char *)&rxBUF;
				HalUARTReadISR(rxBUF, RevDatalen,Channel);
				
				start = strchr(start,'@');
				end = strchr(start,'#');
				if(start != NULL && end != NULL)//秤盘回复的数据
				{
					 if( strstr(start, "@RP,"))//所有参数返回
					 {
						 start = strstr(start, "@RP,");
						 deviceId = strtoul( start+4, NULL, 10);
						 if( deviceId <= 4 )
						 {
							 *(start+4) = 0x30+deviceId;//修改层号
							 pBuflen = strlen(start);
							 
							 	/**************数据打包**************/
								pBuf = (uint8_t *)malloc( pBuflen );
								if( pBuf )
								{
									memcpy(pBuf, (uint8_t *)start, pBuflen );
									ScaleData_AddDataSendList( pBuf,pBuflen );
									HalLedBlink(HAL_LED[Channel], 1, 30, 90);
								}
						 }
						 memset(rxBUF, 0x00, 100);
						 return;
					 }
					 
					 if( strstr(start, "RID"))//ID、版本号返回
					 {
						 start = strstr(start, "@RID,");
						 deviceId = strtoul( start+5, NULL, 10);
						 if(deviceId<=4)
						 {
							 *(start+5) = 0x30+deviceId;//修改层号
							 pBuflen = strlen(start);
							 
							 	/**************数据打包**************/
								pBuf = (uint8_t *)malloc( pBuflen );
								if( pBuf )
								{
									memcpy(pBuf, (uint8_t *)start, pBuflen );
									ScaleData_AddDataSendList( pBuf,pBuflen );
									HalLedBlink(HAL_LED[Channel], 1, 30, 90);
								}
						 }
						 memset(rxBUF, 0x00, 100);
						 return;
					 }
					 memset(rxBUF, 0x00, 100);
					 return;				 
				}
				else
				{
					while( RevDatalen >= 12 )
					{
						if( *ptr == 0x5A )//头数据正确
						{
							if( Sum_CalcFCS(ptr, 11) == *(ptr+11) )//和校验成功
							{
								/**************数据打包**************/
//								*(ptr+12) = sn++;
//								for(i=0; i<=Channel;i++)
//								{
								  Uart_ScaleBuildAndSendMsg( Channel, ptr, 12);
//								}
								
//								pBuf = (uint8_t *)malloc( 12 );
//								if( pBuf )
//								{
//									*(ptr+1) = (*(ptr+1)&0xF0)|Channel;//地址加入
//									*(ptr+RevDatalen-1) = Sum_CalcFCS(ptr, (RevDatalen-1) );//配置校验和
//									
//									memcpy(pBuf, ptr, 12 );
//									ScaleData_AddDataSendList( pBuf,12 );
//									HalLedBlink(HAL_LED[Channel], 1, 30, 90);
//								}
								break;
							}
							else
							{
								ptr++;
								RevDatalen--;	
							}
						}
						else
						{
							 ptr++;
							 RevDatalen--;
						}
					}
				}
				memset(rxBUF, 0x00, 100);
		  }
	  }
	}
}



/*********************************************************************
 * @fn     void UartDataHandle(void)
 *
 * @brief  收到主控板命令数据处理
 *          
 * @param   none        
 *
 * @return  none
 */
void UartMainDataHandle( void )
{
  uint8_t buflen = 0;
	char  *ptr = NULL;
	char  *start = NULL,*end = NULL;

	if( Uart5Handle.RxXferCount == 0xFFFF )//数据接收
	{
		Uart5Handle.RxXferCount = 0;
		start = strchr((char *)Uart5Handle.pRxBuffPtr,'@');
		ptr = start;
		end = strchr((char *)Uart5Handle.pRxBuffPtr,'#');
		
		if( ptr != NULL && end != NULL )
		{
			ptr = strchr(ptr, ',');
			deviceId = strtoul( ptr+1, NULL, 10);
			if( deviceId <= 4 )
			{
				*(ptr+1) = '0';//修改层号
				buflen = end - start+1;
				HAL_UART_Transmit(&UartHandle[deviceId], (uint8_t *)start, buflen, 0xFFFFFFFFU);
				
        memset(Uart5Handle.pRxBuffPtr, 0x00, Uart5Handle.RxXferSize );
			  return;	
			}
		}
		memset(Uart5Handle.pRxBuffPtr, 0x00, Uart5Handle.RxXferSize );
	  HAL_UART_Receive_DMA(&Uart5Handle, Uart5Handle.pRxBuffPtr,  Uart5Handle.RxXferSize);
	 }
}


/***************************************************************************************************
 * @fn      void Uart_BuildAndSendMsgToZigbee( uint8 Addr, uint8 Cmd, uint8 *pData, uint16 dataLen, uint8 SeqNum )
 *
 * @brief   Build and send a Gw msg
 * @param   uint8 Cmd - command ID
 *          uint16 SrcAddr - SrcAddr
 *          byte dataLen
 *          byte *pData
 *
 * @return  void
 ***************************************************************************************************/
void Uart_ScaleBuildAndSendMsg( uint8_t MouID, uint8_t *pData, uint8_t msglen)//msglen=12
{
  uint8_t *msg_ptr = NULL;

  /**************数据打包**************/
	msg_ptr = (uint8_t *)malloc( msglen);
	if( msg_ptr )
	{
		*(pData+1) = (*(pData+1)&0xF0)|MouID;//地址加入
		*(pData+msglen-1) = Sum_CalcFCS(pData, (msglen-1) );//配置校验和
		
		memcpy(msg_ptr, pData, msglen );
		ScaleData_AddDataSendList( msg_ptr,msglen);
		HalLedBlink(HAL_LED[Channel], 1, 30, 90);
	}
}
/*********************************************************************
 * @fn      void ReadScalesDataQueue_Evt( void )
 *
 * @brief   串口数据队列读取,发送
 *          
 * @param   none          
 *
 * @return  none
 */

void ReadScalesDataQueue_Evt( void )
{
	uartMsgList_t *pItem = ScaleDataList.head;
	
	timer_10msDB[ReadScalesDataQueueTask].Flag = 0;
  timer_10msDB[ReadScalesDataQueueTask].Enable = ENABLE;
  timer_10msDB[ReadScalesDataQueueTask].Time = 20;
		
	if( pItem != NULL )//消息缓冲不为空,接收完成标志
	{
		if( pItem->reSendCount == 0 )
		{
			ScaleData_DeleteDataSendList();
			pItem = ScaleDataList.head;
		}
		else
		{
			pItem->reSendCount--;
		}
		
#ifdef DMA_TX
	    HAL_UART_Transmit_DMA(&Uart5Handle, pItem->buf,  pItem->bufLen); 
#else		
		  HAL_UART_Transmit(&Uart5Handle, pItem->buf,  pItem->bufLen,0xFFFFFFFFU);
		  ScaleData_DeleteDataSendList();
#endif
		
		HalLedBlink(HAL_LED_6, 1, 20, 20);
		
	}
}


/**
  * @brief  Tx Transfer completed callback
  * @param  UartHandle: UART handle. 
  * @note   This example shows a simple way to report end of DMA Tx transfer, and 
  *         you can add your own implementation. 
  * @retval None
  */

#ifdef DMA_TX
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  // uartMsgList_t *pItem = ScaleDataList.head;
	 ScaleData_DeleteDataSendList();
	 Timer_Event_Set( &timer_10msDB[ReadScalesDataQueueTask],  10, ENABLE, 0, ReadScalesDataQueue_Evt);//关闭状态包
}

void HAL_UART_TxHalfCpltCallback(UART_HandleTypeDef *huart)
{
   
	 //ScaleData_DeleteDataSendList();
}
#endif
/***************************************************************************************************
 * @fn      void Usart_BuildAndSendErrRsp( uint8_t cmd0, uint8_t errCode )
 *
 * @brief   Build and send rspone
 * @param   
 *          uint8_t cmdtype
 *          byte dataLen
 *          byte *pData
 *
 * @return  void
 ***************************************************************************************************/
void Usart_BuildAndSendErrRsp( uint8_t cmd0, uint8_t cmd1, uint8_t errCode )
{
      
}





/***************************************************************************************************
 * @fn      SPIMgr_CalcFCS
 *
 * @brief   Calculate the FCS of a message buffer by XOR'ing each byte.
 *          Remember to NOT include SOP and FCS fields, so start at the CMD field.
 *
 * @param   byte *msg_ptr - message pointer
 * @param   byte len - length (in bytes) of message
 *
 * @return  result byte
 ***************************************************************************************************/
uint8_t Xor_CalcFCS( uint8_t *msg_ptr, uint8_t len )
{
  uint8_t x;
  uint8_t xorResult;

  xorResult = 0;

  for ( x = 0; x < len; x++, msg_ptr++ )
    xorResult = xorResult ^ *msg_ptr;

  return ( xorResult );
}

/***************************************************************************************************
 * @fn      SPIMgr_CalcFCS
 *
 * @brief   Calculate the FCS of a message buffer by XOR'ing each byte.
 *          Remember to NOT include SOP and FCS fields, so start at the CMD field.
 *
 * @param   byte *msg_ptr - message pointer
 * @param   byte len - length (in bytes) of message
 *
 * @return  result byte
 ***************************************************************************************************/
uint8_t Sum_CalcFCS( uint8_t *msg_ptr, uint8_t len )
{
	 uint32_t sum = 0;
	 uint8_t i, sumResult = 0;
	
	 for( i=0; i<len; i++ )
		 sum += msg_ptr[i];
	
   sumResult = (uint8_t )(sum&0x000000FF); 
	 
  return ( sumResult );
}

/*********************************************************** 
*   函数说明：CRC效验函数                                  * 
*   输入：    字节流，信息长度，校验和                     * 
*   输出：    计算以后的校验和                             * 
*   调用函数：CRC()                                        * 
***********************************************************/ 
uint16_t Get_CRC16_Check_Sum(uint8_t *pchMessage,uint32_t dwLength,uint16_t wCRC) 
{  
	  uint8_t chData ;
	
    if (pchMessage == NULL) 
    { 
        return 0xFFFF; 
    }     
     
    while(dwLength--) 
    { 
        chData = *pchMessage++; 
        Crcs(wCRC, chData);
    } 
    return wCRC; 
} 
                  
/*********************************************************** 
*   函数说明：对已经附加了CRC校验码的字符串进行有效性校验    * 
*   输入：    字节流，信息长度                             * 
*   输出：    校验是否正确                                 * 
*   调用函数：Get_CRC16_Check_Sum()                        * 
***********************************************************/ 
uint16_t Verify_CRC16_Check_Sum( uint8_t *pchMessage, uint32_t dwLength)
{ 
    uint16_t wExpected = 0; 
     
    if ((pchMessage == NULL) || (dwLength <= 2)) 
    { 
        return FALSE; 
    } 
    wExpected = Get_CRC16_Check_Sum \
                    ( \
                        (uint8_t *)pchMessage, \
                        dwLength - 2, \
                        CRC_INIT \
                    ); 
     
    return (wExpected & 0xff) == pchMessage[dwLength - 2] && 
           ((wExpected >> 8) & 0xff) == pchMessage[dwLength - 1]; 
}  

/*********************************************************** 
*   函数说明：向信息流后面附加校验信息                     * 
*   输入：    字节流，字节流实际长度+CRC校验字的长度       * 
*   输出：    校验后的字节流指针                          * 
*   调用函数：Get_CRC16_Check_Sum()                      * 
***********************************************************/ 
uint8_t* Append_CRC16_Check_Sum(uint8_t* pchMessage,uint32_t dwLength)
{  
    uint16_t wCRC = 0;
    if ((pchMessage == NULL) || (dwLength <= 2))
    { 
        return NULL;
    }
    wCRC = Get_CRC16_Check_Sum 
                ( 
                    (uint8_t *)pchMessage, 
                    dwLength-2, 
                    CRC_INIT 
                );
     
    pchMessage[dwLength-2] = (wCRC & 0x00ff);

    pchMessage[dwLength-1] = ((wCRC >> 8));

    return pchMessage;
}



