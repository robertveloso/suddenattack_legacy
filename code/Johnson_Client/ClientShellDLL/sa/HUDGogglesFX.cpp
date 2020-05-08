#include "stdafx.h"
#include "SAHUDMgr.h"

#define MAX_GOGGLES_ADD_VAL	64.0f
#define GOGGLES_NOISE_UPDATE_TIME	0.05f;

CHUDGogglesFX::CHUDGogglesFX()
{
	m_bVisible		= false;	
	m_UpdateFlags	= kHUDFrame;
	m_eLevel		= kHUDRenderNone;

	m_bAdding		= false;
	m_fGammaR		= 1.0f;
	m_fGammaG		= 1.0f;
	m_fGammaB		= 1.0f;
}

LTBOOL CHUDGogglesFX::Init()
{
	char	szTemp[256];

	for( int i=0 ; i<MAX_GOGGLES_NOISE_NUM ; ++i )
	{
		sprintf( szTemp, "SA_INTERFACE/HUD/GOGGLES/GogglesNoise%02d.dtx", i+1 );
		m_hNoise[i] = g_pInterfaceResMgr->GetTexture( szTemp );
		SetupQuadUVs( m_PolyNoise[i], m_hNoise[i], 0.0f, 0.0f, 1.0f, 1.0f );
		g_pDrawPrim->SetRGBA( &m_PolyNoise[i], 0xFFFFFFFF );
	}
	this->ScreenDimChanged();
	
	return LTTRUE;
}

void CHUDGogglesFX::Term()
{
}

void CHUDGogglesFX::SetVisible( bool bVisible )
{
	m_bVisible	= bVisible;
	if( m_bVisible )
	{
		GetConsoleFloat( "GammaR", m_fGammaR );
		GetConsoleFloat( "GammaR", m_fGammaG );
		GetConsoleFloat( "GammaR", m_fGammaB );

		this->SetGammaRamp( m_fGammaR, m_fGammaG*0.2f, m_fGammaB*0.2f );
		
		m_bAdding			= true;
		m_fAddVal			= 0.0f;
		m_nNoiseIndex		= 0;
		m_fNoiseUpdateTime	= GOGGLES_NOISE_UPDATE_TIME;
	}
	else
	{	
		this->SetGammaRamp( m_fGammaR, m_fGammaG, m_fGammaB );

		m_bAdding			= false;
	}
}

bool CHUDGogglesFX::IsVisible()
{
	return m_bVisible;
}

void CHUDGogglesFX::Render()
{
	if( false == IsVisible() ) return;

	
	m_fNoiseUpdateTime -= g_pLTClient->GetFrameTime();
	if( m_fNoiseUpdateTime < 0.0f )
	{
		++m_nNoiseIndex;
		if( m_nNoiseIndex >= MAX_GOGGLES_NOISE_NUM ) m_nNoiseIndex = 0;
		m_fNoiseUpdateTime	= GOGGLES_NOISE_UPDATE_TIME;
	}

	g_pDrawPrim->SetTexture( m_hNoise[m_nNoiseIndex] );
	g_pDrawPrim->DrawPrim( &m_PolyNoise[m_nNoiseIndex] );

	g_pDrawPrim->SetTexture( NULL );
	g_pDrawPrim->SetAlphaBlendMode(DRAWPRIM_BLEND_ADD);
	g_pDrawPrim->DrawPrim( &m_PolyAddAlpha );
	g_pDrawPrim->SetAlphaBlendMode(DRAWPRIM_BLEND_MOD_SRCALPHA);
}

void CHUDGogglesFX::Update()
{
	if( false == IsVisible() ) return;

	if( m_bAdding )
	{
		m_fAddVal += g_pLTClient->GetFrameTime() * 160.0f;	//2초에 80에 도달하려면 1초에 40
		if( m_fAddVal >= MAX_GOGGLES_ADD_VAL )
		{
			m_bAdding	= false;
			m_fAddVal	= MAX_GOGGLES_ADD_VAL;
		}

		uint32	dwAddVal = (uint32)m_fAddVal;
		
		g_pDrawPrim->SetRGBA( &m_PolyAddAlpha, 0xFF000000|dwAddVal|(dwAddVal<<8)|(dwAddVal<<16) );
	}
}

void CHUDGogglesFX::ScreenDimChanged()
{
	for( int i=0 ; i<MAX_GOGGLES_NOISE_NUM ; ++i )
	{
		g_pDrawPrim->SetXYWH( &m_PolyNoise[i],
							  0, 0, 
							  (float)g_pInterfaceResMgr->GetScreenWidth(),
							  (float)g_pInterfaceResMgr->GetScreenHeight() );
	}

	g_pDrawPrim->SetXYWH( &m_PolyAddAlpha,
						  0, 0,
						  (float)g_pInterfaceResMgr->GetScreenWidth(),
						  (float)g_pInterfaceResMgr->GetScreenHeight() );
}

void CHUDGogglesFX::SetGammaRamp( float fR, float fG, float fB )
{
	char str[256];
		
	LTSNPrintF( str, sizeof(str), "%s %f", "GammaR", fR );
	g_pLTClient->RunConsoleString(str);
	LTSNPrintF( str, sizeof(str), "%s %f", "GammaG", fG );
	g_pLTClient->RunConsoleString(str);
	LTSNPrintF( str, sizeof(str), "%s %f", "GammaB", fB );
	g_pLTClient->RunConsoleString(str);
}