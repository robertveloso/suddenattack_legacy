#include "StdAfx.h"

#include "./OnNetLoss.h"

#include "../../../../IRoom.h"
#include "../../../../IChannel.h"
#include "../../../../ICaster.h"

#include "../../../../../Share/Packet/Service/Game.h"
#include "../../../../../Share/Packet/Builder.h"

namespace GameServer {		namespace Player {			namespace State {
namespace Play {			namespace CommonProcess {

void
OnNetLoss::Execute( Memento * i_pMemento, PACKET * i_pPacket )
{
	namespace				G = ::Packet::Service::Game;
	G::CN_NetLoss * pReq	= static_cast<G::CN_NetLoss *>( i_pPacket );

	i_pMemento->SetNetLoss( pReq->m_byNetLoss );
	
	G::SN_NetLossPlayer *
	pNotiPlayer					= ::Packet::Builder::Build<G::SN_NetLossPlayer>();
	pNotiPlayer->m_byNetLoss	= i_pMemento->GetNetLoss();
	pNotiPlayer->m_nPlayerIndex	= i_pMemento->GetPlayer()->GetIndex();
	IPlayer * pPlayer			= i_pMemento->GetRoom()->GetFirstPlayer();
	while( pPlayer != NULL )
	{
		if( pPlayer == i_pMemento->GetPlayer() )
		{
			pPlayer			= i_pMemento->GetRoom()->GetNextPlayer();
			continue;
		}
		if( pPlayer->GetState() < IPlayer::E_STATE::PLAY_READY )
		{
			pPlayer			= i_pMemento->GetRoom()->GetNextPlayer();
			continue;
		}
		ICASTER()->Cast( pPlayer, pNotiPlayer );

		pPlayer				= i_pMemento->GetRoom()->GetNextPlayer();
	}

	if( i_pMemento->GetPlayer() != i_pMemento->GetRoom()->GetSuperPeer() )
	{
		return;
	}

	G::SN_NetLossRoom *
	pNotiRoom				= ::Packet::Builder::Build<G::SN_NetLossRoom>();
	pNotiRoom->m_byNetLoss	= i_pMemento->GetNetLoss();
	pNotiRoom->m_nRoomIndex	= i_pMemento->GetRoom()->GetIndex();
	ICASTER()->Cast( i_pMemento->GetChannel()->GetLobby(), pNotiRoom );
}

} /* CommonProcess */		} /* Play */
} /* State */				} /* Player */				} /* GameServer */