#pragma once

#include "../INoneResponser.h"

namespace GameServer {		namespace NoneResponser {

class OnResult
{
public:
	typedef					::Dispatcher::IQuery
							IQUERY;

public:
							OnResult();
	virtual					~OnResult();

	static void				OnResponse( IQUERY * i_pQuery );

};

} /* NoneResponser */		} /* GameServer */