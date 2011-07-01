#ifndef __APP_TYPES_H__
#define __APP_TYPES_H__

#include "port.h"

typedef union _UNIT_DATA
{
    struct _UNIT_DATA_
    {
        float   fAD0;
        float   fP;
        float   fFIXP;
        float   fTEMP;
        float   fZ;

        uint32_t    wCrcErrors;
        uint32_t    wFrmErrors;

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
        float   fFmin;
        float   fFmax;

        float   fg_Sfa;
        float   fg_Sza;

        float   fg_Ldw;
        float   fg_Lwt;

        float   fProp;
        float   fDamping;

        float   fFixed;
        float   fFixed1;     
        float   fFixed2;

        float   fg_Lic[4];
        float   fg_Tzc[3];
        float   fg_Tsc[3];

		ULONG 	uBau;

        UCHAR   bySn[16];
        UCHAR   byMessage[24];
        UCHAR   byTag[18];
        UCHAR   byDay;
        UCHAR   byMonth;
        UCHAR   byYear;
        UCHAR   byManufacturerID;
        UCHAR   byDeviceType;
        UCHAR   byDeviceIdentifier0;
        UCHAR   byDeviceIdentifier1;
        UCHAR   byDeviceIdentifier2;
        UCHAR   byShortFrameAddr;
        UCHAR   byMbAddr;
        UCHAR   byMode;             
        UCHAR   byBau;              
        UCHAR   byUnit;             
        UCHAR   byPga;
		UCHAR   byNone;
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
