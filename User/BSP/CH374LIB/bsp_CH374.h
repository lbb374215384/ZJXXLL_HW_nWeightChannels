/*
*********************************************************************************************************
*	                                  
*	ģ������ : CH374����ģ��
*	�ļ����� : bsp_CH374.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		v1.0    2011-01-11 armfly  ST�̼���汾ΪV3.4.0�汾��
*
*	Copyright (C), 2010-2011, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __BSP_CH374_H
#define __BSP_CH374_H

#define		CH374HF_NO_CODE		1	/* CH374HFM.H�ļ��ں��˲���c���룬1��ʾ����Ҫ���ɴ��� */
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


