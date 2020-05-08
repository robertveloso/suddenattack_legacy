// ----------------------------------------------------------------------- //
//
// MODULE  : ScreenMgr.cpp
//
// PURPOSE : Interface screen manager
//
// (c) 1999-2001 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //


#include "stdafx.h"
#include "SAScreenMgr.h"

//screens
#include "BaseScreen.h"
#include "ScreenMain.h"

//under main
#include "ScreenOptions.h"
#include "ScreenWeapon.h"

// Added By kwangsik ==>
// Time : 2004-07-20 오전 10:30:55
#include "ScreenRoom.h"
#include "ScreenCreateRoom.h"
#include "ScreenResult.h"
#include "ScreenCharacter.h"
#include "ScreenInventory.h"
#include "ScreenChannel.h"
#include "ScreenShop.h"
// Added By kwangsik <==

//under multi
#include "ScreenHost.h"
#include "ScreenPlayerTeam.h"
#include "ScreenEndDMMission.h"
#include "ScreenPreLoad.h"
#include "ScreenPostLoad.h"


#include "GameClientShell.h"
extern CGameClientShell* g_pGameClientShell;


static char s_aScreenName[SCREEN_ID_UNASSIGNED+1][32] =
{
#define INCLUDE_AS_STRING
#include "ScreenEnum.h"
#undef INCLUDE_AS_STRING

};


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSAScreenMgr::CSAScreenMgr()
{
	m_eCurrentScreenID = SCREEN_ID_NONE;
	m_eLastScreenID = SCREEN_ID_NONE;
}

CSAScreenMgr::~CSAScreenMgr()
{

}


//////////////////////////////////////////////////////////////////////
// Function name	: CSAScreenMgr::Init
// Description	    :
// Return type      : LTBOOL
//////////////////////////////////////////////////////////////////////

LTBOOL CSAScreenMgr::Init()
{
	//build screen array
	m_screenArray.reserve(SCREEN_ID_UNASSIGNED);

	for (int nID = SCREEN_ID_MAIN; nID < SCREEN_ID_UNASSIGNED; ++nID)
	{
		AddScreen((eScreenID)nID);
	}

    return CScreenMgr::Init();
}


void CSAScreenMgr::SwitchToScreen(CBaseScreen *pNewScreen)
{

	CScreenMgr::SwitchToScreen(pNewScreen);

	// Do any special case work for each screen
	if (m_eCurrentScreenID == SCREEN_ID_MAIN)
	{
		m_nHistoryLen = 0;
		m_eScreenHistory[0] = SCREEN_ID_NONE;
	}

}




void CSAScreenMgr::AddScreen(eScreenID screenID)
{
    CBaseScreen* pScreen = LTNULL;
	switch (screenID)
	{
		// 광식 : 초기 가입 시 Character Select 화면
	case SCREEN_ID_CHANNEL:
		pScreen = debug_new(CScreenChannel);
		break;
	case SCREEN_ID_MAIN:
		pScreen = debug_new(CScreenMain);
		break;
	case SCREEN_ID_INVENTORY :
		pScreen = debug_new(CScreenInventory);
		break;
	case SCREEN_ID_SHOP :
		pScreen = debug_new(CScreenShop);
		break;
	case SCREEN_ID_SCREEN_ROOM:
		pScreen = debug_new(CScreenRoom);
		break;
	case SCREEN_ID_RESULT :
		pScreen = debug_new(CScreenResult);
		break;
	}

	if (pScreen)
	{
		pScreen->Init(screenID);
		CScreenMgr::AddScreen(pScreen);

		if( screenID == SCREEN_ID_INVENTORY ) pScreen->Build();
	}

}


const char* CSAScreenMgr::GetScreenName(eScreenID id)
{
	return s_aScreenName[id];
}

// Returns a screen ID (uint16) based on a name
uint16 CSAScreenMgr::GetScreenIDFromName(char * pName)
{
	for (uint16 i=0; i < SCREEN_ID_UNASSIGNED; ++i)
	{
		if (!strcmpi(pName, s_aScreenName[i]))
			return (i);
	}
	return 999;
}
