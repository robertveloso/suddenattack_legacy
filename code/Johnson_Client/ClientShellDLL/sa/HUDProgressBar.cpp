// ----------------------------------------------------------------------- //
//
// MODULE  : HUDProgressBar.cpp
//
// PURPOSE : HUDProgressBar to display a meter for disabling gadget targets....
//
// (c) 2002 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

//
// Includes...
//

	#include "stdafx.h"
	#include "HUDMgr.h"
	#include "PlayerStats.h"
	#include "LayoutMgr.h"
	#include "HUDProgressBar.h"
	#include "SAGameClientShell.h"

// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CHUDProgressBar::CHUDProgressBar
//
//  PURPOSE:	Constructor...
//
// ----------------------------------------------------------------------- //
uint16 g_nTimeBombCnt = 0;

CHUDProgressBar::CHUDProgressBar()
:	CHUDMeter	()
{
	m_UpdateFlags	= kHUDProgressBar;
	m_bUseBar		= LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CHUDProgressBar::Update
//
//  PURPOSE:	Update the values to display the bar...
//
// ----------------------------------------------------------------------- //

void CHUDProgressBar::Update()
{
	m_dwValue		= g_pPlayerStats->GetProgress();
	m_dwMaxValue	= g_pPlayerStats->GetMaxProgress();
	
	m_MeterBasePos.x	= 400 - (uint16)(m_fMeterScale * (float)m_dwMaxValue / 2.0f);

	CHUDMeter::Update();
}

// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CHUDProgressBar::UpdateLayout
//
//  PURPOSE:	Get the values for displaying the bar...
//
// ----------------------------------------------------------------------- //

void CHUDProgressBar::UpdateLayout()
{
	int nCurrentLayout = GetConsoleInt("HUDLayout",0);

	m_MeterOffset		= g_pLayoutMgr->GetProgressBarOffset( nCurrentLayout );
	m_nMeterHeight		= g_pLayoutMgr->GetProgressBarHeight( nCurrentLayout );
	m_fMeterScale		= g_pLayoutMgr->GetProgressBarScale( nCurrentLayout );
	
	g_pLayoutMgr->GetProgressBarTexture( nCurrentLayout, m_szMeterTex, ARRAY_LEN( m_szMeterTex ));

	m_MeterBasePos.x	= (USABLE_WIDTH_I/2) - (uint16)(m_fMeterScale * (float)m_dwMaxValue / 2.0f);
	m_MeterBasePos.y	= int( (USABLE_HEIGHT_I/2) * g_pInterfaceResMgr->GetYRatio() + 20.0f + 16 );

}

void CHUDProgressBar::ScreenDimChanged()
{
}
