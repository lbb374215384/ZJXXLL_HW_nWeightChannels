/* Includes ------------------------------------------------------------------*/
#include "i2c_tm1650.h"
#include "Global.h"
#include "main.h"
#include "gpio.h"

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
 * @fn     void I2C1_TM1650_Init(void)
 *
 * @brief  数码管驱动接口初始化
 *          
 * @param  none
 *         
 * @return none
 *                        
 */
void I2C1_TM1650_Init(void)
{	
   TM1650_SystemCmd(0x10, 0x00,0x00, 0x01);
}


/*********************************************************************
 * @fn   void TM1650_send(uint8_t saddr,uint8_t sdate)
 *
 * @brief  发送显示数据
 *          
 * @param  none
 *         
 * @return none
 *                        
 */
void TM1650_send(uint8_t saddr,uint8_t sdate)
{
 	TM1650_START();
	write_8bit(saddr);
	write_8bit(sdate);
	TM1650_STOP();        
}


/*********************************************************************
 * @fn     void TM1650_SystemCmd(uint8_t light,uint8_t segMod, uint8_t WorkMod, uint8_t Onoff)
 *
 * @brief  TM1650初始化配置
 *          
 * @param  uint8_t light,uint8_t segMod, uint8_t WorkMod, uint8_t Onoff
 *         
 * @return none
 *                        
 */
void TM1650_SystemCmd(uint8_t light,uint8_t segMod, uint8_t WorkMod, uint8_t Onoff)
{
 	TM1650_START();
	write_8bit(0x48);
	write_8bit(light | segMod | WorkMod | Onoff );
	TM1650_STOP();

}



/************ START信号*******************************/
void TM1650_START(void)
{
	SCL(1);
	SDA(1);
  HAL_Delay(1);
	SDA(0);
  HAL_Delay(1);
	SCL(0);
}

/******************** STOP信号************************/
void TM1650_STOP(void)
{
	SDA(0);
  HAL_Delay(1);
	SCL(1);
  HAL_Delay(1);
	SDA(1);
  HAL_Delay(1);
	SCL(0);
	SDA(0);
}

/****************写一个字节到TM1650********************/
void write_8bit( uint8_t dat)
{

 	uint8_t i;
	SCL(0);
	for(i=0;i<8;i++)
  {
		if(dat&0x80)
		{
			SDA(1);
      HAL_Delay(2);
			SCL(1);
      HAL_Delay(2);
			SCL(0);	 
		}
		else
		{
			SDA(0);
			HAL_Delay(2);
			SCL(1);
			HAL_Delay(2);
			SCL(0);
		}	
		dat<<=1;	 
	}
	SDA(1); 
	HAL_Delay(2);
	SCL(1);   
	HAL_Delay(2);
	SCL(0);
	HAL_Delay(1);

}
