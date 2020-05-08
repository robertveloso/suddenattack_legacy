#include "StdAfx.h"

#include "./OnGame.h"

#include "../IServer.h"
#include "../IPlayer.h"
#include "../Query.h"
#include "../Player/IResult.h"
#include "../Player/Memento.h"
#include "../../Dispatcher/ITime.h"
#include "../../Dispatcher/IConsole.h"


namespace GameServer {		namespace NoneResponser {

OnGame::OnGame()
{
}

OnGame::~OnGame()
{
}

void
OnGame::OnResponse( IQUERY * i_pQuery )
{
	typedef					Query::Result
							RESULT;
	Assert( i_pQuery->GetCode() == RESULT::E_CODE::REQ_GAME );

	INT nResult				= -99;
	typedef					String< ::LogicObject::Player::Type::s_nUserNo >
							USERNO;
	try
	{
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

	STRING strDbLog;
	strDbLog.Printf( "DB\t%s\t%s\t%d\t%s",
							::Dispatcher::ITIME()->GetTime().GetCSTR(),
							"SP_GS_GM_ROOM_LOG_C",
							nResult,
							i_pQuery->GetLog().GetCSTR() );
	::Dispatcher::ICONSOLE()->Write( strDbLog );

}

} /* NoneResponser */		} /* GameServer */