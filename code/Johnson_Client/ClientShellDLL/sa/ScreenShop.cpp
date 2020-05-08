#include "stdafx.h"
#include "ScreenShop.h"
#include "ScreenMgr.h"
#include "ScreenCommands.h"
#include "ClientMultiplayerMgr.h"
#include "GameClientShell.h"

CScreenShop::CScreenShop()
{
}

CScreenShop::~CScreenShop()
{
	Term();
}

LTBOOL CScreenShop::Build()
{
	// Create Backframe
	CreateBackWindow(SCREEN_ID_SHOP);

	if (!CScreenBase::Build()) return LTFALSE;

	return LTTRUE;
}

void CScreenShop::Term()
{
}

void CScreenShop::OnFocus(LTBOOL bFocus)
{
	if( bFocus )
	{
	}
	else
	{
	}
	
	CScreenBase::OnFocus(bFocus);
}

void CScreenShop::CreateBackWindow(eScreenID id)
{
}

uint32 CScreenShop::OnCommand(uint32 dwCommand, uint32 dwParam1, uint32 dwParam2)
{
	return 1;
}
