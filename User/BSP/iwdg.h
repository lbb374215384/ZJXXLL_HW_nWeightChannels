/**************************************************************************************************
  Filename:       IWDG.h
  Revised:        $Date: 2018-10-8  $
  Revision:       $Revision: V1.0  $
  User:           $User:Asong      $
  Description:    IWDG config

**************************************************************************************************/
#ifndef _IWDG_H
#define _IWDG_H

#ifdef __cplusplus
extern "C"
{
#endif
  
/*********************************************************************/
#include "stm32f0xx_hal.h"

extern IWDG_HandleTypeDef    IwdgHandle;
extern TIM_HandleTypeDef     Input_Handle;


extern void IWDG_Config(void);




/*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif
