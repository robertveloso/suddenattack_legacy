#pragma once

#include "../../../Memento.h"

namespace Application {		namespace Packets {
struct CPacket;
} /* Packets */				} /* Application */

namespace GameServer {
class IRoom;
} /* GameServer */

namespace GameServer {		namespace Player {			namespace State {
namespace Play {			namespace CommonProcess {

class OnBomb
{
public:
	typedef					::Application::Packets::CPacket
							PACKET;

public:
	static void				Execute( Memento * i_pMemento, PACKET * i_pPacket );

private:
	static void				onDefused( IRoom * i_pRoom, CINT i_nPlayer );
	static void				onDetonated( IRoom * i_pRoom, CINT i_nPlayer );
};

} /* CommonProcess */		} /* Play */
} /* State */				} /* Player */				} /* GameServer */