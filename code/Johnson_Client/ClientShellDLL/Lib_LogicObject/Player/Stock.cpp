#include "StdAfx.h"

#include "./Stock.h"

namespace LogicObject {		namespace Player {

void
Stock::SetNick( const char * i_strNick )
{
	for( int i = 0; i < TYPE::s_nUserNick; i++ )
	{
		m_strNick[ i ]		= i_strNick[ i ];
		if( m_strNick[ i ] == '\0' )
		{
			break;
		}
	}
	m_strNick[ TYPE::s_nUserNick - 1 ] = '\0';
}

const char *
Stock::GetNick() const
{
	return m_strNick;
}

} /* Player */				} /* LogicObject */