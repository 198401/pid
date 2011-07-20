#ifndef __APP_TYPES_H__
#define __APP_TYPES_H__

#include "port.h"

typedef union _UNIT_DATA
{
    struct _UNIT_DATA_
    {
        float   fPos;
        float   fSet;
        float   fPress1;
        float   fPress2;
		float   fTemp;
		float   fTem;
        
		uint32_t   	iAD4;
        uint32_t    iAD5;
        uint32_t    iAD6;
		uint32_t    iAD7;
        uint32_t    iAD8;

        mbBOOL    bOK;
        mbBOOL    bZ;
        mbBOOL  bReboot;
    } dat;
    UCHAR    buf[sizeof(struct _UNIT_DATA_)];
} UNIT_DATA;

typedef union _UNIT_CFG
{
    struct _UNIT_CFG_
    {
        float   fPosMin;
        float   fPosMax;

		float   fSetMin;
        float   fSetMax;

		float   fPress1Min;
        float   fPress1Max;

		float   fPress2Min;
        float   fPress2Max;

        float   fPos_Lic[3];
		float   fSet_Lic[3];
		float   fPress1_Lic[3];
		float   fPress2_Lic[3];
        float   fg_Tzc[3];
        float   fg_Tsc[3];

		ULONG 	uBau; 

        UCHAR   byMbAddr;
		
		mbBOOL  bIsVoltage;        
    } dat;
    unsigned short    buf[sizeof(struct _UNIT_CFG_)/sizeof(short)];
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
    unsigned short  iVal;
    float       buf[8];
} UNIT_BUF;


typedef union _VARIANT
{
    UCHAR    byVal;
    uint32_t    wVal;
    float    fVal;
    UCHAR    aVals[sizeof(float)];
} VARIANT;

typedef union _UN_W_B_
{
    uint32_t     wVal;
    struct _S_B_
    {
        UCHAR    byHi;
        UCHAR    byLo;
    } S_B;
} UN_W_B;


typedef struct _MENU_ITEM
{
    UCHAR   byMenuItemID;
    UCHAR   byChildMenuItems;
    struct  _MENU_ITEM* pChildMenu; 
    struct  _MENU_ITEM* pParentMenu;
    void (*KeyboardHandler)(UCHAR);
    void (*DisplayHandler)(void);
    void (*OnOpeningHandler)(void);
} MENU_ITEM; 

typedef struct _MENU_CTL_BLOCK
{
    MENU_ITEM*  pMenu;
    UCHAR   byStartMenuItemID;
//	UCHAR	byHighlightedMenuItemID;
} MENU_CTL_BLOCK;

#endif
