#ifndef __MURSUM_MYCOMMON_H__
#define __MURSUM_MYCOMMON_H__


#include "../jolyo/casualgame/casualcommon.h"

//	using namespace GamehiSolution::CasualGame::Define;

#define OPT_MAP_INDEX			0	// 0, 1, 2 ... ( 255는 랜덤맵 )
#define OPT_GAME_TYPE			1	// TeamDeathMatch(3) , Team Survival(5) ...
#define OPT_ROUND_LIMIT			2	// 0, 1, 2 ... ( 0은 무제한 )
#define OPT_TIME_LIMIT			3	// 0 - 255 ( 0은 무제한, 단위는 분 )
#define OPT_KILL_LIMIT			4	// 0 - 255 ( 0은 무제한 )
#define OPT_GRADE_MIN_LIMIT		5	// 0, 1, 2 ...
#define OPT_GRADE_MAX_LIMIT		6	// 0, 1, 2 ...
#define OPT_MAX_USERS			7	// 1 - 32
#define OPT_INTRUSION			8	// LTTRUE, LTFALSE
#define OPT_TEAM_BALANCE		9	// LTTRUE, LTFALSE
#define OPT_GAME_STATE			10	// GAME_STATE_WAIT, GAME_STATE_PLAY, 

#define GAME_STATE_WAIT			0
#define GAME_STATE_PLAY			1

#define END_ROUND_NONE				0
#define END_ROUND_ALL_KILL			1
#define END_ROUND_TIMEOUT			2
#define END_ROUND_BOMB_DETONATED	3
#define END_ROUND_BOMB_DEFUSED		4

//#define GAME_TYPE_SINGLE		0
//#define GAME_TYPE_COOP		1
//#define GAME_TYPE_DM			2
#define GAME_TYPE_TDM			3
//#define GAME_TYPE_DOOMS		4
#define GAME_TYPE_TSV			5

#define TEAM_ERROR		-1

#define TEAM_A			0
#define TEAM_B			1
#define TEAM_NONE		255

#define TEAM_DEFENSE	TEAM_B
#define TEAM_ATTACK		TEAM_A

#define GAME_RESULT_WIN			0
#define GAME_RESULT_LOSE		1
#define GAME_RESULT_DRAW		2

#define MAX_TEAM_USER			16
#define MAX_ROOM_USER			MAX_TEAM_USER*2

#define MAX_USERNO_LEN			CCasualCommon::CasualStringLength::UserNo
#define MAX_ID_LEN				CCasualCommon::CasualStringLength::UserID
#define MAX_IP_LEN				CCasualCommon::CasualStringLength::IpAddr
#define MAX_NICK_LEN			CCasualCommon::CasualStringLength::UserNick
#define MAX_TITLE_LEN			CCasualCommon::CasualStringLength::RoomName
#define MAX_PASS_LEN			CCasualCommon::CasualStringLength::RoomPass

#define PLAYER_STATE_LOBBY		0x00
#define PLAYER_STATE_WAITING	0x01
#define PLAYER_STATE_CAPTAIN	0x02
#define PLAYER_STATE_READY		0x04
#define PLAYER_STATE_LOADING	0x08
#define PLAYER_STATE_GAMING		0x10

#define FAIL_HOST				1
#define FAIL_PLAYERS			2
#define FAIL_READY				3
#define FAIL_HOST_DISCONNECTED	4
#define FAIL_HOST_LOAD_ERR		5
#define FAIL_UNKNOWN			100

#define NODE_HEAD				5

#define MYSIGNAL				1000	//CCasualCommon::UserMessage::UserSignal
#define MYQUERY					1000	//CCasualCommon::UserMessage::UserQuery
#define MYPACKET				1000	//CCasualCommon::UserMessage::UserPacket

#define ROOM_STEP_WAIT			0
#define ROOM_STEP_PRE_GAME		1
#define ROOM_STEP_GAME			2
#define ROOM_STEP_POST_GAME		3
#define ROOM_STEP_NEXT_ROUND	4

#define TEAM_CHANGE_NORMAL		0
#define TEAM_CHANGE_AFTER_DEATH	1
#define TEAM_CHANGE_START		2
#define TEAM_CHANGE_INSTRUSION	3

#define TEAM_BALANCE_MIN_LIMIT	-1
#define TEAM_BALANCE_MAX_LIMIT	1

#define INSTRUTION_LIMIT_TICK		15000	//15초

//ERROR CODE
#define ACK_ERR_INSTRUTION			0x1001	//게임중이고 난입이 불가능한 방에 입장을 요청한 경우
#define ACK_ERR_GRADE_LIMIT			0x1002	//등급제한에 걸린 경우
#define ACK_ERR_TIME_LIMIT			0x1003	//난입시 시간이 얼마 안 남아서 안되는 경우
#define ACK_ERR_DONT_EXIT_READY		0x1004

class CMyCommon
{
public:
	enum	MySignalMessage
	{
	};// enum MySignalMessage

	enum	MyQueryMessage
	{
		LoginQuery					= MYQUERY + 1,
		LogoutQuery					= MYQUERY + 2,
		UserInfoQuery				= MYQUERY + 3,
		GameResultQuery				= MYQUERY + 4,
	};// enum MyQueryMessage

	enum	MyPacketCommand
	{
		StartGameRequest			= MYPACKET + 1,
		StartHost					= MYPACKET + 2,
		StartHostReply				= MYPACKET + 3,
		StartGame					= MYPACKET + 4,
		FailStartGame				= MYPACKET + 5,
		EndRound					= MYPACKET + 6,
		JoinRoomReply				= MYPACKET + 7,
		RoomUserProperty			= MYPACKET + 8,
		MyProperty					= MYPACKET + 9,
		RequestUserInfo				= MYPACKET + 10,
		UserInfo					= MYPACKET + 11,
		ChangeOptMaxUsers			= MYPACKET + 12,
		ChangeOptGameLimit			= MYPACKET + 13,
		ChangeOptGradeLimit			= MYPACKET + 14,
		ChangeOptInstrusion			= MYPACKET + 15,
		ChangeOptTeamBalance		= MYPACKET + 16,
		ChangeOptMap				= MYPACKET + 17,
		RoomOption					= MYPACKET + 18,
		GetRoomInfo					= MYPACKET + 19,
		ChangeTeam					= MYPACKET + 20,
		CompulseExit				= MYPACKET + 21,
		Ready						= MYPACKET + 22,
		Kill						= MYPACKET + 23,
		EndGameRequest				= MYPACKET + 24,
		GameResultUser				= MYPACKET + 25,
		GameResultTeam				= MYPACKET + 26,
		GameResultSingle			= MYPACKET + 27,
		GameResultCoop				= MYPACKET + 28,
		RoomStateChange				= MYPACKET + 29,
		Instrution					= MYPACKET + 30,
		InstrutionReply				= MYPACKET + 31,
		ExitGameRequest				= MYPACKET + 32,
		ExitGameUser				= MYPACKET + 33,
		GameChat					= MYPACKET + 34,
		TeamChat					= MYPACKET + 35,
		LoadComplete				= MYPACKET + 36,
		StartNextRound				= MYPACKET + 37,
		ChangeRoomState				= MYPACKET + 38,
		Whisper						= MYPACKET + 39,
		TimeBombState				= MYPACKET + 40,
		RoundScore					= MYPACKET + 41,
	};// enum MyPacketCommand
};

#endif	//#ifndef __MURSUM_MYCOMMON_H__



#ifdef USE_TRACE

#ifndef MURSUM_TRACE
#define MURSUM_TRACE

#define MAX_TRACE_BUFFER_LEN	4096
#define MAX_LIMIT				4000

#include <stdio.h>
#include <stdlib.h>
class CMursumTrace
{
	char	m_szBuffer[MAX_TRACE_BUFFER_LEN];	

public:
	CMursumTrace():m_bEnableTrace(FALSE){};
	~CMursumTrace(){};

	BOOL		InitTracer( LPCSTR lpszFileName )
	{
		strcpy( m_szFileName, lpszFileName );
		m_bEnableTrace	= TRUE;

		return m_bEnableTrace;
	}

	void __cdecl Trace( const char* lpszText,... )
	{
		static SYSTEMTIME SystemTime;
		static char	szTime[64];
		GetLocalTime( &SystemTime );
		wsprintf( szTime, "[%04d.%02d.%02d %02d.%02d.%02d] ",
				SystemTime.wYear,
				SystemTime.wMonth,
				SystemTime.wDay,
				SystemTime.wHour,
				SystemTime.wMinute,
				SystemTime.wSecond );

		if( !lpszText || !m_bEnableTrace ) return;

		int			nBuf;
		va_list		args;

		va_start( args, lpszText );
		nBuf = vsprintf( m_szBuffer, lpszText, args );
		if( nBuf > MAX_LIMIT ) return;
		va_end( args );
		if( nBuf < 0 ) return;
		strcat( m_szBuffer, "\r\n" );

		printf( "%s%s", szTime, m_szBuffer );

		FILE*	fp = NULL;
		fp = fopen( m_szFileName, "a" );
		if( fp==NULL ) return;

		fprintf( fp, szTime );
		fprintf( fp, m_szBuffer );
		fclose(fp);		
	}

	void	DisbleTracer()
	{
		m_bEnableTrace	= FALSE;
	}

protected:
	BOOL	m_bEnableTrace;
	TCHAR	m_szFileName[MAX_PATH];
};

static CMursumTrace	g_MursumTrace;
extern CMursumTrace	g_MursumTrace;

#define MTRACE		g_MursumTrace.Trace
#define INIT_MTRACE	g_MursumTrace.InitTracer
#define DIS_TRACE	g_MursumTrace.DisbleTracer

#endif	//#ifndef MURSUM_TRACE

#else	//#ifdef USE_TRACE

#define MTRACE
#define INIT_MTRACE
#define DIS_TRACE

#endif	//#ifdef USE_TRACE