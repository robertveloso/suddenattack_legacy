#include "StdAfx.h"

#include "./Provider.h"

#include "../Query.h"

#include "./OnServerStart.h"
#include "./OnServerConnections.h"
#include "./OnLevel.h"
#include "./OnResult.h"
#include "./OnGame.h"
#include "./OnLogout.h"
#include "./OnRank.h"

namespace GameServer {		namespace NoneResponser {

Provider::Provider()
{
	this->install();
}

Provider::~Provider()
{
	this->uninstall();
}

void
Provider::OnResponse( IQUERY * i_pQuery )
{
	switch( i_pQuery->GetCode() )
	{
	case Query::Server::E_CODE::REQ_START:
		OnServerStart::OnResponse( i_pQuery );
		return;

	case Query::Server::E_CODE::REQ_LEVEL :
		OnLevel::OnResponse( i_pQuery );
		return;

	case Query::Server::E_CODE::NOT_CONNECTIONS:
		OnServerConnections::OnResponse( i_pQuery );
		return;

	case Query::Server::E_CODE::REQ_RANK :
		OnRank::OnResponse( i_pQuery );
		return;

	case Query::Result::E_CODE::REQ_RECORD :
		OnResult::OnResponse( i_pQuery );
		return;

	case Query::Result::E_CODE::REQ_GAME :
		OnGame::OnResponse( i_pQuery );
		return;

	case Query::Session::E_CODE::NOT_LOGOUT :
		OnLogout::OnResponse( i_pQuery );
		return;

	default:
		DEBUG_BREAK;
		break;
	}

}

} /* NoneResponser */		} /* GameServer */