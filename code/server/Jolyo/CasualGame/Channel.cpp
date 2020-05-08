#include "StdAfx.h"
#include ".\Channel.h"

#include "CasualPacket.h"

using namespace CasualGame;

void CChannel::Init	( INT nMaxUser, INT nMaxRoom, const string& strChannelName, INT nChannelIndex )
{
	m_nMaxUser			= nMaxUser;
	m_nMaxRoom			= nMaxRoom;
	m_strChannelName	= strChannelName;
	m_nChannelIndex		= nChannelIndex;

	m_InnerManagerRoom.Init();
	m_InnerManagerUser.Init();
}

BOOL CChannel::AddRoom( CRoom* pRoom )
{
	if( m_nMaxRoom <= m_InnerManagerRoom.GetNumOfInnerObject() )
		return FALSE;

	BOOL bRet = m_InnerManagerRoom.Add( pRoom->GetInnerObjectForChannel() );

	return bRet;
}

BOOL CChannel::RemoveRoom( CRoom* pRoom )
{
	BOOL bRet = m_InnerManagerRoom.Remove( pRoom->GetInnerObjectForChannel() );

	return bRet;
}

// 채널에 입장을 한다.
BOOL CChannel::Join( CUser* pUser )
{
	if( m_nMaxUser <= m_InnerManagerUser.GetNumOfInnerObject() )
		return FALSE;

	BOOL bRet = m_InnerManagerUser.Add( pUser->GetInnerObjectForChannel() );

	return bRet;
}

// 채널에서 나간다.
BOOL CChannel::Leave( CUser* pUser )
{
	BOOL bRet = m_InnerManagerUser.Remove( pUser->GetInnerObjectForChannel() );

	return bRet;
}
