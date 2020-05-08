#pragma once

#include "../INoneResponser.h"

namespace GameServer {		namespace NoneResponser {

class OnRank
{
public:
	typedef					::Dispatcher::IQuery
							IQUERY;

public:
							OnRank();
	virtual					~OnRank();

	static void				OnResponse( IQUERY * i_pQuery );

private:
	static void				castRanking( CSTRING & strRank );

};

} /* NoneResponser */		} /* GameServer */