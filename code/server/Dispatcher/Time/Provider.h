#pragma once

#include "../ITime.h"

namespace Dispatcher {		namespace Time {

class Provider
:	public					ITime
{
public:
							Provider();
	virtual					~Provider();

	virtual CSTRING &		GetTime();
	virtual void			SetTime( CBOOL i_bSet );

private:
	STRING					m_strTime;
	STRING					m_strReturn;
	BOOL					m_bTime;
	UINT					m_uiOrder;

};

} /* Time */				} /* Dispatcher */