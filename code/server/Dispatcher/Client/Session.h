#pragma once

#include "../IClient.h"

namespace Dispatcher {		namespace Handler {
class Provider;
} /* Handler */				} /* Dispatcher */

namespace Dispatcher {		namespace Client {

class Session
:	public					IClient::ISession
{
public:
							Session( Handler::Provider * i_pHandler );
	virtual					~Session();

	virtual void			Alarm( CUINT i_uiMessage, CUINT i_uiSecond, IClient * i_pClient );
	virtual void			Signal( CUINT i_uiMessage, IClient * i_pClient );
	virtual void			Send( PACKET * i_pPacket, IClient * i_pClient );
	virtual void			Close( IClient * i_pClient );

	virtual CDWORD			GetRTT( IClient * i_pClient );

private:
	Handler::Provider *		m_pHandler;

};

} /* Client */				} /* Dispatcher */