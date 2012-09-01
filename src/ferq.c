/************************************************************************************************

 Author        : Yu

 Date          : 

 File          : freq
                                      
 Hardware      : ADuC702x

 Description   : freq
*************************************************************************************************/
#include "ADuC7024.h"
#include "AppTypes.h"

extern UNIT_DATA                    g_UnitData;

void pla_init(void)
{
	GP3CON = 0x3000;
	PLAELM11 = 0x0035;
	PLAELM12 = 0x008B;
	PLACLK = 0x0000;
	PLAIRQ = 0x1C1B; 
	IRQEN = 0x080000;
}

void pla_irq0_interrupt (void) __irq
{
	T1CON = 0x32180; // start Timer1. capture PLAIRQ1
	IRQCLR = 0x80000; // disable PLA IRQ0
	IRQEN = 0x00100000; // enable PLA IRQ1
}

void pla_irq1_interrupt (void) __irq
{
	g_UnitData.dat.iFreq = T1CAP; // read the capture event
	IRQCLR = 0x00100000; // disable PLA IRQ1
	IRQEN = 0x80000; // enable PLA IRQ0
	T1LD = 0x00; // to reset timer1
	T1CON = 0xC0; // reset timer1
	T1CON = 0; // stop timer1
}
