#include "StdAfx.h"

#include "./Provider.h"

namespace GameServer {		namespace Server {			namespace Rank {

Provider::Provider()
{
	m_nTick					= 60;
	m_bLoaded				= FALSE;
}

Provider::~Provider()
{
}

void
Provider::SetRankTick( CINT nTick )
{
//	Assert( nTick > 60 );
	m_nTick					= nTick;
}

CINT
Provider::GetRankTick()
{
	return m_nTick;
}

void
Provider::SetRank( CSTRING & strRank )
{
	m_bLoaded				= TRUE;
	m_strRank				= strRank;
}

CSTRING *
Provider::GetRank()
{
	if( m_bLoaded == FALSE )
	{
		return NULL;
	}

	return & m_strRank;
}

} /* Rank */				} /* Server */				} /* GameServer */
