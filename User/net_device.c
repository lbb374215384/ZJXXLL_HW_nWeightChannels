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
|  PV  |	Flag |  Date |�豸����|���Ҵ���|	Len   [ Э�����к�|	Tlen |	TerminalSN | Data ]
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
										��Ϣ����
******************************************************************************************/

/***************************************************************************************************
 * @fn      _Bool NET_DEVICE_CheckListHead(void)
 *
 * @brief   ��鷢������ͷ�Ƿ�Ϊ��
 *
 * @param   NULL
 *
 * @return  0-��	1-��Ϊ��
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
 * @brief   ��ȡ��������Ҫ���͵�����ָ��
 *
 * @param   NULL
 *
 * @return  ��ȡ��������Ҫ���͵�����ָ��
 ***************************************************************************************************/
uint8_t *NET_DEVICE_GetListHeadBuf(void)
{

  return netDeviceInfo.head->buf;
}

/***************************************************************************************************
 * @fn      uint16_t NET_DEVICE_GetListHeadLen( void )
 *
 * @brief   ��ȡ��������Ҫ���͵����ݳ���
 *
 * @param   NULL
 *
 * @return  ��ȡ��������Ҫ���͵����ݳ���
 ***************************************************************************************************/
uint16_t NET_DEVICE_GetListHeadLen( void )
{
    return netDeviceInfo.head->bufLen;
}

/***************************************************************************************************
 * @fn      uint8_t NET_DEVICE_AddDataSendList( uint8_t *buf , uint16_t dataLen )
 *
 * @brief   �ڷ�������β����һ����������
 *
 * @param   connId:���Ӻ�  buf����Ҫ���͵����� dataLen�����ݳ���
 *
 * @return  0-�ɹ�	����-ʧ��
 ***************************************************************************************************/
uint8_t NET_DEVICE_AddDataSendList( uint8_t connId, uint8_t *buf , uint16_t dataLen , uint8_t sendcount , uint16_t protocolsn)
{
   netMsgList_t *current = NULL;
	
	 current = (netMsgList_t *)malloc(sizeof(netMsgList_t));//�����ڴ�
   
   if( current == NULL )
	 {
		 free(buf); 
		 return 1;
   }
     
   if( netDeviceInfo.head == NULL )							//���headΪNULL
	 { 
     netDeviceInfo.head = current;							//headָ��ǰ������ڴ���
		 FrameIntervalFlag = 1;//�����¼���ȡ����
	 }
   else										//���head��ΪNULL
     netDeviceInfo.end->next = current;				//��endָ��ǰ������ڴ���

    //memcpy(current->buf, buf, dataLen);						//��������
	  current->connectid = connId;
		current->buf = buf;//��������
    current->bufLen = dataLen;
		current->sn = protocolsn;
    current->next = NULL;								//��һ��ΪNULL
    current->reSendCount = sendcount;
	  current->flag = sendcount;          //0���ظ�֡   1�������Ϸ�֡
    netDeviceInfo.end = current;        //endָ��ǰ������ڴ���
		
   // FrameIntervalFlag = 1;
    return 0;
}

/***************************************************************************************************
 * @fn      _Bool NET_DEVICE_DeleteDataSendList(void)
 *
 * @brief   ������ͷɾ��һ������
 *
 * @param   NULL
 *
 * @return  NULL
 ***************************************************************************************************/
_Bool NET_DEVICE_DeleteDataSendList(void)
{
    netMsgList_t  *pNewhead = netDeviceInfo.head->next;	//��������ͷ����һ�����ݵ�ַ
#ifdef U3print_Debug
	  U3_printf((uint8_t *)"\r\n------ DeleteDataSendList ------------", 0, 0);
#endif	
    netDeviceInfo.head->bufLen = 0;
	  netDeviceInfo.head->reSendCount = 0;
	  netDeviceInfo.head->flag = 0;
	  netDeviceInfo.head->connectid = 0;
  	netDeviceInfo.head->sn = 0;
    netDeviceInfo.head->next = NULL;
    free(netDeviceInfo.head->buf);			//�ͷ��ڴ�
    free(netDeviceInfo.head);				    //�ͷ��ڴ�

    netDeviceInfo.head = pNewhead;					//����ͷָ����һ������

    return 0;
}

/***************************************************************************************************
 * @fn      _Bool NET_DEVICE_DeleteDataSendListHead( uint16_t  ReceiveSn )
 *
 * @brief   ������ͷɾ��һ�����Ӧ���������֡��������֡
 *
 * @param   uint16_t  ReceiveSn  �������
 *
 * @return  0
 ***************************************************************************************************/
_Bool NET_DEVICE_DeleteDataSendListHead( uint16_t  ReceiveSn )
{
  netMsgList_t  *pNewhead = netDeviceInfo.head->next;	//��������ͷ����һ�����ݵ�ַ
	
	if(  netDeviceInfo.head->sn == ReceiveSn || netDeviceInfo.head->sn == 0x7FFF )//�ϱ�֡��������֡
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
			free(netDeviceInfo.head->buf);			//�ͷ��ڴ�
			free(netDeviceInfo.head);				    //�ͷ��ڴ�

			netDeviceInfo.head = pNewhead;					//����ͷָ����һ������
     
		  FrameIntervalFlag = 1;
		  return 0;
	}
	return 1;
}
/*********************************************************************
 * @fn   uint8_t GetElementNumInList( rs485MsgList_t *head )
 *
 * @brief ͳ�Ƹ��б��Ԫ�ظ���
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
 * @brief   WIFIģ���豸�������ݶ��ж�ȡ
 *          
 * @param   none          
 *
 * @return  none
 */
void NetDevSend_Readqueue_Handle( void )
{
	netMsgList_t *pItem = netDeviceInfo.head;
					
	if( pItem && FrameIntervalFlag )//��Ϣ���岻Ϊ��,������ɱ�־
	{
		FrameIntervalFlag = 0;

		NetDevSendFlag = WIFI_DEVICE_SendData(pItem->connectid , pItem->buf,  pItem->bufLen );//���ݷ���
		if( pItem->flag ) //�����Ϸ�֡��5021ms��ʱ�ж�,�˴���Ϊ��ʱ�ж�
		  Timer_Event_Set( &timer_10msDB[NwkrRevTimeout_Task], 5021,  ENABLE,  0,  RevNwkRsponseTimeout_Evt );  //
			
		if( NetDevSendFlag == SUCCESS )	
		{
			SendErrCount = 0;
			if( pItem->reSendCount == 0 || pItem->sn >0x7FFF || pItem->flag == 0)//�������ʾ������Ҫ�ط�����ɾ���ڵ�|| NetDevSendFlag == SUCCESS
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
		else //����ʧ��PINGһ��
		{
			U3_printf((uint8_t *)"\r\n------ SendErr ------------", 0, 0);
			SendErrCount++;
			if( SendErrCount > 4 )//����4�ζ���err,���³�������
			{
				U3_printf((uint8_t *)"\r\n------ SendErr Over------------", 0, 0);
				
				NET_DEVICE_DeleteDataSendList();
        Timer_Event_Set( &timer_10msDB[NwkrRevTimeout_Task], 0, DISABLE, 0, RevNwkRsponseTimeout_Evt );      //��ʱ���ر�

				if( pItem->connectid == CONNID_TCPCLIENT )//�豸����������
				{
//					ServiceState = OFFLINE;
//					 //�ر�������
//					//�ر�״̬��

				}
				
				SendErrCount = 0;
				FrameIntervalFlag = 0;
				
				devState = DEV_NWK_PING;
				Timer_Event_Set( &timer_10msDB[WifiFMS_Task],  100,  ENABLE,  0, NET_DEVICE_StateChange_Evt );//100ms����
			}
		}					
	}
}




/*********************************************************************
 * @fn      void NET_DEVICE_StateChange_Evt( void )
 *
 * @brief   WIFI ģ��״̬��
 *          
 * @param   none        
 *
 * @return  none
 */
void NET_DEVICE_StateChange_Evt( void )
{ 
  static uint16_t ErrCount;

//  DEV_RST,               // Initialized - not started automatically	
//  DEV_SMART,             //һ������״̬
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
	      	Timer_Event_Set( &timer_10msDB[WifiFMS_Task],  5000,  ENABLE,  0, NET_DEVICE_StateChange_Evt );//5��󴥷��¼�
					devState = DEV_READY;
					break; 
				
    case DEV_READY:  
		    {			
		      if( WIFI_DEVICE_SendCmd("AT\r\n", "OK", 500, 1) == SUCCESS )//��⵽ģ������
					{
						U3_printf((uint8_t *)"WIFI Device Find OK...\r\n", 0, 0);		
						
						WIFI_DEVICE_SendCmd("ATE0\r\n", "OK", 500, 1 );
						WIFI_DEVICE_SendCmd("AT+CWMODE_DEF=1\r\n", "OK", 500, 1 );//STAģʽ
						WIFI_DEVICE_SendCmd("AT+CWAUTOCONN=0\r\n","OK",500,1);//�Զ�����wifi�ر�
						
						if( DeviceInfor.SmartCfgState != SMARTCFG_SETUP )
						{
							memset(connData, 0x00, 50);
							sprintf(connData,"AT+CWJAP_DEF=\"%s\",\"%s\"\r\n",DeviceInfor.ssid, DeviceInfor.password);
							WIFI_DEVICE_SendCmd(connData,"OK",10,1);
							//1.�����ɹ���ֱ�����WIFI GET IP  ;2.���ɹ�WIFI DISCONNECT +CWJAP:2/r/nFAIL;3.����һ����ʱ�¼�1����
							
						}
						else 
						{
							WIFI_DEVICE_SendCmd("AT+CWSTARTSMART=1\r\n","OK",500,1);//����һ�����ù���
              //ͬʱ������ʱ����һ���Ӻ�ر�һ�����ã�SMARTCFG_IDLE,����ģ����������DEV_READY
						}
						ErrCount = 0;
					}
					else
					{
						Timer_Event_Set( &timer_10msDB[WifiFMS_Task],  100,  ENABLE,  0, NET_DEVICE_StateChange_Evt );//100ms���һ��
						ErrCount++;
						if( ErrCount > 5)//��� 600
						{
							U3_printf((uint8_t *)"not find WIFI device...\r\n", 0, 0);		
							Set_Event(&timer_10msDB[WifiFMS_Task], NET_DEVICE_StateChange_Evt);
							devState = DEV_RST;//����ģ��
						}
					}
				}
			  break;              

	  case  DEV_WIFICONNECT://����·�������
			    WIFI_DEVICE_SendCmd("AT+CIPMUX=0\r\n","OK",500,1);
					WIFI_DEVICE_SendCmd("AT+CIPMUX=1\r\n","OK",500,1);
					WIFI_DEVICE_SendCmd("AT+CIPSTART=0,\"UDP\",\"127.0.0.1\",8888,8888,2","0,CONNECT",1000,1);//����UDP�������ڹ㲥ɨ��
		
					/******PING�ⲿ����******/
					devState = DEV_NWK_PING;
					Timer_Event_Set( &timer_10msDB[WifiFMS_Task],  1000,  ENABLE,  0, NET_DEVICE_StateChange_Evt );
		      break;
		 
    case DEV_NWK_PING: //������������,ȷ���������� 
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
						if( ErrCount > 4 )//��� 4��
						{
							ErrCount = 0;
							U3_printf((uint8_t *)"PING Err...\r\n", 0, 0);
							
							Set_Event(&timer_10msDB[WifiFMS_Task], NET_DEVICE_StateChange_Evt);
							devState = DEV_RST;//����ģ��
						}
				 }
         break;
	
		case DEV_NWK_CONNECTING:
			     U3_printf((uint8_t *)"-------- DEV CONNECTING SEVER --------\r\n", 0, 0);	
			     if(WIFI_DEVICE_SendCmd("AT+CIPSTART=1,\"TCP\",\"test.168home.net\",9055", "1,CONNECT",1000,1) == SUCCESS )
					 {
						 ErrCount = 0;		
/*********************************************************************************************************************/
//�����������ݷ����߼�					 
/*********************************************************************************************************************/				 
					 }
					 else
					 {
						  Timer_Event_Set( &timer_10msDB[WifiFMS_Task],  2000,  ENABLE,  0, NET_DEVICE_StateChange_Evt );//2S connectting
							ErrCount++;
							if( ErrCount > 4 )//��� 4��
							{
								ErrCount = 0;
								U3_printf((uint8_t *)"CONNECTING Err...\r\n", 0, 0);
								
								Set_Event(&timer_10msDB[WifiFMS_Task], NET_DEVICE_StateChange_Evt);
								devState = DEV_RST;//����ģ��
							}
					 }
		       break;
		
					
					
//		case DEV_NWK_ONLINE: //ģ������,�����������Ӳ�������
//			    WIFI_DEVICE_SendCmd("AT+QIOPEN=1,0,\"TCP\",\"115.28.214.192\",9055,0,1\r\n","OK",1000,1);		//��������		      
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
 * @brief   ����WIFI���ݴ���
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
//	  /********** WIFIģ������״̬�������� **********/
//		if( strstr((char *)Uart5Handle.pRxBuffPtr, "ready\r\n"))// ģ��������־
//		{
//			devState = DEV_READY;
//			Timer_Event_Set( &timer_10msDB[WifiFMS_Task],  50,  ENABLE,  0, NET_DEVICE_StateChange_Evt );
//			memset(Uart5Handle.pRxBuffPtr, 0x00, Uart5Handle.RxXferSize);
//		  return;
//		}
//		
//		if( strstr((char *)Uart5Handle.pRxBuffPtr, "CLOSED\r\n"))//tcp���ӶϿ�
//		{	
//			NET_DEVICE_ClosedRsp_Handle((char *)Uart5Handle.pRxBuffPtr);
//			memset(Uart5Handle.pRxBuffPtr, 0x00, Uart5Handle.RxXferSize);
//		  return;
//		}
//		
//		/********** WIFIģ������·�����ɹ�������������� **********/
//		if( strstr((char *)Uart5Handle.pRxBuffPtr, "WIFI CONNECTED\r\n"))//ģ������·������־
//		{
//			devState = DEV_WIFICONNECT;
//			Timer_Event_Set( &timer_10msDB[WifiFMS_Task],  2000,  ENABLE,  0, NET_DEVICE_StateChange_Evt );
//			memset(Uart5Handle.pRxBuffPtr, 0x00, Uart5Handle.RxXferSize);
//		  return;
//		}
//		if( strstr((char *)Uart5Handle.pRxBuffPtr, "WIFI GOT IP\r\n"))//ģ������·������־
//		{
//			devState = DEV_WIFICONNECT;
//			Timer_Event_Set( &timer_10msDB[WifiFMS_Task],  100,  ENABLE,  0, NET_DEVICE_StateChange_Evt );
//			memset(Uart5Handle.pRxBuffPtr, 0x00, Uart5Handle.RxXferSize);
//		  return;
//		}
//		
//		/********** WIFIģ������·�������ɹ����أ�����·�����˿�Ҳ�᷵�أ�TCP����Ҳ��Ͽ���**********/
//		if( memcmp((char *)Uart5Handle.pRxBuffPtr, "WIFI DISCONNECT\r\n",17)==0)//WIFI���Ӳ��ɹ�����
//		{
//			//�����ʱ�¼�1����
//			devState = DEV_RST;
//			Timer_Event_Set( &timer_10msDB[WifiFMS_Task],  2000,  ENABLE,  0, NET_DEVICE_StateChange_Evt );
//			memset(Uart5Handle.pRxBuffPtr, 0x00, Uart5Handle.RxXferSize);
//		  return;
//		}

//		if( strstr((char *)Uart5Handle.pRxBuffPtr, "+CWJAP:") && strstr((char *)Uart5Handle.pRxBuffPtr, "FAIL"))
//		{
//			//�����ʱ�¼�1����
//			devState = DEV_RST;
//			Timer_Event_Set( &timer_10msDB[WifiFMS_Task],  2000,  ENABLE,  0, NET_DEVICE_StateChange_Evt );
//			memset(Uart5Handle.pRxBuffPtr, 0x00, Uart5Handle.RxXferSize);
//		  return;
//		}
//	
//		/********************************һ������******************************/
//		//����һ�����ù��ܣ��ر�UDP,TCP,����AT+CWSTARTSMART=1һ����������᷵��TCP CLOSE�wWIFI DISCONNECT��������һ�����ó�ʱ�¼�
//		if( strstr((char *)Uart5Handle.pRxBuffPtr, "smartconfig type:ESPTOUCH\r\n"))
//		{
//			DeviceInfor.SmartCfgState = SMARTCFG_DOING;
//			memset(Uart5Handle.pRxBuffPtr, 0x00, Uart5Handle.RxXferSize);
//		  return;
//		}
//		if( strstr((char *)Uart5Handle.pRxBuffPtr, "Smart get wifi info\r\n"))
//		{
//			NET_DEVICE_SmartTouch_Handle((char *)Uart5Handle.pRxBuffPtr);//��ȡssid��password
//			memset(Uart5Handle.pRxBuffPtr, 0x00, Uart5Handle.RxXferSize);
//		  return;
//		}
//		if( strstr((char *)Uart5Handle.pRxBuffPtr, "smartconfig connected wifi\r\n"))
//		{
//			DeviceInfor.SmartCfgState = SMARTCFG_COMPELE;//һ�����ý������
//			if(WIFI_DEVICE_SendCmd("AT+CWSTOPSMART", "OK", 500U, 1)== SUCCESS )
//			{
//				DeviceInfor.SmartCfgState  = SMARTCFG_IDLE;
//			}
//			memset(Uart5Handle.pRxBuffPtr, 0x00, Uart5Handle.RxXferSize);
//		  return;
//		}
///***************************************************************************************************/			
//	 //����Զ�����ݴ���
//	  if( strstr((char *)Uart5Handle.pRxBuffPtr, "+IPD,"))// �յ�Զ������//+IPD,1,20:http://www.cmsoft.cn
//		{ 
//			 RevPtr =(char *)Uart5Handle.pRxBuffPtr;
//			 RevPtr = strchr( RevPtr, ',')+1;
//		   connectId =  strtoul( RevPtr, NULL, 10 );
//			 RevPtr = strchr( RevPtr, ',')+1;
//			 RevLen = strtoul( RevPtr, NULL, 10 );
//			 RevPtr = strchr( RevPtr, ':')+1;

//			 if( connectId == 0 )
//			 {
//				 //UDP�㲥����
//				 UDP_RevNwkData_Handle((uint8_t *)RevPtr, RevLen);
//			 }
//			 else if( connectId == 1)
//			 {
//				 //TCP ���������ݽ���
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
 * @brief   wifi�˺��������û�ȡ
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
		if( DeviceInfor.ssidlen <= 30 )//SSID���ȹ��󣬲���Ҫ
			memcpy(DeviceInfor.ssid, (uint8_t *)startadr, DeviceInfor.ssidlen);

		ptr = strstr( ptr, "password:");
		ptr = strchr( ptr, ':')+1;
		startadr = (uint32_t )ptr;
		ptr = strchr(ptr,'\r');
		endadr = (uint32_t )ptr;
		DeviceInfor.passwordlen = endadr - startadr;
		if( DeviceInfor.passwordlen <= 30 )//password���ȹ��󣬲���Ҫ
			memcpy(DeviceInfor.password, (uint8_t *)startadr, DeviceInfor.passwordlen);
		
}

/*********************************************************************
 * @fn      void NET_DEVICE_RDYRsp_Handle( char *pbuf )
 *
 * @brief   ģ����������״̬���ݴ���
 *          
 * @param   *pbuf: "\r\n+QIURC: \"closed\",4\r\n\r\n+QIURC: \"closed\",2\r\n"
 *
 * @return  none
 */
void NET_DEVICE_RDYRsp_Handle( void )
{
	U3_printf((uint8_t *)" Finded EC20 Device\r\n", 0, 0);
	
	Timer_Event_Set( &timer_10msDB[WifiFMS_Task],  100,  ENABLE,  0, NET_DEVICE_StateChange_Evt );//100ms�󴥷��¼�
	devState = DEV_READY;
	
	return;	
}

/*********************************************************************
 * @fn      void NET_DEVICE_ClosedRsp_Handle( char *pbuf )
 *
 * @brief   Զ��close����״̬���ݴ���
 *          
 * @param   *pbuf: "0,CLOSED
 *
 * @return  none
 */

void NET_DEVICE_ClosedRsp_Handle( char *pbuf )
{
	//char 	*ptr = NULL;
  //uint8_t resultCode1 = 0;
	
	
	//������·�����Ͽ�������WIFI�Ͽ�������Զ��TCP�������Ͽ�����������������,
	devState = DEV_NWK_PING;
	Timer_Event_Set( &timer_10msDB[WifiFMS_Task],  200,  ENABLE,  0,  NET_DEVICE_StateChange_Evt );	
	  
	//ֹͣ���������ϴ��¼�
	/******************************************************/
	
	
	return;	
}

/*********************************************************************
 * @fn      RevNwkRsponseTimeout_Evt( void )
 *
 * @brief   �ȴ��������ظ���ʱ�¼�
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
 * @brief    ���մ���
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
 * @brief   ���մ���
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




