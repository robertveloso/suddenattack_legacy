#pragma once

#include "../IProcess.h"

namespace GameServer {		namespace Manager {			namespace Process {

class Kick
:	public					IProcess
{
public:
							Kick();
	virtual					~Kick();

	virtual CBOOL			Execute( IManager * i_pManager, PACKET * i_pPacket );
	virtual CINT			GetCode();
};

} /* Process */				} /* Manager */				} /* GamerServer */