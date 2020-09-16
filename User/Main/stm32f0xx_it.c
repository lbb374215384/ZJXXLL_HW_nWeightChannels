/**
  ******************************************************************************
  * @file    Templates/Src/stm32f0xx_it.c 
  * @author  MCD Application Team
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f0xx_it.h"
#include "tim6.h"
#include "TimeEvent.h"
#include "usart123.h"

/** @addtogroup STM32F0xx_HAL_Examples
  * @{
  */

/** @addtogroup Templates
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
    HAL_IncTick();
}


/**
  * @brief  This function handles UART1 interrupt request.  
  * @param  None
  * @retval None
  * @Note   This function is redefined in "main.h" and related to DMA  
  *         used for USART data transmission     
  */
void USART1_IRQHandler(void)
{
	HAL_UART12346_IRQHandler(&UartHandle[4],4 );//USART1
}

/**
  * @brief  This function handles UART1 interrupt request.  
  * @param  None
  * @retval None
  * @Note   This function is redefined in "main.h" and related to DMA  
  *         used for USART data transmission     
  */
void USART2_IRQHandler(void)
{
	HAL_UART12346_IRQHandler(&UartHandle[1],1 );//USART2
}
/**
  * @brief  This function handles UART1 interrupt request.  
  * @param  None
  * @retval None
  * @Note   This function is redefined in "main.h" and related to DMA  
  *         used for USART data transmission     
  */
void USART3_6_IRQHandler(void)//
{
	HAL_UART12346_IRQHandler(&UartHandle[0],0 );//USART4
  HAL_UART12346_IRQHandler(&UartHandle[2],2 );//USART6
	HAL_UART12346_IRQHandler(&UartHandle[3],3 );//USART3
	
	HAL_UART12346_IRQHandler(&UartHandle[5],5 );//USART5
	//HAL_UART5_IRQHandler(&Uart5Handle);//USART5
}


/**
  * @brief  This function handles TIM interrupt request.
  * @param  None
  * @retval None
  */
//uint16_t counts=0;
void TIM3_IRQHandler(void)
{
	/* TIM Update event */
  if(__HAL_TIM_GET_FLAG(&Tim3Handle, TIM_FLAG_UPDATE) != RESET)
  {
    if(__HAL_TIM_GET_IT_SOURCE(&Tim3Handle, TIM_IT_UPDATE) !=RESET)
    {
			__HAL_TIM_CLEAR_IT(&Tim3Handle, TIM_IT_UPDATE);
			
		}
	}
}
/**
  * @brief  This function handles TIM interrupt request.
  * @param  None
  * @retval None
  */
void TIM6_IRQHandler(void)
{
	/* TIM Update event */
  if(__HAL_TIM_GET_FLAG(&Tim6Handle, TIM_FLAG_UPDATE) != RESET)
  {
    if(__HAL_TIM_GET_IT_SOURCE(&Tim6Handle, TIM_IT_UPDATE) !=RESET)
    {
			__HAL_TIM_CLEAR_IT(&Tim6Handle, TIM_IT_UPDATE);
			 
			TimerBase_handle();//定时1ms基准
			UartTimeOut_handle(); 
    }
  }
}

#ifdef DMA_TR
/**
* @brief  This function handles DMA1_Channel1_IRQHandler interrupt request.
* @param  None
* @retval None
*/
void DMA1_Channel1_IRQHandler(void)
{
 // HAL_DMA_IRQHandler(AdcHandle.DMA_Handle);
}

/**
* @brief  This function handles DMA1_Channel2_3_IRQHandler interrupt request.
* @param  None
* @retval None
*/
void DMA1_Channel2_3_IRQHandler(void)
{
	//HAL_DMA_IRQHandler(Uart1Handle.hdmarx);
  HAL_DMA_IRQHandler(Uart5Handle.hdmatx);
}
#endif

/**
  * @brief  This function handles TIM14 global interrupt request.
  * @param  None
  * @retval None
  */
void TIM14_IRQHandler(void)
{ 
  HAL_TIM_IRQHandler(&Input_Handle);
}
/******************************************************************************/
/*                 STM32F0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f0xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/


/**
  * @}
  */ 

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
