#include "StdAfx.h"

#include "UserMap.h"


using namespace CasualGame;
using namespace std;

CUser* CUserMap::FindFromUserNo( LPCTSTR strUserNo )
{
	m_IterUserNo = m_mapUserNo.find( strUserNo );
	
	if( m_IterUserNo == m_mapUserNo.end() )
		return NULL;
	else
		return m_IterUserNo->second;
}

CUser* CUserMap::FindFromUserNo( const string& strUserNo )
{
	m_IterUserNo = m_mapUserNo.find( strUserNo );
	
	if( m_IterUserNo == m_mapUserNo.end() )
		return NULL;
	else
		return m_IterUserNo->second;
}

CUser* CUserMap::FindFromID( LPCTSTR lpszID )
{
	m_IterUserID = m_mapUserID.find( lpszID );
	
	if( m_IterUserID == m_mapUserID.end() )
		return NULL;
	else
		return m_IterUserID->second;
}

CUser* CUserMap::FindFromID( const string& strID )
{
	m_IterUserID = m_mapUserID.find( strID );
	
	if( m_IterUserID == m_mapUserID.end() )
		return NULL;
	else
		return m_IterUserID->second;
}

CUser* CUserMap::FindFromNick( LPCTSTR lpszNick )
{
	m_IterUserNick = m_mapUserNick.find( lpszNick );
	
	if( m_IterUserNick == m_mapUserNick.end() )
		return NULL;
	else
		return m_IterUserNick->second;
}

CUser* CUserMap::FindFromNick( const string& strNick )
{
	m_IterUserNick = m_mapUserNick.find( strNick );
	
	if( m_IterUserNick == m_mapUserNick.end() )
		return NULL;
	else
		return m_IterUserNick->second;
}


void CUserMap::Insert( LPCTSTR lpszUserNo, LPCTSTR lpszID, LPCTSTR lpszNick, CUser* pUser )
{
	m_mapUserNo.insert( pair<string, CUser*>(lpszUserNo, pUser) );
	m_mapUserID.insert( pair<string, CUser*>(lpszID, pUser) );
	m_mapUserNick.insert( pair<string, CUser*>(lpszNick, pUser) );
}

void CUserMap::Insert( const string& strUserNo, const string& strID, const string& strNick, CUser* pUser )
{
	m_mapUserNo.insert( pair<string, CUser*>(strUserNo, pUser) );
	m_mapUserID.insert( pair<string, CUser*>(strID, pUser) );
	m_mapUserNick.insert( pair<string, CUser*>(strNick, pUser) );
}

void CUserMap::EraseFromUserNo( CUser* pUser )
{
	m_mapUserNo.erase( pUser->GetUserNo() );

	m_IterUserID = m_mapUserID.find( pUser->GetID() );

	while( m_IterUserID != m_mapUserID.end() )
	{
		if( m_IterUserID->second == pUser )
		{
			m_mapUserID.erase( m_IterUserID );
			break;
		}

		m_IterUserID++;
	}

	m_IterUserNick = m_mapUserNick.find( pUser->GetNick() );
	while( m_IterUserNick != m_mapUserNick.end() )
	{
		if( m_IterUserNick->second == pUser )
		{
			m_mapUserNick.erase( m_IterUserNick );
			break;
		}
		m_IterUserNick++;
	}
 }

void CUserMap::EraseFromID( CUser* pUser )
{
	m_mapUserID.erase( pUser->GetID() );

	m_IterUserNo = m_mapUserNo.find( pUser->GetUserNo() );
	while( m_IterUserNo != m_mapUserNo.end() )
	{
		if( m_IterUserNo->second == pUser )
		{
			m_mapUserNo.erase( m_IterUserNo );
			break;
		}
		m_IterUserNo++;
	}

	m_IterUserNick = m_mapUserNick.find( pUser->GetNick() );
	while( m_IterUserNick != m_mapUserNick.end() )
	{
		if( m_IterUserNick->second == pUser )
		{
			m_mapUserNick.erase( m_IterUserNick );
			break;
		}
		m_IterUserNick++;
	}
}
