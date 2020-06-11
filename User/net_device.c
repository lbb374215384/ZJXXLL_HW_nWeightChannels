/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
#include "Global.h"
#include "usart123.h"
#include "Variable.h"
#include "gpio.h"
#include "Ec20.h"
#include "rtc.h"
#include "net_device.h"
//#include "net_image.h"
#include "TimeEvent.h"
#include "wifimoudle.h"
#include "md5.h"
#include "spi_flash.h"
#include "CameraMoudle.h"
#include "Nv_eeprom.h"
/*********************************************************************
 * MACROS
 */
/*********************************************************************
 * TYPEDEFS
 */

#define FrameHeadSize   10
#define SnTlenSNSize  (3+15)
#define cmdptr  28
/*********************************************************************
 * CONSTANTS
 */

/**
|  PV  |	Flag |  Date |设备类型|厂家代码|	Len   [ 协议序列号|	Tlen |	TerminalSN | Data ]
|  1   |    1  |   4   |    1   |   1    |   2    |       2   |   1  |    X(15)    |   N  |
**/

/*********************************************************************
 * GLOBAL VARIABLES
 */
// #define nbtsMaclistNum    10
// nearbtsMac_t nearbtsMac[nbtsMaclistNum];



netDeviceInforList_t netDeviceInfo;



 char connData[50];


 uint8_t PingCount = 0;
 uint8_t ConnectCount = 0;
 uint8_t SendErrCount = 0;
// uint8_t ImgErrCount = 0;
 
 uint8_t ServiceState = OFFLINE;

 devStates_t devState = DEV_RST;

 
 uint8_t FrameIntervalFlag =0 ;
 uint8_t NetDevSendFlag = 0;
 
 
// uint8_t  revConnectId = 0;
// uint16_t revDatalen=0;
// uint16_t PackageNum = 0; 
// uint16_t PackageCount = 0;
// uint16_t RemainSize = 0;
// uint32_t startPtr = 0,RevAdr = 0;
// uint32_t endPtr = 0;

/******************************************************************************************
										消息队列
******************************************************************************************/

/***************************************************************************************************
 * @fn      _Bool NET_DEVICE_CheckListHead(void)
 *
 * @brief   检查发送链表头是否为空
 *
 * @param   NULL
 *
 * @return  0-空	1-不为空
 ***************************************************************************************************/
_Bool NET_DEVICE_CheckListHead( void )
{
    if( netDeviceInfo.head == NULL )
        return 0;
    else
        return 1;
}

/***************************************************************************************************
 * @fn      uint8_t *NET_DEVICE_GetListHeadBuf(void)
 *
 * @brief   获取链表里需要发送的数据指针
 *
 * @param   NULL
 *
 * @return  获取链表里需要发送的数据指针
 ***************************************************************************************************/
uint8_t *NET_DEVICE_GetListHeadBuf(void)
{

  return netDeviceInfo.head->buf;
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
uint16_t NET_DEVICE_GetListHeadLen( void )
{
    return netDeviceInfo.head->bufLen;
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
uint8_t NET_DEVICE_AddDataSendList( uint8_t connId, uint8_t *buf , uint16_t dataLen , uint8_t sendcount , uint16_t protocolsn)
{
   netMsgList_t *current = NULL;
	
	 current = (netMsgList_t *)malloc(sizeof(netMsgList_t));//分配内存
   
   if( current == NULL )
	 {
		 free(buf); 
		 return 1;
   }
     
   if( netDeviceInfo.head == NULL )							//如果head为NULL
	 { 
     netDeviceInfo.head = current;							//head指向当前分配的内存区
		 FrameIntervalFlag = 1;//触发事件读取任务
	 }
   else										//如果head不为NULL
     netDeviceInfo.end->next = current;				//则end指向当前分配的内存区

    //memcpy(current->buf, buf, dataLen);						//复制数据
	  current->connectid = connId;
		current->buf = buf;//复制数据
    current->bufLen = dataLen;
		current->sn = protocolsn;
    current->next = NULL;								//下一段为NULL
    current->reSendCount = sendcount;
	  current->flag = sendcount;          //0：回复帧   1：主动上发帧
    netDeviceInfo.end = current;        //end指向当前分配的内存区
		
   // FrameIntervalFlag = 1;
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
_Bool NET_DEVICE_DeleteDataSendList(void)
{
    netMsgList_t  *pNewhead = netDeviceInfo.head->next;	//保存链表头的下一段数据地址
#ifdef U3print_Debug
	  U3_printf((uint8_t *)"\r\n------ DeleteDataSendList ------------", 0, 0);
#endif	
    netDeviceInfo.head->bufLen = 0;
	  netDeviceInfo.head->reSendCount = 0;
	  netDeviceInfo.head->flag = 0;
	  netDeviceInfo.head->connectid = 0;
  	netDeviceInfo.head->sn = 0;
    netDeviceInfo.head->next = NULL;
    free(netDeviceInfo.head->buf);			//释放内存
    free(netDeviceInfo.head);				    //释放内存

    netDeviceInfo.head = pNewhead;					//链表头指向下一段数据

    return 0;
}

/***************************************************************************************************
 * @fn      _Bool NET_DEVICE_DeleteDataSendListHead( uint16_t  ReceiveSn )
 *
 * @brief   从链表头删除一个相对应的数据序号帧或者心跳帧
 *
 * @param   uint16_t  ReceiveSn  输入序号
 *
 * @return  0
 ***************************************************************************************************/
_Bool NET_DEVICE_DeleteDataSendListHead( uint16_t  ReceiveSn )
{
  netMsgList_t  *pNewhead = netDeviceInfo.head->next;	//保存链表头的下一段数据地址
	
	if(  netDeviceInfo.head->sn == ReceiveSn || netDeviceInfo.head->sn == 0x7FFF )//上报帧或者心跳帧
	{
#ifdef U3print_Debug
			U3_printf((uint8_t *)"\r\n------ DeleteDataSendList ------------", 0, 0);
#endif	
			netDeviceInfo.head->bufLen = 0;
			netDeviceInfo.head->reSendCount = 0;
			netDeviceInfo.head->flag = 0;
			netDeviceInfo.head->connectid = 0;
			netDeviceInfo.head->sn = 0;
			netDeviceInfo.head->next = NULL;
			free(netDeviceInfo.head->buf);			//释放内存
			free(netDeviceInfo.head);				    //释放内存

			netDeviceInfo.head = pNewhead;					//链表头指向下一段数据
     
		  FrameIntervalFlag = 1;
		  return 0;
	}
	return 1;
}
/*********************************************************************
 * @fn   uint8_t GetElementNumInList( rs485MsgList_t *head )
 *
 * @brief 统计该列表的元素个数
 *          
 * @param            
 *
 * @return  none
 */
uint8_t NET_DEVICE_GetNumInList( netMsgList_t *head )
{    
	netMsgList_t *q = head;
  uint8_t num = 0;

	while(q->next != NULL)
	{
		q = q->next;
		num++;
	}
	
	return num;
}



/*********************************************************************
 * @fn      void NetDevSend_Readqueue_Handle( void )
 *
 * @brief   WIFI模块设备发送数据队列读取
 *          
 * @param   none          
 *
 * @return  none
 */
void NetDevSend_Readqueue_Handle( void )
{
	netMsgList_t *pItem = netDeviceInfo.head;
					
	if( pItem && FrameIntervalFlag )//消息缓冲不为空,接收完成标志
	{
		FrameIntervalFlag = 0;

		NetDevSendFlag = WIFI_DEVICE_SendData(pItem->connectid , pItem->buf,  pItem->bufLen );//数据发送
		if( pItem->flag ) //主动上发帧，5021ms超时判断,此次作为超时判断
		  Timer_Event_Set( &timer_10msDB[NwkrRevTimeout_Task], 5021,  ENABLE,  0,  RevNwkRsponseTimeout_Evt );  //
			
		if( NetDevSendFlag == SUCCESS )	
		{
			SendErrCount = 0;
			if( pItem->reSendCount == 0 || pItem->sn >0x7FFF || pItem->flag == 0)//等于零表示数据需要重发，再删除节点|| NetDevSendFlag == SUCCESS
			{
				U3_printf((uint8_t *)"\r\n -----NET MSG ReSend---- ", 0, 0);
				NET_DEVICE_DeleteDataSendList();
			}
			else
			{
				U3_printf((uint8_t *)"  reSendCount--", 0, 0);
				pItem->reSendCount--;
			}
		}
		else //发送失败PING一下
		{
			U3_printf((uint8_t *)"\r\n------ SendErr ------------", 0, 0);
			SendErrCount++;
			if( SendErrCount > 4 )//发送4次都是err,重新尝试链接
			{
				U3_printf((uint8_t *)"\r\n------ SendErr Over------------", 0, 0);
				
				NET_DEVICE_DeleteDataSendList();
        Timer_Event_Set( &timer_10msDB[NwkrRevTimeout_Task], 0, DISABLE, 0, RevNwkRsponseTimeout_Evt );      //超时检测关闭

				if( pItem->connectid == CONNID_TCPCLIENT )//设备工作服务器
				{
//					ServiceState = OFFLINE;
//					 //关闭心跳包
//					//关闭状态包

				}
				
				SendErrCount = 0;
				FrameIntervalFlag = 0;
				
				devState = DEV_NWK_PING;
				Timer_Event_Set( &timer_10msDB[WifiFMS_Task],  100,  ENABLE,  0, NET_DEVICE_StateChange_Evt );//100ms后检测
			}
		}					
	}
}




/*********************************************************************
 * @fn      void NET_DEVICE_StateChange_Evt( void )
 *
 * @brief   WIFI 模块状态机
 *          
 * @param   none        
 *
 * @return  none
 */
void NET_DEVICE_StateChange_Evt( void )
{ 
  static uint16_t ErrCount;

//  DEV_RST,               // Initialized - not started automatically	
//  DEV_SMART,             //一键配置状态
//  DEV_READY,             
//	DEV_NWK_PING,          // Registered, home network
//	DEV_NWK_CONNECTING,    // Device has lost
//	
//  DEV_NWK_ONLINE,        // Device has lost
//	DEV_NWK_SEND
	
//  CONNID_UDPMASTER,   //0
//	CONNID_TCPCLIENT,   //0

  Timer_Event_Set( &timer_10msDB[WifiFMS_Task],  0,  DISABLE,  0,  NET_DEVICE_StateChange_Evt );
	
  switch( devState )
  {
	  case DEV_RST: 
			    U3_printf((uint8_t *)"WIFI Rest...\r\n", 0, 0);	

          WIFI_DEVICE_Reset(); //RST WIFI
	      	Timer_Event_Set( &timer_10msDB[WifiFMS_Task],  5000,  ENABLE,  0, NET_DEVICE_StateChange_Evt );//5秒后触发事件
					devState = DEV_READY;
					break; 
				
    case DEV_READY:  
		    {			
		      if( WIFI_DEVICE_SendCmd("AT\r\n", "OK", 500, 1) == SUCCESS )//检测到模块起来
					{
						U3_printf((uint8_t *)"WIFI Device Find OK...\r\n", 0, 0);		
						
						WIFI_DEVICE_SendCmd("ATE0\r\n", "OK", 500, 1 );
						WIFI_DEVICE_SendCmd("AT+CWMODE_DEF=1\r\n", "OK", 500, 1 );//STA模式
						WIFI_DEVICE_SendCmd("AT+CWAUTOCONN=0\r\n","OK",500,1);//自动链接wifi关闭
						
						if( DeviceInfor.SmartCfgState != SMARTCFG_SETUP )
						{
							memset(connData, 0x00, 50);
							sprintf(connData,"AT+CWJAP_DEF=\"%s\",\"%s\"\r\n",DeviceInfor.ssid, DeviceInfor.password);
							WIFI_DEVICE_SendCmd(connData,"OK",10,1);
							//1.入网成功，直接输出WIFI GET IP  ;2.不成功WIFI DISCONNECT +CWJAP:2/r/nFAIL;3.还有一个超时事件1分钟
							
						}
						else 
						{
							WIFI_DEVICE_SendCmd("AT+CWSTARTSMART=1\r\n","OK",500,1);//开启一键配置功能
              //同时开启超时处理，一分钟后关闭一键配置，SMARTCFG_IDLE,触发模块重启或者DEV_READY
						}
						ErrCount = 0;
					}
					else
					{
						Timer_Event_Set( &timer_10msDB[WifiFMS_Task],  100,  ENABLE,  0, NET_DEVICE_StateChange_Evt );//100ms检测一次
						ErrCount++;
						if( ErrCount > 5)//检测 600
						{
							U3_printf((uint8_t *)"not find WIFI device...\r\n", 0, 0);		
							Set_Event(&timer_10msDB[WifiFMS_Task], NET_DEVICE_StateChange_Evt);
							devState = DEV_RST;//重启模块
						}
					}
				}
			  break;              

	  case  DEV_WIFICONNECT://链接路由器完成
			    WIFI_DEVICE_SendCmd("AT+CIPMUX=0\r\n","OK",500,1);
					WIFI_DEVICE_SendCmd("AT+CIPMUX=1\r\n","OK",500,1);
					WIFI_DEVICE_SendCmd("AT+CIPSTART=0,\"UDP\",\"127.0.0.1\",8888,8888,2","0,CONNECT",1000,1);//建立UDP服务，用于广播扫描
		
					/******PING外部网络******/
					devState = DEV_NWK_PING;
					Timer_Event_Set( &timer_10msDB[WifiFMS_Task],  1000,  ENABLE,  0, NET_DEVICE_StateChange_Evt );
		      break;
		 
    case DEV_NWK_PING: //尝试外网链接,确保网络正常 
		     U3_printf((uint8_t *)"--------DEV_NWK_PING--------\r\n", 0, 0);	
	       if( WIFI_DEVICE_SendPing("AT+PING=\"www.NBA.com\"\r\n", 1000U, 1) == SUCCESS )
				 {
					  ErrCount = 0;
					  Timer_Event_Set( &timer_10msDB[WifiFMS_Task],  100,  ENABLE,  0, NET_DEVICE_StateChange_Evt );
						devState = DEV_NWK_CONNECTING;
				 }
				 else
				 {
					  Timer_Event_Set( &timer_10msDB[WifiFMS_Task],  1000,  ENABLE,  0, NET_DEVICE_StateChange_Evt );//1S PING
						ErrCount++;
						if( ErrCount > 4 )//检测 4次
						{
							ErrCount = 0;
							U3_printf((uint8_t *)"PING Err...\r\n", 0, 0);
							
							Set_Event(&timer_10msDB[WifiFMS_Task], NET_DEVICE_StateChange_Evt);
							devState = DEV_RST;//重启模块
						}
				 }
         break;
	
		case DEV_NWK_CONNECTING:
			     U3_printf((uint8_t *)"-------- DEV CONNECTING SEVER --------\r\n", 0, 0);	
			     if(WIFI_DEVICE_SendCmd("AT+CIPSTART=1,\"TCP\",\"test.168home.net\",9055", "1,CONNECT",1000,1) == SUCCESS )
					 {
						 ErrCount = 0;		
/*********************************************************************************************************************/
//运行网络数据发送逻辑					 
/*********************************************************************************************************************/				 
					 }
					 else
					 {
						  Timer_Event_Set( &timer_10msDB[WifiFMS_Task],  2000,  ENABLE,  0, NET_DEVICE_StateChange_Evt );//2S connectting
							ErrCount++;
							if( ErrCount > 4 )//检测 4次
							{
								ErrCount = 0;
								U3_printf((uint8_t *)"CONNECTING Err...\r\n", 0, 0);
								
								Set_Event(&timer_10msDB[WifiFMS_Task], NET_DEVICE_StateChange_Evt);
								devState = DEV_RST;//重启模块
							}
					 }
		       break;
		
					
					
//		case DEV_NWK_ONLINE: //模块在线,进行网络链接操作步骤
//			    WIFI_DEVICE_SendCmd("AT+QIOPEN=1,0,\"TCP\",\"115.28.214.192\",9055,0,1\r\n","OK",1000,1);		//重新连接		      
//          break;
//		case DEV_NWK_SEND:
//				   // start = HAL_GetTick();
//		     DeviceInfor.Secs = App_SysGetUTCSecs(1);
//				 memset(tcpdata, 0x00, 1280);
//		     sprintf((char *)tcpdata, "Index:%d, UTC:%d", index++,DeviceInfor.Secs);
//		     NET_DEVICE_SendData(0, tcpdata, strlen((char *)tcpdata));
//         break;
	
    default: break;
  }
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
void ReceiveWifiATResponseData_Handle(void)
{
	char *RevPtr = NULL ;
	
	uint8_t RevLen,connectId;
	//uint32_t startadr, endadr;
	
	if( Uart5Handle.RxXferCount == 0xFFFF )
	{		
		Uart5Handle.RxXferCount = 0;
		U3_printf(Uart5Handle.pRxBuffPtr,0,0);
//	
//	  /********** WIFI模块启动状态返回数据 **********/
//		if( strstr((char *)Uart5Handle.pRxBuffPtr, "ready\r\n"))// 模块启动标志
//		{
//			devState = DEV_READY;
//			Timer_Event_Set( &timer_10msDB[WifiFMS_Task],  50,  ENABLE,  0, NET_DEVICE_StateChange_Evt );
//			memset(Uart5Handle.pRxBuffPtr, 0x00, Uart5Handle.RxXferSize);
//		  return;
//		}
//		
//		if( strstr((char *)Uart5Handle.pRxBuffPtr, "CLOSED\r\n"))//tcp链接断开
//		{	
//			NET_DEVICE_ClosedRsp_Handle((char *)Uart5Handle.pRxBuffPtr);
//			memset(Uart5Handle.pRxBuffPtr, 0x00, Uart5Handle.RxXferSize);
//		  return;
//		}
//		
//		/********** WIFI模块链接路由器成功后进行联网操作 **********/
//		if( strstr((char *)Uart5Handle.pRxBuffPtr, "WIFI CONNECTED\r\n"))//模块链接路由器标志
//		{
//			devState = DEV_WIFICONNECT;
//			Timer_Event_Set( &timer_10msDB[WifiFMS_Task],  2000,  ENABLE,  0, NET_DEVICE_StateChange_Evt );
//			memset(Uart5Handle.pRxBuffPtr, 0x00, Uart5Handle.RxXferSize);
//		  return;
//		}
//		if( strstr((char *)Uart5Handle.pRxBuffPtr, "WIFI GOT IP\r\n"))//模块链接路由器标志
//		{
//			devState = DEV_WIFICONNECT;
//			Timer_Event_Set( &timer_10msDB[WifiFMS_Task],  100,  ENABLE,  0, NET_DEVICE_StateChange_Evt );
//			memset(Uart5Handle.pRxBuffPtr, 0x00, Uart5Handle.RxXferSize);
//		  return;
//		}
//		
//		/********** WIFI模块链接路由器不成功返回，或者路由器端口也会返回（TCP链接也会断开）**********/
//		if( memcmp((char *)Uart5Handle.pRxBuffPtr, "WIFI DISCONNECT\r\n",17)==0)//WIFI链接不成功操作
//		{
//			//清除超时事件1分钟
//			devState = DEV_RST;
//			Timer_Event_Set( &timer_10msDB[WifiFMS_Task],  2000,  ENABLE,  0, NET_DEVICE_StateChange_Evt );
//			memset(Uart5Handle.pRxBuffPtr, 0x00, Uart5Handle.RxXferSize);
//		  return;
//		}

//		if( strstr((char *)Uart5Handle.pRxBuffPtr, "+CWJAP:") && strstr((char *)Uart5Handle.pRxBuffPtr, "FAIL"))
//		{
//			//清除超时事件1分钟
//			devState = DEV_RST;
//			Timer_Event_Set( &timer_10msDB[WifiFMS_Task],  2000,  ENABLE,  0, NET_DEVICE_StateChange_Evt );
//			memset(Uart5Handle.pRxBuffPtr, 0x00, Uart5Handle.RxXferSize);
//		  return;
//		}
//	
//		/********************************一键配置******************************/
//		//进入一键配置功能，关闭UDP,TCP,发送AT+CWSTARTSMART=1一键配置命令（会返回TCP CLOSEWIFI DISCONNECT），开启一键配置超时事件
//		if( strstr((char *)Uart5Handle.pRxBuffPtr, "smartconfig type:ESPTOUCH\r\n"))
//		{
//			DeviceInfor.SmartCfgState = SMARTCFG_DOING;
//			memset(Uart5Handle.pRxBuffPtr, 0x00, Uart5Handle.RxXferSize);
//		  return;
//		}
//		if( strstr((char *)Uart5Handle.pRxBuffPtr, "Smart get wifi info\r\n"))
//		{
//			NET_DEVICE_SmartTouch_Handle((char *)Uart5Handle.pRxBuffPtr);//获取ssid和password
//			memset(Uart5Handle.pRxBuffPtr, 0x00, Uart5Handle.RxXferSize);
//		  return;
//		}
//		if( strstr((char *)Uart5Handle.pRxBuffPtr, "smartconfig connected wifi\r\n"))
//		{
//			DeviceInfor.SmartCfgState = SMARTCFG_COMPELE;//一键配置结束完成
//			if(WIFI_DEVICE_SendCmd("AT+CWSTOPSMART", "OK", 500U, 1)== SUCCESS )
//			{
//				DeviceInfor.SmartCfgState  = SMARTCFG_IDLE;
//			}
//			memset(Uart5Handle.pRxBuffPtr, 0x00, Uart5Handle.RxXferSize);
//		  return;
//		}
///***************************************************************************************************/			
//	 //接收远程数据处理
//	  if( strstr((char *)Uart5Handle.pRxBuffPtr, "+IPD,"))// 收到远程数据//+IPD,1,20:http://www.cmsoft.cn
//		{ 
//			 RevPtr =(char *)Uart5Handle.pRxBuffPtr;
//			 RevPtr = strchr( RevPtr, ',')+1;
//		   connectId =  strtoul( RevPtr, NULL, 10 );
//			 RevPtr = strchr( RevPtr, ',')+1;
//			 RevLen = strtoul( RevPtr, NULL, 10 );
//			 RevPtr = strchr( RevPtr, ':')+1;

//			 if( connectId == 0 )
//			 {
//				 //UDP广播接收
//				 UDP_RevNwkData_Handle((uint8_t *)RevPtr, RevLen);
//			 }
//			 else if( connectId == 1)
//			 {
//				 //TCP 服务器数据接收
//				 TCP_RevNwkData_Handle((uint8_t *)RevPtr, RevLen);
//			 }
//			memset(Uart5Handle.pRxBuffPtr, 0x00, Uart5Handle.RxXferSize);
//		  return;
//		}
//		
		memset(Uart5Handle.pRxBuffPtr, 0x00, Uart5Handle.RxXferSize);
		return;	
	}
}


/*********************************************************************
 * @fn      void NET_DEVICE_SmartTouch_Handle( char *wifiInfor )
 *
 * @brief   wifi账号密码配置获取
 *          
 * @param   *pbuf: "Smart get wifi info\r\n\ssid:ZNSHJ\r\npassword:15200611\r\n"
 *
 * @return   none
 */

void NET_DEVICE_SmartTouch_Handle( char *wifiInfor )
{
		char *ptr = NULL ;
		uint32_t startadr, endadr;

		ptr = strstr( ptr, "ssid:");
		ptr = strchr( ptr, ':')+1;
		startadr = (uint32_t )ptr;
		ptr = strchr(ptr,'\r');
		endadr = (uint32_t )ptr;
		DeviceInfor.ssidlen = endadr - startadr;
		if( DeviceInfor.ssidlen <= 30 )//SSID长度过大，不需要
			memcpy(DeviceInfor.ssid, (uint8_t *)startadr, DeviceInfor.ssidlen);

		ptr = strstr( ptr, "password:");
		ptr = strchr( ptr, ':')+1;
		startadr = (uint32_t )ptr;
		ptr = strchr(ptr,'\r');
		endadr = (uint32_t )ptr;
		DeviceInfor.passwordlen = endadr - startadr;
		if( DeviceInfor.passwordlen <= 30 )//password长度过大，不需要
			memcpy(DeviceInfor.password, (uint8_t *)startadr, DeviceInfor.passwordlen);
		
}

/*********************************************************************
 * @fn      void NET_DEVICE_RDYRsp_Handle( char *pbuf )
 *
 * @brief   模块重启返回状态数据处理
 *          
 * @param   *pbuf: "\r\n+QIURC: \"closed\",4\r\n\r\n+QIURC: \"closed\",2\r\n"
 *
 * @return  none
 */
void NET_DEVICE_RDYRsp_Handle( void )
{
	U3_printf((uint8_t *)" Finded EC20 Device\r\n", 0, 0);
	
	Timer_Event_Set( &timer_10msDB[WifiFMS_Task],  100,  ENABLE,  0, NET_DEVICE_StateChange_Evt );//100ms后触发事件
	devState = DEV_READY;
	
	return;	
}

/*********************************************************************
 * @fn      void NET_DEVICE_ClosedRsp_Handle( char *pbuf )
 *
 * @brief   远程close返回状态数据处理
 *          
 * @param   *pbuf: "0,CLOSED
 *
 * @return  none
 */

void NET_DEVICE_ClosedRsp_Handle( char *pbuf )
{
	//char 	*ptr = NULL;
  //uint8_t resultCode1 = 0;
	
	
	//不管是路由器断开，导致WIFI断开，或者远程TCP服务器断开，都进行主动链接,
	devState = DEV_NWK_PING;
	Timer_Event_Set( &timer_10msDB[WifiFMS_Task],  200,  ENABLE,  0,  NET_DEVICE_StateChange_Evt );	
	  
	//停止网络数据上传事件
	/******************************************************/
	
	
	return;	
}

/*********************************************************************
 * @fn      RevNwkRsponseTimeout_Evt( void )
 *
 * @brief   等待服务器回复超时事件
 *          
 * @param   none        
 *
 * @return  none
 */
void RevNwkRsponseTimeout_Evt( void )
{
	timer_10msDB[NwkrRevTimeout_Task].Flag = 0;
  timer_10msDB[NwkrRevTimeout_Task].Enable = DISABLE;
  timer_10msDB[NwkrRevTimeout_Task].Time = 0;
	
	FrameIntervalFlag = 1;
	
	U3_printf((uint8_t *)"\r\n------ Wait Receive Timeout -------\r\n", 0, 0);
}

/*********************************************************************
 * @fn      void TCP_RevNwkData_Handle(uint8_t *buf,  uint16_t buflen)
 *
 * @brief    接收处理
 *          
 * @param   none        
 *
 * @return  none
 */
void TCP_RevNwkData_Handle(uint8_t *buf,  uint16_t buflen)
{
	
// netMsgList_t *pItem = netDeviceInfo.head;
// flag_t fg;
// uint8_t *dataPtr = NULL,*rspPtr = NULL,lo,hi,data[2];
// uint16_t receiveSn = 0;//datalen = 0;

	
	
}

/*********************************************************************
 * @fn      void UDP_RevNwkData_Handle(uint8_t *buf,  uint16_t buflen)
 *
 * @brief   接收处理
 *          
 * @param   none        
 *
 * @return  none
 */
void UDP_RevNwkData_Handle(uint8_t *buf,  uint16_t buflen)
{
	
// netMsgList_t *pItem = netDeviceInfo.head;
// flag_t fg;
// uint8_t *dataPtr = NULL,*rspPtr = NULL,lo,hi,data[2];
// uint16_t receiveSn = 0;//datalen = 0;

	
	
}




