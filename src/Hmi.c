/************************************************************************************************

 Author        : Yu

 Date          : 2011.4

 File          : 
                                      
 Hardware      : ADuC7060

 Description   : 
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
typedef unsigned int	        WORD;       /* Prefix: w	*/

// KEY0 : PORT3.1
// KEY1 : PORT3.2
// KEY2 : PORT3.3

#define KEYS					3
#define IS_KEY0_DOWN()			((GP3DAT & BIT01) == 0)
#define IS_KEY1_DOWN()			((GP3DAT & BIT02) == 0)
#define IS_KEY2_DOWN()			((GP3DAT & BIT03) == 0)

#define MENU_ITEMS				37
//#define KB_BUF_SIZE				16

const BYTE * display[] =	{
							"ACT[FUNC",
							"[[[INPUT",
							"[[IN[SET",
							"[[[X[SET",
							"[[SYSTEM",
							"[[CONTRL",
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
							"[X[LIMIT",
							"[[X[TIME",
							"X[CONTRL",
							"[[X[TUNE",
							"[XSETEND",
							"[SAFEPOS",
							"[[SIGERR",
							"[[SER[IO",
							"[[[[CODE",
							"[SETFACT",
							"[SYS[END",
							"[[BIN[IN",
							"[[OUTPUT",
							"CAL[USER",
							"P[CONTRL",
							"CTRL[END",
						  	};

extern UNIT_DATA                g_UnitData;

DEFINE_STACK(MENU_CTL_BLOCK, BYTE, STACK_MCB)

MENU_ITEM						m_aMenuItems[MENU_ITEMS];
MENU_CTL_BLOCK					m_mcbCurrent;
STACK_MCB						m_stackMenuCtlBlock;

// Indicate which page to be displayed
static BYTE						m_byPageNo					= 0;
//static BYTE						m_byCursorPos				= 0;
//static BYTE						m_bufKeyboard[KB_BUF_SIZE];

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
	MENU_XSET_XLIMIT,
	MENU_XSET_XTIME,
	MENU_XSET_XCONTRL,
	MENU_XSET_XTUNE,
	MENU_XSET_END,
	// System Menu
	MENU_SYSTEM_SAFEPOS,
	MENU_SYSTEM_SIGERR,
	MENU_SYSTEM_SERIO,
	MENU_SYSTEM_CODE,
	MENU_SYSTEM_SETFACT,
	MENU_SYSTEM_END,
	// Contrl Menu
	MENU_CONTRL_BININ,
	MENU_CONTRL_OUTPUT,
	MENU_CONTRL_CALUSER,
	MENU_CONTRL_PCONTRL,
	MENU_CONTRL_END,
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
			if (m_byPageNo > 1)
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
			floattochar (g_UnitData.dat.fTem, byNum,1);
			display_digital(byNum,1,0);
			display_char("[[[[TEMP");
		}
		if (m_byPageNo == 1)
		{
			floattochar (nVal, byNum,0);
			display_digital(byNum,0,0);
			display_char("[[[[TICK");
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
			if (_cnts[i] >= 5)
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
	BYTE	byMenuID = m_mcbCurrent.byStartMenuItemID;

	display_digital("::::::",0,0);
	display_char((unsigned char *)display[byMenuID]);
}

static bool IsRetMenuItem(MENU_ITEM* pMenuItem)
{
	return (pMenuItem->byMenuItemID == MENU_MAIN_END);
}

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
//static void ZeroConfig_KeyboardHandler(BYTE byKeyCode)
//{
//	if (byKeyCode == KEY_1)
//	{
//		if (m_byCursorPos > 0)
//		{
//			m_byCursorPos--;
//		}
//	}
//	else if (byKeyCode == KEY_2)
//	{
//		if (m_byCursorPos < 2)
//		{
//			m_byCursorPos++;
//		}
//		else
//		{
//			m_mcbCurrent = S_PEEK(m_stackMenuCtlBlock);
//			S_POP(m_stackMenuCtlBlock);
//		}
//	}
//	else if (byKeyCode == KEY_3)
//	{
//		if (m_byCursorPos < 2)
//		{
//			m_bufKeyboard[m_byCursorPos]++;
//			if (m_bufKeyboard[m_byCursorPos] > '9')
//				m_bufKeyboard[m_byCursorPos] = '0';
//		}
//		else
//		{
//			m_mcbCurrent = S_PEEK(m_stackMenuCtlBlock);
//			S_POP(m_stackMenuCtlBlock);
//		}
//	}
//}
//
//static void ZeroConfig_DisplayHandler()
//{
//	BYTE byNum[6]="0";
//	BYTE byStr[10]="XYZABCCC";
//	float nVal;
//
//	nVal = 0;
//	floattochar (nVal, byNum,0);
//	display_char(byStr);
//	display_digital(byNum,3,1);
//}
//
//static void ZeroConfig_OpeningHandler()
//{
//	m_byCursorPos = 0;
//
//	m_bufKeyboard[0] = '0';
//	m_bufKeyboard[1] = '0';
//	m_bufKeyboard[2] = '#';
//	m_bufKeyboard[3] = '\0';
//}

//static void DispConfig_KeyboardHandler(BYTE byKeyCode)
//{
//	if (byKeyCode == KEY_1)
//	{
//		m_bufKeyboard[0]++;
//		if (m_bufKeyboard[0] > '5')
//		{
//			m_bufKeyboard[0] = '0';
//		}
//	}
//	else if (byKeyCode == KEY_2)
//	{
//		m_mcbCurrent = S_PEEK(m_stackMenuCtlBlock);
//		S_POP(m_stackMenuCtlBlock);
//	}
//	else if (byKeyCode == KEY_3)
//	{
//		m_mcbCurrent = S_PEEK(m_stackMenuCtlBlock);
//		S_POP(m_stackMenuCtlBlock);
//	}
//}
//
//static void DispConfig_DisplayHandler()
//{
//	BYTE byNum[6]="0";
//	BYTE byStr[10]="ACEGIKMO";
//	float nVal;
//
//	nVal = 0;
//	clearLCD();
//	floattochar (nVal, byNum,0);
//	display_char(byStr);
//	display_digital(byNum,3,1);
//}
//
//static void DispConfig_OpeningHandler()
//{
////	m_byCursorPos = 0;
//
//	m_bufKeyboard[0] = '0';
//	m_bufKeyboard[1] = '\0';
//}

void HMI_Init()
{
	initLCD_1622();
	// Enable internal pull-up resister
	GP3CON	&= ~0x00003330;

	// Init Main Menu
	m_aMenuItems[MENU_MAIN_ACT].byMenuItemID				= MENU_MAIN_ACT;
	m_aMenuItems[MENU_MAIN_ACT].byChildMenuItems			= 7;
	m_aMenuItems[MENU_MAIN_ACT].pChildMenu					= &m_aMenuItems[MENU_ACT_SNGL];
	m_aMenuItems[MENU_MAIN_ACT].pParentMenu					= NULL;
	m_aMenuItems[MENU_MAIN_ACT].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_MAIN_ACT].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_MAIN_ACT].OnOpeningHandler			= NULL;

	m_aMenuItems[MENU_MAIN_INPUT].byMenuItemID				= MENU_MAIN_INPUT;
	m_aMenuItems[MENU_MAIN_INPUT].byChildMenuItems			= 7;
	m_aMenuItems[MENU_MAIN_INPUT].pChildMenu				= &m_aMenuItems[MENU_INPUT_4];
	m_aMenuItems[MENU_MAIN_INPUT].pParentMenu				= NULL;
	m_aMenuItems[MENU_MAIN_INPUT].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_MAIN_INPUT].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_MAIN_INPUT].OnOpeningHandler			= NULL;

	m_aMenuItems[MENU_MAIN_INSET].byMenuItemID				= MENU_MAIN_INSET;
	m_aMenuItems[MENU_MAIN_INSET].byChildMenuItems			= 7;
	m_aMenuItems[MENU_MAIN_INSET].pChildMenu				= &m_aMenuItems[MENU_INSET_CHARACT];
	m_aMenuItems[MENU_MAIN_INSET].pParentMenu				= NULL;//&m_aMenuItems[MENU_MAIN];
	m_aMenuItems[MENU_MAIN_INSET].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_MAIN_INSET].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_MAIN_INSET].OnOpeningHandler			= NULL;

	m_aMenuItems[MENU_MAIN_XSET].byMenuItemID				= MENU_MAIN_XSET;
	m_aMenuItems[MENU_MAIN_XSET].byChildMenuItems			= 7;
	m_aMenuItems[MENU_MAIN_XSET].pChildMenu					= &m_aMenuItems[MENU_XSET_XLIMIT];
	m_aMenuItems[MENU_MAIN_XSET].pParentMenu				= NULL;
	m_aMenuItems[MENU_MAIN_XSET].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_MAIN_XSET].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_MAIN_XSET].OnOpeningHandler			= NULL; 
	
	m_aMenuItems[MENU_MAIN_SYSTEM].byMenuItemID				= MENU_MAIN_SYSTEM;
	m_aMenuItems[MENU_MAIN_SYSTEM].byChildMenuItems			= 7;
	m_aMenuItems[MENU_MAIN_SYSTEM].pChildMenu				= &m_aMenuItems[MENU_SYSTEM_SAFEPOS];
	m_aMenuItems[MENU_MAIN_SYSTEM].pParentMenu				= NULL;
	m_aMenuItems[MENU_MAIN_SYSTEM].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_MAIN_SYSTEM].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_MAIN_SYSTEM].OnOpeningHandler			= NULL;

	m_aMenuItems[MENU_MAIN_CONTRL].byMenuItemID				= MENU_MAIN_CONTRL;
	m_aMenuItems[MENU_MAIN_CONTRL].byChildMenuItems			= 7;
	m_aMenuItems[MENU_MAIN_CONTRL].pChildMenu				= &m_aMenuItems[MENU_CONTRL_BININ];
	m_aMenuItems[MENU_MAIN_CONTRL].pParentMenu				= NULL;
	m_aMenuItems[MENU_MAIN_CONTRL].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_MAIN_CONTRL].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_MAIN_CONTRL].OnOpeningHandler			= NULL;

	m_aMenuItems[MENU_MAIN_END].byMenuItemID				= MENU_MAIN_END;
	m_aMenuItems[MENU_MAIN_END].byChildMenuItems			= 7;
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
	m_aMenuItems[MENU_INPUT_4].OnOpeningHandler				= NULL;

	m_aMenuItems[MENU_INPUT_0].byMenuItemID					= MENU_INPUT_0;
	m_aMenuItems[MENU_INPUT_0].byChildMenuItems				= 5;
	m_aMenuItems[MENU_INPUT_0].pChildMenu					= NULL;
	m_aMenuItems[MENU_INPUT_0].pParentMenu					= &m_aMenuItems[MENU_MAIN_INPUT];
	m_aMenuItems[MENU_INPUT_0].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_INPUT_0].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_INPUT_0].OnOpeningHandler				= NULL;

	m_aMenuItems[MENU_INPUT_10].byMenuItemID				= MENU_INPUT_10;
	m_aMenuItems[MENU_INPUT_10].byChildMenuItems			= 5;
	m_aMenuItems[MENU_INPUT_10].pChildMenu					= NULL;
	m_aMenuItems[MENU_INPUT_10].pParentMenu					= &m_aMenuItems[MENU_MAIN_INPUT];
	m_aMenuItems[MENU_INPUT_10].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_INPUT_10].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_INPUT_10].OnOpeningHandler			= NULL;

	m_aMenuItems[MENU_INPUT_5].byMenuItemID					= MENU_INPUT_5;
	m_aMenuItems[MENU_INPUT_5].byChildMenuItems				= 5;
	m_aMenuItems[MENU_INPUT_5].pChildMenu					= NULL;
	m_aMenuItems[MENU_INPUT_5].pParentMenu					= &m_aMenuItems[MENU_MAIN_INPUT];
	m_aMenuItems[MENU_INPUT_5].KeyboardHandler				= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_INPUT_5].DisplayHandler				= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_INPUT_5].OnOpeningHandler				= NULL;

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
	m_aMenuItems[MENU_INSET_CHARACT].pChildMenu				= NULL;
	m_aMenuItems[MENU_INSET_CHARACT].pParentMenu			= &m_aMenuItems[MENU_MAIN_INSET];
	m_aMenuItems[MENU_INSET_CHARACT].KeyboardHandler		= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_INSET_CHARACT].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_INSET_CHARACT].OnOpeningHandler		= NULL;

	m_aMenuItems[MENU_INSET_CUTOFF].byMenuItemID			= MENU_INSET_CUTOFF;
	m_aMenuItems[MENU_INSET_CUTOFF].byChildMenuItems		= 6;
	m_aMenuItems[MENU_INSET_CUTOFF].pChildMenu				= NULL;
	m_aMenuItems[MENU_INSET_CUTOFF].pParentMenu				= &m_aMenuItems[MENU_MAIN_INSET];
	m_aMenuItems[MENU_INSET_CUTOFF].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_INSET_CUTOFF].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_INSET_CUTOFF].OnOpeningHandler		= NULL;

	m_aMenuItems[MENU_INSET_DIRCMD].byMenuItemID			= MENU_INSET_DIRCMD;
	m_aMenuItems[MENU_INSET_DIRCMD].byChildMenuItems		= 6;
	m_aMenuItems[MENU_INSET_DIRCMD].pChildMenu				= NULL;
	m_aMenuItems[MENU_INSET_DIRCMD].pParentMenu				= &m_aMenuItems[MENU_MAIN_INSET];
	m_aMenuItems[MENU_INSET_DIRCMD].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_INSET_DIRCMD].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_INSET_DIRCMD].OnOpeningHandler		= NULL;

	m_aMenuItems[MENU_INSET_DIRACT].byMenuItemID			= MENU_INPUT_5;
	m_aMenuItems[MENU_INSET_DIRACT].byChildMenuItems		= 6;
	m_aMenuItems[MENU_INSET_DIRACT].pChildMenu				= NULL;
	m_aMenuItems[MENU_INSET_DIRACT].pParentMenu				= &m_aMenuItems[MENU_MAIN_INSET];
	m_aMenuItems[MENU_INSET_DIRACT].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_INSET_DIRACT].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_INSET_DIRACT].OnOpeningHandler		= NULL;

	m_aMenuItems[MENU_INSET_SPLTRNG].byMenuItemID			= MENU_INSET_SPLTRNG;
	m_aMenuItems[MENU_INSET_SPLTRNG].byChildMenuItems		= 6;
	m_aMenuItems[MENU_INSET_SPLTRNG].pChildMenu				= NULL;
	m_aMenuItems[MENU_INSET_SPLTRNG].pParentMenu			= &m_aMenuItems[MENU_MAIN_INSET];
	m_aMenuItems[MENU_INSET_SPLTRNG].KeyboardHandler		= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_INSET_SPLTRNG].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_INSET_SPLTRNG].OnOpeningHandler		= NULL;

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
	m_aMenuItems[MENU_XSET_XLIMIT].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_XSET_XLIMIT].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_XSET_XLIMIT].OnOpeningHandler			= NULL;

	m_aMenuItems[MENU_XSET_XTIME].byMenuItemID				= MENU_XSET_XTIME;
	m_aMenuItems[MENU_XSET_XTIME].byChildMenuItems			= 5;
	m_aMenuItems[MENU_XSET_XTIME].pChildMenu				= NULL;
	m_aMenuItems[MENU_XSET_XTIME].pParentMenu				= &m_aMenuItems[MENU_MAIN_XSET];
	m_aMenuItems[MENU_XSET_XTIME].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_XSET_XTIME].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_XSET_XTIME].OnOpeningHandler			= NULL;
														
	m_aMenuItems[MENU_XSET_XCONTRL].byMenuItemID			= MENU_XSET_XCONTRL;
	m_aMenuItems[MENU_XSET_XCONTRL].byChildMenuItems		= 5;
	m_aMenuItems[MENU_XSET_XCONTRL].pChildMenu				= NULL;
	m_aMenuItems[MENU_XSET_XCONTRL].pParentMenu				= &m_aMenuItems[MENU_MAIN_XSET];
	m_aMenuItems[MENU_XSET_XCONTRL].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_XSET_XCONTRL].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_XSET_XCONTRL].OnOpeningHandler		= NULL;

	m_aMenuItems[MENU_XSET_XTUNE].byMenuItemID				= MENU_XSET_XTUNE;
	m_aMenuItems[MENU_XSET_XTUNE].byChildMenuItems			= 5;
	m_aMenuItems[MENU_XSET_XTUNE].pChildMenu				= NULL;
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
	m_aMenuItems[MENU_SYSTEM_SAFEPOS].KeyboardHandler		= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_SYSTEM_SAFEPOS].DisplayHandler		= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_SYSTEM_SAFEPOS].OnOpeningHandler		= NULL;

	m_aMenuItems[MENU_SYSTEM_SIGERR].byMenuItemID			= MENU_SYSTEM_SIGERR;
	m_aMenuItems[MENU_SYSTEM_SIGERR].byChildMenuItems		= 6;
	m_aMenuItems[MENU_SYSTEM_SIGERR].pChildMenu				= NULL;
	m_aMenuItems[MENU_SYSTEM_SIGERR].pParentMenu			= &m_aMenuItems[MENU_MAIN_SYSTEM];
	m_aMenuItems[MENU_SYSTEM_SIGERR].KeyboardHandler		= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_SYSTEM_SIGERR].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_SYSTEM_SIGERR].OnOpeningHandler		= NULL;

	m_aMenuItems[MENU_SYSTEM_SERIO].byMenuItemID			= MENU_SYSTEM_SERIO;
	m_aMenuItems[MENU_SYSTEM_SERIO].byChildMenuItems		= 6;
	m_aMenuItems[MENU_SYSTEM_SERIO].pChildMenu				= NULL;
	m_aMenuItems[MENU_SYSTEM_SERIO].pParentMenu				= &m_aMenuItems[MENU_MAIN_SYSTEM];
	m_aMenuItems[MENU_SYSTEM_SERIO].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_SYSTEM_SERIO].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_SYSTEM_SERIO].OnOpeningHandler		= NULL;

	m_aMenuItems[MENU_SYSTEM_CODE].byMenuItemID				= MENU_SYSTEM_CODE;
	m_aMenuItems[MENU_SYSTEM_CODE].byChildMenuItems			= 6;
	m_aMenuItems[MENU_SYSTEM_CODE].pChildMenu				= NULL;
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
	m_aMenuItems[MENU_SYSTEM_SETFACT].OnOpeningHandler		= NULL;

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
	m_aMenuItems[MENU_CONTRL_BININ].pChildMenu				= NULL;
	m_aMenuItems[MENU_CONTRL_BININ].pParentMenu				= &m_aMenuItems[MENU_MAIN_CONTRL];
	m_aMenuItems[MENU_CONTRL_BININ].KeyboardHandler			= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_CONTRL_BININ].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_CONTRL_BININ].OnOpeningHandler		= NULL;

	m_aMenuItems[MENU_CONTRL_OUTPUT].byMenuItemID			= MENU_CONTRL_OUTPUT;
	m_aMenuItems[MENU_CONTRL_OUTPUT].byChildMenuItems		= 5;
	m_aMenuItems[MENU_CONTRL_OUTPUT].pChildMenu				= NULL;
	m_aMenuItems[MENU_CONTRL_OUTPUT].pParentMenu			= &m_aMenuItems[MENU_MAIN_CONTRL];
	m_aMenuItems[MENU_CONTRL_OUTPUT].KeyboardHandler		= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_CONTRL_OUTPUT].DisplayHandler			= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_CONTRL_OUTPUT].OnOpeningHandler		= NULL;
														
	m_aMenuItems[MENU_CONTRL_CALUSER].byMenuItemID			= MENU_CONTRL_CALUSER;
	m_aMenuItems[MENU_CONTRL_CALUSER].byChildMenuItems		= 5;
	m_aMenuItems[MENU_CONTRL_CALUSER].pChildMenu			= NULL;
	m_aMenuItems[MENU_CONTRL_CALUSER].pParentMenu			= &m_aMenuItems[MENU_MAIN_CONTRL];
	m_aMenuItems[MENU_CONTRL_CALUSER].KeyboardHandler		= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_CONTRL_CALUSER].DisplayHandler		= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_CONTRL_CALUSER].OnOpeningHandler		= NULL;

	m_aMenuItems[MENU_CONTRL_PCONTRL].byMenuItemID			= MENU_CONTRL_PCONTRL;
	m_aMenuItems[MENU_CONTRL_PCONTRL].byChildMenuItems		= 5;
	m_aMenuItems[MENU_CONTRL_PCONTRL].pChildMenu			= NULL;
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
