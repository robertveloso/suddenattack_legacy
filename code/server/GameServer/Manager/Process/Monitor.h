#pragma once

#include "../IProcess.h"

namespace GameServer {		namespace Manager {			namespace Process {

class Monitor
:	public					IProcess
{
public:
							Monitor();
	virtual					~Monitor();

	virtual CBOOL			Execute( IManager * i_pManager, PACKET * i_pPacket );
	virtual CINT			GetCode();

private:
	void					monitorPlayers( IManager * i_pManager );
	void					monitorRooms( IManager * i_pManager );
};

} /* Process */				} /* Manager */				} /* GamerServer */