#include "StdAfx.h"

#include "./OnLogout.h"

#include "../IServer.h"
#include "../IPlayer.h"
#include "../Query.h"
#include "../Player/IResult.h"
#include "../Player/Memento.h"
#include "../../Dispatcher/ITime.h"
#include "../../Dispatcher/IConsole.h"


namespace GameServer {		namespace NoneResponser {

OnLogout::OnLogout()
{
}

OnLogout::~OnLogout()
{
}

void
OnLogout::OnResponse( IQUERY * i_pQuery )
{
	typedef					Query::Session
							SESSION;
	Assert( i_pQuery->GetCode() == SESSION::E_CODE::NOT_LOGOUT );

	INT nResult				= 0;
	STRING strUser;
	try
	{
		strUser				= IQUERY::IRESPONSER()->ReadString( i_pQuery, STRING( "@i_user_no" ) );
		nResult				= IQUERY::IRESPONSER()->ReadInt( i_pQuery, STRING( "@o_sp_rtn" ) );
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
	}
	catch( ... )
	{
		nResult				= -99;
	}

	if( i_pQuery->GetResult() == IQUERY::E_RESULT::FAIL )
	{
		nResult				= -99;
	}

	//STRING strLog;
	//strLog.Printf( "[DB_RESP][%s] %d %d %s\r\n", ::Dispatcher::ITIME()->GetTime().GetCSTR(), i_pQuery->GetCode(), nResult, strUser.GetCSTR() );
	//::Dispatcher::ICONSOLE()->Write( strLog );

	STRING strDbLog;
	strDbLog.Printf( "DB\t%s\t%s\t%d\t%s",
							::Dispatcher::ITIME()->GetTime().GetCSTR(),
							"SP_GS_MB_LOGOUT",
							nResult,
							i_pQuery->GetLog().GetCSTR() );
	::Dispatcher::ICONSOLE()->Write( strDbLog );
}

} /* NoneResponser */		} /* GameServer */