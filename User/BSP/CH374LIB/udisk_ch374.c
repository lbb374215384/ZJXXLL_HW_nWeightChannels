/*
*********************************************************************************************************
*	                                  
*	ģ������ : CH374 U���ļ���������
*	�ļ����� : ch374_udisk.c
*	��    �� : V1.0
*	˵    �� : 	U���ļ���дʾ������������STM32��
*				(1) �û�����U�̺�������ʾ����������
*				(2) ��U���е�/STM32/armfly.txt�ļ�������ʾǰ600���ַ�������Ҳ���armfly.txt�ļ�, ��ô����ʾ
*					��Ŀ¼�����е��ļ�����
*				(3) �ڸ�Ŀ¼�´���һ�����ļ�NEWFILE.TXT����д��һ���ַ�����֮���޸��ļ����ڡ�
*				
*				����LED���ڼ����ʾ����Ľ��ȡ�
*				(1) ��U�̲�������LED1���γ�ʱLED1Ϩ��
*				(2) ����дU��ʱ����LED2����ȡ���ʱLED2Ϩ�� 
*				(3) ����д����ʱ��LED3��˸
*
*				CH374��INT#���Ų��ò�ѯ��ʽ����
*
*				���ֽ�Ϊ��λ��дU���ļ�,��д�ٶȽ�����ģʽ����
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		v1.0    2011-01-11 armfly  ST�̼���汾ΪV3.4.0�汾��
*
*	
*
*********************************************************************************************************
*/

#include "stm32f0xx_hal.h"
#include "Global.h"
#include <stdio.h>
#include <string.h>

#include "bsp_CH374.h"

#include "bsp_periph.h"
#include "TimeEvent.h"


#define DEMO_WRITE_EN	1		/* 1��ʾд����ʹ�� */


#define LED_INSERT	1
#define LED_RD_WR	2
#define LED_ERR		3

uint8_t strFileName[256];	/* �洢·��+�ļ��� */
uint8_t ucaDataBuf[128];	/* �洢���������� */

static void StopIfError(uint8_t _ucErr);

usbStates_t USBState = USBDISK_CONNECT;


/*********************************************************************
 * @fn      void USBDiskHandle( void )
 *
 * @brief   U�̶�ȡ ģ��״̬��
 *          
 * @param   none        
 *
 * @return  none
 */
void USBDiskHandle(void)
{
	uint16_t i;
	uint16_t uiTotalCount;
	uint8_t	*pCodeStr;
	uint8_t ucErr;
	int32_t iReadCount;			/* ʵ�ʶ�ȡ���ֽ��� */
	
		UTCTimeStruct UTCC;
	
  timer_10msDB[PeriodUsbDisk_Task].Flag = 0;
  timer_10msDB[PeriodUsbDisk_Task].Enable = ENABLE;
  timer_10msDB[PeriodUsbDisk_Task].Time = 1006;
	
	static uint8_t readyCount=0;
	
  switch( USBState )
  {
	  case USBDISK_CONNECT: 
					if (ch374_DetectOk())
					{
						 if (CH374DiskConnect() == ERR_SUCCESS)
						{
							printf( "Insert Udisk........\r\n" );
							HalLedBlink(HAL_LED_1, 2, 30, 400);

							USBState = USBDISK_READY;
							Timer_Event_Set( &timer_10msDB[PeriodUsbDisk_Task],  20,  ENABLE,  0, USBDiskHandle );
						}
					}
					else
					{
						printf("CH374T Detect Failed\r\n");
					}
					break; 
				
    case USBDISK_READY:
					if (CH374DiskReady( ) == ERR_SUCCESS)
					{
							printf( "DiskReady........\r\n" );
							USBState = USBDISK_HUBDECT;
						  Timer_Event_Set( &timer_10msDB[PeriodUsbDisk_Task],  10,  ENABLE,  0, USBDiskHandle );
					}
					else
					{
						readyCount++;
						if( readyCount > 5 )
						{
							printf( "Maybe USB-HUB Insert...\r\n" );
							readyCount = 0;
							USBState = USBDISK_HUBDECT;
							Timer_Event_Set( &timer_10msDB[PeriodUsbDisk_Task],  10,  ENABLE,  0, USBDiskHandle );
						}
					}
			    break; 
					
    case  USBDISK_HUBDECT:
					if(CH374DiskStatus <= DISK_CONNECT)
					{
						USBState = USBDISK_CONNECT;/* ֧��USB-HUB */
						Timer_Event_Set( &timer_10msDB[PeriodUsbDisk_Task],  10,  ENABLE,  0, USBDiskHandle );  
					}
					else
					{
						USBState = USBDISK_HANDLE;
						Timer_Event_Set( &timer_10msDB[PeriodUsbDisk_Task],  10,  ENABLE,  0, USBDiskHandle );  
					}
					break;
					
					
	  case  USBDISK_HANDLE://����·�������
					{
						/* ��ѯ������������ */
						{
							uint32_t uiSize = 0;

							ucErr = ch374_DiskSize(&uiSize);
							/* ��ʾΪ��MBΪ��λ������*/
							printf( "Disk Size = %u MB \r\n", uiSize >> 11);  
							StopIfError(ucErr);
						}


						/* �г���Ŀ¼�µ��ļ� */
						{  
							/* û���ҵ��ļ�, �г���Ŀ¼�����е��ļ����ļ��� */
							pCodeStr = "/*";
							printf( "List all files %s\r\n", pCodeStr);
							
							/* �û������Լ���������������ļ��� */
							for (i = 0; i < 254; i++ )
							{  
								/* i ��ö����ţ����Դ�0��2147483647 */
								ucErr = ch374_FindFile(pCodeStr, i, strFileName);
								if (ucErr == ERR_MISS_FILE ) 
								{
									break;  /* ��Ҳ��������ƥ����ļ�,�Ѿ�û��ƥ����ļ��� */
								}
								if (ucErr == ERR_FOUND_NAME )
								{  
									/* ��������ͨ�����ƥ����ļ���,�ļ�����������·������������� */
									/* ��ʾ��ź���������ƥ���ļ���������Ŀ¼�� */
									printf( "  %03d#: %s\r\n", i, strFileName);  
									continue;  /* ����������һ��ƥ����ļ���,�´�����ʱ��Ż��1 */
								}
								else 
								{  
									/* ���� */
									StopIfError(ucErr);
									break;
								}
							}
						}

						/* ���ļ� */
						ucErr = ch374_OpenFile("/LBB.CSV");	
						if (ucErr == ERR_SUCCESS)
						{  
							uiTotalCount = sizeof(ucaDataBuf);  /* ׼����ȡ�ܳ��� */
							printf( "���ļ��ж�����ǰ%d���ַ���:\r\n", uiTotalCount );
							
							while (uiTotalCount) 
							{  
								ucErr = ch374_ReadFile(ucaDataBuf, sizeof(ucaDataBuf), &iReadCount);
								StopIfError(ucErr);
								
								uiTotalCount -= iReadCount;  /* ����,��ȥ��ǰʵ���Ѿ��������ַ��� */
								for (i = 0; i < iReadCount; i++)
								{
									printf( "%c", (char)ucaDataBuf[i]);  /* ��ʾ�������ַ� */
								}

								/* ʵ�ʶ������ַ�������Ҫ��������ַ���,˵���Ѿ����ļ��Ľ�β */
								if (iReadCount < sizeof(ucaDataBuf))
								{
									printf( "\r\n" );
									break;
								}				
							}
						}
						else
						{
								/* ����һ�����ļ� */
								printf("Create file\r\n");
								ucErr = ch374_CreateFile("/LBB.CSV");	/* �½��ļ�����,����ļ��Ѿ���������ɾ�������½� */
								StopIfError(ucErr);
						}

						/* ����������ӵ��ļ�β���������ƶ��ļ�ָ�룬�β�ȡ 0xffffffff */
						ch374_SeekFile(0xffffffff);
						
					  I2C1_PCF8563_GetUTCTime( &UTCC);
						memset(ucaDataBuf, 0x00, 128);
						sprintf((char *)ucaDataBuf,"Date,%d-%d-%d %d:%d:%d\r\n ",UTCC.year,UTCC.month,UTCC.day,UTCC.hour,UTCC.minutes,UTCC.seconds);
						ucErr = ch374_WriteFile(ucaDataBuf, strlen((char *)ucaDataBuf));	/* ���ļ�ĩβд��ղŶ�ȡ������ */
						
						StopIfError(ucErr);

							/* д�ļ� */
//							printf("Write file\r\n");
//							pCodeStr = "  ����һ����дU���ļ��ĳ���";
//							ucErr = ch374_WriteFile((uint8_t *)pCodeStr, strlen((char *)pCodeStr));	/* д���ݵ��ļ� */
//							StopIfError(ucErr);
							
							/* �޸��ļ������� */
						printf( "Modify File Create Date\r\n" );
						ch374_ModifyFileDate(2010, 1, 8);
						StopIfError(ucErr);

						printf( "Close\r\n" );
						ucErr = ch374_CloseFile();
						StopIfError(ucErr);
				
						USBState = USBDISK_REMOVE;
						Timer_Event_Set( &timer_10msDB[PeriodUsbDisk_Task],  10,  ENABLE,  0, USBDiskHandle ); 
						}
						break;
						
		case USBDISK_REMOVE:
				 if(CH374DiskConnect() != ERR_SUCCESS) 
				 { 
					 printf( "Remove UDisk.....\r\n" );
					 USBState = USBDISK_CONNECT;
					 Timer_Event_Set( &timer_10msDB[PeriodUsbDisk_Task],  10,  ENABLE,  0, USBDiskHandle ); 
				 }
				 break;
	  default: break;
	}
}

/*********************************************************************
 * @fn      static void StopIfError(uint8_t _ucErr)
 *
 * @brief   ������״̬,�����������ʾ������벢ͣ��
 *          
 * @param   none        
 *
 * @return  none
 */
static void StopIfError(uint8_t _ucErr)
{
	if (_ucErr == ERR_SUCCESS) 
	{
		return;  /* �����ɹ� */
	}
	
	printf( "Error: %02X\r\n", _ucErr );  /* ��ʾ���� */
//	
//	/* 
//		���������,Ӧ�÷����������Լ�CH374DiskStatus״̬,
//		�������CH374DiskConnect��ѯ��ǰU���Ƿ�����,���U���ѶϿ���ô�����µȴ�U�̲����ٲ���,
// 		��������Ĵ�����:
//   		1������һ��CH374DiskReady,�ɹ����������,����Open,Read/Write��,
//   			��CH374DiskReady�л��Զ�����CH374DiskConnect�������������
//   		2�����CH374DiskReady���ɹ�,��ôǿ�н�CH374DiskStatus��ΪDISK_CONNECT״̬,
//   			Ȼ���ͷ��ʼ����(�ȴ�U������CH374DiskConnect��CH374DiskReady��) 
//   	*/
//   	
//   	bsp_LedOff(LED_RD_WR);  /* ��������U�� */
//   	
//   	/* LED��˸ */
//	while (1)
//	{
//		bsp_LedOn(LED_ERR);
//		bsp_DelayMS(100);
//		bsp_LedOff(LED_ERR);
//		bsp_DelayMS(100);
//	}
}

