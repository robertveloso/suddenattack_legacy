#include "StdAfx.h"

#include "./Nick.h"
#include "./NoneNick.h"
#include "./NoneCharacter.h"
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

Nick::Nick()
{
	this->install();
}

Nick::~Nick()
{
	this->uninstall();
}

CBOOL
Nick::OnPacket( Memento * i_pMemento, PACKET * i_pPacket )
{
	return TRUE;
}

void
Nick::OnClose( Memento * i_pMemento )
{
	i_pMemento->GetPlayer()->LogoutProcess();
	i_pMemento->SetState( FIN() );
}

void
Nick::OnResponse( Memento * i_pMemento, IQUERY * i_pQuery )
{
	typedef					Query::Session
							SESSEION;
	Assert( i_pQuery->GetCode() == SESSEION::E_CODE::REQ_NICK );

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
	L::SN_Nick * pRep		= ::Packet::Builder::Build<L::SN_Nick>();

	//if( i_pQuery->GetResult() == IQUERY::E_RESULT::FAIL )
	//{
	//	pRep->m_byResult	= static_cast<BYTE>( L::SN_Nick::E_RESULT::FAIL_ADO );
	//	ICASTER()->Cast( i_pMemento->GetPlayer(), pRep );
	//	i_pMemento->SetState( NONENICK() );

	//	STRING strUser;
	//	INT nResult			= -99;
	//	try
	//	{
	//		strUser			= IQUERY::IRESPONSER()->ReadString( i_pQuery, STRING( "@i_user_no" ) );
	//		nResult			= IQUERY::IRESPONSER()->ReadInt( i_pQuery, STRING( "@o_sp_rtn" ) );
	//	}
	//	catch( ... )
	//	{
	//	}

	//	STRING strLog;
	//	strLog.Printf( "[DB_RESP][%s] %d %d %s\r\n", ::Dispatcher::ITIME()->GetTime().GetCSTR(), i_pQuery->GetCode(), nResult, strUser.GetCSTR() );
	//	::Dispatcher::ICONSOLE()->Write( strLog );

	//	STRING strDbLog;
	//	strDbLog.Printf( "DB\t%s\t%s\t%d\t%s",
	//						::Dispatcher::ITIME()->GetTime().GetCSTR(),
	//						"SP_GS_MB_USER_NICK_U",
	//						nResult,
	//						i_pQuery->GetLog().GetCSTR() );
	//	::Dispatcher::ICONSOLE()->Write( strDbLog );
	//	return;
	//}

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
							"SP_GS_MB_USER_NICK_U",
							nResult,
							i_pQuery->GetLog().GetCSTR() );
	::Dispatcher::ICONSOLE()->Write( strDbLog );

	switch( nResult )
	{
	case 0:
		if( IPlayer::ICONTAINER()->Insert( i_pMemento->GetPlayer() ) == FALSE )
		{
			pRep->m_byResult	= static_cast<BYTE>( L::SN_Nick::E_RESULT::FAIL_LOGINED );
			ICASTER()->Cast( i_pMemento->GetPlayer(), pRep );
			i_pMemento->SetState( NONENICK() );
		}
		this->castCharacter( i_pMemento );
		i_pMemento->SetState( NONECHARACTER() );
		break;

	case -11:
		pRep->m_byResult	= static_cast<BYTE>( L::SN_Nick::E_RESULT::FAIL_DB1 );
		ICASTER()->Cast( i_pMemento->GetPlayer(), pRep );
		i_pMemento->SetState( NONENICK() );
		break;

	case -12:
		pRep->m_byResult	= static_cast<BYTE>( L::SN_Nick::E_RESULT::EXIST );
		ICASTER()->Cast( i_pMemento->GetPlayer(), pRep );
		i_pMemento->SetState( NONENICK() );
		break;

	case -13:
		pRep->m_byResult	= static_cast<BYTE>( L::SN_Nick::E_RESULT::FAIL_DB2 );
		ICASTER()->Cast( i_pMemento->GetPlayer(), pRep );
		i_pMemento->SetState( NONENICK() );
		break;

	case -14:
		pRep->m_byResult	= static_cast<BYTE>( L::SN_Nick::E_RESULT::BAD_NAME );
		ICASTER()->Cast( i_pMemento->GetPlayer(), pRep );
		i_pMemento->SetState( NONENICK() );
		break;
		
	case -21:
		pRep->m_byResult	= static_cast<BYTE>( L::SN_Nick::E_RESULT::FAIL_DB3 );
		ICASTER()->Cast( i_pMemento->GetPlayer(), pRep );
		i_pMemento->SetState( NONENICK() );
		break;

	default:
		DEBUG_BREAK;
		pRep->m_byResult	= static_cast<SHORT>( L::SN_Nick::E_RESULT::FAIL_DB1 );
		ICASTER()->Cast( i_pMemento->GetPlayer(), pRep );
		i_pMemento->SetState( NONENICK() );
		break;
	}

}

inline void
Nick::castCharacter( Memento * i_pMemento )
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