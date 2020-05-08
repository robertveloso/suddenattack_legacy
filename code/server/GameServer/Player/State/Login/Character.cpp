#include "StdAfx.h"

#include "./Character.h"
#include "./NoneCharacter.h"
#include "./Logined.h"
#include "./Closing.h"
#include "./../Fin.h"
#include "../../Memento.h"
#include "../../../ICaster.h"
#include "../../../Query.h"
#include "../../../../Dispatcher/IQuery.h"
#include "../../../../Dispatcher/ITime.h"
#include "../../../../Dispatcher/IConsole.h"
#include "../../../../Share/Packet/Login.h"
#include "../../../../Share/Packet/Builder.h"

namespace GameServer {		namespace Player {			namespace State {
namespace Login {

Character::Character()
{
	this->install();
}

Character::~Character()
{
	this->uninstall();
}

CBOOL
Character::OnPacket( Memento * i_pMemento, PACKET * i_pPacket )
{
	return TRUE;
}

void
Character::OnClose( Memento * i_pMemento )
{
	i_pMemento->GetPlayer()->LogoutProcess();
	i_pMemento->SetState( FIN() );
}

void
Character::OnResponse( Memento * i_pMemento, IQUERY * i_pQuery )
{
	typedef					Query::Session
							SESSEION;
	Assert( i_pQuery->GetCode() == SESSEION::E_CODE::REQ_CHARACTER );

	namespace				L = ::Packet::Login;
	L::SN_Character * pRep	= ::Packet::Builder::Build<L::SN_Character>();
	pRep->m_nIndex			= i_pMemento->GetPlayer()->GetIndex();

	INT nResult				= -99;
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
							"SP_GS_GM_CHARACTER_U",
							nResult,
							i_pQuery->GetLog().GetCSTR() );
	::Dispatcher::ICONSOLE()->Write( strDbLog );

	switch( nResult )
	{
	case 0:
		this->requestResult( i_pMemento );
		i_pMemento->SetState( LOGINED() );
		break;

	case -21:
		pRep->m_byResult	= static_cast<BYTE>( L::SN_Character::E_RESULT::FAIL_DB1 );
		ICASTER()->Cast( i_pMemento->GetPlayer(), pRep );
		i_pMemento->SetState( NONECHARACTER() );
		break;

	case -22:
		pRep->m_byResult	= static_cast<BYTE>( L::SN_Character::E_RESULT::FAIL_DB2 );
		ICASTER()->Cast( i_pMemento->GetPlayer(), pRep );
		i_pMemento->SetState( NONECHARACTER() );
		break;

	case -23:
		pRep->m_byResult	= static_cast<BYTE>( L::SN_Character::E_RESULT::FAIL_DB3 );
		ICASTER()->Cast( i_pMemento->GetPlayer(), pRep );
		i_pMemento->SetState( NONECHARACTER() );
		break;
	}
}

inline void
Character::requestResult( Memento * i_pMemento )
{
	typedef					Query::Result
							RESULT;

	INT nInt = -99;
//	__Int nInt = 0;
	
	//STRING strLog;
	//strLog.Printf( "[DB_REQ][%s] %d %s\r\n", ::Dispatcher::ITIME()->GetTime().GetCSTR(), RESULT::E_CODE::REQ_READ, i_pMemento->GetUserNo().GetCSTR() );
	//::Dispatcher::ICONSOLE()->Write( strLog );

	try
	{
		IQUERY * pQuery			= IQUERY::IFACTORY()->Create( RESULT::E_CODE::REQ_READ );

		pQuery->GetLog().Printf( "%s\r\n", i_pMemento->GetUserNo().GetCSTR() );

		IQUERY::IBUILDER()->Build( pQuery, STRING( "SP_GS_GM_RESULT_R" ) );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_user_no" ), STRING( i_pMemento->GetUserNo().GetCSTR() ), TRUE, 20 );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_level_no" ), nInt, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_level_name" ), STRING( "" ), FALSE, 20 );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_user_exp" ), nInt, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_user_money" ), nInt, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_kill_cnt" ), nInt, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_death_cnt" ), nInt, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_h_kill_cnt" ), nInt, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_h_death_cnt" ), nInt, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_win_cnt" ), nInt, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_lose_cnt" ), nInt, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_draw_cnt" ), nInt, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_max_kill_cnt" ), nInt, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_max_h_kill_cnt" ), nInt, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_user_rank" ), nInt, FALSE );

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
	}
	catch( ... )
	{
	}

}

} /* Login */
} /* State */				} /* Player */				} /* GameServer */