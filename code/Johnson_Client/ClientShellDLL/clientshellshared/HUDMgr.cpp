// ----------------------------------------------------------------------- //
//
// MODULE  : HUDMgr.cpp
//
// PURPOSE : Implementation of CHUDMgr class
//
// CREATED : 07/17/01
//
// (c) 2001 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#include "stdafx.h"
#include "HUDMgr.h"
#include "GameClientShell.h"
#include "MissionMgr.h"

CHUDMgr*				g_pHUDMgr		= LTNULL;
CHUDScores*				g_pScores		= LTNULL;
CHUDRadar*				g_pRadar		= LTNULL;
CHUDChatInput*			g_pChatInput	= LTNULL;
CHUDChatMsg*			g_pChatMsgs		= LTNULL;
CHUDHelpMsg*			g_pHelpMsgs		= LTNULL;
//CHUDPickupMsg*			g_pPickupMsgs	= LTNULL;
CHUDKillMsg*			g_pKillMsgs		= LTNULL;
CHUDNickList*			g_pNickList		= LTNULL;
CHUDHelp*				g_pHelpBoard	= LTNULL;


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CHUDMgr::CHUDMgr()
//
//	PURPOSE:	Constructor
//
// ----------------------------------------------------------------------- //

CHUDMgr::CHUDMgr()
{
	m_itemArray.reserve(12);
	m_eLevel = kHUDRenderNone;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CHUDMgr::~CHUDMgr()
//
//	PURPOSE:	Destructor
//
// ----------------------------------------------------------------------- //

CHUDMgr::~CHUDMgr()
{
	Term();
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CHUDMgr::Init()
//
//	PURPOSE:	Initialize
//
// ----------------------------------------------------------------------- //

LTBOOL CHUDMgr::Init()
{
	m_itemArray.push_back(&m_Radar);
	m_itemArray.push_back(&m_ChatInput);
	m_itemArray.push_back(&m_ChatMsgs);			//채팅 메시지
	m_itemArray.push_back(&m_HelpMsg);
	//m_itemArray.push_back(&m_PickupMsgs);		//무기 주울때 나오는 메시지
	m_itemArray.push_back(&m_KillMsgs);			// 누가 누구 죽임
	m_itemArray.push_back(&m_NickList);
	m_itemArray.push_back(&m_Scores);			//스코어 보드
	m_itemArray.push_back(&m_HelpBoard);		//[yaiin] : 헬프 HUD
		
	g_pScores		= &m_Scores;
	g_pRadar		= &m_Radar;
	g_pHelpBoard	= &m_HelpBoard; //[yaiin]
	
	g_pChatInput	= &m_ChatInput;
	g_pChatMsgs		= &m_ChatMsgs;
	g_pHelpMsgs		= &m_HelpMsg;
	//g_pPickupMsgs	= &m_PickupMsgs;
	g_pKillMsgs		= &m_KillMsgs;
	g_pNickList		= &m_NickList;

	m_nCurrentLayout = GetConsoleInt("HUDLayout",0);
	
	ItemArray::iterator iter = m_itemArray.begin();
	while (iter != m_itemArray.end())
	{
		if (!(*iter)->Init())
			return LTFALSE;
		iter++;
	}

	m_ChatMsgs.Init( 13, 328, 320, 18, 10, 10.0f );
	m_HelpMsg.Init( 10, 100, 512, 20, 4, 10.0f );
	//m_PickupMsgs.Init( 200, 200, 128, 32, 2, 3.0f );
	m_KillMsgs.Init( 800-KILL_MSG_NICK_W-KILL_MSG_NICK_W-KILL_MSG_TEMP_W-KILL_MSG_TEMP_W-80,
					 20, 128, 32, 4, 5.0f );

	m_UpdateFlags = kHUDAll;

	g_pHUDMgr = this;

	m_bVisible = LTTRUE;
	m_bWaitRestart	= false;

    return LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CHUDMgr::Term()
//
//	PURPOSE:	Terminate the HUD
//
// ----------------------------------------------------------------------- //

void CHUDMgr::Term()
{
	ItemArray::iterator iter = m_itemArray.begin();

	while (iter != m_itemArray.end())
	{
		(*iter)->Term();
		iter++;
	}

	m_itemArray.clear();

	g_pHUDMgr = LTNULL;
}


void CHUDMgr::Update()
{
	if( m_bWaitRestart )
	{
		if( m_fRestartTime <= g_pLTClient->GetTime() && g_GameDoc.IsHost() )
		{
			g_pMissionMgr->RestartRound();
			m_bWaitRestart	= false;
#ifdef GAME_SHOW
			g_pScores->Show(false);
#endif
		}
		//return;
	}

	int nLayout = GetConsoleInt("HUDLayout",0);
	if (nLayout != m_nCurrentLayout)
	{
		m_nCurrentLayout = nLayout;
		UpdateLayout();
	}
	m_UpdateFlags |= kHUDFrame;

	ItemArray::iterator iter = m_itemArray.begin();

	while (iter != m_itemArray.end())
	{
		if (m_UpdateFlags & (*iter)->GetUpdateFlags())
			(*iter)->Update();
		iter++;
	}

	m_UpdateFlags = kHUDNone;

}

void CHUDMgr::Render()
{
	if( LTFALSE == g_pGameClientShell->IsMainWindowFocused() ) return;
	
	if( m_bWaitRestart )
	{
		SetRenderState();
		m_Scores.RenderTopScore();
		m_Scores.ForceRender();
		m_ChatInput.Render();
		m_ChatMsgs.Render();
		m_HelpBoard.Render(); //[yaiin]

		return;
	}

	if (!m_bVisible)
	{
		return;
	}

	ItemArray::iterator iter = m_itemArray.begin();

	SetRenderState();

	while (iter != m_itemArray.end())
	{
		//we need to make sure that this hud item is up to date though
		if (m_UpdateFlags & (*iter)->GetUpdateFlags())
			(*iter)->Update();

		if ((*iter)->GetRenderLevel() <= m_eLevel)
			(*iter)->Render();
		iter++;
	}

	//g_pLTClient->CPrint("Respawntime : %.1f", g_pPlayerMgr->GetRespawnTime());
}


void CHUDMgr::QueueUpdate(uint32 nUpdateFlag)
{
	m_UpdateFlags |= nUpdateFlag;
}

void CHUDMgr::ScreenDimsChanged()
{
	m_UpdateFlags = kHUDAll;

	ItemArray::iterator iter = m_itemArray.begin();

	while (iter != m_itemArray.end())
	{
		(*iter)->ScreenDimChanged();
		iter++;
	}
}

void CHUDMgr::NextLayout()
{
	m_nCurrentLayout++;
	if (m_nCurrentLayout >= g_pLayoutMgr->GetNumHUDLayouts())
		m_nCurrentLayout = 0;
    WriteConsoleInt("HUDLayout",m_nCurrentLayout);
	CUserProfile *pProfile = g_pProfileMgr->GetCurrentProfile();
	pProfile->m_nLayout = m_nCurrentLayout;
	pProfile->Save();
	UpdateLayout();
}

void CHUDMgr::PrevLayout()
{
	if (m_nCurrentLayout == 0)
		m_nCurrentLayout =  g_pLayoutMgr->GetNumHUDLayouts() - 1;
	else
		m_nCurrentLayout--;
    WriteConsoleInt("HUDLayout",m_nCurrentLayout);
	CUserProfile *pProfile = g_pProfileMgr->GetCurrentProfile();
	pProfile->m_nLayout = m_nCurrentLayout;
	pProfile->Save();
	UpdateLayout();
}

void CHUDMgr::UpdateLayout()
{
	ItemArray::iterator iter = m_itemArray.begin();

	while (iter != m_itemArray.end())
	{
		(*iter)->UpdateLayout();
		iter++;
	}


	m_UpdateFlags = kHUDAll;

}

void CHUDMgr::SetRenderState()
{
	g_pDrawPrim->SetTransformType(DRAWPRIM_TRANSFORM_SCREEN);
	g_pDrawPrim->SetZBufferMode(DRAWPRIM_NOZ); 
	g_pDrawPrim->SetClipMode(DRAWPRIM_NOCLIP);
	g_pDrawPrim->SetFillMode(DRAWPRIM_FILL);
	g_pDrawPrim->SetColorOp(DRAWPRIM_MODULATE);
	g_pDrawPrim->SetAlphaTestMode(DRAWPRIM_NOALPHATEST);
	g_pDrawPrim->SetAlphaBlendMode(DRAWPRIM_BLEND_MOD_SRCALPHA);
}

void CHUDMgr::WaitRestart()
{
	m_fRestartTime	= g_pLTClient->GetTime() + 5.0f;
	m_bWaitRestart	= true;
	m_Scores.Show(true);
}