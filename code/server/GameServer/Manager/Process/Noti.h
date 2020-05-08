#pragma once

#include "../IProcess.h"

namespace GameServer {
class IRoom;
} /* GameServer */

namespace GameServer {		namespace Manager {			namespace Process {

class Noti
:	public					IProcess
{
public:
							Noti();
	virtual					~Noti();

	virtual CBOOL			Execute( IManager * i_pManager, PACKET * i_pPacket );
	virtual CINT			GetCode();

private:
	void					cast( IRoom * i_pRoom, PACKET * i_pPacket );
};

} /* Process */				} /* Manager */				} /* GamerServer */