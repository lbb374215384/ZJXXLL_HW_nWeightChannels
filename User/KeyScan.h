/**************************************************************************************************
  Filename:       KeyScan.h
  Revised:        $Date: 2015-9-28 $
  Revision:       $Revision: V1.1  $
  User:           $User:Asong      $
  Description:    按键事件处理

**************************************************************************************************/
#ifndef _KEYSCAN_H
#define _KEYSCAN_H

#include "Global.h"



extern void KeyScanTask(void);
extern void KeyPressHandler(void);
extern void KeyLongPressHandler(void);
extern void KeyReleaseHandler(void);

extern void KeyTimeHandler(void);
#endif

