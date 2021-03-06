/**
  ******************************************************************************
  * @file    Templates/Src/stm32f0xx_hal_msp.c
  * @author  MCD Application Team
  * @brief   HAL MSP module.
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

/** @defgroup HAL_MSP_Private_Functions
  * @{
  */

/**
  * @brief UART MSP Initialization 
  *        This function configures the hardware resources used in this example: 
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration  
  *           - DMA configuration for transmission request by peripheral 
  *           - NVIC configuration for DMA interrupt request enable
  * @param huart: UART handle pointer
  * @retval None
  */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
  GPIO_InitTypeDef  GPIO_InitStruct;
	
  if( huart->Instance == USART5)//主串口通信
	{

		__HAL_RCC_GPIOB_CLK_ENABLE();
		__HAL_RCC_USART5_CLK_ENABLE();
#ifdef DMA_TR			
		__HAL_RCC_DMA1_CLK_ENABLE();
#endif
    /**USART5 GPIO Configuration    
    PB3     ------> USART5_TX
    PB4     ------> USART5_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_USART5;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

#ifdef DMA_TR		
     /* Configure the DMA handler for reception process */
		hdma_usart5_tx.Instance                 = DMA1_Channel2;
		hdma_usart5_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
		hdma_usart5_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
		hdma_usart5_tx.Init.MemInc              = DMA_MINC_ENABLE;
		hdma_usart5_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_usart5_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
		hdma_usart5_tx.Init.Mode                = DMA_NORMAL;
		hdma_usart5_tx.Init.Priority            = DMA_PRIORITY_HIGH;
    if (HAL_DMA_Init(&hdma_usart5_tx) != HAL_OK)
    {
     
    }
		__HAL_DMA1_REMAP(HAL_DMA1_CH2_USART5_TX);
    __HAL_LINKDMA(huart,hdmatx,hdma_usart5_tx);
		
		/* Configure the DMA handler for reception process */
		hdma_usart5_rx.Instance                 = DMA1_Channel1;
		hdma_usart5_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
		hdma_usart5_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
		hdma_usart5_rx.Init.MemInc              = DMA_MINC_ENABLE;
		hdma_usart5_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_usart5_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
		hdma_usart5_rx.Init.Mode                = DMA_NORMAL;
		hdma_usart5_rx.Init.Priority            = DMA_PRIORITY_HIGH;
    if (HAL_DMA_Init(&hdma_usart5_rx) != HAL_OK)
    {
      
    }
    __HAL_DMA1_REMAP(HAL_DMA1_CH1_USART5_RX);
    __HAL_LINKDMA(huart,hdmarx,hdma_usart5_rx);
				
		__HAL_UART_CLEAR_FLAG(huart, UART_FLAG_IDLE);
		__HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);
		
//		HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 1, 1);
//		HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
		
		HAL_NVIC_SetPriority(DMA1_Channel2_3_IRQn, 1, 1);
		HAL_NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);
#endif
		
		HAL_NVIC_SetPriority(USART3_6_IRQn, 0, 1);
		HAL_NVIC_EnableIRQ(USART3_6_IRQn);
	}
	else if( huart->Instance == USART1 )
	{
    __HAL_RCC_GPIOA_CLK_ENABLE();
		__HAL_RCC_USART1_CLK_ENABLE();
		
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
		GPIO_InitStruct.Pin       = GPIO_PIN_9|GPIO_PIN_10;
		GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull      = GPIO_PULLUP;
		GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF1_USART1;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* NVIC for USART,RX IDLE */
		HAL_NVIC_SetPriority(USART3_6_IRQn, 0, 2);
		HAL_NVIC_EnableIRQ(USART1_IRQn);	
			
	}
	else if( huart->Instance == USART2 )
	{
    __HAL_RCC_USART2_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
		
		/**USART2 GPIO Configuration    
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX 
    */
		GPIO_InitStruct.Pin       = GPIO_PIN_2|GPIO_PIN_3;
		GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull      = GPIO_PULLUP;
		GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF1_USART2;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* NVIC for USART,RX IDLE */
		HAL_NVIC_SetPriority(USART2_IRQn, 0, 2);
		HAL_NVIC_EnableIRQ(USART2_IRQn);	
			
	}
  else if( huart->Instance == USART3 )
	{
    __HAL_RCC_USART3_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /**USART3 GPIO Configuration    
    PB10     ------> USART3_TX
    PB11     ------> USART3_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
		GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull      = GPIO_PULLUP;
		GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF4_USART3;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		/* NVIC for USART,RX IDLE */
		HAL_NVIC_SetPriority(USART3_6_IRQn, 0, 2);
		HAL_NVIC_EnableIRQ(USART3_6_IRQn);
	}
	else if( huart->Instance == USART4 )
	{
    __HAL_RCC_USART4_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /**USART4 GPIO Configuration    
    PA0     ------> USART4_TX
    PA1     ------> USART4_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
		GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull      = GPIO_PULLUP;
		GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF4_USART4;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* NVIC for USART,RX IDLE */
		HAL_NVIC_SetPriority(USART3_6_IRQn, 0, 2);
		HAL_NVIC_EnableIRQ(USART3_6_IRQn);
	}
	else if( huart->Instance == USART6 )
	{
    __HAL_RCC_USART6_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /**USART6 GPIO Configuration    
    PA4     ------> USART6_TX
    PA5     ------> USART6_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5;
		GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull      = GPIO_PULLUP;
		GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF5_USART6;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* NVIC for USART,RX IDLE */
		HAL_NVIC_SetPriority(USART3_6_IRQn, 0, 2);
		HAL_NVIC_EnableIRQ(USART3_6_IRQn);
	}
}
/**
  * @brief UART MSP De-Initialization
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO, DMA and NVIC configuration to their default state
  * @param huart: UART handle pointer
  * @retval None
  */
void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{
  if( huart->Instance == USART5)//主串口配置
	{
		__HAL_RCC_USART5_FORCE_RESET();
		__HAL_RCC_USART5_RELEASE_RESET();
		__HAL_RCC_USART5_CLK_DISABLE();

    /**USART5 GPIO Configuration    
    PB3     ------> USART5_TX
    PB4     ------> USART5_RX 
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_3|GPIO_PIN_4);
#ifdef DMA_TR		 
    HAL_DMA_DeInit(huart->hdmatx);
    HAL_DMA_DeInit(huart->hdmarx);
		HAL_NVIC_DisableIRQ(DMA1_Channel2_3_IRQn);
#endif		
		HAL_NVIC_DisableIRQ(USART3_6_IRQn);
	}
	else if( huart->Instance == USART1)//
	{
		__HAL_RCC_USART1_FORCE_RESET();
		__HAL_RCC_USART1_RELEASE_RESET();
		__HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);
		
		HAL_NVIC_DisableIRQ(USART1_IRQn);
	}
	else if( huart->Instance == USART2)//
	{
		__HAL_RCC_USART2_FORCE_RESET();
		__HAL_RCC_USART2_RELEASE_RESET();
		__HAL_RCC_USART2_CLK_DISABLE();

    /**USART2 GPIO Configuration    
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);
		
		HAL_NVIC_DisableIRQ(USART2_IRQn);
	}
	else if( huart->Instance == USART3)//
	{
		__HAL_RCC_USART3_FORCE_RESET();
		__HAL_RCC_USART3_RELEASE_RESET();
		__HAL_RCC_USART3_CLK_DISABLE();

		/**USART3 GPIO Configuration    
    PB10     ------> USART3_TX
    PB11     ------> USART3_RX 
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10|GPIO_PIN_11);
		
		HAL_NVIC_DisableIRQ(USART3_6_IRQn);
	}
	else if( huart->Instance == USART4)//
	{
		__HAL_RCC_USART4_FORCE_RESET();
		__HAL_RCC_USART4_RELEASE_RESET();
		__HAL_RCC_USART4_CLK_DISABLE();

		/**USART4 GPIO Configuration    
    PA0     ------> USART4_TX
    PA1     ------> USART4_RX 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0|GPIO_PIN_1);
		
		HAL_NVIC_DisableIRQ(USART3_6_IRQn);
	}
	else if( huart->Instance == USART6)//
	{
		__HAL_RCC_USART6_FORCE_RESET();
		__HAL_RCC_USART6_RELEASE_RESET();
		__HAL_RCC_USART6_CLK_DISABLE();

    /**USART6 GPIO Configuration    
    PA4     ------> USART6_TX
    PA5     ------> USART6_RX 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_4|GPIO_PIN_5);
		
		HAL_NVIC_DisableIRQ(USART3_6_IRQn);
	}
}

/**
  * @brief TIM MSP Initialization 
  *        This function configures the hardware resources used in this example: 
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration  
  * @param htim: TIM handle pointer
  * @retval None
  */
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  
  /*## Enable peripherals and GPIO Clocks ####################################*/
  /* RCC LSI clock enable */
  RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
		//printf("HAL_RCC_OscConfig ERR\r");
  }


  /* TIMx Peripheral clock enable */
  __HAL_RCC_TIM14_CLK_ENABLE();
  
  /*## Configure the NVIC for TIMx ###########################################*/
  HAL_NVIC_SetPriority(TIM14_IRQn,0,0);
  
  /* Enable the TIM14 global Interrupt */
  HAL_NVIC_EnableIRQ(TIM14_IRQn);
}


/**
  * @brief TIM MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration
  * @param htim: TIM handle pointer
  * @retval None
  */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
	
	if( htim->Instance == TIM3 )
	{
		/*##-1- Enable peripherals and GPIO Clocks #################################*/
		/* TIMx Peripheral clock enable */
		__HAL_RCC_TIM3_CLK_ENABLE();
		
		/*##-2- Configure the NVIC for TIMx ########################################*/
		/* Set the TIMx priority */
		HAL_NVIC_SetPriority(TIM3_IRQn, 0, 1);

		/* Enable the TIMx global Interrupt */
		HAL_NVIC_EnableIRQ(TIM3_IRQn);
	}
	else if( htim->Instance == TIM6 )
	{
		/*##-1- Enable peripherals and GPIO Clocks #################################*/
		/* TIMx Peripheral clock enable */
		__HAL_RCC_TIM6_CLK_ENABLE();
		
		/*##-2- Configure the NVIC for TIMx ########################################*/
		/* Set the TIMx priority */
		HAL_NVIC_SetPriority(TIM6_IRQn, 0, 1);

		/* Enable the TIMx global Interrupt */
		HAL_NVIC_EnableIRQ(TIM6_IRQn);
	}
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
