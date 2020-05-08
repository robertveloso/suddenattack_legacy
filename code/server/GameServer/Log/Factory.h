#pragma once

#include "../ILog.h"

namespace GameServer {		namespace Log {

class Factory
:	public					ILog::IFactory
{
public:
							Factory();
	virtual					~Factory();

private:
	virtual ILog *			Create( const STRING & strPath );
	virtual void			Destroy( ILog * pLog );

private:
	ILog::IFacade *			m_pFacade;

};

} /* Log */					} /* GameServer */
