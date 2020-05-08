#pragma once

#include "../IQuery.h"

namespace Dispatcher {		namespace Query {

class						Responser
:	public					IQuery::IResponser
{
public:
							Responser();
	virtual					~Responser();

	virtual CHAR			ReadChar( IQuery * i_pQuery, CSTRING & i_strParam );
	virtual SHORT			ReadShort( IQuery * i_pQuery, CSTRING & i_strParam );
	virtual INT				ReadInt( IQuery * i_pQuery, CSTRING & i_strParam );
	virtual LONGLONG		ReadInt64( IQuery * i_pQuery, CSTRING & i_strParam );
	virtual CSTRING &		ReadString( IQuery * i_pQuery, CSTRING & i_strParam );
	virtual const char *	ReadRawString( IQuery * i_pQuery, CSTRING & i_strParam );

private:
	STRING					m_strResult;
	_bstr_t					m_bstrResult;

};

} /* Query */				} /* Dispatcher */