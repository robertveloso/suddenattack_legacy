#include "StdAfx.h"

#include "./Init.h"
#include "./Logining.h"
#include "./Closing.h"
#include "../../Memento.h"
#include "../../../Query.h"
#include "../../../IServer.h"
#include "../../../IChannel.h"
#include "../../../ICaster.h"
#include "../../../../Share/Packet/Builder.h"
#include "../../../../Share/Packet/Login.h"
#include "../../../../Dispatcher/IQuery.h"
#include "../../../../Dispatcher/IConsole.h"
#include "../../../../Dispatcher/ITime.h"

#include "../../../../NetmarbleLib/CPSSOLib.h"

namespace GameServer {		namespace Player {			namespace State {
namespace Login {

Init::Init()
{
	this->install();
}

Init::~Init()
{
	this->uninstall();
}

CBOOL
Init::OnPacket( Memento * i_pMemento, PACKET * i_pPacket )
{
	namespace				L = ::Packet::Login;
	typedef					Query::Session
							SESSION;

	switch( i_pPacket->GetCommand() )
	{
	case L::CQ_Login::s_nCode :
		return this->onGamehiLogin( i_pMemento, i_pPacket );
		break;

	case L::CQ_LoginNetmarble::s_nCode :
		return this->onNetmarbleLogin( i_pMemento, i_pPacket );
		break;

	default:
		i_pMemento->SetState( CLOSING() );
		return FALSE;
	}

	return TRUE;
}

void
Init::OnClose( Memento * i_pMemento )
{
//	DEBUG_BREAK;
}

void
Init::OnResponse( Memento * i_pMemento, IQUERY * i_pQuery )
{
	DEBUG_BREAK;
}

inline void
Init::onDevLogin( Memento * i_pMemento, PACKET * i_pPacket )
{
#ifndef _DEVDEBUG
	DEBUG_BREAK;
#endif

	typedef					::Packet::Login::CQ_Login
							CQ_LOGIN;
	typedef					Query::Session
							SESSION;
	typedef					::LogicObject::Player::Type
							TYPE;

	INT nCodeH				= 5052;
	INT nCodeL				= 5000001;

	INT nAdd				= ( i_pMemento->GetPlayer()->GetIndex() % 30 ) + 1;

	nCodeL					+= nAdd;

	String<TYPE::s_nUserNo> strCode;
	strCode.Printf( "0%d%d", nCodeH, nCodeL );

	CQ_LOGIN * pReq		= static_cast<CQ_LOGIN *>( i_pPacket );

	i_pMemento->SetUserNo( strCode );
	i_pMemento->SetClientIP( Memento::IPADDR( pReq->GetIP() ) );
	//if( ( i_pMemento->GetIP() == i_pMemento->GetClientIP() ) == FALSE )
	//{
	//	i_pMemento->DisableSuperPeer();
	//}

	INT nResult = -99;
	
	//STRING strLog;
	//strLog.Printf( "[DB_REQ][%s] %d %s\r\n", ::Dispatcher::ITIME()->GetTime().GetCSTR(), SESSION::E_CODE::REQ_LOGIN, i_pMemento->GetUserNo().GetCSTR() );
	//::Dispatcher::ICONSOLE()->Write( strLog );

	try
	{
		IQUERY * pQuery			= IQUERY::IFACTORY()->Create( SESSION::E_CODE::REQ_LOGIN );

		pQuery->GetLog().Printf( "%s\r\n", i_pMemento->GetUserNo().GetCSTR() );

		IQUERY::IBUILDER()->Build( pQuery, STRING( "SP_GS_MB_LOGIN" ) );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_user_no" ), STRING( i_pMemento->GetUserNo().GetCSTR() ), TRUE, 12 );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_login_flag" ), 1, TRUE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_ipt_ip_addr" ), STRING( i_pMemento->GetIP().GetCSTR() ), TRUE, 15 );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_ServerID" ), ISERVER()->GetID(), TRUE, 4 );
		IQUERY::IBUILDER()->BuildLowStr( pQuery, STRING( "@i_comm" ), String<601>( pReq->GetMemo() ).GetCSTR(), TRUE, 600 );
		
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_user_nick" ), STRING( "" ), FALSE, 40 );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_a_model_id" ), nResult, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_a_headskin_id" ), nResult, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_a_bodyskin_id" ), nResult, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_b_model_id" ), nResult, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_b_headskin_id" ), nResult, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_b_bodyskin_id" ), nResult, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_conn_no" ), STRING( "" ), FALSE, 16 );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_sp_rtn" ), nResult, FALSE );

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
	
	i_pMemento->SetAge( 20 );
	i_pMemento->SetRealName( TRUE );

	i_pMemento->SetState( LOGINING() );

	return;

}

inline CBOOL
Init::onGamehiLogin( Memento * i_pMemento, PACKET * i_pPacket )
{
	if( ISERVER()->CanLoginGamehi() == FALSE )
	{
		i_pMemento->SetState( CLOSING() );
		return FALSE;
	}

#ifdef _DEVDEBUG
	this->onDevLogin( i_pMemento, i_pPacket );
	return TRUE;
#endif

	typedef					::Packet::Login::CQ_Login
							CQ_LOGIN;
	typedef					Query::Session
							SESSION;

	Assert( i_pPacket->GetCommand() == CQ_LOGIN::s_nCode );

	CQ_LOGIN * pReq		= static_cast<CQ_LOGIN *>( i_pPacket );

	i_pMemento->SetUserNo( pReq->GetUserNo() );
	i_pMemento->SetClientIP( Memento::IPADDR( pReq->GetIP() ) );

	INT nResult = -99;
	
	//STRING strLog;
	//strLog.Printf( "[DB_REQ][%s] %d %s\r\n", ::Dispatcher::ITIME()->GetTime().GetCSTR(), SESSION::E_CODE::REQ_LOGIN, i_pMemento->GetUserNo().GetCSTR() );
	//::Dispatcher::ICONSOLE()->Write( strLog );

	try
	{
		IQUERY * pQuery			= IQUERY::IFACTORY()->Create( SESSION::E_CODE::REQ_LOGIN );

		pQuery->GetLog().Printf( "%s\r\n", i_pMemento->GetUserNo().GetCSTR() );

		IQUERY::IBUILDER()->Build( pQuery, STRING( "SP_GS_MB_LOGIN" ) );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_user_no" ), STRING( i_pMemento->GetUserNo().GetCSTR() ), TRUE, 12 );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_login_flag" ), 1, TRUE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_ipt_ip_addr" ), STRING( i_pMemento->GetIP().GetCSTR() ), TRUE, 15 );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_ServerID" ), ISERVER()->GetID(), TRUE, 4 );
		IQUERY::IBUILDER()->BuildLowStr( pQuery, STRING( "@i_comm" ), String<601>( pReq->GetMemo() ).GetCSTR(), TRUE, 600 );
		
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_user_nick" ), STRING( "" ), FALSE, 40 );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_a_model_id" ), nResult, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_a_headskin_id" ), nResult, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_a_bodyskin_id" ), nResult, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_b_model_id" ), nResult, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_b_headskin_id" ), nResult, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_b_bodyskin_id" ), nResult, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_conn_no" ), STRING( "" ), FALSE, 16 );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_sp_rtn" ), nResult, FALSE );

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

	i_pMemento->SetAge( 20 );
	i_pMemento->SetRealName( TRUE );
	
	i_pMemento->SetState( LOGINING() );

	return TRUE;
}

inline CBOOL
Init::onNetmarbleLogin( Memento * i_pMemento, PACKET * i_pPacket )
{
	if( ISERVER()->CanLoginNetmarble() == FALSE )
	{
		i_pMemento->SetState( CLOSING() );
		return FALSE;
	}

	namespace				L = ::Packet::Login;
	typedef					Query::Session
							SESSION;

	Assert( i_pPacket->GetCommand() == L::CQ_LoginNetmarble::s_nCode );

	L::CQ_LoginNetmarble *
	pReq					= static_cast<L::CQ_LoginNetmarble *>( i_pPacket );

	CHAR strVal[1024] = {0,};

	//if( IsCPTimeOut( pReq->GetCpCookie() ) == TRUE )
	//{
	//	L::SA_Login * pRep	= ::Packet::Builder::Build<L::SA_Login>();
	//	pRep->m_byResult	= static_cast<SHORT>( L::SA_Login::E_RESULT::FAIL_NETMARBLE_TIMEOUT );
	//	ICASTER()->Cast( i_pMemento->GetPlayer(), pRep );
	//	i_pMemento->SetState( CLOSING() );
	//	return FALSE;
	//}

	if( ::GetCPCookieValue( pReq->GetCpCookie(), "UserID", strVal ) == FALSE )
	{
		L::SA_Login * pRep	= ::Packet::Builder::Build<L::SA_Login>();
		pRep->m_byResult	= static_cast<SHORT>( L::SA_Login::E_RESULT::FAIL_NETMARBLE_DEC );
		ICASTER()->Cast( i_pMemento->GetPlayer(), pRep );
		i_pMemento->SetState( CLOSING() );
		return TRUE;
	}
	STRING strID( strVal );
	if( ::GetCPCookieValue( pReq->GetCpCookie(), "Name", strVal ) == FALSE )
	{
		L::SA_Login * pRep	= ::Packet::Builder::Build<L::SA_Login>();
		pRep->m_byResult	= static_cast<SHORT>( L::SA_Login::E_RESULT::FAIL_NETMARBLE_DEC );
		ICASTER()->Cast( i_pMemento->GetPlayer(), pRep );
		i_pMemento->SetState( CLOSING() );
		return TRUE;
	}
	STRING strName( strVal );
	if( ::GetCPCookieValue( pReq->GetCpCookie(), "Sex", strVal ) == FALSE )
	{
		L::SA_Login * pRep	= ::Packet::Builder::Build<L::SA_Login>();
		pRep->m_byResult	= static_cast<SHORT>( L::SA_Login::E_RESULT::FAIL_NETMARBLE_DEC );
		ICASTER()->Cast( i_pMemento->GetPlayer(), pRep );
		i_pMemento->SetState( CLOSING() );
		return TRUE;
	}
	CINT nSex				= ::atoi( strVal );
	if( ::GetCPCookieValue( pReq->GetCpCookie(), "UniID", strVal ) == FALSE )
	{
		L::SA_Login * pRep	= ::Packet::Builder::Build<L::SA_Login>();
		pRep->m_byResult	= static_cast<SHORT>( L::SA_Login::E_RESULT::FAIL_NETMARBLE_DEC );
		ICASTER()->Cast( i_pMemento->GetPlayer(), pRep );
		i_pMemento->SetState( CLOSING() );
		return TRUE;
	}
	i_pMemento->SetUserNo( strVal );
	if( ::GetCPCookieValue( pReq->GetCpCookie(), "Age", strVal ) == FALSE )
	{
		L::SA_Login * pRep	= ::Packet::Builder::Build<L::SA_Login>();
		pRep->m_byResult	= static_cast<SHORT>( L::SA_Login::E_RESULT::FAIL_NETMARBLE_DEC );
		ICASTER()->Cast( i_pMemento->GetPlayer(), pRep );
		i_pMemento->SetState( CLOSING() );
		return TRUE;
	}
	i_pMemento->SetAge( ::atoi( strVal ) );
	if( ::GetCPCookieValue( pReq->GetCpCookie(), "Certificate", strVal ) == FALSE )
	{
		L::SA_Login * pRep	= ::Packet::Builder::Build<L::SA_Login>();
		pRep->m_byResult	= static_cast<SHORT>( L::SA_Login::E_RESULT::FAIL_NETMARBLE_DEC );
		ICASTER()->Cast( i_pMemento->GetPlayer(), pRep );
		i_pMemento->SetState( CLOSING() );
		return TRUE;
	}
	i_pMemento->SetRealName( ::atoi( strVal ) );

	i_pMemento->SetClientIP( Memento::IPADDR( pReq->GetIP() ) );

	INT nResult = -99;
	
	try
	{
		IQUERY * pQuery			= IQUERY::IFACTORY()->Create( SESSION::E_CODE::REQ_LOGIN_NETMARBLE );

		pQuery->GetLog().Printf( "%s\r\n", i_pMemento->GetUserNo().GetCSTR() );

		IQUERY::IBUILDER()->Build( pQuery, STRING( "SP_GS_MB_LOGIN_NETMARBLE" ) );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_user_id" ), strID, TRUE, 40 );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_user_no" ), STRING( i_pMemento->GetUserNo().GetCSTR() ), TRUE, 12 );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_user_age" ), i_pMemento->GetAge(), TRUE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_user_name" ), strName, TRUE, 20 );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_user_sex" ), nSex, TRUE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_login_flag" ), 1, TRUE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_ipt_ip_addr" ), STRING( i_pMemento->GetIP().GetCSTR() ), TRUE, 15 );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_ServerID" ), ISERVER()->GetID(), TRUE, 4 );
		IQUERY::IBUILDER()->BuildLowStr( pQuery, STRING( "@i_comm" ), String<601>( pReq->GetMemo() ).GetCSTR(), TRUE, 600 );
		
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_user_nick" ), STRING( "" ), FALSE, 40 );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_a_model_id" ), nResult, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_a_headskin_id" ), nResult, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_a_bodyskin_id" ), nResult, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_b_model_id" ), nResult, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_b_headskin_id" ), nResult, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_b_bodyskin_id" ), nResult, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_conn_no" ), STRING( "" ), FALSE, 16 );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_sp_rtn" ), nResult, FALSE );

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
	
	i_pMemento->SetState( LOGINING() );

	return TRUE;
}

} /* Login */
} /* State */				} /* Player */				} /* GameServer */