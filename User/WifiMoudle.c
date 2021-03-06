/* Includes ------------------------------------------------------------------*/
//#include "Ec20.h"
#include "AppTask.h"
#include "Variable.h"
#include "TimeEVent.h"
#include "usart123.h"
#include "gpio.h"
#include "wifimoudle.h"
#include "Global.h"
//#include "net_device.h"
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
 
 /*********************************************************************
 * EXTERNAL VARIABLES
 */
 
/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

nearMac_t nearMac[NearMacListNum];
 /*********************************************************************
 * @fn   void NET_DEVICE_Reset(void)
 *
 * @brief  网络设备复位
 *          
 * @param   NULL
 *
 * @return  NULL
 */
void WIFI_DEVICE_Reset(void)
{
    WIFIPOW_EN(OFF);
    HAL_Delay(500);

    WIFIPOW_EN(ON);		//结束复位
}
 



 /*********************************************************************
 * @fn      WIFI_DEVICE_SendCmd(char *Command, char *Response, unsigned long Timeout, unsigned char Retry)
 *
 * @brief   WIFI发送AT指令
 *          
 * @param   *Command :发送数据地址  *Response:应答匹配地址  Timeout:应答超时时间 Retry:重发次数
 *
 * @return  succes :??   Failure:??
 */
_Bool WIFI_DEVICE_SendCmd( char *Command, char *Response, uint32_t Timeout, uint8_t Retry )
{
  
  uint8_t n;
  
  for (n = 0; n < Retry; n++)
  {
     Timer_Event_Set( &timer_10msDB[WifiAtTimeout_Task], Timeout, ENABLE, 0,  NULL ); 
     
		 U3_printf((uint8_t *)Command, 0, 0);
     
		 memset(Uart5Handle.pRxBuffPtr, 0x00, Uart5Handle.RxXferSize);
	   HAL_UART_Transmit(&Uart5Handle, (uint8_t *)Command, strlen(Command),5000);
    
    do
    {
      if(strlen((char *)Uart5Handle.pRxBuffPtr) !=0 && Uart5Handle.RxXferCount == 0xFFFF ) // 收到命令应答数据
      {
        Uart5Handle.RxXferCount = 0 ;//清空接收完成标志
				
				U3_printf(Uart5Handle.pRxBuffPtr, 0, 0);
				
        Timer_Event_Set( &timer_10msDB[WifiAtTimeout_Task], 0,  DISABLE, 0, NULL ); //清楚超时检测
        if ( strstr((char *)Uart5Handle.pRxBuffPtr, Response))//数据接收完整
				{
					memset(Uart5Handle.pRxBuffPtr, 0x00, Uart5Handle.RxXferSize);
          return 0;
				}
				memset(Uart5Handle.pRxBuffPtr, 0x00, Uart5Handle.RxXferSize);
        return 1;
				
      }
    }while ( timer_10msDB[WifiAtTimeout_Task].Flag == 0 );//超时时间完成
    
   Timer_Event_Set( &timer_10msDB[WifiAtTimeout_Task], 0,  DISABLE, 0, NULL ); //??gprs????
  }
  return 1;
}




/**************************************************************************************************
 * @fn      WIFI_DEVICE_SendData
 *
 * @brief   使网络设备发送数据到平台
 *
 * @param   data：需要发送的数据
 *		      len： 数据长度
 *
 * @return  None
 **************************************************************************************************/
_Bool WIFI_DEVICE_SendData(uint8_t connectid, uint8_t *data, uint16_t len)
{
    char cmdBuf[24];

    sprintf(cmdBuf, "AT+CIPSEND=%d,%d\r\n", connectid, len);		//发送命令
    if(!WIFI_DEVICE_SendCmd(cmdBuf, "OK\r\n>", 1000, 1))	//收到‘>’时可以发送数据
    {
			U3_printf(data, len, 1);
  	  U3_printf((uint8_t *)"\r\n", 0, 0);
			
			Timer_Event_Set( &timer_10msDB[WifiAtTimeout_Task], 1000, ENABLE, 0,  NULL ); 
      memset(Uart5Handle.pRxBuffPtr, 0x00, Uart5Handle.RxXferSize);
	    HAL_UART_Transmit(&Uart5Handle, data, len ,0xFFFFFFFFU);//HAL_UART_Transmit_DMA
			do
			{
				if(strlen((char *)Uart5Handle.pRxBuffPtr) !=0 && Uart5Handle.RxXferCount == 0xFFFF ) // 收到命令应答数据
				{
					Uart5Handle.RxXferCount = 0 ;//清空接收完成标志
					U3_printf(Uart5Handle.pRxBuffPtr, 0, 0);
					if ( strstr((char *)Uart5Handle.pRxBuffPtr, "Recv "))//等待接收Recv
					{					
						Timer_Event_Set( &timer_10msDB[WifiAtTimeout_Task], 1000, ENABLE, 0,  NULL ); 
						memset(Uart5Handle.pRxBuffPtr, 0x00, Uart5Handle.RxXferSize);	
						do
						{
							if(strlen((char *)Uart5Handle.pRxBuffPtr) !=0 && Uart5Handle.RxXferCount == 0xFFFF ) //等待接收SEND OK 
							{
								Uart5Handle.RxXferCount = 0 ;//清空接收完成标志
								U3_printf(Uart5Handle.pRxBuffPtr, 0, 0);
								Timer_Event_Set( &timer_10msDB[WifiAtTimeout_Task], 0,  DISABLE, 0, NULL );	
								if ( strstr((char *)Uart5Handle.pRxBuffPtr, "SEND OK"))//数据接收完整
								{				
									memset(Uart5Handle.pRxBuffPtr, 0x00, Uart5Handle.RxXferSize);			
									return 0;
								}
								memset(Uart5Handle.pRxBuffPtr, 0x00, Uart5Handle.RxXferSize);
								return 1;
							}	
						}
						while ( timer_10msDB[WifiAtTimeout_Task].Flag == 0 );//超时时间完成
						memset(Uart5Handle.pRxBuffPtr, 0x00, Uart5Handle.RxXferSize);
						Timer_Event_Set( &timer_10msDB[WifiAtTimeout_Task], 0,  DISABLE, 0, NULL ); //
						return 1;
					}
					memset(Uart5Handle.pRxBuffPtr, 0x00, Uart5Handle.RxXferSize);
					Timer_Event_Set( &timer_10msDB[WifiAtTimeout_Task], 0,  DISABLE, 0, NULL ); //
					return 1;
				}
			}while ( timer_10msDB[WifiAtTimeout_Task].Flag == 0 );//超时时间完成	
		 Timer_Event_Set( &timer_10msDB[WifiAtTimeout_Task], 0,  DISABLE, 0, NULL ); //
    }
    return 1;		
}
/*********************************************************************
 * @fn      WIFI_DEVICE_SendPing(char *Command, unsigned long Timeout, unsigned char Retry)
 *
 * @brief   WIFI发送AT指令
 *          
 * @param   *Command :发送数据地址    Timeout:应答超时时间 Retry:重发次数
 *
 * @return  succes :??   Failure:??
 */
_Bool WIFI_DEVICE_SendPing( char *Command, uint32_t Timeout, uint8_t Retry )
{
  
  uint8_t n,resultCode;
	
	char *ptr = NULL;
  
  for (n = 0; n < Retry; n++)
  {
     Timer_Event_Set( &timer_10msDB[WifiAtTimeout_Task], Timeout, ENABLE, 0,  NULL ); 
     
		 U3_printf((uint8_t *)Command, 0, 0);
     
		 memset(Uart5Handle.pRxBuffPtr, 0x00, Uart5Handle.RxXferSize);
	   HAL_UART_Transmit(&Uart5Handle, (uint8_t *)Command, strlen(Command),5000);
    
    do
    {
      if(strlen((char *)Uart5Handle.pRxBuffPtr) !=0 && Uart5Handle.RxXferCount == 0xFFFF ) // 收到命令应答数据
      {
        Uart5Handle.RxXferCount = 0 ;//清空接收完成标志
				
				U3_printf(Uart5Handle.pRxBuffPtr, 0, 0);
				
        Timer_Event_Set( &timer_10msDB[WifiAtTimeout_Task], 0,  DISABLE, 0, NULL ); //清楚超时检测
        if ( strstr((char *)Uart5Handle.pRxBuffPtr, "+"))//数据接收完整
				{
					ptr = (char *)Uart5Handle.pRxBuffPtr;
					
					if(strstr( ptr, "+timeout\r\n\r\nERROR"))
					{
						memset(Uart5Handle.pRxBuffPtr, 0x00, Uart5Handle.RxXferSize);
            return 1;
					}
					else
					{
						ptr = strchr( ptr, '+')+1;
		        resultCode = strtoul( ptr, NULL, 10 );
						memset(Uart5Handle.pRxBuffPtr, 0x00, Uart5Handle.RxXferSize);
						if( resultCode )
						{
							return 0;
						}
						else
						{
							return 1;
						}
					}
				}
				memset(Uart5Handle.pRxBuffPtr, 0x00, Uart5Handle.RxXferSize);
        return 1;
      }
    }while ( timer_10msDB[WifiAtTimeout_Task].Flag == 0 );//超时时间完成
    
   Timer_Event_Set( &timer_10msDB[WifiAtTimeout_Task], 0,  DISABLE, 0, NULL ); //??gprs????
  }
  return 1;
}

 /*********************************************************************
 * @fn      _Bool WIFI_DEVICE_GetMac( char *Command, char *Response, uint32_t Timeout, uint8_t Retry )
 *
 * @brief   获取WIFI模块的mac地址
 *          
 * @param   *Response:应答匹配地址  Timeout:应答超时时间 Retry:重发次数
 *
 * @return  succes :0   Failure:1
*/
_Bool WIFI_DEVICE_GetMac( char *Command, uint8_t *Response, uint32_t Timeout, uint8_t Retry )
{
  uint8_t n;
  char 	*ptr = NULL;

  for (n = 0; n < Retry; n++)
  {
     Timer_Event_Set( &timer_10msDB[WifiAtTimeout_Task], Timeout, ENABLE, 0,  NULL ); //开启超时检测
		
     U3_printf((uint8_t *)Command, 0, 0);
		
     memset(Uart5Handle.pRxBuffPtr, 0x00, Uart5Handle.RxXferSize);
		 HAL_UART_Transmit(&Uart5Handle, (uint8_t *)Command, strlen(Command),5000);
    
    do
    {
      if(strlen((char *)Uart5Handle.pRxBuffPtr) !=0 && Uart5Handle.RxXferCount  == 0xFFFF ) // 收到命令应答数据
      {
        Uart5Handle.RxXferCount = 0 ;//清空接收完成标志
				U3_printf(Uart5Handle.pRxBuffPtr, 0, 0);
        if( strstr((char *)Uart5Handle.pRxBuffPtr,"+CIFSR:STAMAC") && strstr((char *)Uart5Handle.pRxBuffPtr, "OK"))
				{
					//+CIFSR:APIP,"192.168.4.1"
					//+CIFSR:APMAC,"62:01:94:8d:26:41"
					//+CIFSR:STAIP,"0.0.0.0"
					//+CIFSR:STAMAC,"60:01:94:8d:26:41"
					Timer_Event_Set( &timer_10msDB[WifiAtTimeout_Task], 0,  DISABLE, 0, NULL ); //清除超时事件
					ptr = strstr((char *)Uart5Handle.pRxBuffPtr,"+CIFSR:APMAC");
		
					MacStringToHex( ptr+15,  Response);//获取本机MAC地址
					
					memset(Uart5Handle.pRxBuffPtr, 0x00, Uart5Handle.RxXferSize);
					return 0;
				}

			  memset(Uart5Handle.pRxBuffPtr, 0x00, Uart5Handle.RxXferSize);
		    return 1;
			}
		}while ( timer_10msDB[WifiAtTimeout_Task].Flag == 0 );//超时时间完成
    Timer_Event_Set( &timer_10msDB[WifiAtTimeout_Task], 0,  DISABLE, 0, NULL ); //??gprs????
	}
  return 1;
}
/*********************************************************************
 * @fn      _Bool WIFI_DEVICE_ScanAPhocList( char *Command, uint8_t *Response, uint32_t Timeout, uint8_t Retry )
 *
 * @brief   获取周边热点信息
 *          
 * @param   *nearMac:应答回复数据  Timeout:应答超时时间  Retry:重发次数
 *
 * @return  succes :0   Failure:1
*/
_Bool WIFI_DEVICE_ScanAPhocList( char *Command, nearMac_t *nearmac, uint32_t Timeout, uint8_t Retry )
{
  uint8_t n,i,len;
	uint32_t startadr, endadr;
	
  char 	*ptr = NULL;

  for (n = 0; n < Retry; n++)
  {
     Timer_Event_Set( &timer_10msDB[WifiAtTimeout_Task], Timeout, ENABLE, 0,  NULL ); //开启超时检测
		
     U3_printf((uint8_t *)Command, 0, 0);
		
     memset(Uart5Handle.pRxBuffPtr, 0x00, Uart5Handle.RxXferSize);
		 HAL_UART_Transmit(&Uart5Handle, (uint8_t *)Command, strlen(Command),5000);
    
    do
    {
      if(strlen((char *)Uart5Handle.pRxBuffPtr) !=0 && Uart5Handle.RxXferCount  == 0xFFFF ) // 收到命令应答数据
      {
        Uart5Handle.RxXferCount = 0 ;//清空接收完成标志
				U3_printf(Uart5Handle.pRxBuffPtr, 0, 0);
				
				if( strstr((char *)Uart5Handle.pRxBuffPtr, "+CWLAP:") && strstr((char *)Uart5Handle.pRxBuffPtr, "OK"))// 收到远程数据//+QIURC: "recv",6,13\r\nReceive  DATA
				{
					memset(nearMac, 0x00, sizeof(nearMac_t)*NearMacListNum);
					ptr = (char *)Uart5Handle.pRxBuffPtr;
					for( i=0; i<NearMacListNum; i++ )
					{
						ptr = strstr( ptr, "+CWLAP:");
						ptr = strchr( ptr, '"');
						startadr = (uint32_t )ptr;
						ptr = strchr(ptr+1,'"');
						endadr = (uint32_t )ptr;
						len = endadr - startadr-1;
						if( len >= SSIDLenMax)//SSID长度过大，不需要
							continue;
						
						nearmac[i].En = 1;
						nearmac[i].nearSSIDLen = len;
						memcpy( nearmac[i].nearSSID, (char *)(startadr+1), nearmac[i].nearSSIDLen );//ssid

						ptr = strchr( ptr, '-' )+1;
						nearmac[i].nearSignal =  0 - strtoul( ptr, NULL, 10 );//rssi
						
						ptr = strchr( ptr,'"');
						MacStringToHex( ptr+1, nearmac[i].nearMac );//mac
						
						if((strstr( ptr, ")")+3) == strstr( ptr, "OK"))
							break;
					}
				}
			  memset(Uart5Handle.pRxBuffPtr, 0x00, Uart5Handle.RxXferSize);
		    return 1;
			}
		}while ( timer_10msDB[WifiAtTimeout_Task].Flag == 0 );//超时时间完成
    Timer_Event_Set( &timer_10msDB[WifiAtTimeout_Task], 0,  DISABLE, 0, NULL ); //??gprs????
	}
  return 1;
}
 /*********************************************************************
 * @fn      void MacStringToHex( char *str, uint8_t *outMac)
 *
 * @brief   mac地址转hex地址
 *          
 * @param  char *str       目标数据
           uint8_t *outMac 输出缓冲
 *
 * @return  none
*/
void MacStringToHex( char *str, uint8_t *outMac)
{
	//"62:01:94:8d:26:41"
    char *p = str;
    char high = 0, low = 0;
    uint8_t cnt = 0;
    while(cnt <6 )
    {
       high = ((*p > '9') && ((*p <= 'F') || (*p <= 'f'))) ? *p - 48 - 7 : *p - 48;
		   low = (*(++ p) > '9' && ((*p <= 'F') || (*p <= 'f'))) ? *(p) - 48 - 7 : *(p) - 48;
       outMac[cnt] = ((high & 0x0f) << 4 | (low & 0x0f));
       p += 2;
       cnt ++;
    }
}

/**************************************************************************************************
 * @fn      void  WifiMoudle_Init(void)
 *
 * @brief   WIFI模块配置初始化
 *
 * @param   null   
 *
 * @return  null
 **************************************************************************************************/
void WifiMoudle_Init(void)
{
	 memset(Uart5Handle.pRxBuffPtr, 0x00, Uart5Handle.RxXferSize);
	 Uart5Handle.RxXferCount = 0;
	
	 U3_printf((uint8_t *)"Wifi moulde Init\r\n", 0, 0);
	
   WIFI_DEVICE_SendCmd("ATE0\r\n", "OK",500,1);
   //WIFI_DEVICE_SendCmd("AT+UART=115200,8,1,0,0\r\n", "OK",500,1);    	
	 WIFI_DEVICE_SendCmd("AT+CWMODE=1\r\n", "OK", 500, 1);//sta模式
	// WIFI_DEVICE_GetMac( "AT+CIFSR\r\n", DeviceInfor.Mac, 500, 1);
	 //WIFI_DEVICE_ScanAPhocList( "AT+CWLAP\r\n", nearMac, 10000, 1);
}






/**************************************************************************************************
 * @fn      void ReceiveWifiATResponseData_Handle(void)
 *
 * @brief   接收WIFI数据处理
 *
 * @param   null   
 *
 * @return  null
 **************************************************************************************************/
//void ReceiveWifiATResponseData_Handle(void)
//{
//	char *ptr = NULL ;
//	
//	uint8_t i,len;
//	uint32_t startadr, endadr;
//	
//	if( Uart5Handle.RxXferCount == 0xFFFF )
//	{		
//		Uart5Handle.RxXferCount = 0;
//		printf((char *)Uart5Handle.pRxBuffPtr);
//		
//		
//		
//	 //接收远程数据处理
//	  if( strstr((char *)Uart5Handle.pRxBuffPtr, "+IPD,"))// 收到远程数据//+QIURC: "recv",6,13\r\nReceive  DATA
//		{ 
//			

//   
//			memset(Uart5Handle.pRxBuffPtr, 0x00, Uart5Handle.RxXferSize);
//		  return;
//		}
//		
//		
//		
//		//+CWLAP:(4,"GNYX",-64,"dc:fe:18:fa:85:c0",1,-17,0)
//		//+CWLAP:(3,"XC0902409116900A10050996",-81,"48:98:ca:e9:3c:54",1,-7,0)
//		//+CWLAP:(4,"ZNSHJ",-66,"48:7d:2e:32:ee:37",6,-4,0)
//		//+CWLAP:(3,"HUAWEI-B23C",-59,"00:be:3b:39:b2:3c",6,5,0)
//		//+CWLAP:(3,"DIRECT-DxDESKTOP-22KCSFImsVO",-66,"7e:76:35:de:10:89",6,32767,0)
//		//+CWLAP:(3,"HUAWEI-B315-1",-58,"ec:89:14:1c:1e:16",11,-7,0)
//		//+CWLAP:(3,"HUAWEI-C663",-81,"6c:b7:49:2f:c6:63",11,-7,0)
//		//+CWLAP:(4,"TP-LINK_6B6D",-79,"8c:a6:df:58:6b:6d",11,0,0)

//		//OK
//		
////		if( strstr((char *)Uart5Handle.pRxBuffPtr, "+CWLAP:") && strstr((char *)Uart5Handle.pRxBuffPtr, "OK"))// 收到远程数据//+QIURC: "recv",6,13\r\nReceive  DATA
////		{
////			
////			memset(nearMac, 0x00, sizeof(nearMac_t));
////			ptr = (char *)Uart5Handle.pRxBuffPtr;
////			for( i=0; i<10; i++ )
////			{
////				ptr = strstr( ptr, "+CWLAP:");
////				ptr = strchr( ptr, '"');
////				startadr = (uint32_t )ptr;
////				ptr = strchr(ptr+1,'"');
////				endadr = (uint32_t )ptr;
////				len = endadr - startadr-1;
////				if( len > 30)//SSID长度过大，不需要
////					continue;
////				
////			  nearMac[i].En = 1;
////				nearMac[i].nearSSIDLen = len;
////				memcpy( nearMac[i].nearSSID, (char *)(startadr+1), nearMac[i].nearSSIDLen );//ssid

////				ptr = strchr( ptr, '-' )+1;
////				nearMac[i].nearSignal =  0 - strtoul( ptr, NULL, 10 );//rssi
////				
////				ptr = strchr( ptr,'"');
////				MacStringToHex( ptr+1, nearMac[i].nearMac );//mac
////        
////				if((strstr( ptr, ")")+3) == strstr( ptr, "OK"))
////					break;
////			}
////		}
//		memset(Uart5Handle.pRxBuffPtr, 0x00, Uart5Handle.RxXferSize);
//		return;	
//	}
//}
