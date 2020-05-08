#pragma once

#include "../IProcess.h"

namespace GameServer {		namespace Manager {			namespace Process {

class Finish
:	public					IProcess
{
public:
							Finish();
	virtual					~Finish();

	virtual CBOOL			Execute( IManager * i_pManager, PACKET * i_pPacket );
	virtual CINT			GetCode();
};

} /* Process */				} /* Manager */				} /* GamerServer */