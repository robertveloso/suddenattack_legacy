#include "stdafx.h"
#include "SAHUDMgr.h"
#include "ClientSoundMgr.h"

CHUDFlashBangFX::CHUDFlashBangFX()
{
	m_bVisible		= false;	
	m_UpdateFlags	= kHUDFrame;
	m_eLevel		= kHUDRenderNone;

	m_bIncrease		= false;
	m_bDecrease		= false;
	m_fBlindTime	= 0.0f;
	m_hScreenBackup	= LTNULL;
	m_hLoopSound	= LTNULL;

	m_fWeaponsSndMul	= 1.0f;
	m_fSpeechSndMul		= 1.0f;
	m_fDefaultSndMul	= 1.0f;
	m_fMursumSndMul		= 1.0f;
}

LTBOOL CHUDFlashBangFX::Init()
{
	this->ScreenDimChanged();
	
	return LTTRUE;
}

void CHUDFlashBangFX::InitVar()
{
	m_fWeaponsSndMul	= GetConsoleFloat( "weaponssoundmultiplier", 1.0f );
	m_fSpeechSndMul		= GetConsoleFloat( "defaultsoundmultiplier", 1.0f );
	m_fDefaultSndMul	= GetConsoleFloat( "speechsoundmultiplier", 1.0f );

	this->Term();
}

void CHUDFlashBangFX::Term()
{
	m_bIncrease		= false;
	m_bDecrease		= false;

	if( m_hLoopSound )
	{
		g_pLTClient->SoundMgr()->KillSound( m_hLoopSound );
		m_hLoopSound	= LTNULL;
	}
	ApplySoundVolume( 0.0f );
	this->DeleteScreenBackup();
}

void CHUDFlashBangFX::SetVisible( bool bVisible, float fBlindTime )
{
	if( m_bVisible && bVisible )
	{
		if( m_bDecrease )
		{
			m_bIncrease				= true;
			m_bDecrease				= false;
			m_fBlindTime			= fBlindTime;
			m_fSurfaceAlpha			= 0.4f;
			m_fSurfaceAlphaDelay	= fBlindTime*0.5f;
		}
		else
		{
			if( fBlindTime > m_fBlindTime )
			{
				m_fBlindTime = fBlindTime;
				m_fSurfaceAlphaDelay	= fBlindTime*0.5f;
			}
		}

		return;
	}

	if( false == m_bVisible && false == bVisible ) return;

	m_bVisible	= bVisible;
	if( m_bVisible )
	{
		m_bIncrease				= true;
		m_bDecrease				= false;
		m_fAddVal				= 0.0f;
		m_fBlindTime			= fBlindTime;
		m_fSurfaceAlpha			= 0.4f;
		m_fSurfaceAlphaDelay	= fBlindTime*0.5f;

		m_hLoopSound = g_pClientSoundMgr->PlaySoundLocal( "SA_Interface/Snd/StunLoop.wav",
														  SOUNDPRIORITY_PLAYER_HIGH,
														  PLAYSOUND_LOOP,
														  90,
														  1.0f,
														  MURSUM_SOUND_CLASS );
		DeleteScreenBackup();
	}
	else
	{	
		this->Term();
	}
}

bool CHUDFlashBangFX::IsVisible()
{
	return m_bVisible;
}

void CHUDFlashBangFX::Render()
{
	if( false == IsVisible() ) return;

	if( m_bIncrease && LTNULL == m_hScreenBackup )
	{
		m_hScreenBackup = g_pLTClient->CreateSurface( g_pInterfaceResMgr->GetScreenWidth(),
													  g_pInterfaceResMgr->GetScreenHeight() );
		g_pLTClient->DrawSurfaceToSurface( m_hScreenBackup, g_pLTClient->GetScreenSurface(), LTNULL, 0, 0 );
	}

	if( m_bDecrease && LTNULL != m_hScreenBackup )
	{
		g_pLTClient->SetSurfaceAlpha( m_hScreenBackup, m_fSurfaceAlpha );
		g_pLTClient->DrawSurfaceToSurface( g_pLTClient->GetScreenSurface(), m_hScreenBackup, LTNULL, 0, 0 );
	}

	g_pDrawPrim->SetTexture( NULL );

	g_pDrawPrim->SetAlphaBlendMode(DRAWPRIM_BLEND_ADD);
	g_pDrawPrim->DrawPrim( &m_PolyAddAlpha );
	g_pDrawPrim->SetAlphaBlendMode(DRAWPRIM_BLEND_MOD_SRCALPHA);
}

void CHUDFlashBangFX::Update()
{
	if( false == IsVisible() ) return;

	if( m_bIncrease )	//증가
	{
		m_fAddVal += g_pLTClient->GetFrameTime() * 2048.0f;
		if( m_fAddVal >= 255.0f )
		{
			m_bIncrease	= false;
			m_fAddVal	= 255.0f;
		}

		uint32	dwAddVal = (uint32)m_fAddVal;
		
		g_pDrawPrim->SetRGBA( &m_PolyAddAlpha, 0xFF000000|dwAddVal|(dwAddVal<<8)|(dwAddVal<<16) );
		ApplySoundVolume( m_fAddVal/255.0f, true );
	}
	else if( m_bDecrease )	//감소
	{
		m_fAddVal -= g_pLTClient->GetFrameTime() * 256.0f;
		
		if( m_fAddVal < 0.0f )
		{
			m_fAddVal = 0.0f;
			
			m_fSurfaceAlphaDelay	-= g_pLTClient->GetFrameTime();
			if( m_fSurfaceAlphaDelay < 0.0f )
			{
				m_fSurfaceAlphaDelay = 0.0f;
				m_fSurfaceAlpha	-= g_pLTClient->GetFrameTime();
			}			
		}
		if( m_fSurfaceAlpha < 0.0f )
		{
			this->SetVisible(false);
			return;
		}

		uint32	dwAddVal = (uint32)m_fAddVal;
		g_pDrawPrim->SetRGBA( &m_PolyAddAlpha, 0xFF000000|dwAddVal|(dwAddVal<<8)|(dwAddVal<<16) );
		ApplySoundVolume( m_fAddVal/255.0f );
	}
	else					//최고치
	{
		m_fBlindTime	-= g_pLTClient->GetFrameTime();
		if( m_fBlindTime < 0.0f )	m_bDecrease = true;
	}
}

void CHUDFlashBangFX::ScreenDimChanged()
{
	g_pDrawPrim->SetXYWH( &m_PolyAddAlpha,
						  0, 0,
						  (float)g_pInterfaceResMgr->GetScreenWidth(),
						  (float)g_pInterfaceResMgr->GetScreenHeight() );
}

void CHUDFlashBangFX::DeleteScreenBackup()
{
	if( m_hScreenBackup )
	{
		g_pLTClient->DeleteSurface( m_hScreenBackup );
		m_hScreenBackup	= LTNULL;
	}
}

void CHUDFlashBangFX::ApplySoundVolume( float fPersent, bool bInc )
{
	ILTClientSoundMgr* pSndMgr;

	pSndMgr = (ILTClientSoundMgr*)g_pLTClient->SoundMgr();
	if( pSndMgr )
	{
		pSndMgr->SetSoundClassMultiplier( WEAPONS_SOUND_CLASS, m_fWeaponsSndMul * (1.0f-fPersent) );
		pSndMgr->SetSoundClassMultiplier( SPEECH_SOUND_CLASS, m_fSpeechSndMul * (1.0f-fPersent) );
		pSndMgr->SetSoundClassMultiplier( DEFAULT_SOUND_CLASS, m_fDefaultSndMul * (1.0f-fPersent) );
		pSndMgr->SetSoundClassMultiplier( MURSUM_SOUND_CLASS, m_fMursumSndMul * fPersent );	
		pSndMgr->UpdateVolumeSettings();
	}
}