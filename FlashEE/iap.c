/************************************************************************************************

 Author        : Yu

 Date          : 

 File          : iap
                                      
 Hardware      : ADuC702x

 Description   : iap
*************************************************************************************************/

#include <ADuC7024.H>
#include "AppTypes.h"

#define USER_ADD_START 0x8F600
#define USER_ADD_END   0x8F7FF
extern UNIT_CFG        g_UnitCfg;
#define CFG_NREGS      (sizeof(g_UnitCfg)/sizeof(short))

void iap_init(void)
{
    FEEMOD = 0x8;                
}

static void EepromWr( uint16_t addr, uint16_t  data)
{
    uint8_t status;
	T3CLRI = 0x55;
    FEEADR = addr;           
    FEEDAT = data;               
    FEECON = WRITE_HALF_WORD;        
    status = FEESTA&0x03;
    while (!(status)) status = FEESTA&0x03;
}

static void erase_page(uint16_t addr)
{
    uint8_t status;
    FEEADR = addr;           
    FEECON = ERASE_PAGE;             
    status = FEESTA&0x03;
    while (!(status)) status = FEESTA&0x03;
}

void  EepromWr_n( uint16_t *pcData )
{
    erase_page((uint16_t)USER_ADD_START);
    for (uint16_t i = 0; i < CFG_NREGS; i ++)
    {
        EepromWr((uint16_t)USER_ADD_START + 2 * i, pcData[i]);
    }
}
