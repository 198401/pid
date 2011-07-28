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
        
		uint16_t   	iAD4;
        uint16_t    iAD5;
        uint16_t    iAD6;
		uint16_t    iAD7;
        uint16_t    iAD8;

        mbBOOL    bOK;
        mbBOOL    bZ;
        mbBOOL  bReboot;
    } dat;
    unsigned short    buf[sizeof(struct _UNIT_DATA_)/sizeof(short)];
} UNIT_DATA;

typedef union _UNIT_CFG
{
    struct _UNIT_CFG_
    {
        float   fPos_Lic[2];
		float   fSet_Lic[2];
		float   fSet_cLic[2];
		float   fPress1_Lic[2];
		float   fPress1_cLic[2];
		float   fPress2_Lic[2];
		float   fPress2_cLic[2];
		float   fTemp_Lic[2];
		float   fTemp_vLic[2];
		float   fTemp_cLic[2];

		ULONG 	uBau; 
        UCHAR   byMbAddr;
		
		UCHAR   byTemp;	   //0 pt100 1 voltage 2 current other pt100

		mbBOOL  bIsVoltage; 
		mbBOOL  bIsP1Voltage;
		mbBOOL  bIsP2Voltage;    
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
