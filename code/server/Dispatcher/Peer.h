#pragma once

#include "../../IFramework.h"
#include "../../jolyo/NJean/Application\Peer.h"

namespace Dispatcher {		namespace Handler {

class Peer
:	public					::Application::CPeer
{
public:
							Peer();
	virtual					~Peer();
};

} /* Handler */				} /* Dispatcher */