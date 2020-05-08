#pragma once

#include "./Provider.h"

namespace Dispatcher {		namespace Thread {

class Factory
:	public					IThread::IFactory
{
public:
							Factory();
	virtual					~Factory();

	virtual IThread *		Create( IOBSERVER * i_pObserver );
	virtual void			Destroy( IThread * i_pThread );

};

} /* Thread */				} /* Dispatcher */
