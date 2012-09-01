/************************************************************************************************

 Author        : Yu

 Date          : 

 File          : dn1022
                                      
 Hardware      : ADuC702x

 Description   : dn1022
*************************************************************************************************/
#include <RTL.h>
#include <ADuC7024.H>
#include "port.h"
#include "Consts.h"
#include "Comm.h"

#include "mb.h"
#include "mbport.h"
#include "AppTypes.h"
#include "HT1622.h"
uint8_t byNum[6];

typedef unsigned char           BYTE; 
#ifndef true
    #define true            	1
#endif

#ifndef false
    #define false           	0
#endif

extern UNIT_DATA                g_UnitData;
extern UNIT_CFG                 g_UnitCfg;

#define IBUF_SIZE				50
#define OBUF_SIZE				50

BYTE                     		m_IBuffer[IBUF_SIZE];
BYTE                     		m_OBuffer[OBUF_SIZE];

COMM_CTL_BLOCK                  m_commPort;

BYTE QryCrc8(const BYTE *pBuffer)   
{
    BYTE i,CRCValue = 0;

    for(i=0;i<pBuffer[1]+1;i++)
    {
        CRCValue += pBuffer[i];
    }

    return CRCValue;
}

static void SendReply(void)
{
    unsigned char i;

    __disable_irq();
    for (i=0;i< m_commPort.cbOMsg;i++)
    {			
        COMTX   = m_OBuffer[i];
        while ((COMSTA0 & BIT06) == 0);
    }
    __enable_irq();	 
}

mbBOOL Dn1022ReceiveFSM(void)
{
	unsigned char    byVal;
	
	byVal = COMRX;
    
	if (m_commPort.bFramed == false)
	{
	    if (m_commPort.bFraming == false)
	    {
	        m_commPort.bFraming = true;				
	    }
	
	    m_commPort.cntFrame = 0;
	
	    if (m_commPort.cbRcvd < IBUF_SIZE)
	    {
	        vMBPortTimersEnable(  );
            m_IBuffer[m_commPort.cbRcvd] = byVal;
	        m_commPort.cbRcvd++;
	    }
	    else
	    {
	        m_commPort.bFraming = false;
	        m_commPort.bFramed = true;
            vMBPortTimersDisable(  );
	    }
	}
	return true;
}

mbBOOL Dn1022TimeOut( void )
{
    if (m_commPort.bFraming)
    {
		m_commPort.bFraming = false;
        m_commPort.bFramed = true;
        m_commPort.bFramed = true;          
    }
    vMBPortTimersDisable(  );
    return true;
}

void PrepareRequestIdentity(void)
{
	m_OBuffer[0]        = 0x01;                                
    m_OBuffer[1]        = 0x15;                 
    m_OBuffer[2]        = 0xf8;
    m_OBuffer[3]        = 0x03;
    m_OBuffer[4]        = 0x00;                                
    m_OBuffer[5]        = 0x00;                 
    m_OBuffer[6]        = 0x16;                                
    m_OBuffer[7]        = 0x00;                 
    m_OBuffer[8]        = 0x0e;
    m_OBuffer[9]        = 0x70;
    m_OBuffer[10]       = 0x4f;                                
    m_OBuffer[11]       = 0xE4;                 
    m_OBuffer[12]       = 0x01;                                
    m_OBuffer[13]       = 0x01;                 
    m_OBuffer[14]       = 0x07;
    m_OBuffer[15]       = 0x61;
    m_OBuffer[16]       = 0x62;                                
    m_OBuffer[17]       = 0x63;                 
    m_OBuffer[18]       = 0x64;                                
    m_OBuffer[19]       = 0x65;                 
    m_OBuffer[20]       = 0x66;
    m_OBuffer[21]       = 0x67;                              
    m_OBuffer[22]       = QryCrc8(&m_OBuffer[0]);
    m_commPort.cbOMsg   = m_OBuffer[1] + 2;
}

void PrepareRequestLink(void)
{
	m_OBuffer[0]        = 0x02;                                
    m_OBuffer[1]        = 0x05;                 
    m_OBuffer[2]        = 0x3F;
    m_OBuffer[3]        = 0x01;     //0x03 auto 
    m_OBuffer[4]        = 0x08;                                
    m_OBuffer[5]        = 0x08;                 
    m_OBuffer[6]        = QryCrc8(&m_OBuffer[0]);                                
    m_commPort.cbOMsg   = m_OBuffer[1] + 2;
}

void PrepareRequestPara(void)
{
	m_OBuffer[0]        = 0x20;                                
    m_OBuffer[1]        = 0x0A;                 
    m_OBuffer[2]        = 0x04;
    m_OBuffer[3]        = 0x02;
    m_OBuffer[4]        = 0x01;                                
    m_OBuffer[5]        = 0x08;                 
    m_OBuffer[6]        = 0x01;                                
    m_OBuffer[7]        = 0x02;                 
    m_OBuffer[8]        = 0x01;
    m_OBuffer[9]        = 0x02;
    m_OBuffer[10]       = 0x01;                                
    m_OBuffer[11]       = QryCrc8(&m_OBuffer[0]);                 
    m_commPort.cbOMsg   = m_OBuffer[1] + 2;
}

void PrepareRequestParaNum(void)
{
	m_OBuffer[0]        = 0x21;                                
    m_OBuffer[1]        = 0x02;                 
    m_OBuffer[2]        = 0x04;
    m_OBuffer[3]        = QryCrc8(&m_OBuffer[0]);               
    m_commPort.cbOMsg   = m_OBuffer[1] + 2;
}

void PrepareIOrequest(void)
{
	m_OBuffer[0]        = 0x05;                                
    m_OBuffer[1]        = 0x09;                 
    m_OBuffer[2]        = g_UnitData.dat.iPos;
    m_OBuffer[3]        = g_UnitData.dat.iPos >> 8;
    m_OBuffer[4]        = 0x00;                                
    m_OBuffer[5]        = 0x00;                 
    m_OBuffer[6]        = 0x16;                                
    m_OBuffer[7]        = 0x00;                 
    m_OBuffer[8]        = 0x0e;
    m_OBuffer[9]        = 0x70;
    m_OBuffer[10]       = QryCrc8(&m_OBuffer[0]);                                

    m_commPort.cbOMsg   = m_OBuffer[1] + 2;
}

void dn1022_init(ULONG ulBaudRate)
{
    ULONG           reload = ( ( 41780000UL / ulBaudRate ) / 32UL );
    volatile char   dummy;

	pxMBFrameCBByteReceived = Dn1022ReceiveFSM;
    pxMBPortCBTimerExpired  = Dn1022TimeOut;
    xMBPortTimersInit( 50 );

    GP1CON |= 0x011;          /* Setup tx & rx pins on P1.0 and P1.1 */   

    COMCON0 = 0x00000003;     /* Configure Data Bits and Parity */

    COMCON0 = 0x080;          /* Set DLAB */
    COMDIV0 = reload;         /* Set Baud */
    COMDIV1 = reload >> 8;    /* Set Baud */
    COMCON0 = 0x007;          /* Clear DLAB */

//    COMCON1 |= 0x10; 
    vMBPortTimersEnable(  );

	COMIEN0 |= 0x01;
	/* Configure ACK singal */
	GP1DAT |= 0x20200000;
	GP1DAT &= ~0x00200000; 

    GP1DAT |= 0x40400000;
	GP1DAT &= ~0x00400000; 
	
    os_dly_wait(20);
     
	PrepareRequestIdentity(); 
	SendReply(); 
    while ((COMSTA0 & 0x01) != 0x01);
    m_IBuffer[0] = COMRX;

    floattochar (m_IBuffer[0], byNum,0);
    display_digital(byNum,0,0);
    os_dly_wait(30);
    
    PrepareRequestLink(); 
	SendReply(); 
    while ((COMSTA0 & 0x01) != 0x01);
    m_IBuffer[0] = COMRX;

    floattochar (m_IBuffer[0], byNum,0);
    display_digital(byNum,0,0);
    os_dly_wait(30);

    PrepareRequestPara(); 
	SendReply(); 
    while ((COMSTA0 & 0x01) != 0x01);
    m_IBuffer[0] = COMRX;
    g_UnitData.dat.fPid = m_IBuffer[0];

    floattochar (m_IBuffer[0], byNum,0);
    display_digital(byNum,0,0);
    os_dly_wait(30);

    PrepareRequestParaNum(); 
	SendReply(); 
    while ((COMSTA0 & 0x01) != 0x01);
    m_IBuffer[0] = COMRX;
    g_UnitData.dat.fPid = m_IBuffer[0];

    floattochar (m_IBuffer[0], byNum,0);
    display_digital(byNum,0,0);   
    os_dly_wait(30);

	m_OBuffer[0]        = 3;                                
    m_OBuffer[1]        = 0;                 
    m_OBuffer[2]        = 3;
	m_commPort.cbOMsg   = 3;
	SendReply();
    while ((COMSTA0 & 0x01) != 0x01);
    m_IBuffer[0] = COMRX;

    floattochar (m_IBuffer[0], byNum,0);
    display_digital(byNum,0,0);

    os_dly_wait(250);
    /* Configure UART Interrupt */

//    PrepareIOrequest();
//	SendReply();
//    while ((COMSTA0 & 0x01) != 0x01);
//    m_IBuffer[0] = COMRX;
//    floattochar (m_IBuffer[0], byNum,0);
//    display_digital(byNum,0,0);   
//    os_dly_wait(300);

    IRQEN  |= UART_BIT;  	       
}

extern BOOL FormReply(void); 
void CheckFrame(void)
{
    BYTE    byStart;

	PrepareIOrequest();
	SendReply();

    os_dly_wait(2);
    if (m_commPort.bFramed)
    {
        byStart = m_IBuffer[0];
//        g_UnitData.dat.fPid = m_IBuffer[0];
        if (byStart == 0x85 || byStart == 0x86 || byStart == 0x0E || byStart == 0x10)
        {
            if (FormReply())
            {
                if (byStart == 0x0E || byStart == 0x10)
        		    SendReply();
            }
        }
        // If Start is not this one or wrong message received, then reset the COMM
        MCB_CLEAR(m_commPort);
    }      
}
