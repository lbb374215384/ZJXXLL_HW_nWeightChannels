/*
*********************************************************************************************************
*	                                  
*	模块名称 : CH374驱动模块
*	文件名称 : bsp_CH374.h
*	版    本 : V1.0
*	说    明 : 头文件
*	修改记录 :
*		版本号  日期       作者    说明
*		v1.0    2011-01-11 armfly  ST固件库版本为V3.4.0版本。
*
*	Copyright (C), 2010-2011, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __BSP_CH374_H
#define __BSP_CH374_H

#define		CH374HF_NO_CODE		1	/* CH374HFM.H文件内含了部分c代码，1表示不需要生成代码 */
#include	"CH374HFM.H"

void bsp_InitCH374(void);
uint8_t ch374_DetectOk(void);
uint8_t ch374_DiskSize(uint32_t *_uiRetSize);
uint8_t ch374_OpenFile(char *_pFileName);
uint8_t ch374_CloseFile(void);
uint8_t ch374_CreateFile(char *_pFileName);
uint8_t ch374_SeekFile(uint32_t _uiPos);
uint8_t ch374_ReadFile(uint8_t *_pReadBuf, int32_t _iByteCount, int32_t *_piReadCount);
uint8_t ch374_WriteFile(uint8_t *_pDataBuf, int32_t _iByteCount);
uint8_t ch374_FindFile(uint8_t *_pPath, uint32_t _iNo, uint8_t *_pFileName);
uint8_t ch374_ModifyFileDate(uint16_t _usYear, uint8_t _ucMonth, uint8_t ucDay);
uint8_t ch374_DeleteFile(char *_pFileName);

#endif


