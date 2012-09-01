/************************************************************************************************

 Author        : Yu

 Date          : 2011.4

 File          : 
                                      
 Hardware      : ADuC7060

 Description   : 
*************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <absacc.h>

#include "Types.h"
#include "Comm.h"
#include "AppTypes.h"

extern UNIT_DATA                g_UnitData;
extern UNIT_BUF                 g_UnitBuf;
extern UNIT_CFG                 g_UnitCfg;
#define EEPADDR 				0x87400
extern UNIT_CFG                  g_UnitCfgInFlash;
#define addr(x) 				((unsigned int)(&x)-EEPADDR)
#define IBUF_SIZE				50
#define OBUF_SIZE				50

extern BYTE                     m_IBuffer[IBUF_SIZE];
extern BYTE                     m_OBuffer[OBUF_SIZE];
extern COMM_CTL_BLOCK           m_commPort;
extern float                    fz;
extern BYTE QryCrc8(const BYTE *pBuffer);

inline static BYTE GetMsgLen(void)
{
    return 2 + m_IBuffer[1];
}

void dn1022_read(void)
{
	m_OBuffer[0]        = 0x8E;                                
    m_OBuffer[1]        = 0x04;                 
    m_OBuffer[2]        = 0x03;
    m_OBuffer[3]        = 0x02;
    m_OBuffer[4]        = 0x00;                              
    m_OBuffer[5]        = QryCrc8(&m_OBuffer[0]);
    m_commPort.cbOMsg   = m_OBuffer[1] + 2;
}

void dn1022_wirte(void)
{
	m_OBuffer[0]        = 0x90;                                
    m_OBuffer[1]        = 0x02;                 
    m_OBuffer[2]        = m_IBuffer[2];
    m_OBuffer[3]        = QryCrc8(&m_OBuffer[0]);
    m_commPort.cbOMsg   = m_OBuffer[1] + 2;
	//wirte
}

void io_in(void)
{

}

BOOL FormReply(void)
{
    BYTE    cbIMsg;

    cbIMsg = GetMsgLen();
    if ((cbIMsg == 0) || (cbIMsg > m_commPort.cbRcvd) || (cbIMsg > IBUF_SIZE))
    {
        return false;
    }
    // Check CRC
    if (QryCrc8(m_IBuffer) != m_IBuffer[cbIMsg-1])
    {
        return false;
    }
    // Analyze message
    switch (m_IBuffer[0])
    {
    case 0x0E:
        dn1022_read();
        break;
    case 0x10:
        dn1022_wirte();
        break;
    case 0x85:
    case 0x86:
        io_in();
        break;
     default:
        return false;
    }

    return true;
}
