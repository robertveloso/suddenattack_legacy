#pragma once

#include "../INoneResponser.h"

namespace GameServer {		namespace NoneResponser {

class OnLevel
{
public:
	typedef					::Dispatcher::IQuery
							IQUERY;

public:
							OnLevel();
	virtual					~OnLevel();

	static void				OnResponse( IQUERY * i_pQuery );

};

} /* NoneResponser */		} /* GameServer */