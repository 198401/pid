/************************************************************************************************

 Author        : Yu

 Date          : 2011.5

 File          : 
                                      
 Hardware      : ADuC7024

 Description   : 
*************************************************************************************************/
#include <ADuC7024.H>
#include "port.h"

#include "mb.h"
#include "mbport.h"

#define OS_CLOCK       41780000
#define OS_TICK        50

mbBOOL
xMBPortTimersInit( USHORT usTim1Timerout50us )
{
    T2LD   = ((USHORT)(((double)OS_CLOCK*(double)OS_TICK)/1E6)-1);                       // Counter Value
	IRQEN |= WAKEUP_TIMER_BIT;              // Enable Timer1 IRQ
    return TRUE;
}


inline void
vMBPortTimersEnable(  )
{
    T2CON  = 0x04;
    T2CON  = 0x84;                       // Enabled,Free,Binary and CLK/16
}

inline void
vMBPortTimersDisable(  )
{
    T2CON  = 0x04;
}

void mb_timer2_interrupt (void) __irq
{
    ( void )pxMBPortCBTimerExpired(  );
    T2CLRI = 0;                             // Clear Timer IRQ
}

