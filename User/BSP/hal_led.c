
/***************************************************************************************************
 *                                             INCLUDES
 ***************************************************************************************************/
#include "hal_led.h"
#include "TimeEvent.h"
//#include "AppTask.h"

/***************************************************************************************************
 *                                             CONSTANTS
 ***************************************************************************************************/

/***************************************************************************************************
 *                                              MACROS
 ***************************************************************************************************/

/***************************************************************************************************
 *                                              TYPEDEFS
 ***************************************************************************************************/
/* LED control structure */
typedef struct {
  uint8_t mode;       /* Operation mode */
  uint8_t todo;       /* Blink cycles left */
  uint8_t onPct;      /* On cycle percentage */
  uint16_t time;      /* On/off cycle time (msec) */
  uint32_t next;      /* Time for next change */
} HalLedControl_t;

typedef struct
{
  HalLedControl_t HalLedControlTable[HAL_LED_DEFAULT_MAX_LEDS];
  uint8_t           sleepActive;
} HalLedStatus_t;


/***************************************************************************************************
 *                                           GLOBAL VARIABLES
 ***************************************************************************************************/


static uint8_t HalLedState;              // LED state at last set/clr/blink update

static uint8_t HalSleepLedState;         // LED state at last set/clr/blink update
static uint8_t preBlinkState;            // Original State before going to blink mode
                                         // bit 0, 1, 2, 3 represent led 0, 1, 2, 3

static HalLedStatus_t HalLedStatusControl;


/***************************************************************************************************
 *                                            LOCAL FUNCTION
 ***************************************************************************************************/
void HalLedUpdate (void);
void HalLedOnOff (uint8_t leds, uint8_t mode);


/***************************************************************************************************
 *                                            FUNCTIONS - API
 ***************************************************************************************************/

/***************************************************************************************************
 * @fn      HalLedInit
 *
 * @brief   Initialize LED Service
 *
 * @param   init - pointer to void that contains the initialized value
 *
 * @return  None
 ***************************************************************************************************/
void HalLedInit (void)
{

 /* Initialize all LEDs to OFF */ 
  HalLedSet (HAL_LED_ALL, HAL_LED_MODE_OFF);

  HalLedStatusControl.sleepActive = 0;
}

/***************************************************************************************************
 * @fn      HalLedSet
 *
 * @brief   Tun ON/OFF/TOGGLE given LEDs
 *
 * @param   led - bit mask value of leds to be turned ON/OFF/TOGGLE
 *          mode - BLINK, FLASH, TOGGLE, ON, OFF
 * @return  None
 ***************************************************************************************************/
uint8_t HalLedSet (uint8_t leds, uint8_t mode)
{

  uint8_t led;
  HalLedControl_t *sts;

  switch (mode)
  {
    case HAL_LED_MODE_BLINK:
      /* Default blink, 1 time, D% duty cycle */
      HalLedBlink (leds, 1, HAL_LED_DEFAULT_DUTY_CYCLE, HAL_LED_DEFAULT_FLASH_TIME);
      break;

    case HAL_LED_MODE_FLASH:
      /* Default flash, N times, D% duty cycle */
      HalLedBlink (leds, HAL_LED_DEFAULT_FLASH_COUNT, HAL_LED_DEFAULT_DUTY_CYCLE, HAL_LED_DEFAULT_FLASH_TIME);
      break;

    case HAL_LED_MODE_ON:
    case HAL_LED_MODE_OFF:
    case HAL_LED_MODE_TOGGLE:

      led = HAL_LED_1;
      leds &= HAL_LED_ALL;
      sts = HalLedStatusControl.HalLedControlTable;

      while (leds)
      {
        if (leds & led)
        {
          if (mode != HAL_LED_MODE_TOGGLE)
          {
            sts->mode = mode;  /* ON or OFF */
          }
          else
          {
            sts->mode ^= HAL_LED_MODE_ON;  /* Toggle */
          }
          HalLedOnOff (led, sts->mode);
          leds ^= led;
        }
        led <<= 1;
        sts++;
      }
      break;

    default:
      break;
  }

  HalLedOnOff(leds, mode);

  return ( HalLedState );

}

/***************************************************************************************************
 * @fn      HalLedBlink
 *
 * @brief   Blink the leds
 *
 * @param   leds       - bit mask value of leds to be blinked
 *          numBlinks  - number of blinks
 *          percent    - the percentage in each period where the led
 *                       will be on
 *          period     - length of each cycle in milliseconds
 *
 * @return  None
 ***************************************************************************************************/
void HalLedBlink (uint8_t leds, uint8_t numBlinks, uint8_t percent, uint16_t period)
{
  uint8_t led;
  HalLedControl_t *sts;

  if (leds && percent && period)
  {
    if (percent < 100)
    {
      led = HAL_LED_1;
      leds &= HAL_LED_ALL;
      sts = HalLedStatusControl.HalLedControlTable;

      while (leds)
      {
        if (leds & led)
        {
          /* Store the current state of the led before going to blinking if not already blinking */
          if(sts->mode < HAL_LED_MODE_BLINK )
          	preBlinkState |= (led & HalLedState);

          sts->mode  = HAL_LED_MODE_OFF;                    /* Stop previous blink */
          sts->time  = period;                              /* Time for one on/off cycle */
          sts->onPct = percent;                             /* % of cycle LED is on */
          sts->todo  = numBlinks;                           /* Number of blink cycles */
          if (!numBlinks) sts->mode |= HAL_LED_MODE_FLASH;  /* Continuous */
          sts->next = HAL_GetTick();                        /* Start now */
          sts->mode |= HAL_LED_MODE_BLINK;                  /* Enable blinking */
          leds ^= led;
        }
        led <<= 1;
        sts++;
      }
			Timer_Event_Set( &timer_10msDB[Led_Task], 0, DISABLE, 1, HalLedUpdate );//调试用LED
    }
    else
    {
      HalLedSet (leds, HAL_LED_MODE_ON);                    /* >= 100%, turn on */
    }
  }
  else
  {
    HalLedSet (leds, HAL_LED_MODE_OFF);                     /* No on time, turn off */
  }
  percent = (leds & HalLedState) ? HAL_LED_MODE_OFF : HAL_LED_MODE_ON;
  HalLedOnOff (leds, percent);                              /* Toggle */

}

/***************************************************************************************************
 * @fn      HalLedUpdate
 *
 * @brief   Update leds to work with blink
 *
 * @param   none
 *
 * @return  none
 ***************************************************************************************************/
void HalLedUpdate (void)
{
  uint8_t led;
  uint8_t pct;
  uint8_t leds;
  HalLedControl_t *sts;
  uint32_t time;
  uint16_t next;
  uint16_t wait;

  next = 0;
  led  = HAL_LED_1;
  leds = HAL_LED_ALL;
  sts = HalLedStatusControl.HalLedControlTable;

  /* Check if sleep is active or not */
  if (!HalLedStatusControl.sleepActive)
  {
    while (leds)
    {
      if (leds & led)
      {
        if (sts->mode & HAL_LED_MODE_BLINK)
        {
          time = HAL_GetTick();
          if (time >= sts->next)
          {
            if (sts->mode & HAL_LED_MODE_ON)
            {
              pct = 100 - sts->onPct;               /* Percentage of cycle for off */
              sts->mode &= ~HAL_LED_MODE_ON;        /* Say it's not on */
              HalLedOnOff (led, HAL_LED_MODE_OFF);  /* Turn it off */

              if (!(sts->mode & HAL_LED_MODE_FLASH))
              {
								if(sts->todo != 0xFF )
                 sts->todo--;                        /* Not continuous, reduce count */
              }
            }            
            else if ( (!sts->todo) && !(sts->mode & HAL_LED_MODE_FLASH) )
            {
              sts->mode ^= HAL_LED_MODE_BLINK;      /* No more blinks */  
            }            
            else
            {
              pct = sts->onPct;                     /* Percentage of cycle for on */
              sts->mode |= HAL_LED_MODE_ON;         /* Say it's on */
              HalLedOnOff (led, HAL_LED_MODE_ON);   /* Turn it on */
            }
            if (sts->mode & HAL_LED_MODE_BLINK)
            {
              wait = (((uint32_t)pct * (uint32_t)sts->time) / 100);
              sts->next = time + wait;
            }
            else
            {
              /* no more blink, no more wait */
              wait = 0;
              /* After blinking, set the LED back to the state before it blinks */
              HalLedSet (led, ((preBlinkState & led)!=0)?HAL_LED_MODE_ON:HAL_LED_MODE_OFF);
              /* Clear the saved bit */
              preBlinkState &= (led ^ 0xFF);
            }
          }
          else
          {
            wait = sts->next - time;  /* Time left */
          }

          if (!next || ( wait && (wait < next) ))
          {
            next = wait;
          }
        }
        leds ^= led;
      }
      led <<= 1;
      sts++;
    }

    if (next)
    {
			Timer_Event_Set( &timer_10msDB[Led_Task], next, ENABLE, 0, HalLedUpdate );//调试用LED
    }
  }
}

/***************************************************************************************************
 * @fn      HalLedOnOff
 *
 * @brief   Turns specified LED ON or OFF
 *
 * @param   leds - LED bit mask
 *          mode - LED_ON,LED_OFF,
 *
 * @return  none
 ***************************************************************************************************/
void HalLedOnOff (uint8_t leds, uint8_t mode )
{
		/************** 输出控制********
	  PA7 -- LED1
		PA8 -- LED5
	
	  PB0 -- LED2
	  PB1 -- LED3
	  PB2 -- LED4
	  PB5 -- LED6
	 ******************************/
  if (leds & HAL_LED_1)
  {
    if (mode == HAL_LED_1)
    {
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
    }
    else
    {
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
    }
  }
	
  if (leds & HAL_LED_2)
  {
    if (mode == HAL_LED_MODE_ON)
    {
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
    }
    else
    {
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    }
  }  
	
	if (leds & HAL_LED_3)
  {
    if (mode == HAL_LED_MODE_ON)
    {
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    }
    else
    {
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    }
  } 
	
	if (leds & HAL_LED_4)
  {
    if (mode == HAL_LED_MODE_ON)
    {
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);
    }
    else
    {
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);
    }
  } 

	if (leds & HAL_LED_5)
  {
    if (mode == HAL_LED_MODE_ON)
    {
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
    }
    else
    {
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
    }
  }
  
	if (leds & HAL_LED_6)
  {
    if (mode == HAL_LED_MODE_ON)
    {
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
    }
    else
    {
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
    }
  } 
	
  /* Remember current state */
  if (mode)
  {
    HalLedState |= leds;
  }
  else
  {
    HalLedState &= (leds ^ 0xFF);
  }
}

/***************************************************************************************************
 * @fn      HalGetLedState
 *
 * @brief   Dim LED2 - Dim (set level) of LED2
 *
 * @param   none
 *
 * @return  led state
 ***************************************************************************************************/
uint8_t HalLedGetState ()
{
  return HalLedState;

}

/***************************************************************************************************
 * @fn      HalLedEnterSleep
 *
 * @brief   Store current LEDs state before sleep
 *
 * @param   none
 *
 * @return  none
 ***************************************************************************************************/
void HalLedEnterSleep( void )
{
  /* Sleep ON */
  HalLedStatusControl.sleepActive = 1;

  /* Save the state of each led */
//  HalSleepLedState = 0;
//  HalSleepLedState |= HAL_STATE_LED1();
//  HalSleepLedState |= HAL_STATE_LED2() << 1;
//  HalSleepLedState |= HAL_STATE_LED3() << 2;
//  HalSleepLedState |= HAL_STATE_LED4() << 3;

  /* TURN OFF all LEDs to save power */
  HalLedOnOff (HAL_LED_ALL, HAL_LED_MODE_OFF);

}

/***************************************************************************************************
 * @fn      HalLedExitSleep
 *
 * @brief   Restore current LEDs state after sleep
 *
 * @param   none
 *
 * @return  none
 ***************************************************************************************************/
void HalLedExitSleep( void )
{

  /* Load back the saved state */
  HalLedOnOff(HalSleepLedState, HAL_LED_MODE_ON);

  /* Restart - This takes care BLINKING LEDS */
  HalLedUpdate();


  /* Sleep OFF */
  HalLedStatusControl.sleepActive = 0;

}
/***************************************************************************************************
***************************************************************************************************/



