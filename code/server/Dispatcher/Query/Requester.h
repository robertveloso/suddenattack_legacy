#pragma once

#include "../IQuery.h"
//#include "../../System/Collections/Map.h"

namespace Dispatcher {		namespace Handler {
class Provider;
} /* Handler */				} /* Dispatcher */

namespace Dispatcher {		namespace Query {

class Requester
:	public					IQuery::IRequester
{
public:
							Requester( Handler::Provider * i_pHandler );
	virtual					~Requester();

	virtual IOBSERVER *		GetObserver();
	virtual void			SetObserver( IOBSERVER * i_pObserver );

	virtual void			Regist( IQuery * i_pQuery, IClient * i_pClient );
	virtual void			Regist( IQuery * i_pQuery );
//	virtual IQuery *		Unregist( CDWORD i_dwCode );

private:
	//typedef					Collections::Map<DWORD, IQuery>
	//						MAP;

private:
//	MAP *					m_pMap;
	IOBSERVER *				m_pObserver;
	Handler::Provider *		m_pHandler;

};

} /* Query */				} /* Dispatcher */