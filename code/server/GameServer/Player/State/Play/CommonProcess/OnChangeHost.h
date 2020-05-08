#pragma once

#include "../../../Memento.h"

namespace GameServer {
class IRoom;
class IPlayer;
} /* GameServer */

namespace GameServer {		namespace Player {			namespace State {
namespace Play {			namespace CommonProcess {

class OnChangeHost
{
public:
	static void				Execute( Memento * i_pMemento, IRoom * i_pRoom );

private:
	static IPlayer *		findSuperPeer( IRoom * i_pRoom );
	static IPlayer *		findSuperPeerOnNat( IRoom * i_pRoom );

};

} /* CommonProcess */		} /* Play */
} /* State */				} /* Player */				} /* GameServer */