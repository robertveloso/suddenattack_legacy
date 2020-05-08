#include "StdAfx.h"

#include "./NoneNick.h"
#include "./Nick.h"
#include "./../Fin.h"
#include "../../Memento.h"
#include "../../../ICaster.h"
#include "../../../Query.h"
#include "../../../../Dispatcher/IQuery.h"
#include "../../../../Dispatcher/IConsole.h"
#include "../../../../Dispatcher/ITime.h"
#include "../../../../Share/Packet/Login.h"
#include "../../../../Share/Packet/Builder.h"

namespace GameServer {		namespace Player {			namespace State {
namespace Login {

NoneNick::NoneNick()
{
	this->install();
}

NoneNick::~NoneNick()
{
	this->uninstall();
}

CBOOL
NoneNick::OnPacket( Memento * i_pMemento, PACKET * i_pPacket )
{
	typedef					::Packet::Login::CN_Nick
							CN_NICK;

	if( i_pPacket->GetCommand() != CN_NICK::s_nCode )
	{
		return TRUE;
	}

	CN_NICK * pReq			= static_cast<CN_NICK *>( i_pPacket );
	i_pMemento->GetStock().SetNick( pReq->GetNick() );

	this->requestNick( i_pMemento );
	i_pMemento->SetState( NICK() );

	return TRUE;
}

void
NoneNick::OnClose( Memento * i_pMemento )
{
	i_pMemento->GetPlayer()->LogoutProcess();
	i_pMemento->SetState( FIN() );
}

void
NoneNick::OnResponse( Memento * i_pMemento, IQUERY * i_pQuery )
{
	DEBUG_BREAK;
}

inline void
NoneNick::requestNick( Memento * i_pMemento )
{
	typedef					Query::Session
							SESSION;

	INT nInt = -99;
	
	//STRING strLog;
	//strLog.Printf( "[DB_REQ][%s] %d %s %s\r\n", ::Dispatcher::ITIME()->GetTime().GetCSTR(), SESSION::E_CODE::REQ_CHARACTER, i_pMemento->GetUserNo().GetCSTR(),
	//					i_pMemento->GetStock().GetNick() );
	//::Dispatcher::ICONSOLE()->Write( strLog );

	try
	{
		IQUERY * pQuery			= IQUERY::IFACTORY()->Create( SESSION::E_CODE::REQ_NICK );

		pQuery->GetLog().Printf( "%s\r\n", i_pMemento->GetUserNo().GetCSTR() );

		IQUERY::IBUILDER()->Build( pQuery, STRING( "SP_GS_MB_USER_NICK_U" ) );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_user_no" ), STRING( i_pMemento->GetUserNo().GetCSTR() ), TRUE, 12 );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_user_nick" ), STRING( i_pMemento->GetStock().GetNick() ), TRUE, 40 );

		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_sp_rtn" ), nInt, FALSE );

		IQUERY::IREQUESTER()->Regist( pQuery, i_pMemento->GetPlayer() );
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

		IPlayer::ISESSION()->Close( i_pMemento->GetPlayer() );
	}
	catch( ... )
	{
		IPlayer::ISESSION()->Close( i_pMemento->GetPlayer() );
	}

}

} /* Login */
} /* State */				} /* Player */				} /* GameServer */