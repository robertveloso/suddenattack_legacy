#pragma once

#include "../../IState.h"

namespace GameServer {
class IRoom;
} /* GameServer */

namespace GameServer {		namespace Player {			namespace State {
namespace Play {

class BombRoom
:	public					IState
,	public					Singleton<BombRoom>
{
public:
							BombRoom();
	virtual					~BombRoom();

	virtual CBOOL			OnPacket( Memento * i_pMemento, PACKET * i_pPacket );
	virtual void			OnClose( Memento * i_pMemento );

	virtual void			OnResponse( Memento * i_pMemento, IQUERY * i_pQuery );

private:
	void					onEnd( Memento * i_pMemento );
	void					onChangeHost( Memento * i_pMemento );
	void					onTimeout( Memento * i_pMemento );

	void					setTeamBalance( Memento * i_pMemento );

};
static inline				BombRoom *
BOMBROOM()					{ return BombRoom::GetInstance(); }

} /* Play */
} /* State */				} /* Player */				} /* GameServer */