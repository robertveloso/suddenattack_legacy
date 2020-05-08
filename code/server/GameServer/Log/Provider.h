#pragma once

#include "../ILog.h"

namespace GameServer {		namespace Log {

class Provider
:	public					ILog
{
public:
							Provider();
	virtual					~Provider();

	void					Capture( const STRING & strFileName );
	void					Release();

private:
	virtual void			ChangeFile();
	virtual void			Write( CSTRING & strMessage );
	
	void					changeFile();

private:
	STRING					m_strFileName;
	FILE *					m_pFile;

};

} /* Log */					} /* GameServer */
