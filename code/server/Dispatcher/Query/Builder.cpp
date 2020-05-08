#include "StdAfx.h"

#include "./Builder.h"
#include "../../Jolyo/NJean/Session/QueryBuffer.h"
#include "atlcomcli.h"

namespace Dispatcher {		namespace Query {

Builder::Builder()
{
	this->install();
}

Builder::~Builder()
{
	this->uninstall();
}

void
Builder::Build( IQuery * i_pQuery, CSTRING & i_strSP )
{
	const _CommandPtr & pCmd = i_pQuery->GetCommand()->GetCommand();

	pCmd->CommandText		= i_strSP.GetCSTR();
	pCmd->CommandType		= adCmdStoredProc;
}
		
void
Builder::Build( IQuery * i_pQuery, CSTRING & i_strParam, CHAR i_cData, CBOOL i_bInput )
{
	CComVariant stData( i_cData );
	const _CommandPtr & pCmd = i_pQuery->GetCommand()->GetCommand();

	ParameterDirectionEnum eParam;
	if( i_bInput == TRUE )
	{
		eParam				= adParamInput;
	}
	else
	{
		eParam				= adParamOutput;
	}
	_ParameterPtr pPrm	= pCmd->CreateParameter( i_strParam.GetCSTR(), adChar, eParam, sizeof(i_cData), stData );
	pCmd->Parameters->Append( pPrm );
	pPrm.Release();
	pPrm.Detach();
}
		
void
Builder::BuildTiny( IQuery * i_pQuery, CSTRING & i_strParam, BYTE i_byData, CBOOL i_bInput )
{
	CComVariant stData( i_byData );
	const _CommandPtr & pCmd = i_pQuery->GetCommand()->GetCommand();

	ParameterDirectionEnum eParam;
	if( i_bInput == TRUE )
	{
		eParam				= adParamInput;
	}
	else
	{
		eParam				= adParamOutput;
	}
	_ParameterPtr pPrm	= pCmd->CreateParameter( i_strParam.GetCSTR(), adTinyInt, eParam, sizeof(i_byData), stData );
	pCmd->Parameters->Append( pPrm );
	pPrm.Release();
	pPrm.Detach();
}

void
Builder::Build( IQuery * i_pQuery, CSTRING & i_strParam, SHORT i_nData, CBOOL i_bInput )
{
	CComVariant stData( i_nData );
	const _CommandPtr & pCmd = i_pQuery->GetCommand()->GetCommand();

	ParameterDirectionEnum eParam;
	if( i_bInput == TRUE )
	{
		eParam				= adParamInput;
	}
	else
	{
		eParam				= adParamOutput;
	}
	_ParameterPtr pPrm	= pCmd->CreateParameter( i_strParam.GetCSTR(), adChar, eParam, sizeof(i_nData), stData );
	pCmd->Parameters->Append( pPrm );
	pPrm.Release();
	pPrm.Detach();
}

void
Builder::Build( IQuery * i_pQuery, CSTRING & i_strParam, INT i_nData, CBOOL i_bInput )
{
	CComVariant stData( i_nData );
	const _CommandPtr & pCmd = i_pQuery->GetCommand()->GetCommand();

	ParameterDirectionEnum eParam;
	if( i_bInput == TRUE )
	{
		eParam				= adParamInput;
	}
	else
	{
		eParam				= adParamOutput;
	}
	_ParameterPtr pPrm	= pCmd->CreateParameter( i_strParam.GetCSTR(), adInteger, eParam, sizeof(i_nData), i_nData );
	pCmd->Parameters->Append( pPrm );
	pPrm.Release();
	pPrm.Detach();
}

void
Builder::Build( IQuery * i_pQuery, CSTRING & i_strParam, __int64 i_nData, CBOOL i_bInput )
{
	const _CommandPtr & pCmd = i_pQuery->GetCommand()->GetCommand();

	ParameterDirectionEnum eParam;
	if( i_bInput == TRUE )
	{
		eParam				= adParamInput;
	}
	else
	{
		eParam				= adParamOutput;
	}
#if (_WIN32_WINNT >= 0x0501)
	_ParameterPtr pPrm	= pCmd->CreateParameter( i_strParam.GetCSTR(), adBigInt, eParam, sizeof(i_nData), i_nData );//!!!
#else
	_ParameterPtr pPrm	= pCmd->CreateParameter( i_strParam.GetCSTR(), adBigInt, eParam, sizeof(i_nData), (int)i_nData );
#endif
	pCmd->Parameters->Append( pPrm );
	pPrm.Release();
	pPrm.Detach();
}

void
Builder::Build( IQuery * i_pQuery, CSTRING & i_strParam, CSTRING & i_strData, CBOOL i_bInput, INT i_nDataLen )
{
	CComVariant stData(i_strData.GetCSTR());

	const _CommandPtr & pCmd = i_pQuery->GetCommand()->GetCommand();

	ParameterDirectionEnum eParam;
	if( i_bInput == TRUE )
	{
		eParam				= adParamInput;
	}
	else
	{
		eParam				= adParamOutput;
	}
	_ParameterPtr pPrm	= pCmd->CreateParameter( i_strParam.GetCSTR(), adVarChar, eParam, i_nDataLen, stData );
	pCmd->Parameters->Append( pPrm );
	pPrm.Release();
	pPrm.Detach();
}

void
Builder::BuildLowStr( IQuery * i_pQuery, CSTRING & i_strParam, const CHAR * i_strData, CBOOL i_bInput, INT i_nDataLen )
{
	CComVariant stData(i_strData);
	const _CommandPtr & pCmd = i_pQuery->GetCommand()->GetCommand();

	ParameterDirectionEnum eParam;
	if( i_bInput == TRUE )
	{
		eParam				= adParamInput;
	}
	else
	{
		eParam				= adParamOutput;
	}
	_ParameterPtr pPrm	= pCmd->CreateParameter( i_strParam.GetCSTR(), adVarChar, eParam, i_nDataLen, stData );
	pCmd->Parameters->Append( pPrm );
	pPrm.Release();
	pPrm.Detach();
}

} /* Query */				} /* Dispatcher */