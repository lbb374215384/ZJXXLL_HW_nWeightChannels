/**************************************************************************************************
  Filename:       rtc.h
  Revised:        $Date: 2018-10-8  $
  Revision:       $Revision: V1.0  $
  User:           $User:Asong      $
  Description:    rtc ��utcʱ�����

**************************************************************************************************/
#ifndef _RTC_H
#define _RTC_H


#ifdef __cplusplus
extern "C"
{
#endif
/*********************************************************************/
  
#include "stm32f0xx_hal.h"
#include "Global.h"
	
	
#define RTC_CLOCK_SOURCE_LSI
/*#define RTC_CLOCK_SOURCE_LSE*/

#ifdef RTC_CLOCK_SOURCE_LSI
#define RTC_ASYNCH_PREDIV    0x7F
#define RTC_SYNCH_PREDIV     0x0130
#endif

#ifdef RTC_CLOCK_SOURCE_LSE
#define RTC_ASYNCH_PREDIV  0x7F
#define RTC_SYNCH_PREDIV   0x00FF
#endif



#define	IsLeapYear(yr)	(!((yr) % 400) || (((yr) % 100) && !((yr) % 4)))
#define	YearLength(yr)	((uint16_t)(IsLeapYear(yr) ? 366 : 365))

#define	BEGYEAR  1970//2000     //  UTC started at 00:00:00 January 1, 2000
#define	DAY      86400UL       // 24 hours * 60 minutes * 60 seconds


extern RTC_HandleTypeDef RtcHandle;

extern void RTC_Init(void);

extern UTCTime RTC_ConvertUTCSecs( UTCTimeStruct *tm );
extern void RTC_ConvertUTCTime( UTCTimeStruct *tm, UTCTime secTime );
extern UTCTime App_SysSetUtcTime( char *pBuf );
extern UTCTime App_SysGetUTCSecs( uint8_t source );

//extern void sort_alarm_info( timeItem_t *timeitem , uint8_t size);
//extern void insert_alarm_info(alarm_ctrl_t *alarm_ctrl, timeItem_t *timeitem );
//extern void delete_alarm_info(alarm_ctrl_t *alarm_ctrl, uint32_t detsec );

/*********************************************************************/
#ifdef __cplusplus
}
#endif

#endif
