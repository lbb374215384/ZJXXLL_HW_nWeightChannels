/*
*********************************************************************************************************
*	                                  
*	模块名称 : CH374 U盘文件操作例程
*	文件名称 : ch374_udisk.c
*	版    本 : V1.0
*	说    明 : 	U盘文件读写示例程序，适用于STM32。
*				(1) 用户插入U盘后，首先显示磁盘容量。
*				(2) 打开U盘中的/STM32/armfly.txt文件，并显示前600个字符。如果找不到armfly.txt文件, 那么将显示
*					根目录下所有的文件名。
*				(3) 在根目录下创建一个新文件NEWFILE.TXT，并写入一个字符串，之后修改文件日期。
*				
*				三个LED用于监控演示程序的进度。
*				(1) 当U盘插入后点亮LED1，拔出时LED1熄灭
*				(2) 当读写U盘时点亮LED2，读取完毕时LED2熄灭 
*				(3) 当读写出错时，LED3闪烁
*
*				CH374的INT#引脚采用查询方式处理。
*
*				以字节为单位读写U盘文件,读写速度较扇区模式慢。
*	修改记录 :
*		版本号  日期       作者    说明
*		v1.0    2011-01-11 armfly  ST固件库版本为V3.4.0版本。
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


#define DEMO_WRITE_EN	1		/* 1表示写测试使能 */


#define LED_INSERT	1
#define LED_RD_WR	2
#define LED_ERR		3

uint8_t strFileName[256];	/* 存储路径+文件名 */
uint8_t ucaDataBuf[128];	/* 存储读出的数据 */

static void StopIfError(uint8_t _ucErr);

usbStates_t USBState = USBDISK_CONNECT;


/*********************************************************************
 * @fn      void USBDiskHandle( void )
 *
 * @brief   U盘读取 模块状态机
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
	int32_t iReadCount;			/* 实际读取的字节数 */
	
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
						USBState = USBDISK_CONNECT;/* 支持USB-HUB */
						Timer_Event_Set( &timer_10msDB[PeriodUsbDisk_Task],  10,  ENABLE,  0, USBDiskHandle );  
					}
					else
					{
						USBState = USBDISK_HANDLE;
						Timer_Event_Set( &timer_10msDB[PeriodUsbDisk_Task],  10,  ENABLE,  0, USBDiskHandle );  
					}
					break;
					
					
	  case  USBDISK_HANDLE://链接路由器完成
					{
						/* 查询磁盘物理容量 */
						{
							uint32_t uiSize = 0;

							ucErr = ch374_DiskSize(&uiSize);
							/* 显示为以MB为单位的容量*/
							printf( "Disk Size = %u MB \r\n", uiSize >> 11);  
							StopIfError(ucErr);
						}


						/* 列出根目录下的文件 */
						{  
							/* 没有找到文件, 列出根目录下所有的文件和文件夹 */
							pCodeStr = "/*";
							printf( "List all files %s\r\n", pCodeStr);
							
							/* 用户可以自己定义搜索的最大文件数 */
							for (i = 0; i < 254; i++ )
							{  
								/* i 是枚举序号，可以从0到2147483647 */
								ucErr = ch374_FindFile(pCodeStr, i, strFileName);
								if (ucErr == ERR_MISS_FILE ) 
								{
									break;  /* 再也搜索不到匹配的文件,已经没有匹配的文件名 */
								}
								if (ucErr == ERR_FOUND_NAME )
								{  
									/* 搜索到与通配符相匹配的文件名,文件名及其完整路径在命令缓冲区中 */
									/* 显示序号和搜索到的匹配文件名或者子目录名 */
									printf( "  %03d#: %s\r\n", i, strFileName);  
									continue;  /* 继续搜索下一个匹配的文件名,下次搜索时序号会加1 */
								}
								else 
								{  
									/* 出错 */
									StopIfError(ucErr);
									break;
								}
							}
						}

						/* 读文件 */
						ucErr = ch374_OpenFile("/LBB.CSV");	
						if (ucErr == ERR_SUCCESS)
						{  
							uiTotalCount = sizeof(ucaDataBuf);  /* 准备读取总长度 */
							printf( "从文件中读出的前%d个字符是:\r\n", uiTotalCount );
							
							while (uiTotalCount) 
							{  
								ucErr = ch374_ReadFile(ucaDataBuf, sizeof(ucaDataBuf), &iReadCount);
								StopIfError(ucErr);
								
								uiTotalCount -= iReadCount;  /* 计数,减去当前实际已经读出的字符数 */
								for (i = 0; i < iReadCount; i++)
								{
									printf( "%c", (char)ucaDataBuf[i]);  /* 显示读出的字符 */
								}

								/* 实际读出的字符数少于要求读出的字符数,说明已经到文件的结尾 */
								if (iReadCount < sizeof(ucaDataBuf))
								{
									printf( "\r\n" );
									break;
								}				
							}
						}
						else
						{
								/* 创建一个新文件 */
								printf("Create file\r\n");
								ucErr = ch374_CreateFile("/LBB.CSV");	/* 新建文件并打开,如果文件已经存在则先删除后再新建 */
								StopIfError(ucErr);
						}

						/* 将新数据添加到文件尾部，可以移动文件指针，形参取 0xffffffff */
						ch374_SeekFile(0xffffffff);
						
					  I2C1_PCF8563_GetUTCTime( &UTCC);
						memset(ucaDataBuf, 0x00, 128);
						sprintf((char *)ucaDataBuf,"Date,%d-%d-%d %d:%d:%d\r\n ",UTCC.year,UTCC.month,UTCC.day,UTCC.hour,UTCC.minutes,UTCC.seconds);
						ucErr = ch374_WriteFile(ucaDataBuf, strlen((char *)ucaDataBuf));	/* 在文件末尾写入刚才读取的数据 */
						
						StopIfError(ucErr);

							/* 写文件 */
//							printf("Write file\r\n");
//							pCodeStr = "  这是一个读写U盘文件的程序";
//							ucErr = ch374_WriteFile((uint8_t *)pCodeStr, strlen((char *)pCodeStr));	/* 写数据到文件 */
//							StopIfError(ucErr);
							
							/* 修改文件的日期 */
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
 * @brief   检查操作状态,如果错误则显示错误代码并停机
 *          
 * @param   none        
 *
 * @return  none
 */
static void StopIfError(uint8_t _ucErr)
{
	if (_ucErr == ERR_SUCCESS) 
	{
		return;  /* 操作成功 */
	}
	
	printf( "Error: %02X\r\n", _ucErr );  /* 显示错误 */
//	
//	/* 
//		遇到错误后,应该分析错误码以及CH374DiskStatus状态,
//		例如调用CH374DiskConnect查询当前U盘是否连接,如果U盘已断开那么就重新等待U盘插上再操作,
// 		建议出错后的处理步骤:
//   		1、调用一次CH374DiskReady,成功则继续操作,例如Open,Read/Write等,
//   			在CH374DiskReady中会自动调用CH374DiskConnect，不必另外调用
//   		2、如果CH374DiskReady不成功,那么强行将CH374DiskStatus置为DISK_CONNECT状态,
//   			然后从头开始操作(等待U盘连接CH374DiskConnect，CH374DiskReady等) 
//   	*/
//   	
//   	bsp_LedOff(LED_RD_WR);  /* 结束操作U盘 */
//   	
//   	/* LED闪烁 */
//	while (1)
//	{
//		bsp_LedOn(LED_ERR);
//		bsp_DelayMS(100);
//		bsp_LedOff(LED_ERR);
//		bsp_DelayMS(100);
//	}
}

