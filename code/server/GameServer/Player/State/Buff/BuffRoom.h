#pragma once

#include "../../IState.h"

namespace GameServer {
class IRoom;
class IChannel;
} /* GameServer */

namespace GameServer {		namespace Player {			namespace State {
namespace Buff {

class BuffRoom
:	public					IState
,	public					Singleton<BuffRoom>
{
public:
							BuffRoom();
	virtual					~BuffRoom();

	virtual CBOOL			OnPacket( Memento * i_pMemento, PACKET * i_pPacket );
	virtual void			OnClose( Memento * i_pMemento );

	virtual void			OnResponse( Memento * i_pMemento, IQUERY * i_pQuery );

private:
	void					onInventory( Memento * i_pMemento, PACKET * i_pPacket );
	void					onAway( Memento * i_pMemento, PACKET * i_pPacket );
	void					onStart( Memento * i_pMemento );
	void					onEnd( Memento * i_pMemento );
	void					onPlay( Memento * i_pMemento );
	void					onOut( Memento * i_pMemento, PACKET * i_pPacket );
	void					onOut( Memento * i_pMemento );
	void					onInfo( Memento * i_pMemento, PACKET * i_pPacket );
	void					onChangeTeam( Memento * i_pMemento );
	void					onChangeTitle( Memento * i_pMemento, PACKET * i_pPacket );
	void					onChangeCaptain( Memento * i_pMemento, PACKET * i_pPacket );
	
	void					OnViewLobby( Memento * i_pMemento);
};
static inline				BuffRoom *
BUFFROOM()					{ return BuffRoom::GetInstance(); }

} /* Buff */
} /* State */				} /* Player */				} /* GameServer */