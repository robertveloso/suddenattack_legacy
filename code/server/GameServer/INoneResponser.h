#pragma once

#include "../Dispatcher/IQuery.h"

namespace GameServer {

class INoneResponser
:	public					::Dispatcher::IQuery::IObserver
,	public					Singleton<INoneResponser>
{
public:
	virtual					~INoneResponser() {}

};
static inline				INoneResponser *
INONERESPONSER()			{ return INoneResponser::GetInstance(); }

} /* GameServer */