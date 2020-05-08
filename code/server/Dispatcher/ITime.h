#pragma once

#include "../System.h"

namespace Dispatcher {

class ITime
:	public					Singleton<ITime>
{
public:
	virtual					~ITime() {}

	virtual CSTRING &		GetTime() = 0;
	virtual void			SetTime( CBOOL i_bSet ) = 0;

};
static inline				ITime *
ITIME()						{ return ITime::GetInstance(); }

} /* Dispatcher */