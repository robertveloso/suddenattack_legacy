#pragma once

#include "../../../Memento.h"

namespace Application {		namespace Packets {
struct CPacket;
} /* Packets */				} /* Application */

namespace GameServer {
class IRoom;
class IPlayer;
} /* GameServer */

namespace GameServer {		namespace Player {			namespace State {
namespace Buff {			namespace BuffRoomProcess {

class OnStart
{
public:
	typedef					::Application::Packets::CPacket
							PACKET;

public:
	static void				Execute( Memento * i_pMemento, PACKET * i_pPacket );

private:
	static IPlayer *		findSuperPeer( IRoom * i_pRoom );
	static IPlayer *		findNatSuperPeer( IRoom * i_pRoom );
	static CBOOL			isRunnable( Memento * i_pMemento, IRoom * i_pRoom );
	static void				successProcess( Memento * i_pMemento, IRoom * i_pRoom );
	static void				setTeamBalance( IRoom * i_pRoom );
	static void				castFail( Memento * i_pMemento, CBYTE i_byResult );
	static void				castSuperPeer( IPlayer * i_pPlayer );

};

} /* BuffRoomProcess */		} /* Buff */
} /* State */				} /* Player */				} /* GameServer */