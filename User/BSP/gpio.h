/**************************************************************************************************
  Filename:       gpio.h
  Revised:        $Date: 2018-10-8  $
  Revision:       $Revision: V1.0  $
  User:           $User:Asong      $
  Description:    IO �ܽſ���

**************************************************************************************************/
#ifndef _GPIO_H
#define _GPIO_H


#ifdef __cplusplus
extern "C"
{
#endif
/*********************************************************************/
  
#include "stm32f0xx_hal.h"
#include "Global.h"
	
	
#define ACTIVE_LOW        !
#define ACTIVE_HIGH       !!    /* double negation forces result to be '1' */
	

/************** ����״̬���********/
// ManualOpenLock ����
#define PUSH1_POLARITY           ACTIVE_HIGH 
#define HAL_PUSH_BUTTON1()       (PUSH1_POLARITY(HAL_GPIO_ReadPin(GPIOB,  GPIO_PIN_5)))

#define PUSH2_POLARITY           ACTIVE_HIGH 
#define HAL_PUSH_BUTTON2()       (PUSH2_POLARITY(HAL_GPIO_ReadPin(GPIOB,  GPIO_PIN_6)))
	
#define PUSH3_POLARITY           ACTIVE_HIGH 
#define HAL_PUSH_BUTTON3()       (PUSH3_POLARITY(HAL_GPIO_ReadPin(GPIOB,  GPIO_PIN_7)))


#define SCL(a)   st( if(a) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET); else HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);)
#define SDA(a)   st( if(a) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET); else HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);)


extern  void BSP_GPIO_Init( void );

/*********************************************************************/
#ifdef __cplusplus
}
#endif

#endif
