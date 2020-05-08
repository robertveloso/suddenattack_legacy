// ----------------------------------------------------------------------- //
//
// MODULE  : HUDRespawn.cpp
//
// PURPOSE : HUDItem to display hiding icon
//
// (c) 2001 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#include "stdafx.h"
#include "SAHUDMgr.h"
#include "HUDRespawn.h"
#include "SAPlayerStats.h"
#include "SAInterfaceMgr.h"
#include "GameClientShell.h"

#include "GameText.h"
#include "GameMessage.h"


extern VarTrack g_vtMultiplayerRespawnWaitTime;
extern VarTrack g_vtDoomsdayRespawnWaitTime;

static char s_aTagName[30] = "HUDRespawn";
static const int kMaxValue = 200;

// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CHUDRespawnBar::CHUDRespawnBar
//
//  PURPOSE:	Constructor...
//
// ----------------------------------------------------------------------- //

CHUDRespawnBar::CHUDRespawnBar()
:	CHUDMeter	()
{
	m_UpdateFlags = kHUDNone;
	m_bUseBar		= LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CHUDRespawnBar::Update
//
//  PURPOSE:	Update the values to display the bar...
//
// ----------------------------------------------------------------------- //

void CHUDRespawnBar::Update()
{
	m_MeterBasePos.x	= 400 - (uint16)((float)m_dwMaxValue * m_fMeterScale/2.0f);

	CHUDMeter::Update();
}

// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CHUDRespawnBar::UpdateLayout
//
//  PURPOSE:	Get the values for displaying the bar...
//
// ----------------------------------------------------------------------- //

void CHUDRespawnBar::UpdateLayout()
{

	int nCurrentLayout = GetConsoleInt("HUDLayout",0);

	m_MeterBasePos.y	= g_pLayoutMgr->GetInt(s_aTagName,"BarPosY");
	m_nMeterHeight		= g_pLayoutMgr->GetInt(s_aTagName,"BarHeight");
	m_fMeterScale		= g_pLayoutMgr->GetFloat(s_aTagName,"BarScale");
	
	g_pLayoutMgr->GetString(s_aTagName,"BarTexture", m_szMeterTex, ARRAY_LEN( m_szMeterTex ));

	
}

void CHUDRespawnBar::ScreenDimChanged()
{
}


//******************************************************************************************
//**
//** HUD Respawn display
//**
//******************************************************************************************

CHUDRespawn::CHUDRespawn()
{
	m_UpdateFlags = kHUDRespawn;
	m_eLevel = kHUDRenderText;
	m_bDraw = LTFALSE;
	m_fDuration = 0.0f;
	m_bReady = false;
	m_pGodMode = LTNULL;
}


LTBOOL CHUDRespawn::Init()
{
	UpdateLayout();

	// Init our bar...
	
	m_RespawnBar.Init();
	m_RespawnBar.SetMaxValue( kMaxValue );

	m_hTex	= g_pInterfaceResMgr->GetTexture( "SA_INTERFACE\\HUD\\Respawn\\RESPAWN.dtx" );
	g_pDrawPrim->SetUVWH( &m_Poly, 0.0f, 0.0f, 0.6796875, 1.0f );
	g_pDrawPrim->SetRGBA( &m_Poly, 0xFFFFFFFF );

	m_pGodMode = new CSAD3DText();
	m_pGodMode->CreateD3DText( 256, 16, true );
	m_pGodMode->SetTextColor( RGB(255,255,255), RGB(0,0,0) );
	m_pGodMode->SetTransparent(true);
	m_pGodMode->DrawD3DText( NF_GetMsg(SA_TEXT_HELP_RESPAWN), 
							 strlen(NF_GetMsg(SA_TEXT_HELP_RESPAWN)),
							 DT_CENTER, D3D_TEXT_OUTLINE );
	ScreenDimChanged();

	return LTTRUE;
}

void CHUDRespawn::Term()
{
	SAFE_DELETE( m_pGodMode );
}

void CHUDRespawn::Render()
{
	if (!m_bDraw) return;

	uint8 nVal = kMaxValue;
	float fTimeLeft = g_pPlayerMgr->GetRespawnTime() - g_pLTClient->GetTime();
	if( fTimeLeft > 0.0f)
	{
		nVal = uint8( (1.0f - (fTimeLeft / m_fDuration)) * kMaxValue );
	}
	else
	{
		if (!m_bReady)
		{
			m_bReady = true;
		}
	}

//	TRACE("Respawn : %d\n",nVal );
	m_RespawnBar.SetValue( nVal );
 	m_RespawnBar.Update();
	m_RespawnBar.Render();

	g_pDrawPrim->SetTexture( m_hTex );
	g_pDrawPrim->DrawPrim( &m_Poly );

	m_pGodMode->Render();
}

void CHUDRespawn::Update()
{
	m_bDraw = IsMultiplayerGame() && g_pPlayerMgr->IsPlayerDead();
	if( g_pGameClientShell->GetGameType() == eGameTypeSurvival )
		m_bDraw = 0.0f;
	if (!m_bDraw)
	{
		m_fDuration = -1.0f;
		return;
	}
	
	if (m_fDuration < 0.0f)
	{
		m_bReady = false;
		m_fDuration = g_vtMultiplayerRespawnWaitTime.GetFloat();		

		m_RespawnBar.SetValue(0);
	}
	m_RespawnBar.Update();
}

void CHUDRespawn::UpdateLayout()
{
	int nCurrentLayout = GetConsoleInt("HUDLayout",0);
	m_RespawnBar.UpdateLayout();
}

void CHUDRespawn::ScreenDimChanged()
{
	g_pDrawPrim->SetXYWH( &m_Poly,
						  (USABLE_WIDTH_I*g_pInterfaceResMgr->GetXRatio()-87)*0.5f,
						  340*g_pInterfaceResMgr->GetYRatio(),
						  87.0f*g_pInterfaceResMgr->GetXRatio(),
						  16.0f*g_pInterfaceResMgr->GetYRatio() );
	m_pGodMode->SetPosition( (USABLE_WIDTH_I*g_pInterfaceResMgr->GetXRatio()-256)*0.5f,
							 340*g_pInterfaceResMgr->GetYRatio()+50 );
}