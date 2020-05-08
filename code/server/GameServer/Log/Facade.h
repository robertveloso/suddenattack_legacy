#pragma once

#include "../ILog.h"

namespace GameServer {		namespace Log {

class Facade
:	public					ILog::IFacade
{
public:
							Facade();
	virtual					~Facade();

public:
	virtual void			Open( CPATH & stPath );

	virtual void			ChangeFile();

	virtual void			RecordFail( CSTRING & strMessage );
	virtual void			Chatting( CSTRING & strMessage );

private:
	ILog *					m_pRecordFail;
	ILog *					m_pChatting;
};

} /* Log */					} /* GameServer */
