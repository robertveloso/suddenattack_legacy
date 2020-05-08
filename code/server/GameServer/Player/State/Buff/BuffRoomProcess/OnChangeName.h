#pragma once

#include "../../../Memento.h"

namespace Application {		namespace Packets {
struct CPacket;
} /* Packets */				} /* Application */

namespace GameServer {		namespace Player {			namespace State {
namespace Buff {			namespace BuffRoomProcess {

class OnChangeOption
{
public:
	typedef					::Application::Packets::CPacket
							PACKET;

public:
	static void				Excute( Memento * i_pMemento, PACKET * i_pPacket );

private:
	static CBOOL			checkTeamCount( Memento * i_pMemento, CINT i_nCount );

};

} /* BuffRoomProcess */		} /* Buff */
} /* State */				} /* Player */				} /* GameServer */