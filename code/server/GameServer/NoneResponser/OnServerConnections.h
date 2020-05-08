#pragma once

#include "../INoneResponser.h"

namespace GameServer {		namespace NoneResponser {

class OnServerConnections
{
public:
	typedef					::Dispatcher::IQuery
							IQUERY;

public:
							OnServerConnections();
	virtual					~OnServerConnections();

	static void				OnResponse( IQUERY * i_pQuery );

};

} /* NoneResponser */		} /* GameServer */