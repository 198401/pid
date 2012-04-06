/************************************************************************************************

 Author        : Yu

 Date          : 

 File          : hmi
                                      
 Hardware      : ADuC702x

 Description   : hmi
*************************************************************************************************/
#include <RTL.h>
#include <aduc7024.h>
#include <stdio.h>
#include <stdbool.h>

#include "Consts.h"

#include "Stack.h"

#include "HT1622.h"
#include "AppTypes.h"

#define IS_KEY0_DOWN()			((GP3DAT & BIT03) == 0)
#define IS_KEY1_DOWN()			((GP0DAT & BIT03) == 0)
#define IS_KEY2_DOWN()			((GP0DAT & BIT04) == 0)

#define KB_BUF_SIZE				63

const uint8_t display[][8] =    
{
    "ACT[FUNC",
    "[[[INPUT",
    "[[CHRACT",
    "[[CUTOFF",
    "[DIR[CMD",
    "[DIR[ACT",
    "[SPLTRNG",
    "[X[LIMIT",
    "[[X[TIME",
    "X[CONTRL",
    "P[CONTRL",
    "[[[[CODE",
    "[SAFEPOS",
    "[[SIGERR",
    "[[BIN[IN",
    "[[OUTPUT",
    "CAL[USER",
    "[SETFACT",
    "[[SER[IO",
    "ADDFUNCT",
    "[[MANUAL",
    "[[X[TUNE",
    "[[[[[END",

    "FUNCSNGL",
    "FUNCDOUB",
    "[ACT[END",
    "[[4[20MA",
    "[[0[20MA",
    "[[[0[10V",
    "[[[[0[5V",
    "INPUTEND",
    "[CHR[LIN",
    "CHR[1[25",
    "CHR[1[33",
    "CHR[1[50",
    "CHR[25[1",
    "CHR[33[1",
    "CHR[50[1",
    "CHR[FREE",
    "[CHR[END",
    "[[C[RISE",
    "[[C[FALL",
    "DIRC[END",
    "[[A[RISE",
    "[[A[FALL",
    "DIRA[END",
    "[XCO[END",
    "XCO[DBMD",
    "XCO[PARA",                         
    "AUTOTUNE",
    "TUNE[POS",
    "TUNE[PWM",
    "TUNE[AIR",
    "TUNE[END",
    "[[FUN[ON",
    "[SPOS[ON",
    "[[[[[OFF",
    "[SIG[END",
    "[[[[9600",
    "[[[19200",
    "[[[38400",
    "[[[[1200",
    "[[[[2400",
    "[[[[4800",
    "[SER[END",
    "CODE[END",
    "CODE[KEY",
    "CODEMENU",
    "CODEGLOB",                         
    "BIN[SPOS",
    "[BIN[M[A",
    "[BIN[END",
    "[OUT[ANL",
    "[ANL[SET",
    "OUT[BIN1",
    "OUT[BIN2",
    "[OUT[END",
    "[CAL[END",
    "[CAL[POS",
    "[CAL[INP",
    "CAL[FACT",                         
    "[PCO[END",
    "PCO[DBND",
    "PCO[PARA",
    "PCO[SETP",
    "[PCO[INP",
    "PCO[FILT",
    "PCO[SCAL",
    "PCO[TUNE",
    "[[PCO[KV",
    "PCO[LEAK",                         
    "NORM[OPN",
    "NORM[CLS",
    "SPOS[END",
    "NORM[OPN",
    "NORM[CLS",
    "[M[A[END",
    "[ANL[POS",
    "[ANL[CMD",
    "[[ANL[SP",
    "[[ANL[P1",
    "[[ANL[P2",
    "[[[ANL[T",
    "[ANL[END",
    "[[4[20MA",
    "[[0[20MA",
    "[[[0[10V",
    "[[[[0[5V",
    "[ANS[END",
    "BIN1[DRV",
    "BIN1[LIM",
    "BIN1SPOS",                         
    "BIN1ERRS",
    "BIN1ERR1",
    "BIN1ERR2",
    "BIN1ERRT",
    "BIN1[OPN",
    "BIN1[CLS",
    "BIN1[END",
    "BIN2[DRV",
    "BIN2[LIM",
    "BIN2SPOS",                         
    "BIN2ERRS",
    "BIN2ERR1",
    "BIN2ERR2",
    "BIN2ERRT",
    "BIN2[OPN",
    "BIN2[CLS",
    "BIN2[END",
    "SETP[INT",
    "SETP[EXT",
    "SETP[END",
    "INP[FREQ",
    "INP[P100",
    "INP[P1P2",
    "INP[20MA",
    "[INP[END",
    "[PCO[END",
    "[PCO[SCS",
    "[PCO[CFR",
    "[PCO[SCT",
    "[PCO[SCP",
    "[TUN[NOT",
    "[TUN[DEF",
    "TUN[FLOW",
    "TUN[TEMP",
    "TUN[PRES",
    "TUN[LEVL",
    "[TUN[END",
    "LAEK[NOT",
    "LEAKMEAS",
    "LEAKCHAR",
    "LEAK[END",
};

enum
{
    KEY_1,
    KEY_2,
    KEY_3,
	KEY_4,
    KEY_5,
    KEY_6,
	KEY_7,

	KEYS,
};

enum
{
    /* Main Menu*/
    MENU_MAIN_ACT,
    MENU_MAIN_INPUT,
    MENU_MAIN_CHARACT,
    MENU_MAIN_CUTOFF,
    MENU_MAIN_DIRCMD,
    MENU_MAIN_DIRACT,
    MENU_MAIN_SPLTRNG,
    MENU_MAIN_XLIMIT,
    MENU_MAIN_XTIME,
    MENU_MAIN_XCONTRL,
    MENU_MAIN_PCONTRL,
    MENU_MAIN_CODE,
    MENU_MAIN_SAFEPOS,
    MENU_MAIN_SIGERR,
    MENU_MAIN_BININ,
    MENU_MAIN_OUTPUT,
    MENU_MAIN_CALUSER,
    MENU_MAIN_SETFACT,
    MENU_MAIN_SERIO,    
    MENU_MAIN_ADDFUN,
    MENU_MAIN_MANUAL,
    MENU_MAIN_XTUNE,
    MENU_MAIN_END,
    /* Act Menu*/
    MENU_ACT_SNGL,
    MENU_ACT_DOUB,
    MENU_ACT_END,
    /* Input Menu*/
    MENU_INPUT_4,
    MENU_INPUT_0,
    MENU_INPUT_10,
    MENU_INPUT_5,
    MENU_INPUT_END,
    /* Charact Menu*/
    MENU_CHARACT_l_1,
    MENU_CHARACT_l_25,
    MENU_CHARACT_l_33,
    MENU_CHARACT_l_50,
    MENU_CHARACT_25_1,
    MENU_CHARACT_33_1,
    MENU_CHARACT_50_1,
    MENU_CHARACT_FREE,
    MENU_CHARACT_END,
    /* Dir Cmd Menu*/
    MENU_DIRCMD_RISE,
    MENU_DIRCMD_FALL,
    MENU_DIRCMD_END,
    /* Dir Act Menu*/
    MENU_DIRACT_RISE,
    MENU_DIRACT_FALL,
    MENU_DIRACT_END,
    /* X Contrl Menu*/
    MENU_XCONTRL_END,
    MENU_XCONTRL_DBMD,
    MENU_XCONTRL_PARA,  
    /* X Tune Menu*/
    MENU_XTUNE_AUTO,
    MENU_XTUNE_POS,
    MENU_XTUNE_PWM,
    MENU_XTUNE_AIR,
    MENU_XTUNE_END,
    /* Sig Err Menu*/
    MENU_SIGERR_FUNON,
    MENU_SIGERR_POSON,
    MENU_SIGERR_OFF,
    MENU_SIGERR_END,
    /* Ser Io Menu*/
    MENU_SERIO_9600,
    MENU_SERIO_19200,
    MENU_SERIO_38400,
    MENU_SERIO_1200,
    MENU_SERIO_2400,
    MENU_SERIO_4800,
    MENU_SERIO_END,
    /* Code Menu*/
    MENU_CODE_END,
    MENU_CODE_KEY,
    MENU_CODE_MENU,
    MENU_CODE_GLOB, 
    /* Bin in Menu*/
    MENU_BININ_SPOS,
    MENU_BININ_MA,
    MENU_BININ_END,
    /* Output Menu*/
    MENU_OUTPUT_ANL,
    MENU_OUTPUT_ANLSET,
    MENU_OUTPUT_BIN1,
    MENU_OUTPUT_BIN2,
    MENU_OUTPUT_END,
    /* Cal User Menu*/
    MENU_CALUSER_END,
    MENU_CALUSER_POS,
    MENU_CALUSER_INP,
    MENU_CALUSER_FACT,  
    /* P Contrl Menu*/
    MENU_PCONTRL_END,
    MENU_PCONTRL_DBND,
    MENU_PCONTRL_PARA,
    MENU_PCONTRL_SETP,
    MENU_PCONTRL_INP,
    MENU_PCONTRL_FILT,
    MENU_PCONTRL_SCAL,
    MENU_PCONTRL_TUNE,
    MENU_PCONTRL_KV,
    MENU_PCONTRL_LEAK,  
    /* S Pos in Menu*/
    MENU_SPOS_OPN,
    MENU_SPOS_CLS,
    MENU_SPOS_END,
    /* M A Menu*/
    MENU_MA_OPN,
    MENU_MA_CLS,
    MENU_MA_END,
    /* Anl Menu*/
    MENU_ANL_POS,
    MENU_ANL_CMD,
    MENU_ANL_SP,
    MENU_ANL_P1,
    MENU_ANL_P2,
    MENU_ANL_T,
    MENU_ANL_END,
    /* Anl Set Menu*/
    MENU_ANLSET_4,
    MENU_ANLSET_0,
    MENU_ANLSET_10,
    MENU_ANLSET_5,
    MENU_ANLSET_END,
    /* Bin1 Menu*/
    MENU_BIN1_DRV,
    MENU_BIN1_LIM,
    MENU_BIN1_SPOS,
    MENU_BIN1_ERRS,
    MENU_BIN1_ERR1,
    MENU_BIN1_ERR2,
    MENU_BIN1_ERRT,
    MENU_BIN1_OPN,
    MENU_BIN1_CLS,
    MENU_BIN1_END,
    /* Bin2 Menu*/
    MENU_BIN2_DRV,
    MENU_BIN2_LIM,
    MENU_BIN2_SPOS,
    MENU_BIN2_ERRS,
    MENU_BIN2_ERR1,
    MENU_BIN2_ERR2,
    MENU_BIN2_ERRT,
    MENU_BIN2_OPN,
    MENU_BIN2_CLS,
    MENU_BIN2_END,
    /* Setp Menu*/
    MENU_SETP_INT,
    MENU_SETP_EXT,
    MENU_SETP_END,
    /* Inp Menu*/
    MENU_INP_FREQ,
    MENU_INP_PT100,
    MENU_INP_P1P2,
    MENU_INP_20MA,
    MENU_INP_END,
    /* Scal Menu*/
    MENU_SCAL_END,
    MENU_SCAL_S,
    MENU_SCAL_F,
    MENU_SCAL_T,
    MENU_SCAL_P,
    /* Tune Menu*/
    MENU_TUNE_NOT,
    MENU_TUNE_DEF,
    MENU_TUNE_FLOW,
    MENU_TUNE_TEMP,
    MENU_TUNE_PRES,
    MENU_TUNE_LEVL,
    MENU_TUNE_END,
    /* Leak Menu*/
    MENU_LEAK_NOT,
    MENU_LEAK_MEAS,
    MENU_LEAK_CHAR,
    MENU_LEAK_END,

    MENU_ITEMS,
};

extern UNIT_DATA                g_UnitData;

DEFINE_STACK(MENU_CTL_BLOCK, uint8_t, STACK_MCB)

MENU_ITEM                       m_aMenuItems[MENU_ITEMS];
MENU_CTL_BLOCK                  m_mcbCurrent;
STACK_MCB                       m_stackMenuCtlBlock;

/* Indicate which page to be displayed*/
static uint8_t                      m_iCnt                      = 0;
static uint8_t                      m_byPageNo                  = 0;
static uint8_t                      m_byCursorPos               = 0;
static uint8_t                      m_byCursorPage              = 0;

static uint8_t                      m_bufKeyboard[KB_BUF_SIZE];

extern UNIT_DATA                g_UnitData;
extern UNIT_CFG                 g_UnitCfg;

static volatile void delay_1s(U32 count)   /*delay_1s(30) is 1 second*/   
{
    U32 i;  
    while (count)
    {
        i =200000;
        while (i>0) i--;
        count--;
    }
}

static void OnKeyDown(uint8_t byKeyCode)
{
    GP3DAT  &= ~0x00020000;
    g_UnitData.dat.iCnt = 0;
    if (m_mcbCurrent.pMenu == NULL)
    {
        if (byKeyCode == KEY_1)
        {

        }
        else if (byKeyCode == KEY_2)
        {

        }
        else if (byKeyCode == KEY_3)
        {
            if (m_mcbCurrent.pMenu == NULL)
	        {
	            GP0DAT  |= 0x01010000;
				S_PUSH(m_stackMenuCtlBlock, m_mcbCurrent);
	
	            m_mcbCurrent.pMenu                      = &m_aMenuItems[MENU_MAIN_ACT];
	            m_mcbCurrent.byStartMenuItemID          = m_mcbCurrent.pMenu->byMenuItemID;
	        } 			
        }
    }
}

//static void OnKeyLongPressUp(uint8_t byKeyCode)
//{
//
//}

static void OnKeyUp(uint8_t byKeyCode)
{
    GP3DAT  &= ~0x00020000;
    g_UnitData.dat.iCnt = 0;
    if (m_mcbCurrent.pMenu == NULL)
    {
        if (byKeyCode == KEY_1)
        {
            m_byPageNo++;
            if ((m_byPageNo&0x3F) > 3)
            {
                m_byPageNo &= ~0x3F;
            }
        }
        else if (byKeyCode == KEY_3)
        {
              m_byPageNo += 0x80;
			  GP0DAT  |= 0x01010000;
        }
    }
    else
    {
        if (m_mcbCurrent.pMenu->KeyboardHandler != NULL)
        {
            m_mcbCurrent.pMenu->KeyboardHandler(byKeyCode);
        }
    }
}

static void CheckKeyboard()
{
    static uint8_t _cnts[KEYS] = {0, 0, 0};

    uint8_t byKeyMask       = 0;
    uint8_t byBitMask;

    if (IS_KEY0_DOWN())
        byKeyMask |= BIT00;
    if (IS_KEY1_DOWN())
        byKeyMask |= BIT01;
    if (IS_KEY2_DOWN())
        byKeyMask |= BIT02;

	if (byKeyMask == 7)
        byKeyMask  = BIT03;	/* key 1,2,3 */
    else if (byKeyMask == 6)
        byKeyMask  = BIT04;	/* key 2,3 */
    else if (byKeyMask == 5)
        byKeyMask  = BIT05;	/* key 1,3 */
	else if (byKeyMask == 3)
        byKeyMask  = BIT06; /* key 1,2 */  

    for (uint8_t i = 0; i < KEYS; ++i)
    {
        byBitMask = (1 << i);
        if ((byKeyMask & byBitMask) != 0)
        {
            if (_cnts[i] >= 30)
            {
                OnKeyDown(i);
            }
            _cnts[i]++;
        }
        else
        {
            if (_cnts[i] >= 20)
            {
//				OnKeyLongPressUp(i);		  
            }
			else if(_cnts[i] >= 1)
			{
                OnKeyUp(i);
            }
            _cnts[i] = 0;
        }
    }
}

static void adj_display(void)
{
	if ((m_byCursorPos%3) == 2)
	{
		if (m_bufKeyboard[m_byCursorPos] > '1')
            m_bufKeyboard[m_byCursorPos] = '0';
		else if (m_bufKeyboard[m_byCursorPos] == '1')
		{
			m_bufKeyboard[m_byCursorPos] = '1';
			m_bufKeyboard[m_byCursorPos - 1] = '0';
			m_bufKeyboard[m_byCursorPos - 2] = '0';
		}
	}
}

static void menu_return(void)
{
	MENU_ITEM*  pMenuItem = &m_aMenuItems[m_mcbCurrent.byStartMenuItemID];            

    S_PUSH(m_stackMenuCtlBlock, m_mcbCurrent);
    m_mcbCurrent.pMenu                      = pMenuItem;
    m_mcbCurrent.byStartMenuItemID          = 0;
}

static void menu_display_done(void)
{
	display_digital("::::::",0,0);
    display_char("[[[[DONE");
	delay_1s(30);
}

static void Display()
{
    uint8_t byNum[7];

    if (m_mcbCurrent.pMenu == NULL)
    {
      	if (!(m_byPageNo&0x80))
        {
            m_iCnt++;
            if (m_iCnt > 10)
            {
                m_iCnt = 0;
                m_byPageNo++;
				GP0DAT  ^= 0x00010000;
                if ((m_byPageNo&0x3F) > 3)
                {
                    m_byPageNo &= ~0x3F;
                }
            }
        }
        switch (m_byPageNo&0x7f)
        {
        case 0x00:
            floattochar (g_UnitData.dat.fPos/10, byNum,0);
            display_digital(byNum,0,0);
            display_char("[[[[[POS");
            break;
        case 0x01:
            floattochar (g_UnitData.dat.fCmd/10, byNum,0);
            display_digital(byNum,0,0);
            display_char("[[[[[CMD");
            break;
        case 0x02:
            floattochar (g_UnitData.dat.iInp/10, byNum,0);
            display_digital(byNum,0,0);
            display_char("[[[[[INP");
			break;
        case 0x03:
            floattochar (g_UnitData.dat.fTem, byNum,1);
            display_digital(byNum,1,0);
            display_char("[[[[TEMP");
            break;
        case 0x40:
            floattochar (g_UnitData.dat.fPv/10, byNum,0);
            display_digital(byNum,0,0);
            display_char("[[[[[[PV");
            break;
        case 0x41:
            floattochar (g_UnitData.dat.fSp/10, byNum,0);
            display_digital(byNum,0,0);
            display_char("[[[[[[SP");
            break;
        case 0x42:
            floattochar (g_UnitData.dat.fCmd/10, byNum,0);
            display_digital(byNum,0,0);
            display_char("[[[[[CMD");
			break;
        case 0x43:
            floattochar (g_UnitData.dat.fTem, byNum,1);
            display_digital(byNum,1,0);
            display_char("[[[[TEMP");
            break;
        default:
            display_digital("::::::",0,0);
            display_char("[[[[[ERR");
            break;
        }
    }
    else
    {
        if (m_mcbCurrent.pMenu->DisplayHandler != NULL)
        {
            m_mcbCurrent.pMenu->DisplayHandler();
        }
    }
}

static void DisplayMenu()
{
    uint16_t    byMenuID = m_mcbCurrent.byStartMenuItemID;

    display_digital("::::::",0,0);
    display_char((unsigned char *)display[byMenuID]);
}

static bool IsRetMenuItem(MENU_ITEM* pMenuItem)
{
    return(pMenuItem->byMenuItemID == MENU_MAIN_END);
}

extern void EepromWr_n( unsigned short *pcData );

static void MenuKeyboardHandler(uint8_t byKeyCode)
{
    if ((byKeyCode == KEY_1) && 
        (m_aMenuItems[m_mcbCurrent.byStartMenuItemID].pParentMenu == m_aMenuItems[m_mcbCurrent.byStartMenuItemID - 1].pParentMenu))
    {
        m_mcbCurrent.byStartMenuItemID--;
		if (m_mcbCurrent.byStartMenuItemID > MENU_ITEMS - 1)
            m_mcbCurrent.byStartMenuItemID  = 0;
		while(m_aMenuItems[m_mcbCurrent.byStartMenuItemID].byChildMenuItems == 0)
			m_mcbCurrent.byStartMenuItemID--;        
    }
    else if ((byKeyCode == KEY_2) && 
             (m_aMenuItems[m_mcbCurrent.byStartMenuItemID].pParentMenu == m_aMenuItems[m_mcbCurrent.byStartMenuItemID + 1].pParentMenu))
    {
        m_mcbCurrent.byStartMenuItemID++;
		while(m_aMenuItems[m_mcbCurrent.byStartMenuItemID].byChildMenuItems == 0)
			m_mcbCurrent.byStartMenuItemID++; 		
        if (m_mcbCurrent.byStartMenuItemID > MENU_ITEMS - 1)
            m_mcbCurrent.byStartMenuItemID  = MENU_ITEMS - 1;
    }
    else if (byKeyCode == KEY_3)
    {
        MENU_ITEM*  pMenuItem = &m_aMenuItems[m_mcbCurrent.byStartMenuItemID];

        if (IsRetMenuItem(pMenuItem))
        {
            if (!S_IS_EMPTY(m_stackMenuCtlBlock))
            {
                display_char("[STORING");
				delay_1s(30);
				EepromWr_n(g_UnitCfg.buf);
                m_mcbCurrent.pMenu = NULL;
                S_POP(m_stackMenuCtlBlock);
            }
        }
        else if (pMenuItem->pChildMenu != NULL)
        {
            S_PUSH(m_stackMenuCtlBlock, m_mcbCurrent);
            m_mcbCurrent.pMenu                      = pMenuItem->pChildMenu;
            m_mcbCurrent.byStartMenuItemID          = pMenuItem->pChildMenu->byMenuItemID;
        }
        else
        {
            S_PUSH(m_stackMenuCtlBlock, m_mcbCurrent);
            m_mcbCurrent.pMenu                      = pMenuItem;
            m_mcbCurrent.byStartMenuItemID          = 0;

            if (pMenuItem->OnOpeningHandler != NULL)
            {
                pMenuItem->OnOpeningHandler();
            }
        }
    }
}

static void MainMenu_KeyboardHandler(uint8_t byKeyCode)
{
    MenuKeyboardHandler(byKeyCode);
}

static void MainMenu_DisplayHandler()
{
    DisplayMenu();
}

static void MENU_MAIN_MANUAL_KeyboardHandler(uint8_t byKeyCode)
{
    if (byKeyCode == KEY_1)
    {
        if (m_byCursorPos > 0)
        {
            m_byCursorPos--;
            m_byCursorPage   = m_byCursorPos/3;
        }
		else 
        {
            g_UnitCfg.dat.bIsManual = FALSE;
			menu_return( );
        }
    }
    else if (byKeyCode == KEY_2)
    {
        m_byCursorPos++;
        m_byCursorPage   = m_byCursorPos/3;
		if (m_byCursorPos > 5)
			m_byCursorPos = 5;
    }
    else if (byKeyCode == KEY_3)
    {
        if (m_byCursorPos < 3)
        {
            m_bufKeyboard[m_byCursorPos]++;			
            if (m_bufKeyboard[m_byCursorPos] > '9')
                m_bufKeyboard[m_byCursorPos] = '0';
			adj_display( );
        }
		else if (m_byCursorPos < 5)
		{
			g_UnitData.dat.fPid = (m_bufKeyboard[2]*100 + m_bufKeyboard[1]*10 + m_bufKeyboard[0] - 5328)*10;
			m_byCursorPos = 5;
		}
        else 
        {
            g_UnitCfg.dat.bIsManual = FALSE;
			menu_return( );
        }
    }
}

static void MENU_MAIN_MANUAL_DisplayHandler()
{
    uint8_t byNum[6];

    byNum[0] = '+';
    byNum[1] = ':';
    byNum[2] = ':';
    byNum[3] = m_bufKeyboard[3*m_byCursorPage + 2];
    byNum[4] = m_bufKeyboard[3*m_byCursorPage + 1];
    byNum[5] = m_bufKeyboard[3*m_byCursorPage];
    if (m_byCursorPos < 3)
        display_digital(byNum,0,m_byCursorPos - 3*m_byCursorPage + 1);
    if (m_byCursorPage == 0)
    {
        display_char("[[[[[POS");
    }
    else
    {
        floattochar (g_UnitData.dat.fPos/10, byNum,0);
        display_digital(byNum,0,0);
        display_char("[[[[[POS");
    }   
}

static void MENU_MAIN_MANUAL_OpeningHandler()
{
    m_byCursorPos = 0;
    for (uint16_t i = 0; i < 6; i++)
        m_bufKeyboard[i] = '0';
    m_byCursorPage   = 0; 
	g_UnitCfg.dat.bIsManual = TRUE;  
}
static void menu_update(void)
{
    for (int i = 0;i < 17;i++)
        m_aMenuItems[MENU_MAIN_CHARACT + i].byChildMenuItems        = ((g_UnitCfg.dat.uMenu&(1L<<i))?1:0);
}
static void AddFun_KeyboardHandler(uint8_t byKeyCode)
{
    if (byKeyCode == KEY_1)
    {
        if (m_byCursorPos > 0)
        {
            m_byCursorPos--;
        }
		else 
        {
            menu_return( );
        }
    }
    else if (byKeyCode == KEY_2)
    {
        m_byCursorPos++;
		if (m_byCursorPos > 17)
			m_byCursorPos = 17;
    }
    else if (byKeyCode == KEY_3)
    {
        if (m_byCursorPos < 17)
        {
            g_UnitCfg.dat.uMenu ^= (1L<<m_byCursorPos);
        }
        else
        {
            menu_update();
            menu_return( );
        }
    }
}

static void AddFun_DisplayHandler()
{
    uint16_t    byMenuID = MENU_MAIN_CHARACT + m_byCursorPos;
    if (m_byCursorPos < 17)
    {
        if ((g_UnitCfg.dat.uMenu&(1L<<m_byCursorPos)) != 0)
            display_digital(":::::1",0,0);
        else
            display_digital(":::::0",0,0);
        display_char((unsigned char *)display[byMenuID]);
    }
    else
    {
        display_digital("::::::",0,0);
        display_char("[[[[DONE");
    }
}

static void AddFun_OpeningHandler()
{
    m_byCursorPos = 0;
}

static void ActSngl_OpeningHandler()
{
    g_UnitCfg.dat.bIsDouble = FALSE;
    menu_display_done( );
}

static void ActDoub_OpeningHandler()
{
    g_UnitCfg.dat.bIsDouble = TRUE;
    menu_display_done( );
}

static void Input4_OpeningHandler()
{
    g_UnitCfg.dat.byInp = 0;
    menu_display_done( );
}

static void Input0_OpeningHandler()
{
    g_UnitCfg.dat.byInp = 1;
    menu_display_done( );
}

static void Input10_OpeningHandler()
{
    g_UnitCfg.dat.byInp = 3;
    menu_display_done( );
}

static void Input5_OpeningHandler()
{
    g_UnitCfg.dat.byInp = 2;
    menu_display_done( );
}

static void InsetCutoff_KeyboardHandler(uint8_t byKeyCode)
{
    if (byKeyCode == KEY_1)
    {
        if (m_byCursorPos > 0)
        {
            m_byCursorPos--;
            m_byCursorPage   = m_byCursorPos/3;
        }
		else 
        {
            menu_return( );
        }
    }
    else if (byKeyCode == KEY_2)
    {
        m_byCursorPos++;
        m_byCursorPage   = m_byCursorPos/3;
		if (m_byCursorPos > 6)
			m_byCursorPos = 6;
    }
    else if (byKeyCode == KEY_3)
    {
        if (m_byCursorPos < 6)
        {
            m_bufKeyboard[m_byCursorPos]++;
            if (m_bufKeyboard[m_byCursorPos] > '9')
                m_bufKeyboard[m_byCursorPos] = '0';
			adj_display( );
        }
        else
        {
            g_UnitCfg.dat.iCutoffMin = 10*(m_bufKeyboard[2]*100 + m_bufKeyboard[1]*10 + m_bufKeyboard[0] - 5328);
            g_UnitCfg.dat.iCutoffMax = 10*(m_bufKeyboard[5]*100 + m_bufKeyboard[4]*10 + m_bufKeyboard[3] - 5328);
            menu_return( );
        }
    }
}

static void InsetCutoff_DisplayHandler()
{
    uint8_t byNum[6];

    byNum[0] = '+';
    byNum[1] = ':';
    byNum[2] = ':';
    byNum[3] = m_bufKeyboard[3*m_byCursorPage + 2];
    byNum[4] = m_bufKeyboard[3*m_byCursorPage + 1];
    byNum[5] = m_bufKeyboard[3*m_byCursorPage];
    if (m_byCursorPos < 6)
        display_digital(byNum,0,m_byCursorPos - 3*m_byCursorPage + 1);
    else
        display_digital("::::::",0,0);  
    if (m_byCursorPage == 0)
    {
        display_char("[CUT[MIN");
    }
    else if (m_byCursorPage == 1)
    {
        display_char("[CUT[MAX");
    }
    else
    {
        display_char("[[[[DONE");
    }
}

static void InsetCutoff_OpeningHandler()
{
    m_byCursorPos = 0;
    for (uint16_t i = 0; i < 6; i++)
        m_bufKeyboard[i] = '0';
    m_byCursorPage   = 0;
}

static void InsetSpltrng_KeyboardHandler(uint8_t byKeyCode)
{
    if (byKeyCode == KEY_1)
    {
        if (m_byCursorPos > 0)
        {
            m_byCursorPos--;
            m_byCursorPage   = m_byCursorPos/3;
        }
		else 
        {
            menu_return( );
        }
    }
    else if (byKeyCode == KEY_2)
    {
        m_byCursorPos++;
        m_byCursorPage   = m_byCursorPos/3;
		if (m_byCursorPos > 6)
			m_byCursorPos = 6;
    }
    else if (byKeyCode == KEY_3)
    {
        if (m_byCursorPos < 6)
        {
            m_bufKeyboard[m_byCursorPos]++;
            if (m_bufKeyboard[m_byCursorPos] > '9')
                m_bufKeyboard[m_byCursorPos] = '0';
			adj_display( );
        }
        else
        {
            g_UnitCfg.dat.iSrD = (m_bufKeyboard[2]*100 + m_bufKeyboard[1]*10 + m_bufKeyboard[0] - 5328)*10;
            g_UnitCfg.dat.iSrU = (m_bufKeyboard[5]*100 + m_bufKeyboard[4]*10 + m_bufKeyboard[3] - 5328)*10; 	
            menu_return( );
        }
    }
}

static void InsetSpltrng_DisplayHandler()
{
    uint8_t byNum[6];

    byNum[0] = '+';
    byNum[1] = ':';
    byNum[2] = ':';
    byNum[3] = m_bufKeyboard[3*m_byCursorPage + 2];
    byNum[4] = m_bufKeyboard[3*m_byCursorPage + 1];
    byNum[5] = m_bufKeyboard[3*m_byCursorPage];
    if (m_byCursorPos < 6)
        display_digital(byNum,0,m_byCursorPos - 3*m_byCursorPage + 1);
    else
        display_digital("::::::",0,0);  
    if (m_byCursorPage == 0)
    {
        display_char("[[[[[SRD");
    }
    else if (m_byCursorPage == 1)
    {
        display_char("[[[[[SRU");
    }
    else
    {
        display_char("[[[[DONE");
    }
}

static void InsetSpltrng_OpeningHandler()
{
    m_byCursorPos = 0;
    for (uint16_t i = 0; i < 6; i++)
        m_bufKeyboard[i] = '0';
    m_byCursorPage   = 0;
}

static void XsetXlimit_KeyboardHandler(uint8_t byKeyCode)
{
    if (byKeyCode == KEY_1)
    {
        if (m_byCursorPos > 0)
        {
            m_byCursorPos--;
            m_byCursorPage   = m_byCursorPos/3;
        }
		else 
        {
            menu_return( );
        }
    }
    else if (byKeyCode == KEY_2)
    {
        m_byCursorPos++;
        m_byCursorPage   = m_byCursorPos/3;
		if (m_byCursorPos > 6)
			m_byCursorPos = 6;
    }
    else if (byKeyCode == KEY_3)
    {
        if (m_byCursorPos < 6)
        {
            m_bufKeyboard[m_byCursorPos]++;
            if (m_bufKeyboard[m_byCursorPos] > '9')
                m_bufKeyboard[m_byCursorPos] = '0';
			adj_display( );
        }
        else
        {
            g_UnitCfg.dat.iLimD = (m_bufKeyboard[2]*100 + m_bufKeyboard[1]*10 + m_bufKeyboard[0] - 5328)*10;
            g_UnitCfg.dat.iLimU = (m_bufKeyboard[5]*100 + m_bufKeyboard[4]*10 + m_bufKeyboard[3] - 5328)*10;
			menu_return( );
        }
    }
}

static void XsetXlimit_DisplayHandler()
{
    uint8_t byNum[6];

    byNum[0] = '+';
    byNum[1] = ':';
    byNum[2] = ':';
    byNum[3] = m_bufKeyboard[3*m_byCursorPage + 2];
    byNum[4] = m_bufKeyboard[3*m_byCursorPage + 1];
    byNum[5] = m_bufKeyboard[3*m_byCursorPage];
    if (m_byCursorPos < 6)
        display_digital(byNum,0,m_byCursorPos - 3*m_byCursorPage + 1);
    else
        display_digital("::::::",0,0);  
    if (m_byCursorPage == 0)
    {
        display_char("[[[[LIMD");
    }
    else if (m_byCursorPage == 1)
    {
        display_char("[[[[LIMU");
    }
    else
    {
        display_char("[[[[DONE");
    }
}

static void XsetXlimit_OpeningHandler()
{
    m_byCursorPos = 0;
    for (uint16_t i = 0; i < 6; i++)
        m_bufKeyboard[i] = '0';
    m_byCursorPage   = 0;
}

static void XsetXtime_KeyboardHandler(uint8_t byKeyCode)
{
    if (byKeyCode == KEY_1)
    {
        if (m_byCursorPos > 0)
        {
            m_byCursorPos--;
            m_byCursorPage   = m_byCursorPos/2;
        }
		else 
        {
            menu_return( );
        }
    }
    else if (byKeyCode == KEY_2)
    {
        m_byCursorPos++;
        m_byCursorPage   = m_byCursorPos/2;
		if (m_byCursorPos > 4)
			m_byCursorPos = 4;
    }
    else if (byKeyCode == KEY_3)
    {
        if (m_byCursorPos < 4)
        {
            m_bufKeyboard[m_byCursorPos]++;
            if (m_bufKeyboard[m_byCursorPos] > '9')
                m_bufKeyboard[m_byCursorPos] = '0';
        }
        else
        {
            g_UnitCfg.dat.iXtimeOpen = m_bufKeyboard[1]*10 + m_bufKeyboard[0] - 528;
            g_UnitCfg.dat.iXtimeClose = m_bufKeyboard[3]*10 + m_bufKeyboard[2] - 528;
			menu_return( );
        }
    }
}

static void XsetXtime_DisplayHandler()
{
    uint8_t byNum[6];

    byNum[0] = '+';
    byNum[1] = ':';
    byNum[2] = ':';
    byNum[3] = ':';
    byNum[4] = m_bufKeyboard[2*m_byCursorPage + 1];
    byNum[5] = m_bufKeyboard[2*m_byCursorPage];
    if (m_byCursorPos < 4)
        display_digital(byNum,0,m_byCursorPos - 2*m_byCursorPage + 1);
    else
        display_digital("::::::",0,0);  
    if (m_byCursorPage == 0)
    {
        display_char("[[[[[OPN");
    }
    else if (m_byCursorPage == 1)
    {
        display_char("[[[[[CLS");
    }
    else
    {
        display_char("[[[[DONE");
    }
}

static void XsetXtime_OpeningHandler()
{
    m_byCursorPos = 0;
    for (uint16_t i = 0; i < 4; i++)
        m_bufKeyboard[i] = '0';
    m_byCursorPage   = 0;
}

static void SystemSafepos_KeyboardHandler(uint8_t byKeyCode)
{
    if (byKeyCode == KEY_1)
    {
        if (m_byCursorPos > 0)
        {
            m_byCursorPos--;
        }
		else 
        {
            menu_return( );
        }
    }
    else if (byKeyCode == KEY_2)
    {
        m_byCursorPos++;
		if (m_byCursorPos > 3)
			m_byCursorPos = 3;
    }
    else if (byKeyCode == KEY_3)
    {
        if (m_byCursorPos < 3)
        {
            m_bufKeyboard[m_byCursorPos]++;
            if (m_bufKeyboard[m_byCursorPos] > '9')
                m_bufKeyboard[m_byCursorPos] = '0';
			adj_display( );
        }
        else
        {
            g_UnitCfg.dat.iSafePos = 10*(m_bufKeyboard[2]*100 + m_bufKeyboard[1]*10 + m_bufKeyboard[0] - 5328);	
            menu_return( );
        }
    }
}

static void SystemSafepos_DisplayHandler()
{
    uint8_t byNum[6];

    byNum[0] = '+';
    byNum[1] = ':';
    byNum[2] = ':';
    byNum[3] = m_bufKeyboard[3*m_byCursorPage + 2];
    byNum[4] = m_bufKeyboard[3*m_byCursorPage + 1];
    byNum[5] = m_bufKeyboard[3*m_byCursorPage];
    if (m_byCursorPos < 3)
    {
        display_digital(byNum,0,m_byCursorPos - 3*m_byCursorPage + 1);
        display_char("[[[[SPOS");
    }
    else
    {
        display_digital("::::::",0,0);
        display_char("[[[[DONE");
    }
}

static void SystemSafepos_OpeningHandler()
{
    m_byCursorPos = 0;
    for (uint16_t i = 0; i < 3; i++)
        m_bufKeyboard[i] = '0';
    m_byCursorPage   = 0;
}

extern void data_init(void);

static void SystemSetfact_OpeningHandler()
{
    clearLCD();
	data_init();
    display_char("[[[[DONE");
	g_UnitCfg.dat.bIsReboot = TRUE;
    clearLCD(); 
}

static void Charact11_OpeningHandler()
{
    g_UnitCfg.dat.bIsChaFree = FALSE;
    g_UnitCfg.dat.byN = 0;
    menu_display_done( );
}

static void Charact125_OpeningHandler()
{
    g_UnitCfg.dat.bIsChaFree = FALSE;
    g_UnitCfg.dat.byN = -25;
    menu_display_done( );
}

static void Charact133_OpeningHandler()
{
    g_UnitCfg.dat.bIsChaFree = FALSE;
    g_UnitCfg.dat.byN = -33;
    menu_display_done( );
}

static void Charact150_OpeningHandler()
{
    g_UnitCfg.dat.bIsChaFree = FALSE;
    g_UnitCfg.dat.byN = -50;
    menu_display_done( );
}

static void Charact251_OpeningHandler()
{
    g_UnitCfg.dat.bIsChaFree = FALSE;
    g_UnitCfg.dat.byN = 25;
    menu_display_done( ); 
}

static void Charact331_OpeningHandler()
{
    g_UnitCfg.dat.bIsChaFree = FALSE;
    g_UnitCfg.dat.byN = 33;
    menu_display_done( );
}

static void Charact501_OpeningHandler()
{
    g_UnitCfg.dat.bIsChaFree = FALSE;
    g_UnitCfg.dat.byN = 50;
    menu_display_done( );
}

static void CharactFree_KeyboardHandler(uint8_t byKeyCode)
{
    if (byKeyCode == KEY_1)
    {
        if (m_byCursorPos > 0)
        {
            m_byCursorPos--;
            m_byCursorPage   = m_byCursorPos/3;
        }
		else 
        {
            menu_return( );
        }
    }
    else if (byKeyCode == KEY_2)
    {
        m_byCursorPos++;
        m_byCursorPage   = m_byCursorPos/3;
		if (m_byCursorPos > 63)
			m_byCursorPos = 63;
    }
    else if (byKeyCode == KEY_3)
    {
        if (m_byCursorPos < 63)
        {
            m_bufKeyboard[m_byCursorPos]++;
            if (m_bufKeyboard[m_byCursorPos] > '9')
                m_bufKeyboard[m_byCursorPos] = '0';
			adj_display( );
        }
        else
        {
            g_UnitCfg.dat.bIsChaFree = TRUE;
            for (uint16_t i = 0; i < 21; i++)
                g_UnitCfg.dat.byCha[i] = m_bufKeyboard[3*i + 2]*100 + m_bufKeyboard[3*i + 1]*10 + m_bufKeyboard[3*i] - 5328;
            menu_return( );
        }
    }
}

static void CharactFree_DisplayHandler()
{
    uint8_t byNum[6];

    byNum[0] = '+';
    byNum[1] = '0' + (m_byCursorPage/2);
    byNum[2] = '0' + 5*(m_byCursorPage%2);
    byNum[3] = m_bufKeyboard[3*m_byCursorPage + 2];
    byNum[4] = m_bufKeyboard[3*m_byCursorPage + 1];
    byNum[5] = m_bufKeyboard[3*m_byCursorPage];

    if (m_byCursorPos < 63)
    {
        display_digital(byNum,0,m_byCursorPos - 3*m_byCursorPage + 1);
        display_char("CHA[FREE");
    }
    else
    {
        display_digital("::::::",0,0);
        display_char("[[[[DONE");
    }
}

static void CharactFree_OpeningHandler()
{
    m_byCursorPos = 0;  
    clearLCD();
    for (uint16_t i = 0; i < 63; i++)
        m_bufKeyboard[i] = '0';
    m_byCursorPage   = 0;
}

static void DircmdRise_OpeningHandler()
{
    g_UnitCfg.dat.bIsCmdInverse = FALSE;
    menu_display_done( ); 
}

static void DircmdFall_OpeningHandler()
{
    g_UnitCfg.dat.bIsCmdInverse = TRUE;
    menu_display_done( );
}

static void DiractRise_OpeningHandler()
{
    g_UnitCfg.dat.bIsActInverse = FALSE;
    menu_display_done( );
}

static void DiractFall_OpeningHandler()
{
    g_UnitCfg.dat.bIsActInverse = TRUE;
    menu_display_done( ); 
}

static void XcontrolDbnd_KeyboardHandler(uint8_t byKeyCode)
{
    if (byKeyCode == KEY_1)
    {
        if (m_byCursorPos > 0)
        {
            m_byCursorPos--;
        }
		else 
        {
            menu_return( );
        }
    }
    else if (byKeyCode == KEY_2)
    {
        m_byCursorPos++;
		if (m_byCursorPos > 3)
			m_byCursorPos = 3;
    }
    else if (byKeyCode == KEY_3)
    {
        if (m_byCursorPos < 3)
        {
            m_bufKeyboard[m_byCursorPos]++;
            if (m_bufKeyboard[m_byCursorPos] > '9')
                m_bufKeyboard[m_byCursorPos] = '0';
			adj_display( );
        }
        else
        {
            g_UnitCfg.dat.iDbnd = m_bufKeyboard[2]*100 + m_bufKeyboard[1]*10 + m_bufKeyboard[0] - 5328; 
            menu_return( );
        }
    }
}

static void XcontrolDbnd_DisplayHandler()
{
    uint8_t byNum[6];

    byNum[0] = '+';
    byNum[1] = ':';
    byNum[2] = ':';
    byNum[3] = m_bufKeyboard[3*m_byCursorPage + 2];
    byNum[4] = m_bufKeyboard[3*m_byCursorPage + 1];
    byNum[5] = m_bufKeyboard[3*m_byCursorPage];
    if (m_byCursorPos < 3)
    {
        display_digital(byNum,1,m_byCursorPos - 3*m_byCursorPage + 1); 
        display_char("[[[[DBND");
    }
    else
    {
        display_digital("::::::",0,0);
        display_char("[[[[DONE");
    }

}

static void XcontrolDbnd_OpeningHandler()
{
    m_byCursorPos = 0;
    for (uint16_t i = 0; i < 3; i++)
        m_bufKeyboard[i] = '0';
    m_byCursorPage   = 0;
}

static void XcontrolPara_KeyboardHandler(uint8_t byKeyCode)
{
    if (byKeyCode == KEY_1)
    {
        if (m_byCursorPos > 0)
        {
            m_byCursorPos--;
            m_byCursorPage   = m_byCursorPos/3;
        }
		else 
        {
            menu_return( );
        }
    }
    else if (byKeyCode == KEY_2)
    {
        m_byCursorPos++;
        m_byCursorPage   = m_byCursorPos/3;
		if (m_byCursorPos > 6)
			m_byCursorPos = 6;
    }
    else if (byKeyCode == KEY_3)
    {
        if (m_byCursorPos < 6)
        {
            m_bufKeyboard[m_byCursorPos]++;
            if (m_bufKeyboard[m_byCursorPos] > '9')
                m_bufKeyboard[m_byCursorPos] = '0';
			adj_display( );
        }
        else
        {
            g_UnitCfg.dat.iKxD = m_bufKeyboard[2]*100 + m_bufKeyboard[1]*10 + m_bufKeyboard[0] - 5328;
            g_UnitCfg.dat.iKxU = m_bufKeyboard[5]*100 + m_bufKeyboard[4]*10 + m_bufKeyboard[3] - 5328;	
            menu_return( );
        }
    }
}

static void XcontrolPara_DisplayHandler()
{
    uint8_t byNum[6];

    byNum[0] = '+';
    byNum[1] = ':';
    byNum[2] = ':';
    byNum[3] = m_bufKeyboard[3*m_byCursorPage + 2];
    byNum[4] = m_bufKeyboard[3*m_byCursorPage + 1];
    byNum[5] = m_bufKeyboard[3*m_byCursorPage];
    if (m_byCursorPos < 6)
        display_digital(byNum,0,m_byCursorPos - 3*m_byCursorPage + 1);
    else
        display_digital("::::::",0,0);  
    if (m_byCursorPage == 0)
    {
        display_char("[[[[[KXD");
    }
    else if (m_byCursorPage == 1)
    {
        display_char("[[[[[KXU");
    }
    else
    {
        display_char("[[[[DONE");
    }
}

static void XcontrolPara_OpeningHandler()
{
    m_byCursorPos = 0;
    for (uint16_t i = 0; i < 6; i++)
        m_bufKeyboard[i] = '0';
    m_byCursorPage   = 0;
}

static uint16_t temp1 = 0;
static uint16_t temp2 = 100;

extern void SetPwmDutyCycle2(int16_t uiDutyCycle);

static void XtuneAuto_OpeningHandler()
{
    clearLCD();
    display_char("11111111");

    static float fDiff  = 100;
    static float fPos   = 0;
    static float fLast  = 0;
    short iPwm = 0;

    g_UnitCfg.dat.byMode |= 0x80;

    SetPwmDutyCycle2(-1000);

    delay_1s(90);

    while (ABS(fDiff) > 5)
    {
        os_dly_wait(100);
        fPos    = g_UnitData.dat.fPos;
        fDiff   = fPos - fLast;
        fLast   = fPos;
    }

    temp1 = g_UnitData.dat.iAD4;

    fDiff   = 100;
    fPos    = 0;
    fLast   = 0;

    SetPwmDutyCycle2(1000);
    display_char("22222222");

	delay_1s(60);

    while (ABS(fDiff) > 5)
    {
        os_dly_wait(100);
        fPos    = g_UnitData.dat.fPos;
        fDiff   = fPos - fLast;
        fLast   = fPos;
    }

    temp2 = g_UnitData.dat.iAD4;

	while(temp1 > temp2)
	{
		display_char("[[[ERR[1");
	}

	while(temp2 - temp1 < 100)
	{
		display_char("[[[ERR[2");
	}

    g_UnitCfg.dat.iAd4Min = temp1;
    g_UnitCfg.dat.iAd4Max = temp2;

    SetPwmDutyCycle2(-1000);
    display_char("33333333");
	delay_1s(60);

    while (g_UnitData.dat.fPos > 660)
        ;    
    
    SetPwmDutyCycle2(0);
    display_char("44444444");
    os_dly_wait(200);

    fDiff = 0;
    iPwm  = 0;
    fLast = g_UnitData.dat.fPos;
    while (ABS(fDiff) < 5)
    {
        SetPwmDutyCycle2(iPwm);
        os_dly_wait(10);
        fPos    = g_UnitData.dat.fPos;
        fDiff   = fPos - fLast;
        fLast   = fPos;
        iPwm   -= 10;
    }
    g_UnitCfg.dat.iYeU = -iPwm;

    SetPwmDutyCycle2(0);
    display_char("55555555");
    delay_1s(60);

    fDiff = 0;
    iPwm  = 0;
    fLast = g_UnitData.dat.fPos;
    while (ABS(fDiff) < 5)
    {
        SetPwmDutyCycle2(iPwm);
        os_dly_wait(10);
        fPos    = g_UnitData.dat.fPos;
        fDiff   = fPos - fLast;
        fLast   = fPos;
        iPwm   += 10;
    }
    g_UnitCfg.dat.iYbU = iPwm;

    g_UnitCfg.dat.iDbnd = 5;

    g_UnitCfg.dat.byMode &= 0x7F;

    display_char("[[[[DONE");
	EepromWr_n(g_UnitCfg.buf); 	
    delay_1s(30);
	RSTSTA  |= 0x04;
}

static void XtunePwm_KeyboardHandler(uint8_t byKeyCode)
{
    if (byKeyCode == KEY_1)
    {
        if (m_byCursorPos > 0)
        {
            m_byCursorPos--;
            m_byCursorPage   = m_byCursorPos/3;
        }
		else 
        {
            menu_return( );
        }
    }
    else if (byKeyCode == KEY_2)
    {
        m_byCursorPos++;
        m_byCursorPage   = m_byCursorPos/3;
		if (m_byCursorPos > 6)
			m_byCursorPos = 6;
    }
    else if (byKeyCode == KEY_3)
    {
        if (m_byCursorPos < 6)
        {
            m_bufKeyboard[m_byCursorPos]++;
            if (m_bufKeyboard[m_byCursorPos] > '9')
                m_bufKeyboard[m_byCursorPos] = '0';
			adj_display( );
        }
        else
        {
            g_UnitCfg.dat.iYbU = (m_bufKeyboard[2]*100 + m_bufKeyboard[1]*10 + m_bufKeyboard[0] - 5328)*10;
            g_UnitCfg.dat.iYeU = (m_bufKeyboard[5]*100 + m_bufKeyboard[4]*10 + m_bufKeyboard[3] - 5328)*10;
            menu_return( );
		}
    }
}

static void XtunePwm_DisplayHandler()
{
    uint8_t byNum[6];

    byNum[0] = '+';
    byNum[1] = ':';
    byNum[2] = ':';
    byNum[3] = m_bufKeyboard[3*m_byCursorPage + 2];
    byNum[4] = m_bufKeyboard[3*m_byCursorPage + 1];
    byNum[5] = m_bufKeyboard[3*m_byCursorPage];
    if (m_byCursorPos < 6)
        display_digital(byNum,0,m_byCursorPos - 3*m_byCursorPage + 1);
    else
        display_digital("::::::",0,0);  
    if (m_byCursorPage == 0)
    {
        display_char("[[[[[YBU");
    }
    else if (m_byCursorPage == 1)
    {
        display_char("[[[[[YEU");
    }
    else
    {
        display_char("[[[[DONE");
    }
}

static void XtunePwm_OpeningHandler()
{
    m_byCursorPos = 0;
    for (uint16_t i = 0; i < 6; i++)
        m_bufKeyboard[i] = '0';
    m_byCursorPage   = 0;
}

static void XtuneAir_KeyboardHandler(uint8_t byKeyCode)
{
    if (byKeyCode == KEY_1)
    {
        if (m_byCursorPos > 0)
        {
            m_byCursorPos--;
            m_byCursorPage   = m_byCursorPos/2;
        }
		else 
        {
            menu_return( );
        }
    }
    else if (byKeyCode == KEY_2)
    {
        m_byCursorPos++;
        m_byCursorPage   = m_byCursorPos/2;
		if (m_byCursorPos > 4)
			m_byCursorPos = 4;
    }
    else if (byKeyCode == KEY_3)
    {
        if (m_byCursorPos < 4)
        {
            m_bufKeyboard[m_byCursorPos]++;
            if (m_bufKeyboard[m_byCursorPos] > '9')
                m_bufKeyboard[m_byCursorPos] = '0';
        }
        else
        {
            g_UnitCfg.dat.iAirOpen = m_bufKeyboard[1]*10 + m_bufKeyboard[0] - 528;
            g_UnitCfg.dat.iAirClose = m_bufKeyboard[3]*10 + m_bufKeyboard[2] - 528;

            menu_return( );
        }
    }
}

static void XtuneAir_DisplayHandler()
{
    uint8_t byNum[6];

    byNum[0] = '+';
    byNum[1] = ':';
    byNum[2] = ':';
    byNum[3] = ':';
    byNum[4] = m_bufKeyboard[2*m_byCursorPage + 1];
    byNum[5] = m_bufKeyboard[2*m_byCursorPage];
    if (m_byCursorPos < 4)
        display_digital(byNum,0,m_byCursorPos - 2*m_byCursorPage + 1);
    else
        display_digital("::::::",0,0);  
    if (m_byCursorPage == 0)
    {
        display_char("[[[[[OPN");
    }
    else if (m_byCursorPage == 1)
    {
        display_char("[[[[[CLS");
    }
    else
    {
        display_char("[[[[DONE");
    }
}

static void XtuneAir_OpeningHandler()
{
    m_byCursorPos = 0;
    for (uint16_t i = 0; i < 4; i++)
        m_bufKeyboard[i] = '0';
    m_byCursorPage   = 0;
}

static void SigerrFunon_OpeningHandler()
{
    g_UnitCfg.dat.byErr = 1;
    menu_display_done( );
}

static void SigerrPoson_OpeningHandler()
{
	g_UnitCfg.dat.byErr = 0;
    menu_display_done( );
}

static void SigerrOff_OpeningHandler()
{
    g_UnitCfg.dat.byErr = 2;
    menu_display_done( );
}

static void Serio9600_OpeningHandler()
{
    g_UnitCfg.dat.uBau = 9600;
    menu_display_done( );
}

static void Serio19200_OpeningHandler()
{
    g_UnitCfg.dat.uBau = 19200;
    menu_display_done( );
}

static void Serio38400_OpeningHandler()
{
    g_UnitCfg.dat.uBau = 38400;
    menu_display_done( );
}

static void Serio1200_OpeningHandler()
{
    g_UnitCfg.dat.uBau = 1200;
    menu_display_done( );
}

static void Serio2400_OpeningHandler()
{
    g_UnitCfg.dat.uBau = 2400;
    menu_display_done( ); 
}

static void Serio4800_OpeningHandler()
{
    g_UnitCfg.dat.uBau = 4800;
    menu_display_done( ); 
}

static void CodeKey_KeyboardHandler(uint8_t byKeyCode)
{
    if (byKeyCode == KEY_1)
    {
        if (m_byCursorPos > 0)
        {
            m_byCursorPos--;
            m_byCursorPage   = m_byCursorPos/4;
        }
		else 
        {
            menu_return( );
        }
    }
    else if (byKeyCode == KEY_2)
    {
        m_byCursorPos++;
        m_byCursorPage   = m_byCursorPos/4;
		if (m_byCursorPos > 4)
			m_byCursorPos = 4;
    }
    else if (byKeyCode == KEY_3)
    {
        if (m_byCursorPos < 4)
        {
            m_bufKeyboard[m_byCursorPos]++;
            if (m_bufKeyboard[m_byCursorPos] > '9')
                m_bufKeyboard[m_byCursorPos] = '0';
        }
        else
        {
            g_UnitCfg.dat.iCode = m_bufKeyboard[3]*1000 + m_bufKeyboard[2]*100 + m_bufKeyboard[1]*10 + m_bufKeyboard[0] - 53328; 
            menu_return( );
        }
    }
}

static void CodeKey_DisplayHandler()
{
    uint8_t byNum[6];

    byNum[0] = '+';
    byNum[1] = ':';
    byNum[2] = m_bufKeyboard[4*m_byCursorPage + 3];
    byNum[3] = m_bufKeyboard[4*m_byCursorPage + 2];
    byNum[4] = m_bufKeyboard[4*m_byCursorPage + 1];
    byNum[5] = m_bufKeyboard[4*m_byCursorPage];
    if (m_byCursorPos < 4)
    {
        display_digital(byNum,0,m_byCursorPos - 4*m_byCursorPage + 1);
        display_char("[[[[CODE");
    }
    else
    {
        display_digital("::::::",0,0);
        display_char("[[[[DONE");
    }   
}

static void CodeKey_OpeningHandler()
{
    m_byCursorPos = 0;
    for (uint16_t i = 0; i < 4; i++)
        m_bufKeyboard[i] = '0';
    m_byCursorPage   = 0;
}

static void CodeMenu_KeyboardHandler(uint8_t byKeyCode)
{
    if (byKeyCode == KEY_1)
    {
        if (m_byCursorPos > 0)
        {
            m_byCursorPos--;
            m_byCursorPage   = m_byCursorPos/4;
        }
		else 
        {
            menu_return( );
        }
    }
    else if (byKeyCode == KEY_2)
    {
        m_byCursorPos++;
        m_byCursorPage   = m_byCursorPos/4;
		if (m_byCursorPos > 4)
			m_byCursorPos = 4;
    }
    else if (byKeyCode == KEY_3)
    {
        if (m_byCursorPos < 4)
        {
            m_bufKeyboard[m_byCursorPos]++;
            if (m_bufKeyboard[m_byCursorPos] > '9')
                m_bufKeyboard[m_byCursorPos] = '0';
        }
        else
        {
            g_UnitCfg.dat.iCode = m_bufKeyboard[3]*1000 + m_bufKeyboard[2]*100 + m_bufKeyboard[1]*10 + m_bufKeyboard[0] - 53328; 
            menu_return( );
        }
    }
}

static void CodeMenu_DisplayHandler()
{
    uint8_t byNum[6];

    byNum[0] = '+';
    byNum[1] = ':';
    byNum[2] = m_bufKeyboard[4*m_byCursorPage + 3];
    byNum[3] = m_bufKeyboard[4*m_byCursorPage + 2];
    byNum[4] = m_bufKeyboard[4*m_byCursorPage + 1];
    byNum[5] = m_bufKeyboard[4*m_byCursorPage];
    if (m_byCursorPos < 4)
    {
        display_digital(byNum,0,m_byCursorPos - 4*m_byCursorPage + 1);
        display_char("[[[[CODE");
    }
    else
    {
        display_digital("::::::",0,0);
        display_char("[[[[DONE");
    }

}

static void CodeMenu_OpeningHandler()
{
    m_byCursorPos = 0;
    for (uint16_t i = 0; i < 4; i++)
        m_bufKeyboard[i] = '0';
    m_byCursorPage   = 0;
}

static void CodeGlob_KeyboardHandler(uint8_t byKeyCode)
{
    if (byKeyCode == KEY_1)
    {
        if (m_byCursorPos > 0)
        {
            m_byCursorPos--;
            m_byCursorPage   = m_byCursorPos/4;
        }
		else 
        {
            menu_return( );
        }
    }
    else if (byKeyCode == KEY_2)
    {
        m_byCursorPos++;
        m_byCursorPage   = m_byCursorPos/4;
		if (m_byCursorPos > 4)
			m_byCursorPos = 4;
    }
    else if (byKeyCode == KEY_3)
    {
        if (m_byCursorPos < 4)
        {
            m_bufKeyboard[m_byCursorPos]++;
            if (m_bufKeyboard[m_byCursorPos] > '9')
                m_bufKeyboard[m_byCursorPos] = '0';
        }
        else
        {
            g_UnitCfg.dat.iCode = m_bufKeyboard[3]*1000 + m_bufKeyboard[2]*100 + m_bufKeyboard[1]*10 + m_bufKeyboard[0] - 53328; 
            menu_return( );
        }
    }
}

static void CodeGlob_DisplayHandler()
{
    uint8_t byNum[6];

    byNum[0] = '+';
    byNum[1] = ':';
    byNum[2] = m_bufKeyboard[4*m_byCursorPage + 3];
    byNum[3] = m_bufKeyboard[4*m_byCursorPage + 2];
    byNum[4] = m_bufKeyboard[4*m_byCursorPage + 1];
    byNum[5] = m_bufKeyboard[4*m_byCursorPage];
    if (m_byCursorPos < 4)
    {
        display_digital(byNum,0,m_byCursorPos - 4*m_byCursorPage + 1);
        display_char("[[[[CODE");
    }
    else
    {
        display_digital("::::::",0,0);
        display_char("[[[[DONE");
    }   
}

static void CodeGlob_OpeningHandler()
{
    m_byCursorPos = 0;
    for (uint16_t i = 0; i < 4; i++)
        m_bufKeyboard[i] = '0';
    m_byCursorPage   = 0;
}

static void CaluserPos_KeyboardHandler(uint8_t byKeyCode)
{
    if (byKeyCode == KEY_1)
    {
        SetPwmDutyCycle2(-1000);
    }
    else if (byKeyCode == KEY_2)
    {
        SetPwmDutyCycle2(1000);
    }
    else if (byKeyCode == KEY_3)
    {
        if (m_byCursorPage == 0)
        {
            temp1 = g_UnitData.dat.iAD4;
            m_byCursorPage++;
        }
        else if (m_byCursorPage == 1)
        {
            temp2 = g_UnitData.dat.iAD4;
            m_byCursorPage++;
        }
        else
        {
            g_UnitCfg.dat.iAd4Min = temp1;
            g_UnitCfg.dat.iAd4Max = temp2;
            g_UnitCfg.dat.byMode &= 0x7F;
            menu_return( );
        }
    }
}

static void CaluserPos_DisplayHandler()
{
    uint8_t byNum[6];

    if (m_byCursorPage < 2)
    {
        floattochar (g_UnitData.dat.fPos/10, byNum,0);
        display_digital(byNum,0,0);
    }
    else
        display_digital("::::::",0,0);
    if (m_byCursorPage == 0)
    {
        display_char("[[[[[MIN");
    }
    else if (m_byCursorPage == 1)
    {
        display_char("[[[[[MAX");
    }
    else
        display_char("[[[[DONE");
}

static void CaluserPos_OpeningHandler()
{
    m_byCursorPage      = 0;
    g_UnitCfg.dat.byMode |= 0x80;
}

static void CaluserInp_KeyboardHandler(uint8_t byKeyCode)
{
    if (byKeyCode == KEY_1)
    {

    }
    else if (byKeyCode == KEY_2)
    {

    }
    else if (byKeyCode == KEY_3)
    {
        if (m_byCursorPage == 0)
        {
            temp1 = g_UnitData.dat.iAD5;
            m_byCursorPage++;
        }
        else if (m_byCursorPage == 1)
        {
            temp2 = g_UnitData.dat.iAD5;
            m_byCursorPage++;
        }
        else
        {
            switch(g_UnitCfg.dat.byInp)
			{
	        case 1:
	            g_UnitCfg.dat.iAd5Ma0 = temp1;
				g_UnitCfg.dat.iAd5Ma20 = temp2;
	            break;
	        case 2:
	            g_UnitCfg.dat.iAd5V0  = temp1;
				g_UnitCfg.dat.iAd5V5   = temp2;
	            break;
			case 3:
	            g_UnitCfg.dat.iAd5V0  = temp1;
				g_UnitCfg.dat.iAd5V10  = temp2;
	            break;
	        default:            
	            g_UnitCfg.dat.iAd5Ma4 = temp1;
				g_UnitCfg.dat.iAd5Ma20 = temp2;
	            break;
	        }
            menu_return( );
        }
    }
}

static void CaluserInp_DisplayHandler()
{
    uint8_t byNum[6];

    if (m_byCursorPage < 2)
    {
        floattochar (g_UnitData.dat.iInp/10, byNum,0);
        display_digital(byNum,0,0);
    }
    else
        display_digital("::::::",0,0);
    if (m_byCursorPage == 0)
    {
        display_char("[[[[[MIN");
    }
    else if (m_byCursorPage == 1)
    {
        display_char("[[[[[MAX");
    }
    else
        display_char("[[[[DONE");
}

static void CaluserInp_OpeningHandler()
{
    m_byCursorPage      = 0;
}

static void CaluserFact_OpeningHandler()
{
    menu_display_done( );
} 

static void PcontrlDbnd_KeyboardHandler(uint8_t byKeyCode)
{
    if (byKeyCode == KEY_1)
    {
        if (m_byCursorPos > 0)
        {
            m_byCursorPos--;
        }
		else 
        {
            menu_return( );
        }
    }
    else if (byKeyCode == KEY_2)
    {
        m_byCursorPos++;
		if (m_byCursorPos > 3)
			m_byCursorPos = 3;
    }
    else if (byKeyCode == KEY_3)
    {
        if (m_byCursorPos < 3)
        {
            m_bufKeyboard[m_byCursorPos]++;
            if (m_bufKeyboard[m_byCursorPos] > '9')
                m_bufKeyboard[m_byCursorPos] = '0';
        }
        else
        {
            g_UnitCfg.dat.iPidDbnd = m_bufKeyboard[2]*100 + m_bufKeyboard[1]*10 + m_bufKeyboard[0] - 5328;
            menu_return( );
        }
    }
}

static void PcontrlDbnd_DisplayHandler()
{
    uint8_t byNum[6];

    byNum[0] = '+';
    byNum[1] = ':';
    byNum[2] = ':';
    byNum[3] = m_bufKeyboard[3*m_byCursorPage + 2];
    byNum[4] = m_bufKeyboard[3*m_byCursorPage + 1];
    byNum[5] = m_bufKeyboard[3*m_byCursorPage];
    if (m_byCursorPos < 3)
        display_digital(byNum,1,m_byCursorPos - 3*m_byCursorPage + 1);
    else
        display_digital("::::::",0,0);
    display_char("[[[[DBND");
}

static void PcontrlDbnd_OpeningHandler()
{
    m_byCursorPos = 0;
    for (uint16_t i = 0; i < 3; i++)
        m_bufKeyboard[i] = '0';
    m_byCursorPage   = 0;
}

static void PcontrlPara_KeyboardHandler(uint8_t byKeyCode)
{
    if (byKeyCode == KEY_1)
    {
        if (m_byCursorPos > 0)
        {
            m_byCursorPos--;
            m_byCursorPage   = m_byCursorPos/4;
        }
		else 
        {
            menu_return( );
        }
    }
    else if (byKeyCode == KEY_2)
    {
        m_byCursorPos++;
        m_byCursorPage   = m_byCursorPos/4;
		if (m_byCursorPos > 16)
			m_byCursorPos = 16;
    }
    else if (byKeyCode == KEY_3)
    {
        if (m_byCursorPos < 16)
        {
            m_bufKeyboard[m_byCursorPos]++;
            if (m_bufKeyboard[m_byCursorPos] > '9')
                m_bufKeyboard[m_byCursorPos] = '0';
        }
        else
        {
            menu_return( );
        }
    }
}

static void PcontrlPara_DisplayHandler()
{
    uint8_t byNum[6];

    byNum[0] = '+';
    byNum[1] = ':';
    byNum[2] = m_bufKeyboard[4*m_byCursorPage + 3];;
    byNum[3] = m_bufKeyboard[4*m_byCursorPage + 2];
    byNum[4] = m_bufKeyboard[4*m_byCursorPage + 1];
    byNum[5] = m_bufKeyboard[4*m_byCursorPage];

    if (m_byCursorPage == 0)
    {
        display_char("[[[[[[KP");
        display_digital(byNum,2,m_byCursorPos - 4*m_byCursorPage + 1);
    }
    else if (m_byCursorPage == 1)
    {
        display_char("[[[[[[TN");
        display_digital(byNum,1,m_byCursorPos - 4*m_byCursorPage + 1);
    }
    else if (m_byCursorPage == 2)
    {
        display_char("[[[[[[TV");
        display_digital(byNum,1,m_byCursorPos - 4*m_byCursorPage + 1);
    }
    else if (m_byCursorPage == 3)
    {
        display_char("[[[[[[XO");
        display_digital(byNum,1,m_byCursorPos - 4*m_byCursorPage + 1);
    }
    else
    {
        display_char("[[[[DONE");
        display_digital("::::::",0,0);
    }
}

static void PcontrlPara_OpeningHandler()
{
    m_byCursorPos = 0;
    for (uint16_t i = 0; i < 16; i++)
        m_bufKeyboard[i] = '0';
    m_byCursorPage   = 0;
}

static void PcontrlFilt_KeyboardHandler(uint8_t byKeyCode)
{
    if (byKeyCode == KEY_1)
    {
        if (m_byCursorPos > 0)
        {
            m_byCursorPos--;
        }
		else 
        {
            menu_return( );
        }
    }
    else if (byKeyCode == KEY_2)
    {
        m_byCursorPos++;
		if (m_byCursorPos > 1)
			m_byCursorPos = 1;
    }
    else if (byKeyCode == KEY_3)
    {
        if (m_byCursorPos < 1)
        {
            m_bufKeyboard[m_byCursorPos]++;
            if (m_bufKeyboard[m_byCursorPos] > '9')
                m_bufKeyboard[m_byCursorPos] = '0';
        }
        else
        {
            g_UnitCfg.dat.byFilt = m_bufKeyboard[0] - 48;
            menu_return( );
        }
    }
}

static void PcontrlFilt_DisplayHandler()
{
    uint8_t byNum[6];

    byNum[0] = '+';
    byNum[1] = ':';
    byNum[2] = ':';
    byNum[3] = ':';
    byNum[4] = ':';
    byNum[5] = m_bufKeyboard[0];
    if (m_byCursorPos < 1)
    {
        display_digital(byNum,0,1);
        display_char("[[[[FILT");
    }
    else
    {
        display_digital("::::::",0,0);
        display_char("[[[[DONE");
    }       
}

static void PcontrlFilt_OpeningHandler()
{
    m_byCursorPos = 0;
    m_bufKeyboard[0] = '0';
}

static void PcontrlKv_KeyboardHandler(uint8_t byKeyCode)
{
    if (byKeyCode == KEY_1)
    {
        if (m_byCursorPos > 0)
        {
            m_byCursorPos--;
            m_byCursorPage   = m_byCursorPos/4;
        }
		else 
        {
            menu_return( );
        }
    }
    else if (byKeyCode == KEY_2)
    {
        m_byCursorPos++;
        m_byCursorPage   = m_byCursorPos/4;
		if (m_byCursorPos > 16)
			m_byCursorPos = 16;
    }
    else if (byKeyCode == KEY_3)
    {
        if (m_byCursorPos < 16)
        {
            m_bufKeyboard[m_byCursorPos]++;
            if (m_bufKeyboard[m_byCursorPos] > '9')
                m_bufKeyboard[m_byCursorPos] = '0';
        }
        else
        {
            menu_return( );
        }
    }
}

static void PcontrlKv_DisplayHandler()
{
    uint8_t byNum[6];

    byNum[0] = '+';
    byNum[1] = ':';
    byNum[2] = m_bufKeyboard[4*m_byCursorPage + 3];;
    byNum[3] = m_bufKeyboard[4*m_byCursorPage + 2];
    byNum[4] = m_bufKeyboard[4*m_byCursorPage + 1];
    byNum[5] = m_bufKeyboard[4*m_byCursorPage];

    if (m_byCursorPage == 0)
    {
        display_char("[[[[[[KP");
        display_digital(byNum,2,m_byCursorPos - 4*m_byCursorPage + 1);
    }
    else if (m_byCursorPage == 1)
    {
        display_char("[[[[[[TN");
        display_digital(byNum,1,m_byCursorPos - 4*m_byCursorPage + 1);
    }
    else if (m_byCursorPage == 2)
    {
        display_char("[[[[[[TV");
        display_digital(byNum,1,m_byCursorPos - 4*m_byCursorPage + 1);
    }
    else if (m_byCursorPage == 3)
    {
        display_char("[[[[[[XO");
        display_digital(byNum,1,m_byCursorPos - 4*m_byCursorPage + 1);
    }
    else
    {
        display_char("[[[[DONE");
        display_digital("::::::",0,0);
    }
}

static void PcontrlKv_OpeningHandler()
{
    m_byCursorPos = 0;
    for (uint16_t i = 0; i < 16; i++)
        m_bufKeyboard[i] = '0';
    m_byCursorPage   = 0;
}

static void PortD_OpeningHandler()
{
    g_UnitCfg.dat.byIN = (uint8_t)GP4DAT;
    menu_display_done( );
}

static void AnlPos_OpeningHandler()
{
    g_UnitCfg.dat.byAnlDat = 0;
	menu_display_done( );
}

static void AnlCmd_OpeningHandler()
{
    g_UnitCfg.dat.byAnlDat = 1;
	menu_display_done( );
}

static void AnlSp_OpeningHandler()
{
    g_UnitCfg.dat.byAnlDat = 2;
	menu_display_done( ); 
}

static void AnlP1_OpeningHandler()
{
    g_UnitCfg.dat.byAnlDat = 3;
	menu_display_done( );
}

static void AnlP2_OpeningHandler()
{
    g_UnitCfg.dat.byAnlDat = 4;
	menu_display_done( );
}

static void AnlT_OpeningHandler()
{
    g_UnitCfg.dat.byAnlDat = 5;
	menu_display_done( );
}

static void AnlSet4_OpeningHandler()
{
    g_UnitCfg.dat.byAnlCtl = 5;
	menu_display_done( );
}

static void AnlSet0_OpeningHandler()
{
    g_UnitCfg.dat.byAnlCtl = 6;
	menu_display_done( );
}

static void AnlSet10_OpeningHandler()
{
    g_UnitCfg.dat.byAnlCtl = 1;
	menu_display_done( );
}

static void AnlSet5_OpeningHandler()
{
    g_UnitCfg.dat.byAnlCtl = 0;
	menu_display_done( ); 
}

static void Bin1Drv_OpeningHandler()
{
    g_UnitCfg.dat.byOUT &= ~0x38; 
    g_UnitCfg.dat.byOUT |= 0x08;
    menu_display_done( );
}

static void Bin1Lim_OpeningHandler()
{
    g_UnitCfg.dat.byOUT &= ~0x38; 
    g_UnitCfg.dat.byOUT |= 0x10;
    menu_display_done( );
}

static void Bin1Spos_OpeningHandler()
{
    g_UnitCfg.dat.byOUT &= ~0x38; 
    g_UnitCfg.dat.byOUT |= 0x18;
    menu_display_done( );
}

static void Bin1ErrS_OpeningHandler()
{
    g_UnitCfg.dat.byOUT &= ~0x38; 
    g_UnitCfg.dat.byOUT |= 0x20;
    menu_display_done( ); 
}

static void Bin1Err1_OpeningHandler()
{
    g_UnitCfg.dat.byOUT &= ~0x38; 
    g_UnitCfg.dat.byOUT |= 0x28;
    menu_display_done( );
}

static void Bin1Err2_OpeningHandler()
{
    g_UnitCfg.dat.byOUT &= ~0x38; 
    g_UnitCfg.dat.byOUT |= 0x30;
    menu_display_done( );
}

static void Bin1ErrT_OpeningHandler()
{
    g_UnitCfg.dat.byOUT &= ~0x38; 
    g_UnitCfg.dat.byOUT |= 0x38; 
    menu_display_done( ); 
}

static void Bin1Opn_OpeningHandler()
{
    g_UnitCfg.dat.byOUT |= 0x80; 
    menu_display_done( );
}

static void Bin1Cls_OpeningHandler()
{
    g_UnitCfg.dat.byOUT &= ~0x80;
    menu_display_done( ); 
}

static void Bin2Drv_OpeningHandler()
{
    g_UnitCfg.dat.byOUT &= ~0x07; 
    g_UnitCfg.dat.byOUT |= 0x01;
    menu_display_done( );
}

static void Bin2Lim_OpeningHandler()
{
    g_UnitCfg.dat.byOUT &= ~0x07; 
    g_UnitCfg.dat.byOUT |= 0x01;
    menu_display_done( ); 
}

static void Bin2Spos_OpeningHandler()
{
    g_UnitCfg.dat.byOUT &= ~0x07; 
    g_UnitCfg.dat.byOUT |= 0x01;
    menu_display_done( );
}

static void Bin2ErrS_OpeningHandler()
{
    g_UnitCfg.dat.byOUT &= ~0x07; 
    g_UnitCfg.dat.byOUT |= 0x01;
    menu_display_done( );
}

static void Bin2Err1_OpeningHandler()
{
    g_UnitCfg.dat.byOUT &= ~0x07; 
    g_UnitCfg.dat.byOUT |= 0x01;
    menu_display_done( );
}

static void Bin2Err2_OpeningHandler()
{
    g_UnitCfg.dat.byOUT &= ~0x07; 
    g_UnitCfg.dat.byOUT |= 0x01;
    menu_display_done( );
}

static void Bin2ErrT_OpeningHandler()
{
    g_UnitCfg.dat.byOUT &= ~0x07; 
    g_UnitCfg.dat.byOUT |= 0x07;
    menu_display_done( );
}

static void Bin2Opn_OpeningHandler()
{
    g_UnitCfg.dat.byOUT |= 0x40; 
    menu_display_done( );
}

static void Bin2Cls_OpeningHandler()
{
    g_UnitCfg.dat.byOUT &= ~0x40;
    menu_display_done( );
}

static void ScalS_KeyboardHandler(uint8_t byKeyCode)
{
    if (byKeyCode == KEY_1)
    {
        if (m_byCursorPos > 0)
        {
            m_byCursorPos--;
            m_byCursorPage   = m_byCursorPos/4;
        }
		else 
        {
            menu_return( );
        }
    }
    else if (byKeyCode == KEY_2)
    {
        m_byCursorPos++;
        m_byCursorPage   = m_byCursorPos/4;
		if (m_byCursorPos > 16)
			m_byCursorPos = 16;
    }
    else if (byKeyCode == KEY_3)
    {
        if (m_byCursorPos < 16)
        {
            m_bufKeyboard[m_byCursorPos]++;
            if (m_bufKeyboard[m_byCursorPos] > '9')
                m_bufKeyboard[m_byCursorPos] = '0';
        }
        else
        {
            menu_return( );
        }
    }
}

static void ScalS_DisplayHandler()
{
    uint8_t byNum[6];

    byNum[0] = '+';
    byNum[1] = ':';
    byNum[2] = m_bufKeyboard[4*m_byCursorPage + 3];;
    byNum[3] = m_bufKeyboard[4*m_byCursorPage + 2];
    byNum[4] = m_bufKeyboard[4*m_byCursorPage + 1];
    byNum[5] = m_bufKeyboard[4*m_byCursorPage];

    if (m_byCursorPage == 0)
    {
        display_char("[[[[[[KP");
        display_digital(byNum,2,m_byCursorPos - 4*m_byCursorPage + 1);
    }
    else if (m_byCursorPage == 1)
    {
        display_char("[[[[[[TN");
        display_digital(byNum,1,m_byCursorPos - 4*m_byCursorPage + 1);
    }
    else if (m_byCursorPage == 2)
    {
        display_char("[[[[[[TV");
        display_digital(byNum,1,m_byCursorPos - 4*m_byCursorPage + 1);
    }
    else if (m_byCursorPage == 3)
    {
        display_char("[[[[[[XO");
        display_digital(byNum,1,m_byCursorPos - 4*m_byCursorPage + 1);
    }
    else
    {
        display_char("[[[[DONE");
        display_digital("::::::",0,0);
    }
}

static void ScalS_OpeningHandler()
{
    m_byCursorPos = 0;
    for (uint16_t i = 0; i < 16; i++)
        m_bufKeyboard[i] = '0';
    m_byCursorPage   = 0;
}

static void ScalF_KeyboardHandler(uint8_t byKeyCode)
{
    if (byKeyCode == KEY_1)
    {
        if (m_byCursorPos > 0)
        {
            m_byCursorPos--;
            m_byCursorPage   = m_byCursorPos/4;
        }
		else 
        {
            menu_return( );
        }
    }
    else if (byKeyCode == KEY_2)
    {
        m_byCursorPos++;
        m_byCursorPage   = m_byCursorPos/4;
		if (m_byCursorPos > 16)
			m_byCursorPos = 16;
    }
    else if (byKeyCode == KEY_3)
    {
        if (m_byCursorPos < 16)
        {
            m_bufKeyboard[m_byCursorPos]++;
            if (m_bufKeyboard[m_byCursorPos] > '9')
                m_bufKeyboard[m_byCursorPos] = '0';
        }
        else
        {
            menu_return( );
        }
    }
}

static void ScalF_DisplayHandler()
{
    uint8_t byNum[6];

    byNum[0] = '+';
    byNum[1] = ':';
    byNum[2] = m_bufKeyboard[4*m_byCursorPage + 3];;
    byNum[3] = m_bufKeyboard[4*m_byCursorPage + 2];
    byNum[4] = m_bufKeyboard[4*m_byCursorPage + 1];
    byNum[5] = m_bufKeyboard[4*m_byCursorPage];

    if (m_byCursorPage == 0)
    {
        display_char("[[[[[[KP");
        display_digital(byNum,2,m_byCursorPos - 4*m_byCursorPage + 1);
    }
    else if (m_byCursorPage == 1)
    {
        display_char("[[[[[[TN");
        display_digital(byNum,1,m_byCursorPos - 4*m_byCursorPage + 1);
    }
    else if (m_byCursorPage == 2)
    {
        display_char("[[[[[[TV");
        display_digital(byNum,1,m_byCursorPos - 4*m_byCursorPage + 1);
    }
    else if (m_byCursorPage == 3)
    {
        display_char("[[[[[[XO");
        display_digital(byNum,1,m_byCursorPos - 4*m_byCursorPage + 1);
    }
    else
    {
        display_char("[[[[DONE");
        display_digital("::::::",0,0);
    }
}

static void ScalF_OpeningHandler()
{
    m_byCursorPos = 0;
    for (uint16_t i = 0; i < 16; i++)
        m_bufKeyboard[i] = '0';
    m_byCursorPage   = 0;
}

static void ScalT_KeyboardHandler(uint8_t byKeyCode)
{
    if (byKeyCode == KEY_1)
    {
        if (m_byCursorPos > 0)
        {
            m_byCursorPos--;
            m_byCursorPage   = m_byCursorPos/4;
        }
		else 
        {
            menu_return( );
        }
    }
    else if (byKeyCode == KEY_2)
    {
        m_byCursorPos++;
        m_byCursorPage   = m_byCursorPos/4;
		if (m_byCursorPos > 16)
			m_byCursorPos = 16;
    }
    else if (byKeyCode == KEY_3)
    {
        if (m_byCursorPos < 16)
        {
            m_bufKeyboard[m_byCursorPos]++;
            if (m_bufKeyboard[m_byCursorPos] > '9')
                m_bufKeyboard[m_byCursorPos] = '0';
        }
        else
        {
            menu_return( );
        }
    }
}

static void ScalT_DisplayHandler()
{
    uint8_t byNum[6];

    byNum[0] = '+';
    byNum[1] = ':';
    byNum[2] = m_bufKeyboard[4*m_byCursorPage + 3];;
    byNum[3] = m_bufKeyboard[4*m_byCursorPage + 2];
    byNum[4] = m_bufKeyboard[4*m_byCursorPage + 1];
    byNum[5] = m_bufKeyboard[4*m_byCursorPage];

    if (m_byCursorPage == 0)
    {
        display_char("[[[[[[KP");
        display_digital(byNum,2,m_byCursorPos - 4*m_byCursorPage + 1);
    }
    else if (m_byCursorPage == 1)
    {
        display_char("[[[[[[TN");
        display_digital(byNum,1,m_byCursorPos - 4*m_byCursorPage + 1);
    }
    else if (m_byCursorPage == 2)
    {
        display_char("[[[[[[TV");
        display_digital(byNum,1,m_byCursorPos - 4*m_byCursorPage + 1);
    }
    else if (m_byCursorPage == 3)
    {
        display_char("[[[[[[XO");
        display_digital(byNum,1,m_byCursorPos - 4*m_byCursorPage + 1);
    }
    else
    {
        display_char("[[[[DONE");
        display_digital("::::::",0,0);
    }
}

static void ScalT_OpeningHandler()
{
    m_byCursorPos = 0;
    for (uint16_t i = 0; i < 16; i++)
        m_bufKeyboard[i] = '0';
    m_byCursorPage   = 0;
}
static void ScalP_KeyboardHandler(uint8_t byKeyCode)
{
    if (byKeyCode == KEY_1)
    {
        if (m_byCursorPos > 0)
        {
            m_byCursorPos--;
            m_byCursorPage   = m_byCursorPos/4;
        }
		else 
        {
            menu_return( );
        }
    }
    else if (byKeyCode == KEY_2)
    {
        m_byCursorPos++;
        m_byCursorPage   = m_byCursorPos/4;
		if (m_byCursorPos > 16)
			m_byCursorPos = 16;
    }
    else if (byKeyCode == KEY_3)
    {
        if (m_byCursorPos < 16)
        {
            m_bufKeyboard[m_byCursorPos]++;
            if (m_bufKeyboard[m_byCursorPos] > '9')
                m_bufKeyboard[m_byCursorPos] = '0';
        }
        else
        {
            menu_return( );
        }
    }
}

static void ScalP_DisplayHandler()
{
    uint8_t byNum[6];

    byNum[0] = '+';
    byNum[1] = ':';
    byNum[2] = m_bufKeyboard[4*m_byCursorPage + 3];;
    byNum[3] = m_bufKeyboard[4*m_byCursorPage + 2];
    byNum[4] = m_bufKeyboard[4*m_byCursorPage + 1];
    byNum[5] = m_bufKeyboard[4*m_byCursorPage];

    if (m_byCursorPage == 0)
    {
        display_char("[[[[[[KP");
        display_digital(byNum,2,m_byCursorPos - 4*m_byCursorPage + 1);
    }
    else if (m_byCursorPage == 1)
    {
        display_char("[[[[[[TN");
        display_digital(byNum,1,m_byCursorPos - 4*m_byCursorPage + 1);
    }
    else if (m_byCursorPage == 2)
    {
        display_char("[[[[[[TV");
        display_digital(byNum,1,m_byCursorPos - 4*m_byCursorPage + 1);
    }
    else if (m_byCursorPage == 3)
    {
        display_char("[[[[[[XO");
        display_digital(byNum,1,m_byCursorPos - 4*m_byCursorPage + 1);
    }
    else
    {
        display_char("[[[[DONE");
        display_digital("::::::",0,0);
    }
}

static void ScalP_OpeningHandler()
{
    m_byCursorPos = 0;
    for (uint16_t i = 0; i < 16; i++)
        m_bufKeyboard[i] = '0';
    m_byCursorPage   = 0;
}

static void TuneNot_OpeningHandler()
{
    menu_display_done( );
}

static void TuneDef_OpeningHandler()
{
    menu_display_done( ); 
}

static void TuneFlow_OpeningHandler()
{
    menu_display_done( ); 
}

static void TuneTemp_OpeningHandler()
{
    menu_display_done( );
}

static void TunePres_OpeningHandler()
{
    menu_display_done( ); 
}

static void TuneLevl_OpeningHandler()
{
    menu_display_done( );
}

static void LeakNot_OpeningHandler()
{
    menu_display_done( );
}

static void LeakMeas_KeyboardHandler(uint8_t byKeyCode)
{
    if (byKeyCode == KEY_1)
    {
        if (m_byCursorPos > 0)
        {
            m_byCursorPos--;
            m_byCursorPage   = m_byCursorPos/4;
        }
		else 
        {
            menu_return( );
        }
    }
    else if (byKeyCode == KEY_2)
    {
        m_byCursorPos++;
        m_byCursorPage   = m_byCursorPos/4;
		if (m_byCursorPos > 16)
			m_byCursorPos = 16;
    }
    else if (byKeyCode == KEY_3)
    {
        if (m_byCursorPos < 16)
        {
            m_bufKeyboard[m_byCursorPos]++;
            if (m_bufKeyboard[m_byCursorPos] > '9')
                m_bufKeyboard[m_byCursorPos] = '0';
        }
        else
        {
            menu_return( );
        }
    }
}

static void LeakMeas_DisplayHandler()
{
    uint8_t byNum[6];

    byNum[0] = '+';
    byNum[1] = ':';
    byNum[2] = m_bufKeyboard[4*m_byCursorPage + 3];;
    byNum[3] = m_bufKeyboard[4*m_byCursorPage + 2];
    byNum[4] = m_bufKeyboard[4*m_byCursorPage + 1];
    byNum[5] = m_bufKeyboard[4*m_byCursorPage];

    if (m_byCursorPage == 0)
    {
        display_char("[[[[[[KP");
        display_digital(byNum,2,m_byCursorPos - 4*m_byCursorPage + 1);
    }
    else if (m_byCursorPage == 1)
    {
        display_char("[[[[[[TN");
        display_digital(byNum,1,m_byCursorPos - 4*m_byCursorPage + 1);
    }
    else if (m_byCursorPage == 2)
    {
        display_char("[[[[[[TV");
        display_digital(byNum,1,m_byCursorPos - 4*m_byCursorPage + 1);
    }
    else if (m_byCursorPage == 3)
    {
        display_char("[[[[[[XO");
        display_digital(byNum,1,m_byCursorPos - 4*m_byCursorPage + 1);
    }
    else
    {
        display_char("[[[[DONE");
        display_digital("::::::",0,0);
    }
}

static void LeakMeas_OpeningHandler()
{
    m_byCursorPos = 0;
    for (uint16_t i = 0; i < 16; i++)
        m_bufKeyboard[i] = '0';
    m_byCursorPage   = 0;
}

static void LeakChar_OpeningHandler()
{
    menu_display_done( );
}

void HMI_Init()
{
    initLCD_1622();
    /* Enable internal pull-up resister*/
    GP2CON  &= ~0x00000033;
    GP0CON  &= ~0x00030000;

    /* Init Main Menu*/
    m_aMenuItems[MENU_MAIN_ACT].byMenuItemID                = MENU_MAIN_ACT;
    m_aMenuItems[MENU_MAIN_ACT].byChildMenuItems            = 8;
    m_aMenuItems[MENU_MAIN_ACT].pChildMenu                  = &m_aMenuItems[MENU_ACT_SNGL];
    m_aMenuItems[MENU_MAIN_ACT].pParentMenu                 = NULL;
    m_aMenuItems[MENU_MAIN_ACT].KeyboardHandler             = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_MAIN_ACT].DisplayHandler              = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_MAIN_ACT].OnOpeningHandler            = NULL;

    m_aMenuItems[MENU_MAIN_INPUT].byMenuItemID              = MENU_MAIN_INPUT;
    m_aMenuItems[MENU_MAIN_INPUT].byChildMenuItems          = 8;
    m_aMenuItems[MENU_MAIN_INPUT].pChildMenu                = &m_aMenuItems[MENU_INPUT_4];
    m_aMenuItems[MENU_MAIN_INPUT].pParentMenu               = NULL;
    m_aMenuItems[MENU_MAIN_INPUT].KeyboardHandler           = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_MAIN_INPUT].DisplayHandler            = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_MAIN_INPUT].OnOpeningHandler          = NULL;

    m_aMenuItems[MENU_MAIN_CHARACT].byMenuItemID            = MENU_MAIN_CHARACT;
    m_aMenuItems[MENU_MAIN_CHARACT].pChildMenu              = &m_aMenuItems[MENU_CHARACT_l_1];
    m_aMenuItems[MENU_MAIN_CHARACT].pParentMenu             = NULL;
    m_aMenuItems[MENU_MAIN_CHARACT].KeyboardHandler         = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_MAIN_CHARACT].DisplayHandler          = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_MAIN_CHARACT].OnOpeningHandler        = NULL;

    m_aMenuItems[MENU_MAIN_CUTOFF].byMenuItemID             = MENU_MAIN_CUTOFF;
    m_aMenuItems[MENU_MAIN_CUTOFF].pChildMenu               = NULL;
    m_aMenuItems[MENU_MAIN_CUTOFF].pParentMenu              = NULL;
    m_aMenuItems[MENU_MAIN_CUTOFF].KeyboardHandler          = InsetCutoff_KeyboardHandler;
    m_aMenuItems[MENU_MAIN_CUTOFF].DisplayHandler           = InsetCutoff_DisplayHandler;
    m_aMenuItems[MENU_MAIN_CUTOFF].OnOpeningHandler         = InsetCutoff_OpeningHandler;

    m_aMenuItems[MENU_MAIN_DIRCMD].byMenuItemID             = MENU_MAIN_DIRCMD;
    m_aMenuItems[MENU_MAIN_DIRCMD].pChildMenu               = &m_aMenuItems[MENU_DIRCMD_RISE];
    m_aMenuItems[MENU_MAIN_DIRCMD].pParentMenu              = NULL;
    m_aMenuItems[MENU_MAIN_DIRCMD].KeyboardHandler          = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_MAIN_DIRCMD].DisplayHandler           = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_MAIN_DIRCMD].OnOpeningHandler         = NULL;

    m_aMenuItems[MENU_MAIN_DIRACT].byMenuItemID             = MENU_MAIN_DIRACT;
    m_aMenuItems[MENU_MAIN_DIRACT].pChildMenu               = &m_aMenuItems[MENU_DIRACT_RISE];
    m_aMenuItems[MENU_MAIN_DIRACT].pParentMenu              = NULL;
    m_aMenuItems[MENU_MAIN_DIRACT].KeyboardHandler          = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_MAIN_DIRACT].DisplayHandler           = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_MAIN_DIRACT].OnOpeningHandler         = NULL;

    m_aMenuItems[MENU_MAIN_SPLTRNG].byMenuItemID            = MENU_MAIN_SPLTRNG;
    m_aMenuItems[MENU_MAIN_SPLTRNG].pChildMenu              = NULL;
    m_aMenuItems[MENU_MAIN_SPLTRNG].pParentMenu             = NULL;
    m_aMenuItems[MENU_MAIN_SPLTRNG].KeyboardHandler         = InsetSpltrng_KeyboardHandler;
    m_aMenuItems[MENU_MAIN_SPLTRNG].DisplayHandler          = InsetSpltrng_DisplayHandler;
    m_aMenuItems[MENU_MAIN_SPLTRNG].OnOpeningHandler        = InsetSpltrng_OpeningHandler;

    m_aMenuItems[MENU_MAIN_XLIMIT].byMenuItemID             = MENU_MAIN_XLIMIT;
    m_aMenuItems[MENU_MAIN_XLIMIT].pChildMenu               = NULL;
    m_aMenuItems[MENU_MAIN_XLIMIT].pParentMenu              = NULL;
    m_aMenuItems[MENU_MAIN_XLIMIT].KeyboardHandler          = XsetXlimit_KeyboardHandler;
    m_aMenuItems[MENU_MAIN_XLIMIT].DisplayHandler           = XsetXlimit_DisplayHandler;
    m_aMenuItems[MENU_MAIN_XLIMIT].OnOpeningHandler         = XsetXlimit_OpeningHandler;

    m_aMenuItems[MENU_MAIN_XTIME].byMenuItemID              = MENU_MAIN_XTIME;
    m_aMenuItems[MENU_MAIN_XTIME].pChildMenu                = NULL;
    m_aMenuItems[MENU_MAIN_XTIME].pParentMenu               = NULL;
    m_aMenuItems[MENU_MAIN_XTIME].KeyboardHandler           = XsetXtime_KeyboardHandler;
    m_aMenuItems[MENU_MAIN_XTIME].DisplayHandler            = XsetXtime_DisplayHandler;
    m_aMenuItems[MENU_MAIN_XTIME].OnOpeningHandler          = XsetXtime_OpeningHandler;

    m_aMenuItems[MENU_MAIN_XCONTRL].byMenuItemID            = MENU_MAIN_XCONTRL;
    m_aMenuItems[MENU_MAIN_XCONTRL].pChildMenu              = &m_aMenuItems[MENU_XCONTRL_END];
    m_aMenuItems[MENU_MAIN_XCONTRL].pParentMenu             = NULL;
    m_aMenuItems[MENU_MAIN_XCONTRL].KeyboardHandler         = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_MAIN_XCONTRL].DisplayHandler          = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_MAIN_XCONTRL].OnOpeningHandler        = NULL;

    m_aMenuItems[MENU_MAIN_PCONTRL].byMenuItemID            = MENU_MAIN_PCONTRL;
    m_aMenuItems[MENU_MAIN_PCONTRL].pChildMenu              = &m_aMenuItems[MENU_PCONTRL_END];
    m_aMenuItems[MENU_MAIN_PCONTRL].pParentMenu             = NULL;
    m_aMenuItems[MENU_MAIN_PCONTRL].KeyboardHandler         = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_MAIN_PCONTRL].DisplayHandler          = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_MAIN_PCONTRL].OnOpeningHandler        = NULL;

    m_aMenuItems[MENU_MAIN_CODE].byMenuItemID               = MENU_MAIN_CODE;
    m_aMenuItems[MENU_MAIN_CODE].pChildMenu                 = &m_aMenuItems[MENU_CODE_END];
    m_aMenuItems[MENU_MAIN_CODE].pParentMenu                = NULL;
    m_aMenuItems[MENU_MAIN_CODE].KeyboardHandler            = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_MAIN_CODE].DisplayHandler             = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_MAIN_CODE].OnOpeningHandler           = NULL;

    m_aMenuItems[MENU_MAIN_SAFEPOS].byMenuItemID            = MENU_MAIN_SAFEPOS;
    m_aMenuItems[MENU_MAIN_SAFEPOS].pChildMenu              = NULL;
    m_aMenuItems[MENU_MAIN_SAFEPOS].pParentMenu             = NULL;
    m_aMenuItems[MENU_MAIN_SAFEPOS].KeyboardHandler         = SystemSafepos_KeyboardHandler;
    m_aMenuItems[MENU_MAIN_SAFEPOS].DisplayHandler          = SystemSafepos_DisplayHandler;
    m_aMenuItems[MENU_MAIN_SAFEPOS].OnOpeningHandler        = SystemSafepos_OpeningHandler;

    m_aMenuItems[MENU_MAIN_SIGERR].byMenuItemID             = MENU_MAIN_SIGERR;
    m_aMenuItems[MENU_MAIN_SIGERR].pChildMenu               = &m_aMenuItems[MENU_SIGERR_FUNON];
    m_aMenuItems[MENU_MAIN_SIGERR].pParentMenu              = NULL;
    m_aMenuItems[MENU_MAIN_SIGERR].KeyboardHandler          = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_MAIN_SIGERR].DisplayHandler           = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_MAIN_SIGERR].OnOpeningHandler         = NULL;

    m_aMenuItems[MENU_MAIN_BININ].byMenuItemID              = MENU_MAIN_BININ;
    m_aMenuItems[MENU_MAIN_BININ].pChildMenu                = &m_aMenuItems[MENU_BININ_SPOS];
    m_aMenuItems[MENU_MAIN_BININ].pParentMenu               = NULL;
    m_aMenuItems[MENU_MAIN_BININ].KeyboardHandler           = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_MAIN_BININ].DisplayHandler            = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_MAIN_BININ].OnOpeningHandler          = NULL;

    m_aMenuItems[MENU_MAIN_OUTPUT].byMenuItemID             = MENU_MAIN_OUTPUT;
    m_aMenuItems[MENU_MAIN_OUTPUT].pChildMenu               = &m_aMenuItems[MENU_OUTPUT_ANL];
    m_aMenuItems[MENU_MAIN_OUTPUT].pParentMenu              = NULL;
    m_aMenuItems[MENU_MAIN_OUTPUT].KeyboardHandler          = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_MAIN_OUTPUT].DisplayHandler           = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_MAIN_OUTPUT].OnOpeningHandler         = NULL;

    m_aMenuItems[MENU_MAIN_CALUSER].byMenuItemID            = MENU_MAIN_CALUSER;
    m_aMenuItems[MENU_MAIN_CALUSER].pChildMenu              = &m_aMenuItems[MENU_CALUSER_END];
    m_aMenuItems[MENU_MAIN_CALUSER].pParentMenu             = NULL;
    m_aMenuItems[MENU_MAIN_CALUSER].KeyboardHandler         = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_MAIN_CALUSER].DisplayHandler          = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_MAIN_CALUSER].OnOpeningHandler        = NULL;

    m_aMenuItems[MENU_MAIN_SETFACT].byMenuItemID            = MENU_MAIN_SETFACT;
    m_aMenuItems[MENU_MAIN_SETFACT].pChildMenu              = NULL;
    m_aMenuItems[MENU_MAIN_SETFACT].pParentMenu             = NULL;
    m_aMenuItems[MENU_MAIN_SETFACT].KeyboardHandler         = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_MAIN_SETFACT].DisplayHandler          = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_MAIN_SETFACT].OnOpeningHandler        = SystemSetfact_OpeningHandler;

    m_aMenuItems[MENU_MAIN_SERIO].byMenuItemID              = MENU_MAIN_SERIO;
    m_aMenuItems[MENU_MAIN_SERIO].pChildMenu                = &m_aMenuItems[MENU_SERIO_9600];
    m_aMenuItems[MENU_MAIN_SERIO].pParentMenu               = NULL;
    m_aMenuItems[MENU_MAIN_SERIO].KeyboardHandler           = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_MAIN_SERIO].DisplayHandler            = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_MAIN_SERIO].OnOpeningHandler          = NULL;    

    m_aMenuItems[MENU_MAIN_ADDFUN].byMenuItemID             = MENU_MAIN_ADDFUN;
    m_aMenuItems[MENU_MAIN_ADDFUN].byChildMenuItems         = 8;
    m_aMenuItems[MENU_MAIN_ADDFUN].pChildMenu               = NULL;
    m_aMenuItems[MENU_MAIN_ADDFUN].pParentMenu              = NULL;
    m_aMenuItems[MENU_MAIN_ADDFUN].KeyboardHandler          = AddFun_KeyboardHandler;
    m_aMenuItems[MENU_MAIN_ADDFUN].DisplayHandler           = AddFun_DisplayHandler;
    m_aMenuItems[MENU_MAIN_ADDFUN].OnOpeningHandler         = AddFun_OpeningHandler;

    m_aMenuItems[MENU_MAIN_MANUAL].byMenuItemID             = MENU_MAIN_MANUAL;
    m_aMenuItems[MENU_MAIN_MANUAL].byChildMenuItems         = 8;
    m_aMenuItems[MENU_MAIN_MANUAL].pChildMenu               = NULL;
    m_aMenuItems[MENU_MAIN_MANUAL].pParentMenu              = NULL;
    m_aMenuItems[MENU_MAIN_MANUAL].KeyboardHandler          = MENU_MAIN_MANUAL_KeyboardHandler;
    m_aMenuItems[MENU_MAIN_MANUAL].DisplayHandler           = MENU_MAIN_MANUAL_DisplayHandler;
    m_aMenuItems[MENU_MAIN_MANUAL].OnOpeningHandler         = MENU_MAIN_MANUAL_OpeningHandler;

    m_aMenuItems[MENU_MAIN_XTUNE].byMenuItemID              = MENU_MAIN_XTUNE;
    m_aMenuItems[MENU_MAIN_XTUNE].byChildMenuItems          = 8;
    m_aMenuItems[MENU_MAIN_XTUNE].pChildMenu                = &m_aMenuItems[MENU_XTUNE_AUTO];
    m_aMenuItems[MENU_MAIN_XTUNE].pParentMenu               = NULL;
    m_aMenuItems[MENU_MAIN_XTUNE].KeyboardHandler           = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_MAIN_XTUNE].DisplayHandler            = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_MAIN_XTUNE].OnOpeningHandler          = NULL;

    m_aMenuItems[MENU_MAIN_END].byMenuItemID                = MENU_MAIN_END;
    m_aMenuItems[MENU_MAIN_END].byChildMenuItems            = 8;
    m_aMenuItems[MENU_MAIN_END].pChildMenu                  = NULL;
    m_aMenuItems[MENU_MAIN_END].pParentMenu                 = NULL;
    m_aMenuItems[MENU_MAIN_END].KeyboardHandler             = NULL;
    m_aMenuItems[MENU_MAIN_END].DisplayHandler              = NULL;
    m_aMenuItems[MENU_MAIN_END].OnOpeningHandler            = NULL;

    /* Init Act Menu*/
    m_aMenuItems[MENU_ACT_SNGL].byMenuItemID                = MENU_ACT_SNGL;
    m_aMenuItems[MENU_ACT_SNGL].byChildMenuItems            = 3;
    m_aMenuItems[MENU_ACT_SNGL].pChildMenu                  = NULL;
    m_aMenuItems[MENU_ACT_SNGL].pParentMenu                 = &m_aMenuItems[MENU_MAIN_ACT];
    m_aMenuItems[MENU_ACT_SNGL].KeyboardHandler             = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_ACT_SNGL].DisplayHandler              = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_ACT_SNGL].OnOpeningHandler            = ActSngl_OpeningHandler;

    m_aMenuItems[MENU_ACT_DOUB].byMenuItemID                = MENU_ACT_DOUB;
    m_aMenuItems[MENU_ACT_DOUB].byChildMenuItems            = 3;
    m_aMenuItems[MENU_ACT_DOUB].pChildMenu                  = NULL;
    m_aMenuItems[MENU_ACT_DOUB].pParentMenu                 = &m_aMenuItems[MENU_MAIN_ACT];
    m_aMenuItems[MENU_ACT_DOUB].KeyboardHandler             = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_ACT_DOUB].DisplayHandler              = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_ACT_DOUB].OnOpeningHandler            = ActDoub_OpeningHandler;

    m_aMenuItems[MENU_ACT_END].byMenuItemID                 = MENU_ACT_END;
    m_aMenuItems[MENU_ACT_END].byChildMenuItems             = 3;
    m_aMenuItems[MENU_ACT_END].pChildMenu                   = &m_aMenuItems[MENU_MAIN_ACT];
    m_aMenuItems[MENU_ACT_END].pParentMenu                  = &m_aMenuItems[MENU_MAIN_ACT];
    m_aMenuItems[MENU_ACT_END].KeyboardHandler              = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_ACT_END].DisplayHandler               = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_ACT_END].OnOpeningHandler             = NULL;

    /* Init Input Menu*/
    m_aMenuItems[MENU_INPUT_4].byMenuItemID                 = MENU_INPUT_4;
    m_aMenuItems[MENU_INPUT_4].byChildMenuItems             = 5;
    m_aMenuItems[MENU_INPUT_4].pChildMenu                   = NULL;
    m_aMenuItems[MENU_INPUT_4].pParentMenu                  = &m_aMenuItems[MENU_MAIN_INPUT];
    m_aMenuItems[MENU_INPUT_4].KeyboardHandler              = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_INPUT_4].DisplayHandler               = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_INPUT_4].OnOpeningHandler             = Input4_OpeningHandler;

    m_aMenuItems[MENU_INPUT_0].byMenuItemID                 = MENU_INPUT_0;
    m_aMenuItems[MENU_INPUT_0].byChildMenuItems             = 5;
    m_aMenuItems[MENU_INPUT_0].pChildMenu                   = NULL;
    m_aMenuItems[MENU_INPUT_0].pParentMenu                  = &m_aMenuItems[MENU_MAIN_INPUT];
    m_aMenuItems[MENU_INPUT_0].KeyboardHandler              = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_INPUT_0].DisplayHandler               = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_INPUT_0].OnOpeningHandler             = Input0_OpeningHandler;

    m_aMenuItems[MENU_INPUT_10].byMenuItemID                = MENU_INPUT_10;
    m_aMenuItems[MENU_INPUT_10].byChildMenuItems            = 5;
    m_aMenuItems[MENU_INPUT_10].pChildMenu                  = NULL;
    m_aMenuItems[MENU_INPUT_10].pParentMenu                 = &m_aMenuItems[MENU_MAIN_INPUT];
    m_aMenuItems[MENU_INPUT_10].KeyboardHandler             = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_INPUT_10].DisplayHandler              = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_INPUT_10].OnOpeningHandler            = Input10_OpeningHandler;

    m_aMenuItems[MENU_INPUT_5].byMenuItemID                 = MENU_INPUT_5;
    m_aMenuItems[MENU_INPUT_5].byChildMenuItems             = 5;
    m_aMenuItems[MENU_INPUT_5].pChildMenu                   = NULL;
    m_aMenuItems[MENU_INPUT_5].pParentMenu                  = &m_aMenuItems[MENU_MAIN_INPUT];
    m_aMenuItems[MENU_INPUT_5].KeyboardHandler              = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_INPUT_5].DisplayHandler               = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_INPUT_5].OnOpeningHandler             = Input5_OpeningHandler;

    m_aMenuItems[MENU_INPUT_END].byMenuItemID               = MENU_INPUT_END;
    m_aMenuItems[MENU_INPUT_END].byChildMenuItems           = 5;
    m_aMenuItems[MENU_INPUT_END].pChildMenu                 = &m_aMenuItems[MENU_MAIN_INPUT];
    m_aMenuItems[MENU_INPUT_END].pParentMenu                = &m_aMenuItems[MENU_MAIN_INPUT];
    m_aMenuItems[MENU_INPUT_END].KeyboardHandler            = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_INPUT_END].DisplayHandler             = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_INPUT_END].OnOpeningHandler           = NULL;

    /* Init Charact Menu*/
    m_aMenuItems[MENU_CHARACT_l_1].byMenuItemID             = MENU_CHARACT_l_1;
    m_aMenuItems[MENU_CHARACT_l_1].byChildMenuItems         = 9;
    m_aMenuItems[MENU_CHARACT_l_1].pChildMenu               = NULL;
    m_aMenuItems[MENU_CHARACT_l_1].pParentMenu              = &m_aMenuItems[MENU_MAIN_CHARACT];
    m_aMenuItems[MENU_CHARACT_l_1].KeyboardHandler          = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_CHARACT_l_1].DisplayHandler           = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_CHARACT_l_1].OnOpeningHandler         = Charact11_OpeningHandler;

    m_aMenuItems[MENU_CHARACT_l_25].byMenuItemID            = MENU_CHARACT_l_25;
    m_aMenuItems[MENU_CHARACT_l_25].byChildMenuItems        = 9;
    m_aMenuItems[MENU_CHARACT_l_25].pChildMenu              = NULL;
    m_aMenuItems[MENU_CHARACT_l_25].pParentMenu             = &m_aMenuItems[MENU_MAIN_CHARACT];
    m_aMenuItems[MENU_CHARACT_l_25].KeyboardHandler         = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_CHARACT_l_25].DisplayHandler          = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_CHARACT_l_25].OnOpeningHandler        = Charact125_OpeningHandler;

    m_aMenuItems[MENU_CHARACT_l_33].byMenuItemID            = MENU_CHARACT_l_33;
    m_aMenuItems[MENU_CHARACT_l_33].byChildMenuItems        = 9;
    m_aMenuItems[MENU_CHARACT_l_33].pChildMenu              = NULL;
    m_aMenuItems[MENU_CHARACT_l_33].pParentMenu             = &m_aMenuItems[MENU_MAIN_CHARACT];
    m_aMenuItems[MENU_CHARACT_l_33].KeyboardHandler         = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_CHARACT_l_33].DisplayHandler          = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_CHARACT_l_33].OnOpeningHandler        = Charact133_OpeningHandler;

    m_aMenuItems[MENU_CHARACT_l_50].byMenuItemID            = MENU_CHARACT_l_50;
    m_aMenuItems[MENU_CHARACT_l_50].byChildMenuItems        = 9;
    m_aMenuItems[MENU_CHARACT_l_50].pChildMenu              = NULL;
    m_aMenuItems[MENU_CHARACT_l_50].pParentMenu             = &m_aMenuItems[MENU_MAIN_CHARACT];
    m_aMenuItems[MENU_CHARACT_l_50].KeyboardHandler         = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_CHARACT_l_50].DisplayHandler          = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_CHARACT_l_50].OnOpeningHandler        = Charact150_OpeningHandler;

    m_aMenuItems[MENU_CHARACT_25_1].byMenuItemID            = MENU_CHARACT_25_1;
    m_aMenuItems[MENU_CHARACT_25_1].byChildMenuItems        = 9;
    m_aMenuItems[MENU_CHARACT_25_1].pChildMenu              = NULL;
    m_aMenuItems[MENU_CHARACT_25_1].pParentMenu             = &m_aMenuItems[MENU_MAIN_CHARACT];
    m_aMenuItems[MENU_CHARACT_25_1].KeyboardHandler         = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_CHARACT_25_1].DisplayHandler          = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_CHARACT_25_1].OnOpeningHandler        = Charact251_OpeningHandler;

    m_aMenuItems[MENU_CHARACT_33_1].byMenuItemID            = MENU_CHARACT_33_1;
    m_aMenuItems[MENU_CHARACT_33_1].byChildMenuItems        = 9;
    m_aMenuItems[MENU_CHARACT_33_1].pChildMenu              = NULL;
    m_aMenuItems[MENU_CHARACT_33_1].pParentMenu             = &m_aMenuItems[MENU_MAIN_CHARACT];
    m_aMenuItems[MENU_CHARACT_33_1].KeyboardHandler         = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_CHARACT_33_1].DisplayHandler          = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_CHARACT_33_1].OnOpeningHandler        = Charact331_OpeningHandler;

    m_aMenuItems[MENU_CHARACT_50_1].byMenuItemID            = MENU_CHARACT_50_1;
    m_aMenuItems[MENU_CHARACT_50_1].byChildMenuItems        = 9;
    m_aMenuItems[MENU_CHARACT_50_1].pChildMenu              = NULL;
    m_aMenuItems[MENU_CHARACT_50_1].pParentMenu             = &m_aMenuItems[MENU_MAIN_CHARACT];
    m_aMenuItems[MENU_CHARACT_50_1].KeyboardHandler         = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_CHARACT_50_1].DisplayHandler          = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_CHARACT_50_1].OnOpeningHandler        = Charact501_OpeningHandler;

    m_aMenuItems[MENU_CHARACT_FREE].byMenuItemID            = MENU_CHARACT_FREE;
    m_aMenuItems[MENU_CHARACT_FREE].byChildMenuItems        = 9;
    m_aMenuItems[MENU_CHARACT_FREE].pChildMenu              = NULL;
    m_aMenuItems[MENU_CHARACT_FREE].pParentMenu             = &m_aMenuItems[MENU_MAIN_CHARACT];
    m_aMenuItems[MENU_CHARACT_FREE].KeyboardHandler         = CharactFree_KeyboardHandler;
    m_aMenuItems[MENU_CHARACT_FREE].DisplayHandler          = CharactFree_DisplayHandler;
    m_aMenuItems[MENU_CHARACT_FREE].OnOpeningHandler        = CharactFree_OpeningHandler;

    m_aMenuItems[MENU_CHARACT_END].byMenuItemID             = MENU_CHARACT_END;
    m_aMenuItems[MENU_CHARACT_END].byChildMenuItems         = 9;
    m_aMenuItems[MENU_CHARACT_END].pChildMenu               = &m_aMenuItems[MENU_MAIN_CHARACT];
    m_aMenuItems[MENU_CHARACT_END].pParentMenu              = &m_aMenuItems[MENU_MAIN_CHARACT];
    m_aMenuItems[MENU_CHARACT_END].KeyboardHandler          = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_CHARACT_END].DisplayHandler           = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_CHARACT_END].OnOpeningHandler         = NULL;

    /* Init Dir Cmd Menu*/
    m_aMenuItems[MENU_DIRCMD_RISE].byMenuItemID             = MENU_DIRCMD_RISE;
    m_aMenuItems[MENU_DIRCMD_RISE].byChildMenuItems         = 3;
    m_aMenuItems[MENU_DIRCMD_RISE].pChildMenu               = NULL;
    m_aMenuItems[MENU_DIRCMD_RISE].pParentMenu              = &m_aMenuItems[MENU_MAIN_DIRCMD];
    m_aMenuItems[MENU_DIRCMD_RISE].KeyboardHandler          = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_DIRCMD_RISE].DisplayHandler           = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_DIRCMD_RISE].OnOpeningHandler         = DircmdRise_OpeningHandler;

    m_aMenuItems[MENU_DIRCMD_FALL].byMenuItemID             = MENU_DIRCMD_FALL;
    m_aMenuItems[MENU_DIRCMD_FALL].byChildMenuItems         = 3;
    m_aMenuItems[MENU_DIRCMD_FALL].pChildMenu               = NULL;
    m_aMenuItems[MENU_DIRCMD_FALL].pParentMenu              = &m_aMenuItems[MENU_MAIN_DIRCMD];
    m_aMenuItems[MENU_DIRCMD_FALL].KeyboardHandler          = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_DIRCMD_FALL].DisplayHandler           = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_DIRCMD_FALL].OnOpeningHandler         = DircmdFall_OpeningHandler;

    m_aMenuItems[MENU_DIRCMD_END].byMenuItemID              = MENU_DIRCMD_END;
    m_aMenuItems[MENU_DIRCMD_END].byChildMenuItems          = 3;
    m_aMenuItems[MENU_DIRCMD_END].pChildMenu                = &m_aMenuItems[MENU_MAIN_DIRCMD];
    m_aMenuItems[MENU_DIRCMD_END].pParentMenu               = &m_aMenuItems[MENU_MAIN_DIRCMD];
    m_aMenuItems[MENU_DIRCMD_END].KeyboardHandler           = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_DIRCMD_END].DisplayHandler            = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_DIRCMD_END].OnOpeningHandler          = NULL;

    /* Init Dir Act Menu*/
    m_aMenuItems[MENU_DIRACT_RISE].byMenuItemID             = MENU_DIRACT_RISE;
    m_aMenuItems[MENU_DIRACT_RISE].byChildMenuItems         = 3;
    m_aMenuItems[MENU_DIRACT_RISE].pChildMenu               = NULL;
    m_aMenuItems[MENU_DIRACT_RISE].pParentMenu              = &m_aMenuItems[MENU_MAIN_DIRACT];
    m_aMenuItems[MENU_DIRACT_RISE].KeyboardHandler          = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_DIRACT_RISE].DisplayHandler           = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_DIRACT_RISE].OnOpeningHandler         = DiractRise_OpeningHandler;

    m_aMenuItems[MENU_DIRACT_FALL].byMenuItemID             = MENU_DIRACT_FALL;
    m_aMenuItems[MENU_DIRACT_FALL].byChildMenuItems         = 3;
    m_aMenuItems[MENU_DIRACT_FALL].pChildMenu               = NULL;
    m_aMenuItems[MENU_DIRACT_FALL].pParentMenu              = &m_aMenuItems[MENU_MAIN_DIRACT];
    m_aMenuItems[MENU_DIRACT_FALL].KeyboardHandler          = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_DIRACT_FALL].DisplayHandler           = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_DIRACT_FALL].OnOpeningHandler         = DiractFall_OpeningHandler;

    m_aMenuItems[MENU_DIRACT_END].byMenuItemID              = MENU_DIRACT_END;
    m_aMenuItems[MENU_DIRACT_END].byChildMenuItems          = 3;
    m_aMenuItems[MENU_DIRACT_END].pChildMenu                = &m_aMenuItems[MENU_MAIN_DIRACT];
    m_aMenuItems[MENU_DIRACT_END].pParentMenu               = &m_aMenuItems[MENU_MAIN_DIRACT];
    m_aMenuItems[MENU_DIRACT_END].KeyboardHandler           = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_DIRACT_END].DisplayHandler            = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_DIRACT_END].OnOpeningHandler          = NULL;

    /* X Contrl Menu*/
    m_aMenuItems[MENU_XCONTRL_DBMD].byMenuItemID            = MENU_XCONTRL_DBMD;
    m_aMenuItems[MENU_XCONTRL_DBMD].byChildMenuItems        = 3;
    m_aMenuItems[MENU_XCONTRL_DBMD].pChildMenu              = NULL;
    m_aMenuItems[MENU_XCONTRL_DBMD].pParentMenu             = &m_aMenuItems[MENU_MAIN_XCONTRL];
    m_aMenuItems[MENU_XCONTRL_DBMD].KeyboardHandler         = XcontrolDbnd_KeyboardHandler;
    m_aMenuItems[MENU_XCONTRL_DBMD].DisplayHandler          = XcontrolDbnd_DisplayHandler;
    m_aMenuItems[MENU_XCONTRL_DBMD].OnOpeningHandler        = XcontrolDbnd_OpeningHandler;

    m_aMenuItems[MENU_XCONTRL_PARA].byMenuItemID            = MENU_XCONTRL_PARA;
    m_aMenuItems[MENU_XCONTRL_PARA].byChildMenuItems        = 3;
    m_aMenuItems[MENU_XCONTRL_PARA].pChildMenu              = NULL;
    m_aMenuItems[MENU_XCONTRL_PARA].pParentMenu             = &m_aMenuItems[MENU_MAIN_XCONTRL];
    m_aMenuItems[MENU_XCONTRL_PARA].KeyboardHandler         = XcontrolPara_KeyboardHandler;
    m_aMenuItems[MENU_XCONTRL_PARA].DisplayHandler          = XcontrolPara_DisplayHandler;
    m_aMenuItems[MENU_XCONTRL_PARA].OnOpeningHandler        = XcontrolPara_OpeningHandler;

    m_aMenuItems[MENU_XCONTRL_END].byMenuItemID             = MENU_XCONTRL_END;
    m_aMenuItems[MENU_XCONTRL_END].byChildMenuItems         = 3;
    m_aMenuItems[MENU_XCONTRL_END].pChildMenu               = &m_aMenuItems[MENU_MAIN_XCONTRL];
    m_aMenuItems[MENU_XCONTRL_END].pParentMenu              = &m_aMenuItems[MENU_MAIN_XCONTRL];
    m_aMenuItems[MENU_XCONTRL_END].KeyboardHandler          = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_XCONTRL_END].DisplayHandler           = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_XCONTRL_END].OnOpeningHandler         = NULL;

    /* X Tune Menu*/
    m_aMenuItems[MENU_XTUNE_AUTO].byMenuItemID              = MENU_XTUNE_AUTO;
    m_aMenuItems[MENU_XTUNE_AUTO].byChildMenuItems          = 5;
    m_aMenuItems[MENU_XTUNE_AUTO].pChildMenu                = NULL;
    m_aMenuItems[MENU_XTUNE_AUTO].pParentMenu               = &m_aMenuItems[MENU_MAIN_XTUNE];
    m_aMenuItems[MENU_XTUNE_AUTO].KeyboardHandler           = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_XTUNE_AUTO].DisplayHandler            = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_XTUNE_AUTO].OnOpeningHandler          = XtuneAuto_OpeningHandler;

    m_aMenuItems[MENU_XTUNE_POS].byMenuItemID               = MENU_XTUNE_POS;
    m_aMenuItems[MENU_XTUNE_POS].byChildMenuItems           = 5;
    m_aMenuItems[MENU_XTUNE_POS].pChildMenu                 = NULL;
    m_aMenuItems[MENU_XTUNE_POS].pParentMenu                = &m_aMenuItems[MENU_MAIN_XTUNE];
    m_aMenuItems[MENU_XTUNE_POS].KeyboardHandler            = CaluserPos_KeyboardHandler;
    m_aMenuItems[MENU_XTUNE_POS].DisplayHandler             = CaluserPos_DisplayHandler;
    m_aMenuItems[MENU_XTUNE_POS].OnOpeningHandler           = CaluserPos_OpeningHandler;

    m_aMenuItems[MENU_XTUNE_PWM].byMenuItemID               = MENU_XTUNE_PWM;
    m_aMenuItems[MENU_XTUNE_PWM].byChildMenuItems           = 5;
    m_aMenuItems[MENU_XTUNE_PWM].pChildMenu                 = NULL;
    m_aMenuItems[MENU_XTUNE_PWM].pParentMenu                = &m_aMenuItems[MENU_MAIN_XTUNE];
    m_aMenuItems[MENU_XTUNE_PWM].KeyboardHandler            = XtunePwm_KeyboardHandler;
    m_aMenuItems[MENU_XTUNE_PWM].DisplayHandler             = XtunePwm_DisplayHandler;
    m_aMenuItems[MENU_XTUNE_PWM].OnOpeningHandler           = XtunePwm_OpeningHandler;

    m_aMenuItems[MENU_XTUNE_AIR].byMenuItemID               = MENU_XTUNE_AIR;
    m_aMenuItems[MENU_XTUNE_AIR].byChildMenuItems           = 5;
    m_aMenuItems[MENU_XTUNE_AIR].pChildMenu                 = NULL;
    m_aMenuItems[MENU_XTUNE_AIR].pParentMenu                = &m_aMenuItems[MENU_MAIN_XTUNE];
    m_aMenuItems[MENU_XTUNE_AIR].KeyboardHandler            = XtuneAir_KeyboardHandler;
    m_aMenuItems[MENU_XTUNE_AIR].DisplayHandler             = XtuneAir_DisplayHandler;
    m_aMenuItems[MENU_XTUNE_AIR].OnOpeningHandler           = XtuneAir_OpeningHandler;

    m_aMenuItems[MENU_XTUNE_END].byMenuItemID               = MENU_XTUNE_END;
    m_aMenuItems[MENU_XTUNE_END].byChildMenuItems           = 5;
    m_aMenuItems[MENU_XTUNE_END].pChildMenu                 = &m_aMenuItems[MENU_MAIN_XTUNE];
    m_aMenuItems[MENU_XTUNE_END].pParentMenu                = &m_aMenuItems[MENU_MAIN_XTUNE];
    m_aMenuItems[MENU_XTUNE_END].KeyboardHandler            = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_XTUNE_END].DisplayHandler             = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_XTUNE_END].OnOpeningHandler           = NULL;

    /* Sig Err Menu*/
    m_aMenuItems[MENU_SIGERR_FUNON].byMenuItemID            = MENU_SIGERR_FUNON;
    m_aMenuItems[MENU_SIGERR_FUNON].byChildMenuItems        = 4;
    m_aMenuItems[MENU_SIGERR_FUNON].pChildMenu              = NULL;
    m_aMenuItems[MENU_SIGERR_FUNON].pParentMenu             = &m_aMenuItems[MENU_MAIN_SIGERR];
    m_aMenuItems[MENU_SIGERR_FUNON].KeyboardHandler         = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_SIGERR_FUNON].DisplayHandler          = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_SIGERR_FUNON].OnOpeningHandler        = SigerrFunon_OpeningHandler;

    m_aMenuItems[MENU_SIGERR_POSON].byMenuItemID            = MENU_SIGERR_POSON;
    m_aMenuItems[MENU_SIGERR_POSON].byChildMenuItems        = 4;
    m_aMenuItems[MENU_SIGERR_POSON].pChildMenu              = NULL;
    m_aMenuItems[MENU_SIGERR_POSON].pParentMenu             = &m_aMenuItems[MENU_MAIN_SIGERR];
    m_aMenuItems[MENU_SIGERR_POSON].KeyboardHandler         = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_SIGERR_POSON].DisplayHandler          = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_SIGERR_POSON].OnOpeningHandler        = SigerrPoson_OpeningHandler;

    m_aMenuItems[MENU_SIGERR_OFF].byMenuItemID              = MENU_SIGERR_OFF;
    m_aMenuItems[MENU_SIGERR_OFF].byChildMenuItems          = 4;
    m_aMenuItems[MENU_SIGERR_OFF].pChildMenu                = NULL;
    m_aMenuItems[MENU_SIGERR_OFF].pParentMenu               = &m_aMenuItems[MENU_MAIN_SIGERR];
    m_aMenuItems[MENU_SIGERR_OFF].KeyboardHandler           = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_SIGERR_OFF].DisplayHandler            = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_SIGERR_OFF].OnOpeningHandler          = SigerrOff_OpeningHandler;

    m_aMenuItems[MENU_SIGERR_END].byMenuItemID              = MENU_SIGERR_END;
    m_aMenuItems[MENU_SIGERR_END].byChildMenuItems          = 4;
    m_aMenuItems[MENU_SIGERR_END].pChildMenu                = &m_aMenuItems[MENU_MAIN_SIGERR];
    m_aMenuItems[MENU_SIGERR_END].pParentMenu               = &m_aMenuItems[MENU_MAIN_SIGERR];
    m_aMenuItems[MENU_SIGERR_END].KeyboardHandler           = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_SIGERR_END].DisplayHandler            = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_SIGERR_END].OnOpeningHandler          = NULL; 

    /* Ser Io Menu*/
    m_aMenuItems[MENU_SERIO_9600].byMenuItemID              = MENU_SERIO_9600;
    m_aMenuItems[MENU_SERIO_9600].byChildMenuItems          = 7;
    m_aMenuItems[MENU_SERIO_9600].pChildMenu                = NULL;
    m_aMenuItems[MENU_SERIO_9600].pParentMenu               = &m_aMenuItems[MENU_MAIN_SERIO];
    m_aMenuItems[MENU_SERIO_9600].KeyboardHandler           = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_SERIO_9600].DisplayHandler            = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_SERIO_9600].OnOpeningHandler          = Serio9600_OpeningHandler;

    m_aMenuItems[MENU_SERIO_19200].byMenuItemID             = MENU_SERIO_19200;
    m_aMenuItems[MENU_SERIO_19200].byChildMenuItems         = 7;
    m_aMenuItems[MENU_SERIO_19200].pChildMenu               = NULL;
    m_aMenuItems[MENU_SERIO_19200].pParentMenu              = &m_aMenuItems[MENU_MAIN_SERIO];
    m_aMenuItems[MENU_SERIO_19200].KeyboardHandler          = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_SERIO_19200].DisplayHandler           = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_SERIO_19200].OnOpeningHandler         = Serio19200_OpeningHandler;

    m_aMenuItems[MENU_SERIO_38400].byMenuItemID             = MENU_SERIO_38400;
    m_aMenuItems[MENU_SERIO_38400].byChildMenuItems         = 7;
    m_aMenuItems[MENU_SERIO_38400].pChildMenu               = NULL;
    m_aMenuItems[MENU_SERIO_38400].pParentMenu              = &m_aMenuItems[MENU_MAIN_SERIO];
    m_aMenuItems[MENU_SERIO_38400].KeyboardHandler          = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_SERIO_38400].DisplayHandler           = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_SERIO_38400].OnOpeningHandler         = Serio38400_OpeningHandler;

    m_aMenuItems[MENU_SERIO_1200].byMenuItemID              = MENU_SERIO_1200;
    m_aMenuItems[MENU_SERIO_1200].byChildMenuItems          = 7;
    m_aMenuItems[MENU_SERIO_1200].pChildMenu                = NULL;
    m_aMenuItems[MENU_SERIO_1200].pParentMenu               = &m_aMenuItems[MENU_MAIN_SERIO];
    m_aMenuItems[MENU_SERIO_1200].KeyboardHandler           = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_SERIO_1200].DisplayHandler            = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_SERIO_1200].OnOpeningHandler          = Serio1200_OpeningHandler;

    m_aMenuItems[MENU_SERIO_2400].byMenuItemID              = MENU_SERIO_2400;
    m_aMenuItems[MENU_SERIO_2400].byChildMenuItems          = 7;
    m_aMenuItems[MENU_SERIO_2400].pChildMenu                = NULL;
    m_aMenuItems[MENU_SERIO_2400].pParentMenu               = &m_aMenuItems[MENU_MAIN_SERIO];
    m_aMenuItems[MENU_SERIO_2400].KeyboardHandler           = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_SERIO_2400].DisplayHandler            = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_SERIO_2400].OnOpeningHandler          = Serio2400_OpeningHandler;

    m_aMenuItems[MENU_SERIO_4800].byMenuItemID              = MENU_SERIO_4800;
    m_aMenuItems[MENU_SERIO_4800].byChildMenuItems          = 7;
    m_aMenuItems[MENU_SERIO_4800].pChildMenu                = NULL;
    m_aMenuItems[MENU_SERIO_4800].pParentMenu               = &m_aMenuItems[MENU_MAIN_SERIO];
    m_aMenuItems[MENU_SERIO_4800].KeyboardHandler           = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_SERIO_4800].DisplayHandler            = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_SERIO_4800].OnOpeningHandler          = Serio4800_OpeningHandler;

    m_aMenuItems[MENU_SERIO_END].byMenuItemID               = MENU_SERIO_END;
    m_aMenuItems[MENU_SERIO_END].byChildMenuItems           = 7;
    m_aMenuItems[MENU_SERIO_END].pChildMenu                 = &m_aMenuItems[MENU_MAIN_SERIO];
    m_aMenuItems[MENU_SERIO_END].pParentMenu                = &m_aMenuItems[MENU_MAIN_SERIO];
    m_aMenuItems[MENU_SERIO_END].KeyboardHandler            = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_SERIO_END].DisplayHandler             = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_SERIO_END].OnOpeningHandler           = NULL;

    /* Code Menu*/
    m_aMenuItems[MENU_CODE_KEY].byMenuItemID                = MENU_CODE_KEY;
    m_aMenuItems[MENU_CODE_KEY].byChildMenuItems            = 4;
    m_aMenuItems[MENU_CODE_KEY].pChildMenu                  = NULL;
    m_aMenuItems[MENU_CODE_KEY].pParentMenu                 = &m_aMenuItems[MENU_MAIN_CODE];
    m_aMenuItems[MENU_CODE_KEY].KeyboardHandler             = CodeKey_KeyboardHandler;
    m_aMenuItems[MENU_CODE_KEY].DisplayHandler              = CodeKey_DisplayHandler;
    m_aMenuItems[MENU_CODE_KEY].OnOpeningHandler            = CodeKey_OpeningHandler;

    m_aMenuItems[MENU_CODE_MENU].byMenuItemID               = MENU_CODE_MENU;
    m_aMenuItems[MENU_CODE_MENU].byChildMenuItems           = 4;
    m_aMenuItems[MENU_CODE_MENU].pChildMenu                 = NULL;
    m_aMenuItems[MENU_CODE_MENU].pParentMenu                = &m_aMenuItems[MENU_MAIN_CODE];
    m_aMenuItems[MENU_CODE_MENU].KeyboardHandler            = CodeMenu_KeyboardHandler;
    m_aMenuItems[MENU_CODE_MENU].DisplayHandler             = CodeMenu_DisplayHandler;
    m_aMenuItems[MENU_CODE_MENU].OnOpeningHandler           = CodeMenu_OpeningHandler;

    m_aMenuItems[MENU_CODE_GLOB].byMenuItemID               = MENU_CODE_GLOB;
    m_aMenuItems[MENU_CODE_GLOB].byChildMenuItems           = 4;
    m_aMenuItems[MENU_CODE_GLOB].pChildMenu                 = NULL;
    m_aMenuItems[MENU_CODE_GLOB].pParentMenu                = &m_aMenuItems[MENU_MAIN_CODE];
    m_aMenuItems[MENU_CODE_GLOB].KeyboardHandler            = CodeGlob_KeyboardHandler;
    m_aMenuItems[MENU_CODE_GLOB].DisplayHandler             = CodeGlob_DisplayHandler;
    m_aMenuItems[MENU_CODE_GLOB].OnOpeningHandler           = CodeGlob_OpeningHandler;

    m_aMenuItems[MENU_CODE_END].byMenuItemID                = MENU_CODE_END;
    m_aMenuItems[MENU_CODE_END].byChildMenuItems            = 4;
    m_aMenuItems[MENU_CODE_END].pChildMenu                  = &m_aMenuItems[MENU_MAIN_CODE];
    m_aMenuItems[MENU_CODE_END].pParentMenu                 = &m_aMenuItems[MENU_MAIN_CODE];
    m_aMenuItems[MENU_CODE_END].KeyboardHandler             = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_CODE_END].DisplayHandler              = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_CODE_END].OnOpeningHandler            = NULL; 

    /* Bin In Menu*/
    m_aMenuItems[MENU_BININ_SPOS].byMenuItemID              = MENU_BININ_SPOS;
    m_aMenuItems[MENU_BININ_SPOS].byChildMenuItems          = 3;
    m_aMenuItems[MENU_BININ_SPOS].pChildMenu                = &m_aMenuItems[MENU_SPOS_OPN];
    m_aMenuItems[MENU_BININ_SPOS].pParentMenu               = &m_aMenuItems[MENU_MAIN_BININ];
    m_aMenuItems[MENU_BININ_SPOS].KeyboardHandler           = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_BININ_SPOS].DisplayHandler            = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_BININ_SPOS].OnOpeningHandler          = NULL;

    m_aMenuItems[MENU_BININ_MA].byMenuItemID                = MENU_BININ_MA;
    m_aMenuItems[MENU_BININ_MA].byChildMenuItems            = 3;
    m_aMenuItems[MENU_BININ_MA].pChildMenu                  = &m_aMenuItems[MENU_MA_OPN];
    m_aMenuItems[MENU_BININ_MA].pParentMenu                 = &m_aMenuItems[MENU_MAIN_BININ];
    m_aMenuItems[MENU_BININ_MA].KeyboardHandler             = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_BININ_MA].DisplayHandler              = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_BININ_MA].OnOpeningHandler            = NULL;

    m_aMenuItems[MENU_BININ_END].byMenuItemID               = MENU_BININ_END;
    m_aMenuItems[MENU_BININ_END].byChildMenuItems           = 3;
    m_aMenuItems[MENU_BININ_END].pChildMenu                 = &m_aMenuItems[MENU_MAIN_BININ];
    m_aMenuItems[MENU_BININ_END].pParentMenu                = &m_aMenuItems[MENU_MAIN_BININ];
    m_aMenuItems[MENU_BININ_END].KeyboardHandler            = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_BININ_END].DisplayHandler             = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_BININ_END].OnOpeningHandler           = NULL;

    /* Output Menu*/
    m_aMenuItems[MENU_OUTPUT_ANL].byMenuItemID              = MENU_OUTPUT_ANL;
    m_aMenuItems[MENU_OUTPUT_ANL].byChildMenuItems          = 5;
    m_aMenuItems[MENU_OUTPUT_ANL].pChildMenu                = &m_aMenuItems[MENU_ANL_POS];
    m_aMenuItems[MENU_OUTPUT_ANL].pParentMenu               = &m_aMenuItems[MENU_MAIN_OUTPUT];
    m_aMenuItems[MENU_OUTPUT_ANL].KeyboardHandler           = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_OUTPUT_ANL].DisplayHandler            = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_OUTPUT_ANL].OnOpeningHandler          = NULL;

    m_aMenuItems[MENU_OUTPUT_ANLSET].byMenuItemID           = MENU_OUTPUT_ANLSET;
    m_aMenuItems[MENU_OUTPUT_ANLSET].byChildMenuItems       = 5;
    m_aMenuItems[MENU_OUTPUT_ANLSET].pChildMenu             = &m_aMenuItems[MENU_ANLSET_4];
    m_aMenuItems[MENU_OUTPUT_ANLSET].pParentMenu            = &m_aMenuItems[MENU_MAIN_OUTPUT];
    m_aMenuItems[MENU_OUTPUT_ANLSET].KeyboardHandler        = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_OUTPUT_ANLSET].DisplayHandler         = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_OUTPUT_ANLSET].OnOpeningHandler       = NULL;

    m_aMenuItems[MENU_OUTPUT_BIN1].byMenuItemID             = MENU_OUTPUT_BIN1;
    m_aMenuItems[MENU_OUTPUT_BIN1].byChildMenuItems         = 5;
    m_aMenuItems[MENU_OUTPUT_BIN1].pChildMenu               = &m_aMenuItems[MENU_BIN1_DRV];
    m_aMenuItems[MENU_OUTPUT_BIN1].pParentMenu              = &m_aMenuItems[MENU_MAIN_OUTPUT];
    m_aMenuItems[MENU_OUTPUT_BIN1].KeyboardHandler          = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_OUTPUT_BIN1].DisplayHandler           = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_OUTPUT_BIN1].OnOpeningHandler         = NULL;

    m_aMenuItems[MENU_OUTPUT_BIN2].byMenuItemID             = MENU_OUTPUT_BIN2;
    m_aMenuItems[MENU_OUTPUT_BIN2].byChildMenuItems         = 5;
    m_aMenuItems[MENU_OUTPUT_BIN2].pChildMenu               = &m_aMenuItems[MENU_BIN2_DRV];
    m_aMenuItems[MENU_OUTPUT_BIN2].pParentMenu              = &m_aMenuItems[MENU_MAIN_OUTPUT];
    m_aMenuItems[MENU_OUTPUT_BIN2].KeyboardHandler          = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_OUTPUT_BIN2].DisplayHandler           = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_OUTPUT_BIN2].OnOpeningHandler         = NULL;

    m_aMenuItems[MENU_OUTPUT_END].byMenuItemID              = MENU_OUTPUT_END;
    m_aMenuItems[MENU_OUTPUT_END].byChildMenuItems          = 5;
    m_aMenuItems[MENU_OUTPUT_END].pChildMenu                = &m_aMenuItems[MENU_MAIN_OUTPUT];
    m_aMenuItems[MENU_OUTPUT_END].pParentMenu               = &m_aMenuItems[MENU_MAIN_OUTPUT];
    m_aMenuItems[MENU_OUTPUT_END].KeyboardHandler           = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_OUTPUT_END].DisplayHandler            = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_OUTPUT_END].OnOpeningHandler          = NULL; 

    /* Cal User Menu*/
    m_aMenuItems[MENU_CALUSER_POS].byMenuItemID             = MENU_CALUSER_POS;
    m_aMenuItems[MENU_CALUSER_POS].byChildMenuItems         = 4;
    m_aMenuItems[MENU_CALUSER_POS].pChildMenu               = NULL;
    m_aMenuItems[MENU_CALUSER_POS].pParentMenu              = &m_aMenuItems[MENU_MAIN_CALUSER];
    m_aMenuItems[MENU_CALUSER_POS].KeyboardHandler          = CaluserPos_KeyboardHandler;
    m_aMenuItems[MENU_CALUSER_POS].DisplayHandler           = CaluserPos_DisplayHandler;
    m_aMenuItems[MENU_CALUSER_POS].OnOpeningHandler         = CaluserPos_OpeningHandler;

    m_aMenuItems[MENU_CALUSER_INP].byMenuItemID             = MENU_CALUSER_INP;
    m_aMenuItems[MENU_CALUSER_INP].byChildMenuItems         = 4;
    m_aMenuItems[MENU_CALUSER_INP].pChildMenu               = NULL;
    m_aMenuItems[MENU_CALUSER_INP].pParentMenu              = &m_aMenuItems[MENU_MAIN_CALUSER];
    m_aMenuItems[MENU_CALUSER_INP].KeyboardHandler          = CaluserInp_KeyboardHandler;
    m_aMenuItems[MENU_CALUSER_INP].DisplayHandler           = CaluserInp_DisplayHandler;
    m_aMenuItems[MENU_CALUSER_INP].OnOpeningHandler         = CaluserInp_OpeningHandler;

    m_aMenuItems[MENU_CALUSER_FACT].byMenuItemID            = MENU_CALUSER_FACT;
    m_aMenuItems[MENU_CALUSER_FACT].byChildMenuItems        = 4;
    m_aMenuItems[MENU_CALUSER_FACT].pChildMenu              = NULL;
    m_aMenuItems[MENU_CALUSER_FACT].pParentMenu             = &m_aMenuItems[MENU_MAIN_CALUSER];
    m_aMenuItems[MENU_CALUSER_FACT].KeyboardHandler         = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_CALUSER_FACT].DisplayHandler          = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_CALUSER_FACT].OnOpeningHandler        = CaluserFact_OpeningHandler;

    m_aMenuItems[MENU_CALUSER_END].byMenuItemID             = MENU_CALUSER_END;
    m_aMenuItems[MENU_CALUSER_END].byChildMenuItems         = 4;
    m_aMenuItems[MENU_CALUSER_END].pChildMenu               = &m_aMenuItems[MENU_MAIN_CALUSER];
    m_aMenuItems[MENU_CALUSER_END].pParentMenu              = &m_aMenuItems[MENU_MAIN_CALUSER];
    m_aMenuItems[MENU_CALUSER_END].KeyboardHandler          = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_CALUSER_END].DisplayHandler           = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_CALUSER_END].OnOpeningHandler         = NULL; 

    /* P Contrl Menu*/
    m_aMenuItems[MENU_PCONTRL_DBND].byMenuItemID            = MENU_PCONTRL_DBND;
    m_aMenuItems[MENU_PCONTRL_DBND].byChildMenuItems        = 10;
    m_aMenuItems[MENU_PCONTRL_DBND].pChildMenu              = NULL;
    m_aMenuItems[MENU_PCONTRL_DBND].pParentMenu             = &m_aMenuItems[MENU_MAIN_PCONTRL];
    m_aMenuItems[MENU_PCONTRL_DBND].KeyboardHandler         = PcontrlDbnd_KeyboardHandler;
    m_aMenuItems[MENU_PCONTRL_DBND].DisplayHandler          = PcontrlDbnd_DisplayHandler;
    m_aMenuItems[MENU_PCONTRL_DBND].OnOpeningHandler        = PcontrlDbnd_OpeningHandler;

    m_aMenuItems[MENU_PCONTRL_PARA].byMenuItemID            = MENU_PCONTRL_PARA;
    m_aMenuItems[MENU_PCONTRL_PARA].byChildMenuItems        = 10;
    m_aMenuItems[MENU_PCONTRL_PARA].pChildMenu              = NULL;
    m_aMenuItems[MENU_PCONTRL_PARA].pParentMenu             = &m_aMenuItems[MENU_MAIN_PCONTRL];
    m_aMenuItems[MENU_PCONTRL_PARA].KeyboardHandler         = PcontrlPara_KeyboardHandler;
    m_aMenuItems[MENU_PCONTRL_PARA].DisplayHandler          = PcontrlPara_DisplayHandler;
    m_aMenuItems[MENU_PCONTRL_PARA].OnOpeningHandler        = PcontrlPara_OpeningHandler;

    m_aMenuItems[MENU_PCONTRL_SETP].byMenuItemID            = MENU_PCONTRL_SETP;
    m_aMenuItems[MENU_PCONTRL_SETP].byChildMenuItems        = 10;
    m_aMenuItems[MENU_PCONTRL_SETP].pChildMenu              = &m_aMenuItems[MENU_SETP_INT];
    m_aMenuItems[MENU_PCONTRL_SETP].pParentMenu             = &m_aMenuItems[MENU_MAIN_PCONTRL];
    m_aMenuItems[MENU_PCONTRL_SETP].KeyboardHandler         = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_PCONTRL_SETP].DisplayHandler          = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_PCONTRL_SETP].OnOpeningHandler        = NULL;

    m_aMenuItems[MENU_PCONTRL_INP].byMenuItemID             = MENU_PCONTRL_INP;
    m_aMenuItems[MENU_PCONTRL_INP].byChildMenuItems         = 10;
    m_aMenuItems[MENU_PCONTRL_INP].pChildMenu               = &m_aMenuItems[MENU_INP_FREQ];
    m_aMenuItems[MENU_PCONTRL_INP].pParentMenu              = &m_aMenuItems[MENU_MAIN_PCONTRL];
    m_aMenuItems[MENU_PCONTRL_INP].KeyboardHandler          = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_PCONTRL_INP].DisplayHandler           = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_PCONTRL_INP].OnOpeningHandler         = NULL;

    m_aMenuItems[MENU_PCONTRL_FILT].byMenuItemID            = MENU_PCONTRL_FILT;
    m_aMenuItems[MENU_PCONTRL_FILT].byChildMenuItems        = 10;
    m_aMenuItems[MENU_PCONTRL_FILT].pChildMenu              = NULL;
    m_aMenuItems[MENU_PCONTRL_FILT].pParentMenu             = &m_aMenuItems[MENU_MAIN_PCONTRL];
    m_aMenuItems[MENU_PCONTRL_FILT].KeyboardHandler         = PcontrlFilt_KeyboardHandler;
    m_aMenuItems[MENU_PCONTRL_FILT].DisplayHandler          = PcontrlFilt_DisplayHandler;
    m_aMenuItems[MENU_PCONTRL_FILT].OnOpeningHandler        = PcontrlFilt_OpeningHandler;

    m_aMenuItems[MENU_PCONTRL_SCAL].byMenuItemID            = MENU_PCONTRL_SCAL;
    m_aMenuItems[MENU_PCONTRL_SCAL].byChildMenuItems        = 10;
    m_aMenuItems[MENU_PCONTRL_SCAL].pChildMenu              = &m_aMenuItems[MENU_SCAL_END];
    m_aMenuItems[MENU_PCONTRL_SCAL].pParentMenu             = &m_aMenuItems[MENU_MAIN_PCONTRL];
    m_aMenuItems[MENU_PCONTRL_SCAL].KeyboardHandler         = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_PCONTRL_SCAL].DisplayHandler          = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_PCONTRL_SCAL].OnOpeningHandler        = NULL;

    m_aMenuItems[MENU_PCONTRL_TUNE].byMenuItemID            = MENU_PCONTRL_TUNE;
    m_aMenuItems[MENU_PCONTRL_TUNE].byChildMenuItems        = 10;
    m_aMenuItems[MENU_PCONTRL_TUNE].pChildMenu              = &m_aMenuItems[MENU_TUNE_NOT];
    m_aMenuItems[MENU_PCONTRL_TUNE].pParentMenu             = &m_aMenuItems[MENU_MAIN_PCONTRL];
    m_aMenuItems[MENU_PCONTRL_TUNE].KeyboardHandler         = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_PCONTRL_TUNE].DisplayHandler          = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_PCONTRL_TUNE].OnOpeningHandler        = NULL;

    m_aMenuItems[MENU_PCONTRL_KV].byMenuItemID              = MENU_PCONTRL_KV;
    m_aMenuItems[MENU_PCONTRL_KV].byChildMenuItems          = 10;
    m_aMenuItems[MENU_PCONTRL_KV].pChildMenu                = NULL;
    m_aMenuItems[MENU_PCONTRL_KV].pParentMenu               = &m_aMenuItems[MENU_MAIN_PCONTRL];
    m_aMenuItems[MENU_PCONTRL_KV].KeyboardHandler           = PcontrlKv_KeyboardHandler;
    m_aMenuItems[MENU_PCONTRL_KV].DisplayHandler            = PcontrlKv_DisplayHandler;
    m_aMenuItems[MENU_PCONTRL_KV].OnOpeningHandler          = PcontrlKv_OpeningHandler;

    m_aMenuItems[MENU_PCONTRL_LEAK].byMenuItemID            = MENU_PCONTRL_LEAK;
    m_aMenuItems[MENU_PCONTRL_LEAK].byChildMenuItems        = 10;
    m_aMenuItems[MENU_PCONTRL_LEAK].pChildMenu              = &m_aMenuItems[MENU_LEAK_NOT];
    m_aMenuItems[MENU_PCONTRL_LEAK].pParentMenu             = &m_aMenuItems[MENU_MAIN_PCONTRL];
    m_aMenuItems[MENU_PCONTRL_LEAK].KeyboardHandler         = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_PCONTRL_LEAK].DisplayHandler          = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_PCONTRL_LEAK].OnOpeningHandler        = NULL;

    m_aMenuItems[MENU_PCONTRL_END].byMenuItemID             = MENU_PCONTRL_END;
    m_aMenuItems[MENU_PCONTRL_END].byChildMenuItems         = 10;
    m_aMenuItems[MENU_PCONTRL_END].pChildMenu               = &m_aMenuItems[MENU_MAIN_PCONTRL];
    m_aMenuItems[MENU_PCONTRL_END].pParentMenu              = &m_aMenuItems[MENU_MAIN_PCONTRL];
    m_aMenuItems[MENU_PCONTRL_END].KeyboardHandler          = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_PCONTRL_END].DisplayHandler           = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_PCONTRL_END].OnOpeningHandler         = NULL; 

    /* S Pos In Menu*/
    m_aMenuItems[MENU_SPOS_OPN].byMenuItemID                = MENU_SPOS_OPN;
    m_aMenuItems[MENU_SPOS_OPN].byChildMenuItems            = 3;
    m_aMenuItems[MENU_SPOS_OPN].pChildMenu                  = NULL;
    m_aMenuItems[MENU_SPOS_OPN].pParentMenu                 = &m_aMenuItems[MENU_BININ_SPOS];
    m_aMenuItems[MENU_SPOS_OPN].KeyboardHandler             = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_SPOS_OPN].DisplayHandler              = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_SPOS_OPN].OnOpeningHandler            = PortD_OpeningHandler;

    m_aMenuItems[MENU_SPOS_CLS].byMenuItemID                = MENU_SPOS_CLS;
    m_aMenuItems[MENU_SPOS_CLS].byChildMenuItems            = 3;
    m_aMenuItems[MENU_SPOS_CLS].pChildMenu                  = NULL;
    m_aMenuItems[MENU_SPOS_CLS].pParentMenu                 = &m_aMenuItems[MENU_BININ_SPOS];
    m_aMenuItems[MENU_SPOS_CLS].KeyboardHandler             = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_SPOS_CLS].DisplayHandler              = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_SPOS_CLS].OnOpeningHandler            = PortD_OpeningHandler;

    m_aMenuItems[MENU_SPOS_END].byMenuItemID                = MENU_SPOS_END;
    m_aMenuItems[MENU_SPOS_END].byChildMenuItems            = 3;
    m_aMenuItems[MENU_SPOS_END].pChildMenu                  = &m_aMenuItems[MENU_BININ_SPOS];
    m_aMenuItems[MENU_SPOS_END].pParentMenu                 = &m_aMenuItems[MENU_BININ_SPOS];
    m_aMenuItems[MENU_SPOS_END].KeyboardHandler             = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_SPOS_END].DisplayHandler              = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_SPOS_END].OnOpeningHandler            = NULL;

    /* M A Menu*/
    m_aMenuItems[MENU_MA_OPN].byMenuItemID                  = MENU_MA_OPN;
    m_aMenuItems[MENU_MA_OPN].byChildMenuItems              = 3;
    m_aMenuItems[MENU_MA_OPN].pChildMenu                    = NULL;
    m_aMenuItems[MENU_MA_OPN].pParentMenu                   = &m_aMenuItems[MENU_BININ_MA];
    m_aMenuItems[MENU_MA_OPN].KeyboardHandler               = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_MA_OPN].DisplayHandler                = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_MA_OPN].OnOpeningHandler              = PortD_OpeningHandler;

    m_aMenuItems[MENU_MA_CLS].byMenuItemID                  = MENU_MA_CLS;
    m_aMenuItems[MENU_MA_CLS].byChildMenuItems              = 3;
    m_aMenuItems[MENU_MA_CLS].pChildMenu                    = NULL;
    m_aMenuItems[MENU_MA_CLS].pParentMenu                   = &m_aMenuItems[MENU_BININ_MA];
    m_aMenuItems[MENU_MA_CLS].KeyboardHandler               = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_MA_CLS].DisplayHandler                = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_MA_CLS].OnOpeningHandler              = PortD_OpeningHandler;

    m_aMenuItems[MENU_MA_END].byMenuItemID                  = MENU_MA_END;
    m_aMenuItems[MENU_MA_END].byChildMenuItems              = 3;
    m_aMenuItems[MENU_MA_END].pChildMenu                    = &m_aMenuItems[MENU_BININ_MA];
    m_aMenuItems[MENU_MA_END].pParentMenu                   = &m_aMenuItems[MENU_BININ_MA];
    m_aMenuItems[MENU_MA_END].KeyboardHandler               = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_MA_END].DisplayHandler                = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_MA_END].OnOpeningHandler              = NULL;

    /* Anl Menu*/
    m_aMenuItems[MENU_ANL_POS].byMenuItemID                 = MENU_ANL_POS;
    m_aMenuItems[MENU_ANL_POS].byChildMenuItems             = 7;
    m_aMenuItems[MENU_ANL_POS].pChildMenu                   = NULL;
    m_aMenuItems[MENU_ANL_POS].pParentMenu                  = &m_aMenuItems[MENU_OUTPUT_ANL];
    m_aMenuItems[MENU_ANL_POS].KeyboardHandler              = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_ANL_POS].DisplayHandler               = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_ANL_POS].OnOpeningHandler             = AnlPos_OpeningHandler;

    m_aMenuItems[MENU_ANL_CMD].byMenuItemID                 = MENU_ANL_CMD;
    m_aMenuItems[MENU_ANL_CMD].byChildMenuItems             = 7;
    m_aMenuItems[MENU_ANL_CMD].pChildMenu                   = NULL;
    m_aMenuItems[MENU_ANL_CMD].pParentMenu                  = &m_aMenuItems[MENU_OUTPUT_ANL];
    m_aMenuItems[MENU_ANL_CMD].KeyboardHandler              = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_ANL_CMD].DisplayHandler               = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_ANL_CMD].OnOpeningHandler             = AnlCmd_OpeningHandler;

    m_aMenuItems[MENU_ANL_SP].byMenuItemID                  = MENU_ANL_SP;
    m_aMenuItems[MENU_ANL_SP].byChildMenuItems              = 7;
    m_aMenuItems[MENU_ANL_SP].pChildMenu                    = NULL;
    m_aMenuItems[MENU_ANL_SP].pParentMenu                   = &m_aMenuItems[MENU_OUTPUT_ANL];
    m_aMenuItems[MENU_ANL_SP].KeyboardHandler               = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_ANL_SP].DisplayHandler                = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_ANL_SP].OnOpeningHandler              = AnlSp_OpeningHandler;

    m_aMenuItems[MENU_ANL_P1].byMenuItemID                  = MENU_ANL_P1;
    m_aMenuItems[MENU_ANL_P1].byChildMenuItems              = 7;
    m_aMenuItems[MENU_ANL_P1].pChildMenu                    = NULL;
    m_aMenuItems[MENU_ANL_P1].pParentMenu                   = &m_aMenuItems[MENU_OUTPUT_ANL];
    m_aMenuItems[MENU_ANL_P1].KeyboardHandler               = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_ANL_P1].DisplayHandler                = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_ANL_P1].OnOpeningHandler              = AnlP1_OpeningHandler;

    m_aMenuItems[MENU_ANL_P2].byMenuItemID                  = MENU_ANL_P2;
    m_aMenuItems[MENU_ANL_P2].byChildMenuItems              = 7;
    m_aMenuItems[MENU_ANL_P2].pChildMenu                    = NULL;
    m_aMenuItems[MENU_ANL_P2].pParentMenu                   = &m_aMenuItems[MENU_OUTPUT_ANL];
    m_aMenuItems[MENU_ANL_P2].KeyboardHandler               = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_ANL_P2].DisplayHandler                = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_ANL_P2].OnOpeningHandler              = AnlP2_OpeningHandler;

    m_aMenuItems[MENU_ANL_T].byMenuItemID                   = MENU_ANL_T;
    m_aMenuItems[MENU_ANL_T].byChildMenuItems               = 7;
    m_aMenuItems[MENU_ANL_T].pChildMenu                     = NULL;
    m_aMenuItems[MENU_ANL_T].pParentMenu                    = &m_aMenuItems[MENU_OUTPUT_ANL];
    m_aMenuItems[MENU_ANL_T].KeyboardHandler                = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_ANL_T].DisplayHandler                 = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_ANL_T].OnOpeningHandler               = AnlT_OpeningHandler;

    m_aMenuItems[MENU_ANL_END].byMenuItemID                 = MENU_ANL_END;
    m_aMenuItems[MENU_ANL_END].byChildMenuItems             = 7;
    m_aMenuItems[MENU_ANL_END].pChildMenu                   = &m_aMenuItems[MENU_OUTPUT_ANL];
    m_aMenuItems[MENU_ANL_END].pParentMenu                  = &m_aMenuItems[MENU_OUTPUT_ANL];
    m_aMenuItems[MENU_ANL_END].KeyboardHandler              = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_ANL_END].DisplayHandler               = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_ANL_END].OnOpeningHandler             = NULL;

    /* Anl Set Menu*/
    m_aMenuItems[MENU_ANLSET_4].byMenuItemID                = MENU_ANLSET_4;
    m_aMenuItems[MENU_ANLSET_4].byChildMenuItems            = 5;
    m_aMenuItems[MENU_ANLSET_4].pChildMenu                  = NULL;
    m_aMenuItems[MENU_ANLSET_4].pParentMenu                 = &m_aMenuItems[MENU_OUTPUT_ANLSET];
    m_aMenuItems[MENU_ANLSET_4].KeyboardHandler             = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_ANLSET_4].DisplayHandler              = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_ANLSET_4].OnOpeningHandler            = AnlSet4_OpeningHandler;

    m_aMenuItems[MENU_ANLSET_0].byMenuItemID                = MENU_ANLSET_0;
    m_aMenuItems[MENU_ANLSET_0].byChildMenuItems            = 5;
    m_aMenuItems[MENU_ANLSET_0].pChildMenu                  = NULL;
    m_aMenuItems[MENU_ANLSET_0].pParentMenu                 = &m_aMenuItems[MENU_OUTPUT_ANLSET];
    m_aMenuItems[MENU_ANLSET_0].KeyboardHandler             = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_ANLSET_0].DisplayHandler              = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_ANLSET_0].OnOpeningHandler            = AnlSet0_OpeningHandler;

    m_aMenuItems[MENU_ANLSET_10].byMenuItemID               = MENU_ANLSET_10;
    m_aMenuItems[MENU_ANLSET_10].byChildMenuItems           = 5;
    m_aMenuItems[MENU_ANLSET_10].pChildMenu                 = NULL;
    m_aMenuItems[MENU_ANLSET_10].pParentMenu                = &m_aMenuItems[MENU_OUTPUT_ANLSET];
    m_aMenuItems[MENU_ANLSET_10].KeyboardHandler            = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_ANLSET_10].DisplayHandler             = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_ANLSET_10].OnOpeningHandler           = AnlSet10_OpeningHandler;

    m_aMenuItems[MENU_ANLSET_5].byMenuItemID                = MENU_ANLSET_5;
    m_aMenuItems[MENU_ANLSET_5].byChildMenuItems            = 5;
    m_aMenuItems[MENU_ANLSET_5].pChildMenu                  = NULL;
    m_aMenuItems[MENU_ANLSET_5].pParentMenu                 = &m_aMenuItems[MENU_OUTPUT_ANLSET];
    m_aMenuItems[MENU_ANLSET_5].KeyboardHandler             = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_ANLSET_5].DisplayHandler              = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_ANLSET_5].OnOpeningHandler            = AnlSet5_OpeningHandler;

    m_aMenuItems[MENU_ANLSET_END].byMenuItemID              = MENU_ANLSET_END;
    m_aMenuItems[MENU_ANLSET_END].byChildMenuItems          = 5;
    m_aMenuItems[MENU_ANLSET_END].pChildMenu                = &m_aMenuItems[MENU_OUTPUT_ANLSET];
    m_aMenuItems[MENU_ANLSET_END].pParentMenu               = &m_aMenuItems[MENU_OUTPUT_ANLSET];
    m_aMenuItems[MENU_ANLSET_END].KeyboardHandler           = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_ANLSET_END].DisplayHandler            = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_ANLSET_END].OnOpeningHandler          = NULL;

    /* Bin1 Menu*/
    m_aMenuItems[MENU_BIN1_DRV].byMenuItemID                = MENU_BIN1_DRV;
    m_aMenuItems[MENU_BIN1_DRV].byChildMenuItems            = 10;
    m_aMenuItems[MENU_BIN1_DRV].pChildMenu                  = NULL;
    m_aMenuItems[MENU_BIN1_DRV].pParentMenu                 = &m_aMenuItems[MENU_OUTPUT_BIN1];
    m_aMenuItems[MENU_BIN1_DRV].KeyboardHandler             = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_BIN1_DRV].DisplayHandler              = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_BIN1_DRV].OnOpeningHandler            = Bin1Drv_OpeningHandler;

    m_aMenuItems[MENU_BIN1_LIM].byMenuItemID                = MENU_BIN1_LIM;
    m_aMenuItems[MENU_BIN1_LIM].byChildMenuItems            = 10;
    m_aMenuItems[MENU_BIN1_LIM].pChildMenu                  = NULL;
    m_aMenuItems[MENU_BIN1_LIM].pParentMenu                 = &m_aMenuItems[MENU_OUTPUT_BIN1];
    m_aMenuItems[MENU_BIN1_LIM].KeyboardHandler             = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_BIN1_LIM].DisplayHandler              = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_BIN1_LIM].OnOpeningHandler            = Bin1Lim_OpeningHandler;

    m_aMenuItems[MENU_BIN1_SPOS].byMenuItemID               = MENU_BIN1_SPOS;
    m_aMenuItems[MENU_BIN1_SPOS].byChildMenuItems           = 10;
    m_aMenuItems[MENU_BIN1_SPOS].pChildMenu                 = NULL;
    m_aMenuItems[MENU_BIN1_SPOS].pParentMenu                = &m_aMenuItems[MENU_OUTPUT_BIN1];
    m_aMenuItems[MENU_BIN1_SPOS].KeyboardHandler            = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_BIN1_SPOS].DisplayHandler             = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_BIN1_SPOS].OnOpeningHandler           = Bin1Spos_OpeningHandler;

    m_aMenuItems[MENU_BIN1_ERRS].byMenuItemID               = MENU_BIN1_ERRS;
    m_aMenuItems[MENU_BIN1_ERRS].byChildMenuItems           = 10;
    m_aMenuItems[MENU_BIN1_ERRS].pChildMenu                 = NULL;
    m_aMenuItems[MENU_BIN1_ERRS].pParentMenu                = &m_aMenuItems[MENU_OUTPUT_BIN1];
    m_aMenuItems[MENU_BIN1_ERRS].KeyboardHandler            = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_BIN1_ERRS].DisplayHandler             = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_BIN1_ERRS].OnOpeningHandler           = Bin1ErrS_OpeningHandler;

    m_aMenuItems[MENU_BIN1_ERR1].byMenuItemID               = MENU_BIN1_ERR1;
    m_aMenuItems[MENU_BIN1_ERR1].byChildMenuItems           = 10;
    m_aMenuItems[MENU_BIN1_ERR1].pChildMenu                 = NULL;
    m_aMenuItems[MENU_BIN1_ERR1].pParentMenu                = &m_aMenuItems[MENU_OUTPUT_BIN1];
    m_aMenuItems[MENU_BIN1_ERR1].KeyboardHandler            = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_BIN1_ERR1].DisplayHandler             = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_BIN1_ERR1].OnOpeningHandler           = Bin1Err1_OpeningHandler;

    m_aMenuItems[MENU_BIN1_ERR2].byMenuItemID               = MENU_BIN1_ERR2;
    m_aMenuItems[MENU_BIN1_ERR2].byChildMenuItems           = 10;
    m_aMenuItems[MENU_BIN1_ERR2].pChildMenu                 = NULL;
    m_aMenuItems[MENU_BIN1_ERR2].pParentMenu                = &m_aMenuItems[MENU_OUTPUT_BIN1];
    m_aMenuItems[MENU_BIN1_ERR2].KeyboardHandler            = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_BIN1_ERR2].DisplayHandler             = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_BIN1_ERR2].OnOpeningHandler           = Bin1Err2_OpeningHandler;

    m_aMenuItems[MENU_BIN1_ERRT].byMenuItemID               = MENU_BIN1_ERRT;
    m_aMenuItems[MENU_BIN1_ERRT].byChildMenuItems           = 10;
    m_aMenuItems[MENU_BIN1_ERRT].pChildMenu                 = NULL;
    m_aMenuItems[MENU_BIN1_ERRT].pParentMenu                = &m_aMenuItems[MENU_OUTPUT_BIN1];
    m_aMenuItems[MENU_BIN1_ERRT].KeyboardHandler            = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_BIN1_ERRT].DisplayHandler             = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_BIN1_ERRT].OnOpeningHandler           = Bin1ErrT_OpeningHandler;

    m_aMenuItems[MENU_BIN1_OPN].byMenuItemID                = MENU_BIN1_OPN;
    m_aMenuItems[MENU_BIN1_OPN].byChildMenuItems            = 10;
    m_aMenuItems[MENU_BIN1_OPN].pChildMenu                  = NULL;
    m_aMenuItems[MENU_BIN1_OPN].pParentMenu                 = &m_aMenuItems[MENU_OUTPUT_BIN1];
    m_aMenuItems[MENU_BIN1_OPN].KeyboardHandler             = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_BIN1_OPN].DisplayHandler              = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_BIN1_OPN].OnOpeningHandler            = Bin1Opn_OpeningHandler;

    m_aMenuItems[MENU_BIN1_CLS].byMenuItemID                = MENU_BIN1_CLS;
    m_aMenuItems[MENU_BIN1_CLS].byChildMenuItems            = 10;
    m_aMenuItems[MENU_BIN1_CLS].pChildMenu                  = NULL;
    m_aMenuItems[MENU_BIN1_CLS].pParentMenu                 = &m_aMenuItems[MENU_OUTPUT_BIN1];
    m_aMenuItems[MENU_BIN1_CLS].KeyboardHandler             = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_BIN1_CLS].DisplayHandler              = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_BIN1_CLS].OnOpeningHandler            = Bin1Cls_OpeningHandler;

    m_aMenuItems[MENU_BIN1_END].byMenuItemID                = MENU_BIN1_END;
    m_aMenuItems[MENU_BIN1_END].byChildMenuItems            = 10;
    m_aMenuItems[MENU_BIN1_END].pChildMenu                  = &m_aMenuItems[MENU_OUTPUT_BIN1];
    m_aMenuItems[MENU_BIN1_END].pParentMenu                 = &m_aMenuItems[MENU_OUTPUT_BIN1];
    m_aMenuItems[MENU_BIN1_END].KeyboardHandler             = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_BIN1_END].DisplayHandler              = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_BIN1_END].OnOpeningHandler            = NULL;

    /* Bin2 Menu*/
    m_aMenuItems[MENU_BIN2_DRV].byMenuItemID                = MENU_BIN2_DRV;
    m_aMenuItems[MENU_BIN2_DRV].byChildMenuItems            = 10;
    m_aMenuItems[MENU_BIN2_DRV].pChildMenu                  = NULL;
    m_aMenuItems[MENU_BIN2_DRV].pParentMenu                 = &m_aMenuItems[MENU_OUTPUT_BIN2];
    m_aMenuItems[MENU_BIN2_DRV].KeyboardHandler             = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_BIN2_DRV].DisplayHandler              = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_BIN2_DRV].OnOpeningHandler            = Bin2Drv_OpeningHandler;

    m_aMenuItems[MENU_BIN2_LIM].byMenuItemID                = MENU_BIN2_LIM;
    m_aMenuItems[MENU_BIN2_LIM].byChildMenuItems            = 10;
    m_aMenuItems[MENU_BIN2_LIM].pChildMenu                  = NULL;
    m_aMenuItems[MENU_BIN2_LIM].pParentMenu                 = &m_aMenuItems[MENU_OUTPUT_BIN2];
    m_aMenuItems[MENU_BIN2_LIM].KeyboardHandler             = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_BIN2_LIM].DisplayHandler              = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_BIN2_LIM].OnOpeningHandler            = Bin2Lim_OpeningHandler;

    m_aMenuItems[MENU_BIN2_SPOS].byMenuItemID               = MENU_BIN2_SPOS;
    m_aMenuItems[MENU_BIN2_SPOS].byChildMenuItems           = 10;
    m_aMenuItems[MENU_BIN2_SPOS].pChildMenu                 = NULL;
    m_aMenuItems[MENU_BIN2_SPOS].pParentMenu                = &m_aMenuItems[MENU_OUTPUT_BIN2];
    m_aMenuItems[MENU_BIN2_SPOS].KeyboardHandler            = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_BIN2_SPOS].DisplayHandler             = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_BIN2_SPOS].OnOpeningHandler           = Bin2Spos_OpeningHandler;

    m_aMenuItems[MENU_BIN2_ERRS].byMenuItemID               = MENU_BIN2_ERRS;
    m_aMenuItems[MENU_BIN2_ERRS].byChildMenuItems           = 10;
    m_aMenuItems[MENU_BIN2_ERRS].pChildMenu                 = NULL;
    m_aMenuItems[MENU_BIN2_ERRS].pParentMenu                = &m_aMenuItems[MENU_OUTPUT_BIN2];
    m_aMenuItems[MENU_BIN2_ERRS].KeyboardHandler            = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_BIN2_ERRS].DisplayHandler             = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_BIN2_ERRS].OnOpeningHandler           = Bin2ErrS_OpeningHandler;

    m_aMenuItems[MENU_BIN2_ERR1].byMenuItemID               = MENU_BIN2_ERR1;
    m_aMenuItems[MENU_BIN2_ERR1].byChildMenuItems           = 10;
    m_aMenuItems[MENU_BIN2_ERR1].pChildMenu                 = NULL;
    m_aMenuItems[MENU_BIN2_ERR1].pParentMenu                = &m_aMenuItems[MENU_OUTPUT_BIN2];
    m_aMenuItems[MENU_BIN2_ERR1].KeyboardHandler            = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_BIN2_ERR1].DisplayHandler             = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_BIN2_ERR1].OnOpeningHandler           = Bin2Err1_OpeningHandler;

    m_aMenuItems[MENU_BIN2_ERR2].byMenuItemID               = MENU_BIN2_ERR2;
    m_aMenuItems[MENU_BIN2_ERR2].byChildMenuItems           = 10;
    m_aMenuItems[MENU_BIN2_ERR2].pChildMenu                 = NULL;
    m_aMenuItems[MENU_BIN2_ERR2].pParentMenu                = &m_aMenuItems[MENU_OUTPUT_BIN2];
    m_aMenuItems[MENU_BIN2_ERR2].KeyboardHandler            = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_BIN2_ERR2].DisplayHandler             = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_BIN2_ERR2].OnOpeningHandler           = Bin2Err2_OpeningHandler;

    m_aMenuItems[MENU_BIN2_ERRT].byMenuItemID               = MENU_BIN2_ERRT;
    m_aMenuItems[MENU_BIN2_ERRT].byChildMenuItems           = 10;
    m_aMenuItems[MENU_BIN2_ERRT].pChildMenu                 = NULL;
    m_aMenuItems[MENU_BIN2_ERRT].pParentMenu                = &m_aMenuItems[MENU_OUTPUT_BIN2];
    m_aMenuItems[MENU_BIN2_ERRT].KeyboardHandler            = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_BIN2_ERRT].DisplayHandler             = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_BIN2_ERRT].OnOpeningHandler           = Bin2ErrT_OpeningHandler;

    m_aMenuItems[MENU_BIN2_OPN].byMenuItemID                = MENU_BIN2_OPN;
    m_aMenuItems[MENU_BIN2_OPN].byChildMenuItems            = 10;
    m_aMenuItems[MENU_BIN2_OPN].pChildMenu                  = NULL;
    m_aMenuItems[MENU_BIN2_OPN].pParentMenu                 = &m_aMenuItems[MENU_OUTPUT_BIN2];
    m_aMenuItems[MENU_BIN2_OPN].KeyboardHandler             = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_BIN2_OPN].DisplayHandler              = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_BIN2_OPN].OnOpeningHandler            = Bin2Opn_OpeningHandler;

    m_aMenuItems[MENU_BIN2_CLS].byMenuItemID                = MENU_BIN2_CLS;
    m_aMenuItems[MENU_BIN2_CLS].byChildMenuItems            = 10;
    m_aMenuItems[MENU_BIN2_CLS].pChildMenu                  = NULL;
    m_aMenuItems[MENU_BIN2_CLS].pParentMenu                 = &m_aMenuItems[MENU_OUTPUT_BIN2];
    m_aMenuItems[MENU_BIN2_CLS].KeyboardHandler             = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_BIN2_CLS].DisplayHandler              = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_BIN2_CLS].OnOpeningHandler            = Bin2Cls_OpeningHandler;

    m_aMenuItems[MENU_BIN2_END].byMenuItemID                = MENU_BIN2_END;
    m_aMenuItems[MENU_BIN2_END].byChildMenuItems            = 10;
    m_aMenuItems[MENU_BIN2_END].pChildMenu                  = &m_aMenuItems[MENU_OUTPUT_BIN2];
    m_aMenuItems[MENU_BIN2_END].pParentMenu                 = &m_aMenuItems[MENU_OUTPUT_BIN2];
    m_aMenuItems[MENU_BIN2_END].KeyboardHandler             = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_BIN2_END].DisplayHandler              = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_BIN2_END].OnOpeningHandler            = NULL;

    /* Setp Menu*/
    m_aMenuItems[MENU_SETP_INT].byMenuItemID                = MENU_SETP_INT;
    m_aMenuItems[MENU_SETP_INT].byChildMenuItems            = 3;
    m_aMenuItems[MENU_SETP_INT].pChildMenu                  = NULL;
    m_aMenuItems[MENU_SETP_INT].pParentMenu                 = &m_aMenuItems[MENU_PCONTRL_SETP];
    m_aMenuItems[MENU_SETP_INT].KeyboardHandler             = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_SETP_INT].DisplayHandler              = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_SETP_INT].OnOpeningHandler            = ActSngl_OpeningHandler;

    m_aMenuItems[MENU_SETP_EXT].byMenuItemID                = MENU_SETP_EXT;
    m_aMenuItems[MENU_SETP_EXT].byChildMenuItems            = 3;
    m_aMenuItems[MENU_SETP_EXT].pChildMenu                  = NULL;
    m_aMenuItems[MENU_SETP_EXT].pParentMenu                 = &m_aMenuItems[MENU_PCONTRL_SETP];
    m_aMenuItems[MENU_SETP_EXT].KeyboardHandler             = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_SETP_EXT].DisplayHandler              = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_SETP_EXT].OnOpeningHandler            = ActSngl_OpeningHandler;

    m_aMenuItems[MENU_SETP_END].byMenuItemID                = MENU_SETP_END;
    m_aMenuItems[MENU_SETP_END].byChildMenuItems            = 3;
    m_aMenuItems[MENU_SETP_END].pChildMenu                  = &m_aMenuItems[MENU_PCONTRL_SETP];
    m_aMenuItems[MENU_SETP_END].pParentMenu                 = &m_aMenuItems[MENU_PCONTRL_SETP];
    m_aMenuItems[MENU_SETP_END].KeyboardHandler             = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_SETP_END].DisplayHandler              = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_SETP_END].OnOpeningHandler            = NULL;

    /* Inp Menu*/
    m_aMenuItems[MENU_INP_FREQ].byMenuItemID                = MENU_INP_FREQ;
    m_aMenuItems[MENU_INP_FREQ].byChildMenuItems            = 5;
    m_aMenuItems[MENU_INP_FREQ].pChildMenu                  = NULL;
    m_aMenuItems[MENU_INP_FREQ].pParentMenu                 = &m_aMenuItems[MENU_PCONTRL_INP];
    m_aMenuItems[MENU_INP_FREQ].KeyboardHandler             = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_INP_FREQ].DisplayHandler              = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_INP_FREQ].OnOpeningHandler            = ActSngl_OpeningHandler;

    m_aMenuItems[MENU_INP_PT100].byMenuItemID               = MENU_INP_PT100;
    m_aMenuItems[MENU_INP_PT100].byChildMenuItems           = 5;
    m_aMenuItems[MENU_INP_PT100].pChildMenu                 = NULL;
    m_aMenuItems[MENU_INP_PT100].pParentMenu                = &m_aMenuItems[MENU_PCONTRL_INP];
    m_aMenuItems[MENU_INP_PT100].KeyboardHandler            = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_INP_PT100].DisplayHandler             = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_INP_PT100].OnOpeningHandler           = ActSngl_OpeningHandler;

    m_aMenuItems[MENU_INP_P1P2].byMenuItemID                = MENU_INP_P1P2;
    m_aMenuItems[MENU_INP_P1P2].byChildMenuItems            = 5;
    m_aMenuItems[MENU_INP_P1P2].pChildMenu                  = NULL;
    m_aMenuItems[MENU_INP_P1P2].pParentMenu                 = &m_aMenuItems[MENU_PCONTRL_INP];
    m_aMenuItems[MENU_INP_P1P2].KeyboardHandler             = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_INP_P1P2].DisplayHandler              = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_INP_P1P2].OnOpeningHandler            = ActSngl_OpeningHandler;

    m_aMenuItems[MENU_INP_20MA].byMenuItemID                = MENU_INP_20MA;
    m_aMenuItems[MENU_INP_20MA].byChildMenuItems            = 5;
    m_aMenuItems[MENU_INP_20MA].pChildMenu                  = NULL;
    m_aMenuItems[MENU_INP_20MA].pParentMenu                 = &m_aMenuItems[MENU_PCONTRL_INP];
    m_aMenuItems[MENU_INP_20MA].KeyboardHandler             = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_INP_20MA].DisplayHandler              = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_INP_20MA].OnOpeningHandler            = ActSngl_OpeningHandler;

    m_aMenuItems[MENU_INP_END].byMenuItemID                 = MENU_INP_END;
    m_aMenuItems[MENU_INP_END].byChildMenuItems             = 5;
    m_aMenuItems[MENU_INP_END].pChildMenu                   = &m_aMenuItems[MENU_PCONTRL_INP];
    m_aMenuItems[MENU_INP_END].pParentMenu                  = &m_aMenuItems[MENU_PCONTRL_INP];
    m_aMenuItems[MENU_INP_END].KeyboardHandler              = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_INP_END].DisplayHandler               = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_INP_END].OnOpeningHandler             = NULL;

    /* Inp Menu*/
    m_aMenuItems[MENU_SCAL_S].byMenuItemID                  = MENU_SCAL_S;
    m_aMenuItems[MENU_SCAL_S].byChildMenuItems              = 5;
    m_aMenuItems[MENU_SCAL_S].pChildMenu                    = NULL;
    m_aMenuItems[MENU_SCAL_S].pParentMenu                   = &m_aMenuItems[MENU_PCONTRL_SCAL];
    m_aMenuItems[MENU_SCAL_S].KeyboardHandler               = ScalS_KeyboardHandler;
    m_aMenuItems[MENU_SCAL_S].DisplayHandler                = ScalS_DisplayHandler;
    m_aMenuItems[MENU_SCAL_S].OnOpeningHandler              = ScalS_OpeningHandler;

    m_aMenuItems[MENU_SCAL_F].byMenuItemID                  = MENU_SCAL_F;
    m_aMenuItems[MENU_SCAL_F].byChildMenuItems              = 5;
    m_aMenuItems[MENU_SCAL_F].pChildMenu                    = NULL;
    m_aMenuItems[MENU_SCAL_F].pParentMenu                   = &m_aMenuItems[MENU_PCONTRL_SCAL];
    m_aMenuItems[MENU_SCAL_F].KeyboardHandler               = ScalF_KeyboardHandler;
    m_aMenuItems[MENU_SCAL_F].DisplayHandler                = ScalF_DisplayHandler;
    m_aMenuItems[MENU_SCAL_F].OnOpeningHandler              = ScalF_OpeningHandler;

    m_aMenuItems[MENU_SCAL_T].byMenuItemID                  = MENU_SCAL_T;
    m_aMenuItems[MENU_SCAL_T].byChildMenuItems              = 5;
    m_aMenuItems[MENU_SCAL_T].pChildMenu                    = NULL;
    m_aMenuItems[MENU_SCAL_T].pParentMenu                   = &m_aMenuItems[MENU_PCONTRL_SCAL];
    m_aMenuItems[MENU_SCAL_T].KeyboardHandler               = ScalT_KeyboardHandler;
    m_aMenuItems[MENU_SCAL_T].DisplayHandler                = ScalT_DisplayHandler;
    m_aMenuItems[MENU_SCAL_T].OnOpeningHandler              = ScalT_OpeningHandler;

    m_aMenuItems[MENU_SCAL_P].byMenuItemID                  = MENU_SCAL_P;
    m_aMenuItems[MENU_SCAL_P].byChildMenuItems              = 5;
    m_aMenuItems[MENU_SCAL_P].pChildMenu                    = NULL;
    m_aMenuItems[MENU_SCAL_P].pParentMenu                   = &m_aMenuItems[MENU_PCONTRL_SCAL];
    m_aMenuItems[MENU_SCAL_P].KeyboardHandler               = ScalP_KeyboardHandler;
    m_aMenuItems[MENU_SCAL_P].DisplayHandler                = ScalP_DisplayHandler;
    m_aMenuItems[MENU_SCAL_P].OnOpeningHandler              = ScalP_OpeningHandler;

    m_aMenuItems[MENU_SCAL_END].byMenuItemID                = MENU_SCAL_END;
    m_aMenuItems[MENU_SCAL_END].byChildMenuItems            = 5;
    m_aMenuItems[MENU_SCAL_END].pChildMenu                  = &m_aMenuItems[MENU_PCONTRL_SCAL];
    m_aMenuItems[MENU_SCAL_END].pParentMenu                 = &m_aMenuItems[MENU_PCONTRL_SCAL];
    m_aMenuItems[MENU_SCAL_END].KeyboardHandler             = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_SCAL_END].DisplayHandler              = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_SCAL_END].OnOpeningHandler            = NULL;

    /* Tune Menu*/
    m_aMenuItems[MENU_TUNE_NOT].byMenuItemID                = MENU_TUNE_NOT;
    m_aMenuItems[MENU_TUNE_NOT].byChildMenuItems            = 7;
    m_aMenuItems[MENU_TUNE_NOT].pChildMenu                  = NULL;
    m_aMenuItems[MENU_TUNE_NOT].pParentMenu                 = &m_aMenuItems[MENU_PCONTRL_TUNE];
    m_aMenuItems[MENU_TUNE_NOT].KeyboardHandler             = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_TUNE_NOT].DisplayHandler              = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_TUNE_NOT].OnOpeningHandler            = TuneNot_OpeningHandler;

    m_aMenuItems[MENU_TUNE_DEF].byMenuItemID                = MENU_TUNE_DEF;
    m_aMenuItems[MENU_TUNE_DEF].byChildMenuItems            = 7;
    m_aMenuItems[MENU_TUNE_DEF].pChildMenu                  = NULL;
    m_aMenuItems[MENU_TUNE_DEF].pParentMenu                 = &m_aMenuItems[MENU_PCONTRL_TUNE];
    m_aMenuItems[MENU_TUNE_DEF].KeyboardHandler             = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_TUNE_DEF].DisplayHandler              = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_TUNE_DEF].OnOpeningHandler            = TuneDef_OpeningHandler;

    m_aMenuItems[MENU_TUNE_FLOW].byMenuItemID               = MENU_TUNE_FLOW;
    m_aMenuItems[MENU_TUNE_FLOW].byChildMenuItems           = 7;
    m_aMenuItems[MENU_TUNE_FLOW].pChildMenu                 = NULL;
    m_aMenuItems[MENU_TUNE_FLOW].pParentMenu                = &m_aMenuItems[MENU_PCONTRL_TUNE];
    m_aMenuItems[MENU_TUNE_FLOW].KeyboardHandler            = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_TUNE_FLOW].DisplayHandler             = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_TUNE_FLOW].OnOpeningHandler           = TuneFlow_OpeningHandler;

    m_aMenuItems[MENU_TUNE_TEMP].byMenuItemID               = MENU_TUNE_TEMP;
    m_aMenuItems[MENU_TUNE_TEMP].byChildMenuItems           = 7;
    m_aMenuItems[MENU_TUNE_TEMP].pChildMenu                 = NULL;
    m_aMenuItems[MENU_TUNE_TEMP].pParentMenu                = &m_aMenuItems[MENU_PCONTRL_TUNE];
    m_aMenuItems[MENU_TUNE_TEMP].KeyboardHandler            = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_TUNE_TEMP].DisplayHandler             = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_TUNE_TEMP].OnOpeningHandler           = TuneTemp_OpeningHandler;

    m_aMenuItems[MENU_TUNE_PRES].byMenuItemID               = MENU_TUNE_PRES;
    m_aMenuItems[MENU_TUNE_PRES].byChildMenuItems           = 7;
    m_aMenuItems[MENU_TUNE_PRES].pChildMenu                 = NULL;
    m_aMenuItems[MENU_TUNE_PRES].pParentMenu                = &m_aMenuItems[MENU_PCONTRL_TUNE];
    m_aMenuItems[MENU_TUNE_PRES].KeyboardHandler            = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_TUNE_PRES].DisplayHandler             = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_TUNE_PRES].OnOpeningHandler           = TunePres_OpeningHandler;

    m_aMenuItems[MENU_TUNE_LEVL].byMenuItemID               = MENU_TUNE_LEVL;
    m_aMenuItems[MENU_TUNE_LEVL].byChildMenuItems           = 7;
    m_aMenuItems[MENU_TUNE_LEVL].pChildMenu                 = NULL;
    m_aMenuItems[MENU_TUNE_LEVL].pParentMenu                = &m_aMenuItems[MENU_PCONTRL_TUNE];
    m_aMenuItems[MENU_TUNE_LEVL].KeyboardHandler            = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_TUNE_LEVL].DisplayHandler             = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_TUNE_LEVL].OnOpeningHandler           = TuneLevl_OpeningHandler;

    m_aMenuItems[MENU_TUNE_END].byMenuItemID                = MENU_TUNE_END;
    m_aMenuItems[MENU_TUNE_END].byChildMenuItems            = 7;
    m_aMenuItems[MENU_TUNE_END].pChildMenu                  = &m_aMenuItems[MENU_PCONTRL_TUNE];
    m_aMenuItems[MENU_TUNE_END].pParentMenu                 = &m_aMenuItems[MENU_PCONTRL_TUNE];
    m_aMenuItems[MENU_TUNE_END].KeyboardHandler             = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_TUNE_END].DisplayHandler              = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_TUNE_END].OnOpeningHandler            = NULL;

    /* Leak Menu*/
    m_aMenuItems[MENU_LEAK_NOT].byMenuItemID                = MENU_LEAK_NOT;
    m_aMenuItems[MENU_LEAK_NOT].byChildMenuItems            = 4;
    m_aMenuItems[MENU_LEAK_NOT].pChildMenu                  = NULL;
    m_aMenuItems[MENU_LEAK_NOT].pParentMenu                 = &m_aMenuItems[MENU_PCONTRL_LEAK];
    m_aMenuItems[MENU_LEAK_NOT].KeyboardHandler             = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_LEAK_NOT].DisplayHandler              = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_LEAK_NOT].OnOpeningHandler            = LeakNot_OpeningHandler;

    m_aMenuItems[MENU_LEAK_MEAS].byMenuItemID               = MENU_LEAK_MEAS;
    m_aMenuItems[MENU_LEAK_MEAS].byChildMenuItems           = 4;
    m_aMenuItems[MENU_LEAK_MEAS].pChildMenu                 = NULL;
    m_aMenuItems[MENU_LEAK_MEAS].pParentMenu                = &m_aMenuItems[MENU_PCONTRL_LEAK];
    m_aMenuItems[MENU_LEAK_MEAS].KeyboardHandler            = LeakMeas_KeyboardHandler;
    m_aMenuItems[MENU_LEAK_MEAS].DisplayHandler             = LeakMeas_DisplayHandler;
    m_aMenuItems[MENU_LEAK_MEAS].OnOpeningHandler           = LeakMeas_OpeningHandler;

    m_aMenuItems[MENU_LEAK_CHAR].byMenuItemID               = MENU_LEAK_CHAR;
    m_aMenuItems[MENU_LEAK_CHAR].byChildMenuItems           = 4;
    m_aMenuItems[MENU_LEAK_CHAR].pChildMenu                 = NULL;
    m_aMenuItems[MENU_LEAK_CHAR].pParentMenu                = &m_aMenuItems[MENU_PCONTRL_LEAK];
    m_aMenuItems[MENU_LEAK_CHAR].KeyboardHandler            = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_LEAK_CHAR].DisplayHandler             = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_LEAK_CHAR].OnOpeningHandler           = LeakChar_OpeningHandler;

    m_aMenuItems[MENU_LEAK_END].byMenuItemID                = MENU_LEAK_END;
    m_aMenuItems[MENU_LEAK_END].byChildMenuItems            = 4;
    m_aMenuItems[MENU_LEAK_END].pChildMenu                  = &m_aMenuItems[MENU_PCONTRL_LEAK];
    m_aMenuItems[MENU_LEAK_END].pParentMenu                 = &m_aMenuItems[MENU_PCONTRL_LEAK];
    m_aMenuItems[MENU_LEAK_END].KeyboardHandler             = MainMenu_KeyboardHandler;
    m_aMenuItems[MENU_LEAK_END].DisplayHandler              = MainMenu_DisplayHandler;
    m_aMenuItems[MENU_LEAK_END].OnOpeningHandler            = NULL;

    m_mcbCurrent.pMenu                                      = NULL;
    m_mcbCurrent.byStartMenuItemID                          = 0;

    menu_update();

    S_INIT(m_stackMenuCtlBlock, &m_mcbCurrent, 1);

    clearLCD();
}

void HMI_Handler()
{
	CheckKeyboard();
    Display();
}
