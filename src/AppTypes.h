#ifndef __APP_TYPES_H__
#define __APP_TYPES_H__

#include "port.h"

#define  MAX( x, y ) ( ((x) > (y)) ? (x) : (y) ) 
#define  MIN( x, y ) ( ((x) < (y)) ? (x) : (y) )
#define  ABS(x) ((x)> 0?(x):-(x))  

typedef union _UNIT_DATA
{
    __packed struct  _UNIT_DATA_
    {
        float       fPos;
        float       fInp;
        float       fSp;
        float       fPv;
        float       fCmd;
        float       fPress1;
        float       fPress2;
        float       fTemp;
        float       fTem;
        float       fPid;
		float	    iPos1;		
		float	    iInp;
        float	    iSp;
        float	    iPress1;
        float	    iPress2;
        float	    iTemp;

        uint16_t    iAD4;
        uint16_t    iAD5;
        uint16_t    iAD6;
        uint16_t    iAD7;
        uint16_t    iAD8;
		uint16_t    iFreq;
        uint16_t    iCnt;
		uint16_t    iPos;

		uint8_t     byAd5422[3];

        mbBOOL      bInput;
		mbBOOL      bOut1;
		mbBOOL      bOut2;
		mbBOOL      bErr;
		mbBOOL      bPowerLeak;
		mbBOOL      bIsDaOut;
        mbBOOL      bZZZZ;
    } dat;
    uint16_t        buf[sizeof(struct _UNIT_DATA_)/sizeof(short)];
} UNIT_DATA;

typedef union _UNIT_CFG
{
    __packed struct _UNIT_CFG_
    {
        uint32_t    uBau; 
		uint32_t    uMenu;  
        
        int32_t 	sumError;				/* Summation of errors, used for integrate calculations*/	          
        int32_t 	maxSumError;			/* Maximum allowed sumerror, avoid overflow   */			          
        int16_t 	P_Factor;       		/* P_Factor*128 The Proportional tuning constant, multiplied with SCALING_FACTOR*/          
        int16_t 	I_Factor;       		/* P_Factor*128*0.02/Ti The Integral tuning constant, multiplied with SCALING_FACTOR*/        
        int16_t 	D_Factor;       		/* P_Factor*Td*128 The Derivative tuning constant, multiplied with SCALING_FACTOR*/        
        int16_t 	lastProcessValue;		/* Last process value, used to find derivative of process value.   */	           
        int16_t 	maxError;       		/* Maximum allowed error, avoid overflow*/

        uint16_t    iAd4Max;
        uint16_t    iAd4Min;
        uint16_t    iAd5Ma0;
        uint16_t    iAd5Ma4;
        uint16_t    iAd5Ma20;
        uint16_t    iAd5V0;
        uint16_t    iAd5V5;
        uint16_t    iAd5V10;
        uint16_t    iAd6Ma4;
        uint16_t    iAd6Ma20;
        uint16_t    iAd7Ma4;
        uint16_t    iAd7Ma20;
        uint16_t    iAd8Ma4;
        uint16_t    iAd8Ma20;
        uint16_t    iAd8R100;
        uint16_t    iAd8R200;
        uint16_t    iCode;
		uint16_t    iDbnd;
		uint16_t    iDbnd1;
		uint16_t    iDbnd2; 
		uint16_t    iPos1;
		uint16_t    iPos2;    
		uint16_t    iPidDbnd; 

        int16_t     iLimD;
        int16_t     iLimU;
        int16_t     iSrD;
        int16_t     iSrU;
        int16_t     iSpD;
        int16_t     iSpU;
        int16_t     iP1D;
        int16_t     iP1U;
        int16_t     iP2D;
        int16_t     iP2U;
        int16_t     iTempD;
        int16_t     iTempU;
        int16_t     iCutoffMin;
        int16_t     iCutoffMax;
        int16_t     iXtimeClose;
        int16_t     iXtimeOpen;
        int16_t     iSafePos;        
        int16_t     iKxD;     
        int16_t     iKxU;
        int16_t     iYbU;     
        int16_t     iYeU;
        int16_t     iAirOpen;     
        int16_t     iAirClose; 
		   
        int8_t      byN;
        int8_t      byFilt;    
        int8_t      byCha[21];

        uint8_t     byMbAddr;
        uint8_t     byMode;    /* 0 positioner 1 freq processer 2 pt100 processer 3 flow rate processer 4 inp processer 0x80 in set mode other positioner */
        uint8_t     byInp;     /* 0 4-20ma 1 0-20ma 2 0-5V 3 0-10V other 4-20ma     */
        uint8_t     byErr;     /* 0 pos safe pos 1 pos zero other none     */
        uint8_t     byAnlMode;
		uint8_t     byAnlCtl;
		uint8_t     byAnlDat;
        /*!
         * byIN bit map of portD
         *
         * <code>
         * <---------- byIN (1)------->
         *  +------------------------+
         *  | 7  6  5  4  3  2  1  0 |
         *  +------------------------+
         *  portD
         * </code>
         */
        uint8_t     byIN;      /* bit map to PortD  */
        /*!
         * bit output contrl
         *
         * <code>
         * <---------- Out (1)------->
         *  +------------------------+
         *  | 7  6  5  4  3  2  1  0 |
         *  +------------------------+
         *  7 portD.6
         *  4 portD.7
         *  6...4 err mode of portD.6
         *  2...0 err mode of portD.7
         * </code>
         */
        uint8_t     byOUT;     /* bit output contrl  */

        mbBOOL      bIsDouble;
        mbBOOL      bIsSafePosOn;
        mbBOOL      bIsManual;
        mbBOOL      bIsActInverse;
        mbBOOL      bIsCmdInverse;
        mbBOOL      bIsChaFree;
        mbBOOL      bIsPt100;
        mbBOOL      bIsReboot;
        mbBOOL      bZZZZ;    
    } dat;
    uint16_t        buf[sizeof(struct _UNIT_CFG_)/sizeof(short)];
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
    uint16_t    iVal;
    uint8_t     buf[8];
} UNIT_BUF;	  

typedef union _VARIANT
{
    UCHAR       byVal;
    uint32_t    wVal;
    float       fVal;
    UCHAR       aVals[sizeof(float)];
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
