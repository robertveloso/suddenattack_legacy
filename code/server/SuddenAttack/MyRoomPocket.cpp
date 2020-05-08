#include "StdAfx.h"
#include ".\myroompocket.h"

CMyRoomPocket::CMyRoomPocket(void)
{
}

CMyRoomPocket::~CMyRoomPocket(void)
{
}

void CMyRoomPocket::InitPocket( T_ROOM_OPTIONptr pRoomOption )
{
	m_nPlayerCount_A	= 0;
	m_nPlayerCount_B	= 0;
	m_nStep				= ROOM_STEP_WAIT;
	m_bMustReadyToStart	= !TRUE;

	int i;
	for( i=0 ; i<MAX_TEAM_USER ; ++i )
	{
		m_PlayerSlot_A[i]	= NULL;
		m_PlayerSlot_B[i]	= NULL;
	}

	this->SetRoomOption( pRoomOption );
}

void CMyRoomPocket::InitGameInfo()
{
	int i;
	for( i=0 ; i<MAX_TEAM_USER ; ++i )
	{
		if( m_PlayerSlot_A[i] ) m_PlayerSlot_A[i]->InitGameInfo();
		if( m_PlayerSlot_B[i] ) m_PlayerSlot_B[i]->InitGameInfo();
	}

	m_nKillCount_A	= 0;
	m_nWinRound_A	= 0;

	m_nKillCount_B	= 0;	
	m_nWinRound_B	= 0;
}

void CMyRoomPocket::InitRoundInfo()
{
	int i;
	for( i=0 ; i<MAX_TEAM_USER ; ++i )
	{
		if( m_PlayerSlot_A[i] ) m_PlayerSlot_A[i]->InitRoundInfo();
		if( m_PlayerSlot_B[i] ) m_PlayerSlot_B[i]->InitRoundInfo();
	}
}

void CMyRoomPocket::InitWaitData()
{
	int i;
	for( i=0 ; i<MAX_TEAM_USER ; ++i )
	{
		if( m_PlayerSlot_A[i] )
		{
			if( m_PlayerSlot_A[i]->IsCaptain() ) 
			{
				m_PlayerSlot_A[i]->SetState( PLAYER_STATE_WAITING|PLAYER_STATE_READY|PLAYER_STATE_CAPTAIN );
			}
			else
			{
				m_PlayerSlot_A[i]->SetState( PLAYER_STATE_WAITING );
			}
		}

		if( m_PlayerSlot_B[i] )
		{
			if( m_PlayerSlot_B[i]->IsCaptain() ) 
			{
				m_PlayerSlot_B[i]->SetState( PLAYER_STATE_WAITING|PLAYER_STATE_READY|PLAYER_STATE_CAPTAIN );
			}
			else
			{
				m_PlayerSlot_B[i]->SetState( PLAYER_STATE_WAITING );
			}			
		}
	}

	m_nKillCount_A	= 0;
	m_nKillCount_B	= 0;
}

BOOL CMyRoomPocket::SetRoomOption( T_ROOM_OPTIONptr pRoomOption )
{
	if( !pRoomOption ) return FALSE;

	memcpy( &m_RoomOption, pRoomOption, sizeof(T_ROOM_OPTION) );
	return TRUE;
}

T_ROOM_OPTIONptr CMyRoomPocket::GetRoomOption()
{
	return &m_RoomOption;
}

int CMyRoomPocket::GetPlayerCount()
{
	return GetPlayerCount_A() + GetPlayerCount_B();
}

CMyUserPocket* CMyRoomPocket::GetPlayer( LPCSTR lpszID )
{
	int	i;

	for( i=0 ; i<m_nPlayerCount_A ; ++i )
	{
		if( m_PlayerSlot_A[i]->IsMe(lpszID) )
		{
			return m_PlayerSlot_A[i];
		}
	}

	for( i=0 ; i<m_nPlayerCount_B ; ++i )
	{
		if( m_PlayerSlot_B[i]->IsMe(lpszID) )
		{
			return m_PlayerSlot_B[i];
		}
	}

	return NULL;
}

CMyUserPocket* CMyRoomPocket::GetPlayer( int nIndex )
{
	if( nIndex < GetPlayerCount_A() )
	{
		return m_PlayerSlot_A[nIndex];
	}
	else
	{
		return m_PlayerSlot_B[nIndex-GetPlayerCount_A()];
	}

	return NULL;
}

BOOL CMyRoomPocket::AddPlayer_A( CMyUserPocket* pUserPocket )
{
	if( MAX_TEAM_USER <= m_nPlayerCount_A || NULL == pUserPocket ) return FALSE;
	pUserPocket->SetUserTeam( TEAM_A );
	m_PlayerSlot_A[m_nPlayerCount_A]	= pUserPocket;
	
	++m_nPlayerCount_A;

	return TRUE;
}

CMyUserPocket* CMyRoomPocket::GetPlayer_A( LPCSTR lpszID )
{
	int	i;
	for( i=0 ; i<m_nPlayerCount_A ; ++i )
	{
		if( m_PlayerSlot_A[i]->IsMe(lpszID) )
		{
			return m_PlayerSlot_A[i];
		}
	}

	return NULL;
}

CMyUserPocket* CMyRoomPocket::GetPlayer_A( int nIndex )
{
	if( nIndex >= m_nPlayerCount_A ) return NULL;
	return m_PlayerSlot_A[nIndex];
}

CMyUserPocket* CMyRoomPocket::GetLastPlayer_A()
{
	if( m_nPlayerCount_A <= 0 ) return NULL;
	return m_PlayerSlot_A[m_nPlayerCount_A-1];
}

int CMyRoomPocket::GetPlayerCount_A()
{
	return m_nPlayerCount_A;
}

int CMyRoomPocket::GetPlayerCount_A_InGame()
{
	int i;
	int nCount	= 0;
	CMyUserPocket* pUserPocket;

	for( i=0 ; i<GetPlayerCount_A() ; ++i )
	{
		pUserPocket	= GetPlayer_A(i);
		if( pUserPocket->IsGaming() &&
			FALSE == pUserPocket->IsSpectator() ) ++nCount;
	}

	return nCount;
}

int CMyRoomPocket::GetPlayerCount_B_InGame()
{
	int i;
	int nCount	= 0;
	CMyUserPocket* pUserPocket;

	for( i=0 ; i<GetPlayerCount_B() ; ++i )
	{
		pUserPocket	= GetPlayer_B(i);
		if( pUserPocket->IsGaming() &&
			FALSE == pUserPocket->IsSpectator() ) ++nCount;
	}

	return nCount;
}

int CMyRoomPocket::GetPlayerCount_A_Ready()
{
	int i;
	int nCount	= 0;
	CMyUserPocket* pUserPocket;

	for( i=0 ; i<GetPlayerCount_A() ; ++i )
	{
		pUserPocket	= GetPlayer_A(i);
		if( (PLAYER_STATE_READY&pUserPocket->GetState()) &&
			FALSE == pUserPocket->IsSpectator() ) ++nCount;
	}

	return nCount;
}

int CMyRoomPocket::GetPlayerCount_B_Ready()
{
	int i;
	int nCount	= 0;
	CMyUserPocket* pUserPocket;

	for( i=0 ; i<GetPlayerCount_B() ; ++i )
	{
		pUserPocket	= GetPlayer_B(i);
		if( (PLAYER_STATE_READY&pUserPocket->GetState()) &&
			FALSE == pUserPocket->IsSpectator() ) ++nCount;
	}

	return nCount;
}

int CMyRoomPocket::GetPlayerCount_A_Spectator()
{
	int i;
	int nCount	= 0;
	CMyUserPocket* pUserPocket;

	for( i=0 ; i<GetPlayerCount_A() ; ++i )
	{
		pUserPocket	= GetPlayer_A(i);
		if( pUserPocket->IsSpectator() ) ++nCount;
	}

	return nCount;
}

int CMyRoomPocket::GetPlayerCount_B_Spectator()
{
	int i;
	int nCount	= 0;
	CMyUserPocket* pUserPocket;

	for( i=0 ; i<GetPlayerCount_B() ; ++i )
	{
		pUserPocket	= GetPlayer_B(i);
		if( pUserPocket->IsSpectator() ) ++nCount;
	}

	return nCount;
}

BOOL CMyRoomPocket::RemovePlayer_A( LPCSTR lpszID )
{
	int	i;
	BOOL	bMovePlayer = FALSE;
	for( i=0 ; i<m_nPlayerCount_A ; ++i )
	{
		if( bMovePlayer )
		{
			m_PlayerSlot_A[i-1] = m_PlayerSlot_A[i];
		}
		else
		{
			if( m_PlayerSlot_A[i]->IsMe(lpszID) )
			{
				bMovePlayer	= TRUE;
			}
		}
	}

	if( bMovePlayer )
	{
		--m_nPlayerCount_A;
		m_PlayerSlot_A[MAX_TEAM_USER-1] = NULL;
	}

	return bMovePlayer;
}

BOOL CMyRoomPocket::RemovePlayer_A( int nIndex )
{
	int	i;
	BOOL	bMovePlayer = FALSE;
	for( i=0 ; i<m_nPlayerCount_A ; ++i )
	{
		if( bMovePlayer )
		{
			m_PlayerSlot_A[i-1] = m_PlayerSlot_A[i];
		}
		else
		{
			if( i == nIndex )
			{
				bMovePlayer	= TRUE;
			}
		}
	}

	if( bMovePlayer )
	{
		--m_nPlayerCount_A;
		m_PlayerSlot_A[MAX_TEAM_USER-1] = NULL;
	}

	return bMovePlayer;
}

BOOL CMyRoomPocket::AddPlayer_B( CMyUserPocket* pUserPocket )
{
	if( MAX_TEAM_USER <= m_nPlayerCount_B || NULL == pUserPocket ) return FALSE;
	pUserPocket->SetUserTeam( TEAM_B );
	m_PlayerSlot_B[m_nPlayerCount_B]	= pUserPocket;
	
	++m_nPlayerCount_B;

	return TRUE;
}

CMyUserPocket* CMyRoomPocket::GetPlayer_B( LPCSTR lpszID )
{
	int	i;
	for( i=0 ; i<m_nPlayerCount_B ; ++i )
	{
		if( m_PlayerSlot_B[i]->IsMe(lpszID) )
		{
			return m_PlayerSlot_B[i];
		}
	}

	return NULL;
}

CMyUserPocket* CMyRoomPocket::GetPlayer_B( int nIndex )
{
	if( nIndex >= m_nPlayerCount_B ) return NULL;
	return m_PlayerSlot_B[nIndex];
}

CMyUserPocket* CMyRoomPocket::GetLastPlayer_B()
{
	if( m_nPlayerCount_B <= 0 ) return NULL;
	return m_PlayerSlot_B[m_nPlayerCount_B-1];
}

int CMyRoomPocket::GetPlayerCount_B()
{
	return m_nPlayerCount_B;
}

BOOL CMyRoomPocket::RemovePlayer_B( LPCSTR lpszID )
{
	int	i;
	BOOL	bMovePlayer = FALSE;
	for( i=0 ; i<m_nPlayerCount_B ; ++i )
	{
		if( bMovePlayer )
		{
			m_PlayerSlot_B[i-1] = m_PlayerSlot_B[i];
		}
		else
		{
			if( m_PlayerSlot_B[i]->IsMe(lpszID) )
			{
				bMovePlayer	= TRUE;
			}
		}
	}

	if( bMovePlayer )
	{
		--m_nPlayerCount_B;
		m_PlayerSlot_B[MAX_TEAM_USER-1] = NULL;
	}

	return bMovePlayer;
}

BOOL CMyRoomPocket::RemovePlayer_B( int nIndex )
{
	int	i;
	BOOL	bMovePlayer = FALSE;
	for( i=0 ; i<m_nPlayerCount_B ; ++i )
	{
		if( bMovePlayer )
		{
			m_PlayerSlot_B[i-1] = m_PlayerSlot_B[i];
		}
		else
		{
			if( i == nIndex )
			{
				bMovePlayer	= TRUE;
			}
		}
	}

	if( bMovePlayer )
	{
		--m_nPlayerCount_B;
		m_PlayerSlot_B[MAX_TEAM_USER-1] = NULL;
	}

	return bMovePlayer;
}

int CMyRoomPocket::ChangeTeam( LPCSTR lpszID )
{
	CMyUserPocket* pUserPocket;

	pUserPocket = GetPlayer_A( lpszID );
	if( AddPlayer_B( pUserPocket ) )
	{
		RemovePlayer_A( lpszID );
		return TEAM_B;
	}

	pUserPocket = GetPlayer_B( lpszID );
	if( AddPlayer_A( pUserPocket ) )
	{
		RemovePlayer_B( lpszID );
		return TEAM_A;
	}

	return TEAM_ERROR;
}

int CMyRoomPocket::ChangeTeam_A( int nIndex )
{
	CMyUserPocket* pUserPocket;

	pUserPocket = GetPlayer_A( nIndex );
	if( AddPlayer_B( pUserPocket ) )
	{
		RemovePlayer_A( nIndex );
		return TEAM_B;
	}

	return TEAM_ERROR;
}

int CMyRoomPocket::ChangeTeam_B( int nIndex )
{
	CMyUserPocket* pUserPocket;

	pUserPocket = GetPlayer_B( nIndex );
	if( AddPlayer_A( pUserPocket ) )
	{
		RemovePlayer_B( nIndex );
		return TEAM_A;
	}

	return TEAM_ERROR;
}

void CMyRoomPocket::SetStep( int nStep )
{
	m_nStep	= nStep;

	switch( m_nStep )
	{
	case ROOM_STEP_PRE_GAME:
        break;
	case ROOM_STEP_GAME:
		m_nTimebomeState	= -1;
		m_nWinRoundTeam		= TEAM_NONE;
		m_dwStartTick		= ::timeGetTime();
		break;
	case ROOM_STEP_POST_GAME:
		break;
	}
}

int CMyRoomPocket::GetStep()
{
	return m_nStep;
}

void CMyRoomPocket::SetTeamBalance()
{
	int		i, nDelta, nMovePlayerCounts;
	BOOL	bMoveReserved[MAX_TEAM_USER] = {0,};

	CMyUserPocket* pUserPocket;

	nDelta = GetPlayerCount_A_Ready() - GetPlayerCount_B_Ready();
	if( (TEAM_BALANCE_MIN_LIMIT <= nDelta) && (nDelta <= TEAM_BALANCE_MAX_LIMIT) ) return;
	nMovePlayerCounts	= abs(nDelta)/2;

	if( nDelta>0 )	// A -> B
	{
		for( i=GetPlayerCount_A()-1 ; i>=0 ; --i )
		{
			pUserPocket	= GetPlayer_A(i);
			if( pUserPocket->GetState()&PLAYER_STATE_READY )
			{
				bMoveReserved[i] = TRUE;	//Move 예약

				--nMovePlayerCounts;
				if( nMovePlayerCounts<=0 ) break;
			}
		}

		//Move...
		for( i=GetPlayerCount_A()-1 ; i>=0 ; --i )
		{
			if( bMoveReserved[i] )
			{
				this->ChangeTeam_A(i);
			}
		}
	}
	else		// A <- B
	{
		for( i=GetPlayerCount_B()-1 ; i>=0 ; --i )
		{
			pUserPocket	= GetPlayer_B(i);
			if( pUserPocket->GetState()&PLAYER_STATE_READY )
			{
				bMoveReserved[i] = TRUE;	//Move 예약
				--nMovePlayerCounts;
				if( nMovePlayerCounts<=0 ) break;
			}
		}

		//Move...
		for( i=GetPlayerCount_B()-1 ; i>=0 ; --i )
		{
			if( bMoveReserved[i] )
			{
				this->ChangeTeam_B(i);
			}
		}
	}
}

BOOL CMyRoomPocket::SetHostInfo( LPCSTR lpszID, LPCSTR lpszIP, int nPort )
{
	if( !lpszIP ) return FALSE;

	strcpy( m_szHostIP, lpszIP );
	m_nHostPort	= nPort;

	CMyUserPocket* pUserPocket;
	pUserPocket	= GetPlayer_A( lpszID );
	if( pUserPocket )
	{
		pUserPocket->SetP2PHost(TRUE);
		return TRUE;
	}

	pUserPocket	= GetPlayer_B( lpszID );
	if( pUserPocket )
	{
		pUserPocket->SetP2PHost(TRUE);
		return TRUE;
	}
	return FALSE;
}

LPCSTR CMyRoomPocket::GetHostIP()
{
	return m_szHostIP;
}

int CMyRoomPocket::GetHostPort()
{
	return m_nHostPort;
}

void CMyRoomPocket::SetCanBeCaptain( LPCSTR lpszID, BOOL bCan )
{
	int i;
	for( i=0 ; i<m_nPlayerCount_A ; ++i )
	{
		if( m_PlayerSlot_A[i]->IsMe(lpszID) )
		{
			m_PlayerSlot_A[i]->GetUserGameInfo()->bCanBeCaptain	= bCan;
			return;
		}
	}

	for( i=0 ; i<m_nPlayerCount_B ; ++i )
	{
		if( m_PlayerSlot_B[i]->IsMe(lpszID) )
		{
			m_PlayerSlot_B[i]->GetUserGameInfo()->bCanBeCaptain	= bCan;
			return;
		}
	}
}

BOOL CMyRoomPocket::CanBeCaptain( LPCSTR lpszID )
{
	int i;
	for( i=0 ; i<m_nPlayerCount_A ; ++i )
	{
		if( m_PlayerSlot_A[i]->IsMe(lpszID) )
		{
			return m_PlayerSlot_A[i]->GetUserGameInfo()->bCanBeCaptain;
		}
	}

	for( i=0 ; i<m_nPlayerCount_B ; ++i )
	{
		if( m_PlayerSlot_B[i]->IsMe(lpszID) )
		{
			return m_PlayerSlot_B[i]->GetUserGameInfo()->bCanBeCaptain;
		}
	}

	return FALSE;
}

BOOL CMyRoomPocket::IsReadyAll()
{
	int i;
	for( i=0 ; i<m_nPlayerCount_A ; ++i )
	{
		if( FALSE == m_PlayerSlot_A[i]->IsReady() ) return FALSE;
	}

	for( i=0 ; i<m_nPlayerCount_B ; ++i )
	{
		if( FALSE == m_PlayerSlot_B[i]->IsReady() ) return FALSE;
	}

	return TRUE;
}

void CMyRoomPocket::AddKillCount( LPCSTR lpszKillerID, LPCSTR lpszVictimID, UINT nHitNode )
{
	int i;
	for( i=0 ; i<m_nPlayerCount_A ; ++i )
	{
		if( m_PlayerSlot_A[i]->IsMe(lpszKillerID) )
		{
			if( GetGrade(lpszVictimID) < m_PlayerSlot_A[i]->GetGrade() )
			{
				m_PlayerSlot_A[i]->AddGameKill( FALSE, nHitNode );
			}
			else
			{
				m_PlayerSlot_A[i]->AddGameKill( TRUE, nHitNode );
			}
						
			++m_nKillCount_A;
			return;
		}
	}

	for( i=0 ; i<m_nPlayerCount_B ; ++i )
	{
		if( m_PlayerSlot_B[i]->IsMe(lpszKillerID) )
		{
			if( GetGrade(lpszVictimID) < m_PlayerSlot_B[i]->GetGrade() )
			{
				m_PlayerSlot_B[i]->AddGameKill( FALSE, nHitNode );
			}
			else
			{
				m_PlayerSlot_B[i]->AddGameKill( TRUE, nHitNode );
			}

			++m_nKillCount_B;
			return;
		}
	}
}

void CMyRoomPocket::AddDeadCount( LPCSTR lpszKillerID, LPCSTR lpszVictimID,  UINT nHitNode )
{
	int i;
	for( i=0 ; i<m_nPlayerCount_A ; ++i )
	{
		if( m_PlayerSlot_A[i]->IsMe(lpszVictimID) )
		{
			if( GetGrade(lpszKillerID) < m_PlayerSlot_A[i]->GetGrade() )
			{
				m_PlayerSlot_A[i]->AddGameDead( FALSE, nHitNode );
			}
			else
			{
				m_PlayerSlot_A[i]->AddGameDead( TRUE, nHitNode );
			}
			return;
		}
	}

	for( i=0 ; i<m_nPlayerCount_B ; ++i )
	{
		if( m_PlayerSlot_B[i]->IsMe(lpszVictimID) )
		{
			if( GetGrade(lpszKillerID) < m_PlayerSlot_B[i]->GetGrade() )
			{
				m_PlayerSlot_B[i]->AddGameDead( FALSE, nHitNode );
			}
			else
			{
				m_PlayerSlot_B[i]->AddGameDead( TRUE, nHitNode );
			}
			return;
		}
	}
}

int CMyRoomPocket::GetTotalKillCount_A()
{
	return m_nKillCount_A;
}


int CMyRoomPocket::GetTotalKillCount_B()
{
	return m_nKillCount_B;
}

void CMyRoomPocket::AddWinRound_A()
{
	int i;

	for( i=0 ; i<m_nPlayerCount_A ; ++i )
	{
		if( m_PlayerSlot_A[i]->IsGaming() &&
			FALSE == m_PlayerSlot_A[i]->IsSpectator() )
		{
			m_PlayerSlot_A[i]->AddWinRound();
		}
	}

	for( i=0 ; i<m_nPlayerCount_B ; ++i )
	{
		if( m_PlayerSlot_B[i]->IsGaming() &&
			FALSE == m_PlayerSlot_B[i]->IsSpectator() )
		{
			m_PlayerSlot_B[i]->AddLoseRound();
		}
	}

	++m_nWinRound_A;
}

void CMyRoomPocket::AddWinRound_B()
{
	int i;

	for( i=0 ; i<m_nPlayerCount_A ; ++i )
	{
		if( m_PlayerSlot_A[i]->IsGaming() &&
			FALSE == m_PlayerSlot_A[i]->IsSpectator() )
		{
			m_PlayerSlot_A[i]->AddLoseRound();
		}
	}

	for( i=0 ; i<m_nPlayerCount_B ; ++i )
	{
		if( m_PlayerSlot_B[i]->IsGaming() &&
			FALSE == m_PlayerSlot_B[i]->IsSpectator() )
		{
			m_PlayerSlot_B[i]->AddWinRound();
		}
	}

	++m_nWinRound_B;
}

void CMyRoomPocket::AddDrawRound()
{
	int i;

	for( i=0 ; i<m_nPlayerCount_A ; ++i )
	{
		if( m_PlayerSlot_A[i]->IsGaming() &&
			FALSE == m_PlayerSlot_A[i]->IsSpectator() )
		{
			m_PlayerSlot_A[i]->AddDrawRound();
		}
	}

	for( i=0 ; i<m_nPlayerCount_B ; ++i )
	{
		if( m_PlayerSlot_B[i]->IsGaming() &&
			FALSE == m_PlayerSlot_B[i]->IsSpectator() )
		{
			m_PlayerSlot_B[i]->AddDrawRound();
		}
	}
}

int CMyRoomPocket::GetWinRound_A()
{
	return m_nWinRound_A;
}

int CMyRoomPocket::GetWinRound_B()
{
	return m_nWinRound_B;
}

void CMyRoomPocket::SetWinRoundTeam( int nWinTeam )
{
	m_nWinRoundTeam	= nWinTeam;
}

int CMyRoomPocket::GetWinRoundTeam()
{
	return m_nWinRoundTeam;
}

int CMyRoomPocket::GetWinGameTeam()
{
	if( GetWinRound_A() > GetWinRound_B() )
	{
		return TEAM_A;
	}
	else if( GetWinRound_A() < GetWinRound_B() )
	{
		return TEAM_B;
	}

	return TEAM_NONE;
}

UINT CMyRoomPocket::GetGrade( LPCSTR lpszID )
{
	int i;
	for( i=0 ; i<m_nPlayerCount_A ; ++i )
	{
		if( m_PlayerSlot_A[i]->IsMe(lpszID) )
		{
			return m_PlayerSlot_A[i]->GetGrade();
		}
	}

	for( i=0 ; i<m_nPlayerCount_B ; ++i )
	{
		if( m_PlayerSlot_B[i]->IsMe(lpszID) )
		{
			return m_PlayerSlot_B[i]->GetGrade();
		}
	}

	return 0;
}

void CMyRoomPocket::SetMustReadyToStart( BOOL bMust )
{
	m_bMustReadyToStart	= bMust;
}

BOOL CMyRoomPocket::IsMustReadyToStart()
{
	return m_bMustReadyToStart;
}

DWORD CMyRoomPocket::GetStartTick()
{
	return m_dwStartTick;
}

BOOL CMyRoomPocket::CanRespawn()
{
	switch( GetRoomOption()->nGameType )
	{
	case GAME_TYPE_TDM:
		return TRUE;
	}

	return FALSE;
}

void CMyRoomPocket::SetTimeBombState( int nState )
{
	if( m_nTimebomeState >= nState ) return;
	m_nTimebomeState	= nState;
}

int CMyRoomPocket::GetTimeBombState()
{
	return m_nTimebomeState;
}