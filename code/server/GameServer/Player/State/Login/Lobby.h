#pragma once

#include "../../IState.h"
#include "../../../IPlayer.h"

namespace GameServer {
class IRoom;
} /* GameServer */

namespace GameServer {		namespace Player {			namespace State {
namespace Login {

class Lobby
:	public					IState
,	public					Singleton<Lobby>
{
public:
							Lobby();
	virtual					~Lobby();

	virtual CBOOL			OnPacket( Memento * i_pMemento, PACKET * i_pPacket );
	virtual void			OnClose( Memento * i_pMemento );

	virtual void			OnResponse( Memento * i_pMemento, IQUERY * i_pQuery );

private:
	typedef					::LogicObject::Player::Type::E_TEAM
							E_TEAM;

private:
	void					onAwayChannel( Memento * i_pMemento );
	void					onJoinRoom( Memento * i_pMemento, PACKET * i_pPacket );
	void					onQuickJoinRoom( Memento * i_pMemento, PACKET * i_pPacket );
	void					onCreateRoom( Memento * i_pMemento, PACKET * i_pPacket );
	E_TEAM::ENUM			choiceTeam( IRoom * i_pRoom );
	IRoom *					findFreeRoom( Memento * i_pMemento );

	void					onChat( Memento * i_pMemento, PACKET * i_pPacket );
	void					onInfo( Memento * i_pMemento, PACKET * i_pPacket );

	void					onQuickChannel( Memento * i_pMemento, PACKET * i_pPacket );

};
static inline				Lobby *
LOBBY()						{ return Lobby::GetInstance(); }

} /* Login */
} /* State */				} /* Player */				} /* GameServer */