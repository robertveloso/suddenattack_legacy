#pragma once

#include "../System.h"

namespace Dispatcher {		namespace Handler {
class Peer;
} /* Handler */				} /* Dispatcher */

namespace Session {			namespace Manage {
class CDbObject;
} /* Manage */				} /* Session */

namespace Dispatcher {

class IHandler
:	public					Singleton<IHandler>
{
public:
	struct					Argument
	{
		typedef				String<256>
							STRING;
		INT					nClientCount;
		INT					nRecvQSize;
		INT					nSendQSize;
		INT					nQueryQSize;
		INT					nSignalQSize;
		INT					nResponseBufferSize;
		INT					nAcceptPoolSize;		// 4
		INT					nPortNumber;

		INT					nDbConnect;
		STRING				strDbConnection;
	};

	typedef					Handler::Peer
							PEER;
	typedef					::Session::Manage::CDbObject
							COMMAND;

public:
	virtual					~IHandler() {}

	virtual CBOOL			Initialize( const Argument & i_stArg ) = 0;
	virtual void			Start( const Argument & i_stArg ) = 0;
	
	virtual
	const String<17> &		GetPeerIP( PEER * i_pPeer ) = 0;

	virtual CULONG			GetTickCount() = 0;

};
static inline				IHandler *
IHANDLER()					{ return IHandler::GetInstance(); }

} /* Dispatcher */