#pragma once

#include <vector>
#include "..\jolyo\casualgame\pocket.h"
using namespace CasualGame;

#include "MyCommon.h"
#include "MyUserPocket.h"

typedef struct _tagROOMOPTION
{
	BYTE	nGameType;
	BYTE	nMapIndex;
	BYTE	nMaxUser;
	BYTE	nKillLimit;
	BYTE	nTimeLimit;
	BYTE	nGradeMinLimit;
	BYTE	nGradeMaxLimit;
	BYTE	bInstrusion;
	BYTE	bTeamBalance;
	short	nRounds;

	BYTE	bReserved1;
	BYTE	bReserved2;
	BYTE	bReserved3;
} T_ROOM_OPTION, *T_ROOM_OPTIONptr;


class CMyRoomPocket : public CPocket
{
public:
	CMyRoomPocket(void);
	virtual ~CMyRoomPocket(void);

	void				InitPocket( T_ROOM_OPTIONptr pRoomOption );
	void				InitGameInfo();
	void				InitRoundInfo();
	void				InitWaitData();

	BOOL				SetRoomOption( T_ROOM_OPTIONptr pRoomOption );
	T_ROOM_OPTIONptr	GetRoomOption();

	int					GetPlayerCount();
	CMyUserPocket*		GetPlayer( LPCSTR lpszID );
	CMyUserPocket*		GetPlayer( int nIndex );

	BOOL				AddPlayer_A( CMyUserPocket* pUserPocket );
	CMyUserPocket*		GetPlayer_A( LPCSTR lpszID );
	CMyUserPocket*		GetPlayer_A( int nIndex );

	CMyUserPocket*		GetLastPlayer_A();
	CMyUserPocket*		GetLastPlayer_B();

	int					GetPlayerCount_A();
	BOOL				RemovePlayer_A( LPCSTR lpszID );
	BOOL				RemovePlayer_A( int nIndex );

	BOOL				AddPlayer_B( CMyUserPocket* pUserPocket );
	CMyUserPocket*		GetPlayer_B( LPCSTR lpszID );
	CMyUserPocket*		GetPlayer_B( int nIndex );
	int					GetPlayerCount_B();
	BOOL				RemovePlayer_B( LPCSTR lpszID );
	BOOL				RemovePlayer_B( int nIndex );

	int					ChangeTeam( LPCSTR lpszID );
	int					ChangeTeam_A( int nIndex );
	int					ChangeTeam_B( int nIndex );
	void				SetCanBeCaptain( LPCSTR lpszID, BOOL bCan );
	BOOL				CanBeCaptain( LPCSTR lpszID );

	void				SetStep( int nStep );
	int					GetStep();

	BOOL				SetHostInfo( LPCSTR lpszID, LPCSTR lpszIP, int nPort );
	LPCSTR				GetHostIP();
	int					GetHostPort();

	BOOL				IsReadyAll();

	void				AddKillCount( LPCSTR lpszKillerID, LPCSTR lpszVictimID, UINT nHitNode );
	void				AddDeadCount( LPCSTR lpszKillerID, LPCSTR lpszVictimID, UINT nHitNode );
	
	int					GetTotalKillCount_A();
	int					GetTotalKillCount_B();

	void				AddWinRound_A();
	void				AddWinRound_B();
	void				AddDrawRound();


	int					GetWinRound_A();
	int					GetWinRound_B();

	void				SetWinRoundTeam( int nWinTeam );
	int					GetWinRoundTeam();
	int					GetWinGameTeam();
    
	void				SetMustReadyToStart( BOOL bMust );
	BOOL				IsMustReadyToStart();

	DWORD				GetStartTick();

	int					GetPlayerCount_A_InGame();
	int					GetPlayerCount_B_InGame();

	int					GetPlayerCount_A_Ready();
	int					GetPlayerCount_B_Ready();

	int					GetPlayerCount_A_Spectator();
	int					GetPlayerCount_B_Spectator();

	void				SetTeamBalance();

	BOOL				CanRespawn();

	void				SetTimeBombState( int nState );
	int					GetTimeBombState();
	
protected:
	UINT				GetGrade( LPCSTR lpszID );
	
	int				m_nPlayerCount_A;
	CMyUserPocket*	m_PlayerSlot_A[MAX_TEAM_USER];
	int				m_nKillCount_A;
	int				m_nWinRound_A;
	
	int				m_nWinRoundTeam;

	int				m_nPlayerCount_B;
	CMyUserPocket*	m_PlayerSlot_B[MAX_TEAM_USER];
	int				m_nKillCount_B;
	int				m_nWinRound_B;

	T_ROOM_OPTION	m_RoomOption;
	int				m_nStep;

	TCHAR			m_szHostIP[MAX_IP_LEN];
	int				m_nHostPort;

	BOOL			m_bMustReadyToStart;

	DWORD			m_dwStartTick;

    int				m_nTimebomeState;
};

