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
 * @brief   ���У��
 *          
 * @param   uint8_t *msg_ptr  uint8_t len   
 *
 * @return  none
 */
void KeyScanTask(void)
{ 
  /********��ȡIO�ڵ�ƽ***********/
	KeyBuff.OneBit.Set       = HAL_PUSH_BUTTON1();//SET��
	KeyBuff.OneBit.Increase  = HAL_PUSH_BUTTON2();//SET��
	KeyBuff.OneBit.Decrease  = HAL_PUSH_BUTTON3();//SET��

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
				Flag1.Bits.PressFlutterFree = 1;//��������ȥ�����
			}
		}	
		else if((KeyChangedBuff.allBits &(1<<keyCount))&&(KeyBuff.allBits&(1<<keyCount)))
		{
			if( KeyNum == keyCount )
			{
				 FF_ReleaseCount = 0;
				 Flag1.Bits.ReleaseFlutterFree = 1;//�����ͷ�ȥ�����
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
 * @brief   �������´���
 *          
 * @param   none
 *
 * @return  none
 */
void KeyPressHandler(void)
{
   if( FlagKeyPress.allBits )
   {
		 
		 if( FlagKeyPress.OneBit.Set == 1 )//��-
		 {
			 printf("Press1\r\n");
			 HAL_NVIC_SystemReset();

		 } 
		 if( FlagKeyPress.OneBit.Decrease == 1 )//��-
		 {
       printf("Press2\r\n");
		 }
		 
		 if( FlagKeyPress.OneBit.Increase ==  1 )//��+
		 {
		    printf("Press3\r\n");
		 }
		 
		 FlagKeyPress.allBits = 0;
	 } 
}

/*********************************************************************
 * @fn      void KeyReleaseHandler(void)
 *
 * @brief   �����ͷŴ���
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
		
		
		 if( FlagKeyRelease.OneBit.Set == 1 )//��SET
		 {
			 printf("Release1\r\n");
       HAL_NVIC_SystemReset();
			 printf("Release11\r\n");
		 } 
		 if( FlagKeyRelease.OneBit.Decrease == 1 )//��-
		 {
			 printf("Release2\r\n");
			  HalLedSet (Buzzer, HAL_LED_MODE_OFF); 

		 }
		 
		 if( FlagKeyRelease.OneBit.Increase ==  1 )//��+
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
 * @brief   ���������´���
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
 * @brief   ������ʱ����
 *          
 * @param   none
 *
 * @return  none
 */
void KeyTimeHandler(void)
{
/******************��������ȥ�����******************/
	 if( Flag1.Bits.PressFlutterFree ) //����ȥ�����
	 {
		 if((~KeyBuff.allBits)&(1<<KeyNum))
		 {
			 FF_ReleaseCount = 0;
			 FF_PressCount++;
			 if( FF_PressCount > 2U )
			 {
				 FF_PressCount = 0;
				 Flag1.Bits.PressFlutterFree = 0;
				 Flag1.Bits.LongScan = 1; //��������		
				 
				 tPara.LPCount1 = 0;
				 tPara.LPCount2 = 0;
				 tPara.LPCount3 = 0;
			
				 FlagKeyPress.allBits |= (1<<KeyNum);//�������±�־
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
/******************�����ͷ�ȥ�����******************/ 
	 if( Flag1.Bits.ReleaseFlutterFree ) //�ͷ�ȥ�����
	 {
		 if( KeyBuff.allBits & (1<<KeyNum))
		 {
			 FF_PressCount =0;
			 FF_ReleaseCount++;
			 if( FF_ReleaseCount > 4U )//50ms
			 {
				 FF_ReleaseCount = 0;
				 Flag1.Bits.ReleaseFlutterFree = 0;
				 Flag1.Bits.LongScan = 0; //�رճ���		
				 
				 //FlagLongKey.allBits = 0;
				 FlagKeyRelease.allBits |= (1<<KeyNum);//�����ͷű�־
				 KeyNum = 0x0F;
				 KeyStatus = Release;
			 }
		 }
		 else 
		 {
			 FF_ReleaseCount = 0;
		 }
	 }
	 
	 if( Flag1.Bits.LongScan )//����ɨ��
	 {
		 if( KeyBuff.OneBit.Set == 0 && KeyBuff.OneBit.Increase && KeyBuff.OneBit.Decrease )//����set
			{
						tPara.LPCount2 = 0;
						tPara.LPCount3 = 0;
				 
						tPara.LPCount1++;
						if( tPara.LPCount1 > 2999UL ) //3s ������һ���˵�
						{
							// Flag1.Bits.LongScan = 0;  //����ɨ��ر�
							 tPara.LPCount1 = 0;
							
							 LongKeyEvt = 1 ;          //������������
						}
				
			}
			else if( KeyBuff.OneBit.Set && KeyBuff.OneBit.Increase == 0 && KeyBuff.OneBit.Decrease)//����+
			{
						tPara.LPCount1 = 0;
						tPara.LPCount3 = 0;
				 
						tPara.LPCount2++;
						if( tPara.LPCount2 > 2999UL ) //3s ������һ���˵�
						{
							// Flag1.Bits.LongScan = 0;  //����ɨ��ر�
							 tPara.LPCount2 = 0;
							
							 LongKeyEvt = 2 ;          //������������
						}
				 
			}
		  else if( KeyBuff.OneBit.Set && KeyBuff.OneBit.Increase && KeyBuff.OneBit.Decrease == 0)//����-
			{
				
						tPara.LPCount1 = 0;
						tPara.LPCount2 = 0;
				 
						tPara.LPCount3++;
						if( tPara.LPCount3 > 2999UL ) //3s ������һ���˵�
						{
							 //Flag1.Bits.LongScan = 0;  //����ɨ��ر�
							 tPara.LPCount3 = 0;
							
							 LongKeyEvt = 1 ;          //������������
						}
				 
			}
		  else
		  {
				Flag1.Bits.LongScan = 0;//����ɨ��ر�
				tPara.LPCount3 = 0;
				tPara.LPCount1 = 0;
				tPara.LPCount2 = 0;
				LongKeyEvt = 0;
		  }
	 }
}

