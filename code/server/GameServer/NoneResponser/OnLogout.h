#pragma once

#include "../INoneResponser.h"

namespace GameServer {		namespace NoneResponser {

class OnLogout
{
public:
	typedef					::Dispatcher::IQuery
							IQUERY;

public:
							OnLogout();
	virtual					~OnLogout();

	static void				OnResponse( IQUERY * i_pQuery );

};

} /* NoneResponser */		} /* GameServer */