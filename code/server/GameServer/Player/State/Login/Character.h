#pragma once

#include "../../IState.h"

namespace GameServer {		namespace Player {			namespace State {
namespace Login {

class Character
:	public					IState
,	public					Singleton<Character>
{
public:
							Character();
	virtual					~Character();

	virtual CBOOL			OnPacket( Memento * i_pMemento, PACKET * i_pPacket );
	virtual void			OnClose( Memento * i_pMemento );

	virtual void			OnResponse( Memento * i_pMemento, IQUERY * i_pQuery );

private:
	void					requestResult( Memento * i_pMemento );

};
static inline				Character *
CHARACTER()					{ return Character::GetInstance(); }

} /* Login */
} /* State */				} /* Player */				} /* GameServer */