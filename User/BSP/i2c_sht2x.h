/**************************************************************************************************
  Filename:       I2C_PCF8563.h
  Revised:        $Date: 2019-9-8  $
  Revision:       $Revision: V1.0  $
  User:           $User:Asong      $
  Description:    PCF8563 I2C ���

**************************************************************************************************/
#ifndef _I2C_SHT2X_H
#define _I2C_SHT2X_H


#ifdef __cplusplus
extern "C"
{
#endif
/*********************************************************************/
//---------- Includes ----------------------------------------------------------
#include "stm32f0xx_hal.h"
#include "Global.h"
//---------- Defines -----------------------------------------------------------
//  CRC

// sensor command
typedef enum{
  TRIG_T_MEASUREMENT_HM    = 0xE3, // command trig. temp meas. hold master
  TRIG_RH_MEASUREMENT_HM   = 0xE5, // command trig. humidity meas. hold master
  TRIG_T_MEASUREMENT_POLL  = 0xF3, // command trig. temp meas. no hold master
  TRIG_RH_MEASUREMENT_POLL = 0xF5, // command trig. humidity meas. no hold master
  USER_REG_W               = 0xE6, // command writing user register
  USER_REG_R               = 0xE7, // command reading user register
  SOFT_RESET               = 0xFE  // command soft reset
}etSHT2xCommand;

typedef enum {
  SHT2x_RES_12_14BIT       = 0x00, // RH=12bit, T=14bit
  SHT2x_RES_8_12BIT        = 0x01, // RH= 8bit, T=12bit
  SHT2x_RES_10_13BIT       = 0x80, // RH=10bit, T=13bit
  SHT2x_RES_11_11BIT       = 0x81, // RH=11bit, T=11bit
  SHT2x_RES_MASK           = 0x81  // Mask for res. bits (7,0) in user reg.
} etSHT2xResolution;

typedef enum {
  SHT2x_EOB_ON             = 0x40, // end of battery
  SHT2x_EOB_MASK           = 0x40, // Mask for EOB bit(6) in user reg.
} etSHT2xEob;

typedef enum {
  SHT2x_HEATER_ON          = 0x04, // heater on
  SHT2x_HEATER_OFF         = 0x00, // heater off
  SHT2x_HEATER_MASK        = 0x04, // Mask for Heater bit(2) in user reg.
} etSHT2xHeater;

// measurement signal selection
typedef enum{
  HUMIDITY,
  TEMP
}etSHT2xMeasureType;

typedef enum{
  I2C_ADR_W                = 128,   // sensor I2C address + write bit
  I2C_ADR_R                = 129    // sensor I2C address + read bit
}etI2cHeader;



extern void I2C2_SHT2x_Init(void);

extern uint8_t SHT2x_CheckCrc(uint8_t data[], uint8_t nbrOfBytes, uint8_t checksum);

extern uint8_t SHT2x_ReadUserRegister(uint8_t *pRegisterValue);

extern uint8_t SHT2x_WriteUserRegister(uint8_t *pRegisterValue);

extern uint8_t SHT2x_MeasureHM(etSHT2xMeasureType eSHT2xMeasureType, uint8_t *pMeasurand);


extern uint8_t SHT2x_SoftReset(void);

extern float SHT2x_CalcRH(uint16_t u16sRH);

extern float SHT2x_CalcTemperatureC(uint16_t u16sT);

extern uint8_t SHT2x_GetSerialNumber(uint8_t u8SerialNumber[]);

/*********************************************************************/
#ifdef __cplusplus
}
#endif

#endif

