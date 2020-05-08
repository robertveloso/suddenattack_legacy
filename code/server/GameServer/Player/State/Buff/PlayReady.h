#pragma once

#include "../../IState.h"

namespace GameServer {
class IRoom;
} /* GameServer */

namespace GameServer {		namespace Player {			namespace State {
namespace Buff {

class PlayReady
:	public					IState
,	public					Singleton<PlayReady>
{
public:
							PlayReady();
	virtual					~PlayReady();

	virtual CBOOL			OnPacket( Memento * i_pMemento, PACKET * i_pPacket );
	virtual void			OnClose( Memento * i_pMemento );

	virtual void			OnResponse( Memento * i_pMemento, IQUERY * i_pQuery );

private:
	void					returnBuff( Memento * i_pMemento );
	void					onPlay( Memento * i_pMemento );
	void					onEnd( Memento * i_pMemento );
	void					onExit( Memento * i_pMemento );

};
static inline				PlayReady *
PLAYREADY()					{ return PlayReady::GetInstance(); }

} /* Buff */
} /* State */				} /* Player */				} /* GameServer */