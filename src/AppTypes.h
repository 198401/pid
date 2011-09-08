#ifndef __APP_TYPES_H__
#define __APP_TYPES_H__

#include "port.h"

#define  MAX( x, y ) ( ((x) > (y)) ? (x) : (y) ) 
#define  MIN( x, y ) ( ((x) < (y)) ? (x) : (y) ) 

typedef union _UNIT_DATA
{
    __packed struct  _UNIT_DATA_
    {
        float   	fPos;
		float   	fInp;
        float   	fCmd;
        float   	fPress1;
        float   	fPress2;
		float   	fTemp;
		float   	fTem;
		float   	fPid;
        
		uint16_t   	iAD4;
        uint16_t    iAD5;
        uint16_t    iAD6;
		uint16_t    iAD7;
        uint16_t    iAD8;

        mbBOOL    	bOK;
        mbBOOL    	bZ;
        mbBOOL  	bZZZZ;
    } dat;
    uint16_t    	buf[sizeof(struct _UNIT_DATA_)/sizeof(short)];
} UNIT_DATA;

typedef union _UNIT_CFG
{
    __packed struct _UNIT_CFG_
    {
		uint32_t 	uBau;
		
	    //! Summation of errors, used for integrate calculations
	    int32_t sumError;
		//! Maximum allowed sumerror, avoid overflow
	    int32_t maxSumError;
	    //! The Proportional tuning constant, multiplied with SCALING_FACTOR
	    int16_t P_Factor;		// P_Factor*128
	    //! The Integral tuning constant, multiplied with SCALING_FACTOR
	    int16_t I_Factor;		// P_Factor*128*0.05/Ti
	    //! The Derivative tuning constant, multiplied with SCALING_FACTOR
	    int16_t D_Factor;		// P_Factor*Td*128
		//! Last process value, used to find derivative of process value.
	    int16_t lastProcessValue;
	    //! Maximum allowed error, avoid overflow
	    int16_t maxError;  	    

		uint16_t   	iAd4Max;
		uint16_t   	iAd4Min;
		uint16_t   	iAd5Ma0;
		uint16_t   	iAd5Ma4;
		uint16_t   	iAd5Ma20;
		uint16_t   	iAd5V0;
		uint16_t   	iAd5V5;
		uint16_t   	iAd5V10;
		uint16_t   	iAd6Ma4;
		uint16_t   	iAd6Ma20;
		uint16_t   	iAd7Ma4;
		uint16_t   	iAd7Ma20;
		uint16_t   	iAd8Ma4;
		uint16_t   	iAd8Ma20;
		uint16_t   	iAd8R100;
		uint16_t   	iAd8R200;
		uint16_t   	iCode;

        uint8_t   	byMbAddr;

		int8_t		byLimD;
		int8_t		byLimU;
		int8_t		bySrD;
		int8_t		bySrU;
		int8_t		byP1D;
		int8_t		byP1U;
		int8_t		byP2D;
		int8_t		byP2U;
		int8_t		byTempD;
		int8_t		byTempU;
		int8_t		byCutoffMin;
		int8_t		byCutoffMax;
		int8_t		byXtimeClose;
		int8_t		byXtimeOpen;
		int8_t		bySafePos;
		int8_t		byDbnd;	   	// div 10
		int8_t		byPidDbnd;  // div 10
		int8_t		byKxD;	   
		int8_t		byKxU;
		int8_t		byYbU;	   
		int8_t		byYeU;
		int8_t		byAirOpen;	   
		int8_t		byAirClose;	   
		int8_t		byN;
		int8_t		byFilt;	   
		int8_t		byCha[21];
		
		uint8_t   	byMode;	   //0 positioner 1 freq processer 2 pt100 processer 3 flow rate processer 2 inp processer other positioner
		uint8_t   	byInp;	   //0 4-20ma 1 0-20ma 2 0-5V 3 0-10V other pt100
		uint8_t   	byErr;	   //0 none 1 pos safe pos 2 pos zero other none

		mbBOOL		bIsManual;
		mbBOOL		bIsActInverse;
		mbBOOL		bIsCmdInverse;
		mbBOOL  	bIsChaFree;
		mbBOOL  	bIsPt100;
		mbBOOL  	bIsReboot;
		mbBOOL  	bZZZZ;    
    } dat;
    uint16_t    	buf[sizeof(struct _UNIT_CFG_)/sizeof(short)];
} UNIT_CFG;

typedef union _UNIT_BUF
{
    uint32_t    wVal;
    float       fVal;
    double      dVal;
    struct _FLOAT_PAIR
    {
        float   fVal1;
        float   fVal2;
    } f_pair; 
    uint16_t  	iVal;
    float       buf[8];
} UNIT_BUF;


typedef union _VARIANT
{
    UCHAR    	byVal;
    uint32_t    wVal;
    float    	fVal;
    UCHAR    	aVals[sizeof(float)];
} VARIANT;

typedef union _UN_W_B_
{
    uint32_t     wVal;
    struct _S_B_
    {
        uint8_t  byHi;
        uint8_t  byLo;
    } S_B;
} UN_W_B;

typedef struct _MENU_ITEM
{
    uint8_t   byMenuItemID;
    uint8_t   byChildMenuItems;
    struct  _MENU_ITEM* pChildMenu; 
    struct  _MENU_ITEM* pParentMenu;
    void (*KeyboardHandler)(uint8_t);
    void (*DisplayHandler)(void);
    void (*OnOpeningHandler)(void);
} MENU_ITEM; 

typedef struct _MENU_CTL_BLOCK
{
    MENU_ITEM*  pMenu;
    uint8_t   byStartMenuItemID;
//	uint8_t	byHighlightedMenuItemID;
} MENU_CTL_BLOCK;

#endif
