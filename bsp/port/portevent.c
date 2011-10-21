/************************************************************************************************

 Author        : Yu

 Date          : 

 File          : portevent
                                      
 Hardware      : ADuC702x

 Description   : portevent
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

    if ( xEventInQueue )
    {
        *eEvent = eQueuedEvent;
        xEventInQueue = FALSE;
        xEventHappened = TRUE;
    }
    return xEventHappened;
}
