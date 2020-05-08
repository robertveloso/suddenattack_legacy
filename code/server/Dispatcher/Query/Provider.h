#pragma once

#include "../IQuery.h"

namespace Dispatcher {		namespace Query {

class Provider
:	public					IQuery
{
public:
							Provider();
	virtual					~Provider();

	void					Capture( CDWORD i_dwCode, COMMAND * i_pCommand );
	void					Release();

	virtual CDWORD			GetCode();
	virtual COMMAND *		GetCommand();

	virtual STRING &		GetLog();

	virtual E_RESULT::ENUM	GetResult();
	virtual void			SetResult( E_RESULT::ENUM i_eResult );

private:
	DWORD					m_dwCode;
	COMMAND *				m_pCommand;
	E_RESULT::ENUM			m_eResult;

	STRING					m_strLog;

};

} /* Query */				} /* Dispatcher */