#pragma once

#include "../INoneResponser.h"

namespace GameServer {		namespace NoneResponser {

class OnGame
{
public:
	typedef					::Dispatcher::IQuery
							IQUERY;

public:
							OnGame();
	virtual					~OnGame();

	static void				OnResponse( IQUERY * i_pQuery );

};

} /* NoneResponser */		} /* GameServer */