#pragma once

#include "../System.h"

namespace Dispatcher {

class IConsole
:	public					Singleton<IConsole>
{
public:
	virtual					~IConsole() {}

	virtual CSTRING &		Read( CSTRING & i_strKey ) = 0;
	virtual CSTRING &		ReadTable( CSTRING & i_strTitle, CSTRING & i_strKey ) = 0;
	virtual void			Write( CSTRING & i_strData ) = 0;
	virtual void			Output( CSTRING & i_strData ) = 0;

	virtual void			WriteUnsafe( const char * i_strData ) = 0;
};
static inline				IConsole *
ICONSOLE()					{ return IConsole::GetInstance(); }

} /* Dispatcher */