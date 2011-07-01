/************************************************************************************************

 Author        : Yu

 Date          : 2011.5

 File          : 
                                      
 Hardware      : ADuC7024

 Description   : 
*************************************************************************************************/

#include <ADuC7024.H>
#include "AppTypes.h"

#define USER_ADD_START 0x8F400
#define USER_ADD_END   0x8F5FF
extern UNIT_CFG        g_UnitCfg;
#define CFG_NREGS           sizeof(g_UnitCfg)

static void erase_page(unsigned short paddrs);
void Init_FEE(void)
{
	FEEMOD = 0x8;				// bit 3 should be set to allow erase/write command	
}

static void EepromWr( unsigned short addr, unsigned short  data)
{
	static unsigned int status;
	FEEADR = addr;				// set data address
	FEEDAT = data;				// set data value
	FEECON = WRITE_HALF_WORD;				// single Write command
	status = FEESTA&0x03;
	while (!(status)) status = FEESTA&0x03;
}

void  EepromWr_n( unsigned short *pcData )
{
	erase_page((unsigned short)USER_ADD_START);
	for(unsigned short i=0; i < CFG_NREGS; i += 2)
	{
		EepromWr((unsigned short)USER_ADD_START + i, pcData[i]);
	}	

}

static void erase_page(unsigned short addr)
{
	static unsigned int status;
	FEEADR = addr;				// set data address
	FEECON = ERASE_PAGE;				// erase page command
	status = FEESTA&0x03;
	while (!(status)) status = FEESTA&0x03;
}
