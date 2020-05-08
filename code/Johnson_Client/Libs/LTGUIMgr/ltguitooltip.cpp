#include "ltguimgr.h"

CLTGUITooltip::CLTGUITooltip()
{
}

CLTGUITooltip::~CLTGUITooltip()
{
	this->Term();
}

LTBOOL CLTGUITooltip::CreateTooltip( uint16 nWidth, uint16 nHeight )
{
	m_baseWidth		= nWidth;
	m_baseHeight	= nHeight;

	this->Create( LTNULL, LTNULL, LTNULL, LTNULL );
	return LTTRUE;
}

void CLTGUITooltip::CtrlShow( LTIntPt& Pos )
{
	m_basePos = Pos;

	if( m_basePos.x < 0 ) m_basePos.x = 0;
	if( m_basePos.y < 0 ) m_basePos.y = 0;

	if( m_basePos.x > 800-m_baseWidth ) m_basePos.x = 800-m_baseWidth;
	if( m_basePos.y > 600-m_baseHeight ) m_basePos.y = 600-m_baseHeight;

	OnPosChanged();
	
	this->Show( LTTRUE );
}

void CLTGUITooltip::CtrlHide()
{
	this->Show( LTFALSE );
}

void CLTGUITooltip::Render()
{
}



