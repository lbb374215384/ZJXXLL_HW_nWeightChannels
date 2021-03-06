/**************************************************************************************************
  Filename:       AppTask.h
  Revised:        $Date: 2015-9-28  $
  Revision:       $Revision: V1.1  $
  User:           $User:Asong      $
  Description:    任务相关函数

**************************************************************************************************/
#ifndef __SPI_FLASH_H
#define __SPI_FLASH_H


#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************/
#include "stm32f0xx_hal.h"
#include "Global.h"

	
#define NV_ImageHeadSize	25
	
#define SPI_FLASH_BlockSize      0x00010000  //64k
#define MaxImageSize             0x00040000  //256k

#define NV_Image1HeadAddr 	     0x00000000
#define NV_Image1DataAddr	       0x00000100
#define NV_Image2DataAddr	       (NV_Image1DataAddr+MaxImageSize)
#define NV_Image3DataAddr	       (NV_Image2DataAddr+MaxImageSize)	
	
#define NV_Image1_Block0Addr	   NV_Image1DataAddr
#define NV_Image1_Block1Addr	   (NV_Image1_Block0Addr+SPI_FLASH_BlockSize)
#define NV_Image1_Block2Addr	   (NV_Image1_Block1Addr+SPI_FLASH_BlockSize)
#define NV_Image1_Block3Addr	   (NV_Image1_Block2Addr+SPI_FLASH_BlockSize)	

	

#define SPI_FLASH_CS_LOW()     HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET)
#define SPI_FLASH_CS_HIGH()    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET)


extern SPI_HandleTypeDef SPI2Handle;


void SPI_FLASH_Init(void);
void SPI_FLASH_SectorErase(uint32_t SectorAddr);
void SPI_FLASH_BlockErase(uint32_t BlockAddr);
void SPI_FLASH_BulkErase(void);
void SPI_FLASH_PageWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void SPI_FLASH_BufferWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void SPI_FLASH_BufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
uint32_t SPI_FLASH_ReadID(void);
uint32_t SPI_FLASH_ReadDeviceID(void);
void SPI_FLASH_StartReadSequence(uint32_t ReadAddr);
void SPI_Flash_PowerDown(void);
void SPI_Flash_WAKEUP(void);


uint8_t SPI_FLASH_ReadByte(void);
uint8_t SPI_FLASH_SendByte(uint8_t byte);
uint16_t SPI_FLASH_SendHalfWord(uint16_t HalfWord);
void SPI_FLASH_WriteEnable(void);
void SPI_FLASH_WaitForWriteEnd(void);
/*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* __SPI_FLASH_H */

