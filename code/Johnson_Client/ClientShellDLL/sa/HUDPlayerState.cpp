#include "stdafx.h"
#include "SAHUDMgr.h"

//------------------------------------------- (B) Novice (minjin) [03/12/2005]
#include "GameText.h"
#include "GameMessage.h"
#include "PlayerMgr.h"
//------------------------------------------- (E) Novice (minjin) [03/12/2005]

#define TIME_BOMB_AREA_NOTIFY_ELAPSE	0.3f

CHUDPlayerState::CHUDPlayerState()
{
	m_UpdateFlags	= kHUDNone;
	m_eLevel		= kHUDRenderNone;
	m_fLastTimeBombNotifyBlinkTime = 0.0f;
}

LTBOOL CHUDPlayerState::Init()
{
	m_hTimeBombNotify = g_pInterfaceResMgr->GetTexture( "SA_INTERFACE/HUD/TIMER/TimeBombNotify.dtx" );
	SetupQuadUVs( m_PolyTimeBombNotify, m_hTimeBombNotify, 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_PolyTimeBombNotify, 0xFFFFFFFF );

	m_pAttachedPlayer = new CSAD3DText();
	m_pAttachedPlayer->CreateD3DTextWithFontSize( 512, 16, 14, true );
	m_pAttachedPlayer->SetTextColor( RGB(95,178,248) );
	m_pAttachedPlayer->SetTransparent(true);
	m_pAttachedPlayer->DrawD3DText( "", 0 );
	
	m_pHelpText = new CSAD3DText();
	m_pHelpText->CreateD3DText( 512, 16, true );
	m_pHelpText->SetTextColor( RGB(95,178,248) );
	m_pHelpText->SetTransparent(true);
	m_pHelpText->DrawD3DText( "", 0 );

	ScreenDimChanged();
	return LTTRUE;
}

void CHUDPlayerState::Term()
{
	SAFE_DELETE( m_pAttachedPlayer );
	SAFE_DELETE( m_pHelpText );
}

void CHUDPlayerState::Update()
{
}

void CHUDPlayerState::Render()
{
	//////////////////////////////////////////////////////////////////////////
	// 타임밤 지역 알림 아이콘 표시

	// 타임밤 지역이면서,
	if ( g_pPlayerMgr->InTimeBombArea() )
	{
		// 타임밤이 설치되지 않은 경우에만
		if ( LTFALSE == g_pPlayerMgr->GetPlantedTimeBomb() )
		{
			// 그린다.
			RenderTimeBombNotifyIcon();
		}
	}
	else
	{
		// 타임밤 지역이 아니면, 리셋!
		ResetTimeBombNotifyIcon();
	}
	//////////////////////////////////////////////////////////////////////////

	if( m_bShowAttachBar )
	{
		g_pDrawPrim->DrawPrim( &m_PolyAttachBar );
		m_pAttachedPlayer->Render();
		m_pHelpText->Render();
	}
}

void CHUDPlayerState::ScreenDimChanged()
{
	g_pDrawPrim->SetXYWH( &m_PolyTimeBombNotify, 
		(800 - 48) * 0.5f * g_pInterfaceResMgr->GetXRatio(),
							34 * g_pInterfaceResMgr->GetYRatio(),
							64.0f * g_pInterfaceResMgr->GetXRatio(),
							64.0f * g_pInterfaceResMgr->GetYRatio() );

	g_pDrawPrim->SetXYWH( &m_PolyAttachBar,
						  0.0f, (float)g_pInterfaceResMgr->GetScreenHeight()-55.0f,
						  (float)g_pInterfaceResMgr->GetScreenWidth(),
						  55.0f );
	g_pDrawPrim->SetRGBA( &m_PolyAttachBar, 0x70000000 );

	m_pAttachedPlayer->SetPosition( 10, g_pInterfaceResMgr->GetScreenHeight()-50 );
	m_pHelpText->SetPosition( 10, g_pInterfaceResMgr->GetScreenHeight()-20 );
}

void CHUDPlayerState::RenderTimeBombNotifyIcon()
{
	int		nColor	= 0;
	LTFLOAT fColor	= 0.0f;
	
	m_fLastTimeBombNotifyBlinkTime += g_pLTClient->GetFrameTime();
	
	g_pDrawPrim->SetTexture( m_hTimeBombNotify );
	
	if ( m_fLastTimeBombNotifyBlinkTime < TIME_BOMB_AREA_NOTIFY_ELAPSE )
	{
		fColor = m_fLastTimeBombNotifyBlinkTime * 255.0f / TIME_BOMB_AREA_NOTIFY_ELAPSE;
		nColor = (int)fColor;
		nColor = 0xFFFF0000 | nColor << 8 | nColor;

		g_pDrawPrim->SetRGBA( &m_PolyTimeBombNotify, nColor );
	}
	else if ( m_fLastTimeBombNotifyBlinkTime < TIME_BOMB_AREA_NOTIFY_ELAPSE * 2 )
	{
		fColor = 255.0f - ( (m_fLastTimeBombNotifyBlinkTime - TIME_BOMB_AREA_NOTIFY_ELAPSE) * 255.0f / TIME_BOMB_AREA_NOTIFY_ELAPSE );
		nColor = (int)fColor;
		nColor = 0xFFFF0000 | nColor << 8 | nColor;

		g_pDrawPrim->SetRGBA( &m_PolyTimeBombNotify, nColor );
	}
	else
	{
		m_fLastTimeBombNotifyBlinkTime = 0.0f;
	}
	
	g_pDrawPrim->DrawPrim( &m_PolyTimeBombNotify );
}

void CHUDPlayerState::ResetTimeBombNotifyIcon()
{
	m_fLastTimeBombNotifyBlinkTime = 0.0f;
}

void CHUDPlayerState::ShowAttachBar()
{
	if( LTTRUE == IsAttachBarVisible() ) return;

	m_bShowAttachBar = LTTRUE;
	

	m_pHelpText->DrawD3DText( NF_GetMsg(SA_TEXT_HELP_ATTACH_OTHER_PLAYER),
							  strlen( NF_GetMsg(SA_TEXT_HELP_ATTACH_OTHER_PLAYER) ),
							  DT_LEFT, D3D_TEXT_SHADOW );
}

void CHUDPlayerState::AttachPlayer( const char* lpszNick, int nAttachState )
{
	m_bShowAttachBar = LTTRUE;
	m_bAttached		= LTTRUE;
	m_nAttachState	= nAttachState;

	char szTemp[256];
	
	if( lpszNick )
	{
		sprintf( szTemp, NF_GetMsg(SA_TEXT_HELP_AFTER_ATTACH), lpszNick );
	}
	else
	{
		sprintf( szTemp, NF_GetMsg(SA_TEXT_FREE_SPECTATOR) );
	}

	m_pAttachedPlayer->DrawD3DText( szTemp, strlen(szTemp), DT_LEFT, D3D_TEXT_OUTLINE );

	sprintf( szTemp, NF_GetMsg(SA_TEXT_SPEC_HELP) );
	m_pHelpText->DrawD3DText( szTemp,
							  strlen(szTemp),
							  DT_LEFT, D3D_TEXT_SHADOW );
}

void CHUDPlayerState::HideAttachBar()
{
	if( LTFALSE == IsAttachBarVisible() ) return;
	
	m_bShowAttachBar = LTFALSE;
	m_bAttached		= LTFALSE;
	m_pAttachedPlayer->DrawD3DText( "", 0 );
	m_pHelpText->DrawD3DText( "", 0 );
	m_nAttachState = -1;
}

LTBOOL CHUDPlayerState::IsAttachBarVisible()
{
	return m_bShowAttachBar;
}