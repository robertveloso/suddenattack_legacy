#include "stdafx.h"
#include "SAHUDMgr.h"

CHUDCameraBloodFX::CHUDCameraBloodFX()
{
	m_bVisible		= false;
	
	m_UpdateFlags	= kHUDFrame;
	m_eLevel		= kHUDRenderNone;
}

CHUDCameraBloodFX::~CHUDCameraBloodFX()
{
	this->Term();
}

LTBOOL CHUDCameraBloodFX::Init()
{
	char szTex[256];
	for( int i=0 ; i<MAX_NORMAL_BLOOD_TEX ; ++i )
	{
		sprintf( szTex, "SA_INTERFACE/HUD/CAMERABLOOD/NORMALBLOOD%d.DTX", i );
		m_hNormalBlood[i] = g_pInterfaceResMgr->GetTexture(szTex);
	}

	this->ScreenDimChanged();
	
	return LTTRUE;
}

void CHUDCameraBloodFX::Term()
{
}

void CHUDCameraBloodFX::Show( bool bHeadShot, int nDir )
{
	/**/

	
	CUserProfile* pProfile = g_pProfileMgr->GetCurrentProfile();
	if( !pProfile || !pProfile->m_bGore ) return;

	if( bHeadShot )
	{
		if( GetRandom( 0, 10 ) < 3 ) return;
	}
	else
	{
		if( GetRandom( 0, 10 ) < 5 ) return;
	}
	
	nDir = GetRandom(0,2);
	switch( nDir )
	{
	case DIR_LEFT:// 谅 -> 快
		m_fHorzDir		= 1.0f;
		m_fBloodPosX	= GetRandom( m_fBloodTexW*(-0.5f), 0.0f );
		break;
	case DIR_RIGHT:// 快 -> 谅
		m_fHorzDir		= -1.0f;
		m_fBloodPosX	= g_pInterfaceResMgr->GetScreenWidth() - GetRandom( m_fBloodTexW*0.5f, m_fBloodTexW );
		break;
	case DIR_FRONT://啊款单
	case DIR_BACK:
		m_fHorzDir		= 1.0f;
		m_fBloodPosX	= GetRandom( 350.0f, 450.0f ) * g_pInterfaceResMgr->GetXRatio();
		break;
	}
	
	m_fBloodPosY	= 100.0f * g_pInterfaceResMgr->GetYRatio();
	m_fLastUpdate	= m_fStartTime = g_pLTClient->GetTime();
	m_nDrawCount	= 0;
	m_nAlpha		= 0xFF;
	
	m_bVisible		= true;

	for( int i=0 ; i<MAX_NORMAL_BLOOD_POLY ; ++i )
	{
		if( i < MAX_NORMAL_BLOOD_POLY*0.3 )
		{
			m_BloodData[i].hTex	= m_hNormalBlood[ GetRandom( 0, 1 ) ];
		}
		else
		{
			m_BloodData[i].hTex	= m_hNormalBlood[ GetRandom( 2, 4 ) ];
		}
		g_pDrawPrim->SetXYWH( &m_BloodData[i].hPoly,
							  m_fBloodPosX,
							  m_fBloodPosY + GetRandom( 0.0f, 200.0f * g_pInterfaceResMgr->GetYRatio() ),
							  m_fBloodTexW,
							  m_fBloodTexH );	
		SetupQuadUVs( m_BloodData[i].hPoly, m_BloodData[i].hTex, 0.0f, 0.0f, 1.0f, 1.0f );
		g_pDrawPrim->SetRGBA( &m_BloodData[i].hPoly, 0xFFFFFFFF );

		if( nDir == DIR_LEFT || nDir == DIR_RIGHT )
		{
			m_fBloodPosX	+= GetRandom( 0.0f, m_fBloodTexW ) * m_fHorzDir;
		}
		else
		{
			m_fBloodPosX	+= GetRandom( -m_fBloodTexW, m_fBloodTexW );
			m_nDrawCount = MAX_NORMAL_BLOOD_POLY;
		}
	}
}

void CHUDCameraBloodFX::Hide()
{
	m_bVisible		= false;
}

bool CHUDCameraBloodFX::IsVisible()
{
	return m_bVisible;
}

void CHUDCameraBloodFX::Render()
{
	if( false == IsVisible() ) return;

	for( int i=0 ; i<m_nDrawCount ; ++i )
	{
		g_pDrawPrim->SetTexture( m_BloodData[i].hTex );
		g_pDrawPrim->DrawPrim( &m_BloodData[i].hPoly );
	}
}


void CHUDCameraBloodFX::Update()
{
	if( false == IsVisible() ) return;

	float	fCurTime = g_pLTClient->GetTime();

	if( m_nDrawCount < MAX_NORMAL_BLOOD_POLY )
	{
		if( (fCurTime-m_fLastUpdate) > 0.01f )
		{
			m_fLastUpdate = fCurTime;		
			m_nDrawCount += 2;
			if( m_nDrawCount > MAX_NORMAL_BLOOD_POLY ) m_nDrawCount = MAX_NORMAL_BLOOD_POLY;
		}
	}
	else
	{
		if( fCurTime - m_fStartTime > CAMERA_BLOOD_SHOW_TIME )
		{
			int32 n = int32( g_pLTClient->GetFrameTime() * 100 );
			m_nAlpha -= (n<=0) ? 1 : n;

			if( m_nAlpha < 10 )
			{
				this->Hide();
				return;
			}
				
			for( int i=0 ; i<MAX_NORMAL_BLOOD_POLY ; ++i )
			{
				g_pDrawPrim->SetRGBA( &m_BloodData[i].hPoly, ( 0x00FFFFFF|(m_nAlpha<<24) ) );
			}
		}
	}
}

void CHUDCameraBloodFX::ScreenDimChanged()
{
	uint32 nTexW, nTexH;
	g_pTexInterface->GetTextureDims( m_hNormalBlood[0], nTexW, nTexH );

	m_fBloodTexW	= nTexW*g_pInterfaceResMgr->GetXRatio();
	m_fBloodTexH	= nTexH*g_pInterfaceResMgr->GetYRatio();

	m_fBloodDeltaX	= float( g_pInterfaceResMgr->GetScreenWidth() / MAX_NORMAL_BLOOD_POLY );
}