/************************************************************************************************

 Author        : Yu

 Date          : 2011.5

 File          : 
                                      
 Hardware      : ADuC7024

 Description   : 
*************************************************************************************************/
#include "mb.h"
#include "mbport.h"

static eMBEventType eQueuedEvent;
static mbBOOL     xEventInQueue;

mbBOOL
xMBPortEventInit( void )
{
    xEventInQueue = FALSE;
    return TRUE;
}

mbBOOL
xMBPortEventPost( eMBEventType eEvent )
{
    xEventInQueue = TRUE;
    eQueuedEvent = eEvent;
    return TRUE;
}

mbBOOL
xMBPortEventGet( eMBEventType * eEvent )
{
    mbBOOL            xEventHappened = FALSE;

    if( xEventInQueue )
    {
        *eEvent = eQueuedEvent;
        xEventInQueue = FALSE;
        xEventHappened = TRUE;
    }
    return xEventHappened;
}
