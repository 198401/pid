/************************************************************************************************

 Author        : Yu

 Date          : 

 File          : porttimer
                                      
 Hardware      : ADuC7024

 Description   : porttimer
*************************************************************************************************/
#include <ADuC7024.H>
#include "port.h"

#include "mb.h"
#include "mbport.h"

#define OS_CLOCK       320000
#define OS_TICK        50

static ULONG mb_timer;

mbBOOL
xMBPortTimersInit( USHORT usTim1Timerout50us )
{
    mb_timer   = ((ULONG)(((double)OS_CLOCK*(double)OS_TICK)/1E6)-1);                       
    return TRUE;
}


inline void
vMBPortTimersEnable(  )
{
    T2CON  = 0x84;                       
    T2LD   = mb_timer;
    IRQEN |= WAKEUP_TIMER_BIT;              
}

inline void
vMBPortTimersDisable(  )
{
    IRQEN &= ~WAKEUP_TIMER_BIT;               
}

void mb_timer2_interrupt (void) __irq
{
    ( void )pxMBPortCBTimerExpired(  );
    T2CLRI = 0xFF;                           
}

