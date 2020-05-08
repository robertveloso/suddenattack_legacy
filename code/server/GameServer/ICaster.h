#pragma once

#include "../System.h"

namespace Application {		namespace Packets {
struct CPacket;
} /* Packets */				} /* Application */

namespace GameServer {
class IManager;
class IPlayer;
class IRoom;
} /* GameServer */

namespace GameServer {

class ICaster
:	public					Singleton<ICaster>
{
public:
	typedef					::Application::Packets::CPacket
							PACKET;

public:
	virtual					~ICaster() {}

	virtual void			Cast( IManager * i_pManager, PACKET * i_pPacket ) = 0;
	virtual void			Cast( IPlayer * i_pPlayer, PACKET * i_pPacket ) = 0;
	virtual void			Cast( IRoom * i_pRoom, PACKET * i_pPacket ) = 0;
	virtual void			Cast( IRoom * i_pRoom, IPlayer * i_pExceptPlayer, PACKET * i_pPacket ) = 0;
	virtual void			Signal( IPlayer * i_pPlayer, CUINT i_uiMessage ) = 0;

};
static inline				ICaster *ICASTER()	{ return ICaster::GetInstance(); }

} /* GameServer */