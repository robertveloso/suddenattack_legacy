// ----------------------------------------------------------------------- //
//
// MODULE  : HUDCompass.cpp
//
// PURPOSE : HUDItem to display a compass
//
// (c) 2001 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#include "stdafx.h"
#include "SAHUDMgr.h"
#include "PlayerMgr.h"

//******************************************************************************************
//**
//** HUD Compass display
//**
//******************************************************************************************
// ±¤½Ä TEST [ Mini Map ]
//-----------------------------------------------------------------
/*
namespace
{
	HTEXTURE		g_hTexMiniMap		= LTNULL;
	HTEXTURE		g_hTexPlayer		= LTNULL;
	LTPoly_GT4		g_hPolyMiniMap;
	LTPoly_GT4		g_hPolyPlayer;
	int				g_Map_x				= 4592;
	int				g_Map_z				= 4422;
	LTVector		g_MapSourceOffset;
	float			g_MapScale_x		= 512.0f/4592.0f;
	float			g_MapScale_z		= 512.0f/4422.0f;
}
*/
//-----------------------------------------------------------------

CHUDCompass::CHUDCompass()
{
	m_UpdateFlags = kHUDFrame;
	

//	m_hRadarOutline	= LTNULL;
	m_hRadarBack	= LTNULL;
	m_hRadarRot_1	= LTNULL;
	m_hRadarRot_2	= LTNULL;
	
	m_bDraw = LTFALSE;
	m_eLevel = kHUDRenderGameOver;
	m_fLastRotation = 0.0f;
}

LTBOOL CHUDCompass::Init()
{
	m_BasePos.x	= 0;
	m_BasePos.y	= 0;
	m_nSize		= 128;
	m_fCenterX	= (float)(m_BasePos.x + m_nSize/2);
	m_fCenterY	= (float)(m_BasePos.y + m_nSize/2);
	m_fRadius	= (float)m_nSize / (2.0f * (float)sin(MATH_PI/4.0f));

/*
	m_hRadarOutline	= g_pInterfaceResMgr->GetTexture( "SA_INTERFACE/HUD/RADAR/RD_OUTLINE.dtx" );
	SetupQuadUVs( m_hPolyRadarOutline, m_hRadarOutline, 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_hPolyRadarOutline, 0xFFFFFFFF );
*/
	
	m_hRadarBack = g_pInterfaceResMgr->GetTexture("SA_INTERFACE/HUD/RADAR/RadarBackground.dtx");
	SetupQuadUVs( m_hPolyRadarBack, m_hRadarBack, 0.0f, 0.0f, 1.0f, 1.0f);
	g_pDrawPrim->SetRGBA( &m_hPolyRadarBack, 0xFFFFFFFF );

	m_hRadarRot_1 = g_pInterfaceResMgr->GetTexture("SA_INTERFACE/HUD/RADAR/RadarDirecrtion.dtx");
	SetupQuadUVs( m_hPolyRadarRot_1, m_hRadarRot_1, 0.0f, 0.0f, 1.0f, 1.0f);
	g_pDrawPrim->SetRGBA( &m_hPolyRadarRot_1, 0xFFFFFFFF );
	
	m_hRadarRot_2 = g_pInterfaceResMgr->GetTexture("SA_INTERFACE/HUD/RADAR/RadarScan.dtx");
	SetupQuadUVs( m_hPolyRadarRot_2, m_hRadarRot_2, 0.0f, 0.0f, 1.0f, 1.0f);
	g_pDrawPrim->SetRGBA( &m_hPolyRadarRot_2, 0xFFFFFFFF );

	// ±¤½Ä TEST [ Mini Map ]
	//-----------------------------------------------------------------
	/*
	g_hTexMiniMap = g_pInterfaceResMgr->GetTexture("SA_INTERFACE\\warehouse_top.dtx");	
	SetupQuadUVs( g_hPolyMiniMap, g_hTexMiniMap, 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &g_hPolyMiniMap, 0x55FFFFFF );
	g_hTexPlayer  = g_pInterfaceResMgr->GetTexture("SA_INTERFACE\\HUD\\RADAR\\RADAR_PLAYER_ALIVE.dtx");
	SetupQuadUVs( g_hPolyPlayer, g_hTexPlayer, 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &g_hPolyPlayer, 0xFFFFFFFF );	
	g_pLTClient->GetSourceWorldOffset(g_MapSourceOffset);
	*/
	//-----------------------------------------------------------------

	
	this->ScreenDimChanged();

	return LTTRUE;
}

void CHUDCompass::Term()
{

}

void CHUDCompass::Render()
{
	if (!m_bDraw) return;

	// ±¤½Ä TEST [ Mini Map ]
	//-----------------------------------------------------------------
	/*
	g_pDrawPrim->SetTexture( g_hTexMiniMap );
	g_pDrawPrim->DrawPrim( &g_hPolyMiniMap );
	g_pDrawPrim->SetTexture( g_hTexPlayer );
	g_pDrawPrim->DrawPrim( &g_hPolyPlayer );
	return;
	*/
	//-----------------------------------------------------------------

	g_pDrawPrim->SetTexture( m_hRadarBack );
	g_pDrawPrim->DrawPrim( &m_hPolyRadarBack );

	g_pDrawPrim->SetTexture( m_hRadarRot_2 );
	g_pDrawPrim->DrawPrim( &m_hPolyRadarRot_2 );

	g_pDrawPrim->SetTexture( m_hRadarRot_1 );
	g_pDrawPrim->DrawPrim( &m_hPolyRadarRot_1 );

/*
	g_pDrawPrim->SetTexture( m_hRadarOutline );
	g_pDrawPrim->DrawPrim( &m_hPolyRadarOutline );
*/
}

void CHUDCompass::Update()
{
	if (!m_bDraw) return;

	// ±¤½Ä TEST [ Mini Map ]
	//-----------------------------------------------------------------
	/*
	LTVector vLocalPos, vResult, vMapPos, vTempPos;
	HOBJECT hLocalObj = g_pLTClient->GetClientObject();	
	g_pLTClient->GetObjectPos(hLocalObj, &vLocalPos);
	vResult = g_MapSourceOffset - vLocalPos;
	vTempPos.x = vResult.x * g_MapScale_x;
	vTempPos.z = vResult.z * g_MapScale_z;
	vMapPos.x = 174 - vTempPos.x;
	vMapPos.z = 192 + vTempPos.z;
	g_pDrawPrim->SetXYWH( &g_hPolyPlayer,
						   (float)vMapPos.x * g_pInterfaceResMgr->GetXRatio(),
						   (float)vMapPos.z * g_pInterfaceResMgr->GetYRatio(),
						   (float)16 * g_pInterfaceResMgr->GetXRatio(),
						   (float)16 * g_pInterfaceResMgr->GetYRatio() );

	LTPoly_GT4 ttt = g_hPolyPlayer;
	*/
	//-----------------------------------------------------------------

	float fWorldNorth = GetConsoleFloat("WorldNorth",0.0f);
	float fRot = (fWorldNorth - (MATH_PI * 0.75f)) - g_pPlayerMgr->GetYaw();

	if (g_pPlayerMgr->IsCameraAttachedToHead())
	{
		fRot = m_fLastRotation;
	}
	else
	{
		m_fLastRotation = fRot;
	}


	uint8	i;
	float	fX[4];
	float	fY[4];
	
	for ( i = 0; i < 4; ++i )
	{
		fX[i] = (m_fCenterX + (float)cos(fRot) * m_fRadius) * g_pInterfaceResMgr->GetXRatio();
		fY[i] = (m_fCenterY + (float)sin(fRot) * m_fRadius) * g_pInterfaceResMgr->GetXRatio();

		fRot += (MATH_PI / 2);
	}
	g_pDrawPrim->SetXY4( &m_hPolyRadarRot_1,
						 fX[0], fY[0],
						 fX[1], fY[1],
						 fX[2], fY[2],
						 fX[3], fY[3] );


	fRot	= g_pLTClient->GetTime();
	for ( i = 0; i < 4; ++i )
	{
		fX[i] = (m_fCenterX + (float)cos(fRot) * m_fRadius) * g_pInterfaceResMgr->GetXRatio();
		fY[i] = (m_fCenterY + (float)sin(fRot) * m_fRadius) * g_pInterfaceResMgr->GetXRatio();

		fRot += (MATH_PI / 2);
	}
	g_pDrawPrim->SetXY4( &m_hPolyRadarRot_2,
						 fX[0], fY[0],
						 fX[1], fY[1],
						 fX[2], fY[2],
						 fX[3], fY[3] );
}

void CHUDCompass::ScreenDimChanged()
{
/*
	g_pDrawPrim->SetXYWH( &m_hPolyRadarOutline,
						   (float)m_BasePos.x * g_pInterfaceResMgr->GetXRatio(),
						   (float)m_BasePos.y * g_pInterfaceResMgr->GetYRatio(),
						   (float)m_nSize * g_pInterfaceResMgr->GetXRatio(),
						   (float)m_nSize * g_pInterfaceResMgr->GetYRatio() );
*/

	g_pDrawPrim->SetXYWH( &m_hPolyRadarBack,
						   (float)m_BasePos.x * g_pInterfaceResMgr->GetXRatio(),
						   (float)m_BasePos.y * g_pInterfaceResMgr->GetYRatio(),
						   (float)m_nSize * g_pInterfaceResMgr->GetXRatio(),
						   (float)m_nSize * g_pInterfaceResMgr->GetYRatio() );

	g_pDrawPrim->SetXYWH( &m_hPolyRadarRot_1,
						   (float)m_BasePos.x * g_pInterfaceResMgr->GetXRatio(),
						   (float)m_BasePos.x * g_pInterfaceResMgr->GetYRatio(),
						   (float)m_nSize * g_pInterfaceResMgr->GetXRatio(),
						   (float)m_nSize * g_pInterfaceResMgr->GetYRatio() );

	g_pDrawPrim->SetXYWH( &m_hPolyRadarRot_2,
						   (float)m_BasePos.x * g_pInterfaceResMgr->GetXRatio(),
						   (float)m_BasePos.x * g_pInterfaceResMgr->GetYRatio(),
						   (float)m_nSize * g_pInterfaceResMgr->GetXRatio(),
						   (float)m_nSize * g_pInterfaceResMgr->GetYRatio() );

	// ±¤½Ä TEST [ Mini Map ]
	//-----------------------------------------------------------------
	/*
	g_pDrawPrim->SetXYWH( &g_hPolyMiniMap,
						   (float)m_BasePos.x * g_pInterfaceResMgr->GetXRatio(),
						   (float)m_BasePos.x * g_pInterfaceResMgr->GetYRatio(),
						   (float)512 * g_pInterfaceResMgr->GetXRatio(),
						   (float)512 * g_pInterfaceResMgr->GetYRatio() );
	g_pDrawPrim->SetXYWH( &g_hPolyPlayer,
						   (float)m_BasePos.x * g_pInterfaceResMgr->GetXRatio(),
						   (float)m_BasePos.x * g_pInterfaceResMgr->GetYRatio(),
						   (float)16 * g_pInterfaceResMgr->GetXRatio(),
						   (float)16 * g_pInterfaceResMgr->GetYRatio() );
						   */
	//-----------------------------------------------------------------

}