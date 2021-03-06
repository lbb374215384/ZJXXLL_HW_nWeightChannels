/**************************************************************************************************
  Filename:       lis3dh.h
  Revised:        $Date: 2018-10-8  $
  Revision:       $Revision: V1.0  $
  User:           $User:Asong      $
  Description:    三轴加速度传感器

**************************************************************************************************/
#ifndef _LIS3DH_H
#define _LIS3DH_H


#ifdef __cplusplus
extern "C"
{
#endif
/*********************************************************************/
  
#include "stm32f0xx_hal.h"
#include "Global.h"
#include "lis3dh_reg.h"
  
#define lis3dhAddress    0x80
  
#define ACK          0
#define NO_ACK       1
 

//#define MASTER_BOARD
#define I2C_ADDRESS        0x30F

/* I2C TIMING Register define when I2C clock source is SYSCLK */
/* I2C TIMING is calculated in case of the I2C Clock source is the SYSCLK = 48 MHz */
/* This example use TIMING to 0x00A51314 to reach 1 MHz speed (Rise time = 100 ns, Fall time = 100 ns) */
#define I2C_TIMING      0x2000090E//0x00201D2B

	
	/* Definition for I2Cx clock resources */
#define I2Cx                            I2C2
#define RCC_PERIPHCLK_I2Cx              RCC_PERIPHCLK_I2C1
#define RCC_I2CxCLKSOURCE_SYSCLK        RCC_I2C1CLKSOURCE_SYSCLK
#define I2Cx_CLK_ENABLE()               __HAL_RCC_I2C2_CLK_ENABLE()
#define I2Cx_SDA_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define I2Cx_SCL_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE() 

#define I2Cx_FORCE_RESET()              __HAL_RCC_I2C2_FORCE_RESET()
#define I2Cx_RELEASE_RESET()            __HAL_RCC_I2C2_RELEASE_RESET()

/* Definition for I2Cx Pins */
#define I2Cx_SCL_PIN                    GPIO_PIN_11
#define I2Cx_SCL_GPIO_PORT              GPIOB
#define I2Cx_SDA_PIN                    GPIO_PIN_12
#define I2Cx_SDA_GPIO_PORT              GPIOB
#define I2Cx_SCL_SDA_AF                 GPIO_AF5_I2C2

/* Size of Transmission buffer */
#define TXBUFFERSIZE                      (COUNTOF(aTxBuffer) - 1)
/* Size of Reception buffer */
#define RXBUFFERSIZE                      TXBUFFERSIZE

/* Exported macro ------------------------------------------------------------*/
#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))
/* Exported functions ------------------------------------------------------- */


extern lis3dh_ctx_t       dev_ctx;
extern lis3dh_int1_cfg_t  _6d_cfg;
extern lis3dh_ctrl_reg3_t ctrl_reg3;

extern I2C_HandleTypeDef I2cHandle;
extern void LIS3DH_Init( void );


/*********************************************************************/
#ifdef __cplusplus
}
#endif

#endif
