#include "StdAfx.h"

#include "KillTimerMap.h"

using namespace Session::Gate;
using namespace std;

void CKillTimerMap::Insert( ULONG_PTR lpTimerKey )
{
	m_mapKillTimer.insert( pair<ULONG_PTR, ULONG_PTR>(lpTimerKey, lpTimerKey) );
}

BOOL CKillTimerMap::Erase( ULONG_PTR lpTimerKey )
{
	m_IterKillTimer = m_mapKillTimer.find( lpTimerKey );
	
	if( m_IterKillTimer == m_mapKillTimer.end() )
		return FALSE;
	else
	{
		m_mapKillTimer.erase( m_IterKillTimer );
		return TRUE;
	}
}
