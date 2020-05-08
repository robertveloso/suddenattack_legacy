#pragma once

#include "../INoneResponser.h"

namespace GameServer {		namespace NoneResponser {

class Provider
:	public					INoneResponser
{
public:
	typedef					::Dispatcher::IQuery
							IQUERY;

public:
							Provider();
	virtual					~Provider();

	virtual void			OnResponse( IQUERY * i_pQuery );

};

} /* NoneResponser */		} /* GameServer */