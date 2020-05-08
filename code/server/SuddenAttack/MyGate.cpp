#include "StdAfx.h"
#include ".\mygate.h"

#include "MyUserPocket.h"
#include "MyRoomPocket.h"
#include "MyChannelPocket.h"

#include <mmsystem.h>

#ifdef __TEST__
#pragma message( "=====================================================" )
#pragma message( " #define __TEST__  <-- 배포버전에서는 꼭 지우자!!!!! " )
#pragma message( "=====================================================" )
#endif

#define SCORE_DM_BASE	50
#define SCORE_SV_BASE	15

CMyGate::CMyGate(VOID)
{
}

CMyGate::~CMyGate(VOID)
{
}


BOOL CMyGate::Create( LP_SERVER_INFO pServerInfo )
{
	// added by minjin. 2005. 01. 31.
	// for Create a Gate by Server Information INI File..
	return Create(	pServerInfo->nMaxPacketSize,
					pServerInfo->nMaxClients,
					pServerInfo->nMaxResponseBufferSize,
					pServerInfo->nMaxRoomUser,
					pServerInfo->nMaxChannels,
					pServerInfo->szIP,
					pServerInfo->nPort );
}

BOOL CMyGate::Create( INT nMaxPacketSize, INT nMaxClients, INT nMaxResponseBufferSize, INT nMaxRoomUser, INT nMaxChannels, LPCSTR lpszIP, INT nPort )
{
	srand( ::timeGetTime() );

	if( !CNJeanGate::Create( nMaxPacketSize, nMaxClients, nMaxResponseBufferSize, nMaxRoomUser, nMaxChannels, nPort ) )
	{
		MTRACE( "ERROR  CNJeanGate::Create()" );
		return FALSE;
	}	

#ifdef MURSUM_TRACE
	SYSTEMTIME SystemTime;
	GetLocalTime( &SystemTime );

	TCHAR szFileName[MAX_PATH];
	wsprintf( szFileName, "[%d년%02d월%02d일 %d시%d분%d초]SA_DEV.LOG",
		SystemTime.wYear,
		SystemTime.wMonth,
		SystemTime.wDay,
		SystemTime.wHour,
		SystemTime.wMinute,
		SystemTime.wSecond );

	INIT_MTRACE( szFileName );

	MTRACE( "==[Start Sudden Attack Server]===========================" );

	MTRACE( "@@ Start Time\t\t: %d/%02d/%02d %d:%d:%d",
		SystemTime.wYear,
		SystemTime.wMonth,
		SystemTime.wDay,
		SystemTime.wHour,
		SystemTime.wMinute,
		SystemTime.wSecond );
	MTRACE( "@@ Address\t\t: %s (%d)", lpszIP, nPort );
	MTRACE( "@@ MaxPacketSize\t\t: %d", nMaxPacketSize );
	MTRACE( "@@ MaxClients\t\t: %d", nMaxClients );
	MTRACE( "@@ MaxResponseBufferSize\t: %d", nMaxResponseBufferSize );
	MTRACE( "@@ MaxRoomUser\t\t: %d", nMaxRoomUser );
	MTRACE( "@@ MaxChannels\t\t: %d", nMaxChannels );
#endif

	m_pPackets = new TagPackets;
	if( !m_pPackets ) 
	{
		MTRACE( "## Memory not Allocated!   m_pPacket = NULL" );
		return FALSE;
	}

	DB_INFO di;
	GetDBInfo( &di );

	INT nConnected = CNJeanGate::CreateDatabase( &di );

	if( nConnected != 4 )
	{
		MTRACE( "## ERROR CreateDataBase  Connected : %d", nConnected );
	}
	else
	{
		MTRACE( "@@ Create Database Success..." );
	}


	if( FALSE == m_GradeTable.Load( "./GameData.ini" ) )
	{
		MTRACE( "ERROR Missing Grade Table..." );
		return FALSE;
	}

	return TRUE;
}

BOOL
CMyGate::Create( INT nMaxPacketSize, INT nMaxClients, INT nMaxResponseBufferSize, INT nMaxRoomUser, INT nMaxChannels, INT nPort )
{
	MTRACE( "ERROR !!!!!!!!!!!!" );
	return FALSE;
}

VOID
CMyGate::Destroy()
{
#ifdef MURSUM_TRACE
	SYSTEMTIME SystemTime;
	GetLocalTime( &SystemTime );
	MTRACE( "==[End Sudden Attack Server]================================" );
	MTRACE( "\tEnd Time\t\t: %d/%02d/%02d %d:%d:%d", 
		SystemTime.wYear,
		SystemTime.wMonth,
		SystemTime.wDay,
		SystemTime.wHour,
		SystemTime.wMinute,
		SystemTime.wSecond );
#endif
	CNJeanGate::Destroy();
}

// Pocket을 생성한다.
CPocket* CMyGate::NewPocket( PocketType type )
{
	CPocket*	pPocket;

	switch( type )
	{
	case CNJeanGate::PocketType::User :		pPocket	= new CMyUserPocket;		break;
	case CNJeanGate::PocketType::Room :		pPocket	= new CMyRoomPocket;		break;
	case CNJeanGate::PocketType::Channel :	pPocket	= new CMyChannelPocket;		break;
	}

	return pPocket;
}

// Pocket을 제거한다.
VOID CMyGate::DeletePocket( PocketType type, CPocket* pPocket )
{
	switch( type )
	{
	case CNJeanGate::PocketType::User :
		CMyUserPocket*		pMyUserPocket;
		pMyUserPocket		= static_cast<CMyUserPocket*>( pPocket );
		delete pMyUserPocket;
		break;

	case CNJeanGate::PocketType::Room :
		CMyRoomPocket*		pMyRoomPocket;
		pMyRoomPocket		= static_cast<CMyRoomPocket*>( pPocket );
		delete pMyRoomPocket;
		break;

	case CNJeanGate::PocketType::Channel :
		CMyChannelPocket*	pMyChannelPocket;
		pMyChannelPocket	= static_cast<CMyChannelPocket*>( pPocket );
		delete pMyChannelPocket;
		break;
	}
}

// 채널을 초기화 한다.
VOID CMyGate::InitChannel( INT nChannelIndex, INT& out_nMaxUsers, INT& out_nMaxRooms, string& out_strChannelName )
{
	out_nMaxUsers		= 200;
	out_nMaxRooms		= 200;
	out_strChannelName	= "채널테스트";

	MTRACE( "==[ Init Channel ]======================================" );
	MTRACE( "\tIndex\t: %d", nChannelIndex );
	MTRACE( "\tMaxUser\t: %d", out_nMaxUsers );
	MTRACE( "\tMaxRoom\t: %d", out_nMaxRooms );
	MTRACE( "\tName\t: %s", out_strChannelName.c_str() );
	MTRACE( "\r\n" );
}

// 상위 클래스의 시그널 조건검사
BOOL CMyGate::CanBeCaptine( CUser* pUser )
{
	// 방장이 나갈때 호출되는 함수이나, 이 전에 IsCorrectChangeCaptine에서 FALSE를 반환했기에 절대 호출되지 않는다.

	BOOL bGame	= pUser->CanBeGameCaptine();	// 게임 룰에서 방장이 될 수 있는지
	BOOL bP2P	= pUser->CanBeP2PCaptine();		// P2P에서 방장이 될 수 있는지

	// 두 조건에 따른 결과 전송 : 게임에 따라 구현
	return bGame && bP2P;
}

// 방장이 없는 방이 생성될 때
VOID CMyGate::OnNoneCaptine( CRoom* pRoom )
{
}

// 상위 클래스 패킷의 조건검사
BOOL CMyGate::IsCorrectCondition( SHORT &nResult, CUser* pUser, PacketCondition condition, ULONG_PTR rParam, ULONG_PTR lParam )
{
	switch( condition )
	{
	case CNJeanGate::PacketCondition::JoinChannel :
		return IsCorrectJoinChannel( nResult, pUser, reinterpret_cast<CChannel*>( rParam ) );

	case CNJeanGate::PacketCondition::OpenRoom :
		return IsCorrectOpenRoom( nResult, pUser );

	case CNJeanGate::PacketCondition::JoinRoom :
		return IsCorrectJoinRoom( nResult, pUser, reinterpret_cast<CRoom*>( rParam ) );

	case CNJeanGate::PacketCondition::AutoJoinRoom :
		return IsCorrectAutoJoinRoom( nResult, pUser );

	case CNJeanGate::PacketCondition::LeaveRoom :
		return IsCorrectLeaveRoom( nResult, pUser );

	case CNJeanGate::PacketCondition::ChangeCaptine :
		return IsCorrectChangeCaptine( nResult, pUser, reinterpret_cast<CRoom*>( rParam ) );

	case CNJeanGate::PacketCondition::CompulseExit:
		return IsCorrectCompluseExit( nResult, pUser, reinterpret_cast<CUser*>(rParam), reinterpret_cast<CRoom*>(lParam) );

	default :
		return FALSE;
	}
}

inline BOOL
CMyGate::IsCorrectJoinChannel( SHORT &nResult, CUser* pUser, CChannel* pJoinChannel )
{
	//자유채널이 아닐경우 조건검사
	return TRUE;
}

inline BOOL
CMyGate::IsCorrectOpenRoom( SHORT &nResult, CUser* pUser )
{
	return TRUE;
}

inline BOOL
CMyGate::IsCorrectJoinRoom( SHORT &nResult, CUser* pUser, CRoom* pJoinRoom )
{
	CMyRoomPocket* pRoomPocket = static_cast<CMyRoomPocket*>( pJoinRoom->GetPocket() );

	//난입가능 검사
	if( !pRoomPocket ) return FALSE;

	if( ROOM_STEP_WAIT != pRoomPocket->GetStep() && !pJoinRoom->GetOption(OPT_INTRUSION) )
	{
		nResult	= ACK_ERR_INSTRUTION;
		return FALSE;
	}
    
	//등급제한 검사
	if( (char)pUser->GetGrade() < pJoinRoom->GetOption(OPT_GRADE_MIN_LIMIT) ||
		(char)pUser->GetGrade() > pJoinRoom->GetOption(OPT_GRADE_MAX_LIMIT) )
	{
		nResult	= ACK_ERR_GRADE_LIMIT;
		return FALSE;
	}

	return TRUE;
}

inline BOOL
CMyGate::IsCorrectAutoJoinRoom( SHORT &nResult, CUser* pUser )
{
	//아이템이 있을경우 사용~~!!
	return TRUE;
}

inline BOOL
CMyGate::IsCorrectLeaveRoom( SHORT &nResult, CUser* pUser )
{
	CMyUserPocket*	pUserPocket	= static_cast<CMyUserPocket*>( pUser->GetPocket() );
	if( pUserPocket )
	{
		if( pUserPocket->IsWaiting() && pUserPocket->IsCaptain() ) return TRUE;
		if( pUserPocket->IsWaiting() && !pUserPocket->IsReady() ) return TRUE;
	}

	nResult = ACK_ERR_DONT_EXIT_READY;
	return FALSE;
}

inline BOOL
CMyGate::IsCorrectCompluseExit( SHORT &nResult, CUser* pCaptain, CUser* pExitUser, CRoom* pRoom )
{
	CChannel*		pChannel;
	CRoom*			pCurrRoom;
	BOOL			bWaitRoom;
	CMyRoomPocket*	pRoomPocket;
	CMyUserPocket*	pUserPocket;
	GetUserPlace( pExitUser, pChannel, pCurrRoom, bWaitRoom );

	if( FALSE == pCaptain->IsCaptine() ) return FALSE;
	if( bWaitRoom || pCurrRoom != pRoom ) return FALSE;

	pUserPocket	= static_cast<CMyUserPocket*>( pExitUser->GetPocket() );
	pRoomPocket = static_cast<CMyRoomPocket*>( pRoom->GetPocket() );

	if( pRoomPocket->GetStep() != ROOM_STEP_WAIT ) return FALSE;
	
	return TRUE;
}

VOID
CMyGate::OnOpenRoomReply( CUser* pUser, CRoom* pRoom )
{
	MTRACE( "R(%d) U(%s) CreateRoom", pRoom->GetRoomIndex(), pUser->GetID().c_str() );

	CMyUserPocket*	pUserPocket;
	CMyRoomPocket*	pRoomPocket;	
	pUserPocket	= static_cast<CMyUserPocket*>( pUser->GetPocket() );
	pRoomPocket = static_cast<CMyRoomPocket*>( pRoom->GetPocket() );
	if( !pUserPocket || !pRoomPocket ) return;

	pUserPocket->InitGameRoomInfo();
	pUserPocket->SetState( PLAYER_STATE_WAITING|PLAYER_STATE_READY|PLAYER_STATE_CAPTAIN );

	T_ROOM_OPTION	RoomOption;
	RoomOption.nMapIndex		= pRoom->GetOption(OPT_MAP_INDEX);
	RoomOption.nGameType		= pRoom->GetOption(OPT_GAME_TYPE);
	RoomOption.nRounds			= pRoom->GetOption(OPT_ROUND_LIMIT);
	RoomOption.nTimeLimit		= GetLimitTime( RoomOption.nGameType, RoomOption.nMapIndex, pRoom->GetOption(OPT_TIME_LIMIT) );
	RoomOption.nKillLimit		= pRoom->GetOption(OPT_KILL_LIMIT);
	RoomOption.nGradeMinLimit	= pRoom->GetOption(OPT_GRADE_MIN_LIMIT);
	RoomOption.nGradeMaxLimit	= pRoom->GetOption(OPT_GRADE_MAX_LIMIT);
	RoomOption.nMaxUser			= pRoom->GetMaxUser();
	RoomOption.bInstrusion		= pRoom->GetOption(OPT_INTRUSION);
	RoomOption.bTeamBalance		= pRoom->GetOption(OPT_TEAM_BALANCE);
	pRoom->SetOption( OPT_GAME_STATE, GAME_STATE_WAIT );

	pRoomPocket->InitPocket( &RoomOption );
	pRoomPocket->AddPlayer_A( pUserPocket );

	CNJeanGate::OnOpenRoomReply( pUser, pRoom );

	MTRACE( "\tMAP(%d) TYPE(%d)", RoomOption.nMapIndex, RoomOption.nGameType );
	MTRACE( "R(%d) U(%s) CreateRoom - Success", pRoom->GetRoomIndex(), pUser->GetID().c_str() );
}


inline BOOL
CMyGate::IsCorrectChangeCaptine( SHORT &nResult, CUser* pUser, CRoom* pRoom )
{
	return TRUE;
}

VOID
CMyGate::OnPacketSecLogin( CUser* pUser, const char* pSecData, INT nDataSize )
{
	// 데이타 암호화 풀기

	// 풀린 암호를 바탕으로 사용자 로그인 처리를 한다.
	/*
	CNJeanGate::OnPacketLogin( pUser, strUserNo, strID, strNick, strIP );

	LoginQueryRequest( pUser, "http://221.142.247.141/BetweenGameAndDB/DBDaemon.asmx/getResult?Commands=_Method=GameNotice||_Game_Code=12||" );
	*/
}


#include "Atlbase.h"

VOID CMyGate::OnPacketLogin( CUser* pUser, LPCTSTR strUserNo, LPCTSTR strID, LPCTSTR strNick )
{
	CNJeanGate::OnPacketLogin( pUser, strUserNo, strID, strNick );

#ifdef __TEST__
	if( strcmp( pUser->GetUserNo().c_str() , __TEST_USERNO__ ) == 0 )
	{
		LoginReply( pUser, TRUE );
		return;
	}
#endif
	
	TCHAR	szLoginFlag[2] = {"0"};
	this->RequestLogin( pUser, pUser->GetUserNo().c_str(), pUser->GetIP().c_str(), szLoginFlag );	
}

VOID CMyGate::OnLoginSuccess( CUser* pUser )
{
	MTRACE( "U(%s) Login_Success (%s)", pUser->GetID().c_str(), pUser->GetIP().c_str() );
#ifdef __TEST__
	if( strcmp( pUser->GetUserNo().c_str(), __TEST_USERNO__ ) == 0 )
	{
		CMyUserPocket* pUserPocket	= static_cast<CMyUserPocket*>( pUser->GetPocket() );
		if( !pUserPocket ) return;

		T_USER_INFO UserInfo;
		memset( &UserInfo, 0, sizeof(T_USER_INFO) );
		strcpy( UserInfo.szID, pUser->GetID().c_str() );
		strcpy( UserInfo.szNick, pUser->GetNick().c_str() );
		UserInfo.nExp			= 0;
		UserInfo.nMoney			= 0;
		UserInfo.nGrade			= 0;
		UserInfo.nKill			= 0;
		UserInfo.nDead			= 0;
		UserInfo.nHeadShotKill	= 0;
		UserInfo.nHeadShotDead	= 0;
		UserInfo.nWin			= 0;
		UserInfo.nLose			= 0;
		UserInfo.nDraw			= 0;
		UserInfo.nState			= 0;

		pUserPocket->InitPocket( pUser, &UserInfo );

		strcpy( m_pPackets->MyProperty.m_szID, pUser->GetID().c_str() );
		m_pPackets->MyProperty.m_szID[ pUser->GetID().length() ] = NULL;
		strcpy( m_pPackets->MyProperty.m_szNick, pUser->GetNick().c_str() );
		m_pPackets->MyProperty.m_szNick[ pUser->GetNick().length() ] = NULL;

		m_pPackets->MyProperty.m_nExp			= pUserPocket->GetExp();
		m_pPackets->MyProperty.m_nGrade			= pUserPocket->GetGrade();
		m_pPackets->MyProperty.m_nKill			= pUserPocket->GetKillCount();
		m_pPackets->MyProperty.m_nDeath			= pUserPocket->GetDeadCount();
		m_pPackets->MyProperty.m_nMoney			= pUserPocket->GetMoney();
		m_pPackets->MyProperty.m_bCanbeCaptain	= this->CanBeCaptine( pUser );
		m_pPackets->MyProperty.m_nWin			= UserInfo.nWin;
		m_pPackets->MyProperty.m_nLose			= UserInfo.nLose;
		m_pPackets->MyProperty.m_nDraw			= UserInfo.nDraw;

		SendPacket( pUser, &m_pPackets->MyProperty, m_pPackets->MyProperty.GetLength() );
		return;
	}	
#endif
	this->RequestUserInfo( pUser, pUser->GetUserNo().c_str() );
}

VOID
CMyGate::OnCasualSignal( UINT uiMessage, ULONG_PTR wParam, ULONG_PTR lParam )
{
	/*/
	switch( uiMessage )
	{
	}
	/*/
}

// ADO에 해당하는 Response
VOID
CMyGate::OnCasualResponse( CUser* pUser, UINT uiMessage, BOOL bSuccess, _CommandPtr& pCmd )
{
	switch( uiMessage )
	{
	case CMyCommon::MyQueryMessage::LoginQuery :
		OnResponseLoginQuery( pUser, bSuccess, pCmd );
		break;

	case CMyCommon::MyQueryMessage::LogoutQuery:
		OnResponseLogoutQuery( pUser, bSuccess, pCmd );
		break;

	case CMyCommon::MyQueryMessage::UserInfoQuery:
		OnResponseUserInfo( pUser, bSuccess, pCmd );
		break;

	case CMyCommon::MyQueryMessage::GameResultQuery:
		OnResponseGameResultQuery( pUser, bSuccess, pCmd );
		break;
	}
}


VOID CMyGate::OnResponseLoginQuery( CUser* pUser, BOOL bSuccess, _CommandPtr& pCmd )
{
	if( !bSuccess )
	{
		MTRACE( "ERROR U(%s) ResponseLogin ERROR", pUser->GetID().c_str() );
		LoginReply( pUser, FALSE );
		return;
	}

	int nResult = pCmd->Parameters->GetItem( "@o_sp_rtn" )->Value.intVal;

	switch( nResult )
	{
	case 0:
		LoginReply( pUser, TRUE );
		MTRACE( "U(%s) ResponseLogin - Success", pUser->GetID().c_str() );
		break;
	case -1:
	case -2:
	case -3:
	case -4:
	default:
		MTRACE( "ERROR U(%s) ResponseLogin ERROR : Result = %d", pUser->GetID().c_str(), nResult );
		LoginReply( pUser, FALSE );
		break;
	}
}

VOID CMyGate::OnResponseLogoutQuery( CUser* pUser, BOOL bSuccess, _CommandPtr& pCmd )
{
	USES_CONVERSION;
	_bstr_t bstrUserNo	= pCmd->Parameters->GetItem( "@i_user_no" )->Value.bstrVal;	
	LPCTSTR lpszUserNo	= (LPCTSTR)OLE2T(bstrUserNo);

	if( !bSuccess )
	{
		MTRACE( "ERROR U(%s) ResponseLogout ERROR", lpszUserNo );
		return;
	}
	
	int nResult = pCmd->Parameters->GetItem( "@o_sp_rtn" )->Value.intVal;

	
	switch( nResult )
	{
	case 0:
		MTRACE( "U(%s) ResponseLogout - Success", lpszUserNo );
		break;
	case -1:
	case -2:
	case -3:
	case -4:
	default:
		MTRACE( "ERROR U(%s) ResponseLogout ERROR : Result = %d", lpszUserNo, nResult );
		break;
	}
}

VOID CMyGate::OnResponseUserInfo( CUser* pUser, BOOL bSuccess, _CommandPtr& pCmd )
{
	if( !bSuccess )
	{
		MTRACE( "ERROR U(%s) ResponseUserInfo ERROR", pUser->GetID().c_str() );		
		return;
	}

	int nResult = pCmd->Parameters->GetItem( "@o_sp_rtn" )->Value.intVal;

	switch( nResult )
	{
	case 0:
		this->SetUserInfo( pUser, pCmd );
		MTRACE( "U(%s) ResponseUserInfo - Success", pUser->GetID().c_str() );
		break;
	case -1:
	case -2:
	default:
		MTRACE( "ERROR U(%s) ResponseUserInfo ERROR : Result = %d", pUser->GetID().c_str(), nResult );
		break;
	}
}

VOID CMyGate::OnResponseGameResultQuery( CUser* pUser, BOOL bSuccess, _CommandPtr& pCmd )
{
	if( !bSuccess )
	{
		MTRACE( "ERROR U(%s) ResponseGameResult ERROR", pUser->GetID().c_str() );		
		return;
	}

	int nResult = pCmd->Parameters->GetItem( "@o_sp_rtn" )->Value.intVal;

	switch( nResult )
	{
	case 0:
		MTRACE( "U(%s) ResponseGameResult - Success", pUser->GetID().c_str() );
		break;
	case -1:
	case -2:
	default:
		MTRACE( "ERROR U(%s) ResponseGameResult ERROR : Result = %d", pUser->GetID().c_str(), nResult );
		break;
	}
}

VOID CMyGate::SetUserInfo( CUser* pUser, _CommandPtr& pCmd )
{
	CMyUserPocket* pUserPocket	= static_cast<CMyUserPocket*>( pUser->GetPocket() );
	if( !pUserPocket ) return;

	T_USER_INFO UserInfo;
	memset( &UserInfo, 0, sizeof(T_USER_INFO) );
	strcpy( UserInfo.szID, pUser->GetID().c_str() );
	strcpy( UserInfo.szNick, pUser->GetNick().c_str() );
	UserInfo.nExp			= pCmd->Parameters->GetItem( "@o_user_exp" )->Value.llVal;
	UserInfo.nMoney			= pCmd->Parameters->GetItem( "@o_user_money" )->Value.llVal;
	UserInfo.nGrade			= m_GradeTable.GetGradeFromExp(UserInfo.nExp);
	UserInfo.nKill			= pCmd->Parameters->GetItem( "@o_kill_cnt" )->Value.intVal;
	UserInfo.nDead			= pCmd->Parameters->GetItem( "@o_death_cnt" )->Value.intVal;
	UserInfo.nHeadShotKill	= pCmd->Parameters->GetItem( "@o_h_kill_cnt" )->Value.intVal;
	UserInfo.nHeadShotDead	= pCmd->Parameters->GetItem( "@o_h_death_cnt" )->Value.intVal;
	UserInfo.nWin			= pCmd->Parameters->GetItem( "@o_win_cnt" )->Value.intVal;
	UserInfo.nLose			= pCmd->Parameters->GetItem( "@o_lose_cnt" )->Value.intVal;
	UserInfo.nDraw			= pCmd->Parameters->GetItem( "@o_draw_cnt" )->Value.intVal;
	
	UserInfo.nState	= 0;

	pUserPocket->InitPocket( pUser, &UserInfo );

	strcpy( m_pPackets->MyProperty.m_szID, pUser->GetID().c_str() );
	m_pPackets->MyProperty.m_szID[ pUser->GetID().length() ] = NULL;
	strcpy( m_pPackets->MyProperty.m_szNick, pUser->GetNick().c_str() );
	m_pPackets->MyProperty.m_szNick[ pUser->GetNick().length() ] = NULL;

	m_pPackets->MyProperty.m_nExp			= pUserPocket->GetExp();
	m_pPackets->MyProperty.m_nGrade			= pUserPocket->GetGrade();
	m_pPackets->MyProperty.m_nKill			= pUserPocket->GetKillCount();
	m_pPackets->MyProperty.m_nDeath			= pUserPocket->GetDeadCount();
	m_pPackets->MyProperty.m_nMoney			= pUserPocket->GetMoney();
	m_pPackets->MyProperty.m_bCanbeCaptain	= this->CanBeCaptine( pUser );
	m_pPackets->MyProperty.m_nWin			= UserInfo.nWin;
	m_pPackets->MyProperty.m_nLose			= UserInfo.nLose;
	m_pPackets->MyProperty.m_nDraw			= UserInfo.nDraw;

	SendPacket( pUser, &m_pPackets->MyProperty, m_pPackets->MyProperty.GetLength() );
}

VOID
CMyGate::OnCasualAccept( CUser* pUser )
{
}

VOID
CMyGate::OnCasualConnect( CUser* pUser )
{
}

VOID
CMyGate::OnCasualClose( CUser* pUser )
{
	CChannel*		pChannel;
	CRoom*			pRoom;
	BOOL			bWaitRoom;
	CMyRoomPocket*	pRoomPocket;
	CMyUserPocket*	pUserPocket;

	GetUserPlace( pUser, pChannel, pRoom, bWaitRoom );
	if( pChannel == NULL ) return;

	TCHAR	szLogoutFlag[2] = {"0"};
    if( pRoom )
	{
		if( IsWaitRoom( pRoom ) )
		{
			MTRACE( "U(%s) OnClose", pUser->GetID().c_str() );
			this->RequestLogout( pUser, pUser->GetUserNo().c_str(), pUser->GetIP().c_str(), szLogoutFlag );			
			return;
		}

		// 비정상 종료 ======================================================
		pUserPocket = static_cast<CMyUserPocket*>( pUser->GetPocket() );
		pRoomPocket = static_cast<CMyRoomPocket*>( pRoom->GetPocket() );
		if( FALSE == pRoomPocket->RemovePlayer_A( pUser->GetID().c_str() ) )
		{
			pRoomPocket->RemovePlayer_B( pUser->GetID().c_str() );
		}

		if( pRoomPocket->GetPlayerCount_A() + pRoomPocket->GetPlayerCount_B() <= 0 )
		{
			MTRACE( "U(%s) OnClose 비정상종료 R(%d)", pUser->GetID().c_str(), pRoom->GetRoomIndex() );
			this->RequestLogout( pUser, pUser->GetUserNo().c_str(), pUser->GetIP().c_str(), szLogoutFlag );			
			return;
		}

		switch( pRoomPocket->GetStep() )
		{
		case ROOM_STEP_WAIT:		//게임 대기중
			MTRACE( "U(%s) OnClose 비정상종료 R(%d) STEP_WAIT", pUser->GetID().c_str(), pRoom->GetRoomIndex() );
			this->RequestLogout( pUser, pUser->GetUserNo().c_str(), pUser->GetIP().c_str(), szLogoutFlag );			
			return;

		case ROOM_STEP_PRE_GAME:	//세션 설정, 로딩
			MTRACE( "U(%s) OnClose 비정상종료 R(%d) STEP_PRE_GAME", pUser->GetID().c_str(), pRoom->GetRoomIndex() );

			if( pUserPocket->IsP2PHost() )					//새로운 세션설정
			{	
				//TEST==========================================
				//로딩실패
				StopLoading( pRoom, FAIL_HOST_DISCONNECTED );
				//==============================================
			}
			this->RequestLogout( pUser, pUser->GetUserNo().c_str(), pUser->GetIP().c_str(), szLogoutFlag );
			return;

		case ROOM_STEP_GAME:		//게임중
			MTRACE( "U(%s) OnClose 비정상종료 R(%d) STEP_GAME", pUser->GetID().c_str(), pRoom->GetRoomIndex() );

			if( pUserPocket->IsGaming() )
			{
				//TEST=====================================================
				if( pUserPocket->IsP2PHost() )
				{
					EndGame( pRoom, pRoomPocket );
					this->RequestLogout( pUser, pUser->GetUserNo().c_str(), pUser->GetIP().c_str(), szLogoutFlag );
					return;
				}
				//=========================================================


				if( FALSE == CheckEndRound_Kill( pRoom, pRoomPocket ) )	//게임이 안 끝나면
				{
					if( pUserPocket->IsP2PHost() )					//새로운 세션설정
					{	
						//시간이 얼마 안 남았으면 그냥 게임 종료
						//if( ::timeGetTime() - pRoomPocket->GetStartTick() <  )
						EndGame( pRoom, pRoomPocket );
					}
				}
				else
				{
					NextRound( pRoom, pRoomPocket );
				}
			}
			this->RequestLogout( pUser, pUser->GetUserNo().c_str(), pUser->GetIP().c_str(), szLogoutFlag );
			return;

		case ROOM_STEP_NEXT_ROUND:
			//TEST=====================================================
			if( pUserPocket->IsP2PHost() )
			{
				EndGame( pRoom, pRoomPocket );
				this->RequestLogout( pUser, pUser->GetUserNo().c_str(), pUser->GetIP().c_str(), szLogoutFlag );
				return;
			}
			//=========================================================

			if( FALSE == CheckEndRound_Kill( pRoom, pRoomPocket ) )	//게임이 안 끝나면
			{
				if( pUserPocket->IsP2PHost() )					//새로운 세션설정
				{	
					//시간이 얼마 안 남았으면 그냥 게임 종료
					//if( ::timeGetTime() - pRoomPocket->GetStartTick() <  )
					EndGame( pRoom, pRoomPocket );
				}
			}
			this->RequestLogout( pUser, pUser->GetUserNo().c_str(), pUser->GetIP().c_str(), szLogoutFlag );
			return;

		case ROOM_STEP_POST_GAME:	//게임끝, 결과창
			MTRACE( "U(%s) OnClose 비정상종료 R(%d) STEP_POST_GAME", pUser->GetID().c_str(), pRoom->GetRoomIndex() );
			this->RequestLogout( pUser, pUser->GetUserNo().c_str(), pUser->GetIP().c_str(), szLogoutFlag );			
			return;
		}
	}

	MTRACE( "U(%s) OnClose 비정상종료 -1", pUser->GetID().c_str() );
	this->RequestLogout( pUser, pUser->GetUserNo().c_str(), pUser->GetIP().c_str(), szLogoutFlag );
}

VOID
CMyGate::OnCasualData( CUser* pUser, CPacket* pPacket, INT nPacketLength )
{
	switch( pPacket->GetCommand() )
	{
	case CMyCommon::MyPacketCommand::Kill:
		{
			CMyPacketKill* pPacketKill;
			pPacketKill	= static_cast<CMyPacketKill*>( pPacket );
			if( !pPacketKill->IsCorrectLength( nPacketLength ) ) CloseUser( pUser );

			OnPacketKill( pUser, pPacketKill );
		}
		break;

	case CMyCommon::MyPacketCommand::EndGameRequest:
		{
			CMyPacketEndGameRequest* pEndGameRequest;
			pEndGameRequest	= static_cast<CMyPacketEndGameRequest*>( pPacket );
			if( !pEndGameRequest->IsCorrectLength( nPacketLength ) ) CloseUser( pUser );

			OnPacketEndGameRequest( pUser, pEndGameRequest );
		}
		break;

	case CMyCommon::MyPacketCommand::TimeBombState:
		{
			CMyPacketTimeBombState* pTimeBombState;
			pTimeBombState	= static_cast<CMyPacketTimeBombState*>( pPacket );
			if( !pTimeBombState->IsCorrectLength( nPacketLength ) ) CloseUser( pUser );

			OnPacketTimeBombState( pUser, pTimeBombState );
		}
		break;

	case CMyCommon::MyPacketCommand::RequestUserInfo :
		{
			CMyPacketRequestUserInfo* pPacketRequestUserInfo;
			pPacketRequestUserInfo	= static_cast<CMyPacketRequestUserInfo*>( pPacket );
			if( !pPacketRequestUserInfo->IsCorrectLength( nPacketLength ) ) CloseUser( pUser );

			OnPacketRequestUserInfo( pUser, pPacketRequestUserInfo );
		}
		break;

	case CMyCommon::MyPacketCommand::GetRoomInfo:
		{
			CMyPacketGetRoomInfo* pPacketGetRoomInfo;
			pPacketGetRoomInfo	= static_cast<CMyPacketGetRoomInfo*>( pPacket );
			if( !pPacketGetRoomInfo->IsCorrectLength( nPacketLength ) ) CloseUser( pUser );
			
			OnPacketGetRoomInfo( pUser, pPacketGetRoomInfo );
		}
		break;

	case CMyCommon::MyPacketCommand::ChangeTeam:
		{
			CMyPacketChangeTeam*	pPacketChangeTeam;
			pPacketChangeTeam	= static_cast<CMyPacketChangeTeam*>( pPacket );
			if( !pPacketChangeTeam->IsCorrectLength( nPacketLength ) ) CloseUser( pUser );

			OnPacketChangeTeam( pUser, pPacketChangeTeam );
		}
		break;

	case CMyCommon::MyPacketCommand::ChangeOptMap:
		{
			CMyPacketChangeOptMap*	pPacketChangeOptMap;
			pPacketChangeOptMap	= static_cast<CMyPacketChangeOptMap*>( pPacket );
			if( !pPacketChangeOptMap->IsCorrectLength( nPacketLength ) ) CloseUser( pUser );
			
			OnPacketChangeOptMap( pUser, pPacketChangeOptMap );
		}
		break;

	case CMyCommon::MyPacketCommand::ChangeOptMaxUsers:
		{
			CMyPacketChangeOptMaxUsers*	pPacketChangeOptMaxUsers;
			pPacketChangeOptMaxUsers	= static_cast<CMyPacketChangeOptMaxUsers*>( pPacket );
			if( !pPacketChangeOptMaxUsers->IsCorrectLength( nPacketLength ) ) CloseUser( pUser );
			
			OnPacketChangeOptMaxUsers( pUser, pPacketChangeOptMaxUsers );
		}
		break;

	case CMyCommon::MyPacketCommand::ChangeOptGameLimit:
		{
			CMyPacketChangeOptGameLimit*	pPacketChangeOptGameLimit;
			pPacketChangeOptGameLimit	= static_cast<CMyPacketChangeOptGameLimit*>( pPacket );
			if( !pPacketChangeOptGameLimit->IsCorrectLength( nPacketLength ) ) CloseUser( pUser );

			OnPacketChangeOptGameLimit( pUser, pPacketChangeOptGameLimit );
		}
		break;

	case CMyCommon::MyPacketCommand::ChangeOptGradeLimit:
		{
			CMyPacketChangeOptGradeLimit*	pPacketChangeOptGradeLimit;
			pPacketChangeOptGradeLimit	= static_cast<CMyPacketChangeOptGradeLimit*>( pPacket );
			if( !pPacketChangeOptGradeLimit->IsCorrectLength( nPacketLength ) ) CloseUser( pUser );
			
			OnPacketChangeOptGradeLimit( pUser, pPacketChangeOptGradeLimit );
		}
		break;

	case CMyCommon::MyPacketCommand::ChangeOptInstrusion:
		{
			CMyPacketChangeOptInstrusion*	pPacketChangeOptInstrusion;
			pPacketChangeOptInstrusion	= static_cast<CMyPacketChangeOptInstrusion*>( pPacket );
			if( !pPacketChangeOptInstrusion->IsCorrectLength( nPacketLength ) ) CloseUser( pUser );
			
			OnPacketChangeOptInstrusion( pUser, pPacketChangeOptInstrusion );
		}
		break;

	case CMyCommon::MyPacketCommand::ChangeOptTeamBalance:
		{
			CMyPacketChangeOptTeamBalance*	pPacketChangeOptTeamBalance;
			pPacketChangeOptTeamBalance	= static_cast<CMyPacketChangeOptTeamBalance*>( pPacket );
			if( !pPacketChangeOptTeamBalance->IsCorrectLength( nPacketLength ) ) CloseUser( pUser );

			OnPacketChangeOptTeamBalance( pUser, pPacketChangeOptTeamBalance );
		}
		break;

	case CMyCommon::MyPacketCommand::StartGameRequest:
		{
			CMyPacketStartGameRequest*	pPacketStartGameRequest;
			pPacketStartGameRequest	= static_cast<CMyPacketStartGameRequest*>( pPacket );
			if( !pPacketStartGameRequest->IsCorrectLength( nPacketLength ) ) CloseUser( pUser );

			OnPacketStartGameRequest( pUser, pPacketStartGameRequest );
		}
		break;

	case CMyCommon::MyPacketCommand::StartHostReply:
		{
			CMyPacketStartHostReply*	pPacketStartHostReply;
			pPacketStartHostReply	= static_cast<CMyPacketStartHostReply*>( pPacket );
			if( !pPacketStartHostReply->IsCorrectLength( nPacketLength ) ) CloseUser( pUser );

			OnPacketStartHostReply( pUser, pPacketStartHostReply );
		}
		break;

	case CMyCommon::MyPacketCommand::Instrution:
		{
			CMyPacketInstrution* pPacketInstrution;
			pPacketInstrution	= static_cast<CMyPacketInstrution*>( pPacket );
			if( !pPacketInstrution->IsCorrectLength( nPacketLength ) ) CloseUser( pUser );

			OnPacketInstrution( pUser, pPacketInstrution );
		}
		break;

	case CMyCommon::MyPacketCommand::Ready:
		{
			CMyPacketReady*	pPacketReady;
			pPacketReady	= static_cast<CMyPacketReady*>( pPacket );
			if( !pPacketReady->IsCorrectLength( nPacketLength ) ) CloseUser( pUser );

			OnPacketReady( pUser, pPacketReady );
		}
		break;

	case CMyCommon::MyPacketCommand::ExitGameRequest:
		{
			CMyPacketExitGameRequest*	pPacketExitGameRequest;
			pPacketExitGameRequest	= static_cast<CMyPacketExitGameRequest*>( pPacket );
			if( !pPacketExitGameRequest->IsCorrectLength( nPacketLength ) ) CloseUser( pUser );

			OnPacketExitGameRequest( pUser, pPacketExitGameRequest );
		}
		break;

	case CMyCommon::MyPacketCommand::GameChat:
		{
			CMyPacketGameChat*	pPacketGameChat;
			pPacketGameChat	= static_cast<CMyPacketGameChat*>( pPacket );
			if( pPacketGameChat->GetRealLength() != nPacketLength ) CloseUser( pUser );

			OnPacketGameChat( pUser, pPacketGameChat );
		}
		break;

	case CMyCommon::MyPacketCommand::TeamChat:
		{
			CMyPacketTeamChat*	pPacketTeamChat;
			pPacketTeamChat	= static_cast<CMyPacketTeamChat*>( pPacket );
			if( pPacketTeamChat->GetRealLength() != nPacketLength ) CloseUser( pUser );

			OnPacketTeamChat( pUser, pPacketTeamChat );
		}
		break;

	case CMyCommon::MyPacketCommand::Whisper:
		{
			CMyPacketWhisper*	pPacketWhisper;
			pPacketWhisper	= static_cast<CMyPacketWhisper*>( pPacket );
			if( pPacketWhisper->GetRealLength() != nPacketLength ) CloseUser( pUser );

			OnPacketWhisper( pUser, pPacketWhisper );
		}
		break;

	case CMyCommon::MyPacketCommand::LoadComplete:
		{
			CMyPacketLoadComplete*	pPacketLoadComplete;
			pPacketLoadComplete	= static_cast<CMyPacketLoadComplete*>( pPacket );
			if( !pPacketLoadComplete->IsCorrectLength(nPacketLength) ) CloseUser( pUser );

			OnPacketLoadComplete( pUser, pPacketLoadComplete );
		}
		break;

	case CMyCommon::MyPacketCommand::StartNextRound:
		{
			CMyPacketStartNextRound*	pPacketStartNextRound;
			pPacketStartNextRound	= static_cast<CMyPacketStartNextRound*>( pPacket );
			if( !pPacketStartNextRound->IsCorrectLength(nPacketLength) ) CloseUser( pUser );

			OnPacketStartNextRound( pUser, pPacketStartNextRound );
		}
		break;
	}
}

BOOL
CMyGate::OnCasualTimeout( CUser* pUser )
{
	MTRACE( "U(%12s) Time Out NICK: %s", pUser->GetUserNo().c_str(), pUser->GetNick().c_str() );
	//return TRUE;
	return FALSE;
}

VOID
CMyGate::OnLeaveRoom( CUser* pUser, CRoom* pRoom, CCasualPacketCompulsionExited::Type ExitType )
{
	MTRACE( "R(%d) U(%s) LeaveRoom", pRoom->GetRoomIndex(), pUser->GetID().c_str() );

	CMyUserPocket*	pUserPocket;
	CMyRoomPocket*	pRoomPocket;

	pUserPocket = static_cast<CMyUserPocket*>( pUser->GetPocket() );
	pRoomPocket = static_cast<CMyRoomPocket*>( pRoom->GetPocket() );
	if( !pUserPocket || !pRoomPocket ) return;

	//if( pUserPocket->IsReady() )
	//{
	//}

	if( FALSE == pRoomPocket->RemovePlayer_A( pUser->GetID().c_str() ) )
	{
		pRoomPocket->RemovePlayer_B( pUser->GetID().c_str() );
	}

	pUserPocket->SetState( PLAYER_STATE_LOBBY );
	MTRACE( "R(%d) U(%s) LeaveRoom - Success", pRoom->GetRoomIndex(), pUser->GetID().c_str() );
}
 
VOID
CMyGate::OnChangeCaptine( CUser* pCaptineUser, CRoom* pRoom )                                          
{
	CMyUserPocket* pUserPocket	= static_cast<CMyUserPocket*>( pCaptineUser->GetPocket() );
	if( !pUserPocket ) return;

	pUserPocket->AddState( PLAYER_STATE_CAPTAIN );
	pUserPocket->AddState( PLAYER_STATE_READY );

	CNJeanGate::OnChangeCaptine( pCaptineUser, pRoom );
	MTRACE( "R(%d) U(%s) ChangeCaptain", pRoom->GetRoomIndex(), pCaptineUser->GetID().c_str() );
}

VOID
CMyGate::OnPacketRequestUserInfo( CUser* pUser, CMyPacketRequestUserInfo* pRequestUserInfo )
{
	CUser*			pFindUser;
	CMyUserPocket*	pFindUserPocket;
	T_USER_INFOptr	pUserInfo;

	if( !pUser || !pRequestUserInfo ) return;

	pFindUser = m_UserMap.FindFromID( pRequestUserInfo->m_szID );
    if( !pFindUser ) return;

	pFindUserPocket	= static_cast<CMyUserPocket*>( pFindUser->GetPocket() );
	if( !pFindUserPocket ) return;

	pUserInfo = pFindUserPocket->GetUserInfo();

	if( pRequestUserInfo->m_bDetail )
	{
		// 상세정보창을 사용할 경우...
		// 아직은 기능이 없다.
	}
	else
	{
		m_pPackets->UserInfo.m_nGrade		= pUserInfo->nGrade;
		m_pPackets->UserInfo.m_nKill		= pUserInfo->nKill;
		m_pPackets->UserInfo.m_nDeath		= pUserInfo->nDead;
		m_pPackets->UserInfo.m_nWin			= pUserInfo->nWin;
		m_pPackets->UserInfo.m_nLose		= pUserInfo->nLose;
		m_pPackets->UserInfo.m_nDraw		= pUserInfo->nDraw;
		m_pPackets->UserInfo.m_nMoney		= pUserInfo->nMoney;
		strcpy( m_pPackets->UserInfo.m_szNick, pFindUser->GetNick().c_str() );

		SendPacket( pUser, &m_pPackets->UserInfo, m_pPackets->UserInfo.GetLength() );
	}
}

VOID
CMyGate::OnPacketGetRoomInfo( CUser* pUser, CMyPacketGetRoomInfo* pGetRoomInfo )
{
	CChannel*	pChannel;
	CRoom*		pRoom;
	BOOL		bWaitRoom;

	GetUserPlace( pUser, pChannel, pRoom, bWaitRoom );
	if( bWaitRoom || !pRoom ) return;

	MTRACE( "R(%d) U(%s) GetRoomInfo", pRoom->GetRoomIndex(), pUser->GetID().c_str() );

	CMyRoomPocket* pRoomPocket = static_cast<CMyRoomPocket*>( pRoom->GetPocket() );
	if( !pRoomPocket ) return;

	this->SendRoomUserToJoinUser( pUser, pRoom, pRoomPocket );

	T_ROOM_OPTIONptr pRoomOption = pRoomPocket->GetRoomOption();
	m_pPackets->RoomOption.m_nMapIndex		= pRoomOption->nMapIndex;
	m_pPackets->RoomOption.m_nGameType		= pRoomOption->nGameType;
	m_pPackets->RoomOption.m_nRoundLimit	= pRoom->GetOption(OPT_ROUND_LIMIT);
	m_pPackets->RoomOption.m_nTimeLimit		= pRoomOption->nTimeLimit;
	m_pPackets->RoomOption.m_nKillLimit		= pRoomOption->nKillLimit;
	m_pPackets->RoomOption.m_nGradeMinLimit	= pRoomOption->nGradeMinLimit;
	m_pPackets->RoomOption.m_nGradeMaxLimit	= pRoomOption->nGradeMaxLimit;	
	m_pPackets->RoomOption.m_nMaxUser		= pRoomOption->nMaxUser;	
	m_pPackets->RoomOption.m_bInstrusion	= pRoomOption->bInstrusion;
	m_pPackets->RoomOption.m_bTeamBalance	= pRoomOption->bTeamBalance;
	m_pPackets->RoomOption.m_nRoomStep		= (BYTE)pRoomPocket->GetStep();
	SendPacket( pUser, &m_pPackets->RoomOption, m_pPackets->RoomOption.GetLength() );

	MTRACE( "R(%d) U(%s) GetRoomInfo - Success", pRoom->GetRoomIndex(), pUser->GetID().c_str() );
}

//타임오버~
VOID
CMyGate::OnPacketEndGameRequest( CUser* pUser, CMyPacketEndGameRequest* pEndGameRequest )
{
	CChannel*	pChannel;
	CRoom*		pRoom;
	BOOL		bWaitRoom;

	GetUserPlace( pUser, pChannel, pRoom, bWaitRoom );
	if( bWaitRoom || !pRoom ) return;
	MTRACE( "R(%d) U(%s) EndGameRequest", pRoom->GetRoomIndex(), pUser->GetID().c_str() );

	CMyUserPocket* pUserPocket = static_cast<CMyUserPocket*>( pUser->GetPocket() );
    CMyRoomPocket* pRoomPocket = static_cast<CMyRoomPocket*>( pRoom->GetPocket() );
	if( !pRoomPocket || ROOM_STEP_GAME != pRoomPocket->GetStep() ) return;
	if( FALSE == pUserPocket->IsGaming() ) return;
	
	MTRACE( "R(%d) U(%s) EndGameRequest - Success", pRoom->GetRoomIndex(), pUser->GetID().c_str() );

	CheckWinTeam_TimeOut( pRoom, pRoomPocket );
	NextRound( pRoom, pRoomPocket );
}

VOID
CMyGate::OnPacketKill( CUser* pUser, CMyPacketKill* pKill )
{
	CChannel*	pChannel;
	CRoom*		pRoom;
	BOOL		bWaitRoom;

	GetUserPlace( pUser, pChannel, pRoom, bWaitRoom );
	if( bWaitRoom || !pRoom ) return;

	CMyRoomPocket* pRoomPocket = static_cast<CMyRoomPocket*>( pRoom->GetPocket() );
	if( !pRoomPocket || ROOM_STEP_GAME != pRoomPocket->GetStep() ) return;

	//pKill->m_nHitNode;
	MTRACE( "R(%d)  Kill %s->%s (%d)", pRoom->GetRoomIndex(), pKill->m_szKillerID, pKill->m_szVictimID, pKill->m_nHitNode );

	if( pRoomPocket->CanRespawn() )
	{
		// Team Balance ---	리스폰
		this->SetTeamBalanceAfterDeath( pRoom, pRoomPocket, pRoomPocket->GetPlayer( pKill->m_szVictimID ) );
	}
	
	int nTeam1;
	int nTeam2;

	CMyUserPocket* pTemp;
	pTemp = pRoomPocket->GetPlayer( pKill->m_szKillerID );
	if( pTemp )
	{
		nTeam1 = pTemp->GetUserTeam();
	}
	else
	{
		MTRACE( "ERROR Killer Not Found" );
		return;
	}

	pTemp = pRoomPocket->GetPlayer( pKill->m_szVictimID );
	if( pTemp )
	{
		nTeam2 = pTemp->GetUserTeam();
	}
	else
	{
		MTRACE( "ERROR Victim Not Found" );
		return;
	}

	if( strcmp( pKill->m_szKillerID, pKill->m_szVictimID ) != 0 &&		//자살이 아니고
		nTeam1 != nTeam2 )	//팀킬이 아니면
	{
		pRoomPocket->AddKillCount( pKill->m_szKillerID, pKill->m_szVictimID, pKill->m_nHitNode );
	}
    pRoomPocket->AddDeadCount( pKill->m_szKillerID, pKill->m_szVictimID, pKill->m_nHitNode );

	if( TRUE == CheckEndRound_Kill( pRoom, pRoomPocket ) )
	{
		NextRound( pRoom, pRoomPocket );
	}
}

BOOL
CMyGate::NextRound( CRoom* pRoom, CMyRoomPocket* pRoomPocket )
{
	UINT			nGameExp;
	UINT			nGameMoney;
	CUser*			pUser;
	CMyUserPocket*	pUserPocket;
	int				nWinRoundTeam	= pRoomPocket->GetWinRoundTeam();

	pRoomPocket->SetStep( ROOM_STEP_NEXT_ROUND );

	if( TEAM_A == nWinRoundTeam )
	{
		pRoomPocket->AddWinRound_A();
	}
	else if( TEAM_B == nWinRoundTeam )
	{
		pRoomPocket->AddWinRound_B();
	}
	else
	{
		pRoomPocket->AddDrawRound();
	}

	//점수계산===============================================================
	pUser = pRoom->GetFirstUser();
	while( pUser )
	{
		pUserPocket	= static_cast<CMyUserPocket*>( pUser->GetPocket() );

		if( !pUserPocket ||
			!pUserPocket->IsGaming() ||
			pUserPocket->IsSpectator() )
		{
			pUser = pRoom->GetNextUser();
			continue;
		}

		nGameExp	= GetGameExp( pUserPocket, pRoomPocket, !!(pUserPocket->GetUserTeam()==nWinRoundTeam) );
		nGameMoney	= GetGameMoney( pUserPocket, pRoomPocket, !!(pUserPocket->GetUserTeam()==nWinRoundTeam) );		

		pUserPocket->AddGameExp( nGameExp );
		pUserPocket->AddGameMoney( nGameMoney );
		pUser = pRoom->GetNextUser();
	}



	--pRoomPocket->GetRoomOption()->nRounds;


	//게임끝=================================================================
	//라운드 타입이 아닌경우
	if( pRoomPocket->GetRoomOption()->nGameType == GAME_TYPE_TDM )
	{
		return EndGame( pRoom, pRoomPocket );
	}

	int nInGameCount_A = pRoomPocket->GetPlayerCount_A_InGame();
	int nInGameCount_B = pRoomPocket->GetPlayerCount_B_InGame();
	int	nInSpecCount_A = pRoomPocket->GetPlayerCount_A_Spectator();
	int	nInSpecCount_B = pRoomPocket->GetPlayerCount_B_Spectator();

	//모든 라운드가 끝난경우 || 게임중인 유저가 1명 있는경우
	if( pRoomPocket->GetRoomOption()->nRounds <= 0 ||
		nInGameCount_A+nInGameCount_B+nInSpecCount_A+nInSpecCount_B <= 1 )
	{
		return EndGame( pRoom, pRoomPocket );
	}

	if( FALSE == pRoomPocket->GetRoomOption()->bTeamBalance )	//팀벨런스가 아니고
	{
		if( nInGameCount_A+nInSpecCount_A <= 0 || nInGameCount_B+nInSpecCount_B <= 0 )//한쪽 팀이 0명이면
		{
			return EndGame( pRoom, pRoomPocket );
		}
	}
	//=======================================================================

	MTRACE( "==========================================" );
	MTRACE( "R(%d) NextRound  Limit(%d)", pRoom->GetRoomIndex(), pRoomPocket->GetRoomOption()->nRounds );
    //NextRound...
	pRoomPocket->InitRoundInfo();
	
	// Team Balance --- 다음 라운드
	this->SetTeamBalanceStartGame( pRoom, pRoomPocket );
	
	m_pPackets->EndRound.m_nGameType		= pRoomPocket->GetRoomOption()->nGameType;
	m_pPackets->EndRound.m_bRestartRound	= TRUE;
	m_pPackets->EndRound.m_nWinRound_A	= pRoomPocket->GetWinRound_A();
	m_pPackets->EndRound.m_nWinRound_B	= pRoomPocket->GetWinRound_B();

	if( GAME_TYPE_TSV != pRoomPocket->GetRoomOption()->nGameType )
	{
		m_pPackets->EndRound.m_nHow	= END_ROUND_NONE;
	}
	else
	{
		if( pRoomPocket->GetTimeBombState() == 1 )
		{
			m_pPackets->EndRound.m_nHow = END_ROUND_BOMB_DEFUSED;
		}
		else if( pRoomPocket->GetTimeBombState() == 2 )
		{
			m_pPackets->EndRound.m_nHow = END_ROUND_BOMB_DETONATED;
		}
		else
		{
			m_pPackets->EndRound.m_nHow = END_ROUND_NONE;
		}
	}

	pUser = pRoom->GetFirstUser();
	while( pUser )
	{
		pUserPocket	= static_cast<CMyUserPocket*>( pUser->GetPocket() );

		if( nWinRoundTeam == TEAM_NONE )
		{
			m_pPackets->EndRound.m_nGameResult	= GAME_RESULT_DRAW;
		}
		else if( nWinRoundTeam == pUserPocket->GetUserTeam() )
		{
			m_pPackets->EndRound.m_nGameResult	= GAME_RESULT_WIN;
		}
		else
		{
			m_pPackets->EndRound.m_nGameResult	= GAME_RESULT_LOSE;
		}
        SendPacket( pUser, &m_pPackets->EndRound, m_pPackets->EndRound.GetLength() );

		pUser = pRoom->GetNextUser();
	}

	MTRACE( "\tWIN TEAM(%d)  ROUND SCORE %d : %d",
			nWinRoundTeam, pRoomPocket->GetWinRound_A(), pRoomPocket->GetWinRound_B() );
	MTRACE( "R(%d) NextRound  Limit(%d) - Success", pRoom->GetRoomIndex(), pRoomPocket->GetRoomOption()->nRounds );
	MTRACE( "==========================================" );
	return TRUE;
}

BOOL 
CMyGate::EndGame( CRoom* pRoom, CMyRoomPocket* pRoomPocket )
{
	MTRACE( "==========================================" );
	MTRACE( "R(%d) EndGame", pRoom->GetRoomIndex() );

	int		i;
	int		nWinRoundTeam;
	int		nWinGameTeam;
	int		nIndex			= 0;
	int		nGameUser		= pRoomPocket->GetPlayerCount_A_InGame()+pRoomPocket->GetPlayerCount_B_InGame();

	int					nGameExp, nGameMoney;
	CUser*				pUser;
	CMyUserPocket*		pUserPocket;
	T_USER_INFOptr		pUserInfo;
	T_USER_GAME_INFOptr	pUserGameInfo;

	pRoomPocket->GetRoomOption()->nRounds = 0;
	pRoomPocket->SetStep( ROOM_STEP_POST_GAME );

	nWinRoundTeam	= pRoomPocket->GetWinRoundTeam();
	nWinGameTeam	= pRoomPocket->GetWinGameTeam();
		
	//개인별 결과 입력------------------------------------------------------------
	for( i=0 ; i<pRoomPocket->GetPlayerCount_A() ; ++i )
	{
		pUserPocket		= pRoomPocket->GetPlayer_A(i);
		if( NULL == pUserPocket ||
			!pUserPocket->IsGaming() ||
			pUserPocket->IsSpectator() ) continue;

		pUserInfo		= pUserPocket->GetUserInfo();
		pUserGameInfo	= pUserPocket->GetUserGameInfo();
		pUser			= pUserPocket->GetUserClass();

		strcpy( m_pPackets->GameResultUser[nIndex].m_szID, pUserInfo->szID );
		strcpy( m_pPackets->GameResultUser[nIndex].m_szNick, pUserInfo->szNick );

		switch( nWinGameTeam )
		{
		case TEAM_A:
			pUserPocket->SetWinCount( pUserPocket->GetWinCount()+1 );
			nGameExp	= pUserPocket->GetGameExp()*2;
			nGameMoney	= pUserPocket->GetGameMoney()*2;
			break;
		case TEAM_B:
			pUserPocket->SetLoseCount( pUserPocket->GetLoseCount()+1 );
			nGameExp	= pUserPocket->GetGameExp();
			nGameMoney	= pUserPocket->GetGameMoney();
			break;
		default:
			pUserPocket->SetDrawCount( pUserPocket->GetDrawCount()+1 );
			nGameExp	= pUserPocket->GetGameExp();
			nGameMoney	= pUserPocket->GetGameMoney();
			break;
		}

		pUserPocket->SetKillCount( pUserPocket->GetKillCount()+pUserPocket->GetGameKill() );
		pUserPocket->SetDeadCount( pUserPocket->GetDeadCount()+pUserPocket->GetGameDead() );
		pUserPocket->SetHKillCount( pUserPocket->GetHKillCount()+pUserPocket->GetGameHKill() );
		pUserPocket->SetHDeadCount( pUserPocket->GetHDeadCount()+pUserPocket->GetGameHDead() );
		pUserPocket->SetExp( pUserPocket->GetExp()+nGameExp );
		pUserPocket->SetGrade( m_GradeTable.GetGradeFromExp(pUserPocket->GetExp()) );
		pUserPocket->SetMoney( pUserPocket->GetMoney()+nGameMoney );
		
		m_pPackets->GameResultUser[nIndex].m_nTeam		= TEAM_A;
		m_pPackets->GameResultUser[nIndex].m_nGameType	= pRoomPocket->GetRoomOption()->nGameType;
		m_pPackets->GameResultUser[nIndex].m_nGrade		= pUserPocket->GetGrade();
		m_pPackets->GameResultUser[nIndex].m_nKill		= pUserPocket->GetGameKill();
		m_pPackets->GameResultUser[nIndex].m_nDead		= pUserPocket->GetGameDead();
		m_pPackets->GameResultUser[nIndex].m_nExp		= nGameExp;
		m_pPackets->GameResultUser[nIndex].m_nMoney		= nGameMoney;
		m_pPackets->GameResultUser[nIndex].m_nWinCount	= pUserPocket->GetWinRound();
		m_pPackets->GameResultUser[nIndex].m_nLoseCount	= pUserPocket->GetLoseRound();
		m_pPackets->GameResultUser[nIndex].m_nDrawCount	= pUserPocket->GetDrawRound();

		//DB Update...
		this->RequestGameResult( pUser,
								 pUser->GetUserNo().c_str(),  
								 m_pPackets->GameResultUser[nIndex].m_nExp,
								 m_pPackets->GameResultUser[nIndex].m_nMoney,
								 m_pPackets->GameResultUser[nIndex].m_nKill,
								 m_pPackets->GameResultUser[nIndex].m_nDead,
								 pUserPocket->GetGameHKill(),
								 pUserPocket->GetGameHDead(),
								 m_pPackets->GameResultUser[nIndex].m_nWinCount,
								 m_pPackets->GameResultUser[nIndex].m_nLoseCount,
								 m_pPackets->GameResultUser[nIndex].m_nDrawCount );

		MTRACE( "\t%s TEAM(%d) Grade(%d) Kill(%d) Dead(%d) Exp(%d) Money(%d) W(%d) L(%d) D(%d)",
			pUserInfo->szID,
			m_pPackets->GameResultUser[nIndex].m_nTeam,
			m_pPackets->GameResultUser[nIndex].m_nGrade,
			m_pPackets->GameResultUser[nIndex].m_nKill,
			m_pPackets->GameResultUser[nIndex].m_nDead,
			m_pPackets->GameResultUser[nIndex].m_nExp,
			m_pPackets->GameResultUser[nIndex].m_nMoney,
			m_pPackets->GameResultUser[nIndex].m_nWinCount,
			m_pPackets->GameResultUser[nIndex].m_nLoseCount,
			m_pPackets->GameResultUser[nIndex].m_nDrawCount );

		++nIndex;
	}

	for( i=0 ; i<pRoomPocket->GetPlayerCount_B() ; ++i )
	{
		pUserPocket		= pRoomPocket->GetPlayer_B(i);
		if( NULL == pUserPocket ||
			!pUserPocket->IsGaming() ||
			pUserPocket->IsSpectator() ) continue;

		pUserInfo		= pUserPocket->GetUserInfo();
		pUserGameInfo	= pUserPocket->GetUserGameInfo();
		pUser			= pUserPocket->GetUserClass();
		
		strcpy( m_pPackets->GameResultUser[nIndex].m_szID, pUserInfo->szID );
		strcpy( m_pPackets->GameResultUser[nIndex].m_szNick, pUserInfo->szNick );

		switch( nWinGameTeam )
		{
		case TEAM_A:
			pUserPocket->SetWinCount( pUserPocket->GetWinCount()+1 );
			nGameExp	= pUserPocket->GetGameExp();
			nGameMoney	= pUserPocket->GetGameMoney();
			break;
		case TEAM_B:
			pUserPocket->SetLoseCount( pUserPocket->GetLoseCount()+1 );
			nGameExp	= pUserPocket->GetGameExp()*2;
			nGameMoney	= pUserPocket->GetGameMoney()*2;
			break;
		default:
			pUserPocket->SetDrawCount( pUserPocket->GetDrawCount()+1 );
			nGameExp	= pUserPocket->GetGameExp();
			nGameMoney	= pUserPocket->GetGameMoney();
			break;
		}

		pUserPocket->SetKillCount( pUserPocket->GetKillCount()+pUserPocket->GetGameKill() );
		pUserPocket->SetDeadCount( pUserPocket->GetDeadCount()+pUserPocket->GetGameDead() );
		pUserPocket->SetExp( pUserPocket->GetExp()+nGameExp );
		pUserPocket->SetGrade( m_GradeTable.GetGradeFromExp(pUserPocket->GetExp()) );
		pUserPocket->SetMoney( pUserPocket->GetMoney()+nGameMoney );

		m_pPackets->GameResultUser[nIndex].m_nTeam		= TEAM_B;
		m_pPackets->GameResultUser[nIndex].m_nGameType	= pRoomPocket->GetRoomOption()->nGameType;
		m_pPackets->GameResultUser[nIndex].m_nGrade		= pUserPocket->GetGrade();
		m_pPackets->GameResultUser[nIndex].m_nKill		= pUserPocket->GetGameKill();
		m_pPackets->GameResultUser[nIndex].m_nDead		= pUserPocket->GetGameDead();
		m_pPackets->GameResultUser[nIndex].m_nExp		= nGameExp;
		m_pPackets->GameResultUser[nIndex].m_nMoney		= nGameMoney;
		m_pPackets->GameResultUser[nIndex].m_nWinCount	= pUserPocket->GetWinRound();
		m_pPackets->GameResultUser[nIndex].m_nLoseCount	= pUserPocket->GetLoseRound();
		m_pPackets->GameResultUser[nIndex].m_nDrawCount	= pUserPocket->GetDrawRound();

		//DB Update...
		this->RequestGameResult( pUser,
								 pUser->GetUserNo().c_str(),  
								 m_pPackets->GameResultUser[nIndex].m_nExp,
								 m_pPackets->GameResultUser[nIndex].m_nMoney,
								 m_pPackets->GameResultUser[nIndex].m_nKill,
								 m_pPackets->GameResultUser[nIndex].m_nDead,
								 pUserPocket->GetGameHKill(),
								 pUserPocket->GetGameHDead(),
								 m_pPackets->GameResultUser[nIndex].m_nWinCount,
								 m_pPackets->GameResultUser[nIndex].m_nLoseCount,
								 m_pPackets->GameResultUser[nIndex].m_nDrawCount );

		MTRACE( "\t%s TEAM(%d) Grade(%d) Kill(%d) Dead(%d) Exp(%d) Money(%d) W(%d) L(%d) D(%d)",
			pUserInfo->szID,
			m_pPackets->GameResultUser[nIndex].m_nTeam,
			m_pPackets->GameResultUser[nIndex].m_nGrade,
			m_pPackets->GameResultUser[nIndex].m_nKill,
			m_pPackets->GameResultUser[nIndex].m_nDead,
			m_pPackets->GameResultUser[nIndex].m_nExp,
			m_pPackets->GameResultUser[nIndex].m_nMoney,
			m_pPackets->GameResultUser[nIndex].m_nWinCount,
			m_pPackets->GameResultUser[nIndex].m_nLoseCount,
			m_pPackets->GameResultUser[nIndex].m_nDrawCount );
        
		++nIndex;
	}

	//라운드 결과 입력------------------------------------------------
	m_pPackets->EndRound.m_nGameType		= pRoomPocket->GetRoomOption()->nGameType;
	m_pPackets->EndRound.m_bRestartRound	= FALSE;
	m_pPackets->EndRound.m_nWinRound_A	= pRoomPocket->GetWinRound_A();
	m_pPackets->EndRound.m_nWinRound_B	= pRoomPocket->GetWinRound_B();

	//게임결과 입력---------------------------------------------------
	m_pPackets->GameResultTeam.m_nGameType		= pRoomPocket->GetRoomOption()->nGameType;
	m_pPackets->GameResultTeam.m_nWinTeam		= nWinGameTeam;
	
	MTRACE( "\tWIN TEAM(%d)  ROUND SCORE %d : %d",
		m_pPackets->GameResultTeam.m_nWinTeam, m_pPackets->EndRound.m_nWinRound_A, m_pPackets->EndRound.m_nWinRound_B );

	//전송------------------------------------------------------------
	pUser = pRoom->GetFirstUser();
	while( pUser )
	{
		pUserPocket	= static_cast<CMyUserPocket*>( pUser->GetPocket() );

		if( nWinRoundTeam == TEAM_NONE )
		{
			m_pPackets->EndRound.m_nGameResult	= GAME_RESULT_DRAW;
		}
		else if( nWinRoundTeam == pUserPocket->GetUserTeam() )
		{
			m_pPackets->EndRound.m_nGameResult	= GAME_RESULT_WIN;
		}
		else
		{
			m_pPackets->EndRound.m_nGameResult	= GAME_RESULT_LOSE;
		}
		SendPacket( pUser, &m_pPackets->EndRound, m_pPackets->EndRound.GetLength() );
			
		for( i=0 ; i<nGameUser ; ++i )
		{
			SendPacket( pUser, &m_pPackets->GameResultUser[i], m_pPackets->GameResultUser[i].GetLength() );
		}
		
		SendPacket( pUser, &m_pPackets->GameResultTeam, m_pPackets->GameResultTeam.GetLength() );			
		
		pUser = pRoom->GetNextUser();
	}
	

	//초기화-----------------------------------------------------------------
	pRoomPocket->InitWaitData();
	pRoomPocket->SetStep( ROOM_STEP_WAIT );
	pRoom->SetOption( OPT_GAME_STATE, GAME_STATE_WAIT );

	CChannel*		pChannel;	
	pChannel = GetChannelRef( pRoom->GetChannelIndex() );	
	if( pChannel )
	{
		m_pPackets->RoomStateChange.m_nRoomIndex	= pRoom->GetRoomIndex();
		m_pPackets->RoomStateChange.m_nRoomState	= (int)pRoom->GetOption(OPT_GAME_STATE);
		SendToWaitRoom( pChannel, NULL, &m_pPackets->RoomStateChange, m_pPackets->RoomStateChange.GetLength() );
	}

	MTRACE( "R(%d) EndGame - Success", pRoom->GetRoomIndex() );
	MTRACE( "==========================================" );
	
	return TRUE;
}

VOID
CMyGate::OnPacketChangeTeam( CUser* pUser, CMyPacketChangeTeam* pChangeTeam )
{
	CChannel*	pChannel;
	CRoom*		pRoom;
	BOOL		bWaitRoom;

	GetUserPlace( pUser, pChannel, pRoom, bWaitRoom );
	if( bWaitRoom || !pRoom ) return;

	CMyUserPocket* pUserPocket = static_cast<CMyUserPocket*>( pUser->GetPocket() );
	CMyRoomPocket* pRoomPocket = static_cast<CMyRoomPocket*>( pRoom->GetPocket() );
	if( !pUserPocket || !pRoomPocket ) return;

	if( pUserPocket->IsGaming() ) return;	//게임중
	
	pChangeTeam->m_nTeam	= pRoomPocket->ChangeTeam( pUser->GetID().c_str() );
	if( TEAM_ERROR != pChangeTeam->m_nTeam )
	{
		SendToRoom( pRoom, NULL, pChangeTeam, pChangeTeam->GetLength() );
	}
}

VOID
CMyGate::OnPacketChangeOptMap( CUser* pUser, CMyPacketChangeOptMap* pChangeOptMap )
{
	CChannel*	pChannel;
	CRoom*		pRoom;
	BOOL		bWaitRoom;

	GetUserPlace( pUser, pChannel, pRoom, bWaitRoom );
	if( bWaitRoom || !pRoom ||
		!pUser->IsCaptine() ||
		pRoom->GetRoomIndex() != pChangeOptMap->m_nRoomIndex ) return;

	CMyRoomPocket* pRoomPocket = static_cast<CMyRoomPocket*>( pRoom->GetPocket() );
	if( !pRoomPocket|| ROOM_STEP_WAIT != pRoomPocket->GetStep() ) return;

	if( pRoomPocket->GetRoomOption()->nGameType	!= pChangeOptMap->m_nGameType ||
		pRoomPocket->GetRoomOption()->nMapIndex	!= pChangeOptMap->m_nMapIndex )
	{
		pRoomPocket->GetRoomOption()->nGameType	= pChangeOptMap->m_nGameType;
		pRoomPocket->GetRoomOption()->nMapIndex	= pChangeOptMap->m_nMapIndex;
		pRoom->SetOption( OPT_GAME_TYPE, pChangeOptMap->m_nGameType );
		pRoom->SetOption( OPT_MAP_INDEX, pChangeOptMap->m_nMapIndex );

		SendToRoom( pRoom, NULL, pChangeOptMap, pChangeOptMap->GetLength() );
		SendToWaitRoom( pChannel, NULL, pChangeOptMap, pChangeOptMap->GetLength() );
	}
}

VOID
CMyGate::OnPacketChangeOptMaxUsers( CUser* pUser, CMyPacketChangeOptMaxUsers* pChangeOptMaxUsers )
{
	CChannel*	pChannel;
	CRoom*		pRoom;
	BOOL		bWaitRoom;

	GetUserPlace( pUser, pChannel, pRoom, bWaitRoom );
	if( bWaitRoom || !pRoom ||
		!pUser->IsCaptine() ||
		pRoom->GetRoomIndex() != pChangeOptMaxUsers->m_nRoomIndex ) return;

	CMyRoomPocket* pRoomPocket = static_cast<CMyRoomPocket*>( pRoom->GetPocket() );
	if( !pRoomPocket|| ROOM_STEP_WAIT != pRoomPocket->GetStep() ) return;

	if( pRoomPocket->GetRoomOption()->nMaxUser != (BYTE)pChangeOptMaxUsers->m_nMaxUsers )
	{
		pRoomPocket->GetRoomOption()->nMaxUser	= (BYTE)pChangeOptMaxUsers->m_nMaxUsers;
		pRoom->SetMaxUser( pChangeOptMaxUsers->m_nMaxUsers );

		SendToRoom( pRoom, NULL, pChangeOptMaxUsers, pChangeOptMaxUsers->GetLength() );
		SendToWaitRoom( pChannel, NULL, pChangeOptMaxUsers, pChangeOptMaxUsers->GetLength() );
	}
}

VOID
CMyGate::OnPacketChangeOptGameLimit( CUser* pUser, CMyPacketChangeOptGameLimit* pChangeOptGameLimit )
{
	CChannel*	pChannel;
	CRoom*		pRoom;
	BOOL		bWaitRoom;

	GetUserPlace( pUser, pChannel, pRoom, bWaitRoom );
	if( bWaitRoom || !pRoom ||
		!pUser->IsCaptine() ||
		pRoom->GetRoomIndex() != pChangeOptGameLimit->m_nRoomIndex ) return;

	CMyRoomPocket* pRoomPocket = static_cast<CMyRoomPocket*>( pRoom->GetPocket() );
	if( !pRoomPocket || ROOM_STEP_WAIT != pRoomPocket->GetStep() ) return;

	if( pRoomPocket->GetRoomOption()->nKillLimit != pChangeOptGameLimit->m_nKillLimit ||
        pRoomPocket->GetRoomOption()->nTimeLimit != pChangeOptGameLimit->m_nTimeLimit ||
		pRoomPocket->GetRoomOption()->nRounds != pChangeOptGameLimit->m_nRoundLimit )
	{
		pRoomPocket->GetRoomOption()->nKillLimit	= pChangeOptGameLimit->m_nKillLimit;
		pRoomPocket->GetRoomOption()->nTimeLimit	= pChangeOptGameLimit->m_nTimeLimit;
		pRoomPocket->GetRoomOption()->nRounds		= pChangeOptGameLimit->m_nRoundLimit;
		pRoom->SetOption( OPT_KILL_LIMIT, pChangeOptGameLimit->m_nKillLimit );
		pRoom->SetOption( OPT_TIME_LIMIT, pChangeOptGameLimit->m_nTimeLimit );
		pRoom->SetOption( OPT_ROUND_LIMIT, pChangeOptGameLimit->m_nRoundLimit );

		SendToRoom( pRoom, NULL, pChangeOptGameLimit, pChangeOptGameLimit->GetLength() );
		SendToWaitRoom( pChannel, NULL, pChangeOptGameLimit, pChangeOptGameLimit->GetLength() );
	}
}

VOID
CMyGate::OnPacketChangeOptGradeLimit( CUser* pUser, CMyPacketChangeOptGradeLimit* pChangeOptGradeLimit )
{
	CChannel*	pChannel;
	CRoom*		pRoom;
	BOOL		bWaitRoom;

	GetUserPlace( pUser, pChannel, pRoom, bWaitRoom );
	if( bWaitRoom || !pRoom ||
		!pUser->IsCaptine() ||
		pRoom->GetRoomIndex() != pChangeOptGradeLimit->m_nRoomIndex ) return;

	CMyRoomPocket* pRoomPocket = static_cast<CMyRoomPocket*>( pRoom->GetPocket() );
	if( !pRoomPocket|| ROOM_STEP_WAIT != pRoomPocket->GetStep() ) return;

	if( pRoomPocket->GetRoomOption()->nGradeMinLimit != pChangeOptGradeLimit->m_nMinLimit ||
		pRoomPocket->GetRoomOption()->nGradeMaxLimit != pChangeOptGradeLimit->m_nMaxLimit )
	{
		pRoomPocket->GetRoomOption()->nGradeMinLimit	= pChangeOptGradeLimit->m_nMinLimit;
		pRoomPocket->GetRoomOption()->nGradeMaxLimit	= pChangeOptGradeLimit->m_nMaxLimit;
		pRoom->SetOption( OPT_GRADE_MIN_LIMIT, pChangeOptGradeLimit->m_nMinLimit );
		pRoom->SetOption( OPT_GRADE_MAX_LIMIT, pChangeOptGradeLimit->m_nMaxLimit );

		SendToRoom( pRoom, NULL, pChangeOptGradeLimit, pChangeOptGradeLimit->GetLength() );
		SendToWaitRoom( pChannel, NULL, pChangeOptGradeLimit, pChangeOptGradeLimit->GetLength() );
	}
}

VOID
CMyGate::OnPacketChangeOptInstrusion( CUser* pUser, CMyPacketChangeOptInstrusion* pChangeOptInstrusion )
{
	RequestLogout( pUser, pUser->GetUserNo().c_str(), pUser->GetIP().c_str(), "0" );

	return;			// minjin Test

	CChannel*	pChannel;
	CRoom*		pRoom;
	BOOL		bWaitRoom;

	GetUserPlace( pUser, pChannel, pRoom, bWaitRoom );
	if( bWaitRoom || !pRoom ||
		!pUser->IsCaptine() ||
		pRoom->GetRoomIndex() != pChangeOptInstrusion->m_nRoomIndex ) return;

	CMyRoomPocket* pRoomPocket = static_cast<CMyRoomPocket*>( pRoom->GetPocket() );
	if( !pRoomPocket|| ROOM_STEP_WAIT != pRoomPocket->GetStep() ) return;

	if( pRoomPocket->GetRoomOption()->bInstrusion != (BYTE)pChangeOptInstrusion->m_bEnable )
	{
		pRoomPocket->GetRoomOption()->bInstrusion	= (BYTE)pChangeOptInstrusion->m_bEnable;
		pRoom->SetOption( OPT_INTRUSION, (BYTE)pChangeOptInstrusion->m_bEnable );

		SendToRoom( pRoom, NULL, pChangeOptInstrusion, pChangeOptInstrusion->GetLength() );
		SendToWaitRoom( pChannel, NULL, pChangeOptInstrusion, pChangeOptInstrusion->GetLength() );
	}
}

VOID
CMyGate::OnPacketChangeOptTeamBalance( CUser* pUser, CMyPacketChangeOptTeamBalance* pChangeOptTeamBalance )
{
	CChannel*	pChannel;
	CRoom*		pRoom;
	BOOL		bWaitRoom;

	GetUserPlace( pUser, pChannel, pRoom, bWaitRoom );
	if( bWaitRoom || !pRoom ||
		!pUser->IsCaptine() ||
		pRoom->GetRoomIndex() != pChangeOptTeamBalance->m_nRoomIndex ) return;

	CMyRoomPocket* pRoomPocket = static_cast<CMyRoomPocket*>( pRoom->GetPocket() );
	if( !pRoomPocket || ROOM_STEP_WAIT != pRoomPocket->GetStep() ) return;

	if( pRoomPocket->GetRoomOption()->bTeamBalance != (BYTE)pChangeOptTeamBalance->m_bEnable )
	{
		pRoomPocket->GetRoomOption()->bTeamBalance	= (BYTE)pChangeOptTeamBalance->m_bEnable;
		pRoom->SetOption( OPT_TEAM_BALANCE, (BYTE)pChangeOptTeamBalance->m_bEnable );

		SendToRoom( pRoom, NULL, pChangeOptTeamBalance, pChangeOptTeamBalance->GetLength() );
		//SendToWaitRoom( pChannel, NULL, pChangeOptTeamBalance, pChangeOptTeamBalance->GetLength() );
	}
}

VOID
CMyGate::OnPacketStartGameRequest( CUser* pUser, CMyPacketStartGameRequest* pStartGameRequest )
{
	CChannel*	pChannel;
	CRoom*		pRoom;
	BOOL		bWaitRoom;

	GetUserPlace( pUser, pChannel, pRoom, bWaitRoom );
	if( bWaitRoom || !pRoom ) return;

	MTRACE( "R(%d) U(%d) StartGameRequest", pRoom->GetRoomIndex(), pUser->GetID().c_str() );

	CMyRoomPocket* pRoomPocket = static_cast<CMyRoomPocket*>( pRoom->GetPocket() );
	if( !pRoomPocket ) return;
	if( FALSE == pRoomPocket->GetRoomOption()->bInstrusion &&
		ROOM_STEP_WAIT != pRoomPocket->GetStep() ) return;

	if( FALSE == CheckStart( pUser, pRoom, pRoomPocket ) ) return;	
	pRoomPocket->SetStep( ROOM_STEP_PRE_GAME );

	// Team Balance --- 게임시작
	this->SetTeamBalanceStartGame( pRoom, pRoomPocket );
	
	if( pStartGameRequest->m_bCanBeCaptain )
	{
		SendPacket( pUser, &m_pPackets->StartHost, m_pPackets->StartHost.GetLength() );
		MTRACE( "R(%d) U(%d) StartGameRequest - Success", pRoom->GetRoomIndex(), pUser->GetID().c_str() );
	}
	else
	{
		MTRACE( "R(%d) U(%d) StartGameRequest - DecideHost", pRoom->GetRoomIndex(), pUser->GetID().c_str() );
		pRoomPocket->SetCanBeCaptain( pUser->GetID().c_str(), FALSE );
		
		DecideHost( pRoom, pRoomPocket, NULL );
	}
}

VOID
CMyGate::OnPacketStartHostReply( CUser* pUser, CMyPacketStartHostReply* pStartHostReply )
{
	CChannel*	pChannel;
	CRoom*		pRoom;
	BOOL		bWaitRoom;

	GetUserPlace( pUser, pChannel, pRoom, bWaitRoom );
	if( bWaitRoom || !pRoom ) return;

	CUser*			pRoomUser;
	CMyUserPocket*	pUserPocket;
	CMyRoomPocket*	pRoomPocket = static_cast<CMyRoomPocket*>( pRoom->GetPocket() );
	if( !pRoomPocket || ROOM_STEP_PRE_GAME != pRoomPocket->GetStep() ) return;

	MTRACE( "R(%d) U(%d) Start Game", pRoom->GetRoomIndex(), pUser->GetID().c_str() );

	if( pStartHostReply->m_bHostStarted )
	{
		pRoomPocket->GetRoomOption()->nRounds	= pRoom->GetOption(OPT_ROUND_LIMIT);
		pRoomPocket->InitGameInfo();
		pRoomPocket->SetHostInfo( pUser->GetID().c_str(), pStartHostReply->m_szHostIP,  pStartHostReply->m_nHostPort);

		strcpy( m_pPackets->StartGame.m_szIP, pRoomPocket->GetHostIP() );
		m_pPackets->StartGame.m_nPort			= pRoomPocket->GetHostPort();
		m_pPackets->StartGame.m_bSpectator		= FALSE;

		m_pPackets->ChangeRoomState.m_nState	= ROOM_STEP_GAME;
		
		pRoomUser	= pRoom->GetFirstUser();
		while( pRoomUser )
		{
			pUserPocket	= static_cast<CMyUserPocket*>( pRoomUser->GetPocket() );
			if( pUserPocket && pUserPocket->IsReady() )
			{
				pUserPocket->AddState( PLAYER_STATE_LOADING );
				pUserPocket->AddState( PLAYER_STATE_READY );
				pUserPocket->InitRoundInfo();

				if( pRoomUser != pUser )
				{
					pUserPocket->SetSpectatorMode(FALSE);
					SendPacket( pRoomUser, &m_pPackets->StartGame, m_pPackets->StartGame.GetLength() );
				}
			}
			else
			{
				if( pRoomUser != pUser )
				{
					SendPacket( pRoomUser, &m_pPackets->ChangeRoomState, m_pPackets->ChangeRoomState.GetLength() );
				}
			}
			
			pRoomUser	= pRoom->GetNextUser();
		}

		pRoomPocket->SetStep( ROOM_STEP_GAME );
		pRoom->SetOption( OPT_GAME_STATE, GAME_STATE_PLAY );
		
		m_pPackets->RoomStateChange.m_nRoomIndex	= pRoom->GetRoomIndex();
		m_pPackets->RoomStateChange.m_nRoomState	= (int)pRoom->GetOption(OPT_GAME_STATE);
		SendToWaitRoom( pChannel, NULL, &m_pPackets->RoomStateChange, m_pPackets->RoomStateChange.GetLength() );
		MTRACE( "R(%d) Start Game HOST( %s, %s, %d )", 
				pRoom->GetRoomIndex(), pUser->GetID().c_str(), pStartHostReply->m_szHostIP, pStartHostReply->m_nHostPort );
	}
	else
	{
		pRoomPocket->SetCanBeCaptain( pUser->GetID().c_str(), FALSE );
		DecideHost( pRoom, pRoomPocket, NULL );
	}
}

VOID
CMyGate::OnPacketInstrution( CUser* pUser, CMyPacketInstrution* pInstrution )
{
	CChannel*	pChannel;
	CRoom*		pRoom;
	BOOL		bWaitRoom;

	GetUserPlace( pUser, pChannel, pRoom, bWaitRoom );
	if( bWaitRoom || !pRoom ) return;

	CMyUserPocket*	pUserPocket	= static_cast<CMyUserPocket*>( pUser->GetPocket() );
	CMyRoomPocket*	pRoomPocket = static_cast<CMyRoomPocket*>( pRoom->GetPocket() );
	if( !pRoomPocket || ROOM_STEP_GAME != pRoomPocket->GetStep() ) return;

	BOOL	bSpectator	= FALSE;
	DWORD	dwTimeLeft = ::timeGetTime() - pRoomPocket->GetStartTick();
	if( GAME_TYPE_TSV == pRoomPocket->GetRoomOption()->nGameType && dwTimeLeft > INSTRUTION_LIMIT_TICK )
	{
		bSpectator	= TRUE;
	} 
	
	// Team Balance --- 난입
	this->SetTeamBalanceInstrution( pRoom, pRoomPocket, pUserPocket );

	strcpy( m_pPackets->StartGame.m_szIP, pRoomPocket->GetHostIP() );
	m_pPackets->StartGame.m_nPort		= pRoomPocket->GetHostPort();
	m_pPackets->StartGame.m_bSpectator	= (BYTE)bSpectator;	
	
	SendPacket( pUser, &m_pPackets->StartGame, m_pPackets->StartGame.GetLength() );
	
	if( pUserPocket )
	{
		pUserPocket->AddState( PLAYER_STATE_LOADING );
		pUserPocket->AddState( PLAYER_STATE_READY );
		pUserPocket->InitGameInfo();
		pUserPocket->InitRoundInfo();
		pUserPocket->SetSpectatorMode( bSpectator );
	}
}

VOID
CMyGate::OnPacketExitGameRequest( CUser* pUser, CMyPacketExitGameRequest* pExitGameRequest )
{
	CChannel*	pChannel;
	CRoom*		pRoom;
	BOOL		bWaitRoom;
	BOOL		bGaming;

	GetUserPlace( pUser, pChannel, pRoom, bWaitRoom );
	if( bWaitRoom || !pRoom ) return;

	MTRACE( "R(%d) U(%s) ExitGameRequest", pRoom->GetRoomIndex(), pUser->GetID().c_str() );

	CMyUserPocket*	pUserPocket = static_cast<CMyUserPocket*>( pUser->GetPocket() );
	CMyRoomPocket*	pRoomPocket = static_cast<CMyRoomPocket*>( pRoom->GetPocket() );
	if( !pRoomPocket ||	!pUserPocket ) return;
	if(	!pUserPocket->IsGaming() && !pUserPocket->IsLoading() ) return;

	bGaming	= pUserPocket->IsGaming();

	pUserPocket->RemoveState( PLAYER_STATE_GAMING );
	pUserPocket->RemoveState( PLAYER_STATE_LOADING );
	pUserPocket->RemoveState( PLAYER_STATE_READY );
	pUserPocket->AddState( PLAYER_STATE_WAITING );

	strcpy( m_pPackets->ExitGameUser.m_szExitGameUser, pUser->GetID().c_str() );

	SendToRoom( pRoom, NULL, &m_pPackets->ExitGameUser, m_pPackets->ExitGameUser.GetLength() );

	MTRACE( "R(%d) U(%s) ExitGameRequest - Success", pRoom->GetRoomIndex(), pUser->GetID().c_str() );

	if( bGaming )
	{
		if( pUserPocket->IsP2PHost() )
		{
			EndGame( pRoom, pRoomPocket );
		}
		else if( TRUE == CheckEndRound_Kill( pRoom, pRoomPocket ) )
		{
			NextRound( pRoom, pRoomPocket );
		}
	}
	else	//로딩중 튕긴경우
	{
		if( pUserPocket->IsP2PHost() )
		{
			//게임 종료
		}		
	}
}

VOID
CMyGate::DecideHost( CRoom* pRoom, CMyRoomPocket* pRoomPocket, CUser* pExeptUser )
{
	CUser*	pHost		= NULL;	
	CUser*	pUser		= NULL;
	DWORD	dwMinRTT	= 1000000;

	pUser = pRoom->GetFirstUser();
	while( pUser )
	{
		if( pUser!=pExeptUser &&
			TRUE == pRoomPocket->CanBeCaptain( pUser->GetID().c_str() ) &&
			CanBeCaptine( pUser ) )
		{
			if( dwMinRTT > GetRttTime( pUser ) )
			{
				pHost = pUser;
				dwMinRTT = GetRttTime( pUser );
			}
		}
		pUser = pRoom->GetNextUser();
	}

	if( pHost == NULL )
	{	
		/*
		MTRACE( "R(%d) DecideHost FAIL", pRoom->GetRoomIndex() );
		m_pPackets->FailStartGame.m_nResult	= FAIL_HOST;
		SendToRoom( pRoom, NULL, &m_pPackets->FailStartGame, m_pPackets->FailStartGame.GetLength() );
		pRoomPocket->SetStep( ROOM_STEP_WAIT );
		*/
		//NAT

	//////////////////////////////////////////////////////////////////////////
	// 공유기 내에 있는 유저 검사
	// ( BEGINE )
		int nDebugCnt = 0;
		BOOL	bInSameBoundary = TRUE;			// 같은 공유기 안에 있을경우!

		LPVOID	pos = NULL;
		pUser	= NULL;
		pUser	= pRoom->GetFirstUser();
		pos		= pRoom->GetFirstUser();

		while ( pos )
		{
			if( NULL != lstrcmp(GetPeerAddr((CUser*)pos), GetPeerAddr((CUser*)pUser)) )
			{
				bInSameBoundary = FALSE;
				break;
			}
			pos = (LPVOID)pRoom->GetNextUser();
		}
		
		if ( FALSE == bInSameBoundary )		return;			// 나중에 수정한다. -0-;;

		pos = NULL;
		pos = pRoom->GetFirstUser();

		while ( pos )
		{
			if ( ((CUser*)pos)->IsCaptine() )
			{
				pHost = (CUser*)pos;
				MTRACE( "R(%d) U(%s) DecideHost [All Users are in Same Boundary, This is Captine]", pRoom->GetRoomIndex(), pHost->GetID().c_str() );
				SendPacket( pHost, &m_pPackets->StartHost, m_pPackets->StartHost.GetLength() );
				break;
			}
			pos = (LPVOID)pRoom->GetNextUser();
		}
	// ( END )
	//////////////////////////////////////////////////////////////////////////
	}
	else
	{	
		MTRACE( "R(%d) U(%s) DecideHost", pRoom->GetRoomIndex(), pHost->GetID().c_str() );
		SendPacket( pHost, &m_pPackets->StartHost, m_pPackets->StartHost.GetLength() );
	}

}

VOID
CMyGate::OnPacketReady( CUser* pUser, CMyPacketReady* pReady )
{
	CChannel*	pChannel;
	CRoom*		pRoom;
	BOOL		bWaitRoom;

	GetUserPlace( pUser, pChannel, pRoom, bWaitRoom );
	if( bWaitRoom || !pRoom ) return;

	CMyRoomPocket* pRoomPocket = static_cast<CMyRoomPocket*>( pRoom->GetPocket() );
	if( !pRoomPocket || ROOM_STEP_WAIT != pRoomPocket->GetStep() ) return;

	CMyUserPocket* pUserPocket = static_cast<CMyUserPocket*>( pUser->GetPocket() );
	if( !pUserPocket ) return;

	if( FALSE == pUserPocket->IsGaming() &&	pReady->m_bReady != pUserPocket->IsReady() )
	{
		SendToRoom( pRoom, NULL, pReady, pReady->GetLength() );

		if( pReady->m_bReady )
		{
			pUserPocket->AddState( PLAYER_STATE_READY );
		}
		else
		{
			pUserPocket->RemoveState( PLAYER_STATE_READY );
		}
	}
}

VOID
CMyGate::OnPacketGameChat( CUser* pUser, CMyPacketGameChat* pGameChat )
{
	CChannel*	pChannel;
	CRoom*		pRoom;
	BOOL		bWaitRoom;
	CUser*		pRoomUser;

	GetUserPlace( pUser, pChannel, pRoom, bWaitRoom );
	if( bWaitRoom || !pRoom ) return;

	CMyUserPocket* pUserPocket;
	CMyRoomPocket* pRoomPocket = static_cast<CMyRoomPocket*>( pRoom->GetPocket() );
	if( !pRoomPocket || ROOM_STEP_GAME != pRoomPocket->GetStep() ) return;
    
	pRoomUser	= pRoom->GetFirstUser();

	while( pRoomUser )
	{
		pUserPocket	= static_cast<CMyUserPocket*>( pRoomUser->GetPocket() );

		if( pUser != pRoomUser && pUserPocket->IsGaming() )
		{
			SendPacket( pRoomUser, pGameChat, pGameChat->GetLength() );
		}

		pRoomUser	= pRoom->GetNextUser();
	}
}

VOID
CMyGate::OnPacketTeamChat( CUser* pUser, CMyPacketTeamChat* pTeamChat )
{
	CChannel*	pChannel;
	CRoom*		pRoom;
	BOOL		bWaitRoom;
	CUser*		pRoomUser;
	int			nTeam;

	GetUserPlace( pUser, pChannel, pRoom, bWaitRoom );
	if( bWaitRoom || !pRoom ) return;

	CMyUserPocket* pUserPocket = static_cast<CMyUserPocket*>( pUser->GetPocket() );
	CMyRoomPocket* pRoomPocket = static_cast<CMyRoomPocket*>( pRoom->GetPocket() );
	if( !pRoomPocket || ROOM_STEP_GAME != pRoomPocket->GetStep() ) return;

	nTeam		= pUserPocket->GetUserTeam();
	pRoomUser	= pRoom->GetFirstUser();

	while( pRoomUser )
	{
		pUserPocket	= static_cast<CMyUserPocket*>( pRoomUser->GetPocket() );

		if( pUser != pRoomUser &&
			pUserPocket->IsGaming() &&
			pUserPocket->GetUserTeam() == nTeam )
		{
			SendPacket( pRoomUser, pTeamChat, pTeamChat->GetLength() );
		}

		pRoomUser	= pRoom->GetNextUser();
	}
}

VOID
CMyGate::OnPacketWhisper( CUser* pUser, CMyPacketWhisper* pWhisper )
{
	CUser* pReceiver;
	if( strlen( pWhisper->m_szReceiverID ) > 0 )
	{
		pReceiver = m_UserMap.FindFromID( pWhisper->m_szReceiverID );
	}
	else if( strlen( pWhisper->m_szReceiverNick ) > 0 )
	{
		pReceiver = m_UserMap.FindFromNick( pWhisper->m_szReceiverNick );
	}
	else
	{
		pReceiver = NULL;
	}

	if( pReceiver )
	{
		SendPacket( pReceiver, pWhisper, pWhisper->GetLength() );
	}
}

VOID
CMyGate::OnPacketLoadComplete( CUser* pUser, CMyPacketLoadComplete* pLoadComplete )
{
	CChannel*	pChannel;
	CRoom*		pRoom;
	BOOL		bWaitRoom;
		
	GetUserPlace( pUser, pChannel, pRoom, bWaitRoom );
	if( bWaitRoom || !pRoom ) return;
	MTRACE( "R(%d) U(%s) LoadComplete", pRoom->GetRoomIndex(), pUser->GetID().c_str() );

	CMyUserPocket* pUserPocket = static_cast<CMyUserPocket*>( pUser->GetPocket() );
	CMyRoomPocket* pRoomPocket = static_cast<CMyRoomPocket*>( pRoom->GetPocket() );
	if( !pRoomPocket ) return;
	if( ROOM_STEP_GAME != pRoomPocket->GetStep() && 
		ROOM_STEP_NEXT_ROUND != pRoomPocket->GetStep() ) return;
	
	pUserPocket->RemoveState( PLAYER_STATE_LOADING );
	pUserPocket->AddState( PLAYER_STATE_GAMING );

	m_pPackets->RoundScore.m_nWinRound_A	= pRoomPocket->GetWinRound_A();
	m_pPackets->RoundScore.m_nWinRound_B	= pRoomPocket->GetWinRound_B();
	SendPacket( pUser, &m_pPackets->RoundScore, m_pPackets->RoundScore.GetLength() );

	MTRACE( "R(%d) U(%s) LoadComplete - Success", pRoom->GetRoomIndex(), pUser->GetID().c_str() );
}

VOID
CMyGate::OnPacketStartNextRound( CUser* pUser, CMyPacketStartNextRound* pStartNextRound )
{
	CChannel*	pChannel;
	CRoom*		pRoom;
	BOOL		bWaitRoom;

	GetUserPlace( pUser, pChannel, pRoom, bWaitRoom );
	if( bWaitRoom || !pRoom ) return;

	CMyUserPocket* pUserPocket = static_cast<CMyUserPocket*>( pUser->GetPocket() );
	CMyRoomPocket* pRoomPocket = static_cast<CMyRoomPocket*>( pRoom->GetPocket() );

	if( pUserPocket->IsP2PHost() && pUserPocket->IsGaming() ) 
	{
		MTRACE( "R(%d) U(%s) StartNextRound", pRoom->GetRoomIndex(), pUser->GetID().c_str() );
		pRoomPocket->SetStep( ROOM_STEP_GAME );
	}
}

VOID
CMyGate::OnJoinReply( CUser* pUser, CRoom* pRoom )
{
	MTRACE( "R(%d) U(%s) JoinRoom", pRoom->GetRoomIndex(), pUser->GetID().c_str() );
	CMyUserPocket* pUserPocket;
	CMyRoomPocket* pRoomPocket;
	pUserPocket = static_cast<CMyUserPocket*>( pUser->GetPocket() );
	pRoomPocket = static_cast<CMyRoomPocket*>( pRoom->GetPocket() );
	if( !pUserPocket || !pRoomPocket ) return;

	pUserPocket->InitGameRoomInfo();
		
	short	nTeam;

	if( pRoomPocket->GetPlayerCount_A() > pRoomPocket->GetPlayerCount_B() )
	{
		pRoomPocket->AddPlayer_B( pUserPocket );
		nTeam	= TEAM_B;
	}
	else
	{
		pRoomPocket->AddPlayer_A( pUserPocket );
		nTeam	= TEAM_A;
	}
	
	CNJeanGate::OnJoinReply( pUser, pRoom );
	this->SendJoinUserToRoomUser( pUser, pRoom, pRoomPocket, nTeam );

	MTRACE( "R(%d) U(%s) JoinRoom - Success", pRoom->GetRoomIndex(), pUser->GetID().c_str() );
}

VOID
CMyGate::OnAutoJoinReply( CUser* pUser, CRoom* pRoom )
{
	MTRACE( "R(%d) U(%s) QuickJoinRoom", pRoom->GetRoomIndex(), pUser->GetID().c_str() );
	CMyUserPocket* pUserPocket;
	CMyRoomPocket* pRoomPocket;
	pUserPocket = static_cast<CMyUserPocket*>( pUser->GetPocket() );
	pRoomPocket = static_cast<CMyRoomPocket*>( pRoom->GetPocket() );
	if( !pUserPocket || !pRoomPocket ) return;

	pUserPocket->InitGameRoomInfo();

	short	nTeam;

	if( pRoomPocket->GetPlayerCount_A() > pRoomPocket->GetPlayerCount_B() )
	{
		pRoomPocket->AddPlayer_B( pUserPocket );
		nTeam	= TEAM_B;
	}
	else
	{
		pRoomPocket->AddPlayer_A( pUserPocket );
		nTeam	= TEAM_A;
	}

	CNJeanGate::OnAutoJoinReply( pUser, pRoom );	
	this->SendJoinUserToRoomUser( pUser, pRoom, pRoomPocket, nTeam );

	MTRACE( "R(%d) U(%s) QuickJoinRoom - Success", pRoom->GetRoomIndex(), pUser->GetID().c_str() );
}

VOID CMyGate::OnPacketTimeBombState( CUser* pUser, CMyPacketTimeBombState* pTimeBombState )
{
	CChannel*	pChannel;
	CRoom*		pRoom;
	BOOL		bWaitRoom;

	GetUserPlace( pUser, pChannel, pRoom, bWaitRoom );
	if( bWaitRoom || !pRoom ) return;
	
	CMyUserPocket* pUserPocket = static_cast<CMyUserPocket*>( pUser->GetPocket() );
	CMyRoomPocket* pRoomPocket = static_cast<CMyRoomPocket*>( pRoom->GetPocket() );
	if( !pRoomPocket ||
		ROOM_STEP_GAME != pRoomPocket->GetStep() ||
		GAME_TYPE_TSV != pRoomPocket->GetRoomOption()->nGameType ) return;

	switch( pTimeBombState->m_nState )
	{
	case 0:	//MI_BOMB_PLANTED
		pRoomPocket->SetTimeBombState( pTimeBombState->m_nState );
		break;
	case 1: //MI_BOMB_DEFUSED
		if( pRoomPocket->GetTimeBombState() > 0 ) return;
		pRoomPocket->SetTimeBombState( pTimeBombState->m_nState );
		CheckWinTeam_Timebomb( pRoom, pRoomPocket );
		NextRound( pRoom, pRoomPocket );
		break;
	case 2: //MI_BOMB_DETONATED
		if( pRoomPocket->GetTimeBombState() > 0 ) return;
		pRoomPocket->SetTimeBombState( pTimeBombState->m_nState );
		CheckWinTeam_Timebomb( pRoom, pRoomPocket );
		NextRound( pRoom, pRoomPocket );
		break;
	}
}

BOOL
CMyGate::IsEnableIP( LPCSTR lpszIP )
{
	WSADATA WSAData;
	if( WSAStartup( MAKEWORD(1,0), &WSAData ) == 0 ) 
	{
		TCHAR strLocalHostName[MAX_PATH];
		gethostname( strLocalHostName, MAX_PATH );
		HOSTENT* pHostEnt = gethostbyname( strLocalHostName );
		if( pHostEnt )
		{
			int i=0;
			while( true )
			{
				in_addr* pInAddr = (in_addr*) pHostEnt->h_addr_list[i++];
				if( pInAddr == NULL ) break;
				LPSTR szLocalIP = inet_ntoa( *pInAddr );
				if( stricmp( lpszIP, szLocalIP ) == 0 ) return TRUE;
			}
		}

		WSACleanup();
	}

	return FALSE;
}

VOID
CMyGate::SendJoinUserToRoomUser( CUser* pUser, CRoom* pRoom, CMyRoomPocket* pRoomPocket, short nTeam )
{
	CMyUserPocket*	pUserPocket	= static_cast<CMyUserPocket*>( pUser->GetPocket() );
	T_USER_INFOptr	pUserInfo	= pUserPocket->GetUserInfo();

	//S -> CR
	memset( m_pPackets->RoomUserProperty.m_szID, 0, MAX_ID_LEN+1 );
	strcpy( m_pPackets->RoomUserProperty.m_szID, pUser->GetID().c_str() );
	memset( m_pPackets->RoomUserProperty.m_szNick, 0, MAX_NICK_LEN+1 );
	strcpy( m_pPackets->RoomUserProperty.m_szNick, pUser->GetNick().c_str() );	
	m_pPackets->RoomUserProperty.m_nState	= PLAYER_STATE_WAITING;
	m_pPackets->RoomUserProperty.m_nGrade	= pUserInfo->nGrade;
	m_pPackets->RoomUserProperty.m_nTeam	= nTeam;

	SendToRoom( pRoom, pUser, &m_pPackets->RoomUserProperty, m_pPackets->RoomUserProperty.GetLength() );
}

VOID
CMyGate::SendRoomUserToJoinUser( CUser* pUser, CRoom* pRoom, CMyRoomPocket* pRoomPocket )
{
	//CR -> C
	int	i;
	CMyUserPocket* pUserPocket;

	for( i=0 ; i<pRoomPocket->GetPlayerCount_A() ; ++i )
	{
		pUserPocket	= pRoomPocket->GetPlayer_A(i);

		if( pUserPocket )
		{
			memset( m_pPackets->RoomUserProperty.m_szID, 0, CCasualCommon::CasualStringLength::UserID+1 );
			strcpy( m_pPackets->RoomUserProperty.m_szID, pUserPocket->GetUserInfo()->szID );
			memset( m_pPackets->RoomUserProperty.m_szNick, 0, CCasualCommon::CasualStringLength::UserNick+1 );
			strcpy( m_pPackets->RoomUserProperty.m_szNick, pUserPocket->GetUserInfo()->szNick );	

			m_pPackets->RoomUserProperty.m_nState	= pUserPocket->GetUserInfo()->nState;
			m_pPackets->RoomUserProperty.m_nGrade	= pUserPocket->GetUserInfo()->nGrade;
			m_pPackets->RoomUserProperty.m_nTeam	= TEAM_A;

			SendPacket( pUser, &m_pPackets->RoomUserProperty, m_pPackets->RoomUserProperty.GetLength() );
		}
	}

	for( i=0 ; i<pRoomPocket->GetPlayerCount_B() ; ++i )
	{
		pUserPocket	= pRoomPocket->GetPlayer_B(i);

		if( pUserPocket )
		{
			memset( m_pPackets->RoomUserProperty.m_szID, 0, CCasualCommon::CasualStringLength::UserID+1 );
			strcpy( m_pPackets->RoomUserProperty.m_szID, pUserPocket->GetUserInfo()->szID );
			memset( m_pPackets->RoomUserProperty.m_szNick, 0, CCasualCommon::CasualStringLength::UserNick+1 );
			strcpy( m_pPackets->RoomUserProperty.m_szNick, pUserPocket->GetUserInfo()->szNick );	

			m_pPackets->RoomUserProperty.m_nState		= pUserPocket->GetUserInfo()->nState;
				m_pPackets->RoomUserProperty.m_nGrade	= pUserPocket->GetUserInfo()->nGrade;
			m_pPackets->RoomUserProperty.m_nTeam		= TEAM_B;

			SendPacket( pUser, &m_pPackets->RoomUserProperty, m_pPackets->RoomUserProperty.GetLength() );
		}
	}
}

BOOL
CMyGate::CheckStart( CUser* pCaptain, CRoom* pRoom, CMyRoomPocket* pRoomPocket )
{
	if( pRoomPocket->IsMustReadyToStart() && FALSE == pRoomPocket->IsReadyAll() )
	{
		m_pPackets->FailStartGame.m_nResult	= FAIL_READY;
		SendPacket( pCaptain, &m_pPackets->FailStartGame, m_pPackets->FailStartGame.GetLength() );
		return FALSE;
	}

	return TRUE;
}

BOOL
CMyGate::CheckWinTeam_TimeOut( CRoom* pRoom, CMyRoomPocket* pRoomPocket )
{
	switch( pRoomPocket->GetRoomOption()->nGameType )
	{
	case GAME_TYPE_TDM:
		return CheckWinTeamTDM_TimeOut( pRoom, pRoomPocket );
	case GAME_TYPE_TSV:
		return CheckWinTeamTSV_TimeOut( pRoom, pRoomPocket );
	}

	return FALSE;
}

BOOL
CMyGate::CheckWinTeamTDM_TimeOut( CRoom* pRoom, CMyRoomPocket* pRoomPocket )
{
	//많이 죽인 팀이 승리
	if( pRoomPocket->GetTotalKillCount_A() > pRoomPocket->GetTotalKillCount_B() )
	{
		pRoomPocket->SetWinRoundTeam( TEAM_A );
	}
	else if( pRoomPocket->GetTotalKillCount_A() < pRoomPocket->GetTotalKillCount_B() )
	{
		pRoomPocket->SetWinRoundTeam( TEAM_B );
	}
	else
	{
		pRoomPocket->SetWinRoundTeam( TEAM_NONE );
	}
	return TRUE;
}

BOOL
CMyGate::CheckWinTeamTSV_TimeOut( CRoom* pRoom, CMyRoomPocket* pRoomPocket )
{
	pRoomPocket->SetWinRoundTeam( TEAM_DEFENSE ); //방어하는 팀이 승리 
	return TRUE;
}

BOOL
CMyGate::CheckWinTeam_Timebomb( CRoom* pRoom, CMyRoomPocket* pRoomPocket )
{
	switch( pRoomPocket->GetRoomOption()->nGameType )
	{
	case GAME_TYPE_TDM:
		return FALSE;
	case GAME_TYPE_TSV:
		return CheckWinTeamTSV_TimeBomb( pRoom, pRoomPocket );
	}

	return FALSE;
}

BOOL
CMyGate::CheckWinTeamTSV_TimeBomb( CRoom* pRoom, CMyRoomPocket* pRoomPocket )
{
	switch( pRoomPocket->GetTimeBombState() )
	{
	case 1:	//해체
		pRoomPocket->SetWinRoundTeam( TEAM_DEFENSE );
        return TRUE;
	case 2: //폭발
		pRoomPocket->SetWinRoundTeam( TEAM_ATTACK );
		return TRUE;
	}

	pRoomPocket->SetWinRoundTeam( TEAM_NONE );
	return TRUE;
}

BOOL
CMyGate::CheckEndRound_Kill( CRoom* pRoom, CMyRoomPocket* pRoomPocket )
{
	switch( pRoomPocket->GetRoomOption()->nGameType )
	{
	case GAME_TYPE_TDM:
		return CheckEndRoundTDM_Kill( pRoom, pRoomPocket );
	case GAME_TYPE_TSV:
		return CheckEndRoundTSV_Kill( pRoom, pRoomPocket );
	}

	return FALSE;
}

BOOL
CMyGate::CheckEndRoundTDM_Kill( CRoom* pRoom, CMyRoomPocket* pRoomPocket )
{
	if( pRoomPocket->GetRoomOption()->nKillLimit > 0 )
	{
		if( pRoomPocket->GetRoomOption()->nKillLimit <= pRoomPocket->GetTotalKillCount_A() )
		{
			//A Team Win
			pRoomPocket->SetWinRoundTeam( TEAM_A );
			return TRUE;
		}
		else if( pRoomPocket->GetRoomOption()->nKillLimit <= pRoomPocket->GetTotalKillCount_B() )
		{
			//B Team Win
			pRoomPocket->SetWinRoundTeam( TEAM_B );
			return TRUE;
		}
	}

	if( pRoomPocket->GetPlayerCount_A_InGame() <= 0 )
	{
		pRoomPocket->SetWinRoundTeam( TEAM_B );
		return TRUE;
	}
	else if( pRoomPocket->GetPlayerCount_B_InGame() <= 0 )
	{
		pRoomPocket->SetWinRoundTeam( TEAM_A );
		return TRUE;
	}

	pRoomPocket->SetWinRoundTeam( TEAM_NONE );
	return FALSE;
}

BOOL
CMyGate::CheckEndRoundTSV_Kill( CRoom* pRoom, CMyRoomPocket* pRoomPocket )
{
	int i;
	int	nDeadCount;
	CMyUserPocket* pUserPocket;

	if( pRoomPocket->GetTimeBombState() < 0 )	//폭탄 설치를 못한경우
	{
		nDeadCount = 0;
		for( i=0 ; i<pRoomPocket->GetPlayerCount_A() ; ++i )
		{
			pUserPocket = pRoomPocket->GetPlayer_A(i);
			if( pUserPocket->IsGaming() &&
				FALSE == pUserPocket->IsSpectator() &&
				pUserPocket->GetRoundDead() > 0 ) ++nDeadCount;
		}
		if( pRoomPocket->GetPlayerCount_A_InGame() <= nDeadCount )
		{
			//B Team Win
			pRoomPocket->SetWinRoundTeam( TEAM_B );
			return TRUE;
		}

		nDeadCount = 0;
		for( i=0 ; i<pRoomPocket->GetPlayerCount_B() ; ++i )
		{
			pUserPocket = pRoomPocket->GetPlayer_B(i);
			if( pUserPocket->IsGaming() &&
				FALSE == pUserPocket->IsSpectator() &&
				pUserPocket->GetRoundDead() > 0 ) ++nDeadCount;
		}
		if( pRoomPocket->GetPlayerCount_B_InGame() <= nDeadCount )
		{
			//A Team Win
			pRoomPocket->SetWinRoundTeam( TEAM_A );
			return TRUE;
		}

		if( pRoomPocket->GetPlayerCount_A_InGame() <= 0 )
		{
			pRoomPocket->SetWinRoundTeam( TEAM_B );
			return TRUE;
		}
		else if( pRoomPocket->GetPlayerCount_B_InGame() <= 0 )
		{
			pRoomPocket->SetWinRoundTeam( TEAM_A );
			return TRUE;
		}
	}
	else	//폭탄 설치는 됬으나 해체,폭발하지 않은경우
	{
		if( TEAM_ATTACK == TEAM_A )//A팀이 폭탄설치
		{
			nDeadCount = 0;
			for( i=0 ; i<pRoomPocket->GetPlayerCount_B() ; ++i )
			{
				pUserPocket = pRoomPocket->GetPlayer_B(i);
				if( pUserPocket->IsGaming() &&
					FALSE == pUserPocket->IsSpectator() &&
					pUserPocket->GetRoundDead() > 0 ) ++nDeadCount;
			}
			if( pRoomPocket->GetPlayerCount_B_InGame() <= nDeadCount )
			{
				//A Team Win
				pRoomPocket->SetWinRoundTeam( TEAM_A );
				return TRUE;
			}
		}
		else	//B팀이 폭탄설치
		{
			nDeadCount = 0;
			for( i=0 ; i<pRoomPocket->GetPlayerCount_A() ; ++i )
			{
				pUserPocket = pRoomPocket->GetPlayer_A(i);
				if( pUserPocket->IsGaming() &&
					FALSE == pUserPocket->IsSpectator() &&
					pUserPocket->GetRoundDead() > 0 ) ++nDeadCount;
			}
			if( pRoomPocket->GetPlayerCount_A_InGame() <= nDeadCount )
			{
				//B Team Win
				pRoomPocket->SetWinRoundTeam( TEAM_B );
				return TRUE;
			}
		}

		if( pRoomPocket->GetPlayerCount_A_InGame() <= 0 )
		{
			pRoomPocket->SetWinRoundTeam( TEAM_B );
			return TRUE;
		}
		else if( pRoomPocket->GetPlayerCount_B_InGame() <= 0 )
		{
			pRoomPocket->SetWinRoundTeam( TEAM_A );
			return TRUE;
		}
	}

	pRoomPocket->SetWinRoundTeam( TEAM_NONE );
	return FALSE;
}

VOID
CMyGate::SetTeamBalanceStartGame( CRoom* pRoom, CMyRoomPocket* pRoomPocket )
{
	CMyUserPocket*	pUserPocket;

	if( !pRoom || !pRoomPocket ||
		FALSE == pRoomPocket->GetRoomOption()->bTeamBalance ) return;

	pRoomPocket->SetTeamBalance();
	
	int i;
	int nCount = pRoomPocket->GetPlayerCount();

	for( i=0 ; i<nCount ; ++i )
	{
		pUserPocket = pRoomPocket->GetPlayer(i);
		strcpy( m_pPackets->ChangeTeam.m_szID, pUserPocket->GetUserInfo()->szID );
		m_pPackets->ChangeTeam.m_nTeam	= pUserPocket->GetUserTeam();
		m_pPackets->ChangeTeam.m_nType	= TEAM_CHANGE_START;

		SendToRoom( pRoom, NULL, &m_pPackets->ChangeTeam, m_pPackets->ChangeTeam.GetLength() );
	}
}

void
CMyGate::SetTeamBalanceInstrution( CRoom* pRoom, CMyRoomPocket* pRoomPocket, CMyUserPocket* pUserPocket )
{
	if( !pRoom || !pRoomPocket || !pUserPocket ||
		FALSE == pRoomPocket->GetRoomOption()->bTeamBalance ) return;

	int nBeforeTeam	= pUserPocket->GetUserTeam();
	int	nDelta	= pRoomPocket->GetPlayerCount_A_Ready() - pRoomPocket->GetPlayerCount_B_Ready();

	//스팩테이터로 입장할 경우 팀벨런스를 맞추지 않는다.
	//if( pUserPocket->IsSpectator() ) return;

	if( nDelta < 0 )
	{
		if( nBeforeTeam != TEAM_A )
		{
			if( TEAM_A == pRoomPocket->ChangeTeam( pUserPocket->GetUserInfo()->szID ) )
			{
				strcpy( m_pPackets->ChangeTeam.m_szID, pUserPocket->GetUserInfo()->szID );
				m_pPackets->ChangeTeam.m_nTeam = TEAM_A;
				m_pPackets->ChangeTeam.m_nType	= TEAM_CHANGE_INSTRUSION;
				SendToRoom( pRoom, NULL, &m_pPackets->ChangeTeam, m_pPackets->ChangeTeam.GetLength() );				
			}			
		}
	}
	else if( nDelta > 0 )
	{
		if( nBeforeTeam != TEAM_B )
		{
			if( TEAM_B == pRoomPocket->ChangeTeam( pUserPocket->GetUserInfo()->szID ) )
			{
				strcpy( m_pPackets->ChangeTeam.m_szID, pUserPocket->GetUserInfo()->szID );
				m_pPackets->ChangeTeam.m_nTeam = TEAM_B;
				m_pPackets->ChangeTeam.m_nType	= TEAM_CHANGE_INSTRUSION;
				SendToRoom( pRoom, NULL, &m_pPackets->ChangeTeam, m_pPackets->ChangeTeam.GetLength() );	
			}			
		}
	}
}

//클라이언트는 이 패킷을 받아야만 리스폰이 가능해짐
//팀이 바뀌든 안 바뀌든 패킷을 보낸다.
void
CMyGate::SetTeamBalanceAfterDeath( CRoom* pRoom, CMyRoomPocket* pRoomPocket, CMyUserPocket* pUserPocket )
{
	if( !pRoom || !pRoomPocket || !pUserPocket ) return;

	int nBeforeTeam	= pUserPocket->GetUserTeam();
	int	nDelta	= pRoomPocket->GetPlayerCount_A_Ready() - pRoomPocket->GetPlayerCount_B_Ready();

	strcpy( m_pPackets->ChangeTeam.m_szID, pUserPocket->GetUserInfo()->szID );
	m_pPackets->ChangeTeam.m_nTeam	= nBeforeTeam;
	m_pPackets->ChangeTeam.m_nType	= TEAM_CHANGE_AFTER_DEATH;

	if( FALSE == pRoomPocket->GetRoomOption()->bTeamBalance )
	{
		SendToRoom( pRoom, NULL, &m_pPackets->ChangeTeam, m_pPackets->ChangeTeam.GetLength() );
		return;
	}

	if( (TEAM_BALANCE_MIN_LIMIT<=nDelta) && (nDelta<=TEAM_BALANCE_MAX_LIMIT) )
	{	
	}
	else if( nDelta < 0 )
	{
		if( nBeforeTeam != TEAM_A )
		{
			if( TEAM_A == pRoomPocket->ChangeTeam( pUserPocket->GetUserInfo()->szID ) )
			{
				m_pPackets->ChangeTeam.m_nTeam = TEAM_A;
			}			
		}
	}
	else
	{
		if( nBeforeTeam != TEAM_B )
		{
			if( TEAM_B == pRoomPocket->ChangeTeam( pUserPocket->GetUserInfo()->szID ) )
			{
				m_pPackets->ChangeTeam.m_nTeam = TEAM_B;
			}			
		}
	}

	SendToRoom( pRoom, NULL, &m_pPackets->ChangeTeam, m_pPackets->ChangeTeam.GetLength() );
}

BYTE CMyGate::GetLimitTime( BYTE nGameType, BYTE nMapIndex, BYTE nRecvValue )
{
	switch( nGameType )
	{
	case GAME_TYPE_TDM:
		return nRecvValue;
	case GAME_TYPE_TSV:
		switch( nMapIndex )
		{
		case 0:	//Ware House
			return 2;
		case 1:	//Submarine
			return 3;
		}
		break;
	}

	return 5;
}

void CMyGate::StopLoading( CRoom* pRoom, int nResult )
{
	CUser* pRoomUser;
	CMyUserPocket* pUserPocket;

	m_pPackets->FailStartGame.m_nResult = nResult;

	pRoomUser	= pRoom->GetFirstUser();
	while( pRoomUser )
	{
		pUserPocket	= static_cast<CMyUserPocket*>( pRoomUser->GetPocket() );
		if( pUserPocket && pUserPocket->IsReady() )
		{
			SendPacket( pRoomUser, &m_pPackets->FailStartGame, m_pPackets->FailStartGame.GetLength() );
		}
		
		pRoomUser	= pRoom->GetNextUser();
	}
}

VOID CMyGate::RequestLogin( CUser* pUser, LPCTSTR lpszUserNo, LPCTSTR lpszIP, LPCTSTR lpszFlag )
{
#ifdef __TEST__
	if( strcmp( pUser->GetUserNo().c_str(), __TEST_USERNO__ ) == 0 )
	{
		return;
	}
#endif

	Manage::CDbObject* pDB = AllocDbObject();
	_CommandPtr& pCmd = pDB->GetCommand();

	pCmd->CommandText	= "SP_GS_MB_LOGIN";
	pCmd->CommandType	= adCmdStoredProc;

	CComVariant sUserNo( lpszUserNo );
	CComVariant sFlag( lpszFlag );
	CComVariant sAddr( lpszIP );

	int	nResult = 0;
	_ParameterPtr pPrm1	= pCmd->CreateParameter( "@i_user_no", adVarChar, adParamInput, 20, sUserNo );
	_ParameterPtr pPrm2	= pCmd->CreateParameter( "@i_login_flag", adChar, adParamInput, sizeof(char), sFlag );
	_ParameterPtr pPrm3	= pCmd->CreateParameter( "@i_ipt_ip_addr", adVarChar, adParamInput, 15, sAddr );
	_ParameterPtr pPrm4	= pCmd->CreateParameter( "@o_sp_rtn", adInteger, adParamOutput, sizeof(int), nResult );
	pCmd->Parameters->Append( pPrm1 );
	pCmd->Parameters->Append( pPrm2 );
	pCmd->Parameters->Append( pPrm3 );
	pCmd->Parameters->Append( pPrm4 );

	RequestADO( pUser, CMyCommon::MyQueryMessage::LoginQuery, pDB );
}

VOID CMyGate::RequestLogout( CUser* pUser, LPCTSTR lpszUserNo, LPCTSTR lpszIP, LPCTSTR lpszFlag )
{
#ifdef __TEST__
	if( strcmp( pUser->GetUserNo().c_str(), __TEST_USERNO__ ) == 0 )
	{
		return;
	}
#endif

	Manage::CDbObject* pDB = AllocDbObject();
	_CommandPtr& pCmd = pDB->GetCommand();

	pCmd->CommandText	= "SP_GS_MB_LOGOUT";
	pCmd->CommandType	= adCmdStoredProc;

	CComVariant sUserNo( lpszUserNo );
	CComVariant sFlag( lpszFlag );
	CComVariant sAddr( lpszIP );

	int	nResult = 0;
	_ParameterPtr pPrm1	= pCmd->CreateParameter( "@i_user_no", adVarChar, adParamInput, 20, sUserNo );
	_ParameterPtr pPrm2	= pCmd->CreateParameter( "@i_login_flag", adChar, adParamInput, sizeof(char), sFlag );
	_ParameterPtr pPrm3	= pCmd->CreateParameter( "@i_ipt_ip_addr", adVarChar, adParamInput, 15, sAddr );
	_ParameterPtr pPrm4	= pCmd->CreateParameter( "@o_sp_rtn", adInteger, adParamOutput, sizeof(int), nResult );
	pCmd->Parameters->Append( pPrm1 );
	pCmd->Parameters->Append( pPrm2 );
	pCmd->Parameters->Append( pPrm3 );
	pCmd->Parameters->Append( pPrm4 );

	RequestADO( NULL, CMyCommon::MyQueryMessage::LogoutQuery, pDB );
}

VOID CMyGate::RequestUserInfo( CUser* pUser, LPCTSTR lpszUserNo )
{
#ifdef __TEST__
	if( strcmp( pUser->GetUserNo().c_str(), __TEST_USERNO__ ) == 0 )
	{
		return;
	}
#endif

	Manage::CDbObject* pDB = AllocDbObject();
	_CommandPtr& pCmd = pDB->GetCommand();

	pCmd->CommandText	= "SP_GM_RESULT_R";
	pCmd->CommandType	= adCmdStoredProc;

	CComVariant sUserNo( lpszUserNo );

	__int64	nExp		= 0;
	__int64	nMoney		= 0;
	int		nKillCnt	= 0;
	int		nDeathCnt	= 0;
	int		nH_KillCnt	= 0;
	int		nH_DeathCnt	= 0;
	int		nWinCnt		= 0;
	int		nLoseCnt	= 0;
	int		nDrawCnt	= 0;
	int		nResult		= 0;

	_ParameterPtr pPrm1		= pCmd->CreateParameter( "@i_user_no", adVarChar, adParamInput, 20, sUserNo );
	_ParameterPtr pPrm2		= pCmd->CreateParameter( "@o_user_exp", adBigInt, adParamOutput, sizeof(__int64), nExp );
	_ParameterPtr pPrm3		= pCmd->CreateParameter( "@o_user_money", adBigInt, adParamOutput, sizeof(__int64), nMoney );
	_ParameterPtr pPrm4		= pCmd->CreateParameter( "@o_kill_cnt", adInteger, adParamOutput, sizeof(int), nKillCnt );
	_ParameterPtr pPrm5		= pCmd->CreateParameter( "@o_death_cnt", adInteger, adParamOutput, sizeof(int), nDeathCnt );
	_ParameterPtr pPrm6		= pCmd->CreateParameter( "@o_h_kill_cnt", adInteger, adParamOutput, sizeof(int), nH_KillCnt );
	_ParameterPtr pPrm7		= pCmd->CreateParameter( "@o_h_death_cnt", adInteger, adParamOutput, sizeof(int), nH_DeathCnt );
	_ParameterPtr pPrm8		= pCmd->CreateParameter( "@o_win_cnt", adInteger, adParamOutput, sizeof(int), nWinCnt );
	_ParameterPtr pPrm9		= pCmd->CreateParameter( "@o_lose_cnt", adInteger, adParamOutput, sizeof(int), nLoseCnt );
	_ParameterPtr pPrm10	= pCmd->CreateParameter( "@o_draw_cnt", adInteger, adParamOutput, sizeof(int), nDrawCnt );
	_ParameterPtr pPrm11	= pCmd->CreateParameter( "@o_sp_rtn", adInteger, adParamOutput, sizeof(int), nResult );

	pCmd->Parameters->Append( pPrm1 );
	pCmd->Parameters->Append( pPrm2 );
	pCmd->Parameters->Append( pPrm3 );
	pCmd->Parameters->Append( pPrm4 );
	pCmd->Parameters->Append( pPrm5 );
	pCmd->Parameters->Append( pPrm6 );
	pCmd->Parameters->Append( pPrm7 );
	pCmd->Parameters->Append( pPrm8 );
	pCmd->Parameters->Append( pPrm9 );
	pCmd->Parameters->Append( pPrm10 );
	pCmd->Parameters->Append( pPrm11 );

	RequestADO( pUser, CMyCommon::MyQueryMessage::UserInfoQuery, pDB );
}

VOID CMyGate::RequestGameResult( CUser* pUser,
								 LPCTSTR lpszUserNo, __int64 nExp, __int64 nMoney,
								 int nKillCnt, int nDeathCnt, 
								 int nH_KillCnt, int nH_DeathCnt,
								 int nWinCnt, int nLoseCnt, int nDrawCnt )
{
#ifdef __TEST__
	if( strcmp( pUser->GetUserNo().c_str(), __TEST_USERNO__ ) == 0 )
	{
		return;
	}
#endif

	Manage::CDbObject* pDB = AllocDbObject();
	_CommandPtr& pCmd = pDB->GetCommand();

	pCmd->CommandText	= "SP_GS_GM_RESULT_U";
	pCmd->CommandType	= adCmdStoredProc;

	CComVariant	sUserNo( lpszUserNo );
	int			nResult		= 0;

	_ParameterPtr pPrm1		= pCmd->CreateParameter( "@i_user_no", adVarChar, adParamInput, 20, sUserNo );
	_ParameterPtr pPrm2		= pCmd->CreateParameter( "@i_user_exp", adBigInt, adParamInput, sizeof(__int64), nExp );
	_ParameterPtr pPrm3		= pCmd->CreateParameter( "@i_user_money", adBigInt, adParamInput, sizeof(__int64), nMoney );
	_ParameterPtr pPrm4		= pCmd->CreateParameter( "@i_kill_cnt", adInteger, adParamInput, sizeof(int), nKillCnt );
	_ParameterPtr pPrm5		= pCmd->CreateParameter( "@i_death_cnt", adInteger, adParamInput, sizeof(int), nDeathCnt );
	_ParameterPtr pPrm6		= pCmd->CreateParameter( "@i_h_kill_cnt", adInteger, adParamInput, sizeof(int), nH_KillCnt );
	_ParameterPtr pPrm7		= pCmd->CreateParameter( "@i_h_death_cnt", adInteger, adParamInput, sizeof(int), nH_DeathCnt );
	_ParameterPtr pPrm8		= pCmd->CreateParameter( "@i_win_cnt", adInteger, adParamInput, sizeof(int), nWinCnt );
	_ParameterPtr pPrm9		= pCmd->CreateParameter( "@i_lose_cnt", adInteger, adParamInput, sizeof(int), nLoseCnt );
	_ParameterPtr pPrm10	= pCmd->CreateParameter( "@i_draw_cnt", adInteger, adParamInput, sizeof(int), nDrawCnt );
	_ParameterPtr pPrm11	= pCmd->CreateParameter( "@o_sp_rtn", adInteger, adParamOutput, sizeof(int), nResult );

	pCmd->Parameters->Append( pPrm1 );
	pCmd->Parameters->Append( pPrm2 );
	pCmd->Parameters->Append( pPrm3 );
	pCmd->Parameters->Append( pPrm4 );
	pCmd->Parameters->Append( pPrm5 );
	pCmd->Parameters->Append( pPrm6 );
	pCmd->Parameters->Append( pPrm7 );
	pCmd->Parameters->Append( pPrm8 );
	pCmd->Parameters->Append( pPrm9 );
	pCmd->Parameters->Append( pPrm10 );
	pCmd->Parameters->Append( pPrm11 );

	RequestADO( pUser, CMyCommon::MyQueryMessage::GameResultQuery, pDB );
}

UINT CMyGate::GetGameExp( CMyUserPocket* pUserPocket, CMyRoomPocket* pRoomPocket, BOOL bWin )
{
	switch( pRoomPocket->GetRoomOption()->nGameType )
	{
	case GAME_TYPE_TDM:
		return GetGameExpTDM( pUserPocket, pRoomPocket, bWin );
	case GAME_TYPE_TSV:
		return GetGameExpTSV( pUserPocket, pRoomPocket, bWin );
	}

	return 0;
}

UINT CMyGate::GetGameExpTDM( CMyUserPocket* pUserPocket, CMyRoomPocket* pRoomPocket, BOOL bWin )
{
	UINT	nExp;
	T_USER_GAME_INFOptr	pGameInfo = pUserPocket->GetUserGameInfo();

	if( bWin )
	{
		nExp = SCORE_DM_BASE											// 기본점수
			+ ((timeGetTime()-pUserPocket->GetStartTick())/6000)		// + 참여한시간*10
			+ ((pGameInfo->nHighLevelKill+pGameInfo->nLowLevelKill)*3)	// + 킬수 * 3
			+ pGameInfo->nHeadShotKill									// + 헤드샷
			- (pGameInfo->nHighLevelDead+pGameInfo->nLowLevelDead);		// - 죽은수

		switch( pUserPocket->GetUserTeam() )							// + 상대방쪽수*20
		{
		case TEAM_A:
			nExp += pRoomPocket->GetPlayerCount_B_InGame()*20;
			break;
		case TEAM_B:
			nExp += pRoomPocket->GetPlayerCount_A_InGame()*20;
			break;
		default:
			return 0;
		}
	}
	else
	{
		nExp = SCORE_DM_BASE											// 기본점수
			+ ((timeGetTime()-pUserPocket->GetStartTick())/30000)		// + 참여한시간*5
			+ ((pGameInfo->nHighLevelKill+pGameInfo->nLowLevelKill)*3)	// + 킬수 * 3
			+ pGameInfo->nHeadShotKill									// + 헤드샷
			- (pGameInfo->nHighLevelDead+pGameInfo->nLowLevelDead);		// - 죽은수

		switch( pUserPocket->GetUserTeam() )							// + 상대방쪽수*20
		{
		case TEAM_A:
			nExp += pRoomPocket->GetPlayerCount_B_InGame()*20;
			break;
		case TEAM_B:
			nExp += pRoomPocket->GetPlayerCount_A_InGame()*20;
			break;
		default:
			return 0;
		}
	}

	return nExp;
}

UINT CMyGate::GetGameExpTSV( CMyUserPocket* pUserPocket, CMyRoomPocket* pRoomPocket, BOOL bWin )
{
	UINT	nExp;
	T_USER_GAME_INFOptr	pGameInfo = pUserPocket->GetUserGameInfo();

	if( bWin )
	{
		nExp = SCORE_SV_BASE											// 기본점수
			+ 10														// + 10
			+ ((timeGetTime()-pUserPocket->GetStartTick())/6000)		// + 참여한시간*10
			+ ((pGameInfo->nHighLevelKill+pGameInfo->nLowLevelKill)*3)	// + 킬수 * 3
			+ pGameInfo->nHeadShotKill									// + 헤드샷
			- (pGameInfo->nHighLevelDead+pGameInfo->nLowLevelDead);		// - 죽은수

		switch( pUserPocket->GetUserTeam() )							// + 상대방쪽수*5
		{
		case TEAM_A:
			nExp += pRoomPocket->GetPlayerCount_B_InGame()*5;
			break;
		case TEAM_B:
			nExp += pRoomPocket->GetPlayerCount_A_InGame()*5;
			break;
		default:
			return 0;
		}
	}
	else
	{
		nExp = SCORE_SV_BASE											// 기본점수
			+ ((timeGetTime()-pUserPocket->GetStartTick())/30000)		// + 참여한시간*5
			+ ((pGameInfo->nHighLevelKill+pGameInfo->nLowLevelKill)*3)	// + 킬수 * 3
			+ pGameInfo->nHeadShotKill									// + 헤드샷
			- (pGameInfo->nHighLevelDead+pGameInfo->nLowLevelDead);		// - 죽은수

		switch( pUserPocket->GetUserTeam() )							// + 상대방쪽수*5
		{
		case TEAM_A:
			nExp += pRoomPocket->GetPlayerCount_B_InGame()*5;
			break;
		case TEAM_B:
			nExp += pRoomPocket->GetPlayerCount_A_InGame()*5;
			break;
		default:
			return 0;
		}
	}

	return nExp;
}

UINT CMyGate::GetGameMoney( CMyUserPocket* pUserPocket, CMyRoomPocket* pRoomPocket, BOOL bWin )
{
	switch( pRoomPocket->GetRoomOption()->nGameType )
	{
	case GAME_TYPE_TDM:
		return GetGameMoneyTDM( pUserPocket, pRoomPocket, bWin );
	case GAME_TYPE_TSV:
		return GetGameMoneyTSV( pUserPocket, pRoomPocket, bWin );
	}

	return 0;
}

UINT CMyGate::GetGameMoneyTDM( CMyUserPocket* pUserPocket, CMyRoomPocket* pRoomPocket, BOOL bWin )
{
	UINT nNoney;
	T_USER_GAME_INFOptr	pGameInfo = pUserPocket->GetUserGameInfo();

	if( bWin )
	{
		nNoney = 10														// 기본머니
			+ ((timeGetTime()-pUserPocket->GetStartTick())/6000)		// + 참여한시간*10
			+ ((pGameInfo->nHighLevelKill+pGameInfo->nLowLevelKill)*5);	// + 킬수 * 5
	}
	else
	{
		nNoney = 10														// 기본머니
			+ ((timeGetTime()-pUserPocket->GetStartTick())/6000)		// + 참여한시간*10
			+ ((pGameInfo->nHighLevelKill+pGameInfo->nLowLevelKill)*2);	// + 킬수 * 2
	}
	return nNoney;
}

UINT CMyGate::GetGameMoneyTSV( CMyUserPocket* pUserPocket, CMyRoomPocket* pRoomPocket, BOOL bWin )
{
	UINT nNoney;
	T_USER_GAME_INFOptr	pGameInfo = pUserPocket->GetUserGameInfo();

	if( bWin )
	{
		nNoney = 20														// 기본머니
			+ ((timeGetTime()-pUserPocket->GetStartTick())/6000)		// + 참여한시간*10
			+ ((pGameInfo->nHighLevelKill+pGameInfo->nLowLevelKill)*5);	// + 킬수 * 5
	}
	else
	{
		nNoney = 12														// 기본머니
			+ ((timeGetTime()-pUserPocket->GetStartTick())/6000)		// + 참여한시간*10
			+ ((pGameInfo->nHighLevelKill+pGameInfo->nLowLevelKill)*2);	// + 킬수 * 2
	}
	return nNoney;
}