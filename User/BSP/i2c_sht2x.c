/* Includes ------------------------------------------------------------------*/
#include "i2c_sht2x.h"
#include "main.h"

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

I2C_HandleTypeDef I2c2Handle;

const uint16_t sht2xPOLYNOMIAL = 0x131;  //P(x)=x^8+x^5+x^4+1 = 100110001

/*********************************************************************
 * @fn     void I2C1_PCF8563_Init(void)
 *
 * @brief  rtc外部时钟模块I2C初始化
 *          
 * @param  none
 *         
 * @return none
 *                        
 */
 uint8_t regsht=0;
 uint16_t temcode = 0;
 float temps =0;
 uint8_t errs=0;
void I2C2_SHT2x_Init(void)
{
	
  I2c2Handle.Instance             = I2C2;
  I2c2Handle.Init.Timing          = 0x2000090E;
  I2c2Handle.Init.OwnAddress1     = SHT20Address;
  I2c2Handle.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
  I2c2Handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  I2c2Handle.Init.OwnAddress2     = 0xFF;
  I2c2Handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  I2c2Handle.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;
  
  if(HAL_I2C_Init(&I2c2Handle) != HAL_OK)
  {
    	printf("I2C2_SHT2x_Init Err\r\n");
  }
//  if (HAL_I2CEx_ConfigAnalogFilter(&I2c2Handle, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
//  {
//    printf("SHT2x HAL_I2CEx_ConfigAnalogFilter Err\r\n");
//  }
//  if (HAL_I2CEx_ConfigDigitalFilter(&I2c2Handle, 0) != HAL_OK)
//  {
//    printf("SHT2x HAL_I2CEx_ConfigDigitalFilter Err\r\n");
//  }	
	
	errs=  SHT2x_SoftReset();
	errs = SHT2x_ReadUserRegister(&regsht);
	errs= SHT2x_MeasureHM(TEMP, (uint8_t*)&temcode);
	temps =  SHT2x_CalcTemperatureC(temcode);
}


/*********************************************************************
 * @fn    uint8_t SHT2x_CheckCrc(uint8_t *data, uint8_t nbrOfBytes, uint8_t checksum)
 *
 * @brief  温湿度数据CRC校验
 *          
 * @param  none
 *         
 * @return none
 *                        
 */
uint8_t SHT2x_CheckCrc(uint8_t *data, uint8_t nbrOfBytes, uint8_t checksum)
{
  uint8_t crc = 0;	
  uint8_t byteCtr;
  //calculates 8-Bit checksum with given polynomial
  for (byteCtr = 0; byteCtr < nbrOfBytes; ++byteCtr)
  { crc ^= (data[byteCtr]);
    for (uint8_t bit = 8; bit > 0; --bit)
    { if (crc & 0x80) crc = (crc << 1) ^ sht2xPOLYNOMIAL;
      else crc = (crc << 1);
    }
  }
  if (crc != checksum) 
		return 1;
  else 
		return 0;
}

/*********************************************************************
 * @fn     uint8_t SHT2x_ReadUserRegister(uint8_t *pRegisterValue)
 *
 * @brief  读取用户寄存器
 *          
 * @param  uint8_t *pRegisterValue
 *         
 * @return 0：成功  1失败
 *                        
 */
uint8_t SHT2x_ReadUserRegister(uint8_t *pRegisterValue)
{
  uint8_t error=0;    //variable for error code

  HAL_I2C_Mem_Write(&I2c2Handle, SHT20Address, USER_REG_R, I2C_MEMADD_SIZE_8BIT, NULL, 0, 0xFFFFFFFFU);
  HAL_I2C_Mem_Read(&I2c2Handle,  SHT20Address, USER_REG_W,  I2C_MEMADD_SIZE_8BIT, pRegisterValue, 2, 0xFFFFFFFFU);
  error = SHT2x_CheckCrc (pRegisterValue,1,pRegisterValue[1]);
  
  return error;
}

/*********************************************************************
 * @fn    uint8_t SHT2x_WriteUserRegister(uint8_t *pRegisterValue)
 *
 * @brief  配置用户寄存器
 *          
 * @param  uint8_t *pRegisterValue
 *         
 * @return 0：成功  1失败
 *                        
 */
uint8_t SHT2x_WriteUserRegister(uint8_t *pRegisterValue)
{
  return HAL_I2C_Mem_Read(&I2c2Handle,  SHT20Address, USER_REG_W,  I2C_MEMADD_SIZE_8BIT, pRegisterValue, 1, 0xFFFFFFFFU);
}

/*********************************************************************
 * @fn    uint8_t SHT2x_MeasureHM(etSHT2xMeasureType eSHT2xMeasureType, nt16 *pMeasurand)
 *
 * @brief  获取温度或者湿度数据
 *          
 * @param  etSHT2xMeasureType eSHT2xMeasureType, nt16 *pMeasurand
 *         
 * @return 0：成功  1失败
 *                        
 */
uint8_t SHT2x_MeasureHM(etSHT2xMeasureType eSHT2xMeasureType, uint8_t *pMeasurand)
{
  uint8_t  error=0;    //error variable
  
  switch(eSHT2xMeasureType)
  { 
    case HUMIDITY: 
			HAL_I2C_Mem_Read(&I2c2Handle, SHT20Address, TRIG_RH_MEASUREMENT_HM, I2C_MEMADD_SIZE_8BIT, pMeasurand, 3, 0xFFFFFFFFU);
			break;
    case TEMP    :
			HAL_I2C_Mem_Read(&I2c2Handle, SHT20Address, TRIG_T_MEASUREMENT_HM, I2C_MEMADD_SIZE_8BIT, pMeasurand, 3, 0xFFFFFFFFU);
			break;
    default: break;
  }
 

  error = SHT2x_CheckCrc (pMeasurand, 2, pMeasurand[2]);
  
  return error;
}

/*********************************************************************
 * @fn     uint8_t SHT2x_SofloatReset()
 *
 * @brief  SHT2X 软件复位
 *         
 * @param  none
 *         
 * @return 0：成功  1失败
 *                        
 */
uint8_t SHT2x_SoftReset(void)
{
  uint8_t  error=0;           //error variable

  error = HAL_I2C_Mem_Write(&I2c2Handle, SHT20Address, SOFT_RESET, I2C_MEMADD_SIZE_8BIT, NULL, 0, 0xFFFFFFFFU);

  HAL_Delay(1000);
	
  return error;
}

/*********************************************************************
 * @fn     uint8_t SHT2x_SofloatReset()
 *
 * @brief  湿度代码转化成湿度值
 *         
 * @param  uint16_t u16sRH
 *         
 * @return 湿度
 *                        
 */
float SHT2x_CalcRH(uint16_t u16sRH)
{
  float humidityRH;              // variable for result

  u16sRH &= ~0x0003;          // clear bits [1..0] (status bits)
  //-- calculate relative humidity [%RH] --

  humidityRH = -6.0 + 125.0/65536 * (float)u16sRH; // RH= -6 + 125 * SRH/2^16
  return humidityRH;
}

/*********************************************************************
 * @fn     uint8_t SHT2x_SofloatReset()
 *
 * @brief  温度代码转化成湿度值
 *         
 * @param  uint16_t u16sRH
 *         
 * @return 温度
 *                        
 */
float SHT2x_CalcTemperatureC(uint16_t u16sT)
{
  float temperatureC;            // variable for result

  u16sT &= ~0x0003;           // clear bits [1..0] (status bits)

  //-- calculate temperature [?] --
  temperatureC= -46.85 + 175.72/65536 *(float)u16sT; //T= -46.85 + 175.72 * ST/2^16
  return temperatureC;
}


/*********************************************************************
 * @fn     uint8_t SHT2x_GetSerialNumber(uint8_t u8SerialNumber[])
 *
 * @brief  获取温湿度传感器型号
 *         
 * @param  uint8_t *u8SerialNumber
 *         
 * @return 0：成功  1失败
 *                        
 */
uint8_t SHT2x_GetSerialNumber(uint8_t *u8SerialNumber)
{
  uint8_t  error=0,SNB[8],SBAC[6];                          //error variable
  uint16_t  memad = 0;
  
  memad = 0x0FFA;
	
	HAL_I2C_Master_Transmit(&I2c2Handle,SHT20Address,(uint8_t*)&memad, 2, 0xFFFFFFFFU);
	error = HAL_I2C_Master_Receive(&I2c2Handle, SHT20Address, SNB, 8, 0xFFFFFFFFU);
	
  memad = 0xC9FC;
  HAL_I2C_Master_Transmit(&I2c2Handle,SHT20Address,(uint8_t*)&memad, 2, 0xFFFFFFFFU);
	error = HAL_I2C_Master_Receive(&I2c2Handle, SHT20Address, SBAC, 6, 0xFFFFFFFFU);
	
  return error;
}

