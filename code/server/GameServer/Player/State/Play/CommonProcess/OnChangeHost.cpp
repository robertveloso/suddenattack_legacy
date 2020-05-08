#include "StdAfx.h"

#include "./OnChangeHost.h"

#include "../../../../ICaster.h"
#include "../../../../IRoom.h"
#include "../../../../Room/IProcess.h"
#include "../../../../../Share/Packet/Builder.h"
#include "../../../../../Share/Packet/Service/Game.h"
#include "../../../../../Share/Packet/Service/Play.h"

#include "../../../../../Dispatcher/IConsole.h"
#include "../../../../../Dispatcher/ITime.h"

namespace GameServer {		namespace Player {			namespace State {
namespace Play {			namespace CommonProcess {

void
OnChangeHost::Execute( Memento * i_pMemento, IRoom * i_pRoom )
{
	typedef					::Packet::Service::Game::SS_ChangeHost
							SS_CHANGEHOST;
	
	i_pRoom->SetPause( TRUE );

	IPlayer * pPlayer		= i_pRoom->GetFirstPlayer();
	while( pPlayer != NULL )
	{
		if( pPlayer->GetState() == IPlayer::E_STATE::PLAY )
		{
			typedef			::Packet::Service::Game::SN_Pause
							SN_PAUSE;
			SN_PAUSE * pNoti = ::Packet::Builder::Build<SN_PAUSE>();
			ICASTER()->Cast( pPlayer, pNoti );

			pPlayer->GetMemento()->SetReplayReady( TRUE );
		}

		ICASTER()->Signal( pPlayer, SS_CHANGEHOST::s_nCode );
		pPlayer				= i_pRoom->GetNextPlayer();
	}

	IPlayer * pTempSuperPeer = NULL;
	if( i_pRoom->IsNat() == TRUE )
	{
		pTempSuperPeer		= OnChangeHost::findSuperPeerOnNat( i_pRoom );
	}
	else
	{
		pTempSuperPeer		= OnChangeHost::findSuperPeer( i_pRoom );
	}

	if( pTempSuperPeer == NULL )
	{
		STRING strLog;
		strLog.Printf( "Host\t%s\t%s\tR(%d)\r\n",
					::Dispatcher::ITIME()->GetTime().GetCSTR(),
					"NotFound",
					i_pRoom->GetIndex() );
		::Dispatcher::ICONSOLE()->Write( strLog );

		Room::IPROCESS()->CancelRound( i_pRoom );
		return;
	}

	i_pRoom->SetSuperPeer( NULL );
	i_pRoom->SetTempSuperPeer( pTempSuperPeer );

	STRING strLog;
	strLog.Printf( "Host\t%s\t%s\tR(%d)\t%s\t%s\r\n",
					::Dispatcher::ITIME()->GetTime().GetCSTR(),
					"Change",
					i_pRoom->GetIndex(),
					pTempSuperPeer->GetMemento()->GetStock().GetNick(),
					pTempSuperPeer->GetMemento()->GetClientIP().GetCSTR() );
	::Dispatcher::ICONSOLE()->Write( strLog );

	typedef					::Packet::Service::Play::SN_Host
							SN_HOST;
	SN_HOST * pHost = ::Packet::Builder::Build<SN_HOST>();
	ICASTER()->Cast( pTempSuperPeer, pHost );

}

inline IPlayer *
OnChangeHost::findSuperPeer( IRoom * i_pRoom )
{
	IPlayer * pPlayer		= i_pRoom->GetFirstPlayer();
	while( pPlayer != NULL )
	{
		if( pPlayer->GetState() >= IPlayer::E_STATE::PLAY_READY &&
			pPlayer->GetMemento()->CanBeSuperPeer() == TRUE &&
			pPlayer->GetMemento()->GetClientIP() == pPlayer->GetMemento()->GetIP() )
		{
			return pPlayer;
		}

		pPlayer				= i_pRoom->GetNextPlayer();
	}

	return NULL;
}

inline IPlayer *
OnChangeHost::findSuperPeerOnNat( IRoom * i_pRoom )
{
	IPlayer * pPlayer		= i_pRoom->GetFirstPlayer();
	while( pPlayer != NULL )
	{
		if( pPlayer->GetState() >= IPlayer::E_STATE::PLAY_READY &&
			pPlayer->GetMemento()->CanBeSuperPeer() == TRUE )
		{
			return pPlayer;
		}

		pPlayer				= i_pRoom->GetNextPlayer();
	}

	return NULL;
}

} /* CommonProcess */		} /* Play */
} /* State */				} /* Player */				} /* GameServer */