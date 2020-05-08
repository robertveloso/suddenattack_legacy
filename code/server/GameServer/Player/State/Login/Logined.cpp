#include "StdAfx.h"

#include "./Logined.h"
#include "./Closing.h"
#include "./Ready.h"
#include "../Fin.h"
#include "../../Memento.h"
#include "../../../Query.h"
#include "../../../ICaster.h"
#include "../../../IChannel.h"
#include "../../../IServer.h"
#include "../../../Player/IResult.h"
#include "../../../Server/IRank.h"
#include "../../../../Share/Packet/Builder.h"
#include "../../../../Share/Packet/Login.h"
#include "../../../../Share/Packet/Service/Channel.h"
#include "../../../../Dispatcher/IQuery.h"
#include "../../../../Dispatcher/IConsole.h"
#include "../../../../Dispatcher/ITime.h"

namespace GameServer {		namespace Player {			namespace State {
namespace Login {

Logined::Logined()
{
	this->install();
}

Logined::~Logined()
{
	this->uninstall();
}

CBOOL
Logined::OnPacket( Memento * i_pMemento, PACKET * i_pPacket )
{
//	i_pMemento->SetState( CLOSING() );
	return TRUE;
}

void
Logined::OnClose( Memento * i_pMemento )
{
	i_pMemento->GetPlayer()->LogoutProcess();
	i_pMemento->SetState( FIN() );
}

void
Logined::OnResponse( Memento * i_pMemento, IQUERY * i_pQuery )
{
	typedef					Query::Result
							RESULT;
	Assert( i_pQuery->GetCode() == RESULT::E_CODE::REQ_READ );

	typedef					::Packet::Login::SN_GameResultFail
							SN_FAIL;

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
	strDbLog.Printf( "DB\t%s\t%s\t%d\t%s",
							::Dispatcher::ITIME()->GetTime().GetCSTR(),
							"SP_GS_GM_RESULT_R",
							nResult,
							i_pQuery->GetLog().GetCSTR() );
	::Dispatcher::ICONSOLE()->Write( strDbLog );

	SN_FAIL * pFail			= NULL;
	switch( nResult )
	{
	case 0 :
		break;

	case -1 :
		pFail		= ::Packet::Builder::Build<SN_FAIL>();
		pFail->m_byResult	= static_cast<BYTE>( SN_FAIL::E_RESULT::FAIL_DB1 );
		ICASTER()->Cast( i_pMemento->GetPlayer(), pFail );

		i_pMemento->SetState( CLOSING() );
		return;

	case -2:
		pFail		= ::Packet::Builder::Build<SN_FAIL>();
		pFail->m_byResult	= static_cast<BYTE>( SN_FAIL::E_RESULT::FAIL_DB2 );
		ICASTER()->Cast( i_pMemento->GetPlayer(), pFail );

		i_pMemento->SetState( CLOSING() );
		return;
	}

	this->setUserResult( i_pMemento, i_pQuery );
	this->castUserResult( i_pMemento );
	this->castChannel( i_pMemento );

	this->castRank( i_pMemento );

	i_pMemento->SetState( READY() );

}

inline void
Logined::setUserResult( Memento * i_pMemento, IQUERY * i_pQuery )
{
	typedef					::LogicObject::Player::Result
							RESULT;

	RESULT * pResult		= i_pMemento->GetPlayer()->GetResult()->GetTotalResult();

	try
	{
		i_pMemento->GetStock().m_byGrade = IQUERY::IRESPONSER()->ReadInt( i_pQuery, STRING( "@o_level_no" ) );
		pResult->m_llExp		= IQUERY::IRESPONSER()->ReadInt( i_pQuery, STRING( "@o_user_exp" ) ); 
		pResult->m_llMoney		= IQUERY::IRESPONSER()->ReadInt( i_pQuery, STRING( "@o_user_money" ) ); 
		pResult->m_nKill		= IQUERY::IRESPONSER()->ReadInt( i_pQuery, STRING( "@o_kill_cnt" ) ); 
		pResult->m_nDeath		= IQUERY::IRESPONSER()->ReadInt( i_pQuery, STRING( "@o_death_cnt" ) ); 
		pResult->m_nHeadKill	= IQUERY::IRESPONSER()->ReadInt( i_pQuery, STRING( "@o_h_kill_cnt" ) ); 
		pResult->m_nHeadDead	= IQUERY::IRESPONSER()->ReadInt( i_pQuery, STRING( "@o_h_death_cnt" ) );
		pResult->m_nWin			= IQUERY::IRESPONSER()->ReadInt( i_pQuery, STRING( "@o_win_cnt" ) );
		pResult->m_nLose		= IQUERY::IRESPONSER()->ReadInt( i_pQuery, STRING( "@o_lose_cnt" ) );
		pResult->m_nDraw		= IQUERY::IRESPONSER()->ReadInt( i_pQuery, STRING( "@o_draw_cnt" ) );
		pResult->m_nMaxKill		= IQUERY::IRESPONSER()->ReadInt( i_pQuery, STRING( "@o_max_kill_cnt" ) );
		pResult->m_nMaxHeadKill	= IQUERY::IRESPONSER()->ReadInt( i_pQuery, STRING( "@o_max_h_kill_cnt" ) );
		pResult->m_nRank		= IQUERY::IRESPONSER()->ReadInt( i_pQuery, STRING( "@o_user_rank" ) );
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

//		nResult				= -99;
	}
	catch( ... )
	{
//		nResult				= -99;
	}

}

inline void
Logined::castChannel( Memento * i_pMemento )
{
	namespace				C = ::Packet::Service::Channel;

	C::SN_Info * pInfo		= ::Packet::Builder::Build<C::SN_Info>();
	pInfo->m_byChannelCount	= ISERVER()->GetChannelCount();
	ICASTER()->Cast( i_pMemento->GetPlayer(), pInfo );

	IChannel * pChannel		= ISERVER()->GetFirstChannel();
	while( pChannel != NULL )
	{
		C::SN_Enter * pNoti		= ::Packet::Builder::Build<C::SN_Enter>();
		pNoti->m_byIndex		= pChannel->GetIndex();
		pNoti->m_nMaxUser		= pChannel->GetMaxUser();
		pNoti->m_nCurrentUser	= pChannel->GetCurrentUser();
		ICASTER()->Cast( i_pMemento->GetPlayer(), pNoti );
		
		pChannel				= ISERVER()->GetNextChannel();
	}
}

inline void
Logined::castUserResult( Memento * i_pMemento )
{
	typedef					::Packet::Login::SN_MyProperty
							SN_MYPROPERTY;
	typedef					::LogicObject::Player::Result
							RESULT;
	typedef					::LogicObject::Player::Type::E_TEAM
							E_TEAM;

	SN_MYPROPERTY * pNoti	= ::Packet::Builder::Build<SN_MYPROPERTY>();

	pNoti->m_stResult		= * i_pMemento->GetPlayer()->GetResult()->GetTotalResult();
	pNoti->m_stStock		= i_pMemento->GetStock();
	pNoti->m_stRed			= i_pMemento->GetCharacters()[ E_TEAM::RED ];
	pNoti->m_stBlue			= i_pMemento->GetCharacters()[ E_TEAM::BLUE ];
	pNoti->m_byAge			= i_pMemento->GetAge();
	pNoti->m_bRealName		= i_pMemento->IsRealName();

	pNoti->Set( ISERVER()->GetName().GetCSTR(), i_pMemento->GetConnectID());

	STRING strLog;
	strLog.Printf( "Dev\t%s\t%s\tU(%d)\t%s\r\n",
					::Dispatcher::ITIME()->GetTime().GetCSTR(),
					"Nick",
					i_pMemento->GetPlayer()->GetIndex(),
					i_pMemento->GetStock().GetNick() );
	::Dispatcher::ICONSOLE()->Write( strLog );

	ICASTER()->Cast( i_pMemento->GetPlayer(), pNoti );
}

inline void
Logined::castRank( Memento * i_pMemento )
{
	typedef					::Packet::Login::SN_Ranking
							SN_RANKING;

	CSTRING * pRank			= ISERVER()->GetRank()->GetRank();
	if( pRank == NULL )
	{
		return;
	}

	CSTRING & strRank		= * pRank;

	SN_RANKING * pNoti		= ::Packet::Builder::Build<SN_RANKING>();
	pNoti->SetText( strRank.GetCSTR() );
	
	ICASTER()->Cast( i_pMemento->GetPlayer(), pNoti );
}

} /* Login */
} /* State */				} /* Player */				} /* GameServer */
