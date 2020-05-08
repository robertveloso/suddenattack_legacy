// ----------------------------------------------------------------------- //
//
// MODULE  : LTGUICtrl.cpp
//
// PURPOSE : Base clase for controls
//
// (c) 2001 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#include "ltguimgr.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLTGUICtrl::CLTGUICtrl()
{
    m_bSelected     = LTFALSE;
    m_bCreated      = LTFALSE;
    m_bEnabled      = LTTRUE;
    m_bVisible      = LTTRUE;
	m_bClicked		= LTFALSE;	// ±¤½Ä
	m_nCommandID	= 0;
	m_nHelpID		= 0;
	m_nParam1		= 0;
	m_nParam2		= 0;
	m_pos.x			= 0;
	m_pos.y			= 0;
	m_basePos.x		= 0;
	m_basePos.y		= 0;
	m_fScale		= 1.0f;

    m_argbSelected	= 0xFFFFFFFF;
    m_argbNormal	= 0xFF000000;
    m_argbDisabled	= 0xFF808080;

	// Added by IN'T ==>
	m_baseWidth		= 0;
	m_baseHeight	= 0;
	// Added by IN'T <==
	
	m_bEnableTooltip	= LTFALSE;
	m_nHotKey1			= -1;
	m_nHotKey2			= -1;
}

CLTGUICtrl::~CLTGUICtrl()
{
}

void CLTGUICtrl::SetTooltip( const char* lpszTooltip )
{
	if( lpszTooltip )
	{
		lstrcpy( m_szTooltipText, lpszTooltip );
		m_bEnableTooltip	= LTTRUE;
	}
	else
	{
		m_bEnableTooltip	= LTFALSE;
	}
}

const char* CLTGUICtrl::GetTooptip()
{
	return m_szTooltipText;
}

void CLTGUICtrl::SetHotKey( short HotKey1, short HotKey2 )
{
	m_nHotKey1	= HotKey1;
	m_nHotKey2	= HotKey2;
}

LTBOOL CLTGUICtrl::IsHotKey( unsigned char HotKey )
{
	if( !IsEnabled() || !IsVisible() ) return LTFALSE;
	if( m_nHotKey1 == HotKey || m_nHotKey2 == HotKey ) return LTTRUE;

	return LTFALSE;
}

// Select a control
void CLTGUICtrl::Select(LTBOOL bSelected)
{
	if (IsSelected() != bSelected)
	{
		m_bSelected=bSelected;
		OnSelChange();
	}
}

// Click a control : disable µÇÁö ¾Ê¾ÒÀ» ¶§¸¸ click message Ã³¸® [±¤½Ä]
void CLTGUICtrl::Click(LTBOOL  bClicked)
{
	if( m_bClicked != bClicked && !IsDisabled() )
	{
		m_bClicked = bClicked;
	}
}
// ±¤½Ä
LTBOOL CLTGUICtrl::IsOnMe(int x, int y)
{
	if (!m_bVisible) return LTFALSE;

	LTBOOL bResult = (x >= m_pos.x && y >= m_pos.y &&
					  x <= (m_pos.x + GetWidth()) &&
					  y <= (m_pos.y + GetHeight()) );
	if( !bResult )
	{
		Click(LTFALSE);
	}

	return bResult;
}
/*
LTBOOL CLTGUICtrl::IsOnMe(int x, int y)
{
	if (!m_bVisible) return LTFALSE;

	return (x >= m_pos.x && y >= m_pos.y &&
			x <= (m_pos.x + GetWidth()) &&
			y <= (m_pos.y + GetHeight()) );
}
*/

void CLTGUICtrl::SetScale(float fScale)
{
	m_pos.x = (int)(fScale * (float)m_basePos.x);
	m_pos.y = (int)(fScale * (float)m_basePos.y);
	m_fScale = fScale;
}

LTBOOL   
CLTGUICtrl::OnWheelUp(int x, int y)
{
	return LTFALSE;
}

LTBOOL   
CLTGUICtrl::OnWheelDown(int x, int y)
{
	return LTFALSE;
}


CLTGUIDummyCtrl::CLTGUIDummyCtrl()
{
	g_pDrawPrim->SetXYWH( &m_Poly, 0.0f, 0.0f, 800.0f, 600.0f );
	g_pDrawPrim->SetRGBA( &m_Poly, 0x80000000 );
}

LTBOOL
CLTGUIDummyCtrl::IsOnMe( int x, int y )
{
	return LTTRUE;
}

void CLTGUIDummyCtrl::Render()
{
	if( IsVisible() )
	{
		g_pDrawPrim->SetTexture( LTNULL );
		g_pDrawPrim->DrawPrim( &m_Poly );
	}
}