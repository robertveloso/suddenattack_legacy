#include "StdAfx.h"

#include "./OnRank.h"

#include "../IChannel.h"
#include "../IServer.h"
#include "../ICaster.h"
#include "../Query.h"
#include "../Server/IRank.h"

#include "../../Share/Packet/Login.h"
#include "../../Share/Packet/Builder.h"

#include "../../Dispatcher/ITime.h"
#include "../../Dispatcher/IConsole.h"


namespace GameServer {		namespace NoneResponser {

OnRank::OnRank()
{
}

OnRank::~OnRank()
{
}

void
OnRank::OnResponse( IQUERY * i_pQuery )
{
	typedef					Query::Server
							SERVER;
	Assert( i_pQuery->GetCode() == SERVER::E_CODE::REQ_RANK );

	INT nResult				= -99;
	STRING strRank;
	try
	{
		nResult				= IQUERY::IRESPONSER()->ReadInt( i_pQuery, STRING( "@o_sp_rtn" ) );
		strRank				= IQUERY::IRESPONSER()->ReadString( i_pQuery, STRING( "@o_result_str" ) );

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
	strDbLog.Printf( "DB\t%s\t%s\t%d\n%s\r\n",
							::Dispatcher::ITIME()->GetTime().GetCSTR(),
							"SP_GS_GM_RANK_R",
							nResult,
							strRank.GetCSTR()
							 );
	::Dispatcher::ICONSOLE()->Write( strDbLog );
//	::Dispatcher::ICONSOLE()->Output( strDbLog );

	if( nResult != 0 )
	{
		return;
	}

	OnRank::castRanking( strRank );

	ISERVER()->GetRank()->SetRank( strRank );
}

inline void
OnRank::castRanking( CSTRING & strRank )
{
	CSTRING * pRankOld		= ISERVER()->GetRank()->GetRank();
	if( pRankOld != NULL )
	{
		CSTRING & strRankOld = * pRankOld;
		if( strRankOld == strRank )
		{
			return;
		}
	}

	typedef					::Packet::Login::SN_Ranking
							SN_RANKING;
	SN_RANKING * pNoti		= ::Packet::Builder::Build<SN_RANKING>();
	pNoti->SetText( strRank.GetCSTR() );

	IChannel * pChannel		= ISERVER()->GetFirstChannel();
	while( pChannel != NULL )
	{
		IRoom * pRoom		= pChannel->GetFirstRoom();
		while( pRoom != NULL )
		{
			ICASTER()->Cast( pRoom, pNoti );

			pRoom			= pChannel->GetNextRoom();
		}
		pChannel			= ISERVER()->GetNextChannel();
	}
}

} /* NoneResponser */		} /* GameServer */
