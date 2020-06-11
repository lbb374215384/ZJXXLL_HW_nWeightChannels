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
 * @brief  ����������ӿڳ�ʼ��
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
 * @brief  ������ʾ����
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
 * @brief  TM1650��ʼ������
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



/************ START�ź�*******************************/
void TM1650_START(void)
{
	SCL(1);
	SDA(1);
  HAL_Delay(1);
	SDA(0);
  HAL_Delay(1);
	SCL(0);
}

/******************** STOP�ź�************************/
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

/****************дһ���ֽڵ�TM1650********************/
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
