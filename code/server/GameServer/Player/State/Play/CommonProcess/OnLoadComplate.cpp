#include "StdAfx.h"

#include "./OnLoadComplate.h"

#include "../../../../IRoom.h"
#include "../../../../Room/IResult.h"
#include "../../../../Player/IResult.h"
#include "../../../../../Share/Packet/Service/Game.h"

#include "../../../../../Dispatcher/IConsole.h"

namespace GameServer {		namespace Player {			namespace State {
namespace Play {			namespace CommonProcess {

void
OnLoadComplate::Execute( Memento * i_pMemento, PACKET * i_pPacket )
{
	typedef					::Packet::Service::Game::CN_LoadComplate
							CN_LOADCOMPLATE;

	Assert( i_pPacket->GetCommand() == CN_LOADCOMPLATE::s_nCode );

	i_pMemento->SetLoading( TRUE );

	i_pMemento->SetReplayReady( FALSE );

}

} /* CommonProcess */		} /* Play */
} /* State */				} /* Player */				} /* GameServer */