#pragma once

#include "../../../Memento.h"

namespace GameServer {		namespace Player {			namespace State {
namespace Play {			namespace CommonProcess {

class OnInit
{
public:
	static void				Execute( Memento * i_pMemento );

};

} /* CommonProcess */		} /* Play */
} /* State */				} /* Player */				} /* GameServer */