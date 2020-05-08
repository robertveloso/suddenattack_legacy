#pragma once

#include "..\jolyo\casualgame\pocket.h"
using namespace CasualGame;

#include "MyCommon.h"
#include "../Jolyo/CasualGame/User.h"

typedef struct _tagUSER_INFO
{
	//User Info
	char	szID[MAX_ID_LEN+1];
	char	szNick[MAX_NICK_LEN+1];
	int		nState;
	__int64	nExp;
	__int64	nMoney;
	UINT	nGrade;
	UINT	nKill;
	UINT	nDead;
	UINT	nHeadShotKill;
	UINT	nHeadShotDead;
	UINT	nWin;
	UINT	nLose;
	UINT	nDraw;

} T_USER_INFO, *T_USER_INFOptr;

typedef struct _tagUSER_GAME_INFO
{
	//User Game Info
	UINT	nExp;
	UINT	nMoney;
	BOOL	bCanBeCaptain;
	int		nHeadShotKill;
	int		nHeadShotDead;
	int		nHighLevelKill;
	int		nLowLevelKill;
	int		nHighLevelDead;
	int		nLowLevelDead;

	int		nWinRound;
	int		nLoseRound;
	int		nDrawRound;

} T_USER_GAME_INFO, *T_USER_GAME_INFOptr;

typedef struct _tagUSER_ROUND_INFO
{
	DWORD	dwStartTick;
	int		nHeadShotKill;
	int		nHeadShotDead;
	int		nHighLevelKill;
	int		nLowLevelKill;
	int		nHighLevelDead;
	int		nLowLevelDead;
} T_USER_ROUND_INFO, *T_USER_ROUND_INFOptr;


class CMyUserPocket :
	public CPocket
{
public:
	CMyUserPocket(void);
	virtual ~CMyUserPocket(void);

	void				InitPocket( CUser* pUser, T_USER_INFOptr pUserInfo );
	BOOL				SetUserInfo( T_USER_INFOptr pUserInfo );
    T_USER_INFOptr		GetUserInfo();
	CUser*				GetUserClass();

	void				InitGameInfo();
	void				InitRoundInfo();
	BOOL				IsMe( LPCSTR lpszID );
	void				InitGameRoomInfo();
	T_USER_GAME_INFOptr	GetUserGameInfo();

	void				SetUserTeam( int nTeam );
	int					GetUserTeam();

	void				SetSpectatorMode( BOOL bEnable );
	BOOL				IsSpectator();

	int					GetState();
	void				SetState( int nState );
	void				AddState( int nState );
	void				RemoveState( int nState );

	void				SetP2PHost( BOOL bHost );

	BOOL				IsP2PHost();
	BOOL				IsCaptain();
	BOOL				IsGaming();
	BOOL				IsLoading();
	BOOL				IsWaiting();
	BOOL				IsReady();
	BOOL				IsLobby();

	// Total
	__int64				GetExp();
	UINT				GetGrade();
	__int64				GetMoney();
	UINT				GetKillCount();
	UINT				GetDeadCount();
	UINT				GetHKillCount();
	UINT				GetHDeadCount();
	UINT				GetWinCount();
	UINT				GetLoseCount();
	UINT				GetDrawCount();
	UINT				GetWinPercent();	
	void				SetExp( __int64 nExp );
	void				SetGrade( UINT nGrade );
	void				SetMoney( __int64 nMoney );
	void				SetKillCount( UINT nKillCount );
	void				SetDeadCount( UINT nDeadCount );
	void				SetHKillCount( UINT nKillCount );
	void				SetHDeadCount( UINT nDeadCount );
	void				SetWinCount( UINT nWinCount );
	void				SetLoseCount( UINT nLoseCount );
	void				SetDrawCount( UINT nDrawCount );

	// 1 Game
	UINT				GetGameKill();
	UINT				GetGameDead();
	UINT				GetGameHKill();
	UINT				GetGameHDead();
	UINT				GetGameExp();
	UINT				GetGameMoney();
	void				AddGameExp( UINT nExp );
	void				AddGameMoney( UINT nMoney );

	// 1 Round
	UINT				GetRoundKill();
	UINT				GetRoundDead();
	UINT				GetRoundHKill();
	UINT				GetRoundHDead();
	DWORD				GetStartTick();

	void				AddGameKill( BOOL bHigh, UINT nHitNode );
	void				AddGameDead( BOOL bHigh, UINT nHitNode );

	void				AddWinRound();
	void				AddLoseRound();
	void				AddDrawRound();

	UINT				GetWinRound();
	UINT				GetLoseRound();
	UINT				GetDrawRound();				
	
protected:
	CUser*				m_pUser;
	T_USER_INFO			m_UserInfo;
	T_USER_GAME_INFO	m_UserGameInfo;
	T_USER_ROUND_INFO	m_UserRoundInfo;

	BOOL				m_bSpectator;
	int					m_nTeam;
	BOOL				m_bP2PHost;
};
