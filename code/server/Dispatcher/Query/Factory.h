#pragma once

#include "./Provider.h"

namespace Dispatcher {		namespace Handler {
class Provider;
} /* Handler */				} /* Dispatcher */

#include "../../jolyo/DataStructure/MemoryPool.h"

namespace Dispatcher {		namespace Query {

class Factory
:	public					IQuery::IFactory
{
public:
							Factory( Handler::Provider * i_pHandler, CINT i_nPoolCount );
	virtual					~Factory();

	virtual IQuery *		Create( CDWORD i_dwCode );
	virtual void			Destroy( IQuery * i_pQuery );

private:
	typedef					Common::DataStructure::CMemoryPool<Provider>
							POOL;

private:
	POOL *					m_pPool;
	Handler::Provider *		m_pHandler;

};

} /* Query */				} /* Dispatcher */