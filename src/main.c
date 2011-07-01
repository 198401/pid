/************************************************************************************************

 Author        : Yu

 Date          : 2011.7

 File          : 
                                      
 Hardware      : ADuC7024

 Description   : 
*************************************************************************************************/
#include <RTL.h>
#include <ADuC7024.H> 
#include <absacc.h>

#include "mb.h"
#include "mbport.h"
#include "Hmi.h"

#include "AppTypes.h"

UNIT_DATA                       g_UnitData;
UNIT_BUF                        g_UnitBuf;
UNIT_CFG                        g_UnitCfg;

#define EEPADDR 				0x8F400
const UNIT_CFG                  g_UnitCfgInFlash    __at (EEPADDR);

#define REG_INPUT_START             0x1000
#define REG_INPUT_NREGS             sizeof(g_UnitData)/sizeof(short)
#define REG_HOLDING_START           0x1000
#define REG_HOLDING_NREGS           sizeof(g_UnitCfg)/sizeof(short)

OS_TID t_keyin;                         /* assigned task id of task: keyin   */
OS_TID t_blink;                         /* assigned task id of task: blink   */
OS_TID t_modbus;                        /* assigned task id of task: blink   */
OS_TID t_adc;                           /* assigned task id of task: adc     */
OS_TID t_hmi;                           /* assigned task id of task: hmi     */

U16    delay;                           /* clock tick delay global variable  */


__task void keyin(void)
{
    os_itv_set(5);                       /* set wait interval: 5 clock ticks  */
    while (1)
    {
        if (GP0DAT & 0x00000010)          /* check P0.4 key input              */
        {
            delay = 400;                       /* set 4 clock tick delay           */
        }
        else
        {
            delay = 300;                       /* set 3 clock tick delay           */
        }
        os_itv_wait();                     /* wait interval                     */
    }
}

__task void blink(void)
{
    while (1)
    {
        GP4DAT ^= 0x00040000;               /* toggle P4.2 LED                   */
        os_dly_wait(delay);                /* programmed delay                  */
    }
}

__task void modbus(void)
{
    os_itv_set (2);                       /* set wait interval: 2 clock ticks  */
    while (1)
    {
        ( void )eMBPoll(  );
    }
}

U32 GetADC(U8 adc_channel) 
{
    U16 adc_value[33];      
    U32 adc_value_ave      = 0; 
//	U16 max_value=0,min_value=0,max_index=1,min_index=1; 

    ADCCP                    = adc_channel;                     /*  select ADC channel          */
    ADCCON                  |= (1UL<<7);                        /*  start continuous conversion */
    for (U16 i=0;i<33;i++)
    {
        while (!ADCSTA);                                         /*  wait for end of conversion  */
        adc_value[i]         = (ADCDAT >> 16);          
    } 
    ADCCON                  &= ~(1UL<<7);                       /*  stop continuous conversion  */
    for (U16 i=1;i<33;i++)
    {
        adc_value_ave+=adc_value[i];    
    } 
    adc_value_ave          >>= 5;   
    return adc_value_ave; 
} 

__task void adc(void)
{
    os_itv_set (10);                                /* set wait interval: 10 clock ticks  */
    while (1)
    {
        GetADC(3);
    }
}

__task void hmi(void)
{
	while(1)
	{
		HMI_Handler();
		os_dly_wait(100);                /* programmed delay                  */
	}
}

extern void Init_FEE(void);

void ADCpoweron(int time)
{
    ADCCON = 0x20;                                  /* power-on the ADC                                */
    while (time >=0)                                 /* wait for ADC to be fully powered on             */
        time--;
}

__task void init(void)
{
    GP4DAT = 0x04040000;                             /* P4.2 defined as output                         */

    ADCpoweron(20000);                               /* power on ADC				                   */                       
    ADCCP                   = 0x00;                  /* Selecting ADC Channel 0                        */
    REFCON                  = 0x00;                  /* disconnect internal 2.5V reference to Vref pin */
    ADCCON                  = 0xE24;                 /* ADC Config: fADC/8, acq. time = 8 clocks       */

    for (U16 i = 0;i < sizeof(struct _UNIT_DATA_);i++)
        g_UnitData.buf[i]   = 0; 

    for (U16 i = 0;i < sizeof(struct _UNIT_CFG_);i++)
        g_UnitCfg.buf[i]    = g_UnitCfgInFlash.buf[i]; 

	if (g_UnitCfg.dat.byMbAddr == 0)
		g_UnitCfg.dat.byMbAddr = 1;

	if (g_UnitCfg.dat.uBau == 0)
		g_UnitCfg.dat.uBau = 19200;

    Init_FEE( );

	HMI_Init();
	GP3DAT  =0x01000000;		//p3.0设为输出 输出为0

    eMBInit( MB_RTU, g_UnitCfg.dat.byMbAddr, 0, g_UnitCfg.dat.uBau, MB_PAR_EVEN );

    /* Enable the Modbus Protocol Stack. */
    eMBEnable( );

    t_keyin = os_tsk_create (keyin, 2);      /* start task 'keyin'               */
    t_blink = os_tsk_create (blink, 2);      /* start task 'blink'               */
    t_modbus = os_tsk_create (modbus, 1);    /* start task 'modbus'              */
    t_adc = os_tsk_create (adc, 1);          /* start task 'adc'                 */
	t_hmi = os_tsk_create (hmi, 1);          /* start task 'lcd'                 */
	
    os_tsk_delete_self();
}

int main(void)
{
    os_sys_init_prio(init, 1);                   /* Initialize RTX and start init     */
}

eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    S32             iRegIndex;

    if ( ( usAddress >= REG_INPUT_START )
         && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
    {
        iRegIndex = ( S32 )( usAddress - REG_INPUT_START );
        while ( usNRegs > 0 )
        {
            *pucRegBuffer++ =
            ( U8 )( g_UnitData.buf[iRegIndex] >> 8 );
            *pucRegBuffer++ =
            ( U8 )( g_UnitData.buf[iRegIndex] & 0xFF );
            iRegIndex++;
            usNRegs--;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}
extern void EepromWr_n( unsigned short *pcData );

eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs,
                 eMBRegisterMode eMode )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    S32             iRegIndex;

    if ( ( usAddress >= REG_HOLDING_START ) &&
         ( usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS ) )
    {
        iRegIndex = ( S32 )( usAddress - REG_HOLDING_START );
        switch ( eMode )
        {
        /* Pass current register values to the protocol stack. */
        case MB_REG_READ:
            while ( usNRegs > 0 )
            {
                *pucRegBuffer++ = ( unsigned char )( g_UnitCfg.buf[iRegIndex] >> 8 );
                *pucRegBuffer++ = ( unsigned char )( g_UnitCfg.buf[iRegIndex] & 0xFF );
                iRegIndex++;
                usNRegs--;
            }
            break;

            /* Update current register values with new values from the
             * protocol stack. */
        case MB_REG_WRITE:
            while ( usNRegs > 0 )
            {
                g_UnitCfg.buf[iRegIndex]  = *pucRegBuffer++ << 8;
                g_UnitCfg.buf[iRegIndex] |= *pucRegBuffer++;
                iRegIndex++;
                usNRegs--;
            }
            EepromWr_n(g_UnitCfg.buf);
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}


eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils,
               eMBRegisterMode eMode )
{
    return MB_ENOREG;
}

eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    return MB_ENOREG;
}
