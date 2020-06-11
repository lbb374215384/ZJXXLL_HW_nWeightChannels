/******************** (C) COPYRIGHT 2017 **************************
 * �ļ���  ��main.c
 * ����    ����3.5.0�汾���Ĺ���ģ�塣         
 * ��汾  ��ST3.5.0
**********************************************************************************/
#include "stm32f10x.h"
#include "TouchPanel.h"
#include "Systick.h"
#include "GLCD.h"
#include "usart1.h"
#include "usart2.h"
#include "led.h" 
#include "Time_test.h"
#include "Global.h"
#include "TimeEvent.h"
#include "LcmUI.h"
#include "Variable.h"
#include "UART_485.h"
#include "UART_cc2530.h"
#include "spi_flash.h"
#include "tempad.h"
#include "Nv_eeprom.h"

//typedef enum { FAILED = 0, PASSED = !FAILED} TestStatus;


void IWDG_Config(void);

/* 
 * ��������main
 * ����  : ������
 * ����  ����
 * ���  : ��
 */
int main(void)
	{
			/* ����ϵͳʱ��Ϊ 72M */  
			SystemInit();
			/* systick ��ʼ�� */
			SysTick_Init();

			RTC_Configuartion();
			/* ������ʼ�� */  
			Variable_Init();

			/* led �˿����� */ 
			LED_GPIO_Config();
			BUZZER_GPIO_Config();

			/* TIM2 ��ʱ���� */
			TIM2_NVIC_Configuration();
			TIM2_Configuration();
			//TIM3_Configuration();
			START_TIME;//TIM2��ʼ��ʱ

			USART1_Config();//��CC2530ͨѶ

			SPI_FLASH_Init();//2M����flash W25X16��ʼ��
#if defined( Nv_eeprom_item )			
			InitNV_EEpromTask();
#endif

			/* ��������LCD����ʼ�� */
			TP_Init();
			LCD_Initializtion();
			//TouchPanel_Calibrate();
  
			Init_Timer_Event();//��ʱ�����¼���ʼ��
			
			/*****************��������****************
			LCD_DrawJPGImageFromW25X16(0,0, 0x00000000);
			LCD_DrawJPGImageFromW25X16(0,0, 0x00030000);
			LCD_DrawJPGImageFromW25X16(0,0, 0x00060000);
			LCD_DrawJPGImageFromW25X16(0,0, 0x00090000);
			LCD_DrawJPGImageFromW25X16(0,0, 0x000C0000);
			LCD_DrawJPGImageFromW25X16(0,0, 0x000F0000);*/	
			
			LCD_Clear( BG_Color );
			ZNetData.Nowtime.year =2018;
			ZNetData.Nowtime.month =2;
			ZNetData.Nowtime.day = 21;
			ZNetData.Nowtime.hour =17;
			ZNetData.Nowtime.minutes =30;
			ZNetData.Nowtime.seconds =40;
			ZNetData.Nowtime.week =4;
			
			RTC_SetCounter(RTC_ConvertUTCSecs( & ZNetData.Nowtime ));
			TFTLCM_DayTime( &ZNetData.Nowtime );
			
			ZNetData.Znet_status=0;
			TFTLCM_HomePage( SwitchMsg[tTft_Msg.m_PageInfor.m_HomePg], tTft_Msg.m_PageInfor.m_HomePg, & ZNetData.Nowtime, 25, ZNetData.Znet_status);

			TP_IRQ_Config();
			LED(ON);
			
#if defined(WDT)
  IWDG_Config( );
#endif 

  while(1)	
  {
#if defined(WDT)
				IWDG_ReloadCounter( );
#endif 
				Timer_Event_Handle( ); //��ʱ������
			
#if defined(WDT)
				IWDG_ReloadCounter( );
#endif 
			ZigbeeUART_Readqueue_Handle( );
			
#if defined(WDT)
				IWDG_ReloadCounter( );
#endif 
				ZigbeeData_Handle( );  //zigbee���ݴ��� 
  }
}

/* 
 * ��������WDG_Config
 * ����  : 
 * ����  ����
 * ���  : ��
 */
void IWDG_Config(void)
{
	if( RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
	{
		RCC_ClearFlag();
	}
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	IWDG_SetPrescaler(IWDG_Prescaler_32);
	IWDG_SetReload(40000/64);
 IWDG_ReloadCounter();
	IWDG_Enable();
}
/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/


