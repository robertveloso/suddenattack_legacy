#pragma once

#include "../ICaster.h"

namespace GameServer {		namespace Caster {

class Provider
:	public					ICaster
{
public:
							Provider();
	virtual					~Provider();

	virtual void			Cast( IManager * i_pManager, PACKET * i_pPacket );
	virtual void			Cast( IPlayer * i_pPlayer, PACKET * i_pPacket );
	virtual void			Cast( IRoom * i_pRoom, PACKET * i_pPacket );
	virtual void			Cast( IRoom * i_pRoom, IPlayer * i_pExceptPlayer, PACKET * i_pPacket );
	virtual void			Signal( IPlayer * i_pPlayer, CUINT i_uiMessage );

};

} /* Catster */				} /* GameServer */