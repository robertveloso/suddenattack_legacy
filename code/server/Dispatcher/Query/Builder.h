#pragma once

#include "../IQuery.h"

namespace Dispatcher {		namespace Query {

class Builder //数据库操作
:	public					IQuery::IBuilder
{
public:
							Builder();
	virtual					~Builder();
	//注意build的调用顺序，第一个Build重新建立一个Query，以后为在此基础上添加.
	//添加完后，由IRequester中的Regist向Dispatcher::IHandle(即由他和CSessionGate派生的类)处理。
	virtual void			Build( IQuery * i_pQuery, CSTRING & i_strSP );
	virtual void			Build( IQuery * i_pQuery, CSTRING & i_strParam, CHAR i_cData, CBOOL i_bInput );
	virtual void			BuildTiny( IQuery * i_pQuery, CSTRING & i_strParam, BYTE i_byData, CBOOL i_bInput );
	virtual void			Build( IQuery * i_pQuery, CSTRING & i_strParam, SHORT i_nData, CBOOL i_bInput );
	virtual void			Build( IQuery * i_pQuery, CSTRING & i_strParam, INT i_nData, CBOOL i_bInput );
	virtual void			Build( IQuery * i_pQuery, CSTRING & i_strParam, __int64 i_nData, CBOOL i_bInput );
	virtual void			Build( IQuery * i_pQuery, CSTRING & i_strParam, CSTRING & i_strData, CBOOL i_bInput, INT i_nDataLen );
	virtual void			BuildLowStr( IQuery * i_pQuery, CSTRING & i_strParam, const CHAR * i_strData, CBOOL i_bInput, INT i_nDataLen );
};

} /* Query */				} /* Dispatcher */