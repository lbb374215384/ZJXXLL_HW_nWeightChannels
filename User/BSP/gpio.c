/* Includes ------------------------------------------------------------------*/
#include "gpio.h"
#include "TimeEvent.h"
//#include "AppTask.h"
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
 * @fn     uint8_t BSP_GPIO_Init( void )
 *
 * @brief  
 *          
 * @param  none
 *         
 * @return none
 *                        
 */
void BSP_GPIO_Init( void )
{	
  GPIO_InitTypeDef  GPIO_InitStruct;
  
  /* Enable the GPIO_LED Clock */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

	/************** �������********
	  PA7 -- LED1
		PA8 -- LED5
	
	  PB0 -- LED2
	  PB1 -- LED3
	  PB2 -- LED4
	  PB5 -- LED6
	 ******************************/	 
	
  GPIO_InitStruct.Pin   = GPIO_PIN_7|GPIO_PIN_8;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);	

  GPIO_InitStruct.Pin   = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_5;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);	
	
  HalLedSet(HAL_LED_ALL, HAL_MODE_OFF);

}


