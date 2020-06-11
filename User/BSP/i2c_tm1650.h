/**************************************************************************************************
  Filename:       I2C_TM1650.h
  Revised:        $Date: 2019-9-8  $
  Revision:       $Revision: V1.0  $
  User:           $User:Asong      $
  Description:    TM1650 I2C œ‡πÿ

**************************************************************************************************/
#ifndef _I2C_TM1650_H
#define _I2C_TM1650_H


#ifdef __cplusplus
extern "C"
{
#endif
/*********************************************************************/
//---------- Includes ----------------------------------------------------------
#include "stm32f0xx_hal.h"
#include "Global.h"


extern I2C_HandleTypeDef I2c1Handle;
//extern DMA_HandleTypeDef hdma_i2c1_tx;
	
	
extern void I2C1_TM1650_Init( void );
extern void TM1650_send( uint8_t saddr, uint8_t sdate );
extern void TM1650_SystemCmd( uint8_t light,uint8_t segMod, uint8_t WorkMod, uint8_t Onoff);

extern void write_8bit( uint8_t dat);
extern void TM1650_STOP(void);
extern void TM1650_START(void);


/*********************************************************************/
#ifdef __cplusplus
}
#endif

#endif

