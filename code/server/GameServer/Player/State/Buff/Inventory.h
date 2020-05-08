#pragma once

#include "../../IState.h"

namespace GameServer {
class IRoom;
} /* GameServer */

namespace GameServer {		namespace Player {			namespace State {
namespace Buff {

class Inventory
:	public					IState
,	public					Singleton<Inventory>
{
public:
							Inventory();
	virtual					~Inventory();

	virtual CBOOL			OnPacket( Memento * i_pMemento, PACKET * i_pPacket );
	virtual void			OnClose( Memento * i_pMemento );

	virtual void			OnResponse( Memento * i_pMemento, IQUERY * i_pQuery );

private:
	void					onOut( Memento * i_pMemento );

};
static inline				Inventory *
INVENTORY()					{ return Inventory::GetInstance(); }

} /* Buff */
} /* State */				} /* Player */				} /* GameServer */