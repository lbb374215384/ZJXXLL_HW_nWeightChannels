/* Includes ------------------------------------------------------------------*/
#include "Global.h"
#include "KeyScan.h"
#include "TimeEvent.h"
#include "bsp_periph.h"

/* Private define ------------------------------------------------------------*/

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
 * @fn      void KeyScan(void)
 *
 * @brief   异或校验
 *          
 * @param   uint8_t *msg_ptr  uint8_t len   
 *
 * @return  none
 */
void KeyScanTask(void)
{ 
  /********读取IO口电平***********/
	KeyBuff.OneBit.Set       = HAL_PUSH_BUTTON1();//SET键
	KeyBuff.OneBit.Increase  = HAL_PUSH_BUTTON2();//SET键
	KeyBuff.OneBit.Decrease  = HAL_PUSH_BUTTON3();//SET键

	KeyChangedBuff.allBits = LastTimeKey.allBits;
	KeyChangedBuff.allBits = KeyBuff.allBits ^ KeyChangedBuff.allBits;
  
	for(keyCount=0; keyCount<3; keyCount++)
	{
		if((KeyChangedBuff.allBits &(1<<keyCount))&&((~KeyBuff.allBits)&(1<<keyCount)))
		{
			status = KeyBuff.allBits |(1<<keyCount);
			if( status == 0xFF && KeyNum == 0x0F )
			{
				KeyNum = keyCount;
				FF_PressCount = 0;
				Flag1.Bits.PressFlutterFree = 1;//开启按下去抖检测
			}
		}	
		else if((KeyChangedBuff.allBits &(1<<keyCount))&&(KeyBuff.allBits&(1<<keyCount)))
		{
			if( KeyNum == keyCount )
			{
				 FF_ReleaseCount = 0;
				 Flag1.Bits.ReleaseFlutterFree = 1;//开启释放去抖检测
			}
		}
	}
  LastTimeKey.allBits = KeyBuff.allBits;
	
  KeyPressHandler();
	KeyLongPressHandler();
	KeyReleaseHandler();
}

/*********************************************************************
 * @fn      void KeyPressHandler(void)
 *
 * @brief   按键按下处理
 *          
 * @param   none
 *
 * @return  none
 */
void KeyPressHandler(void)
{
   if( FlagKeyPress.allBits )
   {
		 
		 if( FlagKeyPress.OneBit.Set == 1 )//按-
		 {
			 printf("Press1\r\n");
			 HAL_NVIC_SystemReset();

		 } 
		 if( FlagKeyPress.OneBit.Decrease == 1 )//按-
		 {
       printf("Press2\r\n");
		 }
		 
		 if( FlagKeyPress.OneBit.Increase ==  1 )//按+
		 {
		    printf("Press3\r\n");
		 }
		 
		 FlagKeyPress.allBits = 0;
	 } 
}

/*********************************************************************
 * @fn      void KeyReleaseHandler(void)
 *
 * @brief   按键释放处理
 *          
 * @param   none
 *
 * @return  none
 */
void KeyReleaseHandler(void)
{
	if( FlagKeyRelease.allBits )
	{
		Flag1.Bits.PressFlutterFree = 0;
		Flag1.Bits.ReleaseFlutterFree = 0;
		
		
		 if( FlagKeyRelease.OneBit.Set == 1 )//按SET
		 {
			 printf("Release1\r\n");
       HAL_NVIC_SystemReset();
			 printf("Release11\r\n");
		 } 
		 if( FlagKeyRelease.OneBit.Decrease == 1 )//按-
		 {
			 printf("Release2\r\n");
			  HalLedSet (Buzzer, HAL_LED_MODE_OFF); 

		 }
		 
		 if( FlagKeyRelease.OneBit.Increase ==  1 )//按+
		 {
			 HalLedSet (Buzzer, HAL_LED_MODE_OFF); 		
		   printf("Release3\r\n");
		 }

   FlagKeyRelease.allBits = 0;				
	}
}

/*********************************************************************
 * @fn      void KeyLongPressHandler(void)
 *
 * @brief   按键长按下处理
 *          
 * @param   none
 *
 * @return  none
 */
void KeyLongPressHandler(void)
{
	if( LongKeyEvt )
	{
		switch( LongKeyEvt )
		{
			case 1:printf("Long press1\r\n");break;
			case 2:printf("Long press2\r\n");break;	
		  case 3:printf("Long press3\r\n");break;
			default:break;
		}
		

		
	  LongKeyEvt = 0;
	
	}
}

/*********************************************************************
 * @fn      void KeyTimeHandler(void)
 *
 * @brief   按键定时处理
 *          
 * @param   none
 *
 * @return  none
 */
void KeyTimeHandler(void)
{
/******************按键按下去抖相关******************/
	 if( Flag1.Bits.PressFlutterFree ) //按下去抖检测
	 {
		 if((~KeyBuff.allBits)&(1<<KeyNum))
		 {
			 FF_ReleaseCount = 0;
			 FF_PressCount++;
			 if( FF_PressCount > 2U )
			 {
				 FF_PressCount = 0;
				 Flag1.Bits.PressFlutterFree = 0;
				 Flag1.Bits.LongScan = 1; //开启长按		
				 
				 tPara.LPCount1 = 0;
				 tPara.LPCount2 = 0;
				 tPara.LPCount3 = 0;
			
				 FlagKeyPress.allBits |= (1<<KeyNum);//产生按下标志
				 KeyStatus = Press; 
			 }
		 }
		 else 
		 {
			 FF_PressCount = 0;
			 Flag1.Bits.PressFlutterFree = 0;
			 KeyNum = 0x0F;
		 }
	 }
/******************按键释放去抖相关******************/ 
	 if( Flag1.Bits.ReleaseFlutterFree ) //释放去抖检测
	 {
		 if( KeyBuff.allBits & (1<<KeyNum))
		 {
			 FF_PressCount =0;
			 FF_ReleaseCount++;
			 if( FF_ReleaseCount > 4U )//50ms
			 {
				 FF_ReleaseCount = 0;
				 Flag1.Bits.ReleaseFlutterFree = 0;
				 Flag1.Bits.LongScan = 0; //关闭长按		
				 
				 //FlagLongKey.allBits = 0;
				 FlagKeyRelease.allBits |= (1<<KeyNum);//产生释放标志
				 KeyNum = 0x0F;
				 KeyStatus = Release;
			 }
		 }
		 else 
		 {
			 FF_ReleaseCount = 0;
		 }
	 }
	 
	 if( Flag1.Bits.LongScan )//长按扫描
	 {
		 if( KeyBuff.OneBit.Set == 0 && KeyBuff.OneBit.Increase && KeyBuff.OneBit.Decrease )//长按set
			{
						tPara.LPCount2 = 0;
						tPara.LPCount3 = 0;
				 
						tPara.LPCount1++;
						if( tPara.LPCount1 > 2999UL ) //3s 进入下一级菜单
						{
							// Flag1.Bits.LongScan = 0;  //长按扫描关闭
							 tPara.LPCount1 = 0;
							
							 LongKeyEvt = 1 ;          //产生长按标致
						}
				
			}
			else if( KeyBuff.OneBit.Set && KeyBuff.OneBit.Increase == 0 && KeyBuff.OneBit.Decrease)//长按+
			{
						tPara.LPCount1 = 0;
						tPara.LPCount3 = 0;
				 
						tPara.LPCount2++;
						if( tPara.LPCount2 > 2999UL ) //3s 进入下一级菜单
						{
							// Flag1.Bits.LongScan = 0;  //长按扫描关闭
							 tPara.LPCount2 = 0;
							
							 LongKeyEvt = 2 ;          //产生长按标致
						}
				 
			}
		  else if( KeyBuff.OneBit.Set && KeyBuff.OneBit.Increase && KeyBuff.OneBit.Decrease == 0)//长按-
			{
				
						tPara.LPCount1 = 0;
						tPara.LPCount2 = 0;
				 
						tPara.LPCount3++;
						if( tPara.LPCount3 > 2999UL ) //3s 进入下一级菜单
						{
							 //Flag1.Bits.LongScan = 0;  //长按扫描关闭
							 tPara.LPCount3 = 0;
							
							 LongKeyEvt = 1 ;          //产生长按标致
						}
				 
			}
		  else
		  {
				Flag1.Bits.LongScan = 0;//长按扫描关闭
				tPara.LPCount3 = 0;
				tPara.LPCount1 = 0;
				tPara.LPCount2 = 0;
				LongKeyEvt = 0;
		  }
	 }
}


