/**************************************************************************************************
  Filename:       I2C_PCF8563.h
  Revised:        $Date: 2019-9-8  $
  Revision:       $Revision: V1.0  $
  User:           $User:Asong      $
  Description:    PCF8563 I2C ���

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
#define 	SV_REG 			    0x01		//������ѹ�� �˴���������Ϊ 0.1ŷ
#define 	BV_REG 			    0x02		//���ߵ�ѹ
#define 	PWR_REG 		    0x03		//��Դ����
#define 	CUR_REG 		    0x04		//����
#define 	CAL_REG 		    0x05		//У׼���趨�����̷�Χ�Լ������͹��ʲ����� 
#define 	ONFF_REG 		    0x06		//���� ʹ�� �������ú�ת��׼������
#define 	AL_REG 			    0x07		//��������ѡ����������Ƚϵ��޶�ֵ
#define 	INA226_GET_ADDR 0XFF		//����Ψһ��оƬ��ʶ��
#define   INA226_ADDR1	  0x80

//#define   	INA226_GETALADDR	0x14 




extern void I2C2_INA226_Init(void);
extern void INA226_SetRegPointer(uint8_t addr,uint8_t reg);
extern void INA226_SendData(uint8_t addr,uint8_t reg,uint16_t data);

extern uint16_t INA226_ReadData(uint8_t addr);
//uint8_t	INA226_AlertAddr(void);
extern uint16_t INA226_Get_ID(uint8_t addr);				//��ȡ id
extern uint16_t INA226_GetVoltage( uint8_t addr);		//��ȡ���ߵ�ѹ
extern uint16_t INA226_GetShunt_Current(uint8_t addr);	//��ȡ��������
extern uint16_t INA226_GetShuntVoltage(uint8_t addr);	//������ѹ
extern uint16_t INA226_Get_Power(uint8_t addr);			//��ȡ����

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

