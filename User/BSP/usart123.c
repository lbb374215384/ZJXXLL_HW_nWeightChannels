/* Includes ------------------------------------------------------------------*/
#include "usart123.h"
#include "Global.h"
#include "main.h"
#include "gpio.h"
#include "Variable.h"
#include "UartTask.h"

/* Private define ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
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
UART_HandleTypeDef Uart5Handle;
UART_HandleTypeDef UartHandle[6];//5路串口作为秤盘通信口
uartBuf_t    UartPort;        //5路串口接收发送缓冲
uartISRCfg_t isrCfg[6];          //主串口结构

mainISRCfg_t isrMainCfg;
#ifdef DMA_TX	
DMA_HandleTypeDef hdma_usart5_tx;
#endif
DMA_HandleTypeDef hdma_usart5_rx;


/*********************************************************************
 * EXTERNAL VARIABLES
 */


/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */


/**************************************************************************************************
 * @fn     void USART1_OPS_Config(void) 
 *
 * @brief   USART_Main_Config 主串口配置
 *
 * @param   None
 *
 * @return  None
 **************************************************************************************************/
void USART_Main_Config(void)
{	
	Uart5Handle.pRxBuffPtr = UartPort.rxBuf;
	Uart5Handle.pTxBuffPtr = UartPort.txBuf;
	
	Uart5Handle.RxXferSize = BufSize;
	Uart5Handle.TxXferSize = BufSize;
	
  Uart5Handle.Instance        = USART5;
  Uart5Handle.Init.BaudRate   = UART_DEFAULT_BAUDRATE;
  Uart5Handle.Init.WordLength = UART_DEFAULT_WORDLENGTH;
  Uart5Handle.Init.StopBits   = UART_DEFAULT_STOPBITS;
  Uart5Handle.Init.Parity     = UART_DEFAULT_PARITY;
  Uart5Handle.Init.HwFlowCtl  = UART_DEFAULT_HWCONTROL;
  Uart5Handle.Init.Mode       = UART_DEFAULT_MODE;
  Uart5Handle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

  HAL_UART_DeInit(&Uart5Handle);

	if (HAL_UART_Init(&Uart5Handle) != HAL_OK)
  {
		HalLedSet(HAL_LED_ALL, HAL_MODE_ON);
	  while(1);
  }
	
	/* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
  SET_BIT(Uart5Handle.Instance->CR3, USART_CR3_EIE);

	/* Enable the UART Parity Error and Data Register not empty Interrupts */
  SET_BIT(Uart5Handle.Instance->CR1, UART_IT_PE );
	
  HAL_UART_Receive_DMA(&Uart5Handle,Uart5Handle.pRxBuffPtr, Uart5Handle.RxXferSize);//开启dma接收
}

/*****************************************秤盘串口通信配置************************************************************/

/**************************************************************************************************
 * @fn     void UARTx_Config( USART_TypeDef* usart, UART_HandleTypeDef *huart )
 *
 * @brief   单秤盘串口配置
 *
 * @param   None
 *
 * @return  None
 **************************************************************************************************/
void UARTx_Config( USART_TypeDef* usart, UART_HandleTypeDef *huart ,uint8_t index)
{
	// Init the low level hardware : GPIO, CLOCK in HAL_UART_MspInit(huart); */
	/* Configure the UART peripheral */
  /* Put the USART peripheral in the Asynchronous mode (UART Mode) */
  /* UART configured as follows:
      - Word Length = 8 Bits
      - Stop Bit = One Stop bit
      - Parity = None
      - BaudRate = 9600 baud
      - Hardware flow control disabled (RTS and CTS signals) */
	
//	huart->pRxBuffPtr = UartPort[index].rxBuf;
//	huart->pTxBuffPtr = UartPort[index].txBuf;
	
//	huart->RxXferSize = BufSize;
//	huart->TxXferSize = BufSize;
	
//	if(usart == USART5)
//	{
//		huart->pRxBuffPtr = UartPort.rxBuf;
//	  huart->pTxBuffPtr = UartPort.txBuf;
//	}

  huart->Instance        = usart;
  huart->Init.BaudRate   = UART_DEFAULT_BAUDRATE;//默认配置宏定义
  huart->Init.WordLength = UART_DEFAULT_WORDLENGTH;
  huart->Init.StopBits   = UART_DEFAULT_STOPBITS;
  huart->Init.Parity     = UART_DEFAULT_PARITY;
  huart->Init.HwFlowCtl  = UART_DEFAULT_HWCONTROL;
  huart->Init.Mode       = UART_DEFAULT_MODE;
  huart->AdvancedInit.AdvFeatureInit = UART_DEFAULT_ADVFEATURE;
	
	HAL_UART_DeInit(huart);
	
  if (HAL_UART_Init(huart) != HAL_OK)
  {
		HalLedSet(HAL_LED_ALL, HAL_MODE_ON);
	  while(1);
  }
	
	UART_MASK_COMPUTATION(huart);
	
  /* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
  SET_BIT(huart->Instance->CR3, USART_CR3_EIE);
		
	/* Enable the UART Parity Error and Data Register not empty Interrupts */
  SET_BIT(huart->Instance->CR1, USART_CR1_PEIE | USART_CR1_RXNEIE);
}


/**************************************************************************************************
 * @fn     void UartScalesInit(void)
 *
 * @brief   秤盘通信口配置
 *
 * @param   None
 *
 * @return  None
 **************************************************************************************************/
void UartScalesInit(void)
{
	//UART_HandleTypeDef UartHandle[5];//5路串口作为秤盘通信
	//uartBuf_t UartPort;              //5路串口接收发送缓冲

	UARTx_Config( USART4, &UartHandle[0],0 );//&UartHandle[2] 
	UARTx_Config( USART2, &UartHandle[1],1 );//&UartHandle[4]
  UARTx_Config( USART6, &UartHandle[2],2 );//&UartHandle[0]
	UARTx_Config( USART3, &UartHandle[3],3 );//&UartHandle[3]
  UARTx_Config( USART1, &UartHandle[4],4 );//&UartHandle[1]
	UARTx_Config( USART5, &UartHandle[5],5 );//&UartHandle[1]
	
}
/*****************************************************************************/
///**************************************************************************************************
// * @fn      HalUARTTxAvailISR(uint8_t index)
// *
// * @brief   Calculate Tx Buffer length - the number of bytes in the buffer.
// *
// * @param   none
// *
// * @return  length of current Tx Buffer
// **************************************************************************************************/
//uint16_t HalUARTTxAvailISR(uint8_t index)
//{
//  uint16_t tmp = isrCfg[index].txHead;

//  return (tmp > isrCfg[index].txTail) ? (isrCfg[index].txHead - isrCfg[index].txTail - 1) : (HAL_UART_ISR_TX_MAX - isrCfg[index].txTail + isrCfg[index].txHead - 1);
//}
///******************************************************************************
// * @fn      HalUARTWriteISR
// *
// * @brief   Write a buffer to the UART.
// *
// * @param   buf - pointer to the buffer that will be written, not freed
// *          len - length of
// *
// * @return  length of the buffer that was sent
// *****************************************************************************/
//uint16_t HalUARTWriteISR(UART_HandleTypeDef *huart ,uint8_t *buf, uint16_t len, uint8_t index)
//{
//  uint16_t cnt;
// 
//  // Enforce all or none.
//  if (HalUARTTxAvailISR(index) < len)
//  {
//    return 0;
//  }

//  for (cnt = 0; cnt < len; cnt++)
//  {
//    isrCfg[index].txBuf[isrCfg.txTail] = *buf++;
//    isrCfg[index].txMT = 0;

//    if (isrCfg[index].txTail >= HAL_UART_ISR_TX_MAX-1)
//    {
//      isrCfg[index].txTail = 0;
//    }
//    else
//    {
//      isrCfg[index].txTail++;
//    }

//    // Keep re-enabling ISR as it might be keeping up with this loop due to other ints.
//    /* Enable UART1 Transmit interrupt*/
//    /* Enable the UART Transmit Data Register Empty Interrupt */
//		SET_BIT(huart->Instance->CR1, USART_CR1_TXEIE);
//  }

//   return cnt;
//}
/**************************************************************************************************
 * @fn      HalUARTRxAvailISR(uint8_t index)
 *
 * @brief   Calculate Rx Buffer length - the number of bytes in the buffer.
 *
 * @param   none
 *
 * @return  length of current Rx Buffer
 **************************************************************************************************/
uint16_t HalUARTRxAvailISR( uint8_t index )
{
  uint16_t tmp = isrCfg[index].rxTail;

  return (tmp >= isrCfg[index].rxHead) ? (tmp - isrCfg[index].rxHead) : (HAL_UART_ISR_RX_MAX - isrCfg[index].rxHead + tmp);
}

/*****************************************************************************
 * @fn      HalUARTReadISR
 *
 * @brief   Read a buffer from the UART
 *
 * @param   buf  - valid data buffer at least 'len' bytes in size
 *          len  - max length number of bytes to copy to 'buf'
 *
 * @return  length of buffer that was read
 *****************************************************************************/
uint16_t HalUARTReadISR(uint8_t *buf, uint16_t len, uint8_t index )
{
  uint16_t cnt = 0;

  while ((isrCfg[index].rxHead != isrCfg[index].rxTail) && (cnt < len))
  {
    *buf++ = isrCfg[index].rxBuf[isrCfg[index].rxHead++];
    if (isrCfg[index].rxHead >= HAL_UART_ISR_RX_MAX)
    {
      isrCfg[index].rxHead = 0;
    }
    cnt++;
  }

  return cnt;
}
/**************************************************************************************************
 * @fn      void UartTimeOut_handle(void)  
 *
 * @brief   串口帧间隔判断
 *
 * @param   None
 *
 * @return  None
 **************************************************************************************************/
void UartTimeOut_handle(void)  
{
	uint8_t i=0;

	for( i = 0; i<6; i++)
	{
		if( isrCfg[i].rxBusy == 1 )
		{
			 /********usart1帧间隔判定********/
			 if( isrCfg[i].frameIdleCount > 0 )
				 isrCfg[i].frameIdleCount--;   
			 else
			 {
				 isrCfg[i].rxBusy = 0;
				 isrCfg[i].frameIdleCount = 0;
				 isrCfg[i].rxTimeOut = 1;
			 }
	  }
 	}
	
}

/**************************************************************************************************
 * @fn      void UsartReceive_IDLE(UART_HandleTypeDef *huart)  
 *
 * @brief   接收空闲中断处理函数
 *
 * @param   None
 *
 * @return  None
 **************************************************************************************************/
void UsartReceive_IDLE(UART_HandleTypeDef *huart)  
{
	if((__HAL_UART_GET_FLAG(huart,UART_FLAG_IDLE) != RESET))  
	{ 
		 __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_IDLE);
		if( huart->Instance == USART5 )
		{
			__HAL_UART_CLEAR_IDLEFLAG(huart);
			HAL_UART_DMAStop(huart);			
			huart->RxXferCount = 0xFFFF;//数据接收完成
			HAL_UART_Receive_DMA(huart, huart->pRxBuffPtr,  huart->RxXferSize);
		}
	}
}  
/**
  * @brief Handle UART1 interrupt request.  RX DMA  TX NULL
  * @param huart UART1 handle.
  * @retval None
  */
void HAL_UART12346_IRQHandler(UART_HandleTypeDef *huart, uint8_t index )
{
	uint32_t isrflags   = READ_REG(huart->Instance->ISR);
  uint32_t cr1its     = READ_REG(huart->Instance->CR1);
  uint32_t cr3its     = READ_REG(huart->Instance->CR3);
	
  uint32_t errorflags = 0;

  uint16_t  uhdata;
	
	/* UART in mode Receiver ---------------------------------------------------*/
	/* If no error occurs */
  errorflags = (isrflags & (uint32_t)(USART_ISR_PE | USART_ISR_FE | USART_ISR_ORE | USART_ISR_NE));
  if (errorflags == 0U)
  {
		if(((isrflags & USART_ISR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET))
		{
			//*husart->pRxBuffPtr++ = (uint8_t)(husart->Instance->RDR & (uint8_t)uhMask);
			 uhdata = (uint16_t) READ_REG(huart->Instance->RDR);
			 // tmp = (uint8_t)(uhdata & (uint8_t)uhMask);
			 isrCfg[index].rxBuf[isrCfg[index].rxTail] = (uint8_t)(uhdata);
			 if (++isrCfg[index].rxTail >= HAL_UART_ISR_RX_MAX)
			 {
				 isrCfg[index].rxTail = 0;
			 }
			 isrCfg[index].frameIdleCount = TimeOutCount;
			 isrCfg[index].rxBusy = 1;
			 
			 return;
		}
	}
	
  /* If some errors occur */
  cr3its = READ_REG(huart->Instance->CR3);
  if( (errorflags != RESET)&& (((cr3its & USART_CR3_EIE) != RESET)     \
         || ((cr1its & (USART_CR1_RXNEIE | USART_CR1_PEIE)) != RESET)) )
  {
    /* UART parity error interrupt occurred -------------------------------------*/
    if(((isrflags & USART_ISR_PE) != RESET) && ((cr1its & USART_CR1_PEIE) != RESET))
    {
      __HAL_UART_CLEAR_IT(huart, UART_CLEAR_PEF);
      huart->ErrorCode |= HAL_UART_ERROR_PE;
    }

    /* UART frame error interrupt occurred --------------------------------------*/
    if(((isrflags & USART_ISR_FE) != RESET) && ((cr3its & USART_CR3_EIE) != RESET))
    {
      __HAL_UART_CLEAR_IT(huart, UART_CLEAR_FEF);
      huart->ErrorCode |= HAL_UART_ERROR_FE;
    }

    /* UART noise error interrupt occurred --------------------------------------*/
    if(((isrflags & USART_ISR_NE) != RESET) && ((cr3its & USART_CR3_EIE) != RESET))
    {
      __HAL_UART_CLEAR_IT(huart, UART_CLEAR_NEF);
      huart->ErrorCode |= HAL_UART_ERROR_NE;
    }
    
    /* UART Over-Run interrupt occurred -----------------------------------------*/
    if(((isrflags & USART_ISR_ORE) != RESET) &&
       (((cr1its & USART_CR1_RXNEIE) != RESET) || ((cr3its & USART_CR3_EIE) != RESET)))
    {
      __HAL_UART_CLEAR_IT(huart, UART_CLEAR_OREF);
      huart->ErrorCode |= HAL_UART_ERROR_ORE;
    }
		
		/* Call UART Error Call back function if need be --------------------------*/
    if(huart->ErrorCode != HAL_UART_ERROR_NONE)
    {
      /* UART in mode Receiver ---------------------------------------------------*/
      if(((isrflags & USART_ISR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET))
      {
				//*husart->pRxBuffPtr++ = (uint8_t)(husart->Instance->RDR & (uint8_t)uhMask);
				 uhdata = (uint16_t) READ_REG(huart->Instance->RDR);
				 // tmp = (uint8_t)(uhdata & (uint8_t)uhMask);
				 isrCfg[index].rxBuf[isrCfg[index].rxTail] = (uint8_t)(uhdata);
				 if (++isrCfg[index].rxTail >= HAL_UART_ISR_RX_MAX)
				 {
					 isrCfg[index].rxTail = 0;
				 }
				 isrCfg[index].frameIdleCount = TimeOutCount;
				 isrCfg[index].rxBusy = 1;				
      }

      /* If Overrun error occurs, or if any error occurs in DMA mode reception,
         consider error as blocking */
      if (((huart->ErrorCode & HAL_UART_ERROR_ORE) != RESET) ||
          (HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR)))
      {  
        /* Blocking error : transfer is aborted
           Set the UART state ready to be able to start again the process,
           Disable Rx Interrupts, and disable Rx DMA request, if ongoing */
//        UART_EndRxTransfer(huart);
				/* Disable RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts */
				CLEAR_BIT(huart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));
				CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);
				/* At end of Rx process, restore huart->RxState to Ready */
				huart->RxState = HAL_UART_STATE_READY;
				
//        /* Disable the UART DMA Rx request if enabled */
//        if (HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR))
//        {
//          CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAR);

//          /* Abort the UART DMA Rx channel */
//          if(huart->hdmarx != NULL)
//          {
//            /* Set the UART DMA Abort callback : 
//               will lead to call HAL_UART_ErrorCallback() at end of DMA abort procedure */
//            huart->hdmarx->XferAbortCallback = UART_DMAAbortOnError;

//            /* Abort DMA RX */
//            if(HAL_DMA_Abort_IT(huart->hdmarx) != HAL_OK)
//            {
//              /* Call Directly huart->hdmarx->XferAbortCallback function in case of error */
//              huart->hdmarx->XferAbortCallback(huart->hdmarx);
//            }
//          }
//          else
//          {
            /* Call user error callback */
//            HAL_UART_ErrorCallback(huart);
//						UARTx_Config( huart->Instance, huart ,index)；
//          }
//        }
//        else
//        {
          /* Call user error callback */
         // HAL_UART_ErrorCallback(huart);
					UARTx_Config( huart->Instance, huart ,index);
//        }
      }
      else
      {
        /* Non Blocking error : transfer could go on. 
           Error is notified to user through user error callback */
//        HAL_UART_ErrorCallback(huart);
        huart->ErrorCode = HAL_UART_ERROR_NONE;
      }
    }
    return;
  } /* End if some error occurs */
	
	 /* UART in mode Transmitter ------------------------------------------------*/
  if(((isrflags & USART_ISR_TXE) != RESET) && ((cr1its & USART_CR1_TXEIE) != RESET))
  {
//		UART_Transmit_IT(huart);
		/* Disable the UART Transmit Data Register Empty Interrupt */
    CLEAR_BIT(huart->Instance->CR1, USART_CR1_TXEIE);
    /* Enable the UART Transmit Complete Interrupt */
    SET_BIT(huart->Instance->CR1, USART_CR1_TCIE);
    return;
  }

  /* UART in mode Transmitter (transmission end) -----------------------------*/
  if(((isrflags & USART_ISR_TC) != RESET) && ((cr1its & USART_CR1_TCIE) != RESET))
  {
//    UART_EndTransmit_IT(huart);
		  /* Disable the UART Transmit Complete Interrupt */
		CLEAR_BIT(huart->Instance->CR1, USART_CR1_TCIE);
		/* Tx process is ended, restore huart->gState to Ready */
		huart->gState = HAL_UART_STATE_READY;
    return;
  }
}

#ifdef DMA_TR
/**
  * @brief Handle UART1 interrupt request.  RX DMA  TX NULL
  * @param huart UART1 handle.
  * @retval None
  */
void HAL_UART5_IRQHandler(UART_HandleTypeDef *huart)
{
	uint32_t isrflags   = READ_REG(huart->Instance->ISR);
  uint32_t cr1its     = READ_REG(huart->Instance->CR1);
  uint32_t cr3its     = READ_REG(huart->Instance->CR3);
	
  uint32_t errorflags = 0;
	uint32_t errorcode;

	/* If no error occurs */
  errorflags = (isrflags & (uint32_t)(USART_ISR_PE | USART_ISR_FE | USART_ISR_ORE | USART_ISR_NE));
  /* If some errors occur */
  if ((errorflags != 0U) \
      && (((cr3its & USART_CR3_EIE) != 0U)\
          || ((cr1its & (USART_CR1_RXNEIE | USART_CR1_PEIE)) != 0U)))
  {
    /* UART parity error interrupt occurred -------------------------------------*/
    if (((isrflags & USART_ISR_PE) != 0U) && ((cr1its & USART_CR1_PEIE) != 0U))
    {
      __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_PEF);
      huart->ErrorCode |= HAL_UART_ERROR_PE;
    }

    /* UART frame error interrupt occurred --------------------------------------*/
    if (((isrflags & USART_ISR_FE) != 0U) && ((cr3its & USART_CR3_EIE) != 0U))
    {
      __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_FEF);

      huart->ErrorCode |= HAL_UART_ERROR_FE;
    }

    /* UART noise error interrupt occurred --------------------------------------*/
    if (((isrflags & USART_ISR_NE) != 0U) && ((cr3its & USART_CR3_EIE) != 0U))
    {
      __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_NEF);

      huart->ErrorCode |= HAL_UART_ERROR_NE;
    }
    
    /* UART Over-Run interrupt occurred -----------------------------------------*/
    if (((isrflags & USART_ISR_ORE) != 0U)\
        && (((cr1its & USART_CR1_RXNEIE) != 0U) ||\
            ((cr3its & USART_CR3_EIE) != 0U)))
    {
      __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_OREF);

      huart->ErrorCode |= HAL_UART_ERROR_ORE;
    }
		
    /* Call UART Error Call back function if need be --------------------------*/
    if (huart->ErrorCode != HAL_UART_ERROR_NONE)
    {
//      /* UART in mode Receiver ---------------------------------------------------*/
//      if (((isrflags & USART_ISR_RXNE) != 0U) && ((cr1its & USART_CR1_RXNEIE) != 0U))
//      {
//				/* Clear RXNE interrupt flag */
//				__HAL_UART_SEND_REQ(huart, UART_RXDATA_FLUSH_REQUEST);
//      }

      /* If Overrun error occurs, or if any error occurs in DMA mode reception,
         consider error as blocking */
      errorcode = huart->ErrorCode;
      if ((HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR)) ||
          ((errorcode & HAL_UART_ERROR_ORE) != 0U))
      {
        /* Blocking error : transfer is aborted
           Set the UART state ready to be able to start again the process,
           Disable Rx Interrupts, and disable Rx DMA request, if ongoing */
				
				  /* Disable RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts */
          CLEAR_BIT(huart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));
          CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);
				
      }
      else
      {
        /* Non Blocking error : transfer could go on.
           Error is notified to user through user error callback */
        huart->ErrorCode = HAL_UART_ERROR_NONE;
      }
    }
    return;
  } /* End if some error occurs */
	
	if( __HAL_UART_GET_FLAG(huart,UART_FLAG_IDLE) )
	{
	  UsartReceive_IDLE(huart);
		return;
	}
	
	 /* UART in mode Transmitter (transmission end) -----------------------------*/
  if(((isrflags & USART_ISR_TC) != RESET) && ((cr1its & USART_CR1_TCIE) != RESET))
  {
    UART_EndTransmit_IT(huart);
    return;
  }
}
#endif

///**
//  * @brief  Tx Transfer completed callback
//  * @param  UartHandle: UART handle. 
//  * @note   This example shows a simple way to report end of DMA Tx transfer, and 
//  *         you can add your own implementation. 
//  * @retval None
//  */
//void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
//{
//  // uartMsgList_t *pItem = ScaleDataList.head;
//	
//	 ScaleData_DeleteDataSendList();
//	
//}

//void HAL_UART_TxHalfCpltCallback(UART_HandleTypeDef *huart)
//{
//   
//	 ScaleData_DeleteDataSendList();
//}
