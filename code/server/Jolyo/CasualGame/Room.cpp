#include "StdAfx.h"
#include ".\Room.h"

#include "CasualPacket.h"

using namespace CasualGame;

CRoom::CRoom()
:	m_nMaxUser				( 0		)
,	m_nRoomIndex			( -1	)
,	m_nChannelIndex			( -1	)
,	m_pCaptine				( NULL	)
{
	m_InnerObjectForGame	.Init( this	);
	m_InnerObjectForChannel	.Init( this	);
}

CRoom::~CRoom()
{
}

void CRoom::Init( INT nChannelIndex )
{
	m_nChannelIndex			= nChannelIndex;

	m_InnerManagerUser.Init();

	m_pCaptine = NULL;
}

void CRoom::Open( INT nMaxUser, LPCTSTR strRoomName )
{
//	ASSERT( MaxUser >= nMaxUser );

	m_nMaxUser		= nMaxUser;
	m_strRoomName	= strRoomName;
	m_pCaptine		= NULL;

	m_bPassword		= FALSE;

//	m_nUsingIndex	= 0;
//	for( INT i = 0; i < MaxUser; i++ )
	for( INT i = 0; i < MaxUser; i++ )
	{
		if( i < nMaxUser )
			m_aUserIndex[i] = InnerIndexType::Empty;
		else
			m_aUserIndex[i] = InnerIndexType::Close;
	}
}

void CRoom::Open( INT nMaxUser, LPCTSTR strRoomName, LPCTSTR strPassword )
{
	ASSERT( MaxUser >= nMaxUser );

	m_nMaxUser		= nMaxUser;
	m_strRoomName	= strRoomName;
	m_pCaptine		= NULL;

	m_strPassword	= strPassword;
	m_bPassword		= TRUE;

//	m_nUsingIndex	= 0;
	for( INT i = 0; i < nMaxUser; i++ )
	{
		if( i < nMaxUser )
			m_aUserIndex[i] = InnerIndexType::Empty;
		else
			m_aUserIndex[i] = InnerIndexType::Close;
	}
}

// Inner Index를 할당받는다.
CHAR
CRoom::AllocEmptyInnerIndex()
{
	CHAR nRet = -1;
	for( CHAR i = 0; i < MaxUser; i++ )
	{
		if( m_aUserIndex[i] == InnerIndexType::Empty )
		{
			m_aUserIndex[i] = InnerIndexType::User;
			nRet = i;
			break;
		}
	}
	
	ASSERT( nRet >= 0 );
	return nRet;
}

// Inner Index를 제거한다.
VOID
CRoom::FreeEmptyInnerIndex( INT nIndex )
{
	ASSERT( nIndex < MaxUser );
	ASSERT( m_aUserIndex[nIndex] == InnerIndexType::User );

	m_aUserIndex[nIndex] = InnerIndexType::Empty;
}

// Inner Index를 사용못하게 한다.
BOOL
CRoom::SetToCloseInnerIndex( INT nIndex )
{
	ASSERT( nIndex < MaxUser );
	if( m_aUserIndex[nIndex] != InnerIndexType::Empty )
		return FALSE;

	m_aUserIndex[nIndex] = InnerIndexType::Close;
//	m_nUsingIndex++;
	m_nMaxUser--;
	return TRUE;
}

// Inner Index를 사용하게 한다.
VOID
CRoom::ResetToCloseInnerIndex( INT nIndex )
{
	ASSERT( nIndex < MaxUser );
	ASSERT( m_aUserIndex[nIndex] == InnerIndexType::Close );

//	m_nUsingIndex--;
	m_nMaxUser++;
	m_aUserIndex[nIndex] = InnerIndexType::Empty;
}


// 방에 입장을 한다. 
BOOL CRoom::Join( CUser* pUser )
{
//	if( m_InnerManagerUser.GetNumOfInnerObject() + m_nUsingIndex == m_nMaxUser )
	if( m_InnerManagerUser.GetNumOfInnerObject() == m_nMaxUser )
		return FALSE;

	BOOL bRet = m_InnerManagerUser.Add( pUser->GetInnerObjectForRoom() );

	return bRet;
}

// 방에서 나간다. : nIndex(룸안에서의 자신의 번호)
BOOL CRoom::Leave( CUser* pUser )
{
	BOOL bRet = m_InnerManagerUser.Remove( pUser->GetInnerObjectForRoom() );

	return bRet;
}

// 방의 첫번째 사용자를 가져온다.
CUser* CRoom::GetFirstUser()
{
	return (CUser*)m_InnerManagerUser.GetFirstInnerObject();
}

// 방의 다음 클라이언트 서비서를 가져온다.
CUser* CRoom::GetNextUser()
{
	return (CUser*)m_InnerManagerUser.GetNextInnerObject();
}