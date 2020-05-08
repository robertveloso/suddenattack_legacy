#pragma once

#include "../../IState.h"

namespace GameServer {		namespace Player {			namespace State {
namespace Play {

class BombReady
:	public					IState
,	public					Singleton<BombReady>
{
public:
							BombReady();
	virtual					~BombReady();

	virtual CBOOL			OnPacket( Memento * i_pMemento, PACKET * i_pPacket );
	virtual void			OnClose( Memento * i_pMemento );

	virtual void			OnResponse( Memento * i_pMemento, IQUERY * i_pQuery );

private:
	void					onEnd( Memento * i_pMemento );
	void					onNextRound( Memento * i_pMemento );
	void					onStartRound( Memento * i_pMemento );
	void					onChangeHost( Memento * i_pMemento );

};
static inline				BombReady *
BOMBREADY()					{ return BombReady::GetInstance(); }

} /* Play */
} /* State */				} /* Player */				} /* GameServer */