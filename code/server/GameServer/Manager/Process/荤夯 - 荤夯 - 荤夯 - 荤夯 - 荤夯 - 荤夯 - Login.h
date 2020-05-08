#pragma once

#include "../IProcess.h"

namespace GameServer {		namespace Manager {			namespace Process {

class Login
:	public					IProcess
{
public:
							Login();
	virtual					~Login();

	virtual CBOOL			Execute( IManager * i_pManager, PACKET * i_pPacket );
	virtual CINT			GetCode();
};

} /* Process */				} /* Manager */				} /* GamerServer */