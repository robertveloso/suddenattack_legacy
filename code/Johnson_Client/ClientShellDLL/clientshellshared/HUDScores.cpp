// ----------------------------------------------------------------------- //
//
// MODULE  : HUDScores.cpp
//
// PURPOSE : Implementation of CHUDScores to display player scores
//
// (c) 2001 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //
#include "stdafx.h"
#include "HUDMgr.h"
#include "HUDScores.h"
#include "InterfaceMgr.h"
#include "ClientMultiplayerMgr.h"
#include "ResShared.h"

// Added By IN'T ==>
// Time : 2004-04-27 오후 12:08:08
#include "Controlconstant.h"
// Added By IN'T <==

#include "GameText.h"
#include "GameMessage.h"

#include "../SA/SAHUDMgr.h"

#define A_TEAM_COLOR	RGB(238,120,24)
#define B_TEAM_COLOR	RGB(88,142,248)

namespace
{
	COLORREF	g_TextColor[4] =
	{
		RGB(152,152,152),
		RGB(140,140,140),
		RGB(240,240,240),
		RGB(255,255,255)
	};

#define SCORE_TEXTURE_SIZE			32.0f

}

CHUDScores::CHUDScores()
{
	m_UpdateFlags		= kHUDAll;
	m_eLevel			= kHUDRenderNone;
	m_bVisible			= LTFALSE;

	m_pTotalScore		= LTNULL;
	m_hScoreTx[0]		= LTNULL;
	m_hScoreTx[1]		= LTNULL;

	m_nTotalScore[0]		= 0;
	m_nTotalScore[1]		= 0;
	m_pChannelName		= LTNULL;
		
	m_bNeedUpdate		= LTFALSE;
}
	

LTBOOL CHUDScores::Init()
{

	// TEST : NOVICE
#ifndef _FINAL
	ZeroMemory( m_szWeaponSoundList, sizeof(m_szWeaponSoundList) );

	lstrcpy( m_szWeaponSoundList[0], "SA_WEAPONS\\SND\\AK47\\fire.wav" );
	lstrcpy( m_szWeaponSoundList[1], "SA_WEAPONS\\SND\\AK74U\\fire.wav" );
	lstrcpy( m_szWeaponSoundList[2], "SA_WEAPONS\\SND\\ANACONDA\\fire.wav" );
	lstrcpy( m_szWeaponSoundList[3], "SA_WEAPONS\\SND\\Beretta\\fire.wav" );
	lstrcpy( m_szWeaponSoundList[4], "SA_WEAPONS\\SND\\DesertEagle\\fire.wav" );
	lstrcpy( m_szWeaponSoundList[5], "SA_WEAPONS\\SND\\DRAGNOV\\fire.wav" );
	lstrcpy( m_szWeaponSoundList[6], "SA_WEAPONS\\SND\\famas\\fire.wav" );
	lstrcpy( m_szWeaponSoundList[7], "SA_WEAPONS\\SND\\G36\\Fire.wav" );
	lstrcpy( m_szWeaponSoundList[8], "SA_WEAPONS\\SND\\GRENADE\\Fire.wav" );
	lstrcpy( m_szWeaponSoundList[9], "SA_WEAPONS\\SND\\KNIFE\\FIRE.wav" );
	lstrcpy( m_szWeaponSoundList[10], "SA_WEAPONS\\SND\\m16a1\\fire.wav" );
	lstrcpy( m_szWeaponSoundList[11], "SA_WEAPONS\\SND\\M4A3\\fire.wav" );
	lstrcpy( m_szWeaponSoundList[12], "SA_WEAPONS\\SND\\MP5N\\fire.wav" );
	lstrcpy( m_szWeaponSoundList[13], "SA_WEAPONS\\SND\\MSG90A1\\fire.wav" );
	lstrcpy( m_szWeaponSoundList[14], "SA_WEAPONS\\SND\\P90\\fire.wav" );
	lstrcpy( m_szWeaponSoundList[15], "SA_WEAPONS\\SND\\Remington\\fire.wav" );
	lstrcpy( m_szWeaponSoundList[16], "SA_WEAPONS\\SND\\SPAS\\fire.wav" );
	lstrcpy( m_szWeaponSoundList[17], "SA_WEAPONS\\SND\\SSG69\\fire.wav" );
	lstrcpy( m_szWeaponSoundList[18], "SA_WEAPONS\\SND\\USP45\\fire.wav" );
	lstrcpy( m_szWeaponSoundList[19], "SA_WEAPONS\\SND\\UZI\\fire.wav" );
#endif //#ifndef _FINAL
	
	// 각종 위치들 가져오기..
	char *pTag = HUD_SCORE_TAG;

	// 배경 판떼기
	m_ptScoreBoardBasePos	= g_pLayoutMgr->GetPoint( pTag, "BGBasePt" );
	m_nScoreBoardWidth		= g_pLayoutMgr->GetInt( pTag, "BGWidth" );
	m_nScoreBoardHeight		= g_pLayoutMgr->GetInt( pTag, "BGHeight" );

	m_nGameTypeTopOffset	= g_pLayoutMgr->GetInt( pTag, "GameTypeTopOffset" );

	GET_TEXTURE_FROM_LAYOUT_MNGR( m_hScoreTx[0], pTag, "BGScore0" );
	GET_TEXTURE_FROM_LAYOUT_MNGR( m_hScoreTx[1], pTag, "BGScore1" );
	GET_TEXTURE_FROM_LAYOUT_MNGR( m_hGameType[0], pTag, "GameTypeMission" );
	GET_TEXTURE_FROM_LAYOUT_MNGR( m_hGameType[1], pTag, "GameTypeDeathMatch" );
	
	g_pDrawPrim->SetUVWH( &m_PolyGameType, 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_PolyGameType, 0xFFFFFFFF );

	g_pDrawPrim->SetUVWH( &m_PolyScore[0], 0.0f, 0.0f, 512.0f / 512.0f, 446.0f / 512.0f );
	g_pDrawPrim->SetRGBA( &m_PolyScore[0], 0xFFFFFFFF );
	
	g_pDrawPrim->SetUVWH( &m_PolyScore[1], 0.0f, 0.0f, 44.0f / 64.0f, 446.0f / 512.0f );
	g_pDrawPrim->SetRGBA( &m_PolyScore[1], 0xFFFFFFFF );
	
	// 팀별 프레임
	m_ptTeamFrameOffset[ETN_RED]	= g_pLayoutMgr->GetPoint( pTag, "TeamFrameOffset_Red" );
	m_ptTeamFrameOffset[ETN_BLUE]	= g_pLayoutMgr->GetPoint( pTag, "TeamFrameOffset_Blue" );

	m_nTeamFrameWidth		= g_pLayoutMgr->GetInt( pTag, "TeamFrameWidth" );
	m_nTeamFrameHeight		= g_pLayoutMgr->GetInt( pTag, "TeamFrameHeight" );

	m_ptWinCountOffset		= g_pLayoutMgr->GetPoint( pTag, "WinCountOffset" );

	m_ptTeamTotalScoreOffset[ETN_RED].x		= m_ptTeamFrameOffset[ETN_RED].x + 200;
	m_ptTeamTotalScoreOffset[ETN_RED].y		= m_ptTeamFrameOffset[ETN_RED].y + 300;
	m_ptTeamTotalScoreOffset[ETN_BLUE].x	= m_ptTeamFrameOffset[ETN_BLUE].x + 200;
	m_ptTeamTotalScoreOffset[ETN_BLUE].y	= m_ptTeamFrameOffset[ETN_BLUE].y + 300;

	m_nTopOffset	= 66;//55;
	m_nMarkOffset	= 5;
	m_nGradeOffset	= 28;
	m_nNameOffset	= 55;
	m_nKillOffset	= 151;
	m_nDeathOffset	= 186;
	m_nPingOffset	= 231;

	m_nLineHeight	= 30;

	m_nTotalTopOffset = 310;
	
	m_hCaptainTx = g_pInterfaceResMgr->GetTexture( "sa_interface\\menu\\textures\\room\\captaintx.dtx" );
	g_pDrawPrim->SetUVWH( &m_PolyCaptain, 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_PolyCaptain, 0xFFFFFFFF );

	m_hTopScore		= g_pInterfaceResMgr->GetTexture( "sa_interface\\hud\\score\\TopScorebar.dtx" );
	m_hRedLight		= g_pInterfaceResMgr->GetTexture( "sa_interface\\hud\\score\\RedLight.dtx" );
	m_hBlueLight	= g_pInterfaceResMgr->GetTexture( "sa_interface\\hud\\score\\BlueLight.dtx" );

	char szNum[256];
	for( int i=0 ; i<10 ; ++i )
	{
		sprintf( szNum, "sa_interface\\hud\\score\\Num_%d.dtx", i );
		m_hNumTex[i] = g_pInterfaceResMgr->GetTexture( szNum );
	}

	UpdateLayout();

	return LTTRUE;

}

bool CHUDScores::AddPlayer( const char* lpszNick, int nID, uint8 nGrade )
{
	for( int i = 0; i < kMaxPlayers ; i++ )
	{
		if( m_ScorePrint[i].m_nId < 0 || m_ScorePrint[i].m_nId == nID )
		{
			m_ScorePrint[i].m_nId		= nID;
			m_ScorePrint[i].m_nGrade	= nGrade;
			m_ScorePrint[i].m_pNameFont->DrawD3DText( lpszNick,
												 	  strlen(lpszNick),
													  DT_LEFT,
													  D3D_TEXT_NORMAL,
													  true );
			return true;
		}
	}

	return false;
}

bool CHUDScores::RemovePlayer( int nID )
{
	for( int i = 0; i < kMaxPlayers ; i++ )
	{
		if( m_ScorePrint[i].m_nId == nID )
		{
			m_ScorePrint[i].m_nId		= -1;
			return true;
		}
	}

	return false;
}

void CHUDScores::RemoveAll()
{
	for( int i = 0; i < kMaxPlayers ; i++ )
	{
		m_ScorePrint[i].m_nId		= -1;
	}
}

void CHUDScores::Term()
{
//	SAFE_DELETE_ARRAY( g_pGradeTx );

	this->DeleteD3DText();
}

void CHUDScores::DeleteD3DText()
{
	SAFE_DELETE_ARRAY( m_pTotalScore );
	SAFE_DELETE( m_pChannelName );
	
	for ( int n = 0; n < kMaxPlayers; n++ )
	{
		SAFE_DELETE( m_ScorePrint[n].m_pNameFont );
		SAFE_DELETE( m_ScorePrint[n].m_pKillFont );
		SAFE_DELETE( m_ScorePrint[n].m_pDeadFont );
	}
}

void CHUDScores::Show(bool bVisible) 
{	
	m_bVisible = bVisible;
}

void CHUDScores::Render()
{
#ifndef _FINAL
	DoNoisyTextTest();
#endif //#ifndef _FINAL
	RenderTopScore();
	if( !m_bVisible ) return;
	
#ifndef _FINAL
	DoSoundTest();
#endif //#ifndef _FINAL

	ForceRender();
}

void CHUDScores::ForceRender()
{
	//-------------------------------------------------------------------------
	// UI 다시 작업
	//-------------------------------------------------------------------------

	for ( int n = 0; n < 2; n++ )
	{
		g_pDrawPrim->SetTexture(m_hScoreTx[n] );
		g_pDrawPrim->DrawPrim(&m_PolyScore[n] );
	}

	//[SPIKE] 승리횟수
	if ( g_pGameClientShell->GetGameType() == eGameTypeSurvival)
	{
		g_pDrawPrim->SetTexture( m_hGameType[0] );
	}
	else
	{
		g_pDrawPrim->SetTexture( m_hGameType[1] );	
	}

	g_pDrawPrim->DrawPrim( &m_PolyGameType );
	

	for ( n = 0; n < kMaxPlayers; n++ )
	{
		if( m_ScorePrint[n].m_nId > -1 && m_ScorePrint[n].m_bRender )
		{
			m_ScorePrint[n].m_pNameFont->Render();
			m_ScorePrint[n].m_pKillFont->Render();
			m_ScorePrint[n].m_pDeadFont->Render();
			
			g_pDrawPrim->SetTexture( g_Global_Data.GetGradeTexture( m_ScorePrint[n].m_nGrade ) );
			g_pDrawPrim->DrawPrim( &(m_ScorePrint[n].m_PolyGrade) );

			g_pDrawPrim->SetTexture( g_Global_Data.GetPingTexture( m_ScorePrint[n].m_nPing ) );
			g_pDrawPrim->DrawPrim( &(m_ScorePrint[n].m_PolyPing) );
		}
	}
	
	for( n = 0; n < 2; n++ )
	{
		m_pTotalScore[n].Render();
	}

	g_pDrawPrim->SetTexture( m_hCaptainTx );
	g_pDrawPrim->DrawPrim( &m_PolyCaptain );

	g_pDrawPrim->SetTexture( m_hTextCap );
	g_pDrawPrim->DrawPrim( &m_PolyTextCap_A );
	g_pDrawPrim->SetTexture( m_hTextCap );
	g_pDrawPrim->DrawPrim( &m_PolyTextCap_B );

	m_pChannelName->Render();

	//-------------------------------------------------------------------------
}

void CHUDScores::RenderTopScore()
{
	g_pDrawPrim->SetTexture( m_hTopScore );
	g_pDrawPrim->DrawPrim( &m_PolyTopScore );

	if( g_GameDoc.GetTeam() == TEAM_A )
	{
		g_pDrawPrim->SetTexture( m_hRedLight );
		g_pDrawPrim->DrawPrim( &m_PolyRedLight );
	}
	else
	{
		g_pDrawPrim->SetTexture( m_hBlueLight );
		g_pDrawPrim->DrawPrim( &m_PolyBlueLight );
	}
	
	g_pDrawPrim->SetTexture( m_hNumTex[m_nRedScore_100] );
	g_pDrawPrim->DrawPrim( &m_PolyRedScore_100 );
	
	g_pDrawPrim->SetTexture( m_hNumTex[m_nRedScore_10] );
	g_pDrawPrim->DrawPrim( &m_PolyRedScore_10 );
	
	g_pDrawPrim->SetTexture( m_hNumTex[m_nRedScore_1] );
	g_pDrawPrim->DrawPrim( &m_PolyRedScore_1 );

	g_pDrawPrim->SetTexture( m_hNumTex[m_nBlueScore_100] );
	g_pDrawPrim->DrawPrim( &m_PolyBlueScore_100 );
	
	g_pDrawPrim->SetTexture( m_hNumTex[m_nBlueScore_10] );
	g_pDrawPrim->DrawPrim( &m_PolyBlueScore_10 );
	
	g_pDrawPrim->SetTexture( m_hNumTex[m_nBlueScore_1] );
	g_pDrawPrim->DrawPrim( &m_PolyBlueScore_1 );

	g_pDrawPrim->SetTexture( m_hNumTex[m_nObjectiveScore_100] );
	g_pDrawPrim->DrawPrim( &m_PolyScore_100 );
	
	g_pDrawPrim->SetTexture( m_hNumTex[m_nObjectiveScore_10] );
	g_pDrawPrim->DrawPrim( &m_PolyScore_10 );
	
	g_pDrawPrim->SetTexture( m_hNumTex[m_nObjectiveScore_1] );
	g_pDrawPrim->DrawPrim( &m_PolyScore_1 );
}

void CHUDScores::Update()
{
	if( m_bVisible ) UpdateScores();		
}

void CHUDScores::UpdateScores()
{
	if( !m_bNeedUpdate ) return;
	m_bNeedUpdate = false;

	CPlayerInfoMgr*	pPlayerInfoMgr = g_pInterfaceMgr->GetPlayerInfoMgr();
	pPlayerInfoMgr->UpdatePlayerSort();

	uint32 nPlayerCount = pPlayerInfoMgr->GetNumPlayers();
	if( nPlayerCount <= 0 ) return;

	T_PLAYER_INFOptr pPI;
	
	char	szNumber[64] = {0, };
	int		count[2] = {0,0};		
	uint32	nCurIndex = 0;
	sScore_Item* pScoreItem;

	for( int i = 0; i < kMaxPlayers ; i++ )
	{
		m_ScorePrint[i].m_bRender	= LTFALSE;
	}
	
	while( nCurIndex < nPlayerCount )
	{
		pPI	= pPlayerInfoMgr->GetPlayerByIndex(nCurIndex);

		if( pPI->bInGame == LTFALSE || pPI->nPlayerID < 0 ||
			//pPI->ePlayerState == PS_GHOST || 
			pPI->ePlayerState == PS_UNKNOWN )
		{
			++nCurIndex;
			continue;
		}

		if ( pPI->nTeamID >= kNumTeams || pPI->nTeamID < 0 )
		{
			++nCurIndex;
			continue;
		}

		pScoreItem = GetScoreItem(pPI->nPlayerID);
		if( !pScoreItem ) 
		{
			++nCurIndex;
			continue;
		}

		SetScoreItem( pScoreItem, pPI, count[pPI->nTeamID] );	
				
		++count[pPI->nTeamID];
		++nCurIndex;
	}

	for( int n = 0; n < 2; n++ )
	{
		sprintf( szNumber, "%d", m_nTotalScore[n] );
		m_pTotalScore[n].DrawD3DText(szNumber, strlen(szNumber), DT_LEFT, D3D_TEXT_OUTLINE, false );
	}
}

void CHUDScores::SetWinRound_Team(uint16 nWinRoundA, uint16 nWinRoundB)
{	
	m_nTotalScore[0]	= nWinRoundA;
	m_nTotalScore[1]	= nWinRoundB;
	UpdateTopScores();
}

void CHUDScores::SetKillDeath( uint32 nRedKill, uint32 nBlueKill )
{
	m_nTotalScore[0]	= nRedKill;
	m_nTotalScore[1]	= nBlueKill;
	UpdateTopScores();
}

void CHUDScores::ClearTotalScore()
{
	m_nTotalScore[0] = m_nTotalScore[1] = 0;
	UpdateTopScores();
}

void CHUDScores::SetChannelName( const char* lpszName )
{
	if( lpszName ) m_pChannelName->DrawD3DText( lpszName, strlen(lpszName), DT_CENTER, D3D_TEXT_SHADOW );
}

void CHUDScores::UpdateTopScores()
{	
	m_nRedScore_1 = m_nTotalScore[0];

	m_nRedScore_100 = m_nRedScore_1/100;
	m_nRedScore_1 -= 100*m_nRedScore_100;

	m_nRedScore_10 = m_nRedScore_1/10;
	m_nRedScore_1 -= 10*m_nRedScore_10;

#ifndef _FINAL
	if( m_nRedScore_100 < 0 || m_nRedScore_100 > 9 ||
		m_nRedScore_10 < 0 || m_nRedScore_10 > 9 ||
		m_nRedScore_1 < 0 || m_nRedScore_1 > 9 )
	{
		ASSERT( LTFALSE );
	}
#endif

	if( m_nRedScore_100 < 0 || m_nRedScore_100 > 9 ) m_nRedScore_100 = 0;
	if( m_nRedScore_10 < 0 || m_nRedScore_10 > 9 ) m_nRedScore_10 = 0;
	if( m_nRedScore_1 < 0 || m_nRedScore_1 > 9 ) m_nRedScore_1 = 0;



	m_nBlueScore_1 = m_nTotalScore[1];

	m_nBlueScore_100 = m_nBlueScore_1/100;
	m_nBlueScore_1 -= 100*m_nBlueScore_100;

	m_nBlueScore_10 = m_nBlueScore_1/10;
	m_nBlueScore_1 -= 10*m_nBlueScore_10;

#ifndef _FINAL
	if( m_nBlueScore_100 < 0 || m_nBlueScore_100 > 9 ||
		m_nBlueScore_10 < 0 || m_nBlueScore_10 > 9 ||
		m_nBlueScore_1 < 0 || m_nBlueScore_1 > 9 )
	{
		ASSERT( LTFALSE );
	}
#endif

	if( m_nBlueScore_100 < 0 || m_nBlueScore_100 > 9 ) m_nBlueScore_100 = 0;
	if( m_nBlueScore_10 < 0 || m_nBlueScore_10 > 9 ) m_nBlueScore_10 = 0;
	if( m_nBlueScore_1 < 0 || m_nBlueScore_1 > 9 ) m_nBlueScore_1 = 0;
}

void CHUDScores::AddKillCount( uint8 nTeamID )
{
	if( nTeamID == 0 || nTeamID == 1 )
	{
		if( m_eGameType == eGameTypeTeamDeathmatch )
		{
			++m_nTotalScore[nTeamID];
			UpdateTopScores();
		}
	}
}

void CHUDScores::PrePlayingState()
{
	m_eGameType = g_pGameClientShell->GetGameType();
	T_ROOM_OPTIONptr pOpt = g_GameDoc.GetRoomOption();

	switch( m_eGameType )
	{
	case eGameTypeTeamDeathmatch:
		m_hTextCap = g_pInterfaceResMgr->GetTexture( "sa_interface\\hud\\score\\text_kill.dtx" );
		
		m_nObjectiveScore_1 = pOpt->nKillLimit;
		m_nObjectiveScore_100 = m_nObjectiveScore_1/100;
		m_nObjectiveScore_1 -= 100*m_nObjectiveScore_100;
		m_nObjectiveScore_10 = m_nObjectiveScore_1/10;
		m_nObjectiveScore_1 -= 10*m_nObjectiveScore_10;
		break;

	case eGameTypeSurvival:
		m_hTextCap = g_pInterfaceResMgr->GetTexture( "sa_interface\\hud\\score\\text_round.dtx" );
		m_nObjectiveScore_1 = pOpt->nRoundLimit;
		m_nObjectiveScore_100 = m_nObjectiveScore_1/100;
		m_nObjectiveScore_1 -= 100*m_nObjectiveScore_100;
		m_nObjectiveScore_10 = m_nObjectiveScore_1/10;
		m_nObjectiveScore_1 -= 10*m_nObjectiveScore_10;
	default:
		break;
	}
}

void CHUDScores::UpdateLayout()
{
	// 글씨들 포인터 제거..
	this->DeleteD3DText();

	// 글씨들 맹글기..
	m_pTotalScore = new CSAD3DText[kMaxTeams];
	
	for ( int i = 0; i < 2; i++ )
	{
		m_pTotalScore[i].CreateD3DTextWithFontSize( 30, 16, 16, true );
		m_pTotalScore[i].SetTextColor(g_TextColor[3]);
		m_pTotalScore[i].SetTransparent(true);
		m_pTotalScore[i].DrawD3DText("", 0, DT_CENTER, D3D_TEXT_NORMAL, false);		
	}

	m_pChannelName = new CSAD3DText();
	m_pChannelName->CreateD3DText( 275, 16, true );
	m_pChannelName->SetTextColor( RGB(158,173,180), RGB(0,0,0) );
	m_pChannelName->SetTransparent(true);
	m_pChannelName->DrawD3DText( "", 0 );
	
	// 기본 위치 찾아오고..
	this->ScreenDimChanged();
	
	for( i = 0; i < kMaxPlayers; i++ )
	{
		m_ScorePrint[i].m_nId = -1;
		
		m_ScorePrint[i].m_pNameFont = new CSAD3DText();
		m_ScorePrint[i].m_pNameFont->CreateD3DText(100, 16, true);
		m_ScorePrint[i].m_pNameFont->SetTextColor(g_TextColor[2]);		
		m_ScorePrint[i].m_pNameFont->SetTransparent(true);
		
		m_ScorePrint[i].m_pDeadFont = new CSAD3DText();
		m_ScorePrint[i].m_pDeadFont->CreateD3DText(32, 16, true );
		m_ScorePrint[i].m_pDeadFont->SetTextColor(g_TextColor[2]);
		m_ScorePrint[i].m_pDeadFont->SetTransparent(true);
		
		m_ScorePrint[i].m_pKillFont = new CSAD3DText();
		m_ScorePrint[i].m_pKillFont->CreateD3DText(32, 16, true );
		m_ScorePrint[i].m_pKillFont->SetTextColor(g_TextColor[2]);
		m_ScorePrint[i].m_pKillFont->SetTransparent(true);

		g_pDrawPrim->SetUVWH( &(m_ScorePrint[i].m_PolyGrade), 0.0f, 0.0f, 1.0f, 1.0f );
		g_pDrawPrim->SetRGBA( &(m_ScorePrint[i].m_PolyGrade), 0xFFFFFFFF );	
		g_pDrawPrim->SetUVWH( &(m_ScorePrint[i].m_PolyPing), 0.0f, 0.0f, 1.0f, 1.0f );
		g_pDrawPrim->SetRGBA( &(m_ScorePrint[i].m_PolyPing), 0xFFFFFFFF );
	}

}

void CHUDScores::UpdateTeamPos(uint8 nTeam, LTIntPt pos)
{
}

void CHUDScores::UpdateBoardPos()
{
}

void CHUDScores::ScreenDimChanged()
{
	m_ptScoreBoardBasePos.x = (int)( (g_pInterfaceResMgr->GetScreenWidth() - m_nScoreBoardWidth) * 0.5f );
	m_ptScoreBoardBasePos.y = (int)( (g_pInterfaceResMgr->GetScreenHeight() - m_nScoreBoardHeight) * 0.5f );
	
	m_pTotalScore[0].SetPosition( m_ptScoreBoardBasePos.x + m_ptTeamFrameOffset[0].x + 220,
								  m_ptScoreBoardBasePos.y + m_ptTeamFrameOffset[0].y + m_nTotalTopOffset );
	m_pTotalScore[1].SetPosition( m_ptScoreBoardBasePos.x + m_ptTeamFrameOffset[1].x + 220,
								  m_ptScoreBoardBasePos.y + m_ptTeamFrameOffset[1].y + m_nTotalTopOffset );

	g_pDrawPrim->SetXYWH( &m_PolyScore[0], (float)m_ptScoreBoardBasePos.x, (float)m_ptScoreBoardBasePos.y,
							512.0f, 446.0f );
	g_pDrawPrim->SetXYWH( &m_PolyScore[1], (float)(m_ptScoreBoardBasePos.x + 512.0f), (float)m_ptScoreBoardBasePos.y,
							44.0f, 446.0f );
	g_pDrawPrim->SetXYWH( &m_PolyGameType, (float)(m_ptScoreBoardBasePos.x + (m_nScoreBoardWidth - 256.0f) * 0.5f), (float)m_ptScoreBoardBasePos.y + m_nGameTypeTopOffset,
							256.0f, 32.0f );

	m_pChannelName->SetPosition( m_ptScoreBoardBasePos.x+141, m_ptScoreBoardBasePos.y+69 );

	g_pDrawPrim->SetXYWH( &m_PolyTextCap_A, m_ptScoreBoardBasePos.x+95.0f,
											m_ptScoreBoardBasePos.y+397.0f,
											128.0f, 32.0f );
	g_pDrawPrim->SetRGBA( &m_PolyTextCap_A, 0xFFFFFFFF );
	g_pDrawPrim->SetUVWH( &m_PolyTextCap_A, 0.0f, 0.0f, 1.0f, 1.0f );

	g_pDrawPrim->SetXYWH( &m_PolyTextCap_B, m_ptScoreBoardBasePos.x+368.0f,
											m_ptScoreBoardBasePos.y+397.0f,
											128.0f, 32.0f );
	g_pDrawPrim->SetRGBA( &m_PolyTextCap_B, 0xFFFFFFFF );
	g_pDrawPrim->SetUVWH( &m_PolyTextCap_B, 0.0f, 0.0f, 1.0f, 1.0f );

	g_pDrawPrim->SetXYWH( &m_PolyTopScore, 314.0f*g_pInterfaceResMgr->GetXRatio(),
										   4.5f*g_pInterfaceResMgr->GetYRatio(),
										   256.0f*g_pInterfaceResMgr->GetXRatio(),
										   32.0f*g_pInterfaceResMgr->GetYRatio() );
	g_pDrawPrim->SetUVWH( &m_PolyTopScore, 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_PolyTopScore, 0xFFFFFFFF );

	g_pDrawPrim->SetXYWH( &m_PolyRedLight, 318.0f*g_pInterfaceResMgr->GetXRatio(),
										   8.5f*g_pInterfaceResMgr->GetYRatio(),
										   64.0f*g_pInterfaceResMgr->GetXRatio(),
										   32.0f*g_pInterfaceResMgr->GetYRatio() );
	g_pDrawPrim->SetUVWH( &m_PolyRedLight, 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_PolyRedLight, 0xFFFFFFFF );

	g_pDrawPrim->SetXYWH( &m_PolyBlueLight, 426.0f*g_pInterfaceResMgr->GetXRatio(),
										   8.5f*g_pInterfaceResMgr->GetYRatio(),
										   64.0f*g_pInterfaceResMgr->GetXRatio(),
										   32.0f*g_pInterfaceResMgr->GetYRatio() );
	g_pDrawPrim->SetUVWH( &m_PolyBlueLight, 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_PolyBlueLight, 0xFFFFFFFF );

	g_pDrawPrim->SetXYWH( &m_PolyRedScore_100, 342.0f*g_pInterfaceResMgr->GetXRatio(),
										   11.0f*g_pInterfaceResMgr->GetYRatio(),
										   16.0f*g_pInterfaceResMgr->GetXRatio(),
										   16.0f*g_pInterfaceResMgr->GetYRatio() );
	g_pDrawPrim->SetUVWH( &m_PolyRedScore_100, 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_PolyRedScore_100, 0xFFFFFFFF );

	g_pDrawPrim->SetXYWH( &m_PolyRedScore_10, 352.0f*g_pInterfaceResMgr->GetXRatio(),
										   11.0f*g_pInterfaceResMgr->GetYRatio(),
										   16.0f*g_pInterfaceResMgr->GetXRatio(),
										   16.0f*g_pInterfaceResMgr->GetYRatio() );
	g_pDrawPrim->SetUVWH( &m_PolyRedScore_10, 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_PolyRedScore_10, 0xFFFFFFFF );

	g_pDrawPrim->SetXYWH( &m_PolyRedScore_1, 362.0f*g_pInterfaceResMgr->GetXRatio(),
										   11.0f*g_pInterfaceResMgr->GetYRatio(),
										   16.0f*g_pInterfaceResMgr->GetXRatio(),
										   16.0f*g_pInterfaceResMgr->GetYRatio() );
	g_pDrawPrim->SetUVWH( &m_PolyRedScore_1, 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_PolyRedScore_1, 0xFFFFFFFF );

	g_pDrawPrim->SetXYWH( &m_PolyScore_100, 383.0f*g_pInterfaceResMgr->GetXRatio(),
										   11.0f*g_pInterfaceResMgr->GetYRatio(),
										   16.0f*g_pInterfaceResMgr->GetXRatio(),
										   16.0f*g_pInterfaceResMgr->GetYRatio() );
	g_pDrawPrim->SetUVWH( &m_PolyScore_100, 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_PolyScore_100, 0xFFFFFFFF );

	g_pDrawPrim->SetXYWH( &m_PolyScore_10, 393.0f*g_pInterfaceResMgr->GetXRatio(),
										   11.0f*g_pInterfaceResMgr->GetYRatio(),
										   16.0f*g_pInterfaceResMgr->GetXRatio(),
										   16.0f*g_pInterfaceResMgr->GetYRatio() );
	g_pDrawPrim->SetUVWH( &m_PolyScore_10, 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_PolyScore_10, 0xFFFFFFFF );

	g_pDrawPrim->SetXYWH( &m_PolyScore_1, 403.0f*g_pInterfaceResMgr->GetXRatio(),
										   11.0f*g_pInterfaceResMgr->GetYRatio(),
										   16.0f*g_pInterfaceResMgr->GetXRatio(),
										   16.0f*g_pInterfaceResMgr->GetYRatio() );
	g_pDrawPrim->SetUVWH( &m_PolyScore_1, 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_PolyScore_1, 0xFFFFFFFF );

	g_pDrawPrim->SetXYWH( &m_PolyBlueScore_100, 427.0f*g_pInterfaceResMgr->GetXRatio(),
										   11.0f*g_pInterfaceResMgr->GetYRatio(),
										   16.0f*g_pInterfaceResMgr->GetXRatio(),
										   16.0f*g_pInterfaceResMgr->GetYRatio() );
	g_pDrawPrim->SetUVWH( &m_PolyBlueScore_100, 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_PolyBlueScore_100, 0xFFFFFFFF );

	g_pDrawPrim->SetXYWH( &m_PolyBlueScore_10, 437.0f*g_pInterfaceResMgr->GetXRatio(),
										   11.0f*g_pInterfaceResMgr->GetYRatio(),
										   16.0f*g_pInterfaceResMgr->GetXRatio(),
										   16.0f*g_pInterfaceResMgr->GetYRatio() );
	g_pDrawPrim->SetUVWH( &m_PolyBlueScore_10, 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_PolyBlueScore_10, 0xFFFFFFFF );

	g_pDrawPrim->SetXYWH( &m_PolyBlueScore_1, 447.0f*g_pInterfaceResMgr->GetXRatio(),
										   11.0f*g_pInterfaceResMgr->GetYRatio(),
										   16.0f*g_pInterfaceResMgr->GetXRatio(),
										   16.0f*g_pInterfaceResMgr->GetYRatio() );
	g_pDrawPrim->SetUVWH( &m_PolyBlueScore_1, 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_PolyBlueScore_1, 0xFFFFFFFF );	
}

void CHUDScores::SetPing( uint8 nID, uint8 nPing )
{
	for( int i=0 ; i<kMaxPlayers ; ++i )
	{
		if( m_ScorePrint[i].m_nId == nID )
		{
			m_ScorePrint[i].m_nPing = nPing;
			return;
		}
	}
}

void CHUDScores::SetUpdate( bool bUpdate )
{
	m_bNeedUpdate = bUpdate;
}

sScore_Item* CHUDScores::GetScoreItem( int nPlayerID )
{
	for( int i=0 ; i<kMaxPlayers ; ++i )
	{
		if( m_ScorePrint[i].m_nId == nPlayerID ) return &m_ScorePrint[i];
	}

	return LTNULL;
}

void CHUDScores::SetScoreItem( sScore_Item* pScoreItem, T_PLAYER_INFOptr pPlayerInfo, int nCount )
{
	uint32 nLocalID = 0;
	g_pLTClient->GetLocalClientID(&nLocalID);
	uint32 nTeam = pPlayerInfo->nTeamID;
	COLORREF rgbMyTeam = RGB(0,0,0);
	char	szNumber[64] = {0, };

	pScoreItem->m_ePlayerState	= (uint32)pPlayerInfo->ePlayerState;
	pScoreItem->m_nTeamId		= (int)pPlayerInfo->nTeamID;

	pScoreItem->m_pNameFont->SetPosition( m_nNameOffset + m_ptTeamFrameOffset[nTeam].x + m_ptScoreBoardBasePos.x,
		m_nTopOffset + m_ptTeamFrameOffset[nTeam].y + m_ptScoreBoardBasePos.y + nCount * m_nLineHeight );
	pScoreItem->m_pKillFont->SetPosition( m_nKillOffset + m_ptTeamFrameOffset[nTeam].x + m_ptScoreBoardBasePos.x,
		m_nTopOffset + m_ptTeamFrameOffset[nTeam].y + m_ptScoreBoardBasePos.y + nCount * m_nLineHeight );
	pScoreItem->m_pDeadFont->SetPosition( m_nDeathOffset + m_ptTeamFrameOffset[nTeam].x + m_ptScoreBoardBasePos.x,
		m_nTopOffset + m_ptTeamFrameOffset[nTeam].y + m_ptScoreBoardBasePos.y + nCount * m_nLineHeight );
	
	g_pDrawPrim->SetXYWH( &(pScoreItem->m_PolyGrade),
						 (float)(m_nGradeOffset + m_ptTeamFrameOffset[nTeam].x + m_ptScoreBoardBasePos.x),
						 (float)(m_nTopOffset + m_ptTeamFrameOffset[nTeam].y + m_ptScoreBoardBasePos.y + nCount * m_nLineHeight - 5 ),
						 SCORE_TEXTURE_SIZE, SCORE_TEXTURE_SIZE );
	g_pDrawPrim->SetXYWH( &(pScoreItem->m_PolyPing),
						 (float)(m_nPingOffset + m_ptTeamFrameOffset[nTeam].x + m_ptScoreBoardBasePos.x),
						 (float)(m_nTopOffset + m_ptTeamFrameOffset[nTeam].y + m_ptScoreBoardBasePos.y + nCount * m_nLineHeight + 1 ),
						 SCORE_TEXTURE_SIZE, SCORE_TEXTURE_SIZE );	
	if( pPlayerInfo->bCaptain )
	{
		g_pDrawPrim->SetXYWH( &m_PolyCaptain,
								(float)(m_nMarkOffset + m_ptTeamFrameOffset[nTeam].x + m_ptScoreBoardBasePos.x),
								(float)(m_nTopOffset + m_ptTeamFrameOffset[nTeam].y + m_ptScoreBoardBasePos.y + nCount * m_nLineHeight - 1 ),
								16, 16 );
	}
	
	if( pScoreItem->m_nTeamId == 0 )
	{
		rgbMyTeam = A_TEAM_COLOR;
	}
	else
	{
		rgbMyTeam = B_TEAM_COLOR;
	}
	
	// Kill
	sprintf(szNumber, "%d", pPlayerInfo->nKill );
	pScoreItem->m_pKillFont->DrawD3DText(szNumber, strlen(szNumber), DT_RIGHT, D3D_TEXT_NORMAL, false );
	// dead
	sprintf(szNumber, "%d", pPlayerInfo->nDeath );
	pScoreItem->m_pDeadFont->DrawD3DText(szNumber, strlen(szNumber), DT_RIGHT, D3D_TEXT_NORMAL, false );
	pScoreItem->m_nNumber = nCount;
	pScoreItem->m_nTeamId = nTeam;

	if( pPlayerInfo->ePlayerState == PS_ALIVE )
	{
		if ( pScoreItem->m_nId == (int)nLocalID ) // 본인 
		{
			pScoreItem->m_pDeadFont->SetTextColor(rgbMyTeam);
			pScoreItem->m_pKillFont->SetTextColor(rgbMyTeam);
			pScoreItem->m_pNameFont->SetTextColor(rgbMyTeam);
		}
		else	// 타인 
		{
			pScoreItem->m_pDeadFont->SetTextColor(g_TextColor[2]);
			pScoreItem->m_pKillFont->SetTextColor(g_TextColor[2]);
			pScoreItem->m_pNameFont->SetTextColor(g_TextColor[2]);
		}
	}
	else if( pPlayerInfo->ePlayerState == PS_DYING ||
			 pPlayerInfo->ePlayerState == PS_DEAD ||			 
			 pPlayerInfo->ePlayerState == PS_MANDATE_DEAD )
	{
		if ( pScoreItem->m_nId == (int)nLocalID ) // 본인 
		{
			pScoreItem->m_pDeadFont->SetTextColor( rgbMyTeam );
			pScoreItem->m_pKillFont->SetTextColor( rgbMyTeam );
			pScoreItem->m_pNameFont->SetTextColor( rgbMyTeam );
		}
		else
		{
			pScoreItem->m_pDeadFont->SetTextColor( g_TextColor[0] );
			pScoreItem->m_pKillFont->SetTextColor( g_TextColor[0] );
			pScoreItem->m_pNameFont->SetTextColor( g_TextColor[0] );
		}
	}
	else
	{
		if ( pScoreItem->m_nId == (int)nLocalID ) // 본인 
		{
			pScoreItem->m_pDeadFont->SetTextColor( rgbMyTeam );
			pScoreItem->m_pKillFont->SetTextColor( rgbMyTeam );
			pScoreItem->m_pNameFont->SetTextColor( rgbMyTeam );
		}
		else
		{
			pScoreItem->m_pDeadFont->SetTextColor( RGB(115,113,115) );
			pScoreItem->m_pKillFont->SetTextColor( RGB(115,113,115) );
			pScoreItem->m_pNameFont->SetTextColor( RGB(115,113,115) );
		}
	}
		
	pScoreItem->m_bRender	= LTTRUE;
}


#ifndef _FINAL

void CHUDScores::DoNoisyTextTest()
{
	char szTestChatMsg[1024] = {0,};
	char szTestKillMsg[1024] = {0,};
	char szTestNtceMsg[1024] = {0,};

	uint32 dwNoisyTextFlags = GetConsoleInt("NoisyText", 0);

	if ( 0 < dwNoisyTextFlags )
	{
		static uint32 dwCounter;

		if ( dwNoisyTextFlags & 0x01 )
		{
			ZeroMemory( szTestChatMsg, sizeof(szTestChatMsg) );
			wsprintf( szTestChatMsg, "[NoisyText] Chatting Test !!! ( %d )", dwCounter );
			g_pChatMsgs->AddMessage( szTestChatMsg, (eChatMsgType)(dwCounter%12) );		
		}

		if ( dwNoisyTextFlags & 0x02 )
		{
			ZeroMemory( szTestKillMsg, sizeof(szTestKillMsg) );
			wsprintf( szTestKillMsg, "%d", dwCounter );
			g_pKillMsgs->AddMessage( "NoisyTextTest", 0, szTestKillMsg, 1, "SA_INTERFACE\\WEAPON\\DrakunovD.dtx", (rand()%2) ? 255 : NODE_HEAD );
		}

		if ( dwNoisyTextFlags & 0x04 )
		{
			ZeroMemory( szTestNtceMsg, sizeof(szTestNtceMsg) );
			wsprintf( szTestNtceMsg, "[NoisyText] Notice Message Test !!! ( %d )", dwCounter );
			g_pHelpMsgs->AddMessage( szTestNtceMsg, 16, (eChatMsgType)(dwCounter%12), 0.3f );
		}

		if ( dwNoisyTextFlags & 0x08 )
		{
			g_pRadio->Show( (dwCounter % 2) ? (rand() % 3) : -1 );
		}

		dwCounter++;
	}
}

void CHUDScores::DoSoundTest()
{
	uint32 nSoundCountPerSec	= GetConsoleInt( "SndCntPerSec", 0 );
	uint32 nSoundCountSameTime	= LTMIN( GetConsoleInt( "SndCntAtSameTime", 0 ), 20 );
	
	if ( nSoundCountPerSec > 0 )
	{
		static LTFloat	fTime = 0.0f;
		
		fTime = fTime + g_pLTClient->GetFrameTime();
		
		LTVector vPos;
		g_pLTClient->GetObjectPos( g_pLTClient->GetClientObject(), &vPos );
		
		if ( fTime > ( 1.0f / (float)(nSoundCountPerSec) ) )
		{
			char szTestFileName[256] = {0,};
			GetConsoleString( "SndTestFileName", szTestFileName, "" );
			
			if ( lstrcmp(szTestFileName, "0") && lstrcmp(szTestFileName, "" ) )
			{
				int nSoundIdx = (int)szTestFileName[0] - (int)'a';
				
				HLTSOUND hLocal = g_pClientSoundMgr->PlaySoundLocal(m_szWeaponSoundList[nSoundIdx], (SoundPriority)6, 0, 70, 1.0f, 1 );
				
				HLTSOUND hFromP = g_pClientSoundMgr->PlaySoundFromPos(const_cast<LTVector&>(vPos), m_szWeaponSoundList[nSoundIdx],
					3000.0f, (SoundPriority)6, 0, 100, 1.0f, -1.0f, 1 );
				
				//				HLTSOUND hFromZP = g_pClientSoundMgr->PlaySoundFromPos(const_cast<LTVector&>(vZeroPos), m_szWeaponSoundList[nSoundIdx],
				//					3000.0f, (SoundPriority)6, 0, 100, 1.0f, -1.0f, 1 );
			}
			else
			{
				for ( int n = 0; n < (int)nSoundCountSameTime; n++ )
				{
					HLTSOUND hLocal = g_pClientSoundMgr->PlaySoundLocal(m_szWeaponSoundList[n], (SoundPriority)6, 0, 70, 1.0f, 1 );
					
					HLTSOUND hFromP = g_pClientSoundMgr->PlaySoundFromPos(const_cast<LTVector&>(vPos), m_szWeaponSoundList[n],
						3000.0f, (SoundPriority)6, 0, 100, 1.0f, -1.0f, 1 );
					
					//					HLTSOUND hFromZP = g_pClientSoundMgr->PlaySoundFromPos(const_cast<LTVector&>(vZeroPos), m_szWeaponSoundList[n],
					//						3000.0f, (SoundPriority)6, 0, 100, 1.0f, -1.0f, 1 );
				}
			}
			
			fTime = 0.0f;
		}
	}
}
#endif // #ifndef _FINAL
