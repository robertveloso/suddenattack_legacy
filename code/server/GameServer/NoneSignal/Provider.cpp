#include "StdAfx.h"

#include "./Provider.h"

#include "../IServer.h"
#include "../IPlayer.h"
#include "../ILog.h"
#include "../Query.h"
#include "../Server/IRank.h"
#include "../../Dispatcher/IQuery.h"
#include "../../Dispatcher/IConsole.h"
#include "../../Dispatcher/ITime.h"

namespace GameServer {		namespace NoneSignal {

Provider::Provider()
{
	this->install();
}

Provider::~Provider()
{
	this->uninstall();
}

Provider::PEER *
Provider::GetPeer()
{
	DEBUG_BREAK;
	return NULL;
}

void
Provider::OnOpen()
{
	DEBUG_BREAK;
}

void
Provider::OnPacket( PACKET * i_pPacket )
{
	DEBUG_BREAK;
}

void
Provider::OnClose()
{
	DEBUG_BREAK;
}

void
Provider::OnSignal( CUINT i_uiMessage )
{
	switch( i_uiMessage )
	{
	case E_SIGNAL::START :
		this->onStart();
		break;

	case E_SIGNAL::CONNECTION_UPDATE :
		this->onConnectionUpdate();
		break;

	case E_SIGNAL::LOG_FILE :
		this->onLogFile();
		break;

	case E_SIGNAL::RANKING:
		this->onRanking();
		break;
	}
}

void
Provider::OnResponse( IQUERY * i_pQuery )
{
	DEBUG_BREAK;
}

inline void
Provider::onStart()
{
	namespace				D = ::Dispatcher;
	typedef					Query::Server
							SERVER;
	try
	{
		INT nInt				= -99;
		IQUERY * pQuery			= IQUERY::IFACTORY()->Create( SERVER::E_CODE::REQ_START );

		pQuery->GetLog().Printf( "%s\t%d\r\n",
					ISERVER()->GetIP().GetCSTR(),
					ISERVER()->GetPort() );

		Dispatcher::ICONSOLE()->Output( pQuery->GetLog() );

		IQUERY::IBUILDER()->Build( pQuery, STRING( "SP_GS_GM_ServerStatus_R" ) );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_ServerIP" ), ISERVER()->GetIP(), TRUE, 15 );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_ServerPort" ), ISERVER()->GetPort(), TRUE );

		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_ServerID" ), STRING( "" ), FALSE, 4 );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_ServerName" ), STRING( "" ), FALSE, 20 );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_sp_rtn" ), nInt, FALSE );
		IQUERY::IREQUESTER()->Regist( pQuery );
	}
	catch(_com_error &e)
	{
		::_bstr_t clsSource( e.Source() );
		::_bstr_t clsDetail( e.Description() );

		STRING strSource;
		STRING strDetail;
		strSource.Printf( "Dev\t[REQ_ERR] Source: %s\r\n", static_cast<CHAR *>( clsSource ) );
		strDetail.Printf( "Dev\t[REQ_ERR] Detail: %s\r\n", static_cast<CHAR *>( clsDetail ) );

		::Dispatcher::ICONSOLE()->Write( strSource );
		::Dispatcher::ICONSOLE()->Write( strDetail );
	}
	catch( ... )
	{
	}

	Provider::ISESSION()->Alarm( E_SIGNAL::CONNECTION_UPDATE, 2 * 60, this );

	ILog::IFacade::Path		stPath;
	
	stPath.strRecordFail	= ::Dispatcher::ICONSOLE()->Read( STRING( "RECORD_FAIL_PATH" ) );
	stPath.strChatting		= ::Dispatcher::ICONSOLE()->Read( STRING( "CHATTING_PATH" ) );
	ILog::IFACADE()->Open( stPath );
	Provider::ISESSION()->Alarm( E_SIGNAL::LOG_FILE, 60 * 60, this );
	Provider::ISESSION()->Alarm( E_SIGNAL::RANKING, 1, this );
	
	cout << "Server Start" << endl;
}

inline void
Provider::onConnectionUpdate()
{
	namespace				D = ::Dispatcher;
	typedef					Query::Server
							SERVER;

	CINT nCount				= IPlayer::ICONTAINER()->GetCount();

	try
	{
		INT nInt			= -99;
		IQUERY * pQuery		= IQUERY::IFACTORY()->Create( SERVER::E_CODE::NOT_CONNECTIONS );

		pQuery->GetLog().Printf( "%s\t%d\r\n",
							ISERVER()->GetID().GetCSTR(),
							nCount );

		IQUERY::IBUILDER()->Build( pQuery, STRING( "SP_GS_GM_ServerStatus_U" ) );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_ServerID" ), ISERVER()->GetID(), TRUE, 4 );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_UserNum" ), nCount, TRUE );

		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_sp_rtn" ), nInt, FALSE );
		IQUERY::IREQUESTER()->Regist( pQuery );
	}
	catch(_com_error &e)
	{
		::_bstr_t clsSource( e.Source() );
		::_bstr_t clsDetail( e.Description() );

		STRING strSource;
		STRING strDetail;
		strSource.Printf( "Dev\t[REQ_ERR] Source: %s\r\n", static_cast<CHAR *>( clsSource ) );
		strDetail.Printf( "Dev\t[REQ_ERR] Detail: %s\r\n", static_cast<CHAR *>( clsDetail ) );

		::Dispatcher::ICONSOLE()->Write( strSource );
		::Dispatcher::ICONSOLE()->Write( strDetail );
	}
	catch( ... )
	{
	}

	Provider::ISESSION()->Alarm( E_SIGNAL::CONNECTION_UPDATE, 2 * 60, this );
}

inline void
Provider::onLogFile()
{
	ILog::IFACADE()->ChangeFile();

	Provider::ISESSION()->Alarm( E_SIGNAL::LOG_FILE, 60 * 60, this );
}

inline void
Provider::onRanking()
{
#ifdef _DEVDEBUG
	return;
#endif

	typedef					Query::Server
							SERVER;

	try
	{
		INT nInt			= -99;
		IQUERY * pQuery		= IQUERY::IFACTORY()->Create( SERVER::E_CODE::REQ_RANK );

		IQUERY::IBUILDER()->Build( pQuery, STRING( "SP_GS_GM_RANK_R" ) );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_result_str" ), STRING( "" ), FALSE, 256 );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_sp_rtn" ), nInt, FALSE );
		IQUERY::IREQUESTER()->Regist( pQuery );
	}
	catch(_com_error &e)
	{
		::_bstr_t clsSource( e.Source() );
		::_bstr_t clsDetail( e.Description() );

		STRING strSource;
		STRING strDetail;
		strSource.Printf( "Dev\t[REQ_ERR] Source: %s\r\n", static_cast<CHAR *>( clsSource ) );
		strDetail.Printf( "Dev\t[REQ_ERR] Detail: %s\r\n", static_cast<CHAR *>( clsDetail ) );

		::Dispatcher::ICONSOLE()->Write( strSource );
		::Dispatcher::ICONSOLE()->Write( strDetail );
	}
	catch( ... )
	{
	}

//	::Dispatcher::ICONSOLE()->Output( STRING( "Tick\r\n") );

	CINT nTick				= ISERVER()->GetRank()->GetRankTick();
	Provider::ISESSION()->Alarm( E_SIGNAL::RANKING, nTick, this );
}

} /* NoneSignal */			} /* GameServer */
