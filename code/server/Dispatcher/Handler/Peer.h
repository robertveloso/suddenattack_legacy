#pragma once

#include "../../System.h"
#include "../../Jolyo/NJean/Application/Peer.h"

namespace Dispatcher {
class IClient;
} /* Dispatcher */

namespace Dispatcher {		namespace Handler {

class Peer
:	public					::Application::CPeer
{
public:
							Peer();
	virtual					~Peer();

	void					Capture();
	void					Release();

	IClient *				GetClient();
	void					SetClient( IClient * i_pClient );

private:
	IClient *				m_pClient;

};

} /* Handler */				} /* Dispatcher */