#pragma once

#include "../../../Memento.h"

namespace Application {		namespace Packets {
struct CPacket;
} /* Packets */				} /* Application */

namespace GameServer {
class IPlayer;
class IRoom;
} /* GameServer */

namespace GameServer {		namespace Player {			namespace State {
namespace Buff {			namespace PlayReadyProcess {

class OnHost
{
public:
	typedef					::Application::Packets::CPacket
							PACKET;

public:
	static void				Execute( Memento * i_pMemento, PACKET * i_pPacket );

private:
	static void				successProcess( Memento * i_pMemento, CWORD i_wPort );
	static void				castChangeState( IPlayer * i_pPlayer, IRoom * i_pRoom );
	static void				failProcess( Memento * i_pMemento );
	static void				cancelProcess( Memento * i_pMemento );

};

} /* PlayReadyProcess */	} /* Buff */
} /* State */				} /* Player */				} /* GameServer */