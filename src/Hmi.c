/************************************************************************************************

 Author        : Yu

 Date          : 

 File          : hmi
                                      
 Hardware      : ADuC702x

 Description   : hmi
*************************************************************************************************/
#include <aduc7024.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "Consts.h"

#include "Stack.h"

#include "HT1622.h"
#include "AppTypes.h"
														  	
typedef unsigned char           BYTE;       /* Prefix: by	*/
typedef unsigned short          U16;       /* Prefix: by	*/
typedef unsigned int	        WORD;       /* Prefix: w	*/

// KEY0 : PORT3.1
// KEY1 : PORT3.2
// KEY2 : PORT3.3

#define KEYS					3
#define IS_KEY0_DOWN()			((GP3DAT & BIT01) == 0)
#define IS_KEY1_DOWN()			((GP3DAT & BIT02) == 0)
#define IS_KEY2_DOWN()			((GP3DAT & BIT03) == 0)

//#define IS_KEY0_DOWN()			((GP3DAT & BIT03) == 0)
//#define IS_KEY1_DOWN()			((GP0DAT & BIT03) == 0)
//#define IS_KEY2_DOWN()			((GP0DAT & BIT04) == 0)

#define MENU_ITEMS				162
#define KB_BUF_SIZE				63

const BYTE display[][8] =	{
							"ACT[FUNC",
							"[[[INPUT",
							"[[IN[SET",
							"[[[X[SET",
							"[[SYSTEM",
							"[[CONTRL",
							"[[MANUAL",
							"[[[[[END",
							"FUNCSNGL",
							"FUNCDOUB",
							"[ACT[END",
							"[[4[20MA",
							"[[0[20MA",
							"[[[0[10V",
							"[[[[0[5V",
							"INPUTEND",
							"[[CHRACT",
							"[[CUTOFF",
							"[DIR[CMD",
							"[DIR[ACT",
							"[SPLTRNG",
							"INSETEND",
							"[[X[TUNE",
							"[X[LIMIT",
							"[[X[TIME",
							"X[CONTRL",
							"[XSETEND",
							"[[SER[IO",
							"[SAFEPOS",
							"[[SIGERR",
							"[[[[CODE",
							"[SETFACT",
							"[SYS[END",
							"[[BIN[IN",
							"[[OUTPUT",
							"CAL[USER",
							"P[CONTRL",
							"CTRL[END",
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
							"BIN1[OFF",
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
							"BIN2[OFF",
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

extern UNIT_DATA                g_UnitData;

DEFINE_STACK(MENU_CTL_BLOCK, BYTE, STACK_MCB)

MENU_ITEM						m_aMenuItems[MENU_ITEMS];
MENU_CTL_BLOCK					m_mcbCurrent;
STACK_MCB						m_stackMenuCtlBlock;

// Indicate which page to be displayed
static BYTE						m_byPageNo					= 0;
static BYTE						m_byCursorPos				= 0;
static BYTE						m_byCursorPage				= 0;

static BYTE						m_bufKeyboard[KB_BUF_SIZE];

enum
{
	KEY_1,
	KEY_2,
	KEY_3
};

enum
{
	// Main Menu
	MENU_MAIN_ACT,
	MENU_MAIN_INPUT,
	MENU_MAIN_INSET,
	MENU_MAIN_XSET,
	MENU_MAIN_SYSTEM,
	MENU_MAIN_CONTRL,
	MENU_MAIN_MANUAL,
	MENU_MAIN_END,
	// Act Menu
	MENU_ACT_SNGL,
	MENU_ACT_DOUB,
	MENU_ACT_END,
	// Input Menu
	MENU_INPUT_4,
	MENU_INPUT_0,
	MENU_INPUT_10,
	MENU_INPUT_5,
	MENU_INPUT_END,
	// In Set Menu
	MENU_INSET_CHARACT,
	MENU_INSET_CUTOFF,
	MENU_INSET_DIRCMD,
	MENU_INSET_DIRACT,
	MENU_INSET_SPLTRNG,
	MENU_INSET_END,
	// Input Menu
	MENU_XSET_XTUNE,
	MENU_XSET_XLIMIT,
	MENU_XSET_XTIME,
	MENU_XSET_XCONTRL,
	MENU_XSET_END,
	// System Menu
	MENU_SYSTEM_SERIO,
	MENU_SYSTEM_SAFEPOS,
	MENU_SYSTEM_SIGERR,
	MENU_SYSTEM_CODE,
	MENU_SYSTEM_SETFACT,
	MENU_SYSTEM_END,
	// Contrl Menu
	MENU_CONTRL_BININ,
	MENU_CONTRL_OUTPUT,
	MENU_CONTRL_CALUSER,
	MENU_CONTRL_PCONTRL,
	MENU_CONTRL_END,
	// Charact Menu
	MENU_CHARACT_l_1,
	MENU_CHARACT_l_25,
	MENU_CHARACT_l_33,
	MENU_CHARACT_l_50,
	MENU_CHARACT_25_1,
	MENU_CHARACT_33_1,
	MENU_CHARACT_50_1,
	MENU_CHARACT_FREE,
	MENU_CHARACT_END,
	// Dir Cmd Menu
	MENU_DIRCMD_RISE,
	MENU_DIRCMD_FALL,
	MENU_DIRCMD_END,
	// Dir Act Menu
	MENU_DIRACT_RISE,
	MENU_DIRACT_FALL,
	MENU_DIRACT_END,
	// X Contrl Menu
	MENU_XCONTRL_END,
	MENU_XCONTRL_DBMD,
	MENU_XCONTRL_PARA,	
	// X Tune Menu
	MENU_XTUNE_AUTO,
	MENU_XTUNE_POS,
	MENU_XTUNE_PWM,
	MENU_XTUNE_AIR,
	MENU_XTUNE_END,
	// Sig Err Menu
	MENU_SIGERR_FUNON,
	MENU_SIGERR_POSON,
	MENU_SIGERR_OFF,
	MENU_SIGERR_END,
	// Ser Io Menu
	MENU_SERIO_9600,
	MENU_SERIO_19200,
	MENU_SERIO_38400,
	MENU_SERIO_1200,
	MENU_SERIO_2400,
	MENU_SERIO_4800,
	MENU_SERIO_END,
	// Code Menu
	MENU_CODE_END,
	MENU_CODE_KEY,
	MENU_CODE_MENU,
	MENU_CODE_GLOB,	
	// Bin in Menu
	MENU_BININ_SPOS,
	MENU_BININ_MA,
	MENU_BININ_END,
	// Output Menu
	MENU_OUTPUT_ANL,
	MENU_OUTPUT_ANLSET,
	MENU_OUTPUT_BIN1,
	MENU_OUTPUT_BIN2,
	MENU_OUTPUT_END,
	// Cal User Menu
	MENU_CALUSER_END,
	MENU_CALUSER_POS,
	MENU_CALUSER_INP,
	MENU_CALUSER_FACT,	
	// P Contrl Menu
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
	// S Pos in Menu
	MENU_SPOS_OPN,
	MENU_SPOS_CLS,
	MENU_SPOS_END,
	// M A Menu
	MENU_MA_OPN,
	MENU_MA_CLS,
	MENU_MA_END,
	// Anl Menu
	MENU_ANL_POS,
	MENU_ANL_CMD,
	MENU_ANL_SP,
	MENU_ANL_P1,
	MENU_ANL_P2,
	MENU_ANL_T,
	MENU_ANL_END,
	// Anl Set Menu
	MENU_ANLSET_4,
	MENU_ANLSET_0,
	MENU_ANLSET_10,
	MENU_ANLSET_5,
	MENU_ANLSET_END,
	// Bin1 Menu
	MENU_BIN1_DRV,
	MENU_BIN1_LIM,
	MENU_BIN1_SPOS,
	MENU_BIN1_ERRS,
	MENU_BIN1_ERR1,
	MENU_BIN1_ERR2,
	MENU_BIN1_ERRT,
	MENU_BIN1_OFF,
	MENU_BIN1_OPN,
	MENU_BIN1_CLS,
	MENU_BIN1_END,
	// Bin2 Menu
	MENU_BIN2_DRV,
	MENU_BIN2_LIM,
	MENU_BIN2_SPOS,
	MENU_BIN2_ERRS,
	MENU_BIN2_ERR1,
	MENU_BIN2_ERR2,
	MENU_BIN2_ERRT,
	MENU_BIN2_OFF,
	MENU_BIN2_OPN,
	MENU_BIN2_CLS,
	MENU_BIN2_END,
	// Setp Menu
	MENU_SETP_INT,
	MENU_SETP_EXT,
	MENU_SETP_END,
	// Inp Menu
	MENU_INP_FREQ,
	MENU_INP_PT100,
	MENU_INP_P1P2,
	MENU_INP_20MA,
	MENU_INP_END,
	// Scal Menu
	MENU_SCAL_END,
	MENU_SCAL_S,
	MENU_SCAL_F,
	MENU_SCAL_T,
	MENU_SCAL_P,
	// Tune Menu
	MENU_TUNE_NOT,
	MENU_TUNE_DEF,
	MENU_TUNE_FLOW,
	MENU_TUNE_TEMP,
	MENU_TUNE_PRES,
	MENU_TUNE_LEVL,
	MENU_TUNE_END,
	// Leak Menu
	MENU_LEAK_NOT,
	MENU_LEAK_MEAS,
	MENU_LEAK_CHAR,
	MENU_LEAK_END,
};

extern UNIT_DATA				g_UnitData;
extern UNIT_CFG					g_UnitCfg;

static void OnKeyDown(BYTE byKeyCode)
{
}

static void OnKeyUp(BYTE byKeyCode)
{
	if (m_mcbCurrent.pMenu == NULL)
	{
		if (byKeyCode == KEY_1)
		{
			m_byPageNo++;
			if (m_byPageNo > 3)
			{
				m_byPageNo = 0;
			}
		}
		else if (byKeyCode == KEY_3)
		{
			S_PUSH(m_stackMenuCtlBlock, m_mcbCurrent);

			m_mcbCurrent.pMenu						= &m_aMenuItems[MENU_MAIN_ACT];
			m_mcbCurrent.byStartMenuItemID			= m_mcbCurrent.pMenu->byMenuItemID;
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

static void Display()
{
	BYTE byNum[7];
	static float nVal;

	if (nVal > 99999)
		nVal = 0;
	nVal++;

	if (m_mcbCurrent.pMenu == NULL)
	{
		if (m_byPageNo == 0)
		{
			floattochar (g_UnitData.dat.fPos/10, byNum,0);
			display_digital(byNum,0,0);
			display_char("[[[[[POS");
		}
		if (m_byPageNo == 1)
		{
			floattochar (g_UnitData.dat.fCmd/10, byNum,0);
			display_digital(byNum,0,0);
			display_char("[[[[[CMD");
		}
		if (m_byPageNo == 2)
		{
			floattochar (g_UnitData.dat.fInp/10, byNum,0);
			display_digital(byNum,0,0);
			display_char("[[[[[INP");
		}
		if (m_byPageNo == 3)
		{
			floattochar (g_UnitData.dat.fTem, byNum,1);
			display_digital(byNum,1,0);
			display_char("[[[[TEMP");
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

static void CheckKeyboard()
{
	static BYTE _cnts[KEYS] = {0, 0, 0};

	BYTE	byKeyMask		= 0;
	BYTE	byBitMask;
	BYTE	i;

	if (IS_KEY0_DOWN())
		byKeyMask |= BIT00;
	if (IS_KEY1_DOWN())
		byKeyMask |= BIT01;
	if (IS_KEY2_DOWN())
		byKeyMask |= BIT02;

 	for(i = 0; i < KEYS; ++i)
	{
		byBitMask = (1 << i);
		if ((byKeyMask & byBitMask) != 0)
		{
			if (_cnts[i] >= 1)
			{
				OnKeyDown(i);
			}
			_cnts[i]++;
		}
		else
		{
			if (_cnts[i] >= 1)
			{
				OnKeyUp(i);
			}
			_cnts[i] = 0;
		}
	}
}

static void DisplayMenu()
{
	U16	byMenuID = m_mcbCurrent.byStartMenuItemID;

	display_digital("::::::",0,0);
	display_char((unsigned char *)display[byMenuID]);
}

static bool IsRetMenuItem(MENU_ITEM* pMenuItem)
{
	return (pMenuItem->byMenuItemID == MENU_MAIN_END);
}

extern void EepromWr_n( unsigned short *pcData );

static void MenuKeyboardHandler(BYTE byKeyCode)
{
	if ((byKeyCode == KEY_1) && 
		(m_aMenuItems[m_mcbCurrent.byStartMenuItemID].pParentMenu == m_aMenuItems[m_mcbCurrent.byStartMenuItemID - 1].pParentMenu))
	{
	  	m_mcbCurrent.byStartMenuItemID--;
		if(m_mcbCurrent.byStartMenuItemID > MENU_ITEMS - 1)
			m_mcbCurrent.byStartMenuItemID	= 0;
	}
	else if ((byKeyCode == KEY_2) && 
			(m_aMenuItems[m_mcbCurrent.byStartMenuItemID].pParentMenu == m_aMenuItems[m_mcbCurrent.byStartMenuItemID + 1].pParentMenu))
	{
	   	m_mcbCurrent.byStartMenuItemID++;
		if(m_mcbCurrent.byStartMenuItemID > MENU_ITEMS - 1)
			m_mcbCurrent.byStartMenuItemID	= MENU_ITEMS - 1;
	}
	else if (byKeyCode == KEY_3)
	{
		MENU_ITEM*	pMenuItem = &m_aMenuItems[m_mcbCurrent.byStartMenuItemID];

		if (IsRetMenuItem(pMenuItem))
		{
			if (!S_IS_EMPTY(m_stackMenuCtlBlock))
			{
				EepromWr_n(g_UnitCfg.buf);
				m_mcbCurrent.pMenu = NULL;
				S_POP(m_stackMenuCtlBlock);
			}
		}
		else if (pMenuItem->pChildMenu != NULL)
		{
			S_PUSH(m_stackMenuCtlBlock, m_mcbCurrent);
			m_mcbCurrent.pMenu 						= pMenuItem->pChildMenu;
			m_mcbCurrent.byStartMenuItemID			= pMenuItem->pChildMenu->byMenuItemID;
		}
		else
		{
			S_PUSH(m_stackMenuCtlBlock, m_mcbCurrent);
			m_mcbCurrent.pMenu 						= pMenuItem;
			m_mcbCurrent.byStartMenuItemID			= 0;

			if (pMenuItem->OnOpeningHandler != NULL)
			{
				pMenuItem->OnOpeningHandler();
			}
		}
	}
}

static void MainMenu_KeyboardHandler(BYTE byKeyCode)
{
	MenuKeyboardHandler(byKeyCode);
}

static void MainMenu_DisplayHandler()
{
	DisplayMenu();
}

static void MENU_MAIN_MANUAL_KeyboardHandler(BYTE byKeyCode)
{
	if (byKeyCode == KEY_1)
	{
		g_UnitData.dat.fPid -= 10;
		if (g_UnitData.dat.fPid < 0)
			g_UnitData.dat.fPid = 0;
	}
	else if (byKeyCode == KEY_2)
	{
		g_UnitData.dat.fPid += 10;
		if (g_UnitData.dat.fPid > 1000)
			g_UnitData.dat.fPid = 1000;
	}
	else if (byKeyCode == KEY_3)
	{
		MENU_ITEM*	pMenuItem = &m_aMenuItems[m_mcbCurrent.byStartMenuItemID];
   		//add some fun to due with m_bufKeyboard[m_byCursorPos],or do any action
		S_PUSH(m_stackMenuCtlBlock, m_mcbCurrent);
		m_mcbCurrent.pMenu 						= pMenuItem;
		m_mcbCurrent.byStartMenuItemID			= 0;
	}
}

static void MENU_MAIN_MANUAL_DisplayHandler()
{
	BYTE byNum[6];
		
	floattochar (g_UnitData.dat.fPos/10, byNum,0);
	display_digital(byNum,0,0);
	display_char("[[[[[POS");	
}

static void MENU_MAIN_MANUAL_OpeningHandler()
{
	g_UnitData.dat.fPid = 0; 	
}

static void ActSngl_OpeningHandler()
{
	clearLCD();
	display_char("DDDDDDDD");
	clearLCD();	
}

static void ActDoub_OpeningHandler()
{
	clearLCD();
	display_char("DDDDDDDD");
	clearLCD();	
}

static void Input4_OpeningHandler()
{
	clearLCD();
	g_UnitCfg.dat.byInp = 0;
	display_char("DDDDDDDD");
	clearLCD();	
}

static void Input0_OpeningHandler()
{
	clearLCD();
	g_UnitCfg.dat.byInp = 1;
	display_char("DDDDDDDD");
	clearLCD();	
}

static void Input10_OpeningHandler()
{
	clearLCD();
	g_UnitCfg.dat.byInp = 3;
	display_char("DDDDDDDD");
	clearLCD();	
}

static void Input5_OpeningHandler()
{
	clearLCD();
	g_UnitCfg.dat.byInp = 2;
	display_char("DDDDDDDD");
	clearLCD();	
}
static void InsetCutoff_KeyboardHandler(BYTE byKeyCode)
{
	if (byKeyCode == KEY_1)
	{
		if (m_byCursorPos > 0)
		{
			m_byCursorPos--;
			m_byCursorPage	 = m_byCursorPos/3;
		}
	}
	else if (byKeyCode == KEY_2)
	{
		m_byCursorPos++;
		m_byCursorPage	 = m_byCursorPos/3;
	}
	else if (byKeyCode == KEY_3)
	{
		if (m_byCursorPos < 6)
		{
			m_bufKeyboard[m_byCursorPos]++;
			if (m_bufKeyboard[m_byCursorPos] > '9')
				m_bufKeyboard[m_byCursorPos] = '0';
		}
		else
		{
			MENU_ITEM*	pMenuItem = &m_aMenuItems[m_mcbCurrent.byStartMenuItemID];

	   		g_UnitCfg.dat.byCutoffMin = m_bufKeyboard[2]*100 + m_bufKeyboard[1]*10 + m_bufKeyboard[0] - 5328;
			g_UnitCfg.dat.byCutoffMax = m_bufKeyboard[5]*100 + m_bufKeyboard[4]*10 + m_bufKeyboard[3] - 5328;

			S_PUSH(m_stackMenuCtlBlock, m_mcbCurrent);
			m_mcbCurrent.pMenu 						= pMenuItem;
			m_mcbCurrent.byStartMenuItemID			= 0;
		}
	}
}

static void InsetCutoff_DisplayHandler()
{
	BYTE byNum[6];
		
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
	for(uint16_t i = 0; i < 6; i++)
		m_bufKeyboard[i] = '0';
	m_byCursorPage	 = 0;
}

static void InsetSpltrng_KeyboardHandler(BYTE byKeyCode)
{
	if (byKeyCode == KEY_1)
	{
		if (m_byCursorPos > 0)
		{
			m_byCursorPos--;
			m_byCursorPage	 = m_byCursorPos/3;
		}
	}
	else if (byKeyCode == KEY_2)
	{
		m_byCursorPos++;
		m_byCursorPage	 = m_byCursorPos/3;
	}
	else if (byKeyCode == KEY_3)
	{
		if (m_byCursorPos < 6)
		{
			m_bufKeyboard[m_byCursorPos]++;
			if (m_bufKeyboard[m_byCursorPos] > '9')
				m_bufKeyboard[m_byCursorPos] = '0';
		}
		else
		{
			MENU_ITEM*	pMenuItem = &m_aMenuItems[m_mcbCurrent.byStartMenuItemID];
	   		
			g_UnitCfg.dat.bySrD = m_bufKeyboard[2]*100 + m_bufKeyboard[1]*10 + m_bufKeyboard[0] - 5328;
			g_UnitCfg.dat.bySrU = m_bufKeyboard[5]*100 + m_bufKeyboard[4]*10 + m_bufKeyboard[3] - 5328;

			S_PUSH(m_stackMenuCtlBlock, m_mcbCurrent);
			m_mcbCurrent.pMenu 						= pMenuItem;
			m_mcbCurrent.byStartMenuItemID			= 0;
		}
	}
}

static void InsetSpltrng_DisplayHandler()
{
	BYTE byNum[6];
		
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
	for(uint16_t i = 0; i < 6; i++)
		m_bufKeyboard[i] = '0';
	m_byCursorPage	 = 0;
}

static void XsetXlimit_KeyboardHandler(BYTE byKeyCode)
{
	if (byKeyCode == KEY_1)
	{
		if (m_byCursorPos > 0)
		{
			m_byCursorPos--;
			m_byCursorPage	 = m_byCursorPos/3;
		}
	}
	else if (byKeyCode == KEY_2)
	{
		m_byCursorPos++;
		m_byCursorPage	 = m_byCursorPos/3;
	}
	else if (byKeyCode == KEY_3)
	{
		if (m_byCursorPos < 6)
		{
			m_bufKeyboard[m_byCursorPos]++;
			if (m_bufKeyboard[m_byCursorPos] > '9')
				m_bufKeyboard[m_byCursorPos] = '0';
		}
		else
		{
			MENU_ITEM*	pMenuItem = &m_aMenuItems[m_mcbCurrent.byStartMenuItemID];
	   		
			g_UnitCfg.dat.byLimD = m_bufKeyboard[2]*100 + m_bufKeyboard[1]*10 + m_bufKeyboard[0] - 5328;
			g_UnitCfg.dat.byLimU = m_bufKeyboard[5]*100 + m_bufKeyboard[4]*10 + m_bufKeyboard[3] - 5328;

			S_PUSH(m_stackMenuCtlBlock, m_mcbCurrent);
			m_mcbCurrent.pMenu 						= pMenuItem;
			m_mcbCurrent.byStartMenuItemID			= 0;
		}
	}
}

static void XsetXlimit_DisplayHandler()
{
	BYTE byNum[6];
		
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
	for(uint16_t i = 0; i < 6; i++)
		m_bufKeyboard[i] = '0';
	m_byCursorPage	 = 0;
}

static void XsetXtime_KeyboardHandler(BYTE byKeyCode)
{
	if (byKeyCode == KEY_1)
	{
		if (m_byCursorPos > 0)
		{
			m_byCursorPos--;
			m_byCursorPage	 = m_byCursorPos/2;
		}
	}
	else if (byKeyCode == KEY_2)
	{
		m_byCursorPos++;
		m_byCursorPage	 = m_byCursorPos/2;
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
			MENU_ITEM*	pMenuItem = &m_aMenuItems[m_mcbCurrent.byStartMenuItemID];

	   		g_UnitCfg.dat.byXtimeOpen = m_bufKeyboard[1]*10 + m_bufKeyboard[0] - 528;
			g_UnitCfg.dat.byXtimeClose = m_bufKeyboard[3]*10 + m_bufKeyboard[2] - 528;

			S_PUSH(m_stackMenuCtlBlock, m_mcbCurrent);
			m_mcbCurrent.pMenu 						= pMenuItem;
			m_mcbCurrent.byStartMenuItemID			= 0;
		}
	}
}

static void XsetXtime_DisplayHandler()
{
	BYTE byNum[6];
		
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
	for(uint16_t i = 0; i < 4; i++)
		m_bufKeyboard[i] = '0';
	m_byCursorPage	 = 0;
}

static void SystemSafepos_KeyboardHandler(BYTE byKeyCode)
{
	if (byKeyCode == KEY_1)
	{
		if (m_byCursorPos > 0)
		{
			m_byCursorPos--;
		}
	}
	else if (byKeyCode == KEY_2)
	{
		m_byCursorPos++;
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
			MENU_ITEM*	pMenuItem = &m_aMenuItems[m_mcbCurrent.byStartMenuItemID];

	   		g_UnitCfg.dat.bySafePos = m_bufKeyboard[2]*100 + m_bufKeyboard[1]*10 + m_bufKeyboard[0] - 5328;

			S_PUSH(m_stackMenuCtlBlock, m_mcbCurrent);
			m_mcbCurrent.pMenu 						= pMenuItem;
			m_mcbCurrent.byStartMenuItemID			= 0;
		}
	}
}

static void SystemSafepos_DisplayHandler()
{
	BYTE byNum[6];
		
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
	for(uint16_t i = 0; i < 3; i++)
		m_bufKeyboard[i] = '0';
	m_byCursorPage	 = 0;
}

static void SystemSetfact_OpeningHandler()
{
	clearLCD();
	display_char("DDDDDDDD");
	clearLCD();	
}

static void Charact11_OpeningHandler()
{
	clearLCD();
	g_UnitCfg.dat.bIsChaFree = FALSE;
	g_UnitCfg.dat.byN = 0;
	display_char("DDDDDDDD");
	clearLCD();	
}

static void Charact125_OpeningHandler()
{
	clearLCD();
	g_UnitCfg.dat.bIsChaFree = FALSE;
	g_UnitCfg.dat.byN = -25;
	display_char("DDDDDDDD");
	clearLCD();	
}

static void Charact133_OpeningHandler()
{
	clearLCD();
	g_UnitCfg.dat.bIsChaFree = FALSE;
	g_UnitCfg.dat.byN = -33;
	display_char("DDDDDDDD");
	clearLCD();	
}

static void Charact150_OpeningHandler()
{
	clearLCD();
	g_UnitCfg.dat.bIsChaFree = FALSE;
	g_UnitCfg.dat.byN = -50;
	display_char("DDDDDDDD");
	clearLCD();	
}

static void Charact251_OpeningHandler()
{
	clearLCD();
	g_UnitCfg.dat.bIsChaFree = FALSE;
	g_UnitCfg.dat.byN = 25;
	display_char("DDDDDDDD");
	clearLCD();	
}

static void Charact331_OpeningHandler()
{
	clearLCD();
	g_UnitCfg.dat.bIsChaFree = FALSE;
	g_UnitCfg.dat.byN = 33;
	display_char("DDDDDDDD");
	clearLCD();	
}

static void Charact501_OpeningHandler()
{
	clearLCD();
	g_UnitCfg.dat.bIsChaFree = FALSE;
	g_UnitCfg.dat.byN = 50;
	display_char("DDDDDDDD");
	clearLCD();	
}

static void CharactFree_KeyboardHandler(BYTE byKeyCode)
{
	if (byKeyCode == KEY_1)
	{
		if (m_byCursorPos > 0)
		{
			m_byCursorPos--;
			m_byCursorPage	 = m_byCursorPos/3;
		}
	}
	else if (byKeyCode == KEY_2)
	{
		m_byCursorPos++;
		m_byCursorPage	 = m_byCursorPos/3;
	}
	else if (byKeyCode == KEY_3)
	{
		if (m_byCursorPos < 63)
		{
			m_bufKeyboard[m_byCursorPos]++;
			if (m_bufKeyboard[m_byCursorPos] > '9')
				m_bufKeyboard[m_byCursorPos] = '0';
		}
		else
		{
			MENU_ITEM*	pMenuItem = &m_aMenuItems[m_mcbCurrent.byStartMenuItemID];
	   		
			g_UnitCfg.dat.bIsChaFree = TRUE;
			for(uint16_t i = 0; i < 21; i++)
				g_UnitCfg.dat.byCha[i] = m_bufKeyboard[3*i + 2]*100 + m_bufKeyboard[3*i + 1]*10 + m_bufKeyboard[3*i] - 5328;

			S_PUSH(m_stackMenuCtlBlock, m_mcbCurrent);
			m_mcbCurrent.pMenu 						= pMenuItem;
			m_mcbCurrent.byStartMenuItemID			= 0;
		}
	}
}

static void CharactFree_DisplayHandler()
{
	BYTE byNum[6];
		
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
	for(uint16_t i = 0; i < 63; i++)
		m_bufKeyboard[i] = '0';
	m_byCursorPage	 = 0;
}

static void DircmdRise_OpeningHandler()
{
	clearLCD();
	g_UnitCfg.dat.bIsCmdInverse = FALSE;
	display_char("DDDDDDDD");
	clearLCD();	
}

static void DircmdFall_OpeningHandler()
{
	clearLCD();
	g_UnitCfg.dat.bIsCmdInverse = TRUE;
	display_char("DDDDDDDD");
	clearLCD();	
}

static void DiractRise_OpeningHandler()
{
	clearLCD();
	g_UnitCfg.dat.bIsActInverse = FALSE;
	display_char("DDDDDDDD");
	clearLCD();	
}

static void DiractFall_OpeningHandler()
{
	clearLCD();
	g_UnitCfg.dat.bIsActInverse = TRUE;
	display_char("DDDDDDDD");
	clearLCD();	
}

static void XcontrolDbnd_KeyboardHandler(BYTE byKeyCode)
{
	if (byKeyCode == KEY_1)
	{
		if (m_byCursorPos > 0)
		{
			m_byCursorPos--;
		}
	}
	else if (byKeyCode == KEY_2)
	{
		m_byCursorPos++;
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
			MENU_ITEM*	pMenuItem = &m_aMenuItems[m_mcbCurrent.byStartMenuItemID];

	   		g_UnitCfg.dat.byDbnd = m_bufKeyboard[2]*100 + m_bufKeyboard[1]*10 + m_bufKeyboard[0] - 5328;

			S_PUSH(m_stackMenuCtlBlock, m_mcbCurrent);
			m_mcbCurrent.pMenu 						= pMenuItem;
			m_mcbCurrent.byStartMenuItemID			= 0;
		}
	}
}

static void XcontrolDbnd_DisplayHandler()
{
	BYTE byNum[6];
		
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
	for(uint16_t i = 0; i < 3; i++)
		m_bufKeyboard[i] = '0';
	m_byCursorPage	 = 0;
}

static void XcontrolPara_KeyboardHandler(BYTE byKeyCode)
{
	if (byKeyCode == KEY_1)
	{
		if (m_byCursorPos > 0)
		{
			m_byCursorPos--;
			m_byCursorPage	 = m_byCursorPos/3;
		}
	}
	else if (byKeyCode == KEY_2)
	{
		m_byCursorPos++;
		m_byCursorPage	 = m_byCursorPos/3;
	}
	else if (byKeyCode == KEY_3)
	{
		if (m_byCursorPos < 6)
		{
			m_bufKeyboard[m_byCursorPos]++;
			if (m_bufKeyboard[m_byCursorPos] > '9')
				m_bufKeyboard[m_byCursorPos] = '0';
		}
		else
		{
			MENU_ITEM*	pMenuItem = &m_aMenuItems[m_mcbCurrent.byStartMenuItemID];
	   		
			g_UnitCfg.dat.byKxD = m_bufKeyboard[2]*100 + m_bufKeyboard[1]*10 + m_bufKeyboard[0] - 5328;
			g_UnitCfg.dat.byKxU = m_bufKeyboard[5]*100 + m_bufKeyboard[4]*10 + m_bufKeyboard[3] - 5328;

			S_PUSH(m_stackMenuCtlBlock, m_mcbCurrent);
			m_mcbCurrent.pMenu 						= pMenuItem;
			m_mcbCurrent.byStartMenuItemID			= 0;
		}
	}
}

static void XcontrolPara_DisplayHandler()
{
	BYTE byNum[6];
		
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
	for(uint16_t i = 0; i < 6; i++)
		m_bufKeyboard[i] = '0';
	m_byCursorPage	 = 0;
}

#define   ABS(x)   ((x)> 0?(x):-(x)) 

extern void SetPwmDutyCycle2(int32_t uiDutyCycle);

static void XtuneAuto_OpeningHandler()
{
	clearLCD();
	display_char("DDDDDDDD");

	static float fDiff	= 100;
	static float fPos	= 0;
	static float fLast	= 0;
	uint16_t temp1,temp2;	

	SetPwmDutyCycle2(-0x900);

	while(ABS(fDiff) > 5)
	{
	   	fPos	= g_UnitData.dat.fPos;
		fDiff	= fPos - fLast;
		fLast	= fPos;
	}

	temp1 = g_UnitData.dat.iAD4;

	fDiff	= 100;
	fPos	= 0;
	fLast	= 0;

	SetPwmDutyCycle2(0x900);

	while(ABS(fDiff) > 5)
	{
	   	fPos	= g_UnitData.dat.fPos;
		fDiff	= fPos - fLast;
		fLast	= fPos;
	}

	temp2 = g_UnitData.dat.iAD4;

	g_UnitCfg.dat.iAd4Min = temp1;
	g_UnitCfg.dat.iAd4Max = temp2;

	display_char("DDDDDDDD");
}

static void XtunePwm_KeyboardHandler(BYTE byKeyCode)
{
	if (byKeyCode == KEY_1)
	{
		if (m_byCursorPos > 0)
		{
			m_byCursorPos--;
			m_byCursorPage	 = m_byCursorPos/3;
		}
	}
	else if (byKeyCode == KEY_2)
	{
		m_byCursorPos++;
		m_byCursorPage	 = m_byCursorPos/3;
	}
	else if (byKeyCode == KEY_3)
	{
		if (m_byCursorPos < 6)
		{
			m_bufKeyboard[m_byCursorPos]++;
			if (m_bufKeyboard[m_byCursorPos] > '9')
				m_bufKeyboard[m_byCursorPos] = '0';
		}
		else
		{
			MENU_ITEM*	pMenuItem = &m_aMenuItems[m_mcbCurrent.byStartMenuItemID];
	   		
			g_UnitCfg.dat.byYbU = m_bufKeyboard[2]*100 + m_bufKeyboard[1]*10 + m_bufKeyboard[0] - 5328;
			g_UnitCfg.dat.byYeU = m_bufKeyboard[5]*100 + m_bufKeyboard[4]*10 + m_bufKeyboard[3] - 5328;

			S_PUSH(m_stackMenuCtlBlock, m_mcbCurrent);
			m_mcbCurrent.pMenu 						= pMenuItem;
			m_mcbCurrent.byStartMenuItemID			= 0;
		}
	}
}

static void XtunePwm_DisplayHandler()
{
	BYTE byNum[6];
		
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
	for(uint16_t i = 0; i < 6; i++)
		m_bufKeyboard[i] = '0';
	m_byCursorPage	 = 0;
}

static void XtuneAir_KeyboardHandler(BYTE byKeyCode)
{
	if (byKeyCode == KEY_1)
	{
		if (m_byCursorPos > 0)
		{
			m_byCursorPos--;
			m_byCursorPage	 = m_byCursorPos/2;
		}
	}
	else if (byKeyCode == KEY_2)
	{
		m_byCursorPos++;
		m_byCursorPage	 = m_byCursorPos/2;
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
			MENU_ITEM*	pMenuItem = &m_aMenuItems[m_mcbCurrent.byStartMenuItemID];

	   		g_UnitCfg.dat.byAirOpen = m_bufKeyboard[1]*10 + m_bufKeyboard[0] - 528;
			g_UnitCfg.dat.byAirClose = m_bufKeyboard[3]*10 + m_bufKeyboard[2] - 528;

			S_PUSH(m_stackMenuCtlBlock, m_mcbCurrent);
			m_mcbCurrent.pMenu 						= pMenuItem;
			m_mcbCurrent.byStartMenuItemID			= 0;
		}
	}
}

static void XtuneAir_DisplayHandler()
{
	BYTE byNum[6];
		
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
	for(uint16_t i = 0; i < 4; i++)
		m_bufKeyboard[i] = '0';
	m_byCursorPage	 = 0;
}

static void SigerrFunon_OpeningHandler()
{
	clearLCD();
	g_UnitCfg.dat.byErr = 2;
	display_char("DDDDDDDD");
	clearLCD();	
}

static void SigerrPoson_OpeningHandler()
{
	clearLCD();
	g_UnitCfg.dat.byErr = 1;
	display_char("DDDDDDDD");
	clearLCD();	
}

static void SigerrOff_OpeningHandler()
{
	clearLCD();
	g_UnitCfg.dat.byErr = 0;
	display_char("DDDDDDDD");
	clearLCD();	
}

static void Serio9600_OpeningHandler()
{
	clearLCD();
	g_UnitCfg.dat.uBau = 9600;
	display_char("DDDDDDDD");
	clearLCD();	
}

static void Serio19200_OpeningHandler()
{
	clearLCD();
	g_UnitCfg.dat.uBau = 19200;
	display_char("DDDDDDDD");
	clearLCD();	
}

static void Serio38400_OpeningHandler()
{
	clearLCD();
	g_UnitCfg.dat.uBau = 38400;
	display_char("DDDDDDDD");
	clearLCD();	
}

static void Serio1200_OpeningHandler()
{
	clearLCD();
	g_UnitCfg.dat.uBau = 1200;
	display_char("DDDDDDDD");
	clearLCD();	
}

static void Serio2400_OpeningHandler()
{
	clearLCD();
	g_UnitCfg.dat.uBau = 2400;
	display_char("DDDDDDDD");
	clearLCD();	
}

static void Serio4800_OpeningHandler()
{
	clearLCD();
	g_UnitCfg.dat.uBau = 4800;
	display_char("DDDDDDDD");
	clearLCD();	
}

static void CodeKey_KeyboardHandler(BYTE byKeyCode)
{
	if (byKeyCode == KEY_1)
	{
		if (m_byCursorPos > 0)
		{
			m_byCursorPos--;
			m_byCursorPage	 = m_byCursorPos/4;
		}
	}
	else if (byKeyCode == KEY_2)
	{
		m_byCursorPos++;
		m_byCursorPage	 = m_byCursorPos/4;
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
			MENU_ITEM*	pMenuItem = &m_aMenuItems[m_mcbCurrent.byStartMenuItemID];

	   		g_UnitCfg.dat.iCode = m_bufKeyboard[3]*1000 + m_bufKeyboard[2]*100 + m_bufKeyboard[1]*10 + m_bufKeyboard[0] - 53328;

			S_PUSH(m_stackMenuCtlBlock, m_mcbCurrent);
			m_mcbCurrent.pMenu 						= pMenuItem;
			m_mcbCurrent.byStartMenuItemID			= 0;
		}
	}
}

static void CodeKey_DisplayHandler()
{
	BYTE byNum[6];
		
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
	for(uint16_t i = 0; i < 4; i++)
		m_bufKeyboard[i] = '0';
	m_byCursorPage	 = 0;
}

static void CodeMenu_KeyboardHandler(BYTE byKeyCode)
{
	if (byKeyCode == KEY_1)
	{
		if (m_byCursorPos > 0)
		{
			m_byCursorPos--;
			m_byCursorPage	 = m_byCursorPos/4;
		}
	}
	else if (byKeyCode == KEY_2)
	{
		m_byCursorPos++;
		m_byCursorPage	 = m_byCursorPos/4;
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
			MENU_ITEM*	pMenuItem = &m_aMenuItems[m_mcbCurrent.byStartMenuItemID];

	   		g_UnitCfg.dat.iCode = m_bufKeyboard[3]*1000 + m_bufKeyboard[2]*100 + m_bufKeyboard[1]*10 + m_bufKeyboard[0] - 53328;

			S_PUSH(m_stackMenuCtlBlock, m_mcbCurrent);
			m_mcbCurrent.pMenu 						= pMenuItem;
			m_mcbCurrent.byStartMenuItemID			= 0;
		}
	}
}

static void CodeMenu_DisplayHandler()
{
	BYTE byNum[6];
		
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
	for(uint16_t i = 0; i < 4; i++)
		m_bufKeyboard[i] = '0';
	m_byCursorPage	 = 0;
}

static void CodeGlob_KeyboardHandler(BYTE byKeyCode)
{
	if (byKeyCode == KEY_1)
	{
		if (m_byCursorPos > 0)
		{
			m_byCursorPos--;
			m_byCursorPage	 = m_byCursorPos/4;
		}
	}
	else if (byKeyCode == KEY_2)
	{
		m_byCursorPos++;
		m_byCursorPage	 = m_byCursorPos/4;
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
			MENU_ITEM*	pMenuItem = &m_aMenuItems[m_mcbCurrent.byStartMenuItemID];

	   		g_UnitCfg.dat.iCode = m_bufKeyboard[3]*1000 + m_bufKeyboard[2]*100 + m_bufKeyboard[1]*10 + m_bufKeyboard[0] - 53328;

			S_PUSH(m_stackMenuCtlBlock, m_mcbCurrent);
			m_mcbCurrent.pMenu 						= pMenuItem;
			m_mcbCurrent.byStartMenuItemID			= 0;
		}
	}
}

static void CodeGlob_DisplayHandler()
{
	BYTE byNum[6];
		
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
	for(uint16_t i = 0; i < 4; i++)
		m_bufKeyboard[i] = '0';
	m_byCursorPage	 = 0;
}

static void CaluserPos_KeyboardHandler(BYTE byKeyCode)
{
	uint16_t temp1 = 0;
	uint16_t temp2 = 100;	

	if (byKeyCode == KEY_1)
	{
		SetPwmDutyCycle2(-0x900);
	}
	else if (byKeyCode == KEY_2)
	{
		SetPwmDutyCycle2(0x900);
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
			MENU_ITEM*	pMenuItem = &m_aMenuItems[m_mcbCurrent.byStartMenuItemID];

	   		g_UnitCfg.dat.iAd4Min = temp1;
			g_UnitCfg.dat.iAd4Max = temp2;
			g_UnitCfg.dat.bIsManual = FALSE;

			S_PUSH(m_stackMenuCtlBlock, m_mcbCurrent);
			m_mcbCurrent.pMenu 						= pMenuItem;
			m_mcbCurrent.byStartMenuItemID			= 0;
		}
	}
}

static void CaluserPos_DisplayHandler()
{
	BYTE byNum[6];
	
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
	m_byCursorPage	 	= 0;
}

static void CaluserInp_KeyboardHandler(BYTE byKeyCode)
{
	uint16_t temp1 = 0;
	uint16_t temp2 = 100;	

	if (byKeyCode == KEY_1)
	{
		SetPwmDutyCycle2(-0x900);
	}
	else if (byKeyCode == KEY_2)
	{
		SetPwmDutyCycle2(0x900);
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
			MENU_ITEM*	pMenuItem = &m_aMenuItems[m_mcbCurrent.byStartMenuItemID];

	   		g_UnitCfg.dat.iAd4Min = temp1;
			g_UnitCfg.dat.iAd4Max = temp2;
			g_UnitCfg.dat.bIsManual = FALSE;

			S_PUSH(m_stackMenuCtlBlock, m_mcbCurrent);
			m_mcbCurrent.pMenu 						= pMenuItem;
			m_mcbCurrent.byStartMenuItemID			= 0;
		}
	}
}

static void CaluserInp_DisplayHandler()
{
	BYTE byNum[6];
	
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

static void CaluserInp_OpeningHandler()
{
	m_byCursorPage	 	= 0;
}

static void CaluserFact_OpeningHandler()
{
	clearLCD();
	display_char("DDDDDDDD");
	clearLCD();	
}

static void PcontrlDbnd_KeyboardHandler(BYTE byKeyCode)
{
	if (byKeyCode == KEY_1)
	{
		if (m_byCursorPos > 0)
		{
			m_byCursorPos--;
		}
	}
	else if (byKeyCode == KEY_2)
	{
		m_byCursorPos++;
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
			MENU_ITEM*	pMenuItem = &m_aMenuItems[m_mcbCurrent.byStartMenuItemID];

	   		g_UnitCfg.dat.byPidDbnd = m_bufKeyboard[2]*100 + m_bufKeyboard[1]*10 + m_bufKeyboard[0] - 5328;

			S_PUSH(m_stackMenuCtlBlock, m_mcbCurrent);
			m_mcbCurrent.pMenu 						= pMenuItem;
			m_mcbCurrent.byStartMenuItemID			= 0;
		}
	}
}

static void PcontrlDbnd_DisplayHandler()
{
	BYTE byNum[6];
		
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
	for(uint16_t i = 0; i < 3; i++)
		m_bufKeyboard[i] = '0';
	m_byCursorPage	 = 0;
}
  
static void PcontrlPara_KeyboardHandler(BYTE byKeyCode)
{
	if (byKeyCode == KEY_1)
	{
		if (m_byCursorPos > 0)
		{
			m_byCursorPos--;
			m_byCursorPage	 = m_byCursorPos/4;
		}
	}
	else if (byKeyCode == KEY_2)
	{
		m_byCursorPos++;
		m_byCursorPage	 = m_byCursorPos/4;
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
			MENU_ITEM*	pMenuItem = &m_aMenuItems[m_mcbCurrent.byStartMenuItemID];
	   		
//			g_UnitCfg.dat.byLimD = m_bufKeyboard[2]*100 + m_bufKeyboard[1]*10 + m_bufKeyboard[0] - 5328;
//			g_UnitCfg.dat.byLimU = m_bufKeyboard[5]*100 + m_bufKeyboard[4]*10 + m_bufKeyboard[3] - 5328;

			S_PUSH(m_stackMenuCtlBlock, m_mcbCurrent);
			m_mcbCurrent.pMenu 						= pMenuItem;
			m_mcbCurrent.byStartMenuItemID			= 0;
		}
	}
}

static void PcontrlPara_DisplayHandler()
{
	BYTE byNum[6];
		
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
	for(uint16_t i = 0; i < 16; i++)
		m_bufKeyboard[i] = '0';
	m_byCursorPage	 = 0;
}

static void PcontrlFilt_KeyboardHandler(BYTE byKeyCode)
{
	if (byKeyCode == KEY_1)
	{
		if (m_byCursorPos > 0)
		{
			m_byCursorPos--;
		}
	}
	else if (byKeyCode == KEY_2)
	{
		m_byCursorPos++;
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
			MENU_ITEM*	pMenuItem = &m_aMenuItems[m_mcbCurrent.byStartMenuItemID];
	   		
			g_UnitCfg.dat.byFilt = m_bufKeyboard[0] - 48;

			S_PUSH(m_stackMenuCtlBlock, m_mcbCurrent);
			m_mcbCurrent.pMenu 						= pMenuItem;
			m_mcbCurrent.byStartMenuItemID			= 0;
		}
	}
}

static void PcontrlFilt_DisplayHandler()
{
	BYTE byNum[6];
		
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

static void PcontrlKv_KeyboardHandler(BYTE byKeyCode)
{
	if (byKeyCode == KEY_1)
	{
		if (m_byCursorPos > 0)
		{
			m_byCursorPos--;
		}
	}
	else if (byKeyCode == KEY_2)
	{
		m_byCursorPos++;
		if (m_byCursorPos > 4)
			m_byCursorPage	 = 1;
		if (m_byCursorPos > 9)
			m_byCursorPage	 = 2;
		if (m_byCursorPos > 14)
			m_byCursorPage	 = 3;
	}
	else if (byKeyCode == KEY_3)
	{
		if (m_byCursorPos < 20)
		{
			m_bufKeyboard[m_byCursorPos]++;
			if (m_bufKeyboard[m_byCursorPos] > '9')
				m_bufKeyboard[m_byCursorPos] = '0';
		}
		else
		{
			MENU_ITEM*	pMenuItem = &m_aMenuItems[m_mcbCurrent.byStartMenuItemID];
	   		//add some fun to due with m_bufKeyboard[m_byCursorPos],or do any action
			S_PUSH(m_stackMenuCtlBlock, m_mcbCurrent);
			m_mcbCurrent.pMenu 						= pMenuItem;
			m_mcbCurrent.byStartMenuItemID			= 0;
		}
	}
}

static void PcontrlKv_DisplayHandler()
{
	BYTE byNum[6];	
	if (m_byCursorPage == 0)
	{
		for(U16 i = 0; i < 5; i++)
			byNum[5 - i] = m_bufKeyboard[i];
		display_char("[[[[[[KP");
		display_digital(byNum,0,m_byCursorPos + 1);
	}
	if (m_byCursorPage == 1)
	{
		for(U16 i = 0; i < 5; i++)
			byNum[5 - i] = m_bufKeyboard[5 + i];
		display_char("[[[[[[TN");
		display_digital(byNum,0,m_byCursorPos - 4);
	}
	if (m_byCursorPage == 2)
	{
		for(U16 i = 0; i < 5; i++)
			byNum[5 - i] = m_bufKeyboard[10 + i];
		display_char("[[[[[[TV");
		display_digital(byNum,0,m_byCursorPos - 9);
	}
	if (m_byCursorPage == 3)
	{
		for(U16 i = 0; i < 5; i++)
			byNum[5 - i] = m_bufKeyboard[15 + i];
		display_char("[[[[[[XO");
		display_digital(byNum,0,m_byCursorPos - 14);
	}
}

static void PcontrlKv_OpeningHandler()
{
	m_byCursorPos = 0;
	
	clearLCD();
	m_bufKeyboard[0] = '0';
	m_bufKeyboard[1] = '0';
	m_bufKeyboard[2] = '0';
	m_bufKeyboard[3] = '0';
	m_bufKeyboard[4] = '0';
	m_bufKeyboard[5] = '0';
	m_bufKeyboard[6] = '0';
	m_bufKeyboard[7] = '0';
	m_bufKeyboard[8] = '0';
	m_bufKeyboard[9] = '0';
	m_bufKeyboard[10] = '0';
	m_bufKeyboard[11] = '0';
	m_bufKeyboard[12] = '0';
	m_bufKeyboard[13] = '0';
	m_bufKeyboard[14] = '0';
	m_bufKeyboard[15] = '0';
	m_bufKeyboard[16] = '0';
	m_bufKeyboard[17] = '0';
	m_bufKeyboard[18] = '0';
	m_bufKeyboard[19] = '0';
	m_bufKeyboard[20]= '0';
	m_byCursorPage	 = 0;
}

static void SposOpn_OpeningHandler()
{
	clearLCD();
	display_char("DDDDDDDD");
	clearLCD();	
}

static void SposCls_OpeningHandler()
{
	clearLCD();
	display_char("DDDDDDDD");
	clearLCD();	
}

static void MaOpn_OpeningHandler()
{
	clearLCD();
	display_char("DDDDDDDD");
	clearLCD();	
}

static void MaCls_OpeningHandler()
{
	clearLCD();
	display_char("DDDDDDDD");
	clearLCD();	
}

static void AnlPos_OpeningHandler()
{
	clearLCD();
	display_char("DDDDDDDD");
	clearLCD();	
}

static void AnlCmd_OpeningHandler()
{
	clearLCD();
	display_char("DDDDDDDD");
	clearLCD();	
}

static void AnlSp_OpeningHandler()
{
	clearLCD();
	display_char("DDDDDDDD");
	clearLCD();	
}

static void AnlP1_OpeningHandler()
{
	clearLCD();
	display_char("DDDDDDDD");
	clearLCD();	
}

static void AnlP2_OpeningHandler()
{
	clearLCD();
	display_char("DDDDDDDD");
	clearLCD();	
}

static void AnlT_OpeningHandler()
{
	clearLCD();
	display_char("DDDDDDDD");
	clearLCD();	
}

static void AnlSet4_OpeningHandler()
{
	clearLCD();
	display_char("DDDDDDDD");
	clearLCD();	
}

static void AnlSet0_OpeningHandler()
{
	clearLCD();
	display_char("DDDDDDDD");
	clearLCD();	
}

static void AnlSet10_OpeningHandler()
{
	clearLCD();
	display_char("DDDDDDDD");
	clearLCD();	
}

static void AnlSet5_OpeningHandler()
{
	clearLCD();
	display_char("DDDDDDDD");
	clearLCD();	
}

static void ScalS_KeyboardHandler(BYTE byKeyCode)
{
	if (byKeyCode == KEY_1)
	{
		if (m_byCursorPos > 0)
		{
			m_byCursorPos--;
		}
	}
	else if (byKeyCode == KEY_2)
	{
		m_byCursorPos++;
		if (m_byCursorPos > 4)
			m_byCursorPage	 = 1;
		if (m_byCursorPos > 9)
			m_byCursorPage	 = 2;
		if (m_byCursorPos > 14)
			m_byCursorPage	 = 3;
	}
	else if (byKeyCode == KEY_3)
	{
		if (m_byCursorPos < 20)
		{
			m_bufKeyboard[m_byCursorPos]++;
			if (m_bufKeyboard[m_byCursorPos] > '9')
				m_bufKeyboard[m_byCursorPos] = '0';
		}
		else
		{
			MENU_ITEM*	pMenuItem = &m_aMenuItems[m_mcbCurrent.byStartMenuItemID];
	   		//add some fun to due with m_bufKeyboard[m_byCursorPos],or do any action
			S_PUSH(m_stackMenuCtlBlock, m_mcbCurrent);
			m_mcbCurrent.pMenu 						= pMenuItem;
			m_mcbCurrent.byStartMenuItemID			= 0;
		}
	}
}

static void ScalS_DisplayHandler()
{
	BYTE byNum[6];	
	if (m_byCursorPage == 0)
	{
		for(U16 i = 0; i < 5; i++)
			byNum[5 - i] = m_bufKeyboard[i];
		display_char("[[[[[[KP");
		display_digital(byNum,0,m_byCursorPos + 1);
	}
	if (m_byCursorPage == 1)
	{
		for(U16 i = 0; i < 5; i++)
			byNum[5 - i] = m_bufKeyboard[5 + i];
		display_char("[[[[[[TN");
		display_digital(byNum,0,m_byCursorPos - 4);
	}
	if (m_byCursorPage == 2)
	{
		for(U16 i = 0; i < 5; i++)
			byNum[5 - i] = m_bufKeyboard[10 + i];
		display_char("[[[[[[TV");
		display_digital(byNum,0,m_byCursorPos - 9);
	}
	if (m_byCursorPage == 3)
	{
		for(U16 i = 0; i < 5; i++)
			byNum[5 - i] = m_bufKeyboard[15 + i];
		display_char("[[[[[[XO");
		display_digital(byNum,0,m_byCursorPos - 14);
	}
}

static void ScalS_OpeningHandler()
{
	m_byCursorPos = 0;
	
	clearLCD();
	m_bufKeyboard[0] = '0';
	m_bufKeyboard[1] = '0';
	m_bufKeyboard[2] = '0';
	m_bufKeyboard[3] = '0';
	m_bufKeyboard[4] = '0';
	m_bufKeyboard[5] = '0';
	m_bufKeyboard[6] = '0';
	m_bufKeyboard[7] = '0';
	m_bufKeyboard[8] = '0';
	m_bufKeyboard[9] = '0';
	m_bufKeyboard[10] = '0';
	m_bufKeyboard[11] = '0';
	m_bufKeyboard[12] = '0';
	m_bufKeyboard[13] = '0';
	m_bufKeyboard[14] = '0';
	m_bufKeyboard[15] = '0';
	m_bufKeyboard[16] = '0';
	m_bufKeyboard[17] = '0';
	m_bufKeyboard[18] = '0';
	m_bufKeyboard[19] = '0';
	m_bufKeyboard[20]= '0';
	m_byCursorPage	 = 0;
}

static void ScalF_KeyboardHandler(BYTE byKeyCode)
{
	if (byKeyCode == KEY_1)
	{
		if (m_byCursorPos > 0)
		{
			m_byCursorPos--;
		}
	}
	else if (byKeyCode == KEY_2)
	{
		m_byCursorPos++;
		if (m_byCursorPos > 4)
			m_byCursorPage	 = 1;
		if (m_byCursorPos > 9)
			m_byCursorPage	 = 2;
		if (m_byCursorPos > 14)
			m_byCursorPage	 = 3;
	}
	else if (byKeyCode == KEY_3)
	{
		if (m_byCursorPos < 20)
		{
			m_bufKeyboard[m_byCursorPos]++;
			if (m_bufKeyboard[m_byCursorPos] > '9')
				m_bufKeyboard[m_byCursorPos] = '0';
		}
		else
		{
			MENU_ITEM*	pMenuItem = &m_aMenuItems[m_mcbCurrent.byStartMenuItemID];
	   		//add some fun to due with m_bufKeyboard[m_byCursorPos],or do any action
			S_PUSH(m_stackMenuCtlBlock, m_mcbCurrent);
			m_mcbCurrent.pMenu 						= pMenuItem;
			m_mcbCurrent.byStartMenuItemID			= 0;
		}
	}
}

static void ScalF_DisplayHandler()
{
	BYTE byNum[6];	
	if (m_byCursorPage == 0)
	{
		for(U16 i = 0; i < 5; i++)
			byNum[5 - i] = m_bufKeyboard[i];
		display_char("[[[[[[KP");
		display_digital(byNum,0,m_byCursorPos + 1);
	}
	if (m_byCursorPage == 1)
	{
		for(U16 i = 0; i < 5; i++)
			byNum[5 - i] = m_bufKeyboard[5 + i];
		display_char("[[[[[[TN");
		display_digital(byNum,0,m_byCursorPos - 4);
	}
	if (m_byCursorPage == 2)
	{
		for(U16 i = 0; i < 5; i++)
			byNum[5 - i] = m_bufKeyboard[10 + i];
		display_char("[[[[[[TV");
		display_digital(byNum,0,m_byCursorPos - 9);
	}
	if (m_byCursorPage == 3)
	{
		for(U16 i = 0; i < 5; i++)
			byNum[5 - i] = m_bufKeyboard[15 + i];
		display_char("[[[[[[XO");
		display_digital(byNum,0,m_byCursorPos - 14);
	}
}

static void ScalF_OpeningHandler()
{
	m_byCursorPos = 0;
	
	clearLCD();
	m_bufKeyboard[0] = '0';
	m_bufKeyboard[1] = '0';
	m_bufKeyboard[2] = '0';
	m_bufKeyboard[3] = '0';
	m_bufKeyboard[4] = '0';
	m_bufKeyboard[5] = '0';
	m_bufKeyboard[6] = '0';
	m_bufKeyboard[7] = '0';
	m_bufKeyboard[8] = '0';
	m_bufKeyboard[9] = '0';
	m_bufKeyboard[10] = '0';
	m_bufKeyboard[11] = '0';
	m_bufKeyboard[12] = '0';
	m_bufKeyboard[13] = '0';
	m_bufKeyboard[14] = '0';
	m_bufKeyboard[15] = '0';
	m_bufKeyboard[16] = '0';
	m_bufKeyboard[17] = '0';
	m_bufKeyboard[18] = '0';
	m_bufKeyboard[19] = '0';
	m_bufKeyboard[20]= '0';
	m_byCursorPage	 = 0;
}

static void ScalT_KeyboardHandler(BYTE byKeyCode)
{
	if (byKeyCode == KEY_1)
	{
		if (m_byCursorPos > 0)
		{
			m_byCursorPos--;
		}
	}
	else if (byKeyCode == KEY_2)
	{
		m_byCursorPos++;
		if (m_byCursorPos > 4)
			m_byCursorPage	 = 1;
		if (m_byCursorPos > 9)
			m_byCursorPage	 = 2;
		if (m_byCursorPos > 14)
			m_byCursorPage	 = 3;
	}
	else if (byKeyCode == KEY_3)
	{
		if (m_byCursorPos < 20)
		{
			m_bufKeyboard[m_byCursorPos]++;
			if (m_bufKeyboard[m_byCursorPos] > '9')
				m_bufKeyboard[m_byCursorPos] = '0';
		}
		else
		{
			MENU_ITEM*	pMenuItem = &m_aMenuItems[m_mcbCurrent.byStartMenuItemID];
	   		//add some fun to due with m_bufKeyboard[m_byCursorPos],or do any action
			S_PUSH(m_stackMenuCtlBlock, m_mcbCurrent);
			m_mcbCurrent.pMenu 						= pMenuItem;
			m_mcbCurrent.byStartMenuItemID			= 0;
		}
	}
}

static void ScalT_DisplayHandler()
{
	BYTE byNum[6];	
	if (m_byCursorPage == 0)
	{
		for(U16 i = 0; i < 5; i++)
			byNum[5 - i] = m_bufKeyboard[i];
		display_char("[[[[[[KP");
		display_digital(byNum,0,m_byCursorPos + 1);
	}
	if (m_byCursorPage == 1)
	{
		for(U16 i = 0; i < 5; i++)
			byNum[5 - i] = m_bufKeyboard[5 + i];
		display_char("[[[[[[TN");
		display_digital(byNum,0,m_byCursorPos - 4);
	}
	if (m_byCursorPage == 2)
	{
		for(U16 i = 0; i < 5; i++)
			byNum[5 - i] = m_bufKeyboard[10 + i];
		display_char("[[[[[[TV");
		display_digital(byNum,0,m_byCursorPos - 9);
	}
	if (m_byCursorPage == 3)
	{
		for(U16 i = 0; i < 5; i++)
			byNum[5 - i] = m_bufKeyboard[15 + i];
		display_char("[[[[[[XO");
		display_digital(byNum,0,m_byCursorPos - 14);
	}
}

static void ScalT_OpeningHandler()
{
	m_byCursorPos = 0;
	
	clearLCD();
	m_bufKeyboard[0] = '0';
	m_bufKeyboard[1] = '0';
	m_bufKeyboard[2] = '0';
	m_bufKeyboard[3] = '0';
	m_bufKeyboard[4] = '0';
	m_bufKeyboard[5] = '0';
	m_bufKeyboard[6] = '0';
	m_bufKeyboard[7] = '0';
	m_bufKeyboard[8] = '0';
	m_bufKeyboard[9] = '0';
	m_bufKeyboard[10] = '0';
	m_bufKeyboard[11] = '0';
	m_bufKeyboard[12] = '0';
	m_bufKeyboard[13] = '0';
	m_bufKeyboard[14] = '0';
	m_bufKeyboard[15] = '0';
	m_bufKeyboard[16] = '0';
	m_bufKeyboard[17] = '0';
	m_bufKeyboard[18] = '0';
	m_bufKeyboard[19] = '0';
	m_bufKeyboard[20]= '0';
	m_byCursorPage	 = 0;
}

static void ScalP_KeyboardHandler(BYTE byKeyCode)
{
	if (byKeyCode == KEY_1)
	{
		if (m_byCursorPos > 0)
		{
			m_byCursorPos--;
		}
	}
	else if (byKeyCode == KEY_2)
	{
		m_byCursorPos++;
		if (m_byCursorPos > 4)
			m_byCursorPage	 = 1;
		if (m_byCursorPos > 9)
			m_byCursorPage	 = 2;
		if (m_byCursorPos > 14)
			m_byCursorPage	 = 3;
	}
	else if (byKeyCode == KEY_3)
	{
		if (m_byCursorPos < 20)
		{
			m_bufKeyboard[m_byCursorPos]++;
			if (m_bufKeyboard[m_byCursorPos] > '9')
				m_bufKeyboard[m_byCursorPos] = '0';
		}
		else
		{
			MENU_ITEM*	pMenuItem = &m_aMenuItems[m_mcbCurrent.byStartMenuItemID];
	   		//add some fun to due with m_bufKeyboard[m_byCursorPos],or do any action
			S_PUSH(m_stackMenuCtlBlock, m_mcbCurrent);
			m_mcbCurrent.pMenu 						= pMenuItem;
			m_mcbCurrent.byStartMenuItemID			= 0;
		}
	}
}

static void ScalP_DisplayHandler()
{
	BYTE byNum[6];	
	if (m_byCursorPage == 0)
	{
		for(U16 i = 0; i < 5; i++)
			byNum[5 - i] = m_bufKeyboard[i];
		display_char("[[[[[[KP");
		display_digital(byNum,0,m_byCursorPos + 1);
	}
	if (m_byCursorPage == 1)
	{
		for(U16 i = 0; i < 5; i++)
			byNum[5 - i] = m_bufKeyboard[5 + i];
		display_char("[[[[[[TN");
		display_digital(byNum,0,m_byCursorPos - 4);
	}
	if (m_byCursorPage == 2)
	{
		for(U16 i = 0; i < 5; i++)
			byNum[5 - i] = m_bufKeyboard[10 + i];
		display_char("[[[[[[TV");
		display_digital(byNum,0,m_byCursorPos - 9);
	}
	if (m_byCursorPage == 3)
	{
		for(U16 i = 0; i < 5; i++)
			byNum[5 - i] = m_bufKeyboard[15 + i];
		display_char("[[[[[[XO");
		display_digital(byNum,0,m_byCursorPos - 14);
	}
}

static void ScalP_OpeningHandler()
{
	m_byCursorPos = 0;
	
	clearLCD();
	m_bufKeyboard[0] = '0';
	m_bufKeyboard[1] = '0';
	m_bufKeyboard[2] = '0';
	m_bufKeyboard[3] = '0';
	m_bufKeyboard[4] = '0';
	m_bufKeyboard[5] = '0';
	m_bufKeyboard[6] = '0';
	m_bufKeyboard[7] = '0';
	m_bufKeyboard[8] = '0';
	m_bufKeyboard[9] = '0';
	m_bufKeyboard[10] = '0';
	m_bufKeyboard[11] = '0';
	m_bufKeyboard[12] = '0';
	m_bufKeyboard[13] = '0';
	m_bufKeyboard[14] = '0';
	m_bufKeyboard[15] = '0';
	m_bufKeyboard[16] = '0';
	m_bufKeyboard[17] = '0';
	m_bufKeyboard[18] = '0';
	m_bufKeyboard[19] = '0';
	m_bufKeyboard[20]= '0';
	m_byCursorPage	 = 0;
}

static void TuneNot_OpeningHandler()
{
	clearLCD();
	display_char("DDDDDDDD");
	clearLCD();	
}

static void TuneDef_OpeningHandler()
{
	clearLCD();
	display_char("DDDDDDDD");
	clearLCD();	
}

static void TuneFlow_OpeningHandler()
{
	clearLCD();
	display_char("DDDDDDDD");
	clearLCD();	
}

static void TuneTemp_OpeningHandler()
{
	clearLCD();
	display_char("DDDDDDDD");
	clearLCD();	
}

static void TunePres_OpeningHandler()
{
	clearLCD();
	display_char("DDDDDDDD");
	clearLCD();	
}

static void TuneLevl_OpeningHandler()
{
	clearLCD();
	display_char("DDDDDDDD");
	clearLCD();	
}

static void LeakNot_OpeningHandler()
{
	clearLCD();
	display_char("DDDDDDDD");
	clearLCD();	
}

static void LeakMeas_KeyboardHandler(BYTE byKeyCode)
{
	if (byKeyCode == KEY_1)
	{
		if (m_byCursorPos > 0)
		{
			m_byCursorPos--;
		}
	}
	else if (byKeyCode == KEY_2)
	{
		m_byCursorPos++;
		if (m_byCursorPos > 4)
			m_byCursorPage	 = 1;
		if (m_byCursorPos > 9)
			m_byCursorPage	 = 2;
		if (m_byCursorPos > 14)
			m_byCursorPage	 = 3;
	}
	else if (byKeyCode == KEY_3)
	{
		if (m_byCursorPos < 20)
		{
			m_bufKeyboard[m_byCursorPos]++;
			if (m_bufKeyboard[m_byCursorPos] > '9')
				m_bufKeyboard[m_byCursorPos] = '0';
		}
		else
		{
			MENU_ITEM*	pMenuItem = &m_aMenuItems[m_mcbCurrent.byStartMenuItemID];
	   		//add some fun to due with m_bufKeyboard[m_byCursorPos],or do any action
			S_PUSH(m_stackMenuCtlBlock, m_mcbCurrent);
			m_mcbCurrent.pMenu 						= pMenuItem;
			m_mcbCurrent.byStartMenuItemID			= 0;
		}
	}
}

static void LeakMeas_DisplayHandler()
{
	BYTE byNum[6];	
	if (m_byCursorPage == 0)
	{
		for(U16 i = 0; i < 5; i++)
			byNum[5 - i] = m_bufKeyboard[i];
		display_char("[[[[[[KP");
		display_digital(byNum,0,m_byCursorPos + 1);
	}
	if (m_byCursorPage == 1)
	{
		for(U16 i = 0; i < 5; i++)
			byNum[5 - i] = m_bufKeyboard[5 + i];
		display_char("[[[[[[TN");
		display_digital(byNum,0,m_byCursorPos - 4);
	}
	if (m_byCursorPage == 2)
	{
		for(U16 i = 0; i < 5; i++)
			byNum[5 - i] = m_bufKeyboard[10 + i];
		display_char("[[[[[[TV");
		display_digital(byNum,0,m_byCursorPos - 9);
	}
	if (m_byCursorPage == 3)
	{
		for(U16 i = 0; i < 5; i++)
			byNum[5 - i] = m_bufKeyboard[15 + i];
		display_char("[[[[[[XO");
		display_digital(byNum,0,m_byCursorPos - 14);
	}
}

static void LeakMeas_OpeningHandler()
{
	m_byCursorPos = 0;
	
	clearLCD();
	m_bufKeyboard[0] = '0';
	m_bufKeyboard[1] = '0';
	m_bufKeyboard[2] = '0';
	m_bufKeyboard[3] = '0';
	m_bufKeyboard[4] = '0';
	m_bufKeyboard[5] = '0';
	m_bufKeyboard[6] = '0';
	m_bufKeyboard[7] = '0';
	m_bufKeyboard[8] = '0';
	m_bufKeyboard[9] = '0';
	m_bufKeyboard[10] = '0';
	m_bufKeyboard[11] = '0';
	m_bufKeyboard[12] = '0';
	m_bufKeyboard[13] = '0';
	m_bufKeyboard[14] = '0';
	m_bufKeyboard[15] = '0';
	m_bufKeyboard[16] = '0';
	m_bufKeyboard[17] = '0';
	m_bufKeyboard[18] = '0';
	m_bufKeyboard[19] = '0';
	m_bufKeyboard[20]= '0';
	m_byCursorPage	 = 0;
}

static void LeakChar_OpeningHandler()
{
	clearLCD();
	display_char("DDDDDDDD");
	clearLCD();	
}

void HMI_Init()
{
	initLCD_1622();
	// Enable internal pull-up resister
	GP3CON	&= ~0x00003330;
//	GP3CON	&= ~0x00003000;
//	GP0CON	&= ~0x00033000;

	// Init Main Menu
	m_aMenuItems[MENU_MAIN_ACT].byMenuItemID				= MENU_MAIN_ACT;
	m_aMenuItems[MENU_MAIN_ACT].byChildMenuItems			= 8;
	m_aMenuItems[MENU_MAIN_ACT].pChildMenu					= &m_aMenuItems[MENU_ACT_SNGL];
	m_aMenuItems[MENU_MAIN_ACT].pParentMenu					= NULL;
	m_aMenuItems[MENU_MAIN_ACT].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_MAIN_ACT].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_MAIN_ACT].OnOpeningHandler			= NULL;

	m_aMenuItems[MENU_MAIN_INPUT].byMenuItemID				= MENU_MAIN_INPUT;
	m_aMenuItems[MENU_MAIN_INPUT].byChildMenuItems			= 8;
	m_aMenuItems[MENU_MAIN_INPUT].pChildMenu				= &m_aMenuItems[MENU_INPUT_4];
	m_aMenuItems[MENU_MAIN_INPUT].pParentMenu				= NULL;
	m_aMenuItems[MENU_MAIN_INPUT].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_MAIN_INPUT].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_MAIN_INPUT].OnOpeningHandler			= NULL;

	m_aMenuItems[MENU_MAIN_INSET].byMenuItemID				= MENU_MAIN_INSET;
	m_aMenuItems[MENU_MAIN_INSET].byChildMenuItems			= 8;
	m_aMenuItems[MENU_MAIN_INSET].pChildMenu				= &m_aMenuItems[MENU_INSET_CHARACT];
	m_aMenuItems[MENU_MAIN_INSET].pParentMenu				= NULL;
	m_aMenuItems[MENU_MAIN_INSET].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_MAIN_INSET].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_MAIN_INSET].OnOpeningHandler			= NULL;

	m_aMenuItems[MENU_MAIN_XSET].byMenuItemID				= MENU_MAIN_XSET;
	m_aMenuItems[MENU_MAIN_XSET].byChildMenuItems			= 8;
	m_aMenuItems[MENU_MAIN_XSET].pChildMenu					= &m_aMenuItems[MENU_XSET_XTUNE];
	m_aMenuItems[MENU_MAIN_XSET].pParentMenu				= NULL;
	m_aMenuItems[MENU_MAIN_XSET].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_MAIN_XSET].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_MAIN_XSET].OnOpeningHandler			= NULL; 
	
	m_aMenuItems[MENU_MAIN_SYSTEM].byMenuItemID				= MENU_MAIN_SYSTEM;
	m_aMenuItems[MENU_MAIN_SYSTEM].byChildMenuItems			= 8;
	m_aMenuItems[MENU_MAIN_SYSTEM].pChildMenu				= &m_aMenuItems[MENU_SYSTEM_SERIO];
	m_aMenuItems[MENU_MAIN_SYSTEM].pParentMenu				= NULL;
	m_aMenuItems[MENU_MAIN_SYSTEM].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_MAIN_SYSTEM].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_MAIN_SYSTEM].OnOpeningHandler			= NULL;

	m_aMenuItems[MENU_MAIN_CONTRL].byMenuItemID				= MENU_MAIN_CONTRL;
	m_aMenuItems[MENU_MAIN_CONTRL].byChildMenuItems			= 8;
	m_aMenuItems[MENU_MAIN_CONTRL].pChildMenu				= &m_aMenuItems[MENU_CONTRL_BININ];
	m_aMenuItems[MENU_MAIN_CONTRL].pParentMenu				= NULL;
	m_aMenuItems[MENU_MAIN_CONTRL].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_MAIN_CONTRL].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_MAIN_CONTRL].OnOpeningHandler			= NULL;

	m_aMenuItems[MENU_MAIN_MANUAL].byMenuItemID				= MENU_MAIN_MANUAL;
	m_aMenuItems[MENU_MAIN_MANUAL].byChildMenuItems			= 8;
	m_aMenuItems[MENU_MAIN_MANUAL].pChildMenu				= NULL;
	m_aMenuItems[MENU_MAIN_MANUAL].pParentMenu				= NULL;
	m_aMenuItems[MENU_MAIN_MANUAL].KeyboardHandler			= MENU_MAIN_MANUAL_KeyboardHandler;
	m_aMenuItems[MENU_MAIN_MANUAL].DisplayHandler			= MENU_MAIN_MANUAL_DisplayHandler;
	m_aMenuItems[MENU_MAIN_MANUAL].OnOpeningHandler			= MENU_MAIN_MANUAL_OpeningHandler;

	m_aMenuItems[MENU_MAIN_END].byMenuItemID				= MENU_MAIN_END;
	m_aMenuItems[MENU_MAIN_END].byChildMenuItems			= 8;
	m_aMenuItems[MENU_MAIN_END].pChildMenu					= NULL;
	m_aMenuItems[MENU_MAIN_END].pParentMenu					= NULL;
	m_aMenuItems[MENU_MAIN_END].KeyboardHandler				= NULL;
	m_aMenuItems[MENU_MAIN_END].DisplayHandler				= NULL;
	m_aMenuItems[MENU_MAIN_END].OnOpeningHandler			= NULL;

	// Init Act Menu
	m_aMenuItems[MENU_ACT_SNGL].byMenuItemID				= MENU_ACT_SNGL;
	m_aMenuItems[MENU_ACT_SNGL].byChildMenuItems			= 3;
	m_aMenuItems[MENU_ACT_SNGL].pChildMenu					= NULL;
	m_aMenuItems[MENU_ACT_SNGL].pParentMenu					= &m_aMenuItems[MENU_MAIN_ACT];
	m_aMenuItems[MENU_ACT_SNGL].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_ACT_SNGL].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_ACT_SNGL].OnOpeningHandler			= ActSngl_OpeningHandler;

	m_aMenuItems[MENU_ACT_DOUB].byMenuItemID				= MENU_ACT_DOUB;
	m_aMenuItems[MENU_ACT_DOUB].byChildMenuItems			= 3;
	m_aMenuItems[MENU_ACT_DOUB].pChildMenu					= NULL;
	m_aMenuItems[MENU_ACT_DOUB].pParentMenu					= &m_aMenuItems[MENU_MAIN_ACT];
	m_aMenuItems[MENU_ACT_DOUB].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_ACT_DOUB].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_ACT_DOUB].OnOpeningHandler			= ActDoub_OpeningHandler;

	m_aMenuItems[MENU_ACT_END].byMenuItemID					= MENU_ACT_END;
	m_aMenuItems[MENU_ACT_END].byChildMenuItems				= 3;
	m_aMenuItems[MENU_ACT_END].pChildMenu					= &m_aMenuItems[MENU_MAIN_ACT];
	m_aMenuItems[MENU_ACT_END].pParentMenu					= &m_aMenuItems[MENU_MAIN_ACT];
	m_aMenuItems[MENU_ACT_END].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_ACT_END].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_ACT_END].OnOpeningHandler				= NULL;

	// Init Input Menu
	m_aMenuItems[MENU_INPUT_4].byMenuItemID					= MENU_INPUT_4;
	m_aMenuItems[MENU_INPUT_4].byChildMenuItems				= 5;
	m_aMenuItems[MENU_INPUT_4].pChildMenu					= NULL;
	m_aMenuItems[MENU_INPUT_4].pParentMenu					= &m_aMenuItems[MENU_MAIN_INPUT];
	m_aMenuItems[MENU_INPUT_4].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_INPUT_4].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_INPUT_4].OnOpeningHandler				= Input4_OpeningHandler;

	m_aMenuItems[MENU_INPUT_0].byMenuItemID					= MENU_INPUT_0;
	m_aMenuItems[MENU_INPUT_0].byChildMenuItems				= 5;
	m_aMenuItems[MENU_INPUT_0].pChildMenu					= NULL;
	m_aMenuItems[MENU_INPUT_0].pParentMenu					= &m_aMenuItems[MENU_MAIN_INPUT];
	m_aMenuItems[MENU_INPUT_0].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_INPUT_0].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_INPUT_0].OnOpeningHandler				= Input0_OpeningHandler;

	m_aMenuItems[MENU_INPUT_10].byMenuItemID				= MENU_INPUT_10;
	m_aMenuItems[MENU_INPUT_10].byChildMenuItems			= 5;
	m_aMenuItems[MENU_INPUT_10].pChildMenu					= NULL;
	m_aMenuItems[MENU_INPUT_10].pParentMenu					= &m_aMenuItems[MENU_MAIN_INPUT];
	m_aMenuItems[MENU_INPUT_10].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_INPUT_10].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_INPUT_10].OnOpeningHandler			= Input10_OpeningHandler;

	m_aMenuItems[MENU_INPUT_5].byMenuItemID					= MENU_INPUT_5;
	m_aMenuItems[MENU_INPUT_5].byChildMenuItems				= 5;
	m_aMenuItems[MENU_INPUT_5].pChildMenu					= NULL;
	m_aMenuItems[MENU_INPUT_5].pParentMenu					= &m_aMenuItems[MENU_MAIN_INPUT];
	m_aMenuItems[MENU_INPUT_5].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_INPUT_5].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_INPUT_5].OnOpeningHandler				= Input5_OpeningHandler;

	m_aMenuItems[MENU_INPUT_END].byMenuItemID				= MENU_INPUT_END;
	m_aMenuItems[MENU_INPUT_END].byChildMenuItems			= 5;
	m_aMenuItems[MENU_INPUT_END].pChildMenu					= &m_aMenuItems[MENU_MAIN_INPUT];
	m_aMenuItems[MENU_INPUT_END].pParentMenu				= &m_aMenuItems[MENU_MAIN_INPUT];
	m_aMenuItems[MENU_INPUT_END].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_INPUT_END].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_INPUT_END].OnOpeningHandler			= NULL;

	// Init In Set Menu
	m_aMenuItems[MENU_INSET_CHARACT].byMenuItemID			= MENU_INSET_CHARACT;
	m_aMenuItems[MENU_INSET_CHARACT].byChildMenuItems		= 6;
	m_aMenuItems[MENU_INSET_CHARACT].pChildMenu				= &m_aMenuItems[MENU_CHARACT_l_1];
	m_aMenuItems[MENU_INSET_CHARACT].pParentMenu			= &m_aMenuItems[MENU_MAIN_INSET];
	m_aMenuItems[MENU_INSET_CHARACT].KeyboardHandler		= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_INSET_CHARACT].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_INSET_CHARACT].OnOpeningHandler		= NULL;

	m_aMenuItems[MENU_INSET_CUTOFF].byMenuItemID			= MENU_INSET_CUTOFF;
	m_aMenuItems[MENU_INSET_CUTOFF].byChildMenuItems		= 6;
	m_aMenuItems[MENU_INSET_CUTOFF].pChildMenu				= NULL;
	m_aMenuItems[MENU_INSET_CUTOFF].pParentMenu				= &m_aMenuItems[MENU_MAIN_INSET];
	m_aMenuItems[MENU_INSET_CUTOFF].KeyboardHandler			= InsetCutoff_KeyboardHandler;
	m_aMenuItems[MENU_INSET_CUTOFF].DisplayHandler			= InsetCutoff_DisplayHandler;
	m_aMenuItems[MENU_INSET_CUTOFF].OnOpeningHandler		= InsetCutoff_OpeningHandler;

	m_aMenuItems[MENU_INSET_DIRCMD].byMenuItemID			= MENU_INSET_DIRCMD;
	m_aMenuItems[MENU_INSET_DIRCMD].byChildMenuItems		= 6;
	m_aMenuItems[MENU_INSET_DIRCMD].pChildMenu				= &m_aMenuItems[MENU_DIRCMD_RISE];
	m_aMenuItems[MENU_INSET_DIRCMD].pParentMenu				= &m_aMenuItems[MENU_MAIN_INSET];
	m_aMenuItems[MENU_INSET_DIRCMD].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_INSET_DIRCMD].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_INSET_DIRCMD].OnOpeningHandler		= NULL;

	m_aMenuItems[MENU_INSET_DIRACT].byMenuItemID			= MENU_INSET_DIRACT;
	m_aMenuItems[MENU_INSET_DIRACT].byChildMenuItems		= 6;
	m_aMenuItems[MENU_INSET_DIRACT].pChildMenu				= &m_aMenuItems[MENU_DIRACT_RISE];
	m_aMenuItems[MENU_INSET_DIRACT].pParentMenu				= &m_aMenuItems[MENU_MAIN_INSET];
	m_aMenuItems[MENU_INSET_DIRACT].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_INSET_DIRACT].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_INSET_DIRACT].OnOpeningHandler		= NULL;

	m_aMenuItems[MENU_INSET_SPLTRNG].byMenuItemID			= MENU_INSET_SPLTRNG;
	m_aMenuItems[MENU_INSET_SPLTRNG].byChildMenuItems		= 6;
	m_aMenuItems[MENU_INSET_SPLTRNG].pChildMenu				= NULL;
	m_aMenuItems[MENU_INSET_SPLTRNG].pParentMenu			= &m_aMenuItems[MENU_MAIN_INSET];
	m_aMenuItems[MENU_INSET_SPLTRNG].KeyboardHandler		= InsetSpltrng_KeyboardHandler;
	m_aMenuItems[MENU_INSET_SPLTRNG].DisplayHandler			= InsetSpltrng_DisplayHandler;
	m_aMenuItems[MENU_INSET_SPLTRNG].OnOpeningHandler		= InsetSpltrng_OpeningHandler;

	m_aMenuItems[MENU_INSET_END].byMenuItemID				= MENU_INSET_END;
	m_aMenuItems[MENU_INSET_END].byChildMenuItems			= 6;
	m_aMenuItems[MENU_INSET_END].pChildMenu					= &m_aMenuItems[MENU_MAIN_INSET];
	m_aMenuItems[MENU_INSET_END].pParentMenu				= &m_aMenuItems[MENU_MAIN_INSET];
	m_aMenuItems[MENU_INSET_END].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_INSET_END].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_INSET_END].OnOpeningHandler			= NULL;

	// Init X Set Menu
	m_aMenuItems[MENU_XSET_XLIMIT].byMenuItemID				= MENU_XSET_XLIMIT;
	m_aMenuItems[MENU_XSET_XLIMIT].byChildMenuItems			= 5;
	m_aMenuItems[MENU_XSET_XLIMIT].pChildMenu				= NULL;
	m_aMenuItems[MENU_XSET_XLIMIT].pParentMenu				= &m_aMenuItems[MENU_MAIN_XSET];
	m_aMenuItems[MENU_XSET_XLIMIT].KeyboardHandler			= XsetXlimit_KeyboardHandler;
	m_aMenuItems[MENU_XSET_XLIMIT].DisplayHandler			= XsetXlimit_DisplayHandler;
	m_aMenuItems[MENU_XSET_XLIMIT].OnOpeningHandler			= XsetXlimit_OpeningHandler;

	m_aMenuItems[MENU_XSET_XTIME].byMenuItemID				= MENU_XSET_XTIME;
	m_aMenuItems[MENU_XSET_XTIME].byChildMenuItems			= 5;
	m_aMenuItems[MENU_XSET_XTIME].pChildMenu				= NULL;
	m_aMenuItems[MENU_XSET_XTIME].pParentMenu				= &m_aMenuItems[MENU_MAIN_XSET];
	m_aMenuItems[MENU_XSET_XTIME].KeyboardHandler			= XsetXtime_KeyboardHandler;
	m_aMenuItems[MENU_XSET_XTIME].DisplayHandler			= XsetXtime_DisplayHandler;
	m_aMenuItems[MENU_XSET_XTIME].OnOpeningHandler			= XsetXtime_OpeningHandler;
														
	m_aMenuItems[MENU_XSET_XCONTRL].byMenuItemID			= MENU_XSET_XCONTRL;
	m_aMenuItems[MENU_XSET_XCONTRL].byChildMenuItems		= 5;
	m_aMenuItems[MENU_XSET_XCONTRL].pChildMenu				= &m_aMenuItems[MENU_XCONTRL_END];
	m_aMenuItems[MENU_XSET_XCONTRL].pParentMenu				= &m_aMenuItems[MENU_MAIN_XSET];
	m_aMenuItems[MENU_XSET_XCONTRL].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_XSET_XCONTRL].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_XSET_XCONTRL].OnOpeningHandler		= NULL;

	m_aMenuItems[MENU_XSET_XTUNE].byMenuItemID				= MENU_XSET_XTUNE;
	m_aMenuItems[MENU_XSET_XTUNE].byChildMenuItems			= 5;
	m_aMenuItems[MENU_XSET_XTUNE].pChildMenu				= &m_aMenuItems[MENU_XTUNE_AUTO];
	m_aMenuItems[MENU_XSET_XTUNE].pParentMenu				= &m_aMenuItems[MENU_MAIN_XSET];
	m_aMenuItems[MENU_XSET_XTUNE].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_XSET_XTUNE].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_XSET_XTUNE].OnOpeningHandler			= NULL;

	m_aMenuItems[MENU_XSET_END].byMenuItemID				= MENU_XSET_END;
	m_aMenuItems[MENU_XSET_END].byChildMenuItems			= 5;
	m_aMenuItems[MENU_XSET_END].pChildMenu					= &m_aMenuItems[MENU_MAIN_XSET];
	m_aMenuItems[MENU_XSET_END].pParentMenu					= &m_aMenuItems[MENU_MAIN_XSET];
	m_aMenuItems[MENU_XSET_END].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_XSET_END].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_XSET_END].OnOpeningHandler			= NULL;

	// Init System Menu
	m_aMenuItems[MENU_SYSTEM_SAFEPOS].byMenuItemID			= MENU_SYSTEM_SAFEPOS;
	m_aMenuItems[MENU_SYSTEM_SAFEPOS].byChildMenuItems		= 6;
	m_aMenuItems[MENU_SYSTEM_SAFEPOS].pChildMenu			= NULL;
	m_aMenuItems[MENU_SYSTEM_SAFEPOS].pParentMenu			= &m_aMenuItems[MENU_MAIN_SYSTEM];
	m_aMenuItems[MENU_SYSTEM_SAFEPOS].KeyboardHandler		= SystemSafepos_KeyboardHandler;
	m_aMenuItems[MENU_SYSTEM_SAFEPOS].DisplayHandler		= SystemSafepos_DisplayHandler;
	m_aMenuItems[MENU_SYSTEM_SAFEPOS].OnOpeningHandler		= SystemSafepos_OpeningHandler;

	m_aMenuItems[MENU_SYSTEM_SIGERR].byMenuItemID			= MENU_SYSTEM_SIGERR;
	m_aMenuItems[MENU_SYSTEM_SIGERR].byChildMenuItems		= 6;
	m_aMenuItems[MENU_SYSTEM_SIGERR].pChildMenu				= &m_aMenuItems[MENU_SIGERR_FUNON];
	m_aMenuItems[MENU_SYSTEM_SIGERR].pParentMenu			= &m_aMenuItems[MENU_MAIN_SYSTEM];
	m_aMenuItems[MENU_SYSTEM_SIGERR].KeyboardHandler		= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_SYSTEM_SIGERR].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_SYSTEM_SIGERR].OnOpeningHandler		= NULL;

	m_aMenuItems[MENU_SYSTEM_SERIO].byMenuItemID			= MENU_SYSTEM_SERIO;
	m_aMenuItems[MENU_SYSTEM_SERIO].byChildMenuItems		= 6;
	m_aMenuItems[MENU_SYSTEM_SERIO].pChildMenu				= &m_aMenuItems[MENU_SERIO_9600];
	m_aMenuItems[MENU_SYSTEM_SERIO].pParentMenu				= &m_aMenuItems[MENU_MAIN_SYSTEM];
	m_aMenuItems[MENU_SYSTEM_SERIO].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_SYSTEM_SERIO].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_SYSTEM_SERIO].OnOpeningHandler		= NULL;

	m_aMenuItems[MENU_SYSTEM_CODE].byMenuItemID				= MENU_SYSTEM_CODE;
	m_aMenuItems[MENU_SYSTEM_CODE].byChildMenuItems			= 6;
	m_aMenuItems[MENU_SYSTEM_CODE].pChildMenu				= &m_aMenuItems[MENU_CODE_END];
	m_aMenuItems[MENU_SYSTEM_CODE].pParentMenu				= &m_aMenuItems[MENU_MAIN_SYSTEM];
	m_aMenuItems[MENU_SYSTEM_CODE].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_SYSTEM_CODE].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_SYSTEM_CODE].OnOpeningHandler			= NULL;

	m_aMenuItems[MENU_SYSTEM_SETFACT].byMenuItemID			= MENU_SYSTEM_SETFACT;
	m_aMenuItems[MENU_SYSTEM_SETFACT].byChildMenuItems		= 6;
	m_aMenuItems[MENU_SYSTEM_SETFACT].pChildMenu			= NULL;
	m_aMenuItems[MENU_SYSTEM_SETFACT].pParentMenu			= &m_aMenuItems[MENU_MAIN_SYSTEM];
	m_aMenuItems[MENU_SYSTEM_SETFACT].KeyboardHandler		= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_SYSTEM_SETFACT].DisplayHandler		= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_SYSTEM_SETFACT].OnOpeningHandler		= SystemSetfact_OpeningHandler;

	m_aMenuItems[MENU_SYSTEM_END].byMenuItemID				= MENU_SYSTEM_END;
	m_aMenuItems[MENU_SYSTEM_END].byChildMenuItems			= 6;
	m_aMenuItems[MENU_SYSTEM_END].pChildMenu				= &m_aMenuItems[MENU_MAIN_SYSTEM];
	m_aMenuItems[MENU_SYSTEM_END].pParentMenu				= &m_aMenuItems[MENU_MAIN_SYSTEM];
	m_aMenuItems[MENU_SYSTEM_END].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_SYSTEM_END].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_SYSTEM_END].OnOpeningHandler			= NULL;
	
	// Init Control Menu
	m_aMenuItems[MENU_CONTRL_BININ].byMenuItemID			= MENU_CONTRL_BININ;
	m_aMenuItems[MENU_CONTRL_BININ].byChildMenuItems		= 5;
	m_aMenuItems[MENU_CONTRL_BININ].pChildMenu				= &m_aMenuItems[MENU_BININ_SPOS];
	m_aMenuItems[MENU_CONTRL_BININ].pParentMenu				= &m_aMenuItems[MENU_MAIN_CONTRL];
	m_aMenuItems[MENU_CONTRL_BININ].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_CONTRL_BININ].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_CONTRL_BININ].OnOpeningHandler		= NULL;

	m_aMenuItems[MENU_CONTRL_OUTPUT].byMenuItemID			= MENU_CONTRL_OUTPUT;
	m_aMenuItems[MENU_CONTRL_OUTPUT].byChildMenuItems		= 5;
	m_aMenuItems[MENU_CONTRL_OUTPUT].pChildMenu				= &m_aMenuItems[MENU_OUTPUT_ANL];
	m_aMenuItems[MENU_CONTRL_OUTPUT].pParentMenu			= &m_aMenuItems[MENU_MAIN_CONTRL];
	m_aMenuItems[MENU_CONTRL_OUTPUT].KeyboardHandler		= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_CONTRL_OUTPUT].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_CONTRL_OUTPUT].OnOpeningHandler		= NULL;
														
	m_aMenuItems[MENU_CONTRL_CALUSER].byMenuItemID			= MENU_CONTRL_CALUSER;
	m_aMenuItems[MENU_CONTRL_CALUSER].byChildMenuItems		= 5;
	m_aMenuItems[MENU_CONTRL_CALUSER].pChildMenu			= &m_aMenuItems[MENU_CALUSER_END];
	m_aMenuItems[MENU_CONTRL_CALUSER].pParentMenu			= &m_aMenuItems[MENU_MAIN_CONTRL];
	m_aMenuItems[MENU_CONTRL_CALUSER].KeyboardHandler		= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_CONTRL_CALUSER].DisplayHandler		= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_CONTRL_CALUSER].OnOpeningHandler		= NULL;

	m_aMenuItems[MENU_CONTRL_PCONTRL].byMenuItemID			= MENU_CONTRL_PCONTRL;
	m_aMenuItems[MENU_CONTRL_PCONTRL].byChildMenuItems		= 5;
	m_aMenuItems[MENU_CONTRL_PCONTRL].pChildMenu			= &m_aMenuItems[MENU_PCONTRL_END];
	m_aMenuItems[MENU_CONTRL_PCONTRL].pParentMenu			= &m_aMenuItems[MENU_MAIN_CONTRL];
	m_aMenuItems[MENU_CONTRL_PCONTRL].KeyboardHandler		= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_CONTRL_PCONTRL].DisplayHandler		= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_CONTRL_PCONTRL].OnOpeningHandler		= NULL;

	m_aMenuItems[MENU_CONTRL_END].byMenuItemID				= MENU_CONTRL_END;
	m_aMenuItems[MENU_CONTRL_END].byChildMenuItems			= 5;
	m_aMenuItems[MENU_CONTRL_END].pChildMenu				= &m_aMenuItems[MENU_MAIN_CONTRL];
	m_aMenuItems[MENU_CONTRL_END].pParentMenu				= &m_aMenuItems[MENU_MAIN_CONTRL];
	m_aMenuItems[MENU_CONTRL_END].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_CONTRL_END].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_CONTRL_END].OnOpeningHandler			= NULL;

	// Init Charact Menu
	m_aMenuItems[MENU_CHARACT_l_1].byMenuItemID				= MENU_CHARACT_l_1;
	m_aMenuItems[MENU_CHARACT_l_1].byChildMenuItems			= 9;
	m_aMenuItems[MENU_CHARACT_l_1].pChildMenu				= NULL;
	m_aMenuItems[MENU_CHARACT_l_1].pParentMenu				= &m_aMenuItems[MENU_INSET_CHARACT];
	m_aMenuItems[MENU_CHARACT_l_1].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_CHARACT_l_1].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_CHARACT_l_1].OnOpeningHandler			= Charact11_OpeningHandler;

	m_aMenuItems[MENU_CHARACT_l_25].byMenuItemID			= MENU_CHARACT_l_25;
	m_aMenuItems[MENU_CHARACT_l_25].byChildMenuItems		= 9;
	m_aMenuItems[MENU_CHARACT_l_25].pChildMenu				= NULL;
	m_aMenuItems[MENU_CHARACT_l_25].pParentMenu				= &m_aMenuItems[MENU_INSET_CHARACT];
	m_aMenuItems[MENU_CHARACT_l_25].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_CHARACT_l_25].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_CHARACT_l_25].OnOpeningHandler		= Charact125_OpeningHandler;
														
	m_aMenuItems[MENU_CHARACT_l_33].byMenuItemID			= MENU_CHARACT_l_33;
	m_aMenuItems[MENU_CHARACT_l_33].byChildMenuItems		= 9;
	m_aMenuItems[MENU_CHARACT_l_33].pChildMenu				= NULL;
	m_aMenuItems[MENU_CHARACT_l_33].pParentMenu				= &m_aMenuItems[MENU_INSET_CHARACT];
	m_aMenuItems[MENU_CHARACT_l_33].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_CHARACT_l_33].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_CHARACT_l_33].OnOpeningHandler		= Charact133_OpeningHandler;

	m_aMenuItems[MENU_CHARACT_l_50].byMenuItemID			= MENU_CHARACT_l_50;
	m_aMenuItems[MENU_CHARACT_l_50].byChildMenuItems		= 9;
	m_aMenuItems[MENU_CHARACT_l_50].pChildMenu				= NULL;
	m_aMenuItems[MENU_CHARACT_l_50].pParentMenu				= &m_aMenuItems[MENU_INSET_CHARACT];
	m_aMenuItems[MENU_CHARACT_l_50].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_CHARACT_l_50].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_CHARACT_l_50].OnOpeningHandler		= Charact150_OpeningHandler;

	m_aMenuItems[MENU_CHARACT_25_1].byMenuItemID			= MENU_CHARACT_25_1;
	m_aMenuItems[MENU_CHARACT_25_1].byChildMenuItems		= 9;
	m_aMenuItems[MENU_CHARACT_25_1].pChildMenu				= NULL;
	m_aMenuItems[MENU_CHARACT_25_1].pParentMenu				= &m_aMenuItems[MENU_INSET_CHARACT];
	m_aMenuItems[MENU_CHARACT_25_1].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_CHARACT_25_1].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_CHARACT_25_1].OnOpeningHandler		= Charact251_OpeningHandler;

	m_aMenuItems[MENU_CHARACT_33_1].byMenuItemID			= MENU_CHARACT_33_1;
	m_aMenuItems[MENU_CHARACT_33_1].byChildMenuItems		= 9;
	m_aMenuItems[MENU_CHARACT_33_1].pChildMenu				= NULL;
	m_aMenuItems[MENU_CHARACT_33_1].pParentMenu				= &m_aMenuItems[MENU_INSET_CHARACT];
	m_aMenuItems[MENU_CHARACT_33_1].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_CHARACT_33_1].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_CHARACT_33_1].OnOpeningHandler		= Charact331_OpeningHandler;
														
	m_aMenuItems[MENU_CHARACT_50_1].byMenuItemID			= MENU_CHARACT_50_1;
	m_aMenuItems[MENU_CHARACT_50_1].byChildMenuItems		= 9;
	m_aMenuItems[MENU_CHARACT_50_1].pChildMenu				= NULL;
	m_aMenuItems[MENU_CHARACT_50_1].pParentMenu				= &m_aMenuItems[MENU_INSET_CHARACT];
	m_aMenuItems[MENU_CHARACT_50_1].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_CHARACT_50_1].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_CHARACT_50_1].OnOpeningHandler		= Charact501_OpeningHandler;

	m_aMenuItems[MENU_CHARACT_FREE].byMenuItemID			= MENU_CHARACT_FREE;
	m_aMenuItems[MENU_CHARACT_FREE].byChildMenuItems		= 9;
	m_aMenuItems[MENU_CHARACT_FREE].pChildMenu				= NULL;
	m_aMenuItems[MENU_CHARACT_FREE].pParentMenu				= &m_aMenuItems[MENU_INSET_CHARACT];
	m_aMenuItems[MENU_CHARACT_FREE].KeyboardHandler			= CharactFree_KeyboardHandler;
	m_aMenuItems[MENU_CHARACT_FREE].DisplayHandler			= CharactFree_DisplayHandler;
	m_aMenuItems[MENU_CHARACT_FREE].OnOpeningHandler		= CharactFree_OpeningHandler;

	m_aMenuItems[MENU_CHARACT_END].byMenuItemID				= MENU_CHARACT_END;
	m_aMenuItems[MENU_CHARACT_END].byChildMenuItems			= 9;
	m_aMenuItems[MENU_CHARACT_END].pChildMenu				= &m_aMenuItems[MENU_INSET_CHARACT];
	m_aMenuItems[MENU_CHARACT_END].pParentMenu				= &m_aMenuItems[MENU_INSET_CHARACT];
	m_aMenuItems[MENU_CHARACT_END].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_CHARACT_END].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_CHARACT_END].OnOpeningHandler			= NULL;

	// Init Dir Cmd Menu
	m_aMenuItems[MENU_DIRCMD_RISE].byMenuItemID				= MENU_DIRCMD_RISE;
	m_aMenuItems[MENU_DIRCMD_RISE].byChildMenuItems			= 3;
	m_aMenuItems[MENU_DIRCMD_RISE].pChildMenu				= NULL;
	m_aMenuItems[MENU_DIRCMD_RISE].pParentMenu				= &m_aMenuItems[MENU_INSET_DIRCMD];
	m_aMenuItems[MENU_DIRCMD_RISE].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_DIRCMD_RISE].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_DIRCMD_RISE].OnOpeningHandler			= DircmdRise_OpeningHandler;

	m_aMenuItems[MENU_DIRCMD_FALL].byMenuItemID				= MENU_DIRCMD_FALL;
	m_aMenuItems[MENU_DIRCMD_FALL].byChildMenuItems			= 3;
	m_aMenuItems[MENU_DIRCMD_FALL].pChildMenu				= NULL;
	m_aMenuItems[MENU_DIRCMD_FALL].pParentMenu				= &m_aMenuItems[MENU_INSET_DIRCMD];
	m_aMenuItems[MENU_DIRCMD_FALL].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_DIRCMD_FALL].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_DIRCMD_FALL].OnOpeningHandler			= DircmdFall_OpeningHandler;

	m_aMenuItems[MENU_DIRCMD_END].byMenuItemID				= MENU_DIRCMD_END;
	m_aMenuItems[MENU_DIRCMD_END].byChildMenuItems			= 3;
	m_aMenuItems[MENU_DIRCMD_END].pChildMenu				= &m_aMenuItems[MENU_INSET_DIRCMD];
	m_aMenuItems[MENU_DIRCMD_END].pParentMenu				= &m_aMenuItems[MENU_INSET_DIRCMD];
	m_aMenuItems[MENU_DIRCMD_END].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_DIRCMD_END].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_DIRCMD_END].OnOpeningHandler			= NULL;

	// Init Dir Act Menu
	m_aMenuItems[MENU_DIRACT_RISE].byMenuItemID				= MENU_DIRACT_RISE;
	m_aMenuItems[MENU_DIRACT_RISE].byChildMenuItems			= 3;
	m_aMenuItems[MENU_DIRACT_RISE].pChildMenu				= NULL;
	m_aMenuItems[MENU_DIRACT_RISE].pParentMenu				= &m_aMenuItems[MENU_INSET_DIRACT];
	m_aMenuItems[MENU_DIRACT_RISE].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_DIRACT_RISE].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_DIRACT_RISE].OnOpeningHandler			= DiractRise_OpeningHandler;

	m_aMenuItems[MENU_DIRACT_FALL].byMenuItemID				= MENU_DIRACT_FALL;
	m_aMenuItems[MENU_DIRACT_FALL].byChildMenuItems			= 3;
	m_aMenuItems[MENU_DIRACT_FALL].pChildMenu				= NULL;
	m_aMenuItems[MENU_DIRACT_FALL].pParentMenu				= &m_aMenuItems[MENU_INSET_DIRACT];
	m_aMenuItems[MENU_DIRACT_FALL].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_DIRACT_FALL].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_DIRACT_FALL].OnOpeningHandler			= DiractFall_OpeningHandler;

	m_aMenuItems[MENU_DIRACT_END].byMenuItemID				= MENU_DIRACT_END;
	m_aMenuItems[MENU_DIRACT_END].byChildMenuItems			= 3;
	m_aMenuItems[MENU_DIRACT_END].pChildMenu				= &m_aMenuItems[MENU_INSET_DIRACT];
	m_aMenuItems[MENU_DIRACT_END].pParentMenu				= &m_aMenuItems[MENU_INSET_DIRACT];
	m_aMenuItems[MENU_DIRACT_END].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_DIRACT_END].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_DIRACT_END].OnOpeningHandler			= NULL;
	
	// X Contrl Menu
	m_aMenuItems[MENU_XCONTRL_DBMD].byMenuItemID			= MENU_XCONTRL_DBMD;
	m_aMenuItems[MENU_XCONTRL_DBMD].byChildMenuItems		= 3;
	m_aMenuItems[MENU_XCONTRL_DBMD].pChildMenu				= NULL;
	m_aMenuItems[MENU_XCONTRL_DBMD].pParentMenu				= &m_aMenuItems[MENU_XSET_XCONTRL];
	m_aMenuItems[MENU_XCONTRL_DBMD].KeyboardHandler			= XcontrolDbnd_KeyboardHandler;
	m_aMenuItems[MENU_XCONTRL_DBMD].DisplayHandler			= XcontrolDbnd_DisplayHandler;
	m_aMenuItems[MENU_XCONTRL_DBMD].OnOpeningHandler		= XcontrolDbnd_OpeningHandler;

	m_aMenuItems[MENU_XCONTRL_PARA].byMenuItemID			= MENU_XCONTRL_PARA;
	m_aMenuItems[MENU_XCONTRL_PARA].byChildMenuItems		= 3;
	m_aMenuItems[MENU_XCONTRL_PARA].pChildMenu				= NULL;
	m_aMenuItems[MENU_XCONTRL_PARA].pParentMenu				= &m_aMenuItems[MENU_XSET_XCONTRL];
	m_aMenuItems[MENU_XCONTRL_PARA].KeyboardHandler			= XcontrolPara_KeyboardHandler;
	m_aMenuItems[MENU_XCONTRL_PARA].DisplayHandler			= XcontrolPara_DisplayHandler;
	m_aMenuItems[MENU_XCONTRL_PARA].OnOpeningHandler		= XcontrolPara_OpeningHandler;

	m_aMenuItems[MENU_XCONTRL_END].byMenuItemID				= MENU_XCONTRL_END;
	m_aMenuItems[MENU_XCONTRL_END].byChildMenuItems			= 3;
	m_aMenuItems[MENU_XCONTRL_END].pChildMenu				= &m_aMenuItems[MENU_XSET_XCONTRL];
	m_aMenuItems[MENU_XCONTRL_END].pParentMenu				= &m_aMenuItems[MENU_XSET_XCONTRL];
	m_aMenuItems[MENU_XCONTRL_END].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_XCONTRL_END].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_XCONTRL_END].OnOpeningHandler			= NULL;
	
	// X Tune Menu
	m_aMenuItems[MENU_XTUNE_AUTO].byMenuItemID				= MENU_XTUNE_AUTO;
	m_aMenuItems[MENU_XTUNE_AUTO].byChildMenuItems			= 5;
	m_aMenuItems[MENU_XTUNE_AUTO].pChildMenu				= NULL;
	m_aMenuItems[MENU_XTUNE_AUTO].pParentMenu				= &m_aMenuItems[MENU_XSET_XTUNE];
	m_aMenuItems[MENU_XTUNE_AUTO].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_XTUNE_AUTO].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_XTUNE_AUTO].OnOpeningHandler			= XtuneAuto_OpeningHandler;

	m_aMenuItems[MENU_XTUNE_POS].byMenuItemID				= MENU_XTUNE_POS;
	m_aMenuItems[MENU_XTUNE_POS].byChildMenuItems			= 5;
	m_aMenuItems[MENU_XTUNE_POS].pChildMenu					= NULL;
	m_aMenuItems[MENU_XTUNE_POS].pParentMenu				= &m_aMenuItems[MENU_XSET_XTUNE];
	m_aMenuItems[MENU_XTUNE_POS].KeyboardHandler			= CaluserPos_KeyboardHandler;
	m_aMenuItems[MENU_XTUNE_POS].DisplayHandler				= CaluserPos_DisplayHandler;
	m_aMenuItems[MENU_XTUNE_POS].OnOpeningHandler			= CaluserPos_OpeningHandler;

	m_aMenuItems[MENU_XTUNE_PWM].byMenuItemID				= MENU_XTUNE_PWM;
	m_aMenuItems[MENU_XTUNE_PWM].byChildMenuItems			= 5;
	m_aMenuItems[MENU_XTUNE_PWM].pChildMenu					= NULL;
	m_aMenuItems[MENU_XTUNE_PWM].pParentMenu				= &m_aMenuItems[MENU_XSET_XTUNE];
	m_aMenuItems[MENU_XTUNE_PWM].KeyboardHandler			= XtunePwm_KeyboardHandler;
	m_aMenuItems[MENU_XTUNE_PWM].DisplayHandler				= XtunePwm_DisplayHandler;
	m_aMenuItems[MENU_XTUNE_PWM].OnOpeningHandler			= XtunePwm_OpeningHandler;

	m_aMenuItems[MENU_XTUNE_AIR].byMenuItemID				= MENU_XTUNE_AIR;
	m_aMenuItems[MENU_XTUNE_AIR].byChildMenuItems			= 5;
	m_aMenuItems[MENU_XTUNE_AIR].pChildMenu					= NULL;
	m_aMenuItems[MENU_XTUNE_AIR].pParentMenu				= &m_aMenuItems[MENU_XSET_XTUNE];
	m_aMenuItems[MENU_XTUNE_AIR].KeyboardHandler			= XtuneAir_KeyboardHandler;
	m_aMenuItems[MENU_XTUNE_AIR].DisplayHandler				= XtuneAir_DisplayHandler;
	m_aMenuItems[MENU_XTUNE_AIR].OnOpeningHandler			= XtuneAir_OpeningHandler;

	m_aMenuItems[MENU_XTUNE_END].byMenuItemID				= MENU_XTUNE_END;
	m_aMenuItems[MENU_XTUNE_END].byChildMenuItems			= 5;
	m_aMenuItems[MENU_XTUNE_END].pChildMenu					= &m_aMenuItems[MENU_XSET_XTUNE];
	m_aMenuItems[MENU_XTUNE_END].pParentMenu				= &m_aMenuItems[MENU_XSET_XTUNE];
	m_aMenuItems[MENU_XTUNE_END].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_XTUNE_END].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_XTUNE_END].OnOpeningHandler			= NULL;

	// Sig Err Menu
	m_aMenuItems[MENU_SIGERR_FUNON].byMenuItemID			= MENU_SIGERR_FUNON;
	m_aMenuItems[MENU_SIGERR_FUNON].byChildMenuItems		= 4;
	m_aMenuItems[MENU_SIGERR_FUNON].pChildMenu				= NULL;
	m_aMenuItems[MENU_SIGERR_FUNON].pParentMenu				= &m_aMenuItems[MENU_SYSTEM_SIGERR];
	m_aMenuItems[MENU_SIGERR_FUNON].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_SIGERR_FUNON].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_SIGERR_FUNON].OnOpeningHandler		= SigerrFunon_OpeningHandler;

	m_aMenuItems[MENU_SIGERR_POSON].byMenuItemID			= MENU_SIGERR_POSON;
	m_aMenuItems[MENU_SIGERR_POSON].byChildMenuItems		= 4;
	m_aMenuItems[MENU_SIGERR_POSON].pChildMenu				= NULL;
	m_aMenuItems[MENU_SIGERR_POSON].pParentMenu				= &m_aMenuItems[MENU_SYSTEM_SIGERR];
	m_aMenuItems[MENU_SIGERR_POSON].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_SIGERR_POSON].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_SIGERR_POSON].OnOpeningHandler		= SigerrPoson_OpeningHandler;

	m_aMenuItems[MENU_SIGERR_OFF].byMenuItemID				= MENU_SIGERR_OFF;
	m_aMenuItems[MENU_SIGERR_OFF].byChildMenuItems			= 4;
	m_aMenuItems[MENU_SIGERR_OFF].pChildMenu				= NULL;
	m_aMenuItems[MENU_SIGERR_OFF].pParentMenu				= &m_aMenuItems[MENU_SYSTEM_SIGERR];
	m_aMenuItems[MENU_SIGERR_OFF].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_SIGERR_OFF].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_SIGERR_OFF].OnOpeningHandler			= SigerrOff_OpeningHandler;

	m_aMenuItems[MENU_SIGERR_END].byMenuItemID				= MENU_SIGERR_END;
	m_aMenuItems[MENU_SIGERR_END].byChildMenuItems			= 4;
	m_aMenuItems[MENU_SIGERR_END].pChildMenu				= &m_aMenuItems[MENU_SYSTEM_SIGERR];
	m_aMenuItems[MENU_SIGERR_END].pParentMenu				= &m_aMenuItems[MENU_SYSTEM_SIGERR];
	m_aMenuItems[MENU_SIGERR_END].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_SIGERR_END].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_SIGERR_END].OnOpeningHandler			= NULL;	

	// Ser Io Menu
	m_aMenuItems[MENU_SERIO_9600].byMenuItemID				= MENU_SERIO_9600;
	m_aMenuItems[MENU_SERIO_9600].byChildMenuItems			= 7;
	m_aMenuItems[MENU_SERIO_9600].pChildMenu				= NULL;
	m_aMenuItems[MENU_SERIO_9600].pParentMenu				= &m_aMenuItems[MENU_SYSTEM_SERIO];
	m_aMenuItems[MENU_SERIO_9600].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_SERIO_9600].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_SERIO_9600].OnOpeningHandler			= Serio9600_OpeningHandler;

	m_aMenuItems[MENU_SERIO_19200].byMenuItemID				= MENU_SERIO_19200;
	m_aMenuItems[MENU_SERIO_19200].byChildMenuItems			= 7;
	m_aMenuItems[MENU_SERIO_19200].pChildMenu				= NULL;
	m_aMenuItems[MENU_SERIO_19200].pParentMenu				= &m_aMenuItems[MENU_SYSTEM_SERIO];
	m_aMenuItems[MENU_SERIO_19200].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_SERIO_19200].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_SERIO_19200].OnOpeningHandler			= Serio19200_OpeningHandler;

	m_aMenuItems[MENU_SERIO_38400].byMenuItemID				= MENU_SERIO_38400;
	m_aMenuItems[MENU_SERIO_38400].byChildMenuItems			= 7;
	m_aMenuItems[MENU_SERIO_38400].pChildMenu				= NULL;
	m_aMenuItems[MENU_SERIO_38400].pParentMenu				= &m_aMenuItems[MENU_SYSTEM_SERIO];
	m_aMenuItems[MENU_SERIO_38400].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_SERIO_38400].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_SERIO_38400].OnOpeningHandler			= Serio38400_OpeningHandler;

	m_aMenuItems[MENU_SERIO_1200].byMenuItemID				= MENU_SERIO_1200;
	m_aMenuItems[MENU_SERIO_1200].byChildMenuItems			= 7;
	m_aMenuItems[MENU_SERIO_1200].pChildMenu				= NULL;
	m_aMenuItems[MENU_SERIO_1200].pParentMenu				= &m_aMenuItems[MENU_SYSTEM_SERIO];
	m_aMenuItems[MENU_SERIO_1200].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_SERIO_1200].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_SERIO_1200].OnOpeningHandler			= Serio1200_OpeningHandler;

	m_aMenuItems[MENU_SERIO_2400].byMenuItemID				= MENU_SERIO_2400;
	m_aMenuItems[MENU_SERIO_2400].byChildMenuItems			= 7;
	m_aMenuItems[MENU_SERIO_2400].pChildMenu				= NULL;
	m_aMenuItems[MENU_SERIO_2400].pParentMenu				= &m_aMenuItems[MENU_SYSTEM_SERIO];
	m_aMenuItems[MENU_SERIO_2400].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_SERIO_2400].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_SERIO_2400].OnOpeningHandler			= Serio2400_OpeningHandler;

	m_aMenuItems[MENU_SERIO_4800].byMenuItemID				= MENU_SERIO_4800;
	m_aMenuItems[MENU_SERIO_4800].byChildMenuItems			= 7;
	m_aMenuItems[MENU_SERIO_4800].pChildMenu				= NULL;
	m_aMenuItems[MENU_SERIO_4800].pParentMenu				= &m_aMenuItems[MENU_SYSTEM_SERIO];
	m_aMenuItems[MENU_SERIO_4800].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_SERIO_4800].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_SERIO_4800].OnOpeningHandler			= Serio4800_OpeningHandler;

	m_aMenuItems[MENU_SERIO_END].byMenuItemID				= MENU_SERIO_END;
	m_aMenuItems[MENU_SERIO_END].byChildMenuItems			= 7;
	m_aMenuItems[MENU_SERIO_END].pChildMenu					= &m_aMenuItems[MENU_SYSTEM_SERIO];
	m_aMenuItems[MENU_SERIO_END].pParentMenu				= &m_aMenuItems[MENU_SYSTEM_SERIO];
	m_aMenuItems[MENU_SERIO_END].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_SERIO_END].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_SERIO_END].OnOpeningHandler			= NULL;
	
	// Code Menu
	m_aMenuItems[MENU_CODE_KEY].byMenuItemID				= MENU_CODE_KEY;
	m_aMenuItems[MENU_CODE_KEY].byChildMenuItems			= 4;
	m_aMenuItems[MENU_CODE_KEY].pChildMenu					= NULL;
	m_aMenuItems[MENU_CODE_KEY].pParentMenu					= &m_aMenuItems[MENU_SYSTEM_CODE];
	m_aMenuItems[MENU_CODE_KEY].KeyboardHandler				= CodeKey_KeyboardHandler;
	m_aMenuItems[MENU_CODE_KEY].DisplayHandler				= CodeKey_DisplayHandler;
	m_aMenuItems[MENU_CODE_KEY].OnOpeningHandler			= CodeKey_OpeningHandler;

	m_aMenuItems[MENU_CODE_MENU].byMenuItemID				= MENU_CODE_MENU;
	m_aMenuItems[MENU_CODE_MENU].byChildMenuItems			= 4;
	m_aMenuItems[MENU_CODE_MENU].pChildMenu					= NULL;
	m_aMenuItems[MENU_CODE_MENU].pParentMenu				= &m_aMenuItems[MENU_SYSTEM_CODE];
	m_aMenuItems[MENU_CODE_MENU].KeyboardHandler			= CodeMenu_KeyboardHandler;
	m_aMenuItems[MENU_CODE_MENU].DisplayHandler				= CodeMenu_DisplayHandler;
	m_aMenuItems[MENU_CODE_MENU].OnOpeningHandler			= CodeMenu_OpeningHandler;

	m_aMenuItems[MENU_CODE_GLOB].byMenuItemID				= MENU_CODE_GLOB;
	m_aMenuItems[MENU_CODE_GLOB].byChildMenuItems			= 4;
	m_aMenuItems[MENU_CODE_GLOB].pChildMenu					= NULL;
	m_aMenuItems[MENU_CODE_GLOB].pParentMenu				= &m_aMenuItems[MENU_SYSTEM_CODE];
	m_aMenuItems[MENU_CODE_GLOB].KeyboardHandler			= CodeGlob_KeyboardHandler;
	m_aMenuItems[MENU_CODE_GLOB].DisplayHandler				= CodeGlob_DisplayHandler;
	m_aMenuItems[MENU_CODE_GLOB].OnOpeningHandler			= CodeGlob_OpeningHandler;

	m_aMenuItems[MENU_CODE_END].byMenuItemID				= MENU_CODE_END;
	m_aMenuItems[MENU_CODE_END].byChildMenuItems			= 4;
	m_aMenuItems[MENU_CODE_END].pChildMenu					= &m_aMenuItems[MENU_SYSTEM_CODE];
	m_aMenuItems[MENU_CODE_END].pParentMenu					= &m_aMenuItems[MENU_SYSTEM_CODE];
	m_aMenuItems[MENU_CODE_END].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_CODE_END].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_CODE_END].OnOpeningHandler			= NULL;	
	
	// Bin In Menu
	m_aMenuItems[MENU_BININ_SPOS].byMenuItemID				= MENU_BININ_SPOS;
	m_aMenuItems[MENU_BININ_SPOS].byChildMenuItems			= 3;
	m_aMenuItems[MENU_BININ_SPOS].pChildMenu				= &m_aMenuItems[MENU_SPOS_OPN];
	m_aMenuItems[MENU_BININ_SPOS].pParentMenu				= &m_aMenuItems[MENU_CONTRL_BININ];
	m_aMenuItems[MENU_BININ_SPOS].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_BININ_SPOS].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_BININ_SPOS].OnOpeningHandler			= NULL;

	m_aMenuItems[MENU_BININ_MA].byMenuItemID				= MENU_BININ_MA;
	m_aMenuItems[MENU_BININ_MA].byChildMenuItems			= 3;
	m_aMenuItems[MENU_BININ_MA].pChildMenu					= &m_aMenuItems[MENU_MA_OPN];
	m_aMenuItems[MENU_BININ_MA].pParentMenu					= &m_aMenuItems[MENU_CONTRL_BININ];
	m_aMenuItems[MENU_BININ_MA].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_BININ_MA].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_BININ_MA].OnOpeningHandler			= NULL;

	m_aMenuItems[MENU_BININ_END].byMenuItemID				= MENU_BININ_END;
	m_aMenuItems[MENU_BININ_END].byChildMenuItems			= 3;
	m_aMenuItems[MENU_BININ_END].pChildMenu					= &m_aMenuItems[MENU_CONTRL_BININ];
	m_aMenuItems[MENU_BININ_END].pParentMenu				= &m_aMenuItems[MENU_CONTRL_BININ];
	m_aMenuItems[MENU_BININ_END].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_BININ_END].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_BININ_END].OnOpeningHandler			= NULL;
	
	// Output Menu
	m_aMenuItems[MENU_OUTPUT_ANL].byMenuItemID				= MENU_OUTPUT_ANL;
	m_aMenuItems[MENU_OUTPUT_ANL].byChildMenuItems			= 5;
	m_aMenuItems[MENU_OUTPUT_ANL].pChildMenu				= &m_aMenuItems[MENU_ANL_POS];
	m_aMenuItems[MENU_OUTPUT_ANL].pParentMenu				= &m_aMenuItems[MENU_CONTRL_OUTPUT];
	m_aMenuItems[MENU_OUTPUT_ANL].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_OUTPUT_ANL].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_OUTPUT_ANL].OnOpeningHandler			= NULL;

	m_aMenuItems[MENU_OUTPUT_ANLSET].byMenuItemID			= MENU_OUTPUT_ANLSET;
	m_aMenuItems[MENU_OUTPUT_ANLSET].byChildMenuItems		= 5;
	m_aMenuItems[MENU_OUTPUT_ANLSET].pChildMenu				= &m_aMenuItems[MENU_ANLSET_4];
	m_aMenuItems[MENU_OUTPUT_ANLSET].pParentMenu			= &m_aMenuItems[MENU_CONTRL_OUTPUT];
	m_aMenuItems[MENU_OUTPUT_ANLSET].KeyboardHandler		= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_OUTPUT_ANLSET].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_OUTPUT_ANLSET].OnOpeningHandler		= NULL;

	m_aMenuItems[MENU_OUTPUT_BIN1].byMenuItemID				= MENU_OUTPUT_BIN1;
	m_aMenuItems[MENU_OUTPUT_BIN1].byChildMenuItems			= 5;
	m_aMenuItems[MENU_OUTPUT_BIN1].pChildMenu				= &m_aMenuItems[MENU_BIN1_DRV];
	m_aMenuItems[MENU_OUTPUT_BIN1].pParentMenu				= &m_aMenuItems[MENU_CONTRL_OUTPUT];
	m_aMenuItems[MENU_OUTPUT_BIN1].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_OUTPUT_BIN1].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_OUTPUT_BIN1].OnOpeningHandler			= NULL;

	m_aMenuItems[MENU_OUTPUT_BIN2].byMenuItemID				= MENU_OUTPUT_BIN2;
	m_aMenuItems[MENU_OUTPUT_BIN2].byChildMenuItems			= 5;
	m_aMenuItems[MENU_OUTPUT_BIN2].pChildMenu				= &m_aMenuItems[MENU_BIN2_DRV];
	m_aMenuItems[MENU_OUTPUT_BIN2].pParentMenu				= &m_aMenuItems[MENU_CONTRL_OUTPUT];
	m_aMenuItems[MENU_OUTPUT_BIN2].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_OUTPUT_BIN2].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_OUTPUT_BIN2].OnOpeningHandler			= NULL;

	m_aMenuItems[MENU_OUTPUT_END].byMenuItemID				= MENU_OUTPUT_END;
	m_aMenuItems[MENU_OUTPUT_END].byChildMenuItems			= 5;
	m_aMenuItems[MENU_OUTPUT_END].pChildMenu				= &m_aMenuItems[MENU_CONTRL_OUTPUT];
	m_aMenuItems[MENU_OUTPUT_END].pParentMenu				= &m_aMenuItems[MENU_CONTRL_OUTPUT];
	m_aMenuItems[MENU_OUTPUT_END].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_OUTPUT_END].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_OUTPUT_END].OnOpeningHandler			= NULL;	
	
	// Cal User Menu
	m_aMenuItems[MENU_CALUSER_POS].byMenuItemID				= MENU_CALUSER_POS;
	m_aMenuItems[MENU_CALUSER_POS].byChildMenuItems			= 4;
	m_aMenuItems[MENU_CALUSER_POS].pChildMenu				= NULL;
	m_aMenuItems[MENU_CALUSER_POS].pParentMenu				= &m_aMenuItems[MENU_CONTRL_CALUSER];
	m_aMenuItems[MENU_CALUSER_POS].KeyboardHandler			= CaluserPos_KeyboardHandler;
	m_aMenuItems[MENU_CALUSER_POS].DisplayHandler			= CaluserPos_DisplayHandler;
	m_aMenuItems[MENU_CALUSER_POS].OnOpeningHandler			= CaluserPos_OpeningHandler;

	m_aMenuItems[MENU_CALUSER_INP].byMenuItemID				= MENU_CALUSER_INP;
	m_aMenuItems[MENU_CALUSER_INP].byChildMenuItems			= 4;
	m_aMenuItems[MENU_CALUSER_INP].pChildMenu				= NULL;
	m_aMenuItems[MENU_CALUSER_INP].pParentMenu				= &m_aMenuItems[MENU_CONTRL_CALUSER];
	m_aMenuItems[MENU_CALUSER_INP].KeyboardHandler			= CaluserInp_KeyboardHandler;
	m_aMenuItems[MENU_CALUSER_INP].DisplayHandler			= CaluserInp_DisplayHandler;
	m_aMenuItems[MENU_CALUSER_INP].OnOpeningHandler			= CaluserInp_OpeningHandler;

	m_aMenuItems[MENU_CALUSER_FACT].byMenuItemID			= MENU_CALUSER_FACT;
	m_aMenuItems[MENU_CALUSER_FACT].byChildMenuItems		= 4;
	m_aMenuItems[MENU_CALUSER_FACT].pChildMenu				= NULL;
	m_aMenuItems[MENU_CALUSER_FACT].pParentMenu				= &m_aMenuItems[MENU_CONTRL_CALUSER];
	m_aMenuItems[MENU_CALUSER_FACT].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_CALUSER_FACT].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_CALUSER_FACT].OnOpeningHandler		= CaluserFact_OpeningHandler;

	m_aMenuItems[MENU_CALUSER_END].byMenuItemID				= MENU_CALUSER_END;
	m_aMenuItems[MENU_CALUSER_END].byChildMenuItems			= 4;
	m_aMenuItems[MENU_CALUSER_END].pChildMenu				= &m_aMenuItems[MENU_CONTRL_CALUSER];
	m_aMenuItems[MENU_CALUSER_END].pParentMenu				= &m_aMenuItems[MENU_CONTRL_CALUSER];
	m_aMenuItems[MENU_CALUSER_END].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_CALUSER_END].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_CALUSER_END].OnOpeningHandler			= NULL;	
	
	// P Contrl Menu
	m_aMenuItems[MENU_PCONTRL_DBND].byMenuItemID			= MENU_PCONTRL_DBND;
	m_aMenuItems[MENU_PCONTRL_DBND].byChildMenuItems		= 10;
	m_aMenuItems[MENU_PCONTRL_DBND].pChildMenu				= NULL;
	m_aMenuItems[MENU_PCONTRL_DBND].pParentMenu				= &m_aMenuItems[MENU_CONTRL_PCONTRL];
	m_aMenuItems[MENU_PCONTRL_DBND].KeyboardHandler			= PcontrlDbnd_KeyboardHandler;
	m_aMenuItems[MENU_PCONTRL_DBND].DisplayHandler			= PcontrlDbnd_DisplayHandler;
	m_aMenuItems[MENU_PCONTRL_DBND].OnOpeningHandler		= PcontrlDbnd_OpeningHandler;

	m_aMenuItems[MENU_PCONTRL_PARA].byMenuItemID			= MENU_PCONTRL_PARA;
	m_aMenuItems[MENU_PCONTRL_PARA].byChildMenuItems		= 10;
	m_aMenuItems[MENU_PCONTRL_PARA].pChildMenu				= NULL;
	m_aMenuItems[MENU_PCONTRL_PARA].pParentMenu				= &m_aMenuItems[MENU_CONTRL_PCONTRL];
	m_aMenuItems[MENU_PCONTRL_PARA].KeyboardHandler			= PcontrlPara_KeyboardHandler;
	m_aMenuItems[MENU_PCONTRL_PARA].DisplayHandler			= PcontrlPara_DisplayHandler;
	m_aMenuItems[MENU_PCONTRL_PARA].OnOpeningHandler		= PcontrlPara_OpeningHandler;

	m_aMenuItems[MENU_PCONTRL_SETP].byMenuItemID			= MENU_PCONTRL_SETP;
	m_aMenuItems[MENU_PCONTRL_SETP].byChildMenuItems		= 10;
	m_aMenuItems[MENU_PCONTRL_SETP].pChildMenu				= &m_aMenuItems[MENU_SETP_INT];
	m_aMenuItems[MENU_PCONTRL_SETP].pParentMenu				= &m_aMenuItems[MENU_CONTRL_PCONTRL];
	m_aMenuItems[MENU_PCONTRL_SETP].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_PCONTRL_SETP].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_PCONTRL_SETP].OnOpeningHandler		= NULL;

	m_aMenuItems[MENU_PCONTRL_INP].byMenuItemID				= MENU_PCONTRL_INP;
	m_aMenuItems[MENU_PCONTRL_INP].byChildMenuItems			= 10;
	m_aMenuItems[MENU_PCONTRL_INP].pChildMenu				= &m_aMenuItems[MENU_INP_FREQ];
	m_aMenuItems[MENU_PCONTRL_INP].pParentMenu				= &m_aMenuItems[MENU_CONTRL_PCONTRL];
	m_aMenuItems[MENU_PCONTRL_INP].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_PCONTRL_INP].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_PCONTRL_INP].OnOpeningHandler			= NULL;

	m_aMenuItems[MENU_PCONTRL_FILT].byMenuItemID			= MENU_PCONTRL_FILT;
	m_aMenuItems[MENU_PCONTRL_FILT].byChildMenuItems		= 10;
	m_aMenuItems[MENU_PCONTRL_FILT].pChildMenu				= NULL;
	m_aMenuItems[MENU_PCONTRL_FILT].pParentMenu				= &m_aMenuItems[MENU_CONTRL_PCONTRL];
	m_aMenuItems[MENU_PCONTRL_FILT].KeyboardHandler			= PcontrlFilt_KeyboardHandler;
	m_aMenuItems[MENU_PCONTRL_FILT].DisplayHandler			= PcontrlFilt_DisplayHandler;
	m_aMenuItems[MENU_PCONTRL_FILT].OnOpeningHandler		= PcontrlFilt_OpeningHandler;

	m_aMenuItems[MENU_PCONTRL_SCAL].byMenuItemID			= MENU_PCONTRL_SCAL;
	m_aMenuItems[MENU_PCONTRL_SCAL].byChildMenuItems		= 10;
	m_aMenuItems[MENU_PCONTRL_SCAL].pChildMenu				= &m_aMenuItems[MENU_SCAL_END];
	m_aMenuItems[MENU_PCONTRL_SCAL].pParentMenu				= &m_aMenuItems[MENU_CONTRL_PCONTRL];
	m_aMenuItems[MENU_PCONTRL_SCAL].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_PCONTRL_SCAL].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_PCONTRL_SCAL].OnOpeningHandler		= NULL;

	m_aMenuItems[MENU_PCONTRL_TUNE].byMenuItemID			= MENU_PCONTRL_TUNE;
	m_aMenuItems[MENU_PCONTRL_TUNE].byChildMenuItems		= 10;
	m_aMenuItems[MENU_PCONTRL_TUNE].pChildMenu				= &m_aMenuItems[MENU_TUNE_NOT];
	m_aMenuItems[MENU_PCONTRL_TUNE].pParentMenu				= &m_aMenuItems[MENU_CONTRL_PCONTRL];
	m_aMenuItems[MENU_PCONTRL_TUNE].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_PCONTRL_TUNE].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_PCONTRL_TUNE].OnOpeningHandler		= NULL;

	m_aMenuItems[MENU_PCONTRL_KV].byMenuItemID				= MENU_PCONTRL_KV;
	m_aMenuItems[MENU_PCONTRL_KV].byChildMenuItems			= 10;
	m_aMenuItems[MENU_PCONTRL_KV].pChildMenu				= NULL;
	m_aMenuItems[MENU_PCONTRL_KV].pParentMenu				= &m_aMenuItems[MENU_CONTRL_PCONTRL];
	m_aMenuItems[MENU_PCONTRL_KV].KeyboardHandler			= PcontrlKv_KeyboardHandler;
	m_aMenuItems[MENU_PCONTRL_KV].DisplayHandler			= PcontrlKv_DisplayHandler;
	m_aMenuItems[MENU_PCONTRL_KV].OnOpeningHandler			= PcontrlKv_OpeningHandler;

	m_aMenuItems[MENU_PCONTRL_LEAK].byMenuItemID			= MENU_PCONTRL_LEAK;
	m_aMenuItems[MENU_PCONTRL_LEAK].byChildMenuItems		= 10;
	m_aMenuItems[MENU_PCONTRL_LEAK].pChildMenu				= &m_aMenuItems[MENU_LEAK_NOT];
	m_aMenuItems[MENU_PCONTRL_LEAK].pParentMenu				= &m_aMenuItems[MENU_CONTRL_PCONTRL];
	m_aMenuItems[MENU_PCONTRL_LEAK].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_PCONTRL_LEAK].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_PCONTRL_LEAK].OnOpeningHandler		= NULL;

	m_aMenuItems[MENU_PCONTRL_END].byMenuItemID				= MENU_PCONTRL_END;
	m_aMenuItems[MENU_PCONTRL_END].byChildMenuItems			= 10;
	m_aMenuItems[MENU_PCONTRL_END].pChildMenu				= &m_aMenuItems[MENU_CONTRL_PCONTRL];
	m_aMenuItems[MENU_PCONTRL_END].pParentMenu				= &m_aMenuItems[MENU_CONTRL_PCONTRL];
	m_aMenuItems[MENU_PCONTRL_END].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_PCONTRL_END].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_PCONTRL_END].OnOpeningHandler			= NULL;	

	// S Pos In Menu
	m_aMenuItems[MENU_SPOS_OPN].byMenuItemID				= MENU_SPOS_OPN;
	m_aMenuItems[MENU_SPOS_OPN].byChildMenuItems			= 3;
	m_aMenuItems[MENU_SPOS_OPN].pChildMenu					= NULL;
	m_aMenuItems[MENU_SPOS_OPN].pParentMenu					= &m_aMenuItems[MENU_BININ_SPOS];
	m_aMenuItems[MENU_SPOS_OPN].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_SPOS_OPN].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_SPOS_OPN].OnOpeningHandler			= SposOpn_OpeningHandler;

	m_aMenuItems[MENU_SPOS_CLS].byMenuItemID				= MENU_SPOS_CLS;
	m_aMenuItems[MENU_SPOS_CLS].byChildMenuItems			= 3;
	m_aMenuItems[MENU_SPOS_CLS].pChildMenu					= NULL;
	m_aMenuItems[MENU_SPOS_CLS].pParentMenu					= &m_aMenuItems[MENU_BININ_SPOS];
	m_aMenuItems[MENU_SPOS_CLS].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_SPOS_CLS].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_SPOS_CLS].OnOpeningHandler			= SposCls_OpeningHandler;

	m_aMenuItems[MENU_SPOS_END].byMenuItemID				= MENU_SPOS_END;
	m_aMenuItems[MENU_SPOS_END].byChildMenuItems			= 3;
	m_aMenuItems[MENU_SPOS_END].pChildMenu					= &m_aMenuItems[MENU_BININ_SPOS];
	m_aMenuItems[MENU_SPOS_END].pParentMenu					= &m_aMenuItems[MENU_BININ_SPOS];
	m_aMenuItems[MENU_SPOS_END].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_SPOS_END].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_SPOS_END].OnOpeningHandler			= NULL;

	// M A Menu
	m_aMenuItems[MENU_MA_OPN].byMenuItemID					= MENU_MA_OPN;
	m_aMenuItems[MENU_MA_OPN].byChildMenuItems				= 3;
	m_aMenuItems[MENU_MA_OPN].pChildMenu					= NULL;
	m_aMenuItems[MENU_MA_OPN].pParentMenu					= &m_aMenuItems[MENU_BININ_MA];
	m_aMenuItems[MENU_MA_OPN].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_MA_OPN].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_MA_OPN].OnOpeningHandler				= MaOpn_OpeningHandler;

	m_aMenuItems[MENU_MA_CLS].byMenuItemID					= MENU_MA_CLS;
	m_aMenuItems[MENU_MA_CLS].byChildMenuItems				= 3;
	m_aMenuItems[MENU_MA_CLS].pChildMenu					= NULL;
	m_aMenuItems[MENU_MA_CLS].pParentMenu					= &m_aMenuItems[MENU_BININ_MA];
	m_aMenuItems[MENU_MA_CLS].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_MA_CLS].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_MA_CLS].OnOpeningHandler				= MaCls_OpeningHandler;

	m_aMenuItems[MENU_MA_END].byMenuItemID					= MENU_MA_END;
	m_aMenuItems[MENU_MA_END].byChildMenuItems				= 3;
	m_aMenuItems[MENU_MA_END].pChildMenu					= &m_aMenuItems[MENU_BININ_MA];
	m_aMenuItems[MENU_MA_END].pParentMenu					= &m_aMenuItems[MENU_BININ_MA];
	m_aMenuItems[MENU_MA_END].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_MA_END].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_MA_END].OnOpeningHandler				= NULL;

	// Anl Menu
	m_aMenuItems[MENU_ANL_POS].byMenuItemID					= MENU_ANL_POS;
	m_aMenuItems[MENU_ANL_POS].byChildMenuItems				= 7;
	m_aMenuItems[MENU_ANL_POS].pChildMenu					= NULL;
	m_aMenuItems[MENU_ANL_POS].pParentMenu					= &m_aMenuItems[MENU_OUTPUT_ANL];
	m_aMenuItems[MENU_ANL_POS].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_ANL_POS].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_ANL_POS].OnOpeningHandler				= AnlPos_OpeningHandler;

	m_aMenuItems[MENU_ANL_CMD].byMenuItemID					= MENU_ANL_CMD;
	m_aMenuItems[MENU_ANL_CMD].byChildMenuItems				= 7;
	m_aMenuItems[MENU_ANL_CMD].pChildMenu					= NULL;
	m_aMenuItems[MENU_ANL_CMD].pParentMenu					= &m_aMenuItems[MENU_OUTPUT_ANL];
	m_aMenuItems[MENU_ANL_CMD].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_ANL_CMD].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_ANL_CMD].OnOpeningHandler				= AnlCmd_OpeningHandler;

	m_aMenuItems[MENU_ANL_SP].byMenuItemID					= MENU_ANL_SP;
	m_aMenuItems[MENU_ANL_SP].byChildMenuItems				= 7;
	m_aMenuItems[MENU_ANL_SP].pChildMenu					= NULL;
	m_aMenuItems[MENU_ANL_SP].pParentMenu					= &m_aMenuItems[MENU_OUTPUT_ANL];
	m_aMenuItems[MENU_ANL_SP].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_ANL_SP].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_ANL_SP].OnOpeningHandler				= AnlSp_OpeningHandler;

	m_aMenuItems[MENU_ANL_P1].byMenuItemID					= MENU_ANL_P1;
	m_aMenuItems[MENU_ANL_P1].byChildMenuItems				= 7;
	m_aMenuItems[MENU_ANL_P1].pChildMenu					= NULL;
	m_aMenuItems[MENU_ANL_P1].pParentMenu					= &m_aMenuItems[MENU_OUTPUT_ANL];
	m_aMenuItems[MENU_ANL_P1].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_ANL_P1].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_ANL_P1].OnOpeningHandler				= AnlP1_OpeningHandler;

	m_aMenuItems[MENU_ANL_P2].byMenuItemID					= MENU_ANL_P2;
	m_aMenuItems[MENU_ANL_P2].byChildMenuItems				= 7;
	m_aMenuItems[MENU_ANL_P2].pChildMenu					= NULL;
	m_aMenuItems[MENU_ANL_P2].pParentMenu					= &m_aMenuItems[MENU_OUTPUT_ANL];
	m_aMenuItems[MENU_ANL_P2].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_ANL_P2].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_ANL_P2].OnOpeningHandler				= AnlP2_OpeningHandler;

	m_aMenuItems[MENU_ANL_T].byMenuItemID					= MENU_ANL_T;
	m_aMenuItems[MENU_ANL_T].byChildMenuItems				= 7;
	m_aMenuItems[MENU_ANL_T].pChildMenu						= NULL;
	m_aMenuItems[MENU_ANL_T].pParentMenu					= &m_aMenuItems[MENU_OUTPUT_ANL];
	m_aMenuItems[MENU_ANL_T].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_ANL_T].DisplayHandler					= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_ANL_T].OnOpeningHandler				= AnlT_OpeningHandler;

	m_aMenuItems[MENU_ANL_END].byMenuItemID					= MENU_ANL_END;
	m_aMenuItems[MENU_ANL_END].byChildMenuItems				= 7;
	m_aMenuItems[MENU_ANL_END].pChildMenu					= &m_aMenuItems[MENU_OUTPUT_ANL];
	m_aMenuItems[MENU_ANL_END].pParentMenu					= &m_aMenuItems[MENU_OUTPUT_ANL];
	m_aMenuItems[MENU_ANL_END].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_ANL_END].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_ANL_END].OnOpeningHandler				= NULL;

	// Anl Set Menu
	m_aMenuItems[MENU_ANLSET_4].byMenuItemID				= MENU_ANLSET_4;
	m_aMenuItems[MENU_ANLSET_4].byChildMenuItems			= 5;
	m_aMenuItems[MENU_ANLSET_4].pChildMenu					= NULL;
	m_aMenuItems[MENU_ANLSET_4].pParentMenu					= &m_aMenuItems[MENU_OUTPUT_ANLSET];
	m_aMenuItems[MENU_ANLSET_4].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_ANLSET_4].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_ANLSET_4].OnOpeningHandler			= AnlSet4_OpeningHandler;

	m_aMenuItems[MENU_ANLSET_0].byMenuItemID				= MENU_ANLSET_0;
	m_aMenuItems[MENU_ANLSET_0].byChildMenuItems			= 5;
	m_aMenuItems[MENU_ANLSET_0].pChildMenu					= NULL;
	m_aMenuItems[MENU_ANLSET_0].pParentMenu					= &m_aMenuItems[MENU_OUTPUT_ANLSET];
	m_aMenuItems[MENU_ANLSET_0].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_ANLSET_0].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_ANLSET_0].OnOpeningHandler			= AnlSet0_OpeningHandler;

	m_aMenuItems[MENU_ANLSET_10].byMenuItemID				= MENU_ANLSET_10;
	m_aMenuItems[MENU_ANLSET_10].byChildMenuItems			= 5;
	m_aMenuItems[MENU_ANLSET_10].pChildMenu					= NULL;
	m_aMenuItems[MENU_ANLSET_10].pParentMenu				= &m_aMenuItems[MENU_OUTPUT_ANLSET];
	m_aMenuItems[MENU_ANLSET_10].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_ANLSET_10].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_ANLSET_10].OnOpeningHandler			= AnlSet10_OpeningHandler;

	m_aMenuItems[MENU_ANLSET_5].byMenuItemID				= MENU_ANLSET_5;
	m_aMenuItems[MENU_ANLSET_5].byChildMenuItems			= 5;
	m_aMenuItems[MENU_ANLSET_5].pChildMenu					= NULL;
	m_aMenuItems[MENU_ANLSET_5].pParentMenu					= &m_aMenuItems[MENU_OUTPUT_ANLSET];
	m_aMenuItems[MENU_ANLSET_5].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_ANLSET_5].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_ANLSET_5].OnOpeningHandler			= AnlSet5_OpeningHandler;

	m_aMenuItems[MENU_ANLSET_END].byMenuItemID				= MENU_ANLSET_END;
	m_aMenuItems[MENU_ANLSET_END].byChildMenuItems			= 5;
	m_aMenuItems[MENU_ANLSET_END].pChildMenu				= &m_aMenuItems[MENU_OUTPUT_ANLSET];
	m_aMenuItems[MENU_ANLSET_END].pParentMenu				= &m_aMenuItems[MENU_OUTPUT_ANLSET];
	m_aMenuItems[MENU_ANLSET_END].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_ANLSET_END].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_ANLSET_END].OnOpeningHandler			= NULL;

	// Bin1 Menu
	m_aMenuItems[MENU_BIN1_DRV].byMenuItemID				= MENU_BIN1_DRV;
	m_aMenuItems[MENU_BIN1_DRV].byChildMenuItems			= 11;
	m_aMenuItems[MENU_BIN1_DRV].pChildMenu					= NULL;
	m_aMenuItems[MENU_BIN1_DRV].pParentMenu					= &m_aMenuItems[MENU_OUTPUT_BIN1];
	m_aMenuItems[MENU_BIN1_DRV].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_BIN1_DRV].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_BIN1_DRV].OnOpeningHandler			= ActSngl_OpeningHandler;

	m_aMenuItems[MENU_BIN1_LIM].byMenuItemID				= MENU_BIN1_LIM;
	m_aMenuItems[MENU_BIN1_LIM].byChildMenuItems			= 11;
	m_aMenuItems[MENU_BIN1_LIM].pChildMenu					= NULL;
	m_aMenuItems[MENU_BIN1_LIM].pParentMenu					= &m_aMenuItems[MENU_OUTPUT_BIN1];
	m_aMenuItems[MENU_BIN1_LIM].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_BIN1_LIM].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_BIN1_LIM].OnOpeningHandler			= ActSngl_OpeningHandler;

	m_aMenuItems[MENU_BIN1_SPOS].byMenuItemID				= MENU_BIN1_SPOS;
	m_aMenuItems[MENU_BIN1_SPOS].byChildMenuItems			= 11;
	m_aMenuItems[MENU_BIN1_SPOS].pChildMenu					= NULL;
	m_aMenuItems[MENU_BIN1_SPOS].pParentMenu				= &m_aMenuItems[MENU_OUTPUT_BIN1];
	m_aMenuItems[MENU_BIN1_SPOS].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_BIN1_SPOS].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_BIN1_SPOS].OnOpeningHandler			= ActSngl_OpeningHandler;

	m_aMenuItems[MENU_BIN1_ERRS].byMenuItemID				= MENU_BIN1_ERRS;
	m_aMenuItems[MENU_BIN1_ERRS].byChildMenuItems			= 11;
	m_aMenuItems[MENU_BIN1_ERRS].pChildMenu					= NULL;
	m_aMenuItems[MENU_BIN1_ERRS].pParentMenu				= &m_aMenuItems[MENU_OUTPUT_BIN1];
	m_aMenuItems[MENU_BIN1_ERRS].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_BIN1_ERRS].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_BIN1_ERRS].OnOpeningHandler			= ActSngl_OpeningHandler;

	m_aMenuItems[MENU_BIN1_ERR1].byMenuItemID				= MENU_BIN1_ERR1;
	m_aMenuItems[MENU_BIN1_ERR1].byChildMenuItems			= 11;
	m_aMenuItems[MENU_BIN1_ERR1].pChildMenu					= NULL;
	m_aMenuItems[MENU_BIN1_ERR1].pParentMenu				= &m_aMenuItems[MENU_OUTPUT_BIN1];
	m_aMenuItems[MENU_BIN1_ERR1].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_BIN1_ERR1].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_BIN1_ERR1].OnOpeningHandler			= ActSngl_OpeningHandler;

	m_aMenuItems[MENU_BIN1_ERR2].byMenuItemID				= MENU_BIN1_ERR2;
	m_aMenuItems[MENU_BIN1_ERR2].byChildMenuItems			= 11;
	m_aMenuItems[MENU_BIN1_ERR2].pChildMenu					= NULL;
	m_aMenuItems[MENU_BIN1_ERR2].pParentMenu				= &m_aMenuItems[MENU_OUTPUT_BIN1];
	m_aMenuItems[MENU_BIN1_ERR2].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_BIN1_ERR2].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_BIN1_ERR2].OnOpeningHandler			= ActSngl_OpeningHandler;

	m_aMenuItems[MENU_BIN1_ERRT].byMenuItemID				= MENU_BIN1_ERRT;
	m_aMenuItems[MENU_BIN1_ERRT].byChildMenuItems			= 11;
	m_aMenuItems[MENU_BIN1_ERRT].pChildMenu					= NULL;
	m_aMenuItems[MENU_BIN1_ERRT].pParentMenu				= &m_aMenuItems[MENU_OUTPUT_BIN1];
	m_aMenuItems[MENU_BIN1_ERRT].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_BIN1_ERRT].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_BIN1_ERRT].OnOpeningHandler			= ActSngl_OpeningHandler;

	m_aMenuItems[MENU_BIN1_OFF].byMenuItemID				= MENU_BIN1_OFF;
	m_aMenuItems[MENU_BIN1_OFF].byChildMenuItems			= 11;
	m_aMenuItems[MENU_BIN1_OFF].pChildMenu					= NULL;
	m_aMenuItems[MENU_BIN1_OFF].pParentMenu					= &m_aMenuItems[MENU_OUTPUT_BIN1];
	m_aMenuItems[MENU_BIN1_OFF].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_BIN1_OFF].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_BIN1_OFF].OnOpeningHandler			= ActSngl_OpeningHandler;

	m_aMenuItems[MENU_BIN1_OPN].byMenuItemID				= MENU_BIN1_OPN;
	m_aMenuItems[MENU_BIN1_OPN].byChildMenuItems			= 11;
	m_aMenuItems[MENU_BIN1_OPN].pChildMenu					= NULL;
	m_aMenuItems[MENU_BIN1_OPN].pParentMenu					= &m_aMenuItems[MENU_OUTPUT_BIN1];
	m_aMenuItems[MENU_BIN1_OPN].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_BIN1_OPN].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_BIN1_OPN].OnOpeningHandler			= ActSngl_OpeningHandler;

	m_aMenuItems[MENU_BIN1_CLS].byMenuItemID				= MENU_BIN1_CLS;
	m_aMenuItems[MENU_BIN1_CLS].byChildMenuItems			= 11;
	m_aMenuItems[MENU_BIN1_CLS].pChildMenu					= NULL;
	m_aMenuItems[MENU_BIN1_CLS].pParentMenu					= &m_aMenuItems[MENU_OUTPUT_BIN1];
	m_aMenuItems[MENU_BIN1_CLS].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_BIN1_CLS].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_BIN1_CLS].OnOpeningHandler			= ActSngl_OpeningHandler;

	m_aMenuItems[MENU_BIN1_END].byMenuItemID				= MENU_BIN1_END;
	m_aMenuItems[MENU_BIN1_END].byChildMenuItems			= 11;
	m_aMenuItems[MENU_BIN1_END].pChildMenu					= &m_aMenuItems[MENU_OUTPUT_BIN1];
	m_aMenuItems[MENU_BIN1_END].pParentMenu					= &m_aMenuItems[MENU_OUTPUT_BIN1];
	m_aMenuItems[MENU_BIN1_END].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_BIN1_END].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_BIN1_END].OnOpeningHandler			= NULL;

	// Bin2 Menu
	m_aMenuItems[MENU_BIN2_DRV].byMenuItemID				= MENU_BIN2_DRV;
	m_aMenuItems[MENU_BIN2_DRV].byChildMenuItems			= 11;
	m_aMenuItems[MENU_BIN2_DRV].pChildMenu					= NULL;
	m_aMenuItems[MENU_BIN2_DRV].pParentMenu					= &m_aMenuItems[MENU_OUTPUT_BIN2];
	m_aMenuItems[MENU_BIN2_DRV].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_BIN2_DRV].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_BIN2_DRV].OnOpeningHandler			= ActSngl_OpeningHandler;

	m_aMenuItems[MENU_BIN2_LIM].byMenuItemID				= MENU_BIN2_LIM;
	m_aMenuItems[MENU_BIN2_LIM].byChildMenuItems			= 11;
	m_aMenuItems[MENU_BIN2_LIM].pChildMenu					= NULL;
	m_aMenuItems[MENU_BIN2_LIM].pParentMenu					= &m_aMenuItems[MENU_OUTPUT_BIN2];
	m_aMenuItems[MENU_BIN2_LIM].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_BIN2_LIM].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_BIN2_LIM].OnOpeningHandler			= ActSngl_OpeningHandler;

	m_aMenuItems[MENU_BIN2_SPOS].byMenuItemID				= MENU_BIN2_SPOS;
	m_aMenuItems[MENU_BIN2_SPOS].byChildMenuItems			= 11;
	m_aMenuItems[MENU_BIN2_SPOS].pChildMenu					= NULL;
	m_aMenuItems[MENU_BIN2_SPOS].pParentMenu				= &m_aMenuItems[MENU_OUTPUT_BIN2];
	m_aMenuItems[MENU_BIN2_SPOS].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_BIN2_SPOS].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_BIN2_SPOS].OnOpeningHandler			= ActSngl_OpeningHandler;

	m_aMenuItems[MENU_BIN2_ERRS].byMenuItemID				= MENU_BIN2_ERRS;
	m_aMenuItems[MENU_BIN2_ERRS].byChildMenuItems			= 11;
	m_aMenuItems[MENU_BIN2_ERRS].pChildMenu					= NULL;
	m_aMenuItems[MENU_BIN2_ERRS].pParentMenu				= &m_aMenuItems[MENU_OUTPUT_BIN2];
	m_aMenuItems[MENU_BIN2_ERRS].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_BIN2_ERRS].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_BIN2_ERRS].OnOpeningHandler			= ActSngl_OpeningHandler;

	m_aMenuItems[MENU_BIN2_ERR1].byMenuItemID				= MENU_BIN2_ERR1;
	m_aMenuItems[MENU_BIN2_ERR1].byChildMenuItems			= 11;
	m_aMenuItems[MENU_BIN2_ERR1].pChildMenu					= NULL;
	m_aMenuItems[MENU_BIN2_ERR1].pParentMenu				= &m_aMenuItems[MENU_OUTPUT_BIN2];
	m_aMenuItems[MENU_BIN2_ERR1].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_BIN2_ERR1].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_BIN2_ERR1].OnOpeningHandler			= ActSngl_OpeningHandler;

	m_aMenuItems[MENU_BIN2_ERR2].byMenuItemID				= MENU_BIN2_ERR2;
	m_aMenuItems[MENU_BIN2_ERR2].byChildMenuItems			= 11;
	m_aMenuItems[MENU_BIN2_ERR2].pChildMenu					= NULL;
	m_aMenuItems[MENU_BIN2_ERR2].pParentMenu				= &m_aMenuItems[MENU_OUTPUT_BIN2];
	m_aMenuItems[MENU_BIN2_ERR2].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_BIN2_ERR2].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_BIN2_ERR2].OnOpeningHandler			= ActSngl_OpeningHandler;

	m_aMenuItems[MENU_BIN2_ERRT].byMenuItemID				= MENU_BIN2_ERRT;
	m_aMenuItems[MENU_BIN2_ERRT].byChildMenuItems			= 11;
	m_aMenuItems[MENU_BIN2_ERRT].pChildMenu					= NULL;
	m_aMenuItems[MENU_BIN2_ERRT].pParentMenu				= &m_aMenuItems[MENU_OUTPUT_BIN2];
	m_aMenuItems[MENU_BIN2_ERRT].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_BIN2_ERRT].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_BIN2_ERRT].OnOpeningHandler			= ActSngl_OpeningHandler;

	m_aMenuItems[MENU_BIN2_OFF].byMenuItemID				= MENU_BIN2_OFF;
	m_aMenuItems[MENU_BIN2_OFF].byChildMenuItems			= 11;
	m_aMenuItems[MENU_BIN2_OFF].pChildMenu					= NULL;
	m_aMenuItems[MENU_BIN2_OFF].pParentMenu					= &m_aMenuItems[MENU_OUTPUT_BIN2];
	m_aMenuItems[MENU_BIN2_OFF].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_BIN2_OFF].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_BIN2_OFF].OnOpeningHandler			= ActSngl_OpeningHandler;

	m_aMenuItems[MENU_BIN2_OPN].byMenuItemID				= MENU_BIN2_OPN;
	m_aMenuItems[MENU_BIN2_OPN].byChildMenuItems			= 11;
	m_aMenuItems[MENU_BIN2_OPN].pChildMenu					= NULL;
	m_aMenuItems[MENU_BIN2_OPN].pParentMenu					= &m_aMenuItems[MENU_OUTPUT_BIN2];
	m_aMenuItems[MENU_BIN2_OPN].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_BIN2_OPN].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_BIN2_OPN].OnOpeningHandler			= ActSngl_OpeningHandler;

	m_aMenuItems[MENU_BIN2_CLS].byMenuItemID				= MENU_BIN2_CLS;
	m_aMenuItems[MENU_BIN2_CLS].byChildMenuItems			= 11;
	m_aMenuItems[MENU_BIN2_CLS].pChildMenu					= NULL;
	m_aMenuItems[MENU_BIN2_CLS].pParentMenu					= &m_aMenuItems[MENU_OUTPUT_BIN2];
	m_aMenuItems[MENU_BIN2_CLS].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_BIN2_CLS].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_BIN2_CLS].OnOpeningHandler			= ActSngl_OpeningHandler;

	m_aMenuItems[MENU_BIN2_END].byMenuItemID				= MENU_BIN2_END;
	m_aMenuItems[MENU_BIN2_END].byChildMenuItems			= 11;
	m_aMenuItems[MENU_BIN2_END].pChildMenu					= &m_aMenuItems[MENU_OUTPUT_BIN2];
	m_aMenuItems[MENU_BIN2_END].pParentMenu					= &m_aMenuItems[MENU_OUTPUT_BIN2];
	m_aMenuItems[MENU_BIN2_END].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_BIN2_END].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_BIN2_END].OnOpeningHandler			= NULL;

	// Setp Menu
	m_aMenuItems[MENU_SETP_INT].byMenuItemID				= MENU_SETP_INT;
	m_aMenuItems[MENU_SETP_INT].byChildMenuItems			= 3;
	m_aMenuItems[MENU_SETP_INT].pChildMenu					= NULL;
	m_aMenuItems[MENU_SETP_INT].pParentMenu					= &m_aMenuItems[MENU_PCONTRL_SETP];
	m_aMenuItems[MENU_SETP_INT].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_SETP_INT].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_SETP_INT].OnOpeningHandler			= ActSngl_OpeningHandler;

	m_aMenuItems[MENU_SETP_EXT].byMenuItemID				= MENU_SETP_EXT;
	m_aMenuItems[MENU_SETP_EXT].byChildMenuItems			= 3;
	m_aMenuItems[MENU_SETP_EXT].pChildMenu					= NULL;
	m_aMenuItems[MENU_SETP_EXT].pParentMenu					= &m_aMenuItems[MENU_PCONTRL_SETP];
	m_aMenuItems[MENU_SETP_EXT].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_SETP_EXT].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_SETP_EXT].OnOpeningHandler			= ActSngl_OpeningHandler;

	m_aMenuItems[MENU_SETP_END].byMenuItemID				= MENU_SETP_END;
	m_aMenuItems[MENU_SETP_END].byChildMenuItems			= 3;
	m_aMenuItems[MENU_SETP_END].pChildMenu					= &m_aMenuItems[MENU_PCONTRL_SETP];
	m_aMenuItems[MENU_SETP_END].pParentMenu					= &m_aMenuItems[MENU_PCONTRL_SETP];
	m_aMenuItems[MENU_SETP_END].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_SETP_END].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_SETP_END].OnOpeningHandler			= NULL;

	// Inp Menu
	m_aMenuItems[MENU_INP_FREQ].byMenuItemID				= MENU_INP_FREQ;
	m_aMenuItems[MENU_INP_FREQ].byChildMenuItems			= 5;
	m_aMenuItems[MENU_INP_FREQ].pChildMenu					= NULL;
	m_aMenuItems[MENU_INP_FREQ].pParentMenu					= &m_aMenuItems[MENU_PCONTRL_INP];
	m_aMenuItems[MENU_INP_FREQ].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_INP_FREQ].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_INP_FREQ].OnOpeningHandler			= ActSngl_OpeningHandler;

	m_aMenuItems[MENU_INP_PT100].byMenuItemID				= MENU_INP_PT100;
	m_aMenuItems[MENU_INP_PT100].byChildMenuItems			= 5;
	m_aMenuItems[MENU_INP_PT100].pChildMenu					= NULL;
	m_aMenuItems[MENU_INP_PT100].pParentMenu				= &m_aMenuItems[MENU_PCONTRL_INP];
	m_aMenuItems[MENU_INP_PT100].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_INP_PT100].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_INP_PT100].OnOpeningHandler			= ActSngl_OpeningHandler;

	m_aMenuItems[MENU_INP_P1P2].byMenuItemID				= MENU_INP_P1P2;
	m_aMenuItems[MENU_INP_P1P2].byChildMenuItems			= 5;
	m_aMenuItems[MENU_INP_P1P2].pChildMenu					= NULL;
	m_aMenuItems[MENU_INP_P1P2].pParentMenu					= &m_aMenuItems[MENU_PCONTRL_INP];
	m_aMenuItems[MENU_INP_P1P2].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_INP_P1P2].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_INP_P1P2].OnOpeningHandler			= ActSngl_OpeningHandler;

	m_aMenuItems[MENU_INP_20MA].byMenuItemID				= MENU_INP_20MA;
	m_aMenuItems[MENU_INP_20MA].byChildMenuItems			= 5;
	m_aMenuItems[MENU_INP_20MA].pChildMenu					= NULL;
	m_aMenuItems[MENU_INP_20MA].pParentMenu					= &m_aMenuItems[MENU_PCONTRL_INP];
	m_aMenuItems[MENU_INP_20MA].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_INP_20MA].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_INP_20MA].OnOpeningHandler			= ActSngl_OpeningHandler;

	m_aMenuItems[MENU_INP_END].byMenuItemID					= MENU_INP_END;
	m_aMenuItems[MENU_INP_END].byChildMenuItems				= 5;
	m_aMenuItems[MENU_INP_END].pChildMenu					= &m_aMenuItems[MENU_PCONTRL_INP];
	m_aMenuItems[MENU_INP_END].pParentMenu					= &m_aMenuItems[MENU_PCONTRL_INP];
	m_aMenuItems[MENU_INP_END].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_INP_END].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_INP_END].OnOpeningHandler				= NULL;

	// Inp Menu
	m_aMenuItems[MENU_SCAL_S].byMenuItemID					= MENU_SCAL_S;
	m_aMenuItems[MENU_SCAL_S].byChildMenuItems				= 5;
	m_aMenuItems[MENU_SCAL_S].pChildMenu					= NULL;
	m_aMenuItems[MENU_SCAL_S].pParentMenu					= &m_aMenuItems[MENU_PCONTRL_SCAL];
	m_aMenuItems[MENU_SCAL_S].KeyboardHandler				= ScalS_KeyboardHandler;
	m_aMenuItems[MENU_SCAL_S].DisplayHandler				= ScalS_DisplayHandler;
	m_aMenuItems[MENU_SCAL_S].OnOpeningHandler				= ScalS_OpeningHandler;

	m_aMenuItems[MENU_SCAL_F].byMenuItemID					= MENU_SCAL_F;
	m_aMenuItems[MENU_SCAL_F].byChildMenuItems				= 5;
	m_aMenuItems[MENU_SCAL_F].pChildMenu					= NULL;
	m_aMenuItems[MENU_SCAL_F].pParentMenu					= &m_aMenuItems[MENU_PCONTRL_SCAL];
	m_aMenuItems[MENU_SCAL_F].KeyboardHandler				= ScalF_KeyboardHandler;
	m_aMenuItems[MENU_SCAL_F].DisplayHandler				= ScalF_DisplayHandler;
	m_aMenuItems[MENU_SCAL_F].OnOpeningHandler				= ScalF_OpeningHandler;

	m_aMenuItems[MENU_SCAL_T].byMenuItemID					= MENU_SCAL_T;
	m_aMenuItems[MENU_SCAL_T].byChildMenuItems				= 5;
	m_aMenuItems[MENU_SCAL_T].pChildMenu					= NULL;
	m_aMenuItems[MENU_SCAL_T].pParentMenu					= &m_aMenuItems[MENU_PCONTRL_SCAL];
	m_aMenuItems[MENU_SCAL_T].KeyboardHandler				= ScalT_KeyboardHandler;
	m_aMenuItems[MENU_SCAL_T].DisplayHandler				= ScalT_DisplayHandler;
	m_aMenuItems[MENU_SCAL_T].OnOpeningHandler				= ScalT_OpeningHandler;

	m_aMenuItems[MENU_SCAL_P].byMenuItemID					= MENU_SCAL_P;
	m_aMenuItems[MENU_SCAL_P].byChildMenuItems				= 5;
	m_aMenuItems[MENU_SCAL_P].pChildMenu					= NULL;
	m_aMenuItems[MENU_SCAL_P].pParentMenu					= &m_aMenuItems[MENU_PCONTRL_SCAL];
	m_aMenuItems[MENU_SCAL_P].KeyboardHandler				= ScalP_KeyboardHandler;
	m_aMenuItems[MENU_SCAL_P].DisplayHandler				= ScalP_DisplayHandler;
	m_aMenuItems[MENU_SCAL_P].OnOpeningHandler				= ScalP_OpeningHandler;

	m_aMenuItems[MENU_SCAL_END].byMenuItemID				= MENU_SCAL_END;
	m_aMenuItems[MENU_SCAL_END].byChildMenuItems			= 5;
	m_aMenuItems[MENU_SCAL_END].pChildMenu					= &m_aMenuItems[MENU_PCONTRL_SCAL];
	m_aMenuItems[MENU_SCAL_END].pParentMenu					= &m_aMenuItems[MENU_PCONTRL_SCAL];
	m_aMenuItems[MENU_SCAL_END].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_SCAL_END].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_SCAL_END].OnOpeningHandler			= NULL;

	// Tune Menu
	m_aMenuItems[MENU_TUNE_NOT].byMenuItemID				= MENU_TUNE_NOT;
	m_aMenuItems[MENU_TUNE_NOT].byChildMenuItems			= 7;
	m_aMenuItems[MENU_TUNE_NOT].pChildMenu					= NULL;
	m_aMenuItems[MENU_TUNE_NOT].pParentMenu					= &m_aMenuItems[MENU_PCONTRL_TUNE];
	m_aMenuItems[MENU_TUNE_NOT].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_TUNE_NOT].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_TUNE_NOT].OnOpeningHandler			= TuneNot_OpeningHandler;

	m_aMenuItems[MENU_TUNE_DEF].byMenuItemID				= MENU_TUNE_DEF;
	m_aMenuItems[MENU_TUNE_DEF].byChildMenuItems			= 7;
	m_aMenuItems[MENU_TUNE_DEF].pChildMenu					= NULL;
	m_aMenuItems[MENU_TUNE_DEF].pParentMenu					= &m_aMenuItems[MENU_PCONTRL_TUNE];
	m_aMenuItems[MENU_TUNE_DEF].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_TUNE_DEF].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_TUNE_DEF].OnOpeningHandler			= TuneDef_OpeningHandler;

	m_aMenuItems[MENU_TUNE_FLOW].byMenuItemID				= MENU_TUNE_FLOW;
	m_aMenuItems[MENU_TUNE_FLOW].byChildMenuItems			= 7;
	m_aMenuItems[MENU_TUNE_FLOW].pChildMenu					= NULL;
	m_aMenuItems[MENU_TUNE_FLOW].pParentMenu				= &m_aMenuItems[MENU_PCONTRL_TUNE];
	m_aMenuItems[MENU_TUNE_FLOW].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_TUNE_FLOW].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_TUNE_FLOW].OnOpeningHandler			= TuneFlow_OpeningHandler;

	m_aMenuItems[MENU_TUNE_TEMP].byMenuItemID				= MENU_TUNE_TEMP;
	m_aMenuItems[MENU_TUNE_TEMP].byChildMenuItems			= 7;
	m_aMenuItems[MENU_TUNE_TEMP].pChildMenu					= NULL;
	m_aMenuItems[MENU_TUNE_TEMP].pParentMenu				= &m_aMenuItems[MENU_PCONTRL_TUNE];
	m_aMenuItems[MENU_TUNE_TEMP].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_TUNE_TEMP].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_TUNE_TEMP].OnOpeningHandler			= TuneTemp_OpeningHandler;

	m_aMenuItems[MENU_TUNE_PRES].byMenuItemID				= MENU_TUNE_PRES;
	m_aMenuItems[MENU_TUNE_PRES].byChildMenuItems			= 7;
	m_aMenuItems[MENU_TUNE_PRES].pChildMenu					= NULL;
	m_aMenuItems[MENU_TUNE_PRES].pParentMenu				= &m_aMenuItems[MENU_PCONTRL_TUNE];
	m_aMenuItems[MENU_TUNE_PRES].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_TUNE_PRES].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_TUNE_PRES].OnOpeningHandler			= TunePres_OpeningHandler;

	m_aMenuItems[MENU_TUNE_LEVL].byMenuItemID				= MENU_TUNE_LEVL;
	m_aMenuItems[MENU_TUNE_LEVL].byChildMenuItems			= 7;
	m_aMenuItems[MENU_TUNE_LEVL].pChildMenu					= NULL;
	m_aMenuItems[MENU_TUNE_LEVL].pParentMenu				= &m_aMenuItems[MENU_PCONTRL_TUNE];
	m_aMenuItems[MENU_TUNE_LEVL].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_TUNE_LEVL].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_TUNE_LEVL].OnOpeningHandler			= TuneLevl_OpeningHandler;

	m_aMenuItems[MENU_TUNE_END].byMenuItemID				= MENU_TUNE_END;
	m_aMenuItems[MENU_TUNE_END].byChildMenuItems			= 7;
	m_aMenuItems[MENU_TUNE_END].pChildMenu					= &m_aMenuItems[MENU_PCONTRL_TUNE];
	m_aMenuItems[MENU_TUNE_END].pParentMenu					= &m_aMenuItems[MENU_PCONTRL_TUNE];
	m_aMenuItems[MENU_TUNE_END].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_TUNE_END].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_TUNE_END].OnOpeningHandler			= NULL;

	// Leak Menu
	m_aMenuItems[MENU_LEAK_NOT].byMenuItemID				= MENU_LEAK_NOT;
	m_aMenuItems[MENU_LEAK_NOT].byChildMenuItems			= 4;
	m_aMenuItems[MENU_LEAK_NOT].pChildMenu					= NULL;
	m_aMenuItems[MENU_LEAK_NOT].pParentMenu					= &m_aMenuItems[MENU_PCONTRL_LEAK];
	m_aMenuItems[MENU_LEAK_NOT].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_LEAK_NOT].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_LEAK_NOT].OnOpeningHandler			= LeakNot_OpeningHandler;

	m_aMenuItems[MENU_LEAK_MEAS].byMenuItemID				= MENU_LEAK_MEAS;
	m_aMenuItems[MENU_LEAK_MEAS].byChildMenuItems			= 4;
	m_aMenuItems[MENU_LEAK_MEAS].pChildMenu					= NULL;
	m_aMenuItems[MENU_LEAK_MEAS].pParentMenu				= &m_aMenuItems[MENU_PCONTRL_LEAK];
	m_aMenuItems[MENU_LEAK_MEAS].KeyboardHandler			= LeakMeas_KeyboardHandler;
	m_aMenuItems[MENU_LEAK_MEAS].DisplayHandler				= LeakMeas_DisplayHandler;
	m_aMenuItems[MENU_LEAK_MEAS].OnOpeningHandler			= LeakMeas_OpeningHandler;

	m_aMenuItems[MENU_LEAK_CHAR].byMenuItemID				= MENU_LEAK_CHAR;
	m_aMenuItems[MENU_LEAK_CHAR].byChildMenuItems			= 4;
	m_aMenuItems[MENU_LEAK_CHAR].pChildMenu					= NULL;
	m_aMenuItems[MENU_LEAK_CHAR].pParentMenu				= &m_aMenuItems[MENU_PCONTRL_LEAK];
	m_aMenuItems[MENU_LEAK_CHAR].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_LEAK_CHAR].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_LEAK_CHAR].OnOpeningHandler			= LeakChar_OpeningHandler;

	m_aMenuItems[MENU_LEAK_END].byMenuItemID				= MENU_LEAK_END;
	m_aMenuItems[MENU_LEAK_END].byChildMenuItems			= 4;
	m_aMenuItems[MENU_LEAK_END].pChildMenu					= &m_aMenuItems[MENU_PCONTRL_LEAK];
	m_aMenuItems[MENU_LEAK_END].pParentMenu					= &m_aMenuItems[MENU_PCONTRL_LEAK];
	m_aMenuItems[MENU_LEAK_END].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_LEAK_END].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_LEAK_END].OnOpeningHandler			= NULL;

	m_mcbCurrent.pMenu 										= NULL;
	m_mcbCurrent.byStartMenuItemID							= 0;

	S_INIT(m_stackMenuCtlBlock, &m_mcbCurrent, 1);

	clearLCD();
}

void HMI_Handler()
{
	CheckKeyboard();
	Display();
}
