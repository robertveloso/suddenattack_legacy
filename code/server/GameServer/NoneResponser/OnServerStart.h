#pragma once

#include "../INoneResponser.h"

namespace GameServer {		namespace NoneResponser {

class OnServerStart
{
public:
	typedef					::Dispatcher::IQuery
							IQUERY;

public:
							OnServerStart();
	virtual					~OnServerStart();

	static void				OnResponse( IQUERY * i_pQuery );

};

} /* NoneResponser */		} /* GameServer */