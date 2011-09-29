/************************************************************************************************

 Author        : Yu

 Date          : 

 File          : calc
                                      
 Hardware      : ADuC702x

 Description   : calc
*************************************************************************************************/
#include <math.h>
#include "AppTypes.h"

extern UNIT_DATA                	g_UnitData;
extern UNIT_CFG                     g_UnitCfg;

void posHandle(void)
{
	if(!g_UnitCfg.dat.bIsActInverse)
	{
		if(g_UnitCfg.dat.iAd4Max != g_UnitCfg.dat.iAd4Min)
			g_UnitData.dat.fPos	  = 10.0f*g_UnitCfg.dat.byLimD 
								  + 10.0f*(g_UnitCfg.dat.byLimU - g_UnitCfg.dat.byLimD)*(g_UnitData.dat.iAD4 - g_UnitCfg.dat.iAd4Min)/(g_UnitCfg.dat.iAd4Max - g_UnitCfg.dat.iAd4Min);
	}
	else
	{
		if(g_UnitCfg.dat.iAd4Max != g_UnitCfg.dat.iAd4Min)
			g_UnitData.dat.fPos	  = 10.0f*g_UnitCfg.dat.byLimU 
								  + 10.0f*(g_UnitCfg.dat.byLimD - g_UnitCfg.dat.byLimU)*(g_UnitData.dat.iAD4 - g_UnitCfg.dat.iAd4Min)/(g_UnitCfg.dat.iAd4Max - g_UnitCfg.dat.iAd4Min);
	}
}

void spHandle(void)
{
	static int16_t temp;

	temp	= g_UnitData.dat.iAD5;
	switch (g_UnitCfg.dat.byInp)
    {
    case 1:
		if(g_UnitCfg.dat.iAd5Ma0 != g_UnitCfg.dat.iAd5Ma20)
			g_UnitData.dat.fSp	  = 10.0f*g_UnitCfg.dat.bySpD 
								  + 10.0f*(g_UnitCfg.dat.bySpU - g_UnitCfg.dat.bySpD)*(temp - g_UnitCfg.dat.iAd5Ma0)/(g_UnitCfg.dat.iAd5Ma20 - g_UnitCfg.dat.iAd5Ma0);
        break;
	case 2:
		if(g_UnitCfg.dat.iAd5V0 != g_UnitCfg.dat.iAd5V5)
			g_UnitData.dat.fSp	 = 10.0f*g_UnitCfg.dat.bySpD 
								 + 10.0f*(g_UnitCfg.dat.bySpU - g_UnitCfg.dat.bySpD)*(temp - g_UnitCfg.dat.iAd5V0)/(g_UnitCfg.dat.iAd5V5 - g_UnitCfg.dat.iAd5V0);
        break;
    case 3:
		if(g_UnitCfg.dat.iAd5V0 != g_UnitCfg.dat.iAd5V10)
			g_UnitData.dat.fSp	 = 10.0f*g_UnitCfg.dat.bySpD 
								 + 10.0f*(g_UnitCfg.dat.bySpU - g_UnitCfg.dat.bySpD)*(temp - g_UnitCfg.dat.iAd5V0)/(g_UnitCfg.dat.iAd5V10 - g_UnitCfg.dat.iAd5V0);
        break;
    default:
		if(g_UnitCfg.dat.iAd5Ma4 != g_UnitCfg.dat.iAd5Ma20)
			g_UnitData.dat.fSp	  = 10.0f*g_UnitCfg.dat.bySpD 
								  + 10.0f*(g_UnitCfg.dat.bySpU - g_UnitCfg.dat.bySpD)*(temp - g_UnitCfg.dat.iAd5Ma4)/(g_UnitCfg.dat.iAd5Ma20 - g_UnitCfg.dat.iAd5Ma4);
        break;
    } 
}

void p1Handle(void)
{
	if(g_UnitCfg.dat.iAd6Ma20 != g_UnitCfg.dat.iAd6Ma4)
		g_UnitData.dat.fPress1	  = 10.0f*g_UnitCfg.dat.byP1D 
							      + 10.0f*(g_UnitCfg.dat.byP1U - g_UnitCfg.dat.byP1D)*(g_UnitData.dat.iAD6 - g_UnitCfg.dat.iAd6Ma4)/(g_UnitCfg.dat.iAd6Ma20 - g_UnitCfg.dat.iAd6Ma4);
}

void p2Handle(void)
{
	if(g_UnitCfg.dat.iAd7Ma20 != g_UnitCfg.dat.iAd7Ma4)
		g_UnitData.dat.fPress2	  = 10.0f*g_UnitCfg.dat.byP2D 
							  	  + 10.0f*(g_UnitCfg.dat.byP2U - g_UnitCfg.dat.byP2D)*(g_UnitData.dat.iAD7 - g_UnitCfg.dat.iAd7Ma4)/(g_UnitCfg.dat.iAd7Ma20 - g_UnitCfg.dat.iAd7Ma4);
}

void tempHandle(void)
{
	if(g_UnitCfg.dat.byMode != 2)
	{
		if(g_UnitCfg.dat.iAd8Ma20 != g_UnitCfg.dat.iAd8Ma4)
		g_UnitData.dat.fTemp  	  = 10.0f*g_UnitCfg.dat.byTempD 
							      + 10.0f*(g_UnitCfg.dat.byTempU - g_UnitCfg.dat.byTempD)*(g_UnitData.dat.iAD8 - g_UnitCfg.dat.iAd8Ma4)/(g_UnitCfg.dat.iAd8Ma20 - g_UnitCfg.dat.iAd8Ma4);
	}
	else
	{
		if(g_UnitCfg.dat.iAd8R200 != g_UnitCfg.dat.iAd8R100)
		g_UnitData.dat.fTemp      = 10.0f*g_UnitCfg.dat.byTempD 
							      + 10.0f*(g_UnitCfg.dat.byTempU - g_UnitCfg.dat.byTempD)*(g_UnitData.dat.iAD8 - g_UnitCfg.dat.iAd8R100)/(g_UnitCfg.dat.iAd8R200 - g_UnitCfg.dat.iAd8R100);
		g_UnitData.dat.fTemp  	  = ((float) -3.90802e-1+  sqrt((float)0.152726203204 - (float)4*((float)-5.80195e-5)*((float)100-g_UnitData.dat.fTemp)))/((float)-1.16039e-4);
	}
}

void cmdHandle(float xd)
{
	static int16_t temp;

	temp	= g_UnitData.dat.iAD5;

	switch (g_UnitCfg.dat.byInp)
    {
    case 1:
		if(!g_UnitCfg.dat.bIsCmdInverse)
		{
			if(g_UnitCfg.dat.iAd5Ma0 != g_UnitCfg.dat.iAd5Ma20)
				g_UnitData.dat.fInp	  = 10.0f*g_UnitCfg.dat.bySrD + xd 
									  + 10.0f*(g_UnitCfg.dat.bySrU - g_UnitCfg.dat.bySrD)*(temp - g_UnitCfg.dat.iAd5Ma0)/(g_UnitCfg.dat.iAd5Ma20 - g_UnitCfg.dat.iAd5Ma0);
		}
		else
		{
			if(g_UnitCfg.dat.iAd5Ma0 != g_UnitCfg.dat.iAd5Ma20)
				g_UnitData.dat.fInp	  = 10.0f*g_UnitCfg.dat.bySrU - xd 
									  + 10.0f*(g_UnitCfg.dat.bySrD - g_UnitCfg.dat.bySrU)*(temp - g_UnitCfg.dat.iAd5Ma0)/(g_UnitCfg.dat.iAd5Ma20 - g_UnitCfg.dat.iAd5Ma0);
		}
        break;
	case 2:
		if(!g_UnitCfg.dat.bIsCmdInverse)
		{
			if(g_UnitCfg.dat.iAd5V0 != g_UnitCfg.dat.iAd5V5)
				g_UnitData.dat.fInp	  = 10.0f*g_UnitCfg.dat.bySrD + xd 
									  + 10.0f*(g_UnitCfg.dat.bySrU - g_UnitCfg.dat.bySrD)*(temp - g_UnitCfg.dat.iAd5V0)/(g_UnitCfg.dat.iAd5V5 - g_UnitCfg.dat.iAd5V0);
		}
		else
		{
			if(g_UnitCfg.dat.iAd5V0 != g_UnitCfg.dat.iAd5V5)
				g_UnitData.dat.fInp	  = 10.0f*g_UnitCfg.dat.bySrU - xd 
									  + 10.0f*(g_UnitCfg.dat.bySrD - g_UnitCfg.dat.bySrU)*(temp - g_UnitCfg.dat.iAd5V0)/(g_UnitCfg.dat.iAd5V5 - g_UnitCfg.dat.iAd5V0);
		}
        break;
    case 3:
		if(!g_UnitCfg.dat.bIsCmdInverse)
		{
			if(g_UnitCfg.dat.iAd5V0 != g_UnitCfg.dat.iAd5V10)
				g_UnitData.dat.fInp	  = 10.0f*g_UnitCfg.dat.bySrD + xd 
									  + 10.0f*(g_UnitCfg.dat.bySrU - g_UnitCfg.dat.bySrD)*(temp - g_UnitCfg.dat.iAd5V0)/(g_UnitCfg.dat.iAd5V10 - g_UnitCfg.dat.iAd5V0);
		}
		else
		{
			if(g_UnitCfg.dat.iAd5V0 != g_UnitCfg.dat.iAd5V10)
				g_UnitData.dat.fInp	  = 10.0f*g_UnitCfg.dat.bySrU - xd 
									  + 10.0f*(g_UnitCfg.dat.bySrD - g_UnitCfg.dat.bySrU)*(temp - g_UnitCfg.dat.iAd5V0)/(g_UnitCfg.dat.iAd5V10 - g_UnitCfg.dat.iAd5V0);
		}
        break;
    default:
		if(!g_UnitCfg.dat.bIsCmdInverse)
		{
			if(g_UnitCfg.dat.iAd5Ma4 != g_UnitCfg.dat.iAd5Ma20)
				g_UnitData.dat.fInp	  = 10.0f*g_UnitCfg.dat.bySrD + xd 
									  + 10.0f*(g_UnitCfg.dat.bySrU - g_UnitCfg.dat.bySrD)*(temp - g_UnitCfg.dat.iAd5Ma4)/(g_UnitCfg.dat.iAd5Ma20 - g_UnitCfg.dat.iAd5Ma4);
		}
		else
		{
			if(g_UnitCfg.dat.iAd5Ma4 != g_UnitCfg.dat.iAd5Ma20)
				g_UnitData.dat.fInp	  = 10.0f*g_UnitCfg.dat.bySrU - xd 
									  + 10.0f*(g_UnitCfg.dat.bySrD - g_UnitCfg.dat.bySrU)*(temp - g_UnitCfg.dat.iAd5Ma4)/(g_UnitCfg.dat.iAd5Ma20 - g_UnitCfg.dat.iAd5Ma4);
		}
        break;
    } 

	if(!g_UnitCfg.dat.bIsChaFree)
	{	
		if(g_UnitCfg.dat.byN > 0)
		{
			if(g_UnitCfg.dat.byN*g_UnitData.dat.fInp/1000.0f > 1)
				g_UnitData.dat.fCmd	= 1000.0f*log(g_UnitCfg.dat.byN*g_UnitData.dat.fInp/1000.0f)/log(g_UnitCfg.dat.byN);
			else
				g_UnitData.dat.fCmd	= 0;
		}
		else if(g_UnitCfg.dat.byN == 0)
		{
			g_UnitData.dat.fCmd	= g_UnitData.dat.fInp;
		}
		else
		{
			if((-1.0f/g_UnitCfg.dat.byN*g_UnitData.dat.fInp/1000.0f < 1)&&(-1.0f/g_UnitCfg.dat.byN*g_UnitData.dat.fInp/1000.0f > 0))
				g_UnitData.dat.fCmd	= 1000.0f*log(-1.0f/g_UnitCfg.dat.byN*g_UnitData.dat.fInp/1000.0f)/log(-1.0f/g_UnitCfg.dat.byN);
			else
				g_UnitData.dat.fCmd	= 0;
		}
	}
	else
	{
		 static uint8_t	i;
		 i	= (uint8_t)(g_UnitData.dat.fInp/50);
		 if(i > 19)
		 	i	= 19;
		 g_UnitData.dat.fCmd	= g_UnitCfg.dat.byCha[i]*10
		 						+ (g_UnitCfg.dat.byCha[i + 1] - g_UnitCfg.dat.byCha[i])/5.0f*(g_UnitData.dat.fInp - i*50);
	}
}
