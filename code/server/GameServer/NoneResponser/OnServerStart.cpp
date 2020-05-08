#include "StdAfx.h"

#include "./OnServerStart.h"

#include "../IServer.h"
#include "../IPlayer.h"
#include "../Query.h"
#include "../Player/IResult.h"
#include "../Player/Memento.h"
#include "../../Dispatcher/ITime.h"
#include "../../Dispatcher/IConsole.h"


namespace GameServer {		namespace NoneResponser {

OnServerStart::OnServerStart()
{
}

OnServerStart::~OnServerStart()
{
}

void
OnServerStart::OnResponse( IQUERY * i_pQuery )
{
	typedef					Query::Server
							SERVER;
	Assert( i_pQuery->GetCode() == SERVER::E_CODE::REQ_START );

	INT nResult				= -99;
	try
	{
		nResult				= IQUERY::IRESPONSER()->ReadInt( i_pQuery, STRING( "@o_sp_rtn" ) );
		if( nResult == 0 )
		{
			ISERVER()->SetID( IQUERY::IRESPONSER()->ReadString( i_pQuery, STRING( "@o_ServerID" ) ) );
			ISERVER()->SetName( IQUERY::IRESPONSER()->ReadString( i_pQuery, STRING( "@o_ServerName" ) ) );
		}

	}
	catch(_com_error &e)
	{
		::_bstr_t clsSource( e.Source() );
		::_bstr_t clsDetail( e.Description() );

		STRING strSource;
		STRING strDetail;
		strSource.Printf( "Dev\t[RESP_ERR] Source: %s\r\n", static_cast<CHAR *>( clsSource ) );
		strDetail.Printf( "Dev\t[RESP_ERR] Detail: %s\r\n", static_cast<CHAR *>( clsDetail ) );

		::Dispatcher::ICONSOLE()->Write( strSource );
		::Dispatcher::ICONSOLE()->Write( strDetail );

		nResult				= -99;

		printf( "Server Start DB Error \n%s\n%s\n", strSource, strDetail );
		DEBUG_BREAK;
	}
	catch( ... )
	{
		nResult				= -99;

		printf( "Server Start DB Error : %d\n", nResult );
		DEBUG_BREAK;
	}

	if( i_pQuery->GetResult() == IQUERY::E_RESULT::FAIL )
	{
		nResult				= -99;
	}

	STRING strDbLog;
	strDbLog.Printf( "DB\t%s\t%s\t%d\t%s",
							::Dispatcher::ITIME()->GetTime().GetCSTR(),
							"SP_GS_GM_ServerStatus_R",
							nResult,
							i_pQuery->GetLog().GetCSTR() );
	::Dispatcher::ICONSOLE()->Write( strDbLog );

	if( nResult == 0 )
	{
		::Dispatcher::ICONSOLE()->Output( STRING( "OnServer Start\r\n" ) );
		cout << "OnServer Start" << endl;
	}
	else
	{
		::Dispatcher::ICONSOLE()->Output( STRING( "OnServer Fail\r\n" ) );
		cout << "OnServer Fail!!!" << endl;
		return;
	}

	//////////////////////////////////////////////////////////

	try
	{
		INT nInt				= -99;
		IQUERY * pQuery			= IQUERY::IFACTORY()->Create( SERVER::E_CODE::REQ_LEVEL );

		pQuery->GetLog().Printf( "\r\n" );

		Dispatcher::ICONSOLE()->Output( pQuery->GetLog() );

		IQUERY::IBUILDER()->Build( pQuery, STRING( "SP_GS_GM_LEVEL_R" ) );

		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_result_str" ), STRING( "" ), FALSE, SERVER::s_nLevelLength );
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
}

} /* NoneResponser */		} /* GameServer */