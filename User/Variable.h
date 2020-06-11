/**************************************************************************************************
  Filename:       Variable.h
  Revised:        $Date: 2018-10-8  $
  Revision:       $Revision: V1.0  $
  User:           $User:Asong      $
  Description:    相关变量

**************************************************************************************************/

#ifndef _VARIABLE_H
#define _VARIABLE_H

#include "Global.h"

//#define BufLength      25

#define Send           1
#define Receive        0

#define LEDON          0
#define LEDOFF         1

#define EEPROM_Wirte   1
#define EEPROM_Read    0
/////////////////////////////




extern const uint8_t  HAL_LED[6];

/***************************************压缩机控制逻辑***************************************/

extern void Variable_Init(void);



#endif

