#include "StdAfx.h"
#include ".\myuserpocket.h"

CMyUserPocket::CMyUserPocket(void)
{
}

CMyUserPocket::~CMyUserPocket(void)
{
}

void CMyUserPocket::InitPocket( CUser* pUser, T_USER_INFOptr pUserInfo )
{
	if( !pUser || !pUserInfo ) return;
	m_pUser	= pUser;
	memcpy( &m_UserInfo, pUserInfo, sizeof(T_USER_INFO) );

	m_pUser->SetExp( m_UserInfo.nExp );
	m_pUser->SetGrade( m_UserInfo.nGrade );

	this->InitGameInfo();
}

void CMyUserPocket::InitGameRoomInfo()
{
	m_UserInfo.nState				= PLAYER_STATE_WAITING;
}

void CMyUserPocket::InitGameInfo()
{
	memset( &m_UserGameInfo, 0, sizeof(T_USER_GAME_INFO) );
	m_UserGameInfo.bCanBeCaptain	= TRUE;	
	m_bP2PHost						= FALSE;
}

void CMyUserPocket::InitRoundInfo()
{
	memset( &m_UserRoundInfo, 0, sizeof(T_USER_ROUND_INFO) );
	m_UserRoundInfo.dwStartTick	= timeGetTime();
	m_bSpectator	= FALSE;
}

BOOL CMyUserPocket::SetUserInfo( T_USER_INFOptr pUserInfo )
{
	if( !pUserInfo ) return FALSE;

	memcpy( &m_UserInfo, pUserInfo, sizeof(T_USER_INFO) );

	return TRUE;
}

T_USER_INFOptr CMyUserPocket::GetUserInfo()
{
	return &m_UserInfo;
}

T_USER_GAME_INFOptr CMyUserPocket::GetUserGameInfo()
{
	return &m_UserGameInfo;
}

CUser* CMyUserPocket::GetUserClass()
{
	return m_pUser;
}

void CMyUserPocket::SetUserTeam( int nTeam )
{
	m_nTeam	= nTeam;
}

int	CMyUserPocket::GetUserTeam()
{
	return m_nTeam;
}

void CMyUserPocket::SetSpectatorMode( BOOL bEnable )
{
	m_bSpectator	= bEnable;
}

BOOL CMyUserPocket::IsSpectator()
{
	return m_bSpectator;
}

void CMyUserPocket::SetState( int nState )
{
	m_UserInfo.nState	= nState;
}

void CMyUserPocket::AddState( int nState )
{
	m_UserInfo.nState	|= nState;
}

void CMyUserPocket::RemoveState( int nState )
{
	m_UserInfo.nState	&= ~nState;
}

void CMyUserPocket::SetP2PHost( BOOL bHost )
{
	m_bP2PHost	= bHost;
}

BOOL CMyUserPocket::IsP2PHost()
{
	return m_bP2PHost;
}

BOOL CMyUserPocket::IsCaptain()
{
	return !!(m_UserInfo.nState&PLAYER_STATE_CAPTAIN);
}

BOOL CMyUserPocket::IsGaming()
{
	return !!(m_UserInfo.nState&PLAYER_STATE_GAMING);
}

BOOL CMyUserPocket::IsLoading()
{
	return !!(m_UserInfo.nState&PLAYER_STATE_LOADING);
}

BOOL CMyUserPocket::IsWaiting()
{
	return !!(m_UserInfo.nState&PLAYER_STATE_WAITING);
}

BOOL CMyUserPocket::IsReady()
{
	return !!(m_UserInfo.nState&PLAYER_STATE_READY);
}

BOOL CMyUserPocket::IsLobby()
{
	return (m_UserInfo.nState == PLAYER_STATE_LOBBY );
}

__int64 CMyUserPocket::GetExp()
{
	return m_UserInfo.nExp;
}

UINT CMyUserPocket::GetGrade()
{
	return m_UserInfo.nGrade;
}

__int64 CMyUserPocket::GetMoney()
{
	return m_UserInfo.nMoney;
}

UINT CMyUserPocket::GetKillCount()
{
	return m_UserInfo.nKill;
}

UINT CMyUserPocket::GetDeadCount()
{
	return m_UserInfo.nDead;
}

UINT CMyUserPocket::GetHKillCount()
{
	return m_UserInfo.nHeadShotKill;
}

UINT CMyUserPocket::GetHDeadCount()
{
	return m_UserInfo.nHeadShotDead;
}

UINT CMyUserPocket::GetWinCount()
{
	return m_UserInfo.nWin;
}

UINT CMyUserPocket::GetLoseCount()
{
	return m_UserInfo.nLose;
}

UINT CMyUserPocket::GetDrawCount()
{
	return m_UserInfo.nDraw;
}

UINT CMyUserPocket::GetWinPercent()
{
	UINT nGameCount = m_UserInfo.nWin + m_UserInfo.nDraw + m_UserInfo.nLose;
	if( nGameCount <= 0 ) return 0;

	return (int)( (float)m_UserInfo.nWin * 100.0f / (float)nGameCount );
}

void CMyUserPocket::SetExp( __int64 nExp )
{
	m_UserInfo.nExp		= nExp;
	m_pUser->SetExp( nExp );
}

void CMyUserPocket::SetGrade( UINT nGrade )
{
	m_UserInfo.nGrade	= nGrade;
	m_pUser->SetGrade( nGrade );
}
void CMyUserPocket::SetMoney( __int64 nMoney )
{
	m_UserInfo.nMoney	= nMoney;
}

void CMyUserPocket::SetKillCount( UINT nKillCount )
{
	m_UserInfo.nKill	= nKillCount;
}

void CMyUserPocket::SetDeadCount( UINT nDeadCount )
{
	m_UserInfo.nDead	= nDeadCount;
}

void CMyUserPocket::SetHKillCount( UINT nKillCount )
{
	m_UserInfo.nHeadShotKill	= nKillCount;
}

void CMyUserPocket::SetHDeadCount( UINT nDeadCount )
{
	m_UserInfo.nHeadShotDead	= nDeadCount;
}

void CMyUserPocket::SetWinCount( UINT nWinCount )
{
	m_UserInfo.nWin		= nWinCount;
}

void CMyUserPocket::SetLoseCount( UINT nLoseCount )
{
	m_UserInfo.nLose	= nLoseCount;
}

void CMyUserPocket::SetDrawCount( UINT nDrawCount )
{
	m_UserInfo.nDraw	= nDrawCount;
}

UINT CMyUserPocket::GetGameKill()
{
	return m_UserGameInfo.nHighLevelKill+m_UserGameInfo.nLowLevelKill;
}

UINT CMyUserPocket::GetGameDead()
{
	return m_UserGameInfo.nHighLevelDead+m_UserGameInfo.nLowLevelDead;
}

UINT CMyUserPocket::GetGameHKill()
{
	return m_UserGameInfo.nHeadShotKill;
}

UINT CMyUserPocket::GetGameHDead()
{
	return m_UserGameInfo.nHeadShotDead;
}

UINT CMyUserPocket::GetGameExp()
{
	return m_UserGameInfo.nExp;
}

UINT CMyUserPocket::GetGameMoney()
{
	return m_UserGameInfo.nMoney;
}

void CMyUserPocket::AddGameExp( UINT nExp )
{
	m_UserGameInfo.nExp		+= nExp;
}

void CMyUserPocket::AddGameMoney( UINT nMoney )
{
	m_UserGameInfo.nMoney	+= nMoney;
}

UINT CMyUserPocket::GetRoundKill()
{
	return m_UserRoundInfo.nHighLevelKill+m_UserRoundInfo.nLowLevelKill;
}

UINT CMyUserPocket::GetRoundDead()
{
	return m_UserRoundInfo.nHighLevelDead+m_UserRoundInfo.nLowLevelDead;
}

UINT CMyUserPocket::GetRoundHKill()
{
	return m_UserRoundInfo.nHeadShotKill;
}

UINT CMyUserPocket::GetRoundHDead()
{
	return m_UserRoundInfo.nHeadShotDead;
}

void CMyUserPocket::AddGameKill( BOOL bHigh, UINT nHitNode )
{
	if( bHigh )
	{
		++m_UserRoundInfo.nHighLevelKill;
		++m_UserGameInfo.nHighLevelKill;
	}
	else
	{
		++m_UserRoundInfo.nLowLevelKill;
		++m_UserGameInfo.nLowLevelKill;
	}

	if( nHitNode == NODE_HEAD )
	{
		++m_UserRoundInfo.nHeadShotKill;
		++m_UserGameInfo.nHeadShotKill;
	}
}

void CMyUserPocket::AddGameDead( BOOL bHigh, UINT nHitNode )
{
	if( bHigh )
	{
		++m_UserRoundInfo.nHighLevelDead;
		++m_UserGameInfo.nHighLevelDead;
	}
	else
	{
		++m_UserRoundInfo.nLowLevelDead;
		++m_UserGameInfo.nLowLevelDead;
	}

	if( nHitNode == NODE_HEAD )
	{
		++m_UserRoundInfo.nHeadShotDead;
		++m_UserGameInfo.nHeadShotDead;
	}
}

int CMyUserPocket::GetState()
{
	return m_UserInfo.nState;
}

BOOL CMyUserPocket::IsMe( LPCSTR lpszID )
{
	if( strcmp( lpszID, m_UserInfo.szID ) == 0 ) return TRUE;

	return FALSE;
}

void CMyUserPocket::AddWinRound()
{
	++m_UserGameInfo.nWinRound;
}

void CMyUserPocket::AddLoseRound()
{
	++m_UserGameInfo.nLoseRound;
}

void CMyUserPocket::AddDrawRound()
{
	++m_UserGameInfo.nDrawRound;
}

UINT CMyUserPocket::GetWinRound()
{
	return m_UserGameInfo.nWinRound;
}

UINT CMyUserPocket::GetLoseRound()
{
	return m_UserGameInfo.nLoseRound;
}

UINT CMyUserPocket::GetDrawRound()
{
	return m_UserGameInfo.nDrawRound;
}

DWORD CMyUserPocket::GetStartTick()
{
	return m_UserRoundInfo.dwStartTick;
}