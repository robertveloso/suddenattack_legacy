#pragma once

#include "../../../Memento.h"

namespace Application {		namespace Packets {
struct CPacket;
} /* Packets */				} /* Application */

namespace GameServer {		namespace Player {			namespace State {
namespace Play {			namespace CommonProcess {

class OnKill
{
public:
	typedef					::Application::Packets::CPacket
							PACKET;

public:
	static void				Execute( Memento * i_pMemento, PACKET * i_pPacket );

private:
	static void				onRespawn( Memento * i_pMemento, PACKET * i_pPacket );
	static void				onBomb( Memento * i_pMemento, PACKET * i_pPacket );

};

} /* CommonProcess */		} /* Play */
} /* State */				} /* Player */				} /* GameServer */