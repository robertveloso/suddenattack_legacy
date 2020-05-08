#include "StdAfx.h"

#include "./OnRelay.h"

#include "../../../../IRoom.h"
#include "../../../../ICaster.h"
#include "../../../../../Share/Packet/Service/Game.h"

namespace GameServer {		namespace Player {			namespace State {
namespace Play {			namespace CommonProcess {

void
OnRelay::Execute( Memento * i_pMemento, PACKET * i_pPacket )
{
	IPlayer * pPlayer		= i_pMemento->GetRoom()->GetFirstPlayer();
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

		ICASTER()->Cast( pPlayer, i_pPacket );
		pPlayer				= i_pMemento->GetRoom()->GetNextPlayer();
	}
}

} /* CommonProcess */		} /* Play */
} /* State */				} /* Player */				} /* GameServer */