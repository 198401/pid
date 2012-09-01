/************************************************************************************************

 Author        : Yu

 Date          : 

 File          : calc
                                      
 Hardware      : ADuC702x

 Description   : calc
*************************************************************************************************/
#include <math.h>
#include <ADuC7024.H>
#include <RTL.h>
#include "AppTypes.h"


extern UNIT_DATA                    g_UnitData;
extern UNIT_CFG                     g_UnitCfg;

U32 static adc_read_data(U8 adc_channel) 
{   
    U32 adc_value_ave      = 0; 

    T3CLRI = 0x55;
    ADCCP                    = adc_channel;                
    ADCCON                  |= (1UL<<7);                  

	tsk_lock (); 
    while (!ADCSTA);                                     
    adc_value_ave   += (ADCDAT >> 16);
	adc_value_ave      = 0; 
    tsk_unlock ();

    for (U16 i=0;i<128;i++)
    {
        tsk_lock (); 
        while (!ADCSTA);                                     
        adc_value_ave   += (ADCDAT >> 16);
        tsk_unlock ();
    } 
    ADCCON                  &= ~(1UL<<7);                 

    adc_value_ave          >>= 7;   
    return adc_value_ave; 
} 

unsigned int adTest(void)
{
	return adc_read_data(0);
}

void posHandle(void)
{
    static int16_t temp;

    g_UnitData.dat.iAD4     = adc_read_data(4);
	temp    = g_UnitData.dat.iAD4;

	if ((g_UnitCfg.dat.iAd4Max != g_UnitCfg.dat.iAd4Min)&&(g_UnitCfg.dat.iLimD != g_UnitCfg.dat.iLimU))
	{
		if (!g_UnitCfg.dat.bIsActInverse)
			g_UnitData.dat.iPos   = 1000.0f*(1000.0f*temp+(g_UnitCfg.dat.iLimD-1000.0f)*g_UnitCfg.dat.iAd4Min-g_UnitCfg.dat.iLimD*g_UnitCfg.dat.iAd4Max)/(g_UnitCfg.dat.iLimU - g_UnitCfg.dat.iLimD)/(g_UnitCfg.dat.iAd4Max - g_UnitCfg.dat.iAd4Min);	                		
		else
			g_UnitData.dat.iPos   = 1000.0f*(1000.0f*temp+(g_UnitCfg.dat.iLimD-1000.0f)*g_UnitCfg.dat.iAd4Max-g_UnitCfg.dat.iLimD*g_UnitCfg.dat.iAd4Min)/(g_UnitCfg.dat.iLimU - g_UnitCfg.dat.iLimD)/(g_UnitCfg.dat.iAd4Min - g_UnitCfg.dat.iAd4Max);

		g_UnitData.dat.fPos   = 1000.0f*(1000.0f*temp+(g_UnitCfg.dat.iLimD-1000.0f)*g_UnitCfg.dat.iAd4Min-g_UnitCfg.dat.iLimD*g_UnitCfg.dat.iAd4Max)/(g_UnitCfg.dat.iLimU - g_UnitCfg.dat.iLimD)/(g_UnitCfg.dat.iAd4Max - g_UnitCfg.dat.iAd4Min);
		if(g_UnitData.dat.bIsDaOut)
			g_UnitData.dat.iPos1    = (float)(65536.0f)*((float)temp - (float)g_UnitCfg.dat.iAd4Min)/((float)g_UnitCfg.dat.iAd4Max - (float)g_UnitCfg.dat.iAd4Min);
	}
}

void p1Handle(void)
{
    g_UnitData.dat.iAD6     = adc_read_data(6);
	if (g_UnitCfg.dat.iAd6Ma20 != g_UnitCfg.dat.iAd6Ma4)
	{
        g_UnitData.dat.fPress1    = g_UnitCfg.dat.iP1D 
                                    + (g_UnitCfg.dat.iP1U - g_UnitCfg.dat.iP1D)*(g_UnitData.dat.iAD6 - g_UnitCfg.dat.iAd6Ma4)/(g_UnitCfg.dat.iAd6Ma20 - g_UnitCfg.dat.iAd6Ma4);
		if(g_UnitData.dat.bIsDaOut)
			g_UnitData.dat.iPress1    = (float)(65536.0f)*((float)g_UnitData.dat.iAD6 - (float)g_UnitCfg.dat.iAd6Ma4)/((float)g_UnitCfg.dat.iAd6Ma20 - (float)g_UnitCfg.dat.iAd6Ma4);	
	}
}

void p2Handle(void)
{
    g_UnitData.dat.iAD7     = adc_read_data(7);
	if (g_UnitCfg.dat.iAd7Ma20 != g_UnitCfg.dat.iAd7Ma4)
	{
        g_UnitData.dat.fPress2    = g_UnitCfg.dat.iP2D 
                                    + (g_UnitCfg.dat.iP2U - g_UnitCfg.dat.iP2D)*(g_UnitData.dat.iAD7 - g_UnitCfg.dat.iAd7Ma4)/(g_UnitCfg.dat.iAd7Ma20 - g_UnitCfg.dat.iAd7Ma4);
		if(g_UnitData.dat.bIsDaOut)
			g_UnitData.dat.iPress2    = (float)(65536.0f)*((float)g_UnitData.dat.iAD7 - (float)g_UnitCfg.dat.iAd7Ma4)/((float)g_UnitCfg.dat.iAd7Ma20 - (float)g_UnitCfg.dat.iAd7Ma4);
	}
}

void tempHandle(void)
{
    if (g_UnitCfg.dat.byMode != 2)
    {
        g_UnitData.dat.iAD8     = adc_read_data(9);
		if (g_UnitCfg.dat.iAd8Ma20 != g_UnitCfg.dat.iAd8Ma4)
		{
            g_UnitData.dat.fTemp      = 0.1f*g_UnitCfg.dat.iTempD 
                                        + 0.1f*(g_UnitCfg.dat.iTempU - g_UnitCfg.dat.iTempD)*(g_UnitData.dat.iAD8 - g_UnitCfg.dat.iAd8Ma4)/(g_UnitCfg.dat.iAd8Ma20 - g_UnitCfg.dat.iAd8Ma4);
    		if(g_UnitData.dat.bIsDaOut)
				g_UnitData.dat.iTemp      = (float)(65536.0f)*((float)g_UnitData.dat.iAD8 - (float)g_UnitCfg.dat.iAd8Ma4)/((float)g_UnitCfg.dat.iAd8Ma20 - (float)g_UnitCfg.dat.iAd8Ma4);
		}
	}
    else
    {
        g_UnitData.dat.iAD8     = adc_read_data(8);
		if (g_UnitCfg.dat.iAd8R200 != g_UnitCfg.dat.iAd8R100)
            g_UnitData.dat.fTemp      = 0.1f*g_UnitCfg.dat.iTempD 
                                        + 0.1f*(g_UnitCfg.dat.iTempU - g_UnitCfg.dat.iTempD)*(g_UnitData.dat.iAD8 - g_UnitCfg.dat.iAd8R100)/(g_UnitCfg.dat.iAd8R200 - g_UnitCfg.dat.iAd8R100);
        g_UnitData.dat.fTemp      = ((float) -3.90802e-1+  sqrt((float)0.152726203204 - (float)4*((float)-5.80195e-5)*((float)100-g_UnitData.dat.fTemp)))/((float)-1.16039e-4);
    	if(g_UnitData.dat.bIsDaOut)
			g_UnitData.dat.iTemp      = (float)(65536.0f)*(g_UnitData.dat.fTemp + 20.0f)/240.0f;
	}
}

void cmdHandle(uint8_t byMode)
{
    static int16_t temp;

    if ((g_UnitCfg.dat.byInp == 2)||(g_UnitCfg.dat.byInp == 3))
		g_UnitData.dat.iAD5     = adc_read_data(2);
    else
		g_UnitData.dat.iAD5     = adc_read_data(5);

	temp    = g_UnitData.dat.iAD5;

	if (g_UnitCfg.dat.iSrU != g_UnitCfg.dat.iSrD)
	{ 
	    switch (g_UnitCfg.dat.byInp)
	    {
	    case 1:
			if (g_UnitCfg.dat.iAd5Ma0 != g_UnitCfg.dat.iAd5Ma20)
			{
		        if(byMode == 0)
				{
					if (!g_UnitCfg.dat.bIsCmdInverse)
							g_UnitData.dat.fInp   = 1000.0f*(1000.0f*temp+(g_UnitCfg.dat.iSrD-1000.0f)*g_UnitCfg.dat.iAd5Ma0-g_UnitCfg.dat.iSrD*g_UnitCfg.dat.iAd5Ma20)/(g_UnitCfg.dat.iSrU - g_UnitCfg.dat.iSrD)/(g_UnitCfg.dat.iAd5Ma20 - g_UnitCfg.dat.iAd5Ma0);
			        else
			                g_UnitData.dat.fInp   = 1000.0f*(1000.0f*temp+(g_UnitCfg.dat.iSrD-1000.0f)*g_UnitCfg.dat.iAd5Ma20-g_UnitCfg.dat.iSrD*g_UnitCfg.dat.iAd5Ma0)/(g_UnitCfg.dat.iSrU - g_UnitCfg.dat.iSrD)/(g_UnitCfg.dat.iAd5Ma0 - g_UnitCfg.dat.iAd5Ma20);
					g_UnitData.dat.iInp    = 200.0f*((float)temp - (float)g_UnitCfg.dat.iAd5Ma0)/((float)g_UnitCfg.dat.iAd5Ma20 - (float)g_UnitCfg.dat.iAd5Ma0);
				}
				if(byMode == 1)
					g_UnitData.dat.fSp    = g_UnitCfg.dat.iSpD 
			                                    + (g_UnitCfg.dat.iSpU - g_UnitCfg.dat.iSpD)*(temp - g_UnitCfg.dat.iAd5Ma0)/(g_UnitCfg.dat.iAd5Ma20 - g_UnitCfg.dat.iAd5Ma0);
				if(g_UnitData.dat.bIsDaOut)
					g_UnitData.dat.iSp    = (float)(65536.0f)*((float)temp - (float)g_UnitCfg.dat.iAd5Ma0)/((float)g_UnitCfg.dat.iAd5Ma20 - (float)g_UnitCfg.dat.iAd5Ma0);
	        }
			break;
	    case 2:
			if (g_UnitCfg.dat.iAd5V0 != g_UnitCfg.dat.iAd5V5)
			{
				if(byMode == 0)
				{
					if (!g_UnitCfg.dat.bIsCmdInverse)
			                g_UnitData.dat.fInp   = 1000.0f*(1000.0f*temp+(g_UnitCfg.dat.iSrD-1000.0f)*g_UnitCfg.dat.iAd5V0-g_UnitCfg.dat.iSrD*g_UnitCfg.dat.iAd5V5)/(g_UnitCfg.dat.iSrU - g_UnitCfg.dat.iSrD)/(g_UnitCfg.dat.iAd5V5 - g_UnitCfg.dat.iAd5V0);
			        else
			                g_UnitData.dat.fInp   = 1000.0f*(1000.0f*temp+(g_UnitCfg.dat.iSrD-1000.0f)*g_UnitCfg.dat.iAd5V5-g_UnitCfg.dat.iSrD*g_UnitCfg.dat.iAd5V0)/(g_UnitCfg.dat.iSrU - g_UnitCfg.dat.iSrD)/(g_UnitCfg.dat.iAd5V0 - g_UnitCfg.dat.iAd5V5);
					g_UnitData.dat.iInp   = 50.0f*((float)temp - (float)g_UnitCfg.dat.iAd5V0)/((float)g_UnitCfg.dat.iAd5V5 - (float)g_UnitCfg.dat.iAd5V0);	
				}
				if(byMode == 1)
					g_UnitData.dat.fSp   = g_UnitCfg.dat.iSpD 
			                                   + (g_UnitCfg.dat.iSpU - g_UnitCfg.dat.iSpD)*(temp - g_UnitCfg.dat.iAd5V0)/(g_UnitCfg.dat.iAd5V5 - g_UnitCfg.dat.iAd5V0);
	        	if(g_UnitData.dat.bIsDaOut)
					g_UnitData.dat.iSp   = (float)(65536.0f)*((float)temp - (float)g_UnitCfg.dat.iAd5V0)/((float)g_UnitCfg.dat.iAd5V5 - (float)g_UnitCfg.dat.iAd5V0);	
			}
	        break;								
	    case 3:
			if (g_UnitCfg.dat.iAd5V0 != g_UnitCfg.dat.iAd5V10)
			{
		        if(byMode == 0)
				{
					if (!g_UnitCfg.dat.bIsCmdInverse)
			                g_UnitData.dat.fInp   = 1000.0f*(1000.0f*temp+(g_UnitCfg.dat.iSrD-1000.0f)*g_UnitCfg.dat.iAd5V0-g_UnitCfg.dat.iSrD*g_UnitCfg.dat.iAd5V10)/(g_UnitCfg.dat.iSrU - g_UnitCfg.dat.iSrD)/(g_UnitCfg.dat.iAd5V10 - g_UnitCfg.dat.iAd5V0);
			        else
			                g_UnitData.dat.fInp   = 1000.0f*(1000.0f*temp+(g_UnitCfg.dat.iSrD-1000.0f)*g_UnitCfg.dat.iAd5V10-g_UnitCfg.dat.iSrD*g_UnitCfg.dat.iAd5V0)/(g_UnitCfg.dat.iSrU - g_UnitCfg.dat.iSrD)/(g_UnitCfg.dat.iAd5V0 - g_UnitCfg.dat.iAd5V10);
					g_UnitData.dat.iInp   = 100.0f*((float)temp - (float)g_UnitCfg.dat.iAd5V0)/((float)g_UnitCfg.dat.iAd5V10 - (float)g_UnitCfg.dat.iAd5V0);
				}
				if(byMode == 1)
					g_UnitData.dat.fSp   = g_UnitCfg.dat.iSpD 
			                                   + (g_UnitCfg.dat.iSpU - g_UnitCfg.dat.iSpD)*(temp - g_UnitCfg.dat.iAd5V0)/(g_UnitCfg.dat.iAd5V10 - g_UnitCfg.dat.iAd5V0);
	        	if(g_UnitData.dat.bIsDaOut)
					g_UnitData.dat.iSp   = (float)(65536.0f)*((float)temp - (float)g_UnitCfg.dat.iAd5V0)/((float)g_UnitCfg.dat.iAd5V10 - (float)g_UnitCfg.dat.iAd5V0);
			}
	        break;
	    default:
	        if (g_UnitCfg.dat.iAd5Ma4 != g_UnitCfg.dat.iAd5Ma20)
			{
				if(byMode == 0)
				{
					if (!g_UnitCfg.dat.bIsCmdInverse)            
			                g_UnitData.dat.fInp   = 1000.0f*(1000.0f*temp+(g_UnitCfg.dat.iSrD-1000.0f)*g_UnitCfg.dat.iAd5Ma4-g_UnitCfg.dat.iSrD*g_UnitCfg.dat.iAd5Ma20)/(g_UnitCfg.dat.iSrU - g_UnitCfg.dat.iSrD)/(g_UnitCfg.dat.iAd5Ma20 - g_UnitCfg.dat.iAd5Ma4);
			        else
			                g_UnitData.dat.fInp   = 1000.0f*(1000.0f*temp+(g_UnitCfg.dat.iSrD-1000.0f)*g_UnitCfg.dat.iAd5Ma20-g_UnitCfg.dat.iSrD*g_UnitCfg.dat.iAd5Ma4)/(g_UnitCfg.dat.iSrU - g_UnitCfg.dat.iSrD)/(g_UnitCfg.dat.iAd5Ma4 - g_UnitCfg.dat.iAd5Ma20);
					g_UnitData.dat.iInp    = 40.0f + 160.0f*((float)temp - (float)g_UnitCfg.dat.iAd5Ma4)/((float)g_UnitCfg.dat.iAd5Ma20 - (float)g_UnitCfg.dat.iAd5Ma4); 
					if(g_UnitData.dat.iInp < 35)
						g_UnitCfg.dat.bIsSafePosOn = TRUE;
					else
						g_UnitCfg.dat.bIsSafePosOn = FALSE;
					if((g_UnitCfg.dat.byOUT&0x70) == 0x40)
					{
						if(g_UnitData.dat.iInp < 35)
							g_UnitData.dat.bOut1 =TRUE; 
						else
							g_UnitData.dat.bOut1 =FALSE;   	
					}
					if((g_UnitCfg.dat.byOUT&0x07) == 0x04)
					{
						if(g_UnitData.dat.iInp < 35)
							g_UnitData.dat.bOut2 =TRUE; 
						else
							g_UnitData.dat.bOut2 =FALSE;   	
					}
				}
				if(byMode == 1)
					g_UnitData.dat.fSp    = g_UnitCfg.dat.iSpD 
		                                    + (g_UnitCfg.dat.iSpU - g_UnitCfg.dat.iSpD)*(temp - g_UnitCfg.dat.iAd5Ma4)/(g_UnitCfg.dat.iAd5Ma20 - g_UnitCfg.dat.iAd5Ma4);
	        	if(g_UnitData.dat.bIsDaOut)
					g_UnitData.dat.iSp    = (float)(65536.0f)*((float)temp - (float)g_UnitCfg.dat.iAd5Ma4)/((float)g_UnitCfg.dat.iAd5Ma20 - (float)g_UnitCfg.dat.iAd5Ma4);
			}
	        break;
	    } 
	}
	
	if(byMode == 0)
	{
	    if (!g_UnitCfg.dat.bIsChaFree)
	    {                  
	        if (g_UnitCfg.dat.byN == 0)
	            g_UnitData.dat.fCmd = g_UnitData.dat.fInp;
	        else if(g_UnitCfg.dat.byN > 0)
	        	g_UnitData.dat.fCmd = 1000.0f*log(1 + g_UnitData.dat.fInp/g_UnitCfg.dat.byN/1000.0f)/log(1 + 1/g_UnitCfg.dat.byN);
			else
				g_UnitData.dat.fCmd = 1000.0f*log(1 - g_UnitData.dat.fInp*g_UnitCfg.dat.byN/1000.0f)/log(1 - g_UnitCfg.dat.byN);
	    }
	    else
	    {
	        static uint8_t i;
	        i  = (uint8_t)(g_UnitData.dat.fInp/50);
	        if (i > 19)
	            i   = 19;
	        g_UnitData.dat.fCmd    = g_UnitCfg.dat.byCha[i]*10
	                                 + (g_UnitCfg.dat.byCha[i + 1] - g_UnitCfg.dat.byCha[i])/5.0f*(g_UnitData.dat.fInp - i*50);
	    }
	}
}
