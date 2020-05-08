#pragma once

#include "../../IState.h"

namespace GameServer {
class IChannel;
} /* GameServer */

namespace GameServer {		namespace Player {			namespace State {
namespace Login {

class Ready
:	public					IState
,	public					Singleton<Ready>
{
public:
							Ready();
	virtual					~Ready();

	virtual CBOOL			OnPacket( Memento * i_pMemento, PACKET * i_pPacket );
	virtual void			OnClose( Memento * i_pMemento );

	virtual void			OnResponse( Memento * i_pMemento, IQUERY * i_pQuery );

private:
	void					onJoin( Memento * i_pMemento, PACKET * i_pPacket );
	void					onRefresh( Memento * i_pMemento, PACKET * i_pPacket );

	CBYTE					joinable( IChannel * i_pChannel );
	void					castJoinFail( Memento * i_pMemento, CBYTE i_byResult );
	void					castJoinSuccess( Memento * i_pMemento, IChannel * i_pChannel );

};
static inline				Ready *
READY()						{ return Ready::GetInstance(); }

} /* Login */
} /* State */				} /* Player */				} /* GameServer */