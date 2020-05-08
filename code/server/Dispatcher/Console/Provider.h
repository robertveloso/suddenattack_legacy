#pragma once

#include "../IConsole.h"
#include <stdio.h>

namespace Dispatcher {		namespace Console {

class Provider
:	public					IConsole
{
public:
							Provider();
	virtual					~Provider();

	virtual CSTRING &		Read( CSTRING & i_strKey );
	virtual CSTRING &		ReadTable( CSTRING & i_strTitle, CSTRING & i_strKey );
	virtual void			Write( CSTRING & i_strData );
	virtual void			Output( CSTRING & i_strData );

	virtual void			WriteUnsafe( const char * i_strData );

private:
	STRING					m_strOut;
	STRING					m_strRead;
	STRING					m_strReadTable;
	FILE *					m_pWrite;
};

} /* Console */				} /* Dispatcher */