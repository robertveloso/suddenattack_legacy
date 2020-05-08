#pragma once

#include "../IProcess.h"

namespace GameServer {
class IPlayer;
} /* GameServer */

namespace GameServer {		namespace Manager {			namespace Process {

class FinishNoti
:	public					IProcess
{
public:
							FinishNoti();
	virtual					~FinishNoti();

	virtual CBOOL			Execute( IManager * i_pManager, PACKET * i_pPacket );
	virtual CINT			GetCode();

private:
	void					disconnect( IPlayer * i_pPlayer );
};

} /* Process */				} /* Manager */				} /* GamerServer */