#include "StdAfx.h"

#include "./Logining.h"
#include "./Logined.h"
#include "./NoneNick.h"
#include "./NoneCharacter.h"
#include "./Closing.h"
#include "./../Fin.h"
#include "../../Memento.h"
#include "../../../ICaster.h"
#include "../../../Query.h"
#include "../../../../Dispatcher/IQuery.h"
#include "../../../../Dispatcher/IConsole.h"
#include "../../../../Dispatcher/ITime.h"
#include "../../../../Share/Packet/Login.h"
#include "../../../../Share/Packet/Manager.h"
#include "../../../../Share/Packet/Builder.h"

namespace GameServer {		namespace Player {			namespace State {
namespace Login {

Logining::Logining()
{
	this->install();
}

Logining::~Logining()
{
	this->uninstall();
}

CBOOL
Logining::OnPacket( Memento * i_pMemento, PACKET * i_pPacket )
{
	return FALSE;
}

void
Logining::OnClose( Memento * i_pMemento )
{
//	i_pMemento->GetPlayer()->LogoutProcess();
	i_pMemento->SetState( FIN() );
}

void
Logining::OnResponse( Memento * i_pMemento, IQUERY * i_pQuery )
{
	typedef					Query::Session
							SESSEION;
	Assert( i_pQuery->GetCode() == SESSEION::E_CODE::REQ_LOGIN ||
			i_pQuery->GetCode() == SESSEION::E_CODE::REQ_LOGIN_NETMARBLE );

	if( IPlayer::IBUILDER()->IsLock() == TRUE )
	{
		namespace			M = ::Packet::Manager;

		M::NN_FinishNoti *
		pNoti				= ::Packet::Builder::Build<M::NN_FinishNoti>();
		pNoti->m_byFinishMin= IPlayer::IBUILDER()->GetFinishMin();
		pNoti->m_byType		= IPlayer::IBUILDER()->GetFinishType();
		ICASTER()->Cast( i_pMemento->GetPlayer(), pNoti );
		IPlayer::ISESSION()->Close( i_pMemento->GetPlayer() );
          
		return;
	}
	
	namespace				L = ::Packet::Login;	
	L::SA_Login * pRep		= ::Packet::Builder::Build<L::SA_Login>();

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

	STRING strDbLog;
	if( i_pQuery->GetCode() == SESSEION::E_CODE::REQ_LOGIN )
	{
		strDbLog.Printf( "DB\t%s\t%s\t%d\t%s",
							::Dispatcher::ITIME()->GetTime().GetCSTR(),
							"SP_GS_MB_LOGIN",
							nResult,
							i_pQuery->GetLog().GetCSTR() );
	}
	else
	{
		strDbLog.Printf( "DB\t%s\t%s\t%d\t%s",
							::Dispatcher::ITIME()->GetTime().GetCSTR(),
							"SP_GS_MB_LOGIN_NETMARBLE",
							nResult,
							i_pQuery->GetLog().GetCSTR() );
	}
	::Dispatcher::ICONSOLE()->Write( strDbLog );

	switch( nResult )
	{
	case 0:
		if( this->setUserResult( i_pMemento, i_pQuery ) == FALSE )
		{
			break;
		}
		if( IPlayer::ICONTAINER()->Insert( i_pMemento->GetPlayer() ) == FALSE )
		{
			pRep->m_byResult	= static_cast<SHORT>( L::SA_Login::E_RESULT::FAIL_LOGINED );
			ICASTER()->Cast( i_pMemento->GetPlayer(), pRep );
			i_pMemento->SetState( CLOSING() );
			return;
		}
		pRep->m_byResult	= static_cast<SHORT>( L::SA_Login::E_RESULT::SUCCESS );
		pRep->m_nIndex		= i_pMemento->GetPlayer()->GetIndex();
		ICASTER()->Cast( i_pMemento->GetPlayer(), pRep );

		i_pMemento->SetState( LOGINED() );
		
		requestResult( i_pMemento );
		break;

	case 1:
		pRep->m_byResult	= static_cast<SHORT>( L::SA_Login::E_RESULT::FAIL_LOGINED_DB );
		ICASTER()->Cast( i_pMemento->GetPlayer(), pRep );

		i_pMemento->SetState( CLOSING() );
		break;
		
	case 2:
		i_pMemento->SetNewCharacter();
		this->castNoneNick( i_pMemento );
		i_pMemento->SetState( NONENICK() );
		break;

	case 3:
		pRep->m_byResult	= static_cast<SHORT>( L::SA_Login::E_RESULT::FAIL_NETMARBLE_WRONG_IP );
		ICASTER()->Cast( i_pMemento->GetPlayer(), pRep );

		i_pMemento->SetState( CLOSING() );
		break;

	case -1:
		pRep->m_byResult	= static_cast<SHORT>( L::SA_Login::E_RESULT::FAIL_QUERY );
		ICASTER()->Cast( i_pMemento->GetPlayer(), pRep );
		
		i_pMemento->SetState( CLOSING() );
		break;

	case -2:
		pRep->m_byResult	= static_cast<SHORT>( L::SA_Login::E_RESULT::FAIL_WRONG_ID );
		ICASTER()->Cast( i_pMemento->GetPlayer(), pRep );
		
		i_pMemento->SetState( CLOSING() );
		break;

	case -4:
		pRep->m_byResult	= static_cast<SHORT>( L::SA_Login::E_RESULT::FAIL_DB );
		ICASTER()->Cast( i_pMemento->GetPlayer(), pRep );

		i_pMemento->SetState( CLOSING() );
		break;

	case -12:
		i_pMemento->SetNewCharacter();
		this->castNoneNick( i_pMemento );
		i_pMemento->SetState( NONENICK() );
		break;
		//if( this->setUserResult( i_pMemento, i_pQuery ) == FALSE )
		//{
		//	break;
		//}
		//if( IPlayer::ICONTAINER()->Insert( i_pMemento->GetPlayer() ) == FALSE )
		//{
		//	pRep->m_byResult	= static_cast<SHORT>( L::SA_Login::E_RESULT::FAIL_LOGINED );
		//	ICASTER()->Cast( i_pMemento->GetPlayer(), pRep );
		//	return;
		//}

		//this->castNoneCharacter( i_pMemento );
		//i_pMemento->SetState( NONECHARACTER() );
		break;

	default:
		pRep->m_byResult	= static_cast<SHORT>( L::SA_Login::E_RESULT::FAIL_DB_ERROR );
		ICASTER()->Cast( i_pMemento->GetPlayer(), pRep );

		i_pMemento->SetState( CLOSING() );
		break;
	}

}

inline CBOOL
Logining::setUserResult( Memento * i_pMemento, IQUERY * i_pQuery )
{
	typedef					Memento::E_TEAM
							E_TEAM;
	typedef					::LogicObject::Player::Character
							CHARACTER;
	CHARACTER & stRed		= i_pMemento->GetCharacters()[ E_TEAM::RED ];
	CHARACTER & stBlue		= i_pMemento->GetCharacters()[ E_TEAM::BLUE ];

	try
	{
		i_pMemento->GetStock().SetNick( IQUERY::IRESPONSER()->ReadString( i_pQuery, STRING( "@o_user_nick" ) ).GetCSTR() );
		stRed.m_nModel		= IQUERY::IRESPONSER()->ReadInt( i_pQuery, STRING( "@o_a_model_id" ) ); 
		stRed.m_nHeadSkin	= IQUERY::IRESPONSER()->ReadInt( i_pQuery, STRING( "@o_a_headskin_id" ) ); 
		stRed.m_nBodySkin	= IQUERY::IRESPONSER()->ReadInt( i_pQuery, STRING( "@o_a_bodyskin_id" ) ); 
		stBlue.m_nModel		= IQUERY::IRESPONSER()->ReadInt( i_pQuery, STRING( "@o_b_model_id" ) ); 
		stBlue.m_nHeadSkin	= IQUERY::IRESPONSER()->ReadInt( i_pQuery, STRING( "@o_b_headskin_id" ) ); 
		stBlue.m_nBodySkin	= IQUERY::IRESPONSER()->ReadInt( i_pQuery, STRING( "@o_b_bodyskin_id" ) ); 
		i_pMemento->SetConnectID( String<17>( IQUERY::IRESPONSER()->ReadString( i_pQuery, STRING( "@o_conn_no" ) ).GetCSTR() ) );
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
		return FALSE;
	}
	catch( ... )
	{
		IPlayer::ISESSION()->Close( i_pMemento->GetPlayer() );
		return FALSE;
	}

	return TRUE;
}

inline void
Logining::requestResult( Memento * i_pMemento )
{
	typedef					Query::Result
							RESULT;

	INT nInt = -99;
	__int64 nInt64 = 0;
	
	//STRING strLog;
	//strLog.Printf( "[DB_REQ][%s] %d %s\r\n", ::Dispatcher::ITIME()->GetTime().GetCSTR(), RESULT::E_CODE::REQ_READ, i_pMemento->GetUserNo().GetCSTR() );
	//::Dispatcher::ICONSOLE()->Output( strLog );

	try
	{
		IQUERY * pQuery			= IQUERY::IFACTORY()->Create( RESULT::E_CODE::REQ_READ );

		pQuery->GetLog().Printf( "%s\r\n", i_pMemento->GetUserNo().GetCSTR() );

		IQUERY::IBUILDER()->Build( pQuery, STRING( "SP_GS_GM_RESULT_R" ) );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_user_no" ), STRING( i_pMemento->GetUserNo().GetCSTR() ), TRUE, 12 );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_level_no" ), nInt, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_level_name" ), STRING( "" ), FALSE, 20 );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_user_exp" ), nInt64, FALSE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_user_money" ), nInt64, FALSE );
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

		IPlayer::ISESSION()->Close( i_pMemento->GetPlayer() );
	}
	catch( ... )
	{
		IPlayer::ISESSION()->Close( i_pMemento->GetPlayer() );
	}
}

inline void
Logining::castNoneNick( Memento * i_pMemento )
{
	typedef					::Packet::Login::SN_Nick
							SN_NICK;

	SN_NICK * pNoti			= ::Packet::Builder::Build<SN_NICK>();

	pNoti->m_byResult		= static_cast<BYTE>( SN_NICK::E_RESULT::EMPTY );

	ICASTER()->Cast( i_pMemento->GetPlayer(), pNoti );
}

inline void
Logining::castNoneCharacter( Memento * i_pMemento )
{
	typedef					::Packet::Login::SN_Character
							SN_CHARACTER;

	SN_CHARACTER * pNoti	= ::Packet::Builder::Build<SN_CHARACTER>();
	pNoti->m_byResult		= static_cast<BYTE>( SN_CHARACTER::E_RESULT::EMPTY );
	pNoti->m_nIndex			= i_pMemento->GetPlayer()->GetIndex();
	ICASTER()->Cast( i_pMemento->GetPlayer(), pNoti );
}

} /* Login */
} /* State */				} /* Player */				} /* GameServer */