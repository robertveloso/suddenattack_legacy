#include "stdafx.h"
#include "SAHUDMgr.h"
#include "SAInterfaceMgr.h"
#include "HUDGameOver.h"
#include "../ClientshellShared/GameClientShell.h"
#include "../ClientshellShared/MissionMgr.h"
#include "../ClientshellShared/ClientMultiplayerMgr.h"

#include "ScreenRoom.h"

CHUDGameOver::CHUDGameOver()
{
	m_bVisible		= false;
	m_bRestartRound	= false;
	m_UpdateFlags	= kHUDAll;
	m_eLevel = kHUDRenderGameOver;
}

CHUDGameOver::~CHUDGameOver()
{
	this->Term();
}
	

LTBOOL CHUDGameOver::Init()
{
	m_hTexWin		= g_pInterfaceResMgr->GetTexture( "SA_INTERFACE/HUD/GameOver/HUDWin.dtx" );
	m_hTexDefeat	= g_pInterfaceResMgr->GetTexture( "SA_INTERFACE/HUD/GameOver/HUDDefeat.dtx" );
	m_hTexDraw		= g_pInterfaceResMgr->GetTexture( "SA_INTERFACE/HUD/GameOver/HUDDraw.dtx" );
	return LTTRUE;
}

void CHUDGameOver::Term()
{
}

void CHUDGameOver::ShowGameOver( int nResult, bool bRestartRound, float fDelayTime )
{
	switch( nResult )
	{		
	case GAME_RESULT_WIN:
		m_hTexture = m_hTexWin;
		break;
	case GAME_RESULT_LOSE:
		m_hTexture = m_hTexDefeat;
		break;
	case GAME_RESULT_DRAW:
		m_hTexture = m_hTexDraw;
		break;		
	}

	uint32 nTexW, nTexH;
	g_pTexInterface->GetTextureDims( m_hTexture, nTexW, nTexH );

	g_pDrawPrim->SetXYWH( &m_Poly,
						  (USABLE_WIDTH_I-nTexW)*g_pInterfaceResMgr->GetXRatio()*0.5f,
						  (USABLE_HEIGHT_I-nTexH)*g_pInterfaceResMgr->GetYRatio()*0.5f,
						  (float)nTexW*g_pInterfaceResMgr->GetXRatio(),
						  (float)nTexH*g_pInterfaceResMgr->GetYRatio() );

	SetupQuadUVs( m_Poly, m_hTexture, 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_Poly, 0xFFFFFFFF );

	m_fDeltaTime	= 2.5f;	//2.5초 후 게임 종료
	m_fStartTime	= g_pLTClient->GetTime() + fDelayTime;
	m_bVisible		= true;
	m_bRestartRound	= bRestartRound;
	m_bFirstUpdate	= false;
	m_nResult		= nResult;
}

void CHUDGameOver::HideGameOver()
{
	m_bVisible		= false;
	m_bRestartRound	= false;
	g_pMissionMgr->ClearGameOver();
}

bool CHUDGameOver::IsVisible()
{
	return m_bVisible;
}

void CHUDGameOver::Render()
{
	if( false == IsVisible() ) return;

	if( g_pLTClient->GetTime() > m_fStartTime )
	{
		g_pDrawPrim->SetTexture( m_hTexture );
		g_pDrawPrim->DrawPrim( &m_Poly );
	}
}


void CHUDGameOver::Update()
{
	if( false == IsVisible() ) return;

	UpdateAni();

	if( false == m_bFirstUpdate &&
		g_pLTClient->GetTime() > m_fStartTime )
	{
		m_bFirstUpdate = true;

		char szSound[MAX_CS_FILENAME_LEN];
#ifdef BIG_FAT_MODE_TEST
		if( LTFALSE == g_GameDoc.IsBigHeadMode() )
		{
			strcpy( szSound, "SA_INTERFACE\\SND\\Message\\" );
		}
		else
		{
			strcpy( szSound, "SA_INTERFACE\\SND\\FatMessage\\" );
		}
#else
		strcpy( szSound, "SA_INTERFACE\\SND\\Message\\" );
#endif

		switch( g_pGameClientShell->GetGameType() )
		{
		case eGameTypeTeamDeathmatch:
			switch( m_nResult )
			{
			case GAME_RESULT_WIN:
				strcat( szSound, (g_GameDoc.GetTeam() == TEAM_A) ? "A_Win.wav" : "B_Win.wav" );
				break;
			case GAME_RESULT_LOSE:
				strcat( szSound, (g_GameDoc.GetTeam() == TEAM_A) ? "B_Win.wav" : "A_Win.wav" );
				break;
			case GAME_RESULT_DRAW:
				strcat( szSound, "draw.wav" );
				break;
			}
			g_pClientSoundMgr->PlaySoundLocal( szSound, SOUNDPRIORITY_MISC_HIGH, 0, 90 );
			break;
			
		case eGameTypeSurvival:
			switch( m_nResult )
			{
			case GAME_RESULT_WIN:
				strcat( szSound, (g_GameDoc.GetTeam() == TEAM_A) ? "MC_A_Win.wav" : "MC_B_Win.wav" );
				break;
			case GAME_RESULT_LOSE:
				strcat( szSound, (g_GameDoc.GetTeam() == TEAM_A) ? "MF_B_Win.wav" : "MF_A_Win.wav" );
				break;
			case GAME_RESULT_DRAW:
				strcat( szSound, "draw.wav" );
				break;
			}
			g_pClientSoundMgr->PlaySoundLocal( szSound, SOUNDPRIORITY_MISC_HIGH, 0, 90 );
			break;
		}
	}

	if( (g_pLTClient->GetTime()-m_fStartTime)>m_fDeltaTime )
	{
		if( m_bRestartRound )
		{
			g_pHUDMgr->WaitRestart();

			HideGameOver();
		}
		else if( g_Network.IsRecvResult() )
		{
			//게임끝
			g_Network.EnableNetwork(LTFALSE);
			if( g_pGameClientShell->IsMainWindowFocused() )
			{			
				g_pLTClient->CPrint( "Switch To ScreenResult %d", ::GetTickCount() );
				g_pInterfaceMgr->SwitchToScreen(SCREEN_ID_RESULT);
			}
			else
			{
				g_Network.CS_GetRoomInfo();
				g_pInterfaceMgr->SwitchToScreen(SCREEN_ID_SCREEN_ROOM);
			}

			HideGameOver();
		}
	}
}

void CHUDGameOver::UpdateAni()
{
}

void CHUDGameOver::ScreenDimChanged()
{
}