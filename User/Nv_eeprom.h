#ifndef NV_EERPOM_H
#define NV_EERPOM_H

#include "Global.h"
#include "spi_flash.h"

#define NV_EEPROM_Wirte   1
#define NV_EEPROM_Read    0

#define LastBlockAddr            0x001F0000
/********���������ڵ�1������********/
#define STARTUPOPTION_ADDR         LastBlockAddr +0x00000000

/********��ʱ�������ݴ洢�ڵ�2������********/
#define PERIODTIMETASK_ADDR        LastBlockAddr +0x00001000

/********�������ò���********/
#define DEVICEINFOR_ADDR           LastBlockAddr +0x00002000

/********�������ò���********/
#define TOUCHMATRIX_ADDR           LastBlockAddr +0x00003000


extern uint8_t ReadStartupOptions( void );

extern void UserNV_EEPROMInit(uint32_t EEaddr, uint16_t len, uint8_t *buf, uint8_t setDefault );

extern void InitNV_EEpromTask(void );

















#endif

