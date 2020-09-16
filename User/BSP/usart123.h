/**************************************************************************************************
  Filename:       usart123.h
  Revised:        $Date: 2018-10-8  $
  Revision:       $Revision: V1.0  $
  User:           $User:Asong      $
  Description:    usart123 config

**************************************************************************************************/
#ifndef USART123_H
#define USART123_H

#ifdef __cplusplus
extern "C"
{
#endif
  
/*********************************************************************/
#include "stm32f0xx_hal.h"
#include "Global.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* User can use this section to tailor USARTx/UARTx instance used and associated 
   resources */
/* Definition for USARTx clock resources */
//#define USART1                           USART1
#define USART1_CLK_ENABLE()              __HAL_RCC_USART1_CLK_ENABLE()
#define DMA1_CLK_ENABLE()                __HAL_RCC_DMA1_CLK_ENABLE()
#define USART1_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USART1_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()

#define USART1_FORCE_RESET()             __HAL_RCC_USART1_FORCE_RESET()
#define USART1_RELEASE_RESET()           __HAL_RCC_USART1_RELEASE_RESET()

/* Definition for USARTx Pins */
#define USART1_TX_PIN                    GPIO_PIN_9
#define USART1_TX_GPIO_PORT              GPIOA
#define USART1_TX_AF                     GPIO_AF1_USART1
#define USART1_RX_PIN                    GPIO_PIN_10
#define USART1_RX_GPIO_PORT              GPIOA
#define USART1_RX_AF                     GPIO_AF1_USART1

/* Definition for USARTx's DMA */
#define USART1_TX_DMA_CHANNEL             DMA1_Channel2
#define USART1_RX_DMA_CHANNEL             DMA1_Channel3


/* Definition for USARTx's NVIC */
#define USART1_DMA_TX_IRQn                DMA1_Channel2_3_IRQn
#define USART1_DMA_RX_IRQn                DMA1_Channel2_3_IRQn
#define USART1_DMA_TX_IRQHandler          DMA1_Channel2_3_IRQHandler
#define USART1_DMA_RX_IRQHandler          DMA1_Channel2_3_IRQHandler

/* Definition for USARTx's NVIC */
#define USART1_IRQn                      USART1_IRQn
#define USART1_IRQHandler                USART1_IRQHandler

/* Size of Trasmission buffer */
#define TXBUFFERSIZE                      (COUNTOF(aTxBuffer) - 1)
/* Size of Reception buffer */
#define RXBUFFERSIZE                      TXBUFFERSIZE
  
/* Exported macro ------------------------------------------------------------*/
#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))
/* Exported functions ------------------------------------------------------- */


extern UART_HandleTypeDef Uart5Handle;
extern UART_HandleTypeDef UartHandle[6];//5路串口作为秤盘通信口
extern uartBuf_t    UartPort;        //5路串口接收发送缓冲
extern uartISRCfg_t isrCfg[6];          //主串口结构

extern DMA_HandleTypeDef hdma_usart5_tx;
extern DMA_HandleTypeDef hdma_usart5_rx;


extern void USART_Main_Config(void);
extern void UARTx_Config( USART_TypeDef* usart, UART_HandleTypeDef *huart,uint8_t index );
extern void UartScalesInit(void);

extern uint16_t HalUARTTxAvailISR(uint8_t index);
extern uint16_t HalUARTWriteISR(UART_HandleTypeDef *huart,uint8_t *buf, uint16_t len,uint8_t index);
extern uint16_t HalUARTRxAvailISR(uint8_t index);
extern uint16_t HalUARTReadISR(uint8_t *buf, uint16_t len,uint8_t index);

extern void UsartReceive_IDLE(UART_HandleTypeDef *huart);
extern void UartTimeOut_handle(void);

extern void HAL_UART12346_IRQHandler(UART_HandleTypeDef *huart, uint8_t index);
extern void HAL_UART5_IRQHandler(UART_HandleTypeDef *huart);

/*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif
