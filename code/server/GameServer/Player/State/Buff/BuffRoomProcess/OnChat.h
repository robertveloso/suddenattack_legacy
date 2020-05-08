#pragma once

#include "../../../Memento.h"

namespace Application {		namespace Packets {
struct CPacket;
} /* Packets */				} /* Application */

namespace GameServer {		namespace Player {			namespace State {
namespace Buff {			namespace BuffRoomProcess {

class OnChat
{
public:
	typedef					::Application::Packets::CPacket
							PACKET;

public:
	static void				Execute( Memento * i_pMemento, PACKET * i_pPacket );

};

} /* BuffRoomProcess */		} /* Buff */
} /* State */				} /* Player */				} /* GameServer */