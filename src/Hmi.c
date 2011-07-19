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

#include "ds18b20.h"

typedef unsigned char           BYTE;       /* Prefix: by	*/
typedef unsigned int	        WORD;       /* Prefix: w	*/

// KEY0 : PORT3.1
// KEY1 : PORT3.2
// KEY2 : PORT3.3

#define KEYS					3
#define IS_KEY0_DOWN()			((GP3DAT & BIT09) == 0)
#define IS_KEY1_DOWN()			((GP3DAT & BIT10) == 0)
#define IS_KEY2_DOWN()			((GP3DAT & BIT09) == 0)

#define MENU_ITEMS				6
#define KB_BUF_SIZE				16


DEFINE_STACK(MENU_CTL_BLOCK, BYTE, STACK_MCB)

MENU_ITEM						m_aMenuItems[MENU_ITEMS];

MENU_CTL_BLOCK					m_mcbCurrent;
MENU_CTL_BLOCK					m_aMenuCtlBlocks[4];
STACK_MCB						m_stackMenuCtlBlock;

// Indicate which page to be displayed
static BYTE						m_byPageNo					= 0;
static BYTE						m_byCursorPos				= 0;
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
	MENU_MAIN,
	MENU_ITEM_ZERO_CONFIG,
	MENU_ITEM_SPAN_CONFIG,
	MENU_ITEM_DAMP_CONFIG,
	MENU_ITEM_DISP_CONFIG,
	MENU_ITEM_FIX_CONFIG,
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
			if (m_byPageNo > 0)
			{
				m_byPageNo = 0;
			}
		}
		else if (byKeyCode == KEY_3)
		{
			S_PUSH(m_stackMenuCtlBlock, m_mcbCurrent);

			m_mcbCurrent.pMenu						= &m_aMenuItems[MENU_MAIN];
			m_mcbCurrent.byStartMenuItemID			= m_mcbCurrent.pMenu->byMenuItemID + 1;
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
//	char byStr[10]="ACEGIKMO";
	static float nVal;

	if (nVal > 99999)
		nVal = 0;
	nVal++;

	if (m_mcbCurrent.pMenu == NULL)
	{
		if (m_byPageNo == 0)
		{
//			nVal = 0;
			clearLCD();
			floattochar (DS18B20_Temperature(), byNum,1);
//			display_char(byStr);
			display_digital(byNum,1,3);
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
			if (_cnts[i] >= 10)
			{
				OnKeyDown(i);
			}
			_cnts[i]++;
		}
		else
		{
			if (_cnts[i] >= 10)
			{
				OnKeyUp(i);
			}
			_cnts[i] = 0;
		}
	}
}

static void DisplayMenu()
{
//	BYTE	byMenuItemsDisplayed;
//
//	byMenuItemsDisplayed = m_mcbCurrent.pMenu->byChildMenuItems;

	BYTE	byMenuID = m_mcbCurrent.byStartMenuItemID;
//	BYTE	i;

	display_char("[[AA[[AA[[");
//	for(i = 0; i < byMenuItemsDisplayed; ++i)
//	{
////		GetMenuItemText(byMenuID, sz);
////		OutText(0, i, sz);
//		byMenuID++;
//	}
}

static bool IsRetMenuItem(MENU_ITEM* pMenuItem)
{
	return (pMenuItem->byMenuItemID == MENU_ITEM_FIX_CONFIG);
}

static void MenuKeyboardHandler(BYTE byKeyCode)
{
	if (byKeyCode == KEY_1)
	{

	}
	else if (byKeyCode == KEY_2)
	{

	}
	else if (byKeyCode == KEY_3)
	{
		MENU_ITEM*	pMenuItem = &m_aMenuItems[m_mcbCurrent.byStartMenuItemID];

		if (IsRetMenuItem(pMenuItem))
		{
			if (!S_IS_EMPTY(m_stackMenuCtlBlock))
			{
				m_mcbCurrent = S_PEEK(m_stackMenuCtlBlock);
				S_POP(m_stackMenuCtlBlock);
			}
		}
		else if (pMenuItem->pChildMenu != NULL)
		{
			S_PUSH(m_stackMenuCtlBlock, m_mcbCurrent);
			m_mcbCurrent.pMenu 						= pMenuItem->pChildMenu;
			m_mcbCurrent.byStartMenuItemID			= pMenuItem->pChildMenu->byMenuItemID + 1;
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

static void ZeroConfig_KeyboardHandler(BYTE byKeyCode)
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
		if (m_byCursorPos < 2)
		{
			m_byCursorPos++;
		}
		else
		{
			m_mcbCurrent = S_PEEK(m_stackMenuCtlBlock);
			S_POP(m_stackMenuCtlBlock);
		}
	}
	else if (byKeyCode == KEY_3)
	{
		if (m_byCursorPos < 2)
		{
			m_bufKeyboard[m_byCursorPos]++;
			if (m_bufKeyboard[m_byCursorPos] > '9')
				m_bufKeyboard[m_byCursorPos] = '0';
		}
		else
		{
			m_mcbCurrent = S_PEEK(m_stackMenuCtlBlock);
			S_POP(m_stackMenuCtlBlock);
		}
	}
}

static void ZeroConfig_DisplayHandler()
{
	BYTE byNum[6]="0";
	BYTE byStr[10]="XYZABCCC";
	float nVal;

	nVal = 0;
	clearLCD();
	floattochar (nVal, byNum,0);
	display_char(byStr);
	display_digital(byNum,3,1);
}

static void ZeroConfig_OpeningHandler()
{
	m_byCursorPos = 0;

	m_bufKeyboard[0] = '0';
	m_bufKeyboard[1] = '0';
	m_bufKeyboard[2] = '#';
	m_bufKeyboard[3] = '\0';
}

static void DispConfig_KeyboardHandler(BYTE byKeyCode)
{
	if (byKeyCode == KEY_1)
	{
		m_bufKeyboard[0]++;
		if (m_bufKeyboard[0] > '5')
		{
			m_bufKeyboard[0] = '0';
		}
	}
	else if (byKeyCode == KEY_2)
	{
		m_mcbCurrent = S_PEEK(m_stackMenuCtlBlock);
		S_POP(m_stackMenuCtlBlock);
	}
	else if (byKeyCode == KEY_3)
	{
		m_mcbCurrent = S_PEEK(m_stackMenuCtlBlock);
		S_POP(m_stackMenuCtlBlock);
	}
}

static void DispConfig_DisplayHandler()
{
	BYTE byNum[6]="0";
	BYTE byStr[10]="ACEGIKMO";
	float nVal;

	nVal = 0;
	clearLCD();
	floattochar (nVal, byNum,0);
	display_char(byStr);
	display_digital(byNum,3,1);
}

static void DispConfig_OpeningHandler()
{
	m_byCursorPos = 0;

	m_bufKeyboard[0] = '0';
	m_bufKeyboard[1] = '\0';
}

void HMI_Init()
{
	initLCD_1622();
	// Enable internal pull-up resister

	// Init Main Menu
	m_aMenuItems[MENU_MAIN].byMenuItemID						= MENU_MAIN;
	m_aMenuItems[MENU_MAIN].byChildMenuItems					= 5;
	m_aMenuItems[MENU_MAIN].pChildMenu							= &m_aMenuItems[MENU_ITEM_ZERO_CONFIG];
	m_aMenuItems[MENU_MAIN].pParentMenu							= NULL;
	m_aMenuItems[MENU_MAIN].KeyboardHandler						= MainMenu_KeyboardHandler;
	m_aMenuItems[MENU_MAIN].DisplayHandler						= MainMenu_DisplayHandler;
	m_aMenuItems[MENU_MAIN].OnOpeningHandler					= NULL;

	m_aMenuItems[MENU_ITEM_ZERO_CONFIG].byMenuItemID			= MENU_ITEM_ZERO_CONFIG;
	m_aMenuItems[MENU_ITEM_ZERO_CONFIG].byChildMenuItems		= 0;
	m_aMenuItems[MENU_ITEM_ZERO_CONFIG].pChildMenu				= NULL;
	m_aMenuItems[MENU_ITEM_ZERO_CONFIG].pParentMenu				= &m_aMenuItems[MENU_MAIN];
	m_aMenuItems[MENU_ITEM_ZERO_CONFIG].KeyboardHandler			= ZeroConfig_KeyboardHandler;
	m_aMenuItems[MENU_ITEM_ZERO_CONFIG].DisplayHandler			= ZeroConfig_DisplayHandler;
	m_aMenuItems[MENU_ITEM_ZERO_CONFIG].OnOpeningHandler		= ZeroConfig_OpeningHandler;

	m_aMenuItems[MENU_ITEM_SPAN_CONFIG].byMenuItemID			= MENU_ITEM_SPAN_CONFIG;
	m_aMenuItems[MENU_ITEM_SPAN_CONFIG].byChildMenuItems		= 0;
	m_aMenuItems[MENU_ITEM_SPAN_CONFIG].pChildMenu				= NULL;
	m_aMenuItems[MENU_ITEM_SPAN_CONFIG].pParentMenu				= &m_aMenuItems[MENU_MAIN];
	m_aMenuItems[MENU_ITEM_SPAN_CONFIG].KeyboardHandler			= NULL;
	m_aMenuItems[MENU_ITEM_SPAN_CONFIG].DisplayHandler			= NULL;
	m_aMenuItems[MENU_ITEM_SPAN_CONFIG].OnOpeningHandler		= NULL;

	m_aMenuItems[MENU_ITEM_DAMP_CONFIG].byMenuItemID			= MENU_ITEM_DAMP_CONFIG;
	m_aMenuItems[MENU_ITEM_DAMP_CONFIG].byChildMenuItems		= 0;
	m_aMenuItems[MENU_ITEM_DAMP_CONFIG].pChildMenu				= NULL;
	m_aMenuItems[MENU_ITEM_DAMP_CONFIG].pParentMenu				= &m_aMenuItems[MENU_MAIN];
	m_aMenuItems[MENU_ITEM_DAMP_CONFIG].KeyboardHandler			= NULL;
	m_aMenuItems[MENU_ITEM_DAMP_CONFIG].DisplayHandler			= NULL;
	m_aMenuItems[MENU_ITEM_DAMP_CONFIG].OnOpeningHandler		= NULL;

	m_aMenuItems[MENU_ITEM_DISP_CONFIG].byMenuItemID			= MENU_ITEM_DISP_CONFIG;
	m_aMenuItems[MENU_ITEM_DISP_CONFIG].byChildMenuItems		= 0;
	m_aMenuItems[MENU_ITEM_DISP_CONFIG].pChildMenu				= NULL;
	m_aMenuItems[MENU_ITEM_DISP_CONFIG].pParentMenu				= &m_aMenuItems[MENU_MAIN];
	m_aMenuItems[MENU_ITEM_DISP_CONFIG].KeyboardHandler			= DispConfig_KeyboardHandler;
	m_aMenuItems[MENU_ITEM_DISP_CONFIG].DisplayHandler			= DispConfig_DisplayHandler;
	m_aMenuItems[MENU_ITEM_DISP_CONFIG].OnOpeningHandler		= DispConfig_OpeningHandler;

	m_aMenuItems[MENU_ITEM_FIX_CONFIG].byMenuItemID				= MENU_ITEM_FIX_CONFIG;
	m_aMenuItems[MENU_ITEM_FIX_CONFIG].byChildMenuItems			= 0;
	m_aMenuItems[MENU_ITEM_FIX_CONFIG].pChildMenu				= NULL;
	m_aMenuItems[MENU_ITEM_FIX_CONFIG].pParentMenu				= &m_aMenuItems[MENU_MAIN];
	m_aMenuItems[MENU_ITEM_FIX_CONFIG].KeyboardHandler			= NULL;
	m_aMenuItems[MENU_ITEM_FIX_CONFIG].DisplayHandler			= NULL;
	m_aMenuItems[MENU_ITEM_FIX_CONFIG].OnOpeningHandler			= NULL;

	m_mcbCurrent.pMenu 											= NULL;
	m_mcbCurrent.byStartMenuItemID								= 0;

	S_INIT(m_stackMenuCtlBlock, m_aMenuCtlBlocks, 4);
}

void HMI_Handler()
{
	CheckKeyboard();
	Display();
}
