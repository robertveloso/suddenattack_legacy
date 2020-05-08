#include "stdafx.h"
#include "SAHUDMgr.h"
#include "GameClientShell.h"

#define WPN_CHOOSER_VEL					100000.0f
#define WPN_CHOOSER_MIN_ICON_RATIO		0.5f
#define WPN_CHOOSER_REAL_ICON_RATION	0.8f
#define WPN_CHOOSER_SHOW_TIME			2.0f
#define WPN_CHOOSER_FADE_OUT_ELAPSE		0.7f

#define WPN_CHOOSER_MAX_ALPHA			0xFF
#define WPN_CHOOSER_MIN_ALPHA			0x60

CHUDWpnChooser::CHUDWpnChooser()
{
	m_UpdateFlags	= kHUDFrame;
	m_fShowTime		= 0.0f;
}

LTBOOL CHUDWpnChooser::Init()
{
	this->RemoveAllWeapon();
	this->ScreenDimChanged();

	return LTTRUE;
}

void CHUDWpnChooser::Term()
{
	this->RemoveAllWeapon();
}

void CHUDWpnChooser::AddWeapon( int nClass, int nWeaponID, LTBOOL bSelected, LTBOOL bChangePos )
{
	ASSERT( nClass>=0 );
	ASSERT( nClass<MAX_WEAPON_COUNT );
	ASSERT( m_nWeaponCount<=MAX_WEAPON_COUNT);

	if( nWeaponID < 0 || LTFALSE == g_pGameClientShell->IsMainWindowFocused() ) return;

	float	fXRatio = g_pInterfaceResMgr->GetXRatio();
	float	fYRatio = g_pInterfaceResMgr->GetYRatio();

	m_WpnInfo[nClass].nWeaponID	= nWeaponID;
	
	/////////////////////////////////////////////////////////////////////////////////////////////////
	std::string icon;
	WEAPON const* pWeapon = g_pWeaponMgr->GetWeapon( nWeaponID );
	icon = pWeapon->GetSelectedIcon();

	m_WpnInfo[nClass].hTexture	= g_pInterfaceResMgr->GetTexture( icon.c_str() );
	g_pTexInterface->GetTextureDims( m_WpnInfo[nClass].hTexture,
									 m_WpnInfo[nClass].nTexW,
									 m_WpnInfo[nClass].nTexH );

	
	m_WpnInfo[nClass].fTexMinW		= m_WpnInfo[nClass].nTexW * WPN_CHOOSER_MIN_ICON_RATIO * fXRatio;
	m_WpnInfo[nClass].fTexMinH		= m_WpnInfo[nClass].nTexH * WPN_CHOOSER_MIN_ICON_RATIO * fYRatio;
	m_WpnInfo[nClass].fTexMaxW		= m_WpnInfo[nClass].nTexW * WPN_CHOOSER_REAL_ICON_RATION * fXRatio;
	m_WpnInfo[nClass].fTexMaxH		= m_WpnInfo[nClass].nTexH * WPN_CHOOSER_REAL_ICON_RATION * fYRatio;
	m_WpnInfo[nClass].fTexRealW		= m_WpnInfo[nClass].fTexMinW;
	m_WpnInfo[nClass].fTexRealH		= m_WpnInfo[nClass].fTexMinH;
	m_WpnInfo[nClass].fTexWHRatio	= (float)m_WpnInfo[nClass].nTexH / (float)m_WpnInfo[nClass].nTexW;

	SetupQuadUVs( m_WpnInfo[nClass].hPoly, m_WpnInfo[nClass].hTexture, 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_WpnInfo[nClass].hPoly, 0x00FFFFFF | WPN_CHOOSER_MIN_ALPHA << 24 );
	//////////////////////////////////////////////////////////////////////////////////////////////////

	++m_nWeaponCount;
	
	if( bSelected )
	{
		m_WpnInfo[nClass].fTexRealW		= m_WpnInfo[nClass].fTexMaxW;
		m_WpnInfo[nClass].fTexRealH		= m_WpnInfo[nClass].fTexMaxH;
		g_pDrawPrim->SetRGBA( &m_WpnInfo[nClass].hPoly, 0x00FFFFFF | WPN_CHOOSER_MAX_ALPHA << 24 );
		SelectFromClass( nClass );
	}

	if( bChangePos ) this->SetIconPos();
}

void CHUDWpnChooser::RemoveWeapon( int nClass )
{
	ASSERT( nClass>=0 );
	ASSERT( nClass<MAX_WEAPON_COUNT );
	ASSERT( m_nWeaponCount>0 );

	if( m_WpnInfo[nClass].nWeaponID != WMGR_INVALID_ID )
	{
		memset( &m_WpnInfo[nClass], 0, sizeof(T_WPN_CHOOSER) );
		m_WpnInfo[nClass].nWeaponID	= WMGR_INVALID_ID;

		if( m_nCurrentWeapon == nClass ) m_nCurrentWeapon = -1;

		--m_nWeaponCount;
		
		this->SetIconPos();
	}
}

void CHUDWpnChooser::RemoveAllWeapon()
{
	int i;
	for( i=0 ; i<MAX_WEAPON_COUNT ; ++i )
	{
		memset( &m_WpnInfo[i], 0, sizeof(T_WPN_CHOOSER) );
		m_WpnInfo[i].nWeaponID	= WMGR_INVALID_ID;
	}

	m_bChanging			= LTFALSE;
	m_nWeaponCount		= 0;
	m_nCurrentWeapon	= -1;
	m_nLastWeaponID		= -1;
}

void CHUDWpnChooser::SelectFromClass( int nClass )
{
	ASSERT( nClass>=0 );
	ASSERT( nClass<MAX_WEAPON_COUNT );
	
	if( nClass == m_nCurrentWeapon || LTNULL == m_WpnInfo[nClass].hTexture ) return;

	if( m_nCurrentWeapon >= 0 )
	{
		m_WpnInfo[m_nCurrentWeapon].bIncrease = LTFALSE;
		m_WpnInfo[m_nCurrentWeapon].bDecrease = LTTRUE;
		m_nLastWeaponID	= m_nCurrentWeapon;
	}

	m_nCurrentWeapon	= nClass;

	m_WpnInfo[m_nCurrentWeapon].bIncrease	= LTTRUE;
	m_WpnInfo[m_nCurrentWeapon].bDecrease	= LTFALSE;

	m_bChanging	= LTTRUE;
}

void CHUDWpnChooser::SelectFromID( int nWeaponID )
{
	if ( m_nCurrentWeapon>0 && nWeaponID == m_WpnInfo[m_nCurrentWeapon].nWeaponID ) 
		return;
	
	int i;

	for ( i = 0 ; i < MAX_WEAPON_COUNT ; ++i )
	{
		if ( m_WpnInfo[i].hTexture && m_WpnInfo[i].nWeaponID == nWeaponID )
		{
			SelectFromClass(i);
		}
	}
}

void CHUDWpnChooser::LastWeapon()
{
	if ( m_nLastWeaponID < 0 )
		return;


	SelectFromClass( m_nLastWeaponID );
}

void CHUDWpnChooser::SetIconPos()
{
	int	i, nPosX, nPosY, nTotalWidth = 0;

	for ( i = 0 ; i < MAX_WEAPON_COUNT ; ++i )
	{
		if ( m_WpnInfo[i].hTexture )
		{
			nTotalWidth += (int)( m_WpnInfo[i].fTexRealW );
		}
	}

	nPosX	= (int)( m_BasePos.x - ( ( nTotalWidth + m_nIconInterval * (m_nWeaponCount - 1) ) * 0.5f ) );

	for ( i = 0 ; i < MAX_WEAPON_COUNT ; ++i )
	{
		nPosY	= ( m_WpnInfo[i].nWeaponID == m_WpnInfo[m_nCurrentWeapon].nWeaponID ) ? m_BasePos.y - 30 : m_BasePos.y;

		if ( m_WpnInfo[i].hTexture )
		{
			g_pDrawPrim->SetXYWH( &m_WpnInfo[i].hPoly, (float)nPosX, (float)nPosY,
								  m_WpnInfo[i].fTexRealW, m_WpnInfo[i].fTexRealH );

			nPosX += (int)( m_nIconInterval + m_WpnInfo[i].fTexRealW );
		}
	}
}

void CHUDWpnChooser::Update()
{
	if( LTFALSE == m_bChanging )
	{
		m_fShowTime += g_pLTClient->GetFrameTime();
		return;
	}

	m_fShowTime = 0.0f;

	m_bChanging	= LTFALSE;
	float	fFrameTime = g_pLTClient->GetFrameTime();

	int i;
	for( i=0 ; i<MAX_WEAPON_COUNT ; ++i )
	{
		if( m_WpnInfo[i].hTexture )
		{
			if( m_WpnInfo[i].bIncrease )
			{
				m_WpnInfo[i].fTexRealW	+= fFrameTime * WPN_CHOOSER_VEL;
				m_WpnInfo[i].fTexRealH	+= fFrameTime * WPN_CHOOSER_VEL * m_WpnInfo[i].fTexWHRatio;
				if( m_WpnInfo[i].fTexRealW >= m_WpnInfo[i].fTexMaxW )
				{
					g_pDrawPrim->SetALPHA( &m_WpnInfo[i].hPoly, WPN_CHOOSER_MAX_ALPHA );
					m_WpnInfo[i].fTexRealW	= m_WpnInfo[i].fTexMaxW;
					m_WpnInfo[i].fTexRealH	= m_WpnInfo[i].fTexMaxH;
					m_WpnInfo[i].bIncrease	= LTFALSE;
				}
				else
				{
					//
					m_bChanging	= LTTRUE;
				}
			}
			else if( m_WpnInfo[i].bDecrease )
			{
				m_WpnInfo[i].fTexRealW	-= fFrameTime * WPN_CHOOSER_VEL;
				m_WpnInfo[i].fTexRealH	-= fFrameTime * WPN_CHOOSER_VEL * m_WpnInfo[i].fTexWHRatio;
				if( m_WpnInfo[i].fTexRealW <= m_WpnInfo[i].fTexMinW )
				{
					g_pDrawPrim->SetALPHA( &m_WpnInfo[i].hPoly, WPN_CHOOSER_MIN_ALPHA );
					m_WpnInfo[i].fTexRealW	= m_WpnInfo[i].fTexMinW;
					m_WpnInfo[i].fTexRealH	= m_WpnInfo[i].fTexMinH;
					m_WpnInfo[i].bDecrease	= LTFALSE;
				}
				else
				{
					//
					m_bChanging	= LTTRUE;
				}
			}
		}
	}

	SetIconPos();
}

void CHUDWpnChooser::Render()
{
	if( m_nWeaponCount <= 0 ) return;
	if( m_fShowTime > WPN_CHOOSER_SHOW_TIME + WPN_CHOOSER_FADE_OUT_ELAPSE )	return;

	int nColor = 0;
	
	// WPN_CHOOSER_SHOW_TIME 보다 시간이 지난 상황에서,
	// WPN_CHOOSER_FADE_OUT_ELAPSE 동안 Fade Out 시킨다.
	if ( m_fShowTime > WPN_CHOOSER_SHOW_TIME )
	{
		// 시간에 따른 Alpha 변동값..
		// WPN_CHOOSER_MAX_ALPHA 에서 0 으로 조금씩 변한다.
		LTFLOAT fColor = WPN_CHOOSER_MAX_ALPHA - ( (m_fShowTime - WPN_CHOOSER_SHOW_TIME) * WPN_CHOOSER_MAX_ALPHA / WPN_CHOOSER_FADE_OUT_ELAPSE );
		nColor = (int)fColor;
	}
	else
	{
		nColor = WPN_CHOOSER_MAX_ALPHA;
	}

	int i;
	for( i=0 ; i<MAX_WEAPON_COUNT ; ++i )
	{
		if( m_WpnInfo[i].hTexture )
		{
			g_pDrawPrim->SetTexture( m_WpnInfo[i].hTexture );
			g_pDrawPrim->SetRGBA( &m_WpnInfo[i].hPoly, 0x00FFFFFF | nColor << 24 );
			g_pDrawPrim->DrawPrim( &m_WpnInfo[i].hPoly );			
		}
	}
}

void CHUDWpnChooser::ScreenDimChanged()
{
	float	fXRatio = g_pInterfaceResMgr->GetXRatio();
	float	fYRatio = g_pInterfaceResMgr->GetYRatio();

	m_BasePos.x	= int( ( USABLE_WIDTH_I * 0.5f + 20 ) * fXRatio );
	m_BasePos.y	= int( 570 * fYRatio );
	m_nIconInterval	= int( -50 * fXRatio );

	int i;
	for( i=0 ; i<MAX_WEAPON_COUNT ; ++i )
	{
		if( m_WpnInfo[i].hTexture )
		{
			m_WpnInfo[i].fTexMinW	= m_WpnInfo[i].nTexW * WPN_CHOOSER_MIN_ICON_RATIO * fXRatio;
			m_WpnInfo[i].fTexMinH	= m_WpnInfo[i].nTexH * WPN_CHOOSER_MIN_ICON_RATIO * fYRatio;
			m_WpnInfo[i].fTexMaxW	= m_WpnInfo[i].nTexW * WPN_CHOOSER_REAL_ICON_RATION * fXRatio;
			m_WpnInfo[i].fTexMaxH	= m_WpnInfo[i].nTexH * WPN_CHOOSER_REAL_ICON_RATION * fYRatio;
			m_WpnInfo[i].fTexRealW	= m_WpnInfo[i].fTexMinW;
			m_WpnInfo[i].fTexRealH	= m_WpnInfo[i].fTexMinH;
		}
	}

	SetIconPos();
}

void CHUDWpnChooser::ClearLastWeapon()
{
	m_nLastWeaponID = -1;
}