/**************************************************************************************************
  Filename:       hal_led.h
  Revised:        $Date: 2018-10-8  $
  Revision:       $Revision: V1.0  $
  User:           $User:Asong      $
  Description:    led С���¼�

**************************************************************************************************/

#ifndef HAL_LED_H
#define HAL_LED_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "stm32f0xx_hal.h"
#include "Global.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/* LEDS - The LED number is the same as the bit position */
#define HAL_LED_1       0x01
#define HAL_LED_2       0x02
#define HAL_LED_3     	0x04
#define HAL_LED_4     	0x08
#define HAL_LED_5       0x10
#define HAL_LED_6       0x20
#define HAL_LED_7       0x40
#define Buzzer          0x80

#define HAL_LED_ALL ( HAL_LED_1 | HAL_LED_2 | HAL_LED_3 | HAL_LED_4 | HAL_LED_5 | HAL_LED_6 )

#define HAL_LED_REG_ALL  HAL_LED_ALL
/* Modes */
#define HAL_LED_MODE_OFF     0x00
#define HAL_LED_MODE_ON      0x01
#define HAL_LED_MODE_BLINK   0x02
#define HAL_LED_MODE_FLASH   0x04
#define HAL_LED_MODE_TOGGLE  0x08

#define HAL_MODE_OFF     HAL_LED_MODE_OFF
#define HAL_MODE_ON      HAL_LED_MODE_ON

/* Defaults */
#define HAL_LED_DEFAULT_MAX_LEDS      6
#define HAL_LED_DEFAULT_DUTY_CYCLE    30
#define HAL_LED_DEFAULT_FLASH_COUNT   50
#define HAL_LED_DEFAULT_FLASH_TIME    1000

/*********************************************************************
 * TYPEDEFS
 */


/*********************************************************************
 * GLOBAL VARIABLES
 */

/*
 * Initialize LED Service.
 */
extern void HalLedInit( void );

/*
 * Set the LED ON/OFF/TOGGLE.
 */
extern uint8_t HalLedSet( uint8_t led, uint8_t mode );

/*
 * Blink the LED.
 */
extern void HalLedBlink( uint8_t leds, uint8_t cnt, uint8_t duty, uint16_t time );

/*
 * Put LEDs in sleep state - store current values
 */
extern void HalLedEnterSleep( void );

/*
 * Retore LEDs from sleep state
 */
extern void HalLedExitSleep( void );

/*
 * Return LED state
 */
extern uint8_t HalLedGetState ( void );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif
