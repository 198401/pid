/************************************************************************************************

 Author        : Yu

 Date          : 

 File          : portserial
                                      
 Hardware      : ADuC702x

 Description   : portserial
*************************************************************************************************/
#include <RTL.h>
#include <ADuC7024.H>
#include "port.h"
#include "Consts.h"

#include "mb.h"
#include "mbport.h"

void
vMBPortSerialEnable( mbBOOL xRxEnable, mbBOOL xTxEnable )
{
    if ( xRxEnable )
    {
        COMIEN0 |= 0x01;
		/* delay 2 byte time */
		for (ULONG i = 0; i < 2560; ++i)
		{
			;
		}
		GP1DAT &= ~0x00800000;
    }
    else
    {
        COMIEN0 &= ~0x01;
    }
    if ( xTxEnable )
    {
        COMIEN0 |= 0x02;
		GP1DAT |= 0x80800000;
    }
    else
    {
        COMIEN0 &= ~0x02;
    }
}

mbBOOL
xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
    BOOL            bInitialized = TRUE;
    USHORT          cfg = 0;
    ULONG           reload = ( ( 41780000UL / ulBaudRate ) / 32UL );
    volatile char   dummy;

    ( void )ucPORT;

    GP1CON |= 0x011;       /* Setup tx & rx pins on P1.0 and P1.1 */
	GP1DAT |= 0x80000000;       /* Setup tr pins on P1.7 */

    switch ( ucDataBits )
    {
    case 5:
        break;

    case 6:
        cfg |= 0x00000001;
        break;

    case 7:
        cfg |= 0x00000002;
        break;

    case 8:
        cfg |= 0x00000003;
        break;

    default:
        bInitialized = FALSE;
    }

    switch ( eParity )
    {
    case MB_PAR_NONE:
        break;

    case MB_PAR_ODD:
        cfg |= 0x00000008;
        break;

    case MB_PAR_EVEN:
        cfg |= 0x00000018;
        break;
    }

    if ( bInitialized )
    {
        COMCON0 = cfg;            /* Configure Data Bits and Parity */

        COMCON0 = 0x080;          /* Set DLAB */
        COMDIV0 = reload;         /* Set Baud     */
        COMDIV1 = reload >> 8;    /* Set Baud */
        COMCON0 = 0x007;          /* Clear DLAB */

        /* Configure UART Interrupt */
        IRQEN  |= UART_BIT;              // Enable UART IRQ
    }

    return bInitialized;
}

mbBOOL
xMBPortSerialPutByte( CHAR ucByte )
{
    COMTX = ucByte;
    return TRUE;
}

mbBOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
    *pucByte   = COMRX;
    return TRUE;
}

void mb_uart_interrupt (void) __irq 
{
    static UCHAR    ucCOMIID0;
    ucCOMIID0 = COMIID0;
    if ((ucCOMIID0 & BIT01) == BIT01)     // Tx Complete
    {
        // Sending Char
        pxMBFrameCBTransmitterEmpty(  );
    }
    if ((ucCOMIID0 & BIT02) == BIT02)     // Rx Complete
    {
        // Receiving Char
        pxMBFrameCBByteReceived(  );
    }

}
