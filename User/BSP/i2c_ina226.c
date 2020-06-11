/* Includes ------------------------------------------------------------------*/
#include "i2c_ina226.h"
#include "main.h"
#include "Global.h"
#include "variable.h"

#include "hal_defs.h"
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


void I2C2_INA226_Init(void)
{	
	
	I2c2Handle.Instance             = I2C2;
  I2c2Handle.Init.Timing          = 0x2000090E;
  I2c2Handle.Init.OwnAddress1     = INA226_Addr;
  I2c2Handle.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
  I2c2Handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  I2c2Handle.Init.OwnAddress2     = 0xFF;
  I2c2Handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  I2c2Handle.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;
  
  if(HAL_I2C_Init(&I2c2Handle) != HAL_OK)
  {
    printf("I2C2_SHT2x_Init Err\r\n");
  }
	
	INA226_SendData(INA226_ADDR1,CFG_REG,0x484F);//设置转换时间204us,求平均值次数128，采样时间为204*128，设置模式为分流和总线连续模式
	INA226_SendData(INA226_ADDR1,CAL_REG,0x1400);//设置分流电压转电流转换参数//C800 0.1
	
}


void INA226_SetRegPointer(uint8_t addr,uint8_t reg)
{
   HAL_I2C_Master_Transmit(&I2c2Handle, INA226_Addr, &reg, 1, 0xFFFFFFFFU);
}

//发送,写入
void INA226_SendData(uint8_t addr,uint8_t reg,uint16_t data)
{
	uint8_t buf[2];
	
	buf[0] = HI_UINT16(data);
	buf[1] = LO_UINT16(data);
	
	HAL_I2C_Mem_Write(&I2c2Handle, addr, reg, I2C_MEMADD_SIZE_8BIT, buf, 2, 0xFFFFFFFFU);

}

//读取
uint16_t INA226_ReadData(uint8_t addr)
{
	uint16_t temp = 0;
	uint8_t buf[2];
	
	HAL_I2C_Master_Receive(&I2c2Handle, addr, buf, 2, 0xFFFFFFFFU);
	
	temp = BUILD_UINT16(buf[1],buf[0]);
	

	return temp;
}


//1mA/bit
uint16_t INA226_GetShunt_Current(uint8_t addr)
{
	uint16_t temp=0;	
	INA226_SetRegPointer(addr,CUR_REG);
	temp = INA226_ReadData(addr);
	if(temp&0x8000)	
		temp = ~(temp - 1);	
	return temp;
}

//获取 id
uint16_t  INA226_Get_ID(uint8_t addr)
{
	uint16_t temp=0;
	INA226_SetRegPointer(addr,INA226_GET_ADDR);
	temp = INA226_ReadData(addr);
	return (uint16_t)temp;
}

//获取校准值
uint16_t INA226_GET_CAL_REG(uint8_t addr)
{	
	uint16_t temp=0;
	INA226_SetRegPointer(addr,CAL_REG);
	temp = INA226_ReadData(addr);
	return (uint16_t)temp;
}

//1.25mV/bit
uint16_t INA226_GetVoltage(uint8_t addr)
{
	uint16_t temp=0;
	INA226_SetRegPointer(addr,BV_REG);
	temp = INA226_ReadData(addr);
	return (uint16_t)temp;	
}



//2.5uV/bit,感觉这个值是测不准的，所以下面改成2.2了
uint16_t INA226_GetShuntVoltage(uint8_t addr)
{
	int16_t temp=0;
	INA226_SetRegPointer(addr,SV_REG);
	temp = INA226_ReadData(addr);
	if(temp&0x8000)	temp = ~(temp - 1);	
	return (uint16_t)temp;	
}


uint16_t INA226_Get_Power(uint8_t addr)
{
	int16_t temp=0;
	INA226_SetRegPointer(addr,PWR_REG);
	temp = INA226_ReadData(addr);
	return (uint16_t)temp;
}


void GetVoltage(float *Voltage)//mV
{
	Voltage[0] = INA226_GetVoltage(INA226_ADDR1)*1.25f;
}


void Get_Shunt_voltage(float *Voltage)//uV
{
	Voltage[0] = (INA226_GetShuntVoltage(INA226_ADDR1)*2.5);//这里原来乘的系数是2.5
}


void Get_Shunt_Current(float *Current)//mA
{
	Current[0] = (INA226_GetShunt_Current(INA226_ADDR1)* 1.0f);
}


void GetPower(void)//W
{
	GetVoltage(&INA226_data.voltageVal);			//mV
	Get_Shunt_voltage(&INA226_data.Shunt_voltage);	//uV
	Get_Shunt_Current(&INA226_data.Shunt_Current);	//mA
	INA226_data.powerVal=INA226_data.voltageVal*0.001f * INA226_data.Shunt_Current*0.001f;
	
	printf("Vbus:%f  ShuntVotage:%f ShuntCurrent:%f Power:%f \n",INA226_data.voltageVal ,INA226_data.Shunt_voltage,INA226_data.Shunt_Current,INA226_data.powerVal);
}



























