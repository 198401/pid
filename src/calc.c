/************************************************************************************************

 Author        : Yu

 Date          : 

 File          : calc
                                      
 Hardware      : ADuC702x

 Description   : calc
*************************************************************************************************/
#include <math.h>
#include "AppTypes.h"

extern UNIT_DATA                    g_UnitData;
extern UNIT_CFG                     g_UnitCfg;

void posHandle(void)
{
    if (!g_UnitCfg.dat.bIsActInverse)
    {
        if (g_UnitCfg.dat.iAd4Max != g_UnitCfg.dat.iAd4Min)
		{
            g_UnitData.dat.fPos   = g_UnitCfg.dat.iLimD 
                                    + (g_UnitCfg.dat.iLimU - g_UnitCfg.dat.iLimD)*(g_UnitData.dat.iAD4 - g_UnitCfg.dat.iAd4Min)/(g_UnitCfg.dat.iAd4Max - g_UnitCfg.dat.iAd4Min);
    		if(g_UnitData.dat.bIsDaOut)
				g_UnitData.dat.iPos   = (2^16)*(g_UnitData.dat.iAD4 - g_UnitCfg.dat.iAd4Min)/(g_UnitCfg.dat.iAd4Max - g_UnitCfg.dat.iAd4Min);
		}
	}
    else
    {
        if (g_UnitCfg.dat.iAd4Max != g_UnitCfg.dat.iAd4Min)
		{
            g_UnitData.dat.fPos   = g_UnitCfg.dat.iLimU 
                                    + (g_UnitCfg.dat.iLimD - g_UnitCfg.dat.iLimU)*(g_UnitData.dat.iAD4 - g_UnitCfg.dat.iAd4Min)/(g_UnitCfg.dat.iAd4Max - g_UnitCfg.dat.iAd4Min);
  			if(g_UnitData.dat.bIsDaOut)
				g_UnitData.dat.iPos   = (2^16)*(g_UnitData.dat.iAD4 - g_UnitCfg.dat.iAd4Min)/(g_UnitCfg.dat.iAd4Max - g_UnitCfg.dat.iAd4Min);
		}  
	}
}

void p1Handle(void)
{
    if (g_UnitCfg.dat.iAd6Ma20 != g_UnitCfg.dat.iAd6Ma4)
	{
        g_UnitData.dat.fPress1    = g_UnitCfg.dat.iP1D 
                                    + (g_UnitCfg.dat.iP1U - g_UnitCfg.dat.iP1D)*(g_UnitData.dat.iAD6 - g_UnitCfg.dat.iAd6Ma4)/(g_UnitCfg.dat.iAd6Ma20 - g_UnitCfg.dat.iAd6Ma4);
		if(g_UnitData.dat.bIsDaOut)
			g_UnitData.dat.iPress1    = (2^16)*(g_UnitData.dat.iAD6 - g_UnitCfg.dat.iAd6Ma4)/(g_UnitCfg.dat.iAd6Ma20 - g_UnitCfg.dat.iAd6Ma4);	
	}
}

void p2Handle(void)
{
    if (g_UnitCfg.dat.iAd7Ma20 != g_UnitCfg.dat.iAd7Ma4)
	{
        g_UnitData.dat.fPress2    = g_UnitCfg.dat.iP2D 
                                    + (g_UnitCfg.dat.iP2U - g_UnitCfg.dat.iP2D)*(g_UnitData.dat.iAD7 - g_UnitCfg.dat.iAd7Ma4)/(g_UnitCfg.dat.iAd7Ma20 - g_UnitCfg.dat.iAd7Ma4);
		if(g_UnitData.dat.bIsDaOut)
			g_UnitData.dat.iPress2    = (2^16)*(g_UnitData.dat.iAD7 - g_UnitCfg.dat.iAd7Ma4)/(g_UnitCfg.dat.iAd7Ma20 - g_UnitCfg.dat.iAd7Ma4);
	}
}

void tempHandle(void)
{
    if (g_UnitCfg.dat.byMode != 2)
    {
        if (g_UnitCfg.dat.iAd8Ma20 != g_UnitCfg.dat.iAd8Ma4)
		{
            g_UnitData.dat.fTemp      = 0.1f*g_UnitCfg.dat.iTempD 
                                        + 0.1f*(g_UnitCfg.dat.iTempU - g_UnitCfg.dat.iTempD)*(g_UnitData.dat.iAD8 - g_UnitCfg.dat.iAd8Ma4)/(g_UnitCfg.dat.iAd8Ma20 - g_UnitCfg.dat.iAd8Ma4);
    		if(g_UnitData.dat.bIsDaOut)
				g_UnitData.dat.iTemp      = (2^16)*(g_UnitData.dat.iAD8 - g_UnitCfg.dat.iAd8Ma4)/(g_UnitCfg.dat.iAd8Ma20 - g_UnitCfg.dat.iAd8Ma4);
		}
	}
    else
    {
        if (g_UnitCfg.dat.iAd8R200 != g_UnitCfg.dat.iAd8R100)
            g_UnitData.dat.fTemp      = 0.1f*g_UnitCfg.dat.iTempD 
                                        + 0.1f*(g_UnitCfg.dat.iTempU - g_UnitCfg.dat.iTempD)*(g_UnitData.dat.iAD8 - g_UnitCfg.dat.iAd8R100)/(g_UnitCfg.dat.iAd8R200 - g_UnitCfg.dat.iAd8R100);
        g_UnitData.dat.fTemp      = ((float) -3.90802e-1+  sqrt((float)0.152726203204 - (float)4*((float)-5.80195e-5)*((float)100-g_UnitData.dat.fTemp)))/((float)-1.16039e-4);
    	if(g_UnitData.dat.bIsDaOut)
			g_UnitData.dat.iTemp      = (2^16)*(g_UnitData.dat.fTemp + 20)/240;
	}
}

void cmdHandle(uint8_t byMode)
{
    static int16_t temp;

    temp    = g_UnitData.dat.iAD5;

    switch (g_UnitCfg.dat.byInp)
    {
    case 1:
		if (g_UnitCfg.dat.iAd5Ma0 != g_UnitCfg.dat.iAd5Ma20)
		{
	        if(byMode == 0)
			{
				if (!g_UnitCfg.dat.bIsCmdInverse)
		                g_UnitData.dat.fInp   = g_UnitCfg.dat.iSrD
		                                        + (g_UnitCfg.dat.iSrU - g_UnitCfg.dat.iSrD)*(temp - g_UnitCfg.dat.iAd5Ma0)/(g_UnitCfg.dat.iAd5Ma20 - g_UnitCfg.dat.iAd5Ma0);
		        else
		                g_UnitData.dat.fInp   = g_UnitCfg.dat.iSrU
		                                        + (g_UnitCfg.dat.iSrD - g_UnitCfg.dat.iSrU)*(temp - g_UnitCfg.dat.iAd5Ma0)/(g_UnitCfg.dat.iAd5Ma20 - g_UnitCfg.dat.iAd5Ma0);
				g_UnitData.dat.iInp    = 200.0f*((float)temp - (float)g_UnitCfg.dat.iAd5Ma0)/((float)g_UnitCfg.dat.iAd5Ma20 - (float)g_UnitCfg.dat.iAd5Ma0);
			}
			if(byMode == 1)
				g_UnitData.dat.fSp    = g_UnitCfg.dat.iSpD 
		                                    + (g_UnitCfg.dat.iSpU - g_UnitCfg.dat.iSpD)*(temp - g_UnitCfg.dat.iAd5Ma0)/(g_UnitCfg.dat.iAd5Ma20 - g_UnitCfg.dat.iAd5Ma0);
			if(g_UnitData.dat.bIsDaOut)
				g_UnitData.dat.iSp    = (2^16)*((float)temp - (float)g_UnitCfg.dat.iAd5Ma0)/((float)g_UnitCfg.dat.iAd5Ma20 - (float)g_UnitCfg.dat.iAd5Ma0);
        }
		break;
    case 2:
		if (g_UnitCfg.dat.iAd5V0 != g_UnitCfg.dat.iAd5V5)
		{
			if(byMode == 0)
			{
				if (!g_UnitCfg.dat.bIsCmdInverse)
		                g_UnitData.dat.fInp   = g_UnitCfg.dat.iSrD 
		                                        + (g_UnitCfg.dat.iSrU - g_UnitCfg.dat.iSrD)*(temp - g_UnitCfg.dat.iAd5V0)/(g_UnitCfg.dat.iAd5V5 - g_UnitCfg.dat.iAd5V0);
		        else
		                g_UnitData.dat.fInp   = g_UnitCfg.dat.iSrU 
		                                        + (g_UnitCfg.dat.iSrD - g_UnitCfg.dat.iSrU)*(temp - g_UnitCfg.dat.iAd5V0)/(g_UnitCfg.dat.iAd5V5 - g_UnitCfg.dat.iAd5V0);
				g_UnitData.dat.iInp   = 50.0f*((float)temp - (float)g_UnitCfg.dat.iAd5V0)/((float)g_UnitCfg.dat.iAd5V5 - (float)g_UnitCfg.dat.iAd5V0);	
			}
			if(byMode == 1)
				g_UnitData.dat.fSp   = g_UnitCfg.dat.iSpD 
		                                   + (g_UnitCfg.dat.iSpU - g_UnitCfg.dat.iSpD)*(temp - g_UnitCfg.dat.iAd5V0)/(g_UnitCfg.dat.iAd5V5 - g_UnitCfg.dat.iAd5V0);
        	if(g_UnitData.dat.bIsDaOut)
				g_UnitData.dat.iSp   = (2^16)*((float)temp - (float)g_UnitCfg.dat.iAd5V0)/((float)g_UnitCfg.dat.iAd5V5 - (float)g_UnitCfg.dat.iAd5V0);	
		}
        break;								
    case 3:
		if (g_UnitCfg.dat.iAd5V0 != g_UnitCfg.dat.iAd5V10)
		{
	        if(byMode == 0)
			{
				if (!g_UnitCfg.dat.bIsCmdInverse)
		                g_UnitData.dat.fInp   = g_UnitCfg.dat.iSrD 
		                                        + (g_UnitCfg.dat.iSrU - g_UnitCfg.dat.iSrD)*(temp - g_UnitCfg.dat.iAd5V0)/(g_UnitCfg.dat.iAd5V10 - g_UnitCfg.dat.iAd5V0);
		        else
		                g_UnitData.dat.fInp   = g_UnitCfg.dat.iSrU  
		                                        + (g_UnitCfg.dat.iSrD - g_UnitCfg.dat.iSrU)*(temp - g_UnitCfg.dat.iAd5V0)/(g_UnitCfg.dat.iAd5V10 - g_UnitCfg.dat.iAd5V0);
				g_UnitData.dat.iInp   = 100.0f*((float)temp - (float)g_UnitCfg.dat.iAd5V0)/((float)g_UnitCfg.dat.iAd5V10 - (float)g_UnitCfg.dat.iAd5V0);
			}
			if(byMode == 1)
				g_UnitData.dat.fSp   = g_UnitCfg.dat.iSpD 
		                                   + (g_UnitCfg.dat.iSpU - g_UnitCfg.dat.iSpD)*(temp - g_UnitCfg.dat.iAd5V0)/(g_UnitCfg.dat.iAd5V10 - g_UnitCfg.dat.iAd5V0);
        	if(g_UnitData.dat.bIsDaOut)
				g_UnitData.dat.iSp   = (2^16)*((float)temp - (float)g_UnitCfg.dat.iAd5V0)/((float)g_UnitCfg.dat.iAd5V10 - (float)g_UnitCfg.dat.iAd5V0);
		}
        break;
    default:
        if (g_UnitCfg.dat.iAd5Ma4 != g_UnitCfg.dat.iAd5Ma20)
		{
			if(byMode == 0)
			{
				if (!g_UnitCfg.dat.bIsCmdInverse)            
		                g_UnitData.dat.fInp   = g_UnitCfg.dat.iSrD 
		                                        + (g_UnitCfg.dat.iSrU - g_UnitCfg.dat.iSrD)*(temp - g_UnitCfg.dat.iAd5Ma4)/(g_UnitCfg.dat.iAd5Ma20 - g_UnitCfg.dat.iAd5Ma4);
		        else
		                g_UnitData.dat.fInp   = g_UnitCfg.dat.iSrU 
		                                        + (g_UnitCfg.dat.iSrD - g_UnitCfg.dat.iSrU)*(temp - g_UnitCfg.dat.iAd5Ma4)/(g_UnitCfg.dat.iAd5Ma20 - g_UnitCfg.dat.iAd5Ma4);
				g_UnitData.dat.iInp    = 40.0f + 160.0f*((float)temp - (float)g_UnitCfg.dat.iAd5Ma4)/((float)g_UnitCfg.dat.iAd5Ma20 - (float)g_UnitCfg.dat.iAd5Ma4);
				if(g_UnitData.dat.iInp < 35)
					g_UnitCfg.dat.byErr = TRUE;
				else
					g_UnitCfg.dat.byErr = FALSE;
			}
			if(byMode == 1)
				g_UnitData.dat.fSp    = g_UnitCfg.dat.iSpD 
	                                    + (g_UnitCfg.dat.iSpU - g_UnitCfg.dat.iSpD)*(temp - g_UnitCfg.dat.iAd5Ma4)/(g_UnitCfg.dat.iAd5Ma20 - g_UnitCfg.dat.iAd5Ma4);
        	if(g_UnitData.dat.bIsDaOut)
				g_UnitData.dat.iSp    = (2^16)*((float)temp - (float)g_UnitCfg.dat.iAd5Ma4)/((float)g_UnitCfg.dat.iAd5Ma20 - (float)g_UnitCfg.dat.iAd5Ma4);
		}
        break;
    } 
	
	if(byMode == 0)
	{
	    if (!g_UnitCfg.dat.bIsChaFree)
	    {
	        if (g_UnitCfg.dat.byN > 0)
	        {
	            if (g_UnitCfg.dat.byN*g_UnitData.dat.fInp/1000.0f > 1)
	                g_UnitData.dat.fCmd = 1000.0f*log(g_UnitCfg.dat.byN*g_UnitData.dat.fInp/1000.0f)/log(g_UnitCfg.dat.byN);
	            else
	                g_UnitData.dat.fCmd = 0;
	        }
	        else if (g_UnitCfg.dat.byN == 0)
	        {
	            g_UnitData.dat.fCmd = g_UnitData.dat.fInp;
	        }
	        else
	        {
	            if ((-1.0f/g_UnitCfg.dat.byN*g_UnitData.dat.fInp/1000.0f < 1)&&(-1.0f/g_UnitCfg.dat.byN*g_UnitData.dat.fInp/1000.0f > 0))
	                g_UnitData.dat.fCmd = 1000.0f*log(-1.0f/g_UnitCfg.dat.byN*g_UnitData.dat.fInp/1000.0f)/log(-1.0f/g_UnitCfg.dat.byN);
	            else
	                g_UnitData.dat.fCmd = 0;
	        }
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
