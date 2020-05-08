#include "StdAfx.h"

#include "./Grade.h"
#include "../../../Dispatcher/IConsole.h"

namespace GameServer {		namespace Player {			namespace Result {

Grade::Grade()
{
	m_nCount				= 0;
	for( INT i = 0; i < s_nMax; i++ )
	{
		m_stStock[ i ].bUse	= FALSE;
	}
	this->install();
}

Grade::~Grade()
{
	this->uninstall();
}

void
Grade::Add( CINT64 i_nMinExp, CINT64 i_nMaxExp, CBOOL i_bAbs )
{
	if( m_nCount == s_nMax )
	{
		STRING strLog;
		strLog.Printf( "Dev\tGrade Full : %d\r\n", m_nCount );
		::Dispatcher::ICONSOLE()->Write( strLog );
		DEBUG_BREAK;
		return;
	}

	if( m_stStock[ m_nCount ].bUse == TRUE )
	{
		STRING strLog;
		strLog.Printf( "Dev\tExist Grade : %d\r\n", m_nCount );
		::Dispatcher::ICONSOLE()->Write( strLog );
		DEBUG_BREAK;
		return;
	}

	m_stStock[ m_nCount ].nMinExp	= i_nMinExp;
	m_stStock[ m_nCount ].nMaxExp	= i_nMaxExp;
	m_stStock[ m_nCount ].bAbs		= i_bAbs;
	m_stStock[ m_nCount ].bUse		= TRUE;
	m_nCount ++;
}

CBYTE
Grade::Get( CINT64 i_nExp, CBYTE i_byCurrentGrade )
{
	INT nGradeAbs			= 0;
	INT nGrade				= 0;
	for( INT i = 0; i < m_nCount; i++ )
	{
		if( m_stStock[ i ].bAbs == TRUE )
		{
			nGradeAbs		= i;
		}
		if( m_stStock[ i ].nMinExp <= i_nExp &&
			i_nExp <= m_stStock[ i ].nMaxExp )
		{
			nGrade			= i;
			break;
		}
	}

	if( nGradeAbs == nGrade )
	{
		return static_cast<BYTE>( nGrade );
	}

	if( i_byCurrentGrade > nGrade )
	{
		return static_cast<BYTE>( nGrade );
	}

	return static_cast<BYTE>( i_byCurrentGrade );
}

} /* Result */				} /* Player */				} /* GameServer */