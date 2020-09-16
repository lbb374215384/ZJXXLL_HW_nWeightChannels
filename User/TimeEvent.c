/* Includes ------------------------------------------------------------------*/
#include "TimeEvent.h"
//#include "AppTask.h"
#include "Variable.h"
#include "UartTask.h"

//#include "net_device.h"
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

Event_Timer timer_10msDB[Num_10MsBase];


/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
void Timer_Event_Set( Event_Timer *timer, uint32_t set_time, uint8_t enable, uint8_t flag, void(*func)(void));
void Init_Timer_Event(void);
void Timer_Event_Handle(void);
void Timer10msBase_handle(void);

/*********************************************************************
 * @fn      void Timer_Event_Set( Event_Timer *timer, uint32 set_time, uint8 enable, uint8 flag, void(*func)(void))
 *
 * @brief   set time event
 *          
 * @param  timer          
 * @param  set_time   
 * @param  enable     
 * @param  flag       
 * @param  *func               
 *
 * @return  none
 */
void Timer_Event_Set( Event_Timer *timer, uint32_t set_time, uint8_t enable, uint8_t flag, void(*func)(void))
{
    timer->Time = set_time;
    timer->Enable = enable;
    timer->Flag = flag;
    timer->TimersHandle = func;
}

/*********************************************************************
 * @fn      void Set_Event( Event_Timer *timer,  uint8 enable, uint8 flag, void(*func)(void) )
 *
 * @brief   set time event
 *          
 * @param  timer          
 * @param  set_time   
 * @param  enable     
 * @param  flag       
 * @param  *func               
 *
 * @return  none
 */
void Set_Event( Event_Timer *timer,  void(*func)(void) )
{
    timer->Time = 0;
    timer->Enable = 0;
    timer->Flag = 1;
    timer->TimersHandle = func;
}

/*********************************************************************
 * @fn       void Init_Timer_Event(void)
 *
 * @brief    init time event
 *          
 *       
 *
 * @param  none
 * @param  none 
 *                  
 *
 * @return  none
 */
void Init_Timer_Event(void)
{
	Set_Event( &timer_10msDB[ReadScalesDataQueueTask],ReadScalesDataQueue_Evt);

}

/*********************************************************************
 * @fn      Timer_Event_Handle
 *
 * @brief   
 *          
 *       
 *
 * @param  none
 * @param  none 
 *                  
 *
 * @return  none
 */
void Timer_Event_Handle(void)
{ 
  uint8_t i;
  for( i = 0; i < Num_10MsBase; i++ )
  {
    if( timer_10msDB[i].Flag )
    {
      if( timer_10msDB[i].TimersHandle != NULL )
        timer_10msDB[i].TimersHandle();
    }
  }
}
/*********************************************************************
 * @fn      Timer10msBase_handle
 *
 * @brief   10ms
 *          
 *       
 *
 * @param  none
 * @param  none 
 *                  
 *
 * @return  none
 */

void TimerBase_handle(void)
{
  uint8_t i = 0;
  
  for( i = 0; i < Num_10MsBase; i++ )
  {
    if( timer_10msDB[i].Enable )
    {
      if(timer_10msDB[i].Time > 0)
      {
        timer_10msDB[i].Time--;
      }
      else
      {
        timer_10msDB[i].Enable = DISABLE;
        timer_10msDB[i].Time = 0;
        timer_10msDB[i].Flag = 1;
      }
    }
  }
}

