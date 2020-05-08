#include "StdAfx.h"

#include "./OnResult.h"

#include "../IServer.h"
#include "../IPlayer.h"
#include "../Query.h"
#include "../Player/IResult.h"
#include "../Player/Memento.h"
#include "../../Dispatcher/ITime.h"
#include "../../Dispatcher/IConsole.h"


namespace GameServer {		namespace NoneResponser {

OnResult::OnResult()
{
}

OnResult::~OnResult()
{
}

void
OnResult::OnResponse( IQUERY * i_pQuery )
{
	typedef					Query::Result
							RESULT;
	Assert( i_pQuery->GetCode() == RESULT::E_CODE::REQ_RECORD );

	INT nResult				= -99;
	typedef					String< ::LogicObject::Player::Type::s_nUserNo >
							USERNO;
	USERNO					strUserNo;
	try
	{
		strUserNo			= USERNO( IQUERY::IRESPONSER()->ReadString( i_pQuery, STRING( "@i_user_no" ) ).GetCSTR() );
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
							"SP_GS_GM_RESULT_U",
							nResult,
							i_pQuery->GetLog().GetCSTR() );
	::Dispatcher::ICONSOLE()->Write( strDbLog );

	if( nResult != 0 )
	{
		return;
	}

	IPlayer * pPlayer		= IPlayer::ICONTAINER()->FindUserNo( strUserNo );
	if( pPlayer == NULL )
	{
		return;
	}

	try
	{
		namespace				P = ::LogicObject::Player;
		P::Result * pResult		= pPlayer->GetResult()->GetTotalResult();
		pPlayer->GetMemento()->GetStock().m_byGrade = IQUERY::IRESPONSER()->ReadInt( i_pQuery, STRING( "@o_level_no" ) );

		BYTE byGrade = IQUERY::IRESPONSER()->ReadInt( i_pQuery, STRING( "@o_level_no" ) );
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
	}
	catch( ... )
	{
	}
}

} /* NoneResponser */		} /* GameServer */