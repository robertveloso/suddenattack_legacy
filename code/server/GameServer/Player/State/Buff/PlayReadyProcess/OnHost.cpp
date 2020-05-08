#include "StdAfx.h"

#include "./OnHost.h"

#include "../../../../IRoom.h"
#include "../../../../ICaster.h"
#include "../../../../IChannel.h"
#include "../../../../Room/IResult.h"

#include "../../../../../Share/Packet/Builder.h"
#include "../../../../../Share/Packet/Service/Play.h"
#include "../../../../../Share/Packet/Service/Game.h"

#include "../../../../../Share/LogicObject/Room/Option.h"

#include "../../../../../Dispatcher/IConsole.h"
#include "../../../../../Dispatcher/ITime.h"

namespace GameServer {		namespace Player {			namespace State {
namespace Buff {			namespace PlayReadyProcess {

void
OnHost::Execute( Memento * i_pMemento, PACKET * i_pPacket )
{
	typedef					::Packet::Service::Play::CN_Host
							CN_HOST;

	Assert( i_pPacket->GetCommand() == CN_HOST::s_nCode );

	CN_HOST * pReq			= static_cast<CN_HOST * >( i_pPacket );

	switch( pReq->m_byResult )
	{
	case CN_HOST::E_RESULT::SUCCESS :
		OnHost::successProcess( i_pMemento, pReq->m_nPort );
		break;

	case CN_HOST::E_RESULT::FAIL :
		OnHost::failProcess( i_pMemento );
		break;

	default :
		DEBUG_BREAK;
		break;
	}
}

inline void
OnHost::successProcess( Memento * i_pMemento, CWORD i_wPort )
{
	typedef					::LogicObject::Player::Type::E_STATE
							E_STATE;
	namespace				G = ::Packet::Service::Game;

	i_pMemento->GetRoom()->SetSuperPeer( i_pMemento->GetPlayer() );
	i_pMemento->SetPort( i_wPort );

	G::SN_NetLossRoom *
	pNotiRoom				= ::Packet::Builder::Build<G::SN_NetLossRoom>();
	pNotiRoom->m_byNetLoss	= i_pMemento->GetNetLoss();
	pNotiRoom->m_nRoomIndex	= i_pMemento->GetRoom()->GetIndex();
	ICASTER()->Cast( i_pMemento->GetChannel()->GetLobby(), pNotiRoom );

	STRING strLog;
	strLog.Printf( "Host\t%s\t%s\tR(%d)\t%s\t%s\t%d\r\n",
					::Dispatcher::ITIME()->GetTime().GetCSTR(),
					"Success",
					i_pMemento->GetRoom()->GetIndex(),
					i_pMemento->GetStock().GetNick(),
					i_pMemento->GetClientIP().GetCSTR(),
					i_pMemento->GetPort() );
	::Dispatcher::ICONSOLE()->Write( strLog );

	if( i_pMemento->GetRoom()->IsPause() == FALSE )
	{
		i_pMemento->GetRoom()->ResetStartTick();
		i_pMemento->GetRoom()->GetResult()->ResetRound();
	}

	IPlayer * pPlayer		= i_pMemento->GetRoom()->GetFirstPlayer();
	while( pPlayer != NULL )
	{
		if( pPlayer->GetState() != E_STATE::PLAY_READY )
		{
			pPlayer			= i_pMemento->GetRoom()->GetNextPlayer();
			continue;
		}
		G::SN_Play * pNoti	= ::Packet::Builder::Build<G::SN_Play>();
		pNoti->m_nHostIndex	= i_pMemento->GetPlayer()->GetIndex();
		pNoti->m_nPort		= i_pMemento->GetPort();
		if( i_pMemento->GetRoom()->IsPause() == FALSE )
		{
			pPlayer->GetMemento()->SetSpectator( i_pMemento->GetRoom()->IsSpectatorTick() );
		}

		pNoti->m_bSpectator	= ( pPlayer->GetMemento()->IsSpectator() == TRUE );
		pNoti->SetIP( i_pMemento->GetClientIP().GetCSTR() );
		ICASTER()->Cast( pPlayer, pNoti );

		ICASTER()->Signal( pPlayer, G::SS_Play::s_nCode );

		pPlayer				= i_pMemento->GetRoom()->GetNextPlayer();
	}

	i_pMemento->GetRoom()->SetPause( FALSE );

}

inline void
OnHost::failProcess( Memento * i_pMemento )
{
	STRING strLog;
	strLog.Printf( "Host\t%s\t%s\tR(%d)\t%s\t%s\r\n",
					::Dispatcher::ITIME()->GetTime().GetCSTR(),
					"Fail",
					i_pMemento->GetRoom()->GetIndex(),
					i_pMemento->GetStock().GetNick(),
					i_pMemento->GetClientIP().GetCSTR() );
	::Dispatcher::ICONSOLE()->Write( strLog );

	i_pMemento->DisableSuperPeer();

	IRoom * pRoom			= i_pMemento->GetRoom();
	IPlayer * pPlayer		= pRoom->GetFirstPlayer();
	while( pPlayer != NULL )
	{
		if( pPlayer->GetMemento()->CanBeSuperPeer() == TRUE &&
			pPlayer->GetMemento()->IsReady() == TRUE )
		{
			break;
		}

		pPlayer				= pRoom->GetNextPlayer();
	}

	if( pPlayer == NULL )
	{
		OnHost::cancelProcess( i_pMemento );
	}
	else
	{
		typedef				::Packet::Service::Play::SN_Host
							SN_HOST;
		SN_HOST * pNoti		= ::Packet::Builder::Build<SN_HOST>();
		ICASTER()->Cast( pPlayer, pNoti );
	}
}

inline void
OnHost::cancelProcess( Memento * i_pMemento )
{
	IRoom * pRoom			= i_pMemento->GetRoom();
	IPlayer * pPlayer		= pRoom->GetFirstPlayer();
	while( pPlayer != NULL )
	{
		if( pPlayer->GetState() != IPlayer::E_STATE::PLAY_READY )
		{
			pPlayer			= pRoom->GetNextPlayer();
			continue;
		}

		typedef				::Packet::Service::Play::SS_StartFail
							SS_FAIL;
		ICASTER()->Signal( pPlayer, SS_FAIL::s_nCode );

		pPlayer				= pRoom->GetNextPlayer();
	}

	pRoom->GetOption().SetPlaying( FALSE );
}

} /* PlayReadyProcess */	} /* Buff */
} /* State */				} /* Player */				} /* GameServer */