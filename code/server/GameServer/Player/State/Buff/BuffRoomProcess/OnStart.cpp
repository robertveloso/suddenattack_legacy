#include "StdAfx.h"

#include "./OnStart.h"

#include "../../../Memento.h"
#include "../../../../Query.h"
#include "../../../../IRoom.h"
#include "../../../../IServer.h"
#include "../../../../ICaster.h"
#include "../../../../IChannel.h"
#include "../../../../Room/IResult.h"

#include "../../../../../Share/Packet/Builder.h"
#include "../../../../../Share/Packet/Service/Player.h"
#include "../../../../../Share/Packet/Service/Play.h"
#include "../../../../../Share/Packet/Service/Room.h"
#include "../../../../../Dispatcher/IQuery.h"
#include "../../../../../Dispatcher/IConsole.h"
#include "../../../../../Dispatcher/ITime.h"

namespace GameServer {		namespace Player {			namespace State {
namespace Buff {			namespace BuffRoomProcess {

void
OnStart::Execute( Memento * i_pMemento, PACKET * i_pPacket )
{
	typedef					::Packet::Service::Play::CN_Start
							CN_START;
	Assert( i_pPacket->GetCommand() == CN_START::s_nCode );

	if( i_pMemento->GetRoom()->GetCaptain() != i_pMemento->GetPlayer() )
	{
		::Dispatcher::ICONSOLE()->Output( STRING( "WARNNING : OnStart 只能由caption发起\n" ) );
		return;
	}
	if( i_pMemento->GetRoom()->GetOption().IsPlaying() == TRUE )
	{
		::Dispatcher::ICONSOLE()->Output( STRING( "WARNNING : OnStart 已经是playing状态了\n" ) );
		return;
	}

	i_pMemento->SetReady( TRUE );

	IPlayer * pSuperPeer	= OnStart::findSuperPeer( i_pMemento->GetRoom() );

	typedef					::Packet::Service::Play::SN_StartFail::E_RESULT
							E_RESULT;
	if( pSuperPeer == NULL )
	{
		OnStart::castFail( i_pMemento, E_RESULT::NO_HOST );
		return;
	}

	if( OnStart::isRunnable( i_pMemento, i_pMemento->GetRoom() ) == FALSE )
	{
		OnStart::castFail( i_pMemento, E_RESULT::ALONE );
		return;
	}

	OnStart::successProcess( i_pMemento, i_pMemento->GetRoom() );

	STRING strLog;
	strLog.Printf( "Host\t%s\t%s\tR(%d)\t%s\t%s\r\n",
					::Dispatcher::ITIME()->GetTime().GetCSTR(),
					"Start",
					i_pMemento->GetRoom()->GetIndex(),
					pSuperPeer->GetMemento()->GetStock().GetNick(),
					pSuperPeer->GetMemento()->GetClientIP().GetCSTR() );
	::Dispatcher::ICONSOLE()->Write( strLog );

	i_pMemento->GetRoom()->SetSuperPeer( NULL );
	i_pMemento->GetRoom()->SetTempSuperPeer( pSuperPeer );
	OnStart::castSuperPeer( pSuperPeer );


	try
	{
		typedef					::Dispatcher::IQuery
								IQUERY;
		typedef					Query::Result
								RESULT;

		INT nInt				= -99;
		IQUERY * pQuery			= IQUERY::IFACTORY()->Create( RESULT::E_CODE::REQ_GAME );

		BYTE bySecret			= i_pMemento->GetRoom()->GetOption().IsPassword();
		BYTE byUserCount		= i_pMemento->GetRoom()->GetOption().GetTeamMax();
		BYTE byMapCode			= i_pMemento->GetRoom()->GetOption().GetMapIndex();
		BYTE byTimeLimit		= i_pMemento->GetRoom()->GetOption().GetTime();;
		BYTE byKillLimit		= i_pMemento->GetRoom()->GetOption().GetKillLimit();;
		BYTE byRoundLimit		= i_pMemento->GetRoom()->GetOption().GetRoundLimit();;
		BYTE byJoinInBattle		= i_pMemento->GetRoom()->GetOption().CanRunnableInBattle();;
		BYTE byTeamBalance		= i_pMemento->GetRoom()->GetOption().IsTeamBalance();;

		pQuery->GetLog().Printf( "%s\tR(%d)\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%s\t%s\r\n",
					ISERVER()->GetID().GetCSTR(),
					i_pMemento->GetRoom()->GetIndex(),
					bySecret,
					byUserCount,
					byMapCode,
					byTimeLimit,
					byKillLimit,
					byRoundLimit,
					byJoinInBattle,
					byTeamBalance,
					i_pMemento->GetUserNo().GetCSTR(),
					i_pMemento->GetIP().GetCSTR() );

		IQUERY::IBUILDER()->Build( pQuery, STRING( "SP_GS_GM_ROOM_LOG_C" ) );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_ServerID" ), ISERVER()->GetID(), TRUE, 4 );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_room_no" ), i_pMemento->GetRoom()->GetIndex(), TRUE );
		IQUERY::IBUILDER()->BuildTiny( pQuery, STRING( "@i_room_secret_flag" ), bySecret, TRUE );
		IQUERY::IBUILDER()->BuildTiny( pQuery, STRING( "@i_join_user_count" ), byUserCount, TRUE );
		IQUERY::IBUILDER()->BuildTiny( pQuery, STRING( "@i_map_code" ), byMapCode, TRUE );
		IQUERY::IBUILDER()->BuildTiny( pQuery, STRING( "@i_time_limit" ), byTimeLimit, TRUE );
		IQUERY::IBUILDER()->BuildTiny( pQuery, STRING( "@i_kill_limit" ), byKillLimit, TRUE );
		IQUERY::IBUILDER()->BuildTiny( pQuery, STRING( "@i_round_limit" ), byRoundLimit, TRUE );
		IQUERY::IBUILDER()->BuildTiny( pQuery, STRING( "@i_join_in_battle" ), byJoinInBattle, TRUE );
		IQUERY::IBUILDER()->BuildTiny( pQuery, STRING( "@i_team_balance" ), byTeamBalance, TRUE );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_host_user_no" ), STRING( i_pMemento->GetUserNo().GetCSTR() ), TRUE, 14 );
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@i_host_ip_addr" ), STRING( i_pMemento->GetIP().GetCSTR() ), TRUE, 15 );
		
		IQUERY::IBUILDER()->Build( pQuery, STRING( "@o_sp_rtn" ), nInt, FALSE );
		IQUERY::IREQUESTER()->Regist( pQuery );
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

inline IPlayer *
OnStart::findSuperPeer( IRoom * i_pRoom )
{
	if( i_pRoom->GetCaptain()->GetMemento()->CanBeSuperPeer() == TRUE &&
		i_pRoom->GetCaptain()->GetMemento()->GetIP() == i_pRoom->GetCaptain()->GetMemento()->GetClientIP() )
	{
		i_pRoom->SetNat( FALSE );
		return i_pRoom->GetCaptain();
	}

	IPlayer * pPlayer		= i_pRoom->GetFirstPlayer();
	while( pPlayer != NULL )
	{
		if( pPlayer->GetState() != IPlayer::E_STATE::READY )
		{
			pPlayer			= i_pRoom->GetNextPlayer();
			continue;
		}
		if( pPlayer->GetMemento()->CanBeSuperPeer() == TRUE &&
			pPlayer->GetMemento()->GetIP() == pPlayer->GetMemento()->GetClientIP() )
		{
			i_pRoom->SetNat( FALSE );
			return pPlayer;
		}

		pPlayer				= i_pRoom->GetNextPlayer();
	}

	pPlayer					= OnStart::findNatSuperPeer( i_pRoom );

	return pPlayer;
}

inline IPlayer *
OnStart::findNatSuperPeer( IRoom * i_pRoom )
{
	if( i_pRoom->GetCaptain()->GetMemento()->CanBeSuperPeer() == FALSE )
	{
		return NULL;
	}

	INT nNat				= 0;
	IPlayer * pPlayer		= i_pRoom->GetFirstPlayer();
	while( pPlayer != NULL )
	{
		if( pPlayer->GetState() != IPlayer::E_STATE::READY )
		{
			pPlayer			= i_pRoom->GetNextPlayer();
			continue;
		}
		
		if( i_pRoom->GetCaptain()->GetMemento()->GetIP() == pPlayer->GetMemento()->GetIP() )
		{
			nNat ++;
		}

		pPlayer				= i_pRoom->GetNextPlayer();
	}

	if( nNat < 2 )
	{
		return NULL;
	}

	i_pRoom->SetNat( TRUE );
	return i_pRoom->GetCaptain();
}

inline CBOOL
OnStart::isRunnable( Memento * i_pMemento, IRoom * i_pRoom )
{
#ifdef _DEVDEBUG
	return TRUE;
#endif

	typedef					::LogicObject::Player::Type::E_TEAM
							E_TEAM;

	INT nTotal				= 0;
	INT nRed				= 0;
	INT nBlue				= 0;
	IPlayer * pPlayer		= i_pRoom->GetFirstPlayer();
	while( pPlayer != NULL )
	{
		if( i_pRoom->IsNat() == TRUE )
		{
			if( pPlayer->GetMemento()->GetIP() != i_pRoom->GetCaptain()->GetMemento()->GetIP() )
			{
				pPlayer		= i_pRoom->GetNextPlayer();
				continue;
			}
		}

		if( pPlayer->GetMemento()->IsReady() == FALSE )
		{
			pPlayer			= i_pRoom->GetNextPlayer();
			continue;
		}

		nTotal ++;

		if( pPlayer->GetMemento()->GetTeam() == E_TEAM::RED )
		{
			nRed ++;
		}
		else
		{
			nBlue ++;
		}

		pPlayer				= i_pRoom->GetNextPlayer();
	}

	if( nTotal == 1 )
	{
		return FALSE;
	}

	if( nRed == 0 || nBlue == 0 )
	{
		if( nTotal > 1 && i_pRoom->GetOption().IsTeamBalance() == TRUE )
		{
			return TRUE;
		}

		return FALSE;
	}

	return TRUE;
}

inline void
OnStart::successProcess( Memento * i_pMemento, IRoom * i_pRoom )
{
	typedef					::Packet::Service::Play::SS_Start
							SS_START;

	i_pRoom->GetResult()->Reset();

	OnStart::setTeamBalance( i_pRoom );

	IPlayer * pPlayer		= i_pRoom->GetFirstPlayer();
	while( pPlayer != NULL )
	{
		if( i_pRoom->IsNat() == TRUE )
		{
			if( pPlayer->GetMemento()->GetIP() != i_pRoom->GetCaptain()->GetMemento()->GetIP() )
			{
				typedef				::Packet::Service::Room::SN_JoinPlayFail
									SN_PLAYFAIL;
				SN_PLAYFAIL * pNoti	= ::Packet::Builder::Build<SN_PLAYFAIL>();
				ICASTER()->Cast( pPlayer, pNoti );
				
				pPlayer->GetMemento()->SetReady( FALSE );
			}
		}

		pPlayer				= i_pRoom->GetNextPlayer();
	}

	pPlayer					= i_pRoom->GetFirstPlayer();
	while( pPlayer != NULL )
	{
		ICASTER()->Signal( pPlayer, SS_START::s_nCode );

		pPlayer				= i_pRoom->GetNextPlayer();
	}

	i_pRoom->GetOption().SetPlaying( TRUE );
	i_pRoom->SetPause( FALSE );

	i_pRoom->SetBombReady( FALSE );

	typedef					::Packet::Service::Room::SN_Play
							SN_PLAY;
	SN_PLAY * pNoti			= ::Packet::Builder::Build<SN_PLAY>();
	pNoti->m_nRoomIndex		= i_pRoom->GetIndex();
	ICASTER()->Cast( i_pMemento->GetChannel()->GetLobby(), pNoti );
}

inline void
OnStart::setTeamBalance( IRoom * i_pRoom )
{
	if( i_pRoom->GetOption().IsTeamBalance() == FALSE )
	{
		return;
	}

	INT nPlayRed			= 0;
	INT nPlayBlue			= 0;

	IPlayer * pPlayer		= i_pRoom->GetFirstPlayer();
	while( pPlayer != NULL )
	{
		if( pPlayer->GetMemento()->GetTeam() == IPlayer::E_TEAM::RED )
		{
			if( pPlayer->GetMemento()->IsReady() == TRUE )
			{
				nPlayRed ++;
			}
		}
		else
		{
			if( pPlayer->GetMemento()->IsReady() == TRUE )
			{
				nPlayBlue ++;
			}
		}
		pPlayer				= i_pRoom->GetNextPlayer();
	}

	namespace				P = ::Packet::Service::Player;

	if( ( nPlayRed - nPlayBlue ) > 1 )
	{
		INT nMove			= ( nPlayRed - nPlayBlue ) / 2;

		pPlayer				= i_pRoom->GetFirstPlayer();
		while( pPlayer != NULL )
		{
			if( pPlayer == i_pRoom->GetCaptain() )
			{
				pPlayer		= i_pRoom->GetNextPlayer();
				continue;
			}
			if( pPlayer->GetMemento()->GetTeam() == IPlayer::E_TEAM::BLUE )
			{
				pPlayer		= i_pRoom->GetNextPlayer();
				continue;
			}
			if( pPlayer->GetMemento()->IsReady() == FALSE )
			{
				pPlayer		= i_pRoom->GetNextPlayer();
				continue;
			}

			ICASTER()->Signal( pPlayer, P::SS_ChangeTeam::s_nCode );

			nMove --;

			if( nMove == 0 )
			{
				break;
			}

			pPlayer			= i_pRoom->GetNextPlayer();
		}
		return;
	}

	if( ( nPlayBlue - nPlayRed ) > 1 )
	{
		INT nMove			= ( nPlayBlue - nPlayRed ) / 2;

		pPlayer				= i_pRoom->GetFirstPlayer();
		while( pPlayer != NULL )
		{
			if( pPlayer == i_pRoom->GetCaptain() )
			{
				pPlayer		= i_pRoom->GetNextPlayer();
				continue;
			}
			if( pPlayer->GetMemento()->GetTeam() == IPlayer::E_TEAM::RED )
			{
				pPlayer		= i_pRoom->GetNextPlayer();
				continue;
			}
			if( pPlayer->GetMemento()->IsReady() == FALSE )
			{
				pPlayer		= i_pRoom->GetNextPlayer();
				continue;
			}

			ICASTER()->Signal( pPlayer, P::SS_ChangeTeam::s_nCode );

			nMove --;

			if( nMove == 0 )
			{
				break;
			}

			pPlayer			= i_pRoom->GetNextPlayer();
		}

	}
}

inline void
OnStart::castFail( Memento * i_pMemento, CBYTE i_byResult )
{
	typedef					::Packet::Service::Play::SN_StartFail
							SN_FAIL;

	SN_FAIL * pNoti			= ::Packet::Builder::Build<SN_FAIL>();
	pNoti->m_byResult		= i_byResult;

	ICASTER()->Cast( i_pMemento->GetPlayer(), pNoti );

}

inline void
OnStart::castSuperPeer( IPlayer * i_pPlayer )
{
	//typedef					::Packet::Service::Play::SN_Host
	//						SN_HOST;

	//SN_HOST * pNoti			= ::Packet::Builder::Build<SN_HOST>();

	//ICASTER()->Cast( i_pPlayer, pNoti );
}

} /* BuffRoomProcess */		} /* Buff */
} /* State */				} /* Player */				} /* GameServer */