/**************************************************************************************************
  Filename:       TimeEvent.h
  Revised:        $Date: 2015-9-28  $
  Revision:       $Revision: V1.1  $
  User:           $User:Asong      $
  Description:    time tasks config information

**************************************************************************************************/
#ifndef TIMEEVENT_H
#define TIMEEVENT_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************/

#include "Global.h"

extern Event_Timer timer_10msDB[Num_10MsBase];

extern void Init_Timer_Event(void);
extern void Timer_Event_Handle(void);
extern void TimerBase_handle(void);

extern void Timer_Event_Set( Event_Timer *timer, uint32_t set_time, uint8_t enable, uint8_t flag, void(*func)(void));
extern void Set_Event( Event_Timer *timer,  void(*func)(void) );


/*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif
