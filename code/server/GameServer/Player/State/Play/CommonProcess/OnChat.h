#pragma once

#include "../../../Memento.h"

namespace Application {		namespace Packets {
struct CPacket;
} /* Packets */				} /* Application */

namespace GameServer {		namespace Player {			namespace State {
namespace Play {			namespace CommonProcess {

class OnChat
{
public:
	typedef					::Application::Packets::CPacket
							PACKET;

public:
	static void				Execute( Memento * i_pMemento, PACKET * i_pPacket );

private:
	static void				castAll( Memento * i_pMemento, PACKET * i_pPacket );
	static void				castTeam( Memento * i_pMemento, PACKET * i_pPacket );
	static void				castGhostAll( Memento * i_pMemento, PACKET * i_pPacket );
	static void				castGhostTeam( Memento * i_pMemento, PACKET * i_pPacket );

};

} /* CommonProcess */		} /* Play */
} /* State */				} /* Player */				} /* GameServer */