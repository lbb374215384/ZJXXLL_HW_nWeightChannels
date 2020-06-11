/**************************************************************************************************
  Filename:       I2C_PCF8563.h
  Revised:        $Date: 2019-9-8  $
  Revision:       $Revision: V1.0  $
  User:           $User:Asong      $
  Description:    PCF8563 I2C 相关

**************************************************************************************************/
#ifndef _I2C_INA226_H
#define _I2C_INA226_H


#ifdef __cplusplus
extern "C"
{
#endif
/*********************************************************************/
//---------- Includes ----------------------------------------------------------
#include "stm32f0xx_hal.h"
#include "Global.h"
//---------- Defines -----------------------------------------------------------
//  CRC

#define 	CFG_REG	 		    0x00		//
#define 	SV_REG 			    0x01		//分流电压， 此处分流电阻为 0.1欧
#define 	BV_REG 			    0x02		//总线电压
#define 	PWR_REG 		    0x03		//电源功率
#define 	CUR_REG 		    0x04		//电流
#define 	CAL_REG 		    0x05		//校准，设定满量程范围以及电流和功率测数的 
#define 	ONFF_REG 		    0x06		//屏蔽 使能 警报配置和转换准备就绪
#define 	AL_REG 			    0x07		//包含与所选警报功能相比较的限定值
#define 	INA226_GET_ADDR 0XFF		//包含唯一的芯片标识号
#define   INA226_ADDR1	  0x80

//#define   	INA226_GETALADDR	0x14 




extern void I2C2_INA226_Init(void);
extern void INA226_SetRegPointer(uint8_t addr,uint8_t reg);
extern void INA226_SendData(uint8_t addr,uint8_t reg,uint16_t data);

extern uint16_t INA226_ReadData(uint8_t addr);
//uint8_t	INA226_AlertAddr(void);
extern uint16_t INA226_Get_ID(uint8_t addr);				//获取 id
extern uint16_t INA226_GetVoltage( uint8_t addr);		//获取总线电压
extern uint16_t INA226_GetShunt_Current(uint8_t addr);	//获取分流电流
extern uint16_t INA226_GetShuntVoltage(uint8_t addr);	//分流电压
extern uint16_t INA226_Get_Power(uint8_t addr);			//获取功率

extern uint16_t INA226_GET_CAL_REG(uint8_t addr);
extern void GetVoltage(float *Voltage);		
extern void Get_Shunt_voltage(float *Current);
extern void Get_Shunt_Current(float *Current);
//void Get_Power(float *Current);
extern  void GetPower(void);
	
	
	
	
/*********************************************************************/
#ifdef __cplusplus
}
#endif

#endif

