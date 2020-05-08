#pragma once

#include "../../../Memento.h"

namespace Application {		namespace Packets {
struct CPacket;
} /* Packets */				} /* Application */

namespace GameServer {		namespace Player {			namespace State {
namespace Buff {			namespace BuffRoomProcess {

class OnReady
{
public:
	typedef					::Application::Packets::CPacket
							PACKET;

public:
	static void				Execute( Memento * i_pMemento, PACKET * i_pPacket );

private:
	static void				doStart( Memento * i_pMemento );
	static void				doGame( Memento * i_pMemento );

};

} /* BuffRoomProcess */		} /* Buff */
} /* State */				} /* Player */				} /* GameServer */