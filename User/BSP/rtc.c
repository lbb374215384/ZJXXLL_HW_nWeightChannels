/* Includes ------------------------------------------------------------------*/
#include "rtc.h"
#include "Global.h"
#include "TimeEVent.h"
#include "usart123.h"
#include "gpio.h"
#include "Ec20.h"
#include "Variable.h"

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
RTC_HandleTypeDef RtcHandle;

/*********************************************************************
 * EXTERNAL VARIABLES
 */
 
/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
* LOCAL VARIABLES
*/

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/* Private function prototypes -----------------------------------------------*/


static uint8_t monthLength( uint8_t lpyr, uint8_t mon );

	
void RTC_Init( void)
{

  /*##-1- Configure the RTC peripheral #######################################*/
  RtcHandle.Instance = RTC;

  /* Configure RTC prescaler and RTC data registers */
  /* RTC configured as follows:
      - Hour Format    = Format 24
      - Asynch Prediv  = Value according to source clock
      - Synch Prediv   = Value according to source clock
      - OutPut         = Output Disable
      - OutPutPolarity = High Polarity
      - OutPutType     = Open Drain */
  RtcHandle.Init.HourFormat     = RTC_HOURFORMAT_24;
  RtcHandle.Init.AsynchPrediv   = RTC_ASYNCH_PREDIV;
  RtcHandle.Init.SynchPrediv    = RTC_SYNCH_PREDIV;
  RtcHandle.Init.OutPut         = RTC_OUTPUT_DISABLE;
  RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  RtcHandle.Init.OutPutType     = RTC_OUTPUT_TYPE_OPENDRAIN;
  
  if(HAL_RTC_Init(&RtcHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
		printf("RTC_Init\r\n");
  }
	
	App_SysSetUtcTime( "\r\n+CCLK: \"00/01/01,00:00:00+32\"\0");
}


/*********************************************************************
 * @fn      osal_ConvertUTCTime
 *
 * @brief   Converts UTCTime to UTCTimeStruct
 *
 * @param   tm - pointer to breakdown struct
 *
 * @param   secTime - number of seconds since 0 hrs, 0 minutes,
 *          0 seconds, on the 1st of January 1970 UTC
 *
 * @return  none
 */
void RTC_ConvertUTCTime( UTCTimeStruct *tm, UTCTime secTime )
{
	UTCTimeStruct uT;
	uint16_t C,Y;
  // calculate the time less than a day - hours, minutes, seconds
  {
    uint32_t day = secTime % DAY;
    tm->seconds = day % 60UL;
    tm->minutes = (day % 3600UL) / 60UL;
    tm->hour = day / 3600UL;
  }

  // Fill in the calendar - day, month, year
  {
    uint16_t numDays = secTime / DAY;
    tm->year = BEGYEAR;
    while ( numDays >= YearLength( tm->year ) )
    {
      numDays -= YearLength( tm->year );
      tm->year++;
    }

    tm->month = 0;
    while ( numDays >= monthLength( IsLeapYear( tm->year ), tm->month ) )
    {
      numDays -= monthLength( IsLeapYear( tm->year ), tm->month );
      tm->month++;
    }

    tm->day = numDays+1;// Convert to human numbers
		tm->month++;        // Convert to human numbers
  }
	
	/***************计算星期***************/
	memcpy(&uT, tm, sizeof(UTCTimeStruct));
	
  //uT.month += 1;  // Convert to human numbers
  //uT.day += 1;
	if(uT.month==1||uT.month==2)//
  {
    uT.year--;
    uT.month+=12;
  }
  C=uT.year/100;
  Y=uT.year-C*100;

  uT.week=(C/4)-2*C+(Y+Y/4)+(13*(uT.month+1)/5)+uT.day-1;
  while(uT.week<0)
  {
    uT.week+=7;
  }
  uT.week %=7;
		
	if( uT.week )
		tm->week = uT.week;
	else
		tm->week = 7;
}

/*********************************************************************
 * @fn      monthLength
 *
 * @param   lpyr - 1 for leap year, 0 if not
 *
 * @param   mon - 0 - 11 (jan - dec)
 *
 * @return  number of days in specified month
 */
static uint8_t monthLength( uint8_t lpyr, uint8_t mon )
{
  uint8_t days = 31;

	if ( mon == 1 ) // feb
  {
		days = ( 28 + lpyr );
  }
  else
  {
    if ( mon > 6 ) // aug-dec
    {
      mon--;
    }

    if ( mon & 1 )
    {
      days = 30;
    }
  }

	return ( days );
}

/*********************************************************************
 * @fn      RTC_ConvertUTCSecs
 *
 * @brief   Converts a UTCTimeStruct to UTCTime
 *
 * @param   tm - pointer to provided struct
 *
 * @return  number of seconds since 00:00:00 on 01/01/1970 (UTC)
 */
UTCTime RTC_ConvertUTCSecs( UTCTimeStruct *tm )
{
  uint32_t seconds;

  /* Seconds for the partial day */
  seconds = (((tm->hour * 60UL) + tm->minutes) * 60UL) + tm->seconds;

  /* Account for previous complete days */
  {
    /* Start with complete days in current month */
    uint16_t days = tm->day-1;

    /* Next, complete months in current year */
    {
      int8_t month = tm->month-1;
      while ( --month >= 0 )
      {
        days += monthLength( IsLeapYear( tm->year ), month );
      }
    }

    /* Next, complete years before current year */
    {
      uint16_t year = tm->year;
      while ( --year >= BEGYEAR )
      {
        days += YearLength( year );
      }
    }

    /* Add total seconds before partial day */
    seconds += (days * DAY);
  }
	
  return ( seconds );
}

/***************************************************************************************************
 * @fn      App_SysSetUtcTime()
 *
 * @brief   Set the OSAL UTC Time. UTC rollover is: 06:28:16 02/07/2136
 *
 * @param   pBuf - pointer to time parameters
 *
 * @return  None
*/
UTCTime App_SysSetUtcTime( char *pBuf )// 4 byte+hour + min + sec + mon + day +year(2byte)
{
	UTCTimeStruct utc;
	RTC_DateTypeDef  sdatestructure;
  RTC_TimeTypeDef  stimestructure;
//  RTC_DateTypeDef date;
//  RTC_TimeTypeDef time;
  UTCTime utcSecs=0;
	
  if ( memcmp("\r\n+CCLK: ",pBuf,7) == NULL  )
  {
    /* Skip past UTC time flag +CCLK: "13/08/19,11:10:57+32"*/
    pBuf += 10;

    /* Get time and date parameters */
    utc.year = strtoul(pBuf,NULL, 10) + 2000U;
    pBuf += 3;
    utc.month = strtoul(pBuf,NULL, 10);
    pBuf += 3;
    utc.day = strtoul(pBuf,NULL, 10);
    pBuf += 3;
    utc.hour = strtoul(pBuf,NULL, 10);
    pBuf += 3;
    utc.minutes = strtoul(pBuf,NULL, 10);
    pBuf += 3;
    utc.seconds = strtoul(pBuf,NULL, 10);

		
    utcSecs = RTC_ConvertUTCSecs( &utc);
		utcSecs += 28800U;
	  RTC_ConvertUTCTime( &utc,utcSecs );
 
		/*##-1- Configure the Date #################################################*/
		/* Set Date: Tuesday February 18th 2014 */
		sdatestructure.Year =  utc.year-2000;
		sdatestructure.Month = utc.month;
		sdatestructure.Date =  utc.day;
		sdatestructure.WeekDay = utc.week;
		if(HAL_RTC_SetDate(&RtcHandle,&sdatestructure,RTC_FORMAT_BIN) != HAL_OK)
		{
			Error_Handler(); 
		} 
		
		/*##-2- Configure the Time #################################################*/
		stimestructure.Hours = utc.hour;
		stimestructure.Minutes = utc.minutes;
		stimestructure.Seconds = utc.seconds;
		stimestructure.TimeFormat = RTC_HOURFORMAT12_AM;
		stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE ;
		stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;
		if(HAL_RTC_SetTime(&RtcHandle,&stimestructure,RTC_FORMAT_BIN) != HAL_OK)
		{
			Error_Handler(); 
		}  
  }

  return utcSecs;
}

/***************************************************************************************************
 * @fn      App_SysGetUtcTime
 *
 * @brief   Get the UTC time
 *
 * @param   None
 *
 * @return  32-bit and Parsed UTC time
 */
UTCTime App_SysGetUTCSecs( uint8_t source )
{
	
	RTC_DateTypeDef date;
	RTC_TimeTypeDef time;
	UTCTime utcSecs = 0;
	UTCTimeStruct utc;

	if( source )//本地时钟获取
	{
    HAL_RTC_GetTime(&RtcHandle, &time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&RtcHandle, &date, RTC_FORMAT_BIN);
		
		utc.year = date.Year+2000;
		utc.month = date.Month;
		utc.day = date.Date;
		utc.hour = time.Hours;
		utc.minutes = time.Minutes;
		utc.seconds = time.Seconds;
		utc.week = date.WeekDay;
		utcSecs = RTC_ConvertUTCSecs( &utc);
		utcSecs -= 28800U;
		
		return utcSecs;
	}
	else
	{

	}
	return 0;	
}



///***************************************************************************************************
// * @fn      void sort_alarm_info( timeItem_t *timeitem , uint8_t size)
// *
// * @brief   定时数据排序
// *
// * @param   timeItem_t *timeitem , uint8_t size
// *
// * @return  None
//*/
//void sort_alarm_info( timeItem_t *timeitem , uint8_t size)
//{
//  uint8_t i, j;
//	timeItem_t temp;
//	uint32_t t1sec, t2sec;
//	
//  for (i =0; i < size - 1; i++)
//  {
//    for( j = 0; j < size-1-i; j++)
//		{
//			t1sec = timeitem[j].Hour*3600+timeitem[j].Min*60;
//			t2sec = timeitem[j+1].Hour*3600+timeitem[j+1].Min*60;
//      if( t1sec > t2sec ) 
//	    {
//				temp = timeitem[j];
//				timeitem[j] = timeitem[j+1];
//				timeitem[j+1] = temp;
//      }
//   }
//  }
//}
///**
//  * @brief  void insert_alarm_info(alarm_ctrl_t *alarm_ctrl, alarm_info_t *alarm_info)
//  * @param  alarm_ctrl_t *alarm_ctrl  alarm_info_t *alarm_info
//  * @retval None
//  */

///***************************************************************************************************
// * @fn      void insert_alarm_info(alarm_ctrl_t *alarm_ctrl, timeItem_t *timeitem )
// *
// * @brief   定时闹钟插入
// *
// * @param   alarm_ctrl_t *alarm_ctrl, timeItem_t *timeitem 
// *
// * @return  None
//*/
//void insert_alarm_info(alarm_ctrl_t *alarm_ctrl, timeItem_t *timeitem )
//{
//  uint8_t i;
//  uint8_t size = alarm_ctrl->size;
// 
//	if( size >= TimeTaskNum ) 
//    return;

//  for (i = 0; i < size; i++) 
//	{
//    if( alarm_ctrl->list[i].Hour == timeitem->Hour && alarm_ctrl->list[i].Min == timeitem->Min ) 
//		return;
//	}

//	alarm_ctrl->list[i].EN = timeitem->EN;
//  alarm_ctrl->list[i].Hour = timeitem->Hour;
//	alarm_ctrl->list[i].Min = timeitem->Min;
//  alarm_ctrl->size++;
//  sort_alarm_info(alarm_ctrl->list, alarm_ctrl->size);
//}



///***************************************************************************************************
// * @fn      void insert_alarm_info(alarm_ctrl_t *alarm_ctrl, timeItem_t *timeitem )
// *
// * @brief   定时闹钟删除
// *
// * @param   alarm_ctrl_t *alarm_ctrl, timeItem_t *timeitem 
// *
// * @return  None
//*/
//void delete_alarm_info(alarm_ctrl_t *alarm_ctrl, uint32_t detsec )
//{
//  uint8_t i, point, flag = 0;
//	uint32_t t1sec;
//  uint8_t size = alarm_ctrl->size;
//	
//  //查找删除的时间点是否存在
//  for (i = 0; i < size; i++)
//	{
//		t1sec = alarm_ctrl->list[i].Hour*3600+alarm_ctrl->list[i].Min*60;
//		if( t1sec == detsec ) 
//		{
//			flag = 1;
//			point = i;
//			break;
//		}
//  }
//	
//  if (flag) 
//	{
//		//删除的数据在末尾
//    if (point + 1 == size) 
//		{
//			alarm_ctrl->size--; 
//			return;
//    } 
//    //删除的数据不在末尾
//		alarm_ctrl->size--;
//    for (i = point; i < size; i++ ) 
//		{
//			alarm_ctrl->list[i].EN = alarm_ctrl->list[i+1].EN;
//			alarm_ctrl->list[i].Hour = alarm_ctrl->list[i+1].Hour;
//			alarm_ctrl->list[i].Min = alarm_ctrl->list[i+1].Min;
//    }
//  }
//}
