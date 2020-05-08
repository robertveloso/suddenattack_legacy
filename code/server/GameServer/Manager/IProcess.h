#pragma once

#include "../../System.h"

namespace Application {		namespace Packets {
struct CPacket;
} /* Packets */				} /* Application */

namespace GameServer {
class IManager;
} /* GamerServer */

namespace GameServer {		namespace Manager {

class IProcess
{
public:
	typedef					::Application::Packets::CPacket
							PACKET;

public:
	class					IContainer
	:	public				Singleton<IContainer>
	{
	public :
		virtual				~IContainer() {}

		virtual IProcess *	Search( CINT i_nCode ) = 0;
	};
	static inline			IContainer *
	ICONTAINER()			{ return IContainer::GetInstance(); }

public:
	virtual					~IProcess() {}

	virtual CBOOL			Execute( IManager * i_pManager, PACKET * i_pPacket ) = 0;
	virtual CINT			GetCode() = 0;
};

} /* Manager */				} /* GamerServer */