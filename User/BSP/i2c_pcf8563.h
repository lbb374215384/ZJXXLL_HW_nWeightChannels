/**************************************************************************************************
  Filename:       I2C_PCF8563.h
  Revised:        $Date: 2019-9-8  $
  Revision:       $Revision: V1.0  $
  User:           $User:Asong      $
  Description:    PCF8563 I2C ���

**************************************************************************************************/
#ifndef _I2C_PCF8563_H
#define _I2C_PCF8563_H


#ifdef __cplusplus
extern "C"
{
#endif
/*********************************************************************/
  
#include "stm32f0xx_hal.h"
#include "Global.h"
	
	
#define CTRL_REG1		0x00
#define CTRL_REG2		0x01

#define SECOND_DATA_BUF	0x02
#define MINUTE_DATA_BUF	0x03
#define HOUR_DATA_BUF	  0x04

#define DAY_DATA_BUF	 0x05
#define WEEK_DATA_BUF	 0x06
#define MONTH_DATA_BUF 0x07
#define YEAR_DATA_BUF  0x08

#define MINUTE_AE_BUF	0x09
#define HOUR_AE_BUF		0x0A
#define DAY_AE_BUF		0x0B
#define WEEK_AE_BUF		0x0C

#define CLK_FRQ_BUF		 0x0D
#define TIMER_CTRL_BUF 0x0E
#define COUNT_VAL_BUF	 0x0F

#define BcdToHex(dec)		( (((dec)/10) <<4) + ((dec)%10) )
#define HexToBcd(hex)		( (((hex)>>4) *10 ) + ((hex)%16) )

	
extern I2C_HandleTypeDef I2c1Handle;

extern void I2C1_PCF8563_Init(void);
extern void I2C1_PCF8563_SetUTCTime( UTCTimeStruct utc);
extern void I2C1_PCF8563_GetUTCTime( UTCTimeStruct *utc );
extern uint8_t I2C1_PCF8563_Get_VLstatus( void );

/*********************************************************************/
#ifdef __cplusplus
}
#endif

#endif
