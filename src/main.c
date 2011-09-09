/************************************************************************************************

 Author        : Yu

 Date          : 

 File          : 
                                      
 Hardware      : ADuC702x

 Description   : 
*************************************************************************************************/
#include <RTL.h>
#include <ADuC7024.H> 
#include <absacc.h>
#include <math.h>

#include "mb.h"
#include "mbport.h"
#include "Hmi.h"
#include "ds18b20.h"
#include "pid.h"
#include "calc.h"

#include "AppTypes.h"

UNIT_DATA                       g_UnitData;
UNIT_BUF                        g_UnitBuf;
UNIT_CFG                        g_UnitCfg;

#define EEPADDR 				0x8F400
const unsigned short            g_UnitCfgInFlash[256]    __at (EEPADDR);

#define REG_INPUT_START             0x0001
#define REG_INPUT_NREGS             sizeof(g_UnitData)/sizeof(short)
#define REG_HOLDING_START           0x0001
#define REG_HOLDING_NREGS           sizeof(g_UnitCfg)/sizeof(short)

//OS_TID t_feeddog;                       /* assigned task id of task: feeddog */
OS_TID t_pid;                           /* assigned task id of task: pid     */
OS_TID t_modbus;                        /* assigned task id of task: modbus  */
OS_TID t_adc;                           /* assigned task id of task: adc     */
OS_TID t_hmi;                           /* assigned task id of task: hmi     */

void lpReset(void)
{
	RSTSTA	|= 0x04;
}

//__task void feeddog(void)
//{
//    while (1)
//    {
//        T3CLRI = 0x55;
//        os_dly_wait(5);
//    }
//}

#define PWM_DAT0VALUE           0x1000  /* set PWM freq to 5.5KHz */

//void SetPwmDutyCycle0(float uiDutyCycle)
//{
//    if(uiDutyCycle > 0.5f)
//	{
//		PWMEN  &= ~0x100;
//		PWMDAT0	= PWM_DAT0VALUE;
//		PWMCH0 	= PWM_DAT0VALUE * (uiDutyCycle - 0.5f);
//	}
//	else
//	{
//		PWMEN  |= 0x100;
//		PWMDAT0	= PWM_DAT0VALUE;
//		PWMCH0 	= PWM_DAT0VALUE * (0.5 - uiDutyCycle);
//	}
//}
//
//void SetPwmDutyCycle1(float uiDutyCycle)
//{
//    if(uiDutyCycle > 0.5f)
//	{
//		PWMEN  &= ~0x080;
//		PWMDAT0	= PWM_DAT0VALUE;
//		PWMCH1 	= PWM_DAT0VALUE * (uiDutyCycle - 0.5f);
//	}
//	else
//	{
//		PWMEN  |= 0x080;
//		PWMDAT0	= PWM_DAT0VALUE;
//		PWMCH1 	= PWM_DAT0VALUE * (0.5 - uiDutyCycle);
//	}
//}

void SetPwmDutyCycle2(S32 uiDutyCycle)
{
    if(uiDutyCycle > 0x800)
	{
		GP3CON &= ~0x00010000;
		GP3CON |= 0x00100000;
		GP3DAT |= 0x10000000;
		PWMEN  	= 0x07D;
		PWMDAT0	= PWM_DAT0VALUE;
		PWMCH2 	= PWM_DAT0VALUE * (uiDutyCycle/1000 - 0.5f);
	}
	else if(uiDutyCycle > 0)
	{
		GP3CON &= ~0x00010000;
		GP3CON |= 0x00100000;
		GP3DAT |= 0x10000000;
		PWMEN  	= 0x03D;
		PWMDAT0	= PWM_DAT0VALUE;
		PWMCH2 	= PWM_DAT0VALUE * (uiDutyCycle/1000 - 0.5f);
	}
	else if(uiDutyCycle > -0x800)
	{
		GP3CON &= ~0x00100000;
		GP3CON |= 0x00010000;
		GP3DAT |= 0x20200000;
		PWMEN  	= 0x07E;
		PWMDAT0	= PWM_DAT0VALUE;
		PWMCH2 	= PWM_DAT0VALUE * (-uiDutyCycle/1000 - 0.5f);
	}
	else
	{
		GP3CON &= ~0x00100000;
		GP3CON |= 0x00010000;
		GP3DAT |= 0x20200000;
		PWMEN  	= 0x03E;
		PWMDAT0	= PWM_DAT0VALUE;
		PWMCH2 	= PWM_DAT0VALUE * (0.5 + uiDutyCycle/1000);
	}
}

volatile struct Controller_struct
{
	// input and output
	S16 output;			// output
	S16 input;			// user input
	S16 feedback;		// feedback
	// counters
	U16 counts;
} Controller;

__task void pid(void)
{
	while (1)
    {
		os_evt_wait_or (0x0001, 0xffff);

		posHandle( );
		spHandle( );		

		switch (g_UnitCfg.dat.byMode)
        {
        case 1:
			//freq
            break;
		case 2:
			tempHandle( );
            break;
        case 3:
			p1Handle( );
			p2Handle( );
			tempHandle( );
            break;
		case 4:
			tempHandle( );
            break;
        default:
			cmdHandle(0);
            break;
        } 		

		Controller.input	= (S16)g_UnitData.dat.fSp;
		Controller.feedback = (S16)g_UnitData.dat.fPv;

		if(Controller.counts > 20)
			pid_Reset_Integrator(&g_UnitCfg);

		Controller.output = pid_Controller(Controller.input, Controller.feedback, &g_UnitCfg);

		// output value
//		Controller.output = MAX(Controller.output, -1000);
//		Controller.output = MIN(Controller.output,  1000);
		// clamp output value (anti-windup)
		if(!g_UnitCfg.dat.bIsManual)
		{
			SetPwmDutyCycle2(Controller.output);
		}
		// counters
		Controller.counts++;
    }
}

__task void modbus(void)
{
    while (1)
    {
        ( void )eMBPoll(  );
		os_dly_wait(2); 
    }
}

U32 GetADC(U8 adc_channel) 
{
    U16 adc_value[33];      
    U32 adc_value_ave      = 0; 

    T3CLRI = 0x55;
	ADCCP                    = adc_channel;                     /*  select ADC channel          */
    ADCCON                  |= (1UL<<7);                        /*  start continuous conversion */
    for (U16 i=0;i<33;i++)
    {
        tsk_lock (); 
		while (!ADCSTA);                                         /*  wait for end of conversion  */
        adc_value[i]         = (ADCDAT >> 16);
		tsk_unlock ();
		          
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
    while (1)
    {									   
		if((g_UnitCfg.dat.byInp == 2)||(g_UnitCfg.dat.byInp == 3))
			GP0DAT |= 0x40400000;
		else
			GP0DAT &= ~0x00400000;
			
		g_UnitData.dat.iAD4 	= GetADC(4);
		g_UnitData.dat.iAD5 	= GetADC(5);
		g_UnitData.dat.iAD6 	= GetADC(6);
		g_UnitData.dat.iAD7 	= GetADC(7);
		g_UnitData.dat.iAD8 	= GetADC(8);

    	os_evt_set (0x0001, t_pid);

		os_dly_wait(5);                /* programmed delay                  */
	}	
}

__task void hmi(void)
{
	os_itv_set (11);
	while(1)
	{
		static float temp;
		temp					= DS18B20_Temperature();
		if (temp >= -55.0f && temp <= 125.0f)
			g_UnitData.dat.fTem		= temp;
		HMI_Handler();
		if(g_UnitCfg.dat.bIsReboot)
			lpReset( );
		os_itv_wait ();
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
	U32	adctest;    

	GP4DAT = 0x04040000;                             /* P4.2 defined as output                         */

    ADCpoweron(20000);                               /* power on ADC				                   */                       
    ADCCP                   = 0x00;                  /* Selecting ADC Channel 0                        */
    REFCON                  = 0x00;                  /* disconnect internal 2.5V reference to Vref pin */
    ADCCON                  = 0xE24;                 /* ADC Config: fADC/8, acq. time = 8 clocks       */

    for (U16 i = 0;i < sizeof(struct _UNIT_DATA_)/2;i++)
        g_UnitData.buf[i]   = 0; 

    for (U16 i = 0;i < sizeof(struct _UNIT_CFG_)/2;i++)
        g_UnitCfg.buf[i]    = g_UnitCfgInFlash[i]; 

	if(g_UnitCfg.dat.bIsReboot)
			g_UnitCfg.dat.bIsReboot = FALSE;

	if (g_UnitCfg.dat.byMbAddr == 0)
		g_UnitCfg.dat.byMbAddr = 1;

	if (g_UnitCfg.dat.uBau == 0)
		g_UnitCfg.dat.uBau = 19200;

	if (g_UnitCfg.dat.iAd4Min == g_UnitCfg.dat.iAd4Max)
	{
		g_UnitCfg.dat.iAd4Max = 3069;
		g_UnitCfg.dat.iAd4Min = 16;
	}
	if (g_UnitCfg.dat.byLimD == g_UnitCfg.dat.byLimU)
	{
		g_UnitCfg.dat.byLimU = 100;
		g_UnitCfg.dat.byLimD = 0;
	}
	if (g_UnitCfg.dat.bySrD == g_UnitCfg.dat.bySrU)
	{
		g_UnitCfg.dat.bySrU = 100;
		g_UnitCfg.dat.bySrD = 0;
	}

	Init_FEE( );

	HMI_Init();

	GP3DAT  = 0x01000000;		//p3.0 output 0,to light the lcd
//	GP3DAT  = 0x02000000;	

	adctest	= GetADC(0);

    if(adctest < 820)
	{
		//anl
	}
	else if(adctest < 2460)
	{
		eMBInit( MB_RTU, g_UnitCfg.dat.byMbAddr, 8, g_UnitCfg.dat.uBau, MB_PAR_EVEN );
		/* Enable the Modbus Protocol Stack. */
		eMBEnable( );
		t_modbus = os_tsk_create (modbus, 1);    /* start task 'modbus'              */
	}
	else if(adctest < 2460)
	{
		//can
	}
	else
	{
	   ;
	}	

	// Setup the PWM
	GP3CON |= 0x00110000;				// Enable the PWM outputs to the GPIO
	PWMCON 	= 0x01;   				// Ext ASYNC disabled
	PWMDAT0 = PWM_DAT0VALUE;  		// Period register 182uS
	PWMDAT1 = 0;    				// 0us Dead time
	SetPwmDutyCycle2(0);

	pid_Init(400, 0, 0, &g_UnitCfg);
	Controller.counts = 0;

	// Initilize Timer 3 in WatchDog mode with timeout period of   second
    T3LD  = 0x00FF;         		//  clock ticks
    T3CON = 0xE4;  					// WatchDog mode, enable timer, 32768hz clock/256
    T3CLRI = 0x55;      			// Feed Dog


//	t_feeddog = os_tsk_create (feeddog, 254);      	/* start task 'feeddog'             */
    t_pid = os_tsk_create (pid, 3);      			/* start task 'pid'                 */
	
    t_adc = os_tsk_create (adc, 2);          		/* start task 'adc'                 */
	t_hmi = os_tsk_create (hmi, 1);          		/* start task 'lcd'                 */
	
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
