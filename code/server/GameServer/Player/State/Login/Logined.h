#pragma once

#include "../../IState.h"

namespace GameServer {
class IRoom;
class IPlayer;
} /* GameServer */

namespace GameServer {		namespace Player {			namespace State {
namespace Login {

class Logined
:	public					IState
,	public					Singleton<Logined>
{
public:
							Logined();
	virtual					~Logined();

	virtual CBOOL			OnPacket( Memento * i_pMemento, PACKET * i_pPacket );
	virtual void			OnClose( Memento * i_pMemento );

	virtual void			OnResponse( Memento * i_pMemento, IQUERY * i_pQuery );

private:
	void					setUserResult( Memento * i_pMemento, IQUERY * i_pQuery );
	void					castChannel( Memento * i_pMemento );
	void					castUserResult( Memento * i_pMemento );
	void					castRank( Memento * i_pMemento );

};
static inline				Logined *
LOGINED()					{ return Logined::GetInstance(); }

} /* Login */
} /* State */				} /* Player */				} /* GameServer */