/**************************************************************************************************
  Filename:       time6.h
  Revised:        $Date: 2018-10-8  $
  Revision:       $Revision: V1.0  $
  User:           $User:Asong      $
  Description:    time6 config

**************************************************************************************************/
#ifndef _TIME6_H
#define _TIME6_H

#ifdef __cplusplus
extern "C"
{
#endif
  
/*********************************************************************/
#include "stm32f0xx_hal.h"
#include "Global.h"


extern TIM_HandleTypeDef    Tim6Handle;
extern TIM_HandleTypeDef    Tim3Handle;


extern HAL_StatusTypeDef TIME6BASE_Config(void);
extern HAL_StatusTypeDef Timer3_Init( uint32_t period );




/*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif
