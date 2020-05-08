#pragma once

#include "../../IState.h"

namespace GameServer {		namespace Player {			namespace State {
namespace Buff {

class ReturnBuff
:	public					IState
,	public					Singleton<ReturnBuff>
{
public:
							ReturnBuff();
	virtual					~ReturnBuff();

	virtual CBOOL			OnPacket( Memento * i_pMemento, PACKET * i_pPacket );
	virtual void			OnClose( Memento * i_pMemento );

	virtual void			OnResponse( Memento * i_pMemento, IQUERY * i_pQuery );

private:
	void					onReturnBuff( Memento * i_pMemento );

};
static inline				ReturnBuff *
RETURNBUFF()				{ return ReturnBuff::GetInstance(); }

} /* Buff */
} /* State */				} /* Player */				} /* GameServer */