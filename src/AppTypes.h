#ifndef __APP_TYPES_H__
#define __APP_TYPES_H__

#include "port.h"

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
		float   	fPress1_Lic[2];
		float   	fPress1_cLic[2];
		float   	fPress2_Lic[2];
		float   	fPress2_cLic[2];
		float   	fTemp_Lic[2];
		float   	fTemp_vLic[2];
		float   	fTemp_cLic[2];
		
		uint32_t 	uBau;

		uint16_t   	iAd4Max;
		uint16_t   	iAd4Min;
		uint16_t   	iAd5Ma0;
		uint16_t   	iAd5Ma4;
		uint16_t   	iAd5Ma20;
		uint16_t   	iAd5V0;
		uint16_t   	iAd5V5;
		uint16_t   	iAd5V10;

        uint8_t   	byMbAddr;

		int8_t		byLimD;
		int8_t		byLimU;
		int8_t		bySrD;
		int8_t		bySrU;
		int8_t		byCutoffMin;
		int8_t		byCutoffMax;
		int8_t		byXtimeClose;
		int8_t		byXtimeOpen;
		int8_t		bySafePos;
		int8_t		byDbnd;	   // div 10
		int8_t		byKxD;	   
		int8_t		byKxU;
		int8_t		byYbU;	   
		int8_t		byYeU;
		int8_t		byAirOpen;	   
		int8_t		byAirClose;	   
		int8_t		byN;	   
		int8_t		byCha[21];
		
		uint8_t   	byMode;	   //0 positioner 1 processer other positioner
		uint8_t   	byTemp;	   //0 pt100 1 voltage 2 current other pt100
		uint8_t   	byInp;	   //0 4-20ma 1 0-20ma 2 0-5V 3 0-10V other pt100

		mbBOOL		bIsManual;
		mbBOOL		bIsActInverse;
		mbBOOL		bIsCmdInverse;
		mbBOOL  	bIsChaFree;
		mbBOOL  	bIsP1Voltage;
		mbBOOL  	bIsP2Voltage;
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
