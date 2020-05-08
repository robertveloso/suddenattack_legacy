#include "StdAfx.h"

#include "./Responser.h"
#include "../../Jolyo/NJean/Session/QueryBuffer.h"
#include "atlcomcli.h"

namespace Dispatcher {		namespace Query {

Responser::Responser()
{
	this->install();
}

Responser::~Responser()
{
	this->uninstall();
}

CHAR
Responser::ReadChar( IQuery * i_pQuery, CSTRING & i_strParam )
{
	const _CommandPtr & pCmd = i_pQuery->GetCommand()->GetCommand();

	CHAR cResult			= pCmd->Parameters->GetItem( i_strParam.GetCSTR() )->Value.cVal;
	
	return cResult;
}

SHORT
Responser::ReadShort( IQuery * i_pQuery, CSTRING & i_strParam )
{
	const _CommandPtr & pCmd = i_pQuery->GetCommand()->GetCommand();

	SHORT nResult			= pCmd->Parameters->GetItem( i_strParam.GetCSTR() )->Value.iVal;
	
	return nResult;
}

INT
Responser::ReadInt( IQuery * i_pQuery, CSTRING & i_strParam )
{
	const _CommandPtr & pCmd = i_pQuery->GetCommand()->GetCommand();

	INT nResult				= pCmd->Parameters->GetItem( i_strParam.GetCSTR() )->Value.intVal;
	
	return nResult;
}

LONGLONG
Responser::ReadInt64( IQuery * i_pQuery, CSTRING & i_strParam )
{
	const _CommandPtr & pCmd = i_pQuery->GetCommand()->GetCommand();

	LONGLONG llResult		= pCmd->Parameters->GetItem( i_strParam.GetCSTR() )->Value.llVal;
	
	return llResult;
}

CSTRING &
Responser::ReadString( IQuery * i_pQuery, CSTRING & i_strParam )
{
	const _CommandPtr & pCmd = i_pQuery->GetCommand()->GetCommand();

	USES_CONVERSION; 
	_bstr_t bstrResult		= pCmd->Parameters->GetItem( i_strParam.GetCSTR() )->Value.bstrVal;
	m_strResult				= STRING( static_cast<const char *>( bstrResult ) );
	
	return m_strResult;
}

const char *
Responser::ReadRawString( IQuery * i_pQuery, CSTRING & i_strParam )
{
	const _CommandPtr & pCmd = i_pQuery->GetCommand()->GetCommand();

	USES_CONVERSION;
	m_bstrResult			= pCmd->Parameters->GetItem( i_strParam.GetCSTR() )->Value.bstrVal;
	return static_cast<const char *>( m_bstrResult );
}

} /* Query */				} /* Dispatcher */