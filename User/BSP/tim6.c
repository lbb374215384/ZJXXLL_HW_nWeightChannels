/* Includes ------------------------------------------------------------------*/
#include "tim6.h"
#include "Global.h"
/* Private define ------------------------------------------------------------*/

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
TIM_HandleTypeDef    Tim6Handle;
TIM_HandleTypeDef    Tim3Handle;

/*********************************************************************
 * LOCAL FUNCTIONS
 */


/**
  * @brief  This function configures the TIM6 as a time base source. 
  *         The time source is configured  to have 1ms time base with a dedicated 
  *         Tick interrupt priority. 
  * @note   
  *         
  * @param  TickPriority Tick interrupt priority.
  * @retval HAL status
  */
HAL_StatusTypeDef TIME6BASE_Config( void )
{
  RCC_ClkInitTypeDef    clkconfig;
  uint32_t              uwTimclock, uwAPB1Prescaler = 0U;
  uint32_t              uwPrescalerValue = 0U;
  uint32_t              pFLatency;
  
//    /*Configure the TIM6 IRQ priority */
//  HAL_NVIC_SetPriority(TIM6_IRQn, TickPriority ,0U);
//  
//  /* Enable the TIM6 global Interrupt */
//  HAL_NVIC_EnableIRQ(TIM6_IRQn);
//  
//  /* Enable TIM6 clock */
//  __HAL_RCC_TIM6_CLK_ENABLE();
	
	
  
  /* Get clock configuration */
  HAL_RCC_GetClockConfig(&clkconfig, &pFLatency);
  
  /* Get APB1 prescaler */
  uwAPB1Prescaler = clkconfig.APB1CLKDivider;
  
  /* Compute TIM6 clock */
  if (uwAPB1Prescaler == RCC_HCLK_DIV1) 
  {
    uwTimclock = HAL_RCC_GetPCLK1Freq();
  }
  else
  {
    uwTimclock = 2*HAL_RCC_GetPCLK1Freq();
  }
  
  /* Compute the prescaler value to have TIM6 counter clock equal to 1MHz */
  uwPrescalerValue = (uint32_t) ((uwTimclock / 1000000U) - 1U);
  
  /* Initialize TIM6 */
  Tim6Handle.Instance = TIM6;
  
  /* Initialize TIMx peripheral as follow:
  + Period = [(TIM6CLK/1000) - 1]. to have a (1/1000) s time base.
  + Prescaler = (uwTimclock/1000000 - 1) to have a 1MHz counter clock.
  + ClockDivision = 0
  + Counter direction = Up
  */
  Tim6Handle.Init.Period = (1000000U / 1000U) - 1U;
  Tim6Handle.Init.Prescaler = uwPrescalerValue;
  Tim6Handle.Init.ClockDivision = 0U;
  Tim6Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
  Tim6Handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if(HAL_TIM_Base_Init(&Tim6Handle) == HAL_OK)
  {
    /* Start the TIM time Base generation in interrupt mode */
    return HAL_TIM_Base_Start_IT(&Tim6Handle);
  }
  
  /* Return function status */
  return HAL_ERROR;
}

/*
************************************************************
*	函数名称：	Timer3_4_Init
*
*	函数功能：	Timer3或4的定时配置
*
*	入口参数：	TIMx：TIM3 或者 TIM4
*				arr：重载值
*				psc分频值
*
*	返回参数：	无
*
*	说明：		timer3和timer4只具有更新中断功能
************************************************************
*/
HAL_StatusTypeDef Timer3_Init( uint32_t period )
{
	
  RCC_ClkInitTypeDef    clkconfig;
  uint32_t              uwTimclock, uwAPB1Prescaler = 0U;
  uint32_t              uwPrescalerValue = 0U;
  uint32_t              pFLatency;
  

  /* Get clock configuration */
  HAL_RCC_GetClockConfig(&clkconfig, &pFLatency);
  
  /* Get APB1 prescaler */
  uwAPB1Prescaler = clkconfig.APB1CLKDivider;
  
  /* Compute TIM6 clock */
  if (uwAPB1Prescaler == RCC_HCLK_DIV1) 
  {
    uwTimclock = HAL_RCC_GetPCLK1Freq();
  }
  else
  {
    uwTimclock = 2*HAL_RCC_GetPCLK1Freq();
  }
  
  /* Compute the prescaler value to have TIM3 counter clock equal to 10KHz */
  uwPrescalerValue = (uint32_t) ((uwTimclock / 1000000) - 1U);

  /* Set TIMx instance */
  Tim3Handle.Instance = TIM3;

  /* Initialize TIMx peripheral as follows:
       + Period = period - 1
       + Prescaler = (SystemCoreClock/period) - 1
       + ClockDivision = 0
       + Counter direction = Up
  */
  Tim3Handle.Init.Period            = period*1000 - 1;
  Tim3Handle.Init.Prescaler         = uwPrescalerValue;
  Tim3Handle.Init.ClockDivision     = 0;
  Tim3Handle.Init.CounterMode       = TIM_COUNTERMODE_UP;
  Tim3Handle.Init.RepetitionCounter = 0;
  Tim3Handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&Tim3Handle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /*##-2- Start the TIM Base generation in interrupt mode ####################*/
  /* Start Channel1 */
  if (HAL_TIM_Base_Start_IT(&Tim3Handle) != HAL_OK)
  {
    /* Starting Error */
    Error_Handler();
  }	
	return HAL_OK;
}

