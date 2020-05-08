#include "ltguimgr.h"

CLTGUIDragButton::CLTGUIDragButton()
: CLTGUIButton()
{
	m_ptClickDelta.x	= 0;
	m_ptClickDelta.y	= 0;
	m_bLockX			= LTFALSE;
	m_bLockY			= LTFALSE;

	m_nMinLimitX		= -10000;
	m_nMaxLimitX		= 10000;
	m_nMinLimitY		= -10000;
	m_nMaxLimitY		= 10000;

	m_bLButtonDown		= LTFALSE;
}

CLTGUIDragButton::~CLTGUIDragButton()
{
	Destroy();
}

LTBOOL CLTGUIDragButton::OnLButtonDown( int x, int y )
{
	m_ptClickDelta.x	= x - m_pos.x;
	m_ptClickDelta.y	= y - m_pos.y;
	m_bLButtonDown		= LTTRUE;

	m_pCommandHandler->SendCommand( m_nCommandID, CMD_PARAM_LDOWN, LTNULL );
	return LTTRUE;//CLTGUIButton::OnLButtonDown( x, y );
}

LTBOOL CLTGUIDragButton::OnLButtonUp( int x, int y )
{
	m_bLButtonDown	= LTFALSE;
	Select( LTFALSE );

	m_pCommandHandler->SendCommand( m_nCommandID, CMD_PARAM_LUP, LTNULL );
	return LTTRUE;//CLTGUIButton::OnLButtonUp( x, y );
}

LTBOOL CLTGUIDragButton::OnMouseMove( int x, int y )
{
	if( m_bLButtonDown )
	{		
		if( !m_bLockX && m_pos.x != (x-m_ptClickDelta.x) )
		{
			m_pos.x = x - m_ptClickDelta.x;
			if( m_pos.x < m_nMinLimitX )
			{
				m_pos.x = m_nMinLimitX;
			}
			else if( m_pos.x > m_nMaxLimitX )
			{
				m_pos.x = m_nMaxLimitX;
			}
		}

		if( !m_bLockY && m_pos.y != (y-m_ptClickDelta.y) )
		{
			m_pos.y = y - m_ptClickDelta.y;
			if( m_pos.y < m_nMinLimitY )
			{
				m_pos.y = m_nMinLimitY;
			}
			else if( m_pos.y > m_nMaxLimitY )
			{
				m_pos.y = m_nMaxLimitY;
			}
		}
		
		SetSize( GetWidth(), GetHeight() );
		SetBasePos( m_pos.x, m_pos.y );
		CLTGUICtrl::SetScale(1.0f);

		m_pCommandHandler->SendCommand( m_nCommandID, CMD_PARAM_MOUSEMOVE, LTNULL );
	}

	return LTTRUE;//CLTGUIButton::OnMouseMove( x, y );
}

LTBOOL   
CLTGUIDragButton::OnWheelUp(int x, int y)
{
	return LTFALSE;
}

LTBOOL   
CLTGUIDragButton::OnWheelDown(int x, int y)
{
	return LTFALSE;
}

void CLTGUIDragButton::OnSelChange()
{
	if( m_bSelected == LTFALSE ) m_bLButtonDown	= LTFALSE;
}

LTBOOL CLTGUIDragButton::IsOnMe( int x, int y )
{
	if (!m_bVisible) return LTFALSE;

	if( this->IsSelected() && m_bLButtonDown ) return LTTRUE;
	
	return (x >= m_pos.x && y >= m_pos.y &&
			x <= (m_pos.x + GetWidth()) &&
			y <= (m_pos.y + GetHeight()) );
}

void CLTGUIDragButton::SetLockX( LTBOOL bLock )
{
	m_bLockX = bLock;
}

void CLTGUIDragButton::SetLockY( LTBOOL bLock )
{
	m_bLockY = bLock;
}

void CLTGUIDragButton::SetLimitX( int32 nMinX, int32 nMaxX )
{
	m_nMinLimitX	= nMinX;
	m_nMaxLimitX	= nMaxX;
}

void CLTGUIDragButton::SetLimitY( int32 nMinY, int32 nMaxY )
{
	m_nMinLimitY	= nMinY;
	m_nMaxLimitY	= nMaxY;
}

float CLTGUIDragButton::GetPosXRatio()
{
	return ((float)(m_pos.x-m_nMinLimitX) / (float)(m_nMaxLimitX - m_nMinLimitX ));
}

float CLTGUIDragButton::GetPosYRatio()
{
	return ((float)(m_pos.y-m_nMinLimitY) / (float)(m_nMaxLimitY - m_nMinLimitY ));
}

void CLTGUIDragButton::SetPosXRatio( float fXRatio )
{
	if( !m_bLockX && fXRatio >= 0.0f)
	{
		if(fXRatio > 1.0f) fXRatio = 1.0f;
		m_pos.x = (int)( (m_nMaxLimitX-m_nMinLimitX)*fXRatio + m_nMinLimitX );
		SetSize( GetWidth(), GetHeight() );
		SetBasePos( m_pos.x, m_pos.y );
		CLTGUICtrl::SetScale(1.0f);
	}
}

void CLTGUIDragButton::SetPosYRatio( float fYRatio )
{
	if( !m_bLockY && fYRatio >= 0.0f )
	{
		if(fYRatio > 1.0f) fYRatio = 1.0f;
		m_pos.y = (int)( (m_nMaxLimitY-m_nMinLimitY)*fYRatio + m_nMinLimitY );
		SetSize( GetWidth(), GetHeight() );
		SetBasePos( m_pos.x, m_pos.y );
		CLTGUICtrl::SetScale(1.0f);
	}
}


void CLTGUIDragButton::SetBasePos( LTIntPt pos )
{
	CLTGUIButton::SetBasePos( pos );
}

void CLTGUIDragButton::SetBasePos( int x, int y )
{
	CLTGUIButton::SetBasePos( x, y );
}
