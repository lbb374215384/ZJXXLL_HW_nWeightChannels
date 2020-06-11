/**************************************************************************************************
  Filename:       ad_temp.h
  Revised:        $Date: 2018-10-8  $
  Revision:       $Revision: V1.0  $
  User:           $User:Asong      $
  Description:    ntc温度探头和电池电压

**************************************************************************************************/
#ifndef _AD_TEMP_H
#define _AD_TEMP_H


#ifdef __cplusplus
extern "C"
{
#endif
/*********************************************************************/
  
#include "stm32f0xx_hal.h"
#include "Global.h"
	
	
#define ADC_CONVERTED_DATA_BUFFER_SIZE  ((uint32_t) 35 )   /* Size of array aADCxConvertedData[] */
	
extern ADC_HandleTypeDef   AdcHandle;

extern uint16_t aADCxConvertedData[ADC_CONVERTED_DATA_BUFFER_SIZE];

extern void ADC_Temp_Vbat_Config( void );
	
extern void ADC0_Task(void);
extern void ADC1_Task(void);	
	
	
extern uint8_t look_up_table(uint16_t *a,uint8_t ArrayLong,uint16_t Arraydata);

extern uint16_t AD_Filter( uint16_t *adArray, uint8_t rowNum, uint8_t totalRow, uint8_t  num );
/*********************************************************************/
#ifdef __cplusplus
}
#endif

#endif
