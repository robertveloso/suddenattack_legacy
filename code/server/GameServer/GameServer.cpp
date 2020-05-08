// GameServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "./Construct.h"

#include "./IServer.h"
#include "./IChannel.h"
#include "./INoneResponser.h"
#include "./INoneSignal.h"
#include "./Server/IRank.h"

#include "../Dispatcher/IConsole.h"

void
loadServer();

void
loadChannel();

void
loadFormula();

void
loadSpectator();

void
loadFormula( ::LogicObject::Room::Option::E_MODE::ENUM i_eMode, CSTRING & i_strTitle );

void
loadSpectator( ::LogicObject::Room::Option::E_MODE::ENUM i_eMode, CSTRING & i_strKey );

int
main( int nCount, char * pszArgument[] )
{
	namespace D				= ::Dispatcher;
	namespace G				= ::GameServer;

	cout << "begin" << endl;

	G::Construct			stConstruct;

	loadServer();
	loadChannel();
	loadFormula();
	loadSpectator();

	D::IQuery::IREQUESTER()->SetObserver( G::INONERESPONSER() );

	D::IHandler::Argument		stArg;
#ifdef _DEBUG
	stArg.nClientCount			= 100;
#else
	stArg.nClientCount			= 4096;
#endif
	stArg.nRecvQSize			= 1024;
	stArg.nSendQSize			= 1024 * 100;
	stArg.nQueryQSize			= 1024;
	stArg.nSignalQSize			= 1024;
	stArg.nResponseBufferSize	= 1024;
	stArg.nAcceptPoolSize		= 4;
	stArg.nPortNumber			= G::ISERVER()->GetPort();

	stArg.nDbConnect			= ::atoi( D::ICONSOLE()->Read( STRING( "DB_ConnCount" ) ).GetCSTR() );
	stArg.strDbConnection		= D::ICONSOLE()->Read( STRING( "DB_Conn" ) );

	CBOOL bCreate				= D::IHANDLER()->Initialize( stArg );
	if( bCreate == FALSE )
	{
		printf( "Create Fail!!\n" );
		return 0;
	}

	D::IClient::ISESSION()->Alarm( G::INoneSignal::E_SIGNAL::START, 1, G::INONESIGNAL() );

	D::IHANDLER()->Start( stArg );
	
	return 0;
}

inline void
loadServer()
{
	CSTRING strIP				= ::Dispatcher::ICONSOLE()->Read( STRING( "IP" ) );
	CINT nPort					= atoi( ::Dispatcher::ICONSOLE()->Read( STRING( "Port" ) ).GetCSTR() );

	CBOOL bLoginGamehi			= atoi( ::Dispatcher::ICONSOLE()->Read( STRING( "GamehiLogin" ) ).GetCSTR() );
	CBOOL bLoginNetmarble		= atoi( ::Dispatcher::ICONSOLE()->Read( STRING( "NetmarbleLogin" ) ).GetCSTR() );

	CINT nRankTick				= atoi( ::Dispatcher::ICONSOLE()->Read( STRING( "RankTick" ) ).GetCSTR() );

	GameServer::ISERVER()->Open( strIP, nPort );
	GameServer::ISERVER()->SetLoginGamehi( bLoginGamehi );
	GameServer::ISERVER()->SetLoginNetmarble( bLoginNetmarble );
	GameServer::ISERVER()->GetRank()->SetRankTick( nRankTick );
}

inline void
loadChannel()
{
	using namespace ::GameServer;

	CINT nChannelCount = atoi( ::Dispatcher::ICONSOLE()->Read( STRING( "ChannelCount" ) ).GetCSTR() );
	for( INT i = 0; i < nChannelCount; i++ )
	{
		STRING strKey;
		if( i < 10 )
		{
			strKey.Printf( "ChannelMaxUser0%d", i );
		}
		else
		{
			strKey.Printf( "ChannelMaxUser%d", i );
		}
		CINT nMaxCount = atoi( ::Dispatcher::ICONSOLE()->Read( strKey ).GetCSTR() );

		IChannel * pChannel	= IChannel::IFACTORY()->Create( (BYTE) i, nMaxCount );
		ISERVER()->AddChannel( pChannel );
	}
}

inline void
loadFormula()
{
	using namespace			::GameServer;
	typedef					::LogicObject::Room::Option::E_MODE
							E_MODE;
	typedef					::GameServer::IFormula
							IFORMULA;

	loadFormula( E_MODE::RESPAWN, STRING( "DEATH_MATCH" ) );
	loadFormula( E_MODE::BOMB, STRING( "BOMB" ) );

	CINT nExp				= atoi( ::Dispatcher::ICONSOLE()->ReadTable( STRING( "PENALTY" ), STRING( "EXP" ) ) );
	CINT nMoney				= atoi( ::Dispatcher::ICONSOLE()->ReadTable( STRING( "PENALTY" ), STRING( "MONEY" ) ) );
	IFORMULA::ICONTAINER()->SetPenalty( nExp, nMoney );
}

void
loadFormula( ::LogicObject::Room::Option::E_MODE::ENUM i_eMode, CSTRING & i_strTitle )
{
	typedef					::GameServer::IFormula
							IFORMULA;

	IFORMULA::Exp			stWinExp, stLoseExp;
	IFORMULA::Money			stWinMoney, stLoseMoney;

	stWinExp.nKillMul		= atoi( ::Dispatcher::ICONSOLE()->ReadTable( i_strTitle, STRING( "WIN_KILL" ) ) );
	stWinExp.nHeadKillMul	= atoi( ::Dispatcher::ICONSOLE()->ReadTable( i_strTitle, STRING( "WIN_HEAD_KILL" ) ) );
	stWinExp.nEnemyMul		= atoi( ::Dispatcher::ICONSOLE()->ReadTable( i_strTitle, STRING( "WIN_ENEMY" ) ) );
	stWinExp.nDeadMul		= atoi( ::Dispatcher::ICONSOLE()->ReadTable( i_strTitle, STRING( "WIN_DEAD" ) ) );
	stWinExp.nSuicideMul	= atoi( ::Dispatcher::ICONSOLE()->ReadTable( i_strTitle, STRING( "WIN_SUICIDE" ) ) );
	stWinExp.nMissionMul	= atoi( ::Dispatcher::ICONSOLE()->ReadTable( i_strTitle, STRING( "WIN_MISSION" ) ) );
	stWinExp.nAdd			= atoi( ::Dispatcher::ICONSOLE()->ReadTable( i_strTitle, STRING( "WIN_ADD" ) ) );

	stLoseExp.nKillMul		= atoi( ::Dispatcher::ICONSOLE()->ReadTable( i_strTitle, STRING( "LOSE_KILL" ) ) );
	stLoseExp.nHeadKillMul	= atoi( ::Dispatcher::ICONSOLE()->ReadTable( i_strTitle, STRING( "LOSE_HEAD_KILL" ) ) );
	stLoseExp.nEnemyMul		= atoi( ::Dispatcher::ICONSOLE()->ReadTable( i_strTitle, STRING( "LOSE_ENEMY" ) ) );
	stLoseExp.nDeadMul		= atoi( ::Dispatcher::ICONSOLE()->ReadTable( i_strTitle, STRING( "LOSE_DEAD" ) ) );
	stLoseExp.nSuicideMul	= atoi( ::Dispatcher::ICONSOLE()->ReadTable( i_strTitle, STRING( "LOSE_SUICIDE" ) ) );
	stLoseExp.nMissionMul	= atoi( ::Dispatcher::ICONSOLE()->ReadTable( i_strTitle, STRING( "LOSE_MISSION" ) ) );
	stLoseExp.nAdd			= atoi( ::Dispatcher::ICONSOLE()->ReadTable( i_strTitle, STRING( "LOSE_ADD" ) ) );

	stWinMoney.nKillMul		= atoi( ::Dispatcher::ICONSOLE()->ReadTable( i_strTitle, STRING( "WIN_KILL_M" ) ) );
	stWinMoney.nEnemyMul	= atoi( ::Dispatcher::ICONSOLE()->ReadTable( i_strTitle, STRING( "WIN_ENEMY_M" ) ) );
	stWinMoney.nAdd			= atoi( ::Dispatcher::ICONSOLE()->ReadTable( i_strTitle, STRING( "WIN_ADD_M" ) ) );
	stWinMoney.nDeadMul		= atoi( ::Dispatcher::ICONSOLE()->ReadTable( i_strTitle, STRING( "WIN_DEAD_M" ) ) );
	stWinMoney.nSuicideMul	= atoi( ::Dispatcher::ICONSOLE()->ReadTable( i_strTitle, STRING( "WIN_SUICIDE_M" ) ) );
	stWinMoney.nMissionMul	= atoi( ::Dispatcher::ICONSOLE()->ReadTable( i_strTitle, STRING( "WIN_MISSION_M" ) ) );

	stLoseMoney.nKillMul	= atoi( ::Dispatcher::ICONSOLE()->ReadTable( i_strTitle, STRING( "LOSE_KILL_M" ) ) );
	stLoseMoney.nEnemyMul	= atoi( ::Dispatcher::ICONSOLE()->ReadTable( i_strTitle, STRING( "LOSE_ENEMY_M" ) ) );
	stLoseMoney.nAdd		= atoi( ::Dispatcher::ICONSOLE()->ReadTable( i_strTitle, STRING( "LOSE_ADD_M" ) ) );
	stLoseMoney.nDeadMul	= atoi( ::Dispatcher::ICONSOLE()->ReadTable( i_strTitle, STRING( "LOSE_DEAD_M" ) ) );
	stLoseMoney.nSuicideMul	= atoi( ::Dispatcher::ICONSOLE()->ReadTable( i_strTitle, STRING( "LOSE_SUICIDE_M" ) ) );
	stLoseMoney.nMissionMul	= atoi( ::Dispatcher::ICONSOLE()->ReadTable( i_strTitle, STRING( "LOSE_MISSION_M" ) ) );

	::GameServer::IFormula::ICONTAINER()->Add( i_eMode, stWinExp, stLoseExp, stWinMoney, stLoseMoney );
}

inline void
loadSpectator()
{
	typedef					::LogicObject::Room::Option::E_MODE
							E_MODE;

	loadSpectator( E_MODE::RESPAWN, STRING( "DEATH_MATCH" ) );
	loadSpectator( E_MODE::BOMB, STRING( "BOMB" ) );
}

inline void
loadSpectator( ::LogicObject::Room::Option::E_MODE::ENUM i_eMode, CSTRING & i_strKey )
{
	using namespace			::GameServer;

	CINT nValue				= atoi( ::Dispatcher::ICONSOLE()->ReadTable( STRING( "SPECTATOR" ), i_strKey ) );

	IRoom::IFACTORY()->SetSpectatorTime( i_eMode, nValue );
}