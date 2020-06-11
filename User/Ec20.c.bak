/* Includes ------------------------------------------------------------------*/
//#include "Ec20.h"
#include "AppTask.h"
#include "Variable.h"
#include "TimeEVent.h"
#include "usart123.h"
#include "gpio.h"
#include "Ec20.h"
#include "Global.h"
#include "rtc.h"
#include "net_device.h"
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
 NET_DEVICE_INFO netDeviceInfo;
 uint8_t connData[50];
 /*********************************************************************
 * @fn   void NET_DEVICE_Reset(void)
 *
 * @brief  网络设备复位
 *          
 * @param   NULL
 *
 * @return  NULL
*/
void NET_DEVICE_Reset(void)
{

    EC20PERST(ON);		//高电平复位
    HAL_Delay(200);

    EC20PERST(OFF);		//结束复位
    HAL_Delay(200);
	
}
 /*********************************************************************
 * @fn   void NET_DEVICE_SET_BaudRate(void)
 *
 * @brief  修改串口通信波特率
 *          
 * @param   NULL
 *
 * @return  NULL
*/
void NET_DEVICE_SET_BaudRate(void)
{
	NET_DEVICE_SendCmd("ATE0\r\n", "OK",2000,1);//关闭回显
//	HAL_Delay(50);
//	NET_DEVICE_SendCmd("AT+IPR=921600\r\n","OK",2000,1); 
//	Uart2Handle.Init.BaudRate = 921600;
//	HAL_UART_Init(&Uart2Handle);	
//	HAL_Delay(50);
	//NET_DEVICE_SendCmd("AT+QSIMDET=0,0\r\n", "OK", 1000, 10 );
	
	//NET_DEVICE_SendCmd("AT+QURCCFG=\"urcport\",\"uart1\"\r\n", "OK", 1000, 1); //URC报告从uart1串口输出
	NET_DEVICE_SendCmd("AT+QSIMSTAT=1\r\n", "OK", 1000, 1 ); //开启SIM卡状态报告
}

 /*********************************************************************
 * @fn     void NET_DEVICE_GetNwkInfor(void)
 *
 * @brief   获取设备SN, SIM卡序列号, 网络UTC时间等
 *          
 * @param   NULL
 *
 * @return  NULL
*/
void NET_DEVICE_GetDevAndNwkInfor(void)
{
	 NET_DEVICE_GetSN_IMSI_UTCTime_CSQ("AT+CGSN\r\n", DeviceInfor.TerminalSN, 500, 1 ); //获取设备序列号
	 NET_DEVICE_GetSN_IMSI_UTCTime_CSQ("AT+CIMI\r\n", DeviceInfor.IMSI, 500, 1 );       //获取sim卡序列号
	 //memcpy(DeviceInfor.TerminalSN,"868730037429498\0",15);
	 //memcpy(DeviceInfor.IMSI,"460072406002408\0",15);

	 //NET_DEVICE_GetSN_IMSI("AT+CNUM\r\n", (char*)DeviceInfor.CNUM, 500, 1 );          //获取电话号
	
	 memcpy(DeviceInfor.MCC, DeviceInfor.IMSI, 3);   // 移动国家码
	 //memcpy(DeviceInfor.MCC,(uint8_t *)"1cc\0", 3);   // 移动国家码
	 //memcpy(DeviceInfor.MNC, DeviceInfor.IMSI+3, 2); //通信网络类型
	 DeviceInfor.MNC[0]=0x30;
	 DeviceInfor.MNC[1]=0;
	
	 NET_DEVICE_SendCmd("AT+QICSGP=1,1,\"CMNET\",\"\",\"\",0\r\n", "OK",2000,1);//apn
	 NET_DEVICE_SendCmd("AT+QIDEACT=1\r\n", "OK",2000,1);  
	 NET_DEVICE_SendCmd("AT+QIACT=1\r\n",   "OK",2000,1);//激活场
	
	 NET_DEVICE_SendCmd("AT+QURCCFG=\"urcport\",\"uart1\"\r\n", "OK", 1000, 1); //URC报告从uart1串口输出
	 NET_DEVICE_SendCmd("AT+CREG=2\r\n", "OK",200,1); //配置读取小区信息 
	 NET_DEVICE_GetLac_Cellid("AT+CREG?\r\n", DeviceInfor.Lac, DeviceInfor.Cellid, 500, 1 );//读取主基站信息信息
	 NET_DEVICE_GetSN_IMSI_UTCTime_CSQ("AT+CSQ\r\n", &DeviceInfor.CSQ, 500, 1 );
	
   NET_DEVICE_GetSN_IMSI_UTCTime_CSQ("AT+CCLK?\r\n", (uint8_t *)&DeviceInfor.Secs,1000,1);//获取UTC时间

   //NET_DEVICE_SendCmd("AT+CREG=1\r\n", "OK",200,1);  //配置读取小区信息 
}

 /*********************************************************************
 * @fn    _Bool NET_DEVICE_Close(void)
 *
 * @brief  关闭网络连接
 *          
 * @param   NULL
 *
 * @return  0-成功	1-失败
*/
_Bool NET_DEVICE_Close( uint8_t connectid )
{
	 memset(connData, 0, 50);
	 sprintf((char *)connData,"AT+QICLOSE=%d\r\n", connectid );
	
   return NET_DEVICE_SendCmd((char *)connData, "OK",25000, 1); 
}

 /*********************************************************************
 * @fn    _Bool NET_DEVICE_ReLink(char *ip, char *port)
 *
 * @brief  重连平台
 *          
 * @param  ip：IP地址缓存指针
 *				 port：端口缓存指针
 *
 * @return  0-成功	1-失败
*/
_Bool NET_DEVICE_TCPIPLink(uint8_t connectid , uint8_t *ip,  uint16_t *port )//"bgm.sanquan.com"  43571, 42.228.5.151
{

	memset(connData, 0, 50);
	if( ip[4] )//字符串方式的IP
	{
    sprintf((char *)connData,"AT+QIOPEN=1,%d,\"TCP\",\"%s\",%d,0,1\r\n",connectid,(char *)ip, *port );
	}
	else
	{
		sprintf((char *)connData,"AT+QIOPEN=1,%d,\"TCP\",\"%d.%d.%d.%d\",%d,0,1\r\n",connectid,ip[0],ip[1],ip[2],ip[3], *port );
	}
	
	return NET_DEVICE_SendCmd((char *)connData, "OK",1000,1); 
}
 /*********************************************************************
 * @fn    NET_DEVICE_SendCmd(char *Command, char *Response, unsigned long Timeout, unsigned char Retry)
 *
 * @brief  EC20发送AT指令
 *          
 * @param   *Command :发送数据地址  *Response:应答匹配地址  Timeout:应答超时时间 Retry:重发次数
 *
 * @return  succes :??   Failure:??
 */
_Bool NET_DEVICE_SendCmd( char *Command, char *Response, uint32_t Timeout, uint8_t Retry )
{
  
  uint8_t n;
  
  for (n = 0; n < Retry; n++)
  {
    Timer_Event_Set( &timer_10msDB[1], Timeout, ENABLE, 0,  NULL ); //注册EC20 命令超时时间
    
    memset(Uart2Handle.pRxBuffPtr, 0x00, Uart2Handle.RxXferSize);
		U3_printf((uint8_t *)Command, 0, 0);
	  HAL_UART_Transmit_DMA(&Uart2Handle, (uint8_t *)Command, strlen(Command));
    
    do
    {
      if(strlen((char *)Uart2Handle.pRxBuffPtr) !=0 && isrCfg.rxTimeOut == 1 ) // 收到命令应答数据
      {
        isrCfg.rxTimeOut = 0 ;//清空接收完成标志
				U3_printf(Uart2Handle.pRxBuffPtr, 0, 0);
        Timer_Event_Set( &timer_10msDB[1], 0,  DISABLE, 0, NULL ); //清楚超时检测
				
        if ( strstr((char *)Uart2Handle.pRxBuffPtr, Response))//数据接收完整
				{
					memset(Uart2Handle.pRxBuffPtr, 0x00, Uart2Handle.RxXferSize);
          return 0;
				}
        else
				{ 
					memset(Uart2Handle.pRxBuffPtr, 0x00, Uart2Handle.RxXferSize);
          return 1;
				}
      }
    }while ( timer_10msDB[1].Flag == 0 );//超时时间完成
    
   Timer_Event_Set( &timer_10msDB[1], 0,  DISABLE, 0, NULL ); //??gprs????
  }
  return 1;
}


 /*********************************************************************
 * @fn    _Bool NET_DEVICE_DetectNwkCreg( char *Command, uint32_t Timeout, uint8_t Retry )
 *
 * @brief  EC20发送AT网络注册查询指令
 *          
 * @param   *Command :发送数据地址   Timeout:应答超时时间 Retry:重发次数
 *
 * @return  succes :0    Failure:1
 */
_Bool NET_DEVICE_DetectNwkCreg( char *Command, uint32_t Timeout, uint8_t Retry )
{
  uint8_t n;
  
  for (n = 0; n < Retry; n++)
  {
    Timer_Event_Set( &timer_10msDB[1], Timeout, ENABLE, 0,  NULL ); //注册EC20 命令超时时间
    
    memset(Uart2Handle.pRxBuffPtr, 0x00, Uart2Handle.RxXferSize);
		U3_printf((uint8_t *)Command, 0, 0);
	  HAL_UART_Transmit_DMA(&Uart2Handle, (uint8_t *)Command, strlen(Command));
    
    do
    {
      if(strlen((char *)Uart2Handle.pRxBuffPtr) !=0 && isrCfg.rxTimeOut == 1 ) // 收到命令应答数据
      {
        isrCfg.rxTimeOut = 0 ;//清空接收完成标志
				U3_printf(Uart2Handle.pRxBuffPtr, 0, 0);
        Timer_Event_Set( &timer_10msDB[1], 0,  DISABLE, 0, NULL ); //清楚超时检测
				
        if ( strstr((char *)Uart2Handle.pRxBuffPtr,"+CREG: 0,1" ) || \
					     strstr((char *)Uart2Handle.pRxBuffPtr,"+CREG: 2,1" ) || \
						     strstr((char *)Uart2Handle.pRxBuffPtr,"+CREG: 0,5" ) || \
						       strstr((char *)Uart2Handle.pRxBuffPtr,"+CREG: 2,5" ) )//数据接收完整
								{
									memset(Uart2Handle.pRxBuffPtr, 0x00, Uart2Handle.RxXferSize);
									return 0;
								}
								else
								{							
									memset(Uart2Handle.pRxBuffPtr, 0x00, Uart2Handle.RxXferSize);
									return 1;
								}
      }
    }while ( timer_10msDB[1].Flag == 0 );//超时时间完成
    
   Timer_Event_Set( &timer_10msDB[1], 0,  DISABLE, 0, NULL ); //??gprs????
  }
  return 1;
}
 /*********************************************************************
 * @fn      _Bool NET_DEVICE_GetSN_IMSI( char *Command, char *Response, uint32_t Timeout, uint8_t Retry )
 *
 * @brief   获取EC20终端序列号被imsi
 *          
 * @param   *Response:应答匹配地址  Timeout:应答超时时间 Retry:重发次数
 *
 * @return  succes :0   Failure:1
*/
_Bool NET_DEVICE_GetSN_IMSI_UTCTime_CSQ( char *Command, uint8_t *Response, uint32_t Timeout, uint8_t Retry )
{
  uint8_t n,len;
  char 	*ptr = NULL;
	uint32_t startadr, endadr;
	
  //HAL_Delay(50);
	
  for (n = 0; n < Retry; n++)
  {
    Timer_Event_Set( &timer_10msDB[1], Timeout, ENABLE, 0,  NULL ); //注册EC20 命令超时时间
    
    memset(Uart2Handle.pRxBuffPtr, 0x00, Uart2Handle.RxXferSize);
		U3_printf((uint8_t *)Command, 0, 0);
	  HAL_UART_Transmit_DMA(&Uart2Handle, (uint8_t *)Command, strlen(Command));
    
    do
    {
      if(strlen((char *)Uart2Handle.pRxBuffPtr) !=0 && isrCfg.rxTimeOut == 1 ) // 收到命令应答数据
      {
        isrCfg.rxTimeOut = 0 ;//清空接收完成标志
				U3_printf(Uart2Handle.pRxBuffPtr, 0, 0);
        if( strstr((char *)Uart2Handle.pRxBuffPtr,"+QCCID:") && strstr((char *)Uart2Handle.pRxBuffPtr, "OK"))
				{
					//\r\n+QCCID: 898604071118C0501150\r\nOK
					Timer_Event_Set( &timer_10msDB[1], 0,  DISABLE, 0, NULL ); //清除超时事件
					ptr = strchr((char *)Uart2Handle.pRxBuffPtr,' ');
					startadr = (uint32_t )ptr;
					endadr = (uint32_t )strchr(ptr+1,'\r');
					
					len = endadr - startadr-1;
					memcpy( Response, ptr+1, len );//简易方式
          
					memset(Uart2Handle.pRxBuffPtr, 0x00, Uart2Handle.RxXferSize);
					return 0;
				}
				
				if(strstr((char *)Uart2Handle.pRxBuffPtr,"+CNUM: ,\"") && strstr((char *)Uart2Handle.pRxBuffPtr, "OK"))//电话号码
				{
					Timer_Event_Set( &timer_10msDB[1], 0,  DISABLE, 0, NULL ); //清除超时事件
					
					ptr = strchr((char *)Uart2Handle.pRxBuffPtr,'"');
					startadr = (uint32_t )ptr;
					endadr = (uint32_t )strchr(ptr+1,'"');
					
					len = endadr - startadr-1;
					memcpy( Response, ptr+1, len ); //简易方式
					memset(Uart2Handle.pRxBuffPtr, 0x00, Uart2Handle.RxXferSize);
					return 0;
				}
				
				if( memcmp((char *)Uart2Handle.pRxBuffPtr,"\r\n+CCLK: \"", 10) == NULL && strstr((char *)Uart2Handle.pRxBuffPtr, "OK"))
				{
					Timer_Event_Set( &timer_10msDB[1], 0,  DISABLE, 0, NULL ); //清除超时事件
          startadr =  App_SysSetUtcTime( (char *)Uart2Handle.pRxBuffPtr );//配置当前时间
					memcpy( Response, (uint8_t *)&startadr, 4 ); 
					memset(Uart2Handle.pRxBuffPtr, 0x00, Uart2Handle.RxXferSize);
					return 0;
				}	
				 //"\r\n+CSQ: 31,99\r\n\r\nOK\r\n"
				if(memcmp((char *)Uart2Handle.pRxBuffPtr,"\r\n+CSQ: ", 8) == NULL && strstr((char *)Uart2Handle.pRxBuffPtr, "OK"))
				{
					Timer_Event_Set( &timer_10msDB[1], 0,  DISABLE, 0, NULL ); //清除超时事件
					
					ptr = strchr((char *)Uart2Handle.pRxBuffPtr,' ')+1;
	        *Response = strtoul( ptr, NULL, 10 );
					
					memset(Uart2Handle.pRxBuffPtr, 0x00, Uart2Handle.RxXferSize);
					return 0;
				}
				
				if( !memcmp((char *)Uart2Handle.pRxBuffPtr, "\r\n", 2) && !memcmp((char *)(Uart2Handle.pRxBuffPtr+17), "\r\n\r\nOK",6) )//数据接收完整
        {				
          Timer_Event_Set( &timer_10msDB[1], 0,  DISABLE, 0, NULL ); //清除超时事件
					
					ptr = strchr((char *)Uart2Handle.pRxBuffPtr,'\n');
					startadr = (uint32_t )ptr;
					endadr = (uint32_t )strchr(ptr+1,'\r');
					
					len = endadr - startadr-1;
					memcpy( Response, ptr+1, len );//简易方式
          memset(Uart2Handle.pRxBuffPtr, 0x00, Uart2Handle.RxXferSize);
					return 0;
        }
				

				memset(Uart2Handle.pRxBuffPtr, 0x00, Uart2Handle.RxXferSize);
				return 1;
      }
    }while ( timer_10msDB[1].Flag == 0 );//超时时间完成
    
   Timer_Event_Set( &timer_10msDB[1], 0,  DISABLE, 0, NULL ); //??gprs????
  }
  return 1;
}
 /*********************************************************************
 * @fn      NET_DEVICE_GetLac_Cellid( char *Command, uint8_t *lac, uint8_t *cellid,uint32_t Timeout, uint8_t Retry )
 *
 * @brief   获lac 和cellid
 *          
 * @param   *lac:lac数据  cellid：cellid数据  Timeout:应答超时时间 Retry:重发次数
 *
 * @return  succes :0   Failure:1
*/  
_Bool NET_DEVICE_GetLac_Cellid( char *Command, uint8_t *lac, uint8_t *cellid, uint32_t Timeout, uint8_t Retry )
{
  
  uint8_t n,len;
	char 	*ptr = NULL;
	uint32_t startadr, endadr;

  //HAL_Delay(50);
	
  for (n = 0; n < Retry; n++)
  {
    Timer_Event_Set( &timer_10msDB[1], Timeout, ENABLE, 0,  NULL ); //注册EC20 命令超时时间
    
    memset(Uart2Handle.pRxBuffPtr, 0x00, Uart2Handle.RxXferSize);
		U3_printf((uint8_t *)Command, 0, 0);
	  HAL_UART_Transmit_DMA(&Uart2Handle, (uint8_t *)Command, strlen(Command));
    
    do
    {
      if(strlen((char *)Uart2Handle.pRxBuffPtr) !=0 && isrCfg.rxTimeOut == 1 ) // 收到命令应答数据
      {
        isrCfg.rxTimeOut = 0 ;//清空接收完成标志
        U3_printf(Uart2Handle.pRxBuffPtr, 0, 0);
				if( memcmp((char *)Uart2Handle.pRxBuffPtr,"\r\n+CREG:", 8) == NULL && strstr((char *)Uart2Handle.pRxBuffPtr, "OK"))
				{
					Timer_Event_Set( &timer_10msDB[1], 0,  DISABLE, 0, NULL ); //清除超时事件
					
					//  +CREG: 2,1,"676F","6318D05",7
					ptr = strchr((char *)Uart2Handle.pRxBuffPtr,'"');
					startadr = (uint32_t )ptr;
					endadr = (uint32_t )strchr(ptr+1,'"');
					len = endadr - startadr-1;
					memcpy( lac, ptr+1, len ); //简易方式
					
					ptr = strchr((char *)(endadr+1),'"');
					startadr = (uint32_t )ptr;
					endadr = (uint32_t )strchr(ptr+1,'"');
					len = endadr - startadr-1;
					memcpy( cellid, ptr+1, len ); //简易方式
					memset(Uart2Handle.pRxBuffPtr, 0x00, Uart2Handle.RxXferSize);
					return 0;
				}
				else
				{
					memset(Uart2Handle.pRxBuffPtr, 0x00, Uart2Handle.RxXferSize);
					return 1;
				}
      }
    }while ( timer_10msDB[1].Flag == 0 );//超时时间完成
    
   Timer_Event_Set( &timer_10msDB[1], 0,  DISABLE, 0, NULL ); //??gprs????
  }
  return 1;
}
/**************************************************************************************************
 * @fn      NET_DEVICE_SendData
 *
 * @brief   使网络设备发送数据到平台
 *
 * @param   data：需要发送的数据
 *		      len： 数据长度
 *
 * @return  None
 **************************************************************************************************/
_Bool NET_DEVICE_SendData(uint8_t connectid, uint8_t *data, uint16_t len)
{
    char cmdBuf[24];

    //HAL_Delay(50);									//等待一下

    sprintf(cmdBuf, "AT+QISEND=%d,%d\r\n", connectid, len);		//发送命令
    if(!NET_DEVICE_SendCmd(cmdBuf, ">", 1000, 1))	//收到‘>’时可以发送数据
    {
			U3_printf(data, len, 1);
  	  U3_printf((uint8_t *)"\r\n", 0, 0);
			
			Timer_Event_Set( &timer_10msDB[1], 1000, ENABLE, 0,  NULL ); //注册EC20 命令超时时间
      memset(Uart2Handle.pRxBuffPtr, 0x00, Uart2Handle.RxXferSize);
	    HAL_UART_Transmit_DMA(&Uart2Handle, data, len );
			do
			{
				if(strlen((char *)Uart2Handle.pRxBuffPtr) !=0 && isrCfg.rxTimeOut == 1 ) // 收到命令应答数据
				{
					
					isrCfg.rxTimeOut = 0 ;//清空接收完成标志
					U3_printf(Uart2Handle.pRxBuffPtr, 0, 0);
					if ( strstr((char *)Uart2Handle.pRxBuffPtr, "SEND OK"))//数据接收完整
					{				
						memset(Uart2Handle.pRxBuffPtr, 0x00, Uart2Handle.RxXferSize);		
						Timer_Event_Set( &timer_10msDB[1], 0,  DISABLE, 0, NULL );				
						return 0;
					}
					memset(Uart2Handle.pRxBuffPtr, 0x00, Uart2Handle.RxXferSize);
					return 1;
				}
			}while ( timer_10msDB[1].Flag == 0 );//超时时间完成
			
		 Timer_Event_Set( &timer_10msDB[1], 0,  DISABLE, 0, NULL ); //
    }
    return 1;		
}

/**************************************************************************************************
 * @fn      void  EC20_Init(void)
 *
 * @brief   EC20模块配置初始化
 *
 * @param   null   
 *
 * @return  null
 **************************************************************************************************/
void EC20_Init(void)
{
    //uint8_t i =0;
	
    NET_DEVICE_Reset();
    HAL_Delay(1000);
    while(NET_DEVICE_SendCmd("AT\r\n", "OK",2000,1));                                    //等待模块的启动
    HAL_Delay(50);
    NET_DEVICE_SendCmd("ATE0\r\n", "OK",2000,1);                                         //关闭回显
    HAL_Delay(50);
	  NET_DEVICE_SendCmd("AT+IPR=921600\r\n","OK",2000,1); 
	  Uart2Handle.Init.BaudRate = 921600;
	  HAL_UART_Init(&Uart2Handle);	
	  HAL_Delay(50);
   	NET_DEVICE_SendCmd("AT+QSIMSTAT=1\r\n", "OK", 1000, 10 ); //开启SIM卡状态报告
	
	  //NET_DEVICE_SendCmd("AT+QSIMSTAT?\r\n", "+QSIMSTAT: 1,1", 1000, 10 ); //存在sim卡
    //NET_DEVICE_SendCmd("AT+CREG?\r\n", "+CREG: 0,1", 1000, 10 );  
	
    NET_DEVICE_SendCmd("AT+QICLOSE=0\r\n", "OK",20000,1);  
    NET_DEVICE_SendCmd("AT+QICSGP=1,1,\"CMNET\",\"\",\"\",0\r\n", "OK",2000,1);  
	
		NET_DEVICE_SendCmd("AT+CREG=2\r\n", "OK",2000,1);//配置读取小区信息
    NET_DEVICE_GetLac_Cellid("AT+CREG?\r\n", DeviceInfor.Lac, DeviceInfor.Cellid, 1000, 1 );//配置读取基站信息信息
		
  	NET_DEVICE_GetSN_IMSI_UTCTime_CSQ("AT+CGSN\r\n", DeviceInfor.TerminalSN, 1000, 1 );
	  NET_DEVICE_GetSN_IMSI_UTCTime_CSQ("AT+CIMI\r\n", DeviceInfor.IMSI, 1000, 1 );
	  //NET_DEVICE_GetSN_IMSI("AT+CNUM\r\n", (char*)DeviceInfor.CNUM, 1000, 1 );
		
    NET_DEVICE_SendCmd("AT+QIDEACT=1\r\n", "OK",2000,1);  
    NET_DEVICE_SendCmd("AT+QIACT=1\r\n",   "OK",2000,1);  

//    NET_DEVICE_SendCmd("AT+QGPS=1\r\n", "OK",2000,1);                                  //开启GPS
//    HAL_Delay(250);
//		NET_DEVICE_SendCmd("AT+QCELLLOC\r\n", "+",2000,1);
//		HAL_Delay(250);
//		NET_DEVICE_SendCmd("AT+QGPSLOC?\r\n", "+",2000,1);
//		HAL_Delay(250);
		  NET_DEVICE_GetSN_IMSI_UTCTime_CSQ("AT+CCLK?\r\n", (uint8_t *)&DeviceInfor.Secs,1000,1);
			
		  NET_DEVICE_SendCmd("AT+QIOPEN=1,0,\"TCP\",\"115.28.214.192\",9055,0,1\r\n","+QIOPEN: 0,0",2000,1);		//重新连接
		  NET_DEVICE_SendData(0,(uint8_t *)"ESEC20", 6);
			
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
void Uart2_BuildAndSendMsgToEC20( uint8_t connectid,uint8_t flag ,uint8_t cmdtype, uint8_t *pData, uint8_t pDataLen ,uint8_t sn )
{
  uint8_t *msg_ptr = NULL ,*pBuf = NULL;
  uint16_t msglen = pDataLen;

	(void)sn;

  memset(Uart2Handle.pTxBuffPtr, 0x00, Uart2Handle.TxXferSize );
	pBuf = Uart2Handle.pTxBuffPtr;
		
  if( pBuf )
  {
     msg_ptr = pBuf;
    *pBuf++ = PV;  
    *pBuf++ = flag;
    *pBuf++ = HI_UINT16(pDataLen+1);
		*pBuf++ = LO_UINT16(pDataLen+1);
		*pBuf++ = cmdtype;
     memcpy( pBuf, pData, pDataLen );
		 
		 msglen =  pDataLen +5;
     NET_DEVICE_SendData(connectid, msg_ptr, msglen);
	}
}



