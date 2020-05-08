#include "StdAfx.h"

#include "./Provider.h"

namespace Dispatcher {		namespace Query {

Provider::Provider()
{
}

Provider::~Provider()
{
}

void
Provider::Capture( CDWORD i_dwCode, COMMAND * i_pCommand )
{
	Assert( i_pCommand != NULL );
	m_dwCode				= i_dwCode;
	m_pCommand				= i_pCommand;
}

void
Provider::Release()
{
	m_pCommand				= NULL;
}

CDWORD
Provider::GetCode()
{
	return m_dwCode;
}

Provider::E_RESULT::ENUM
Provider::GetResult()
{
	return m_eResult;
}

void
Provider::SetResult( E_RESULT::ENUM i_eResult )
{
	m_eResult				= i_eResult;
}

Provider::COMMAND *
Provider::GetCommand()
{
	return m_pCommand;
}

STRING &
Provider::GetLog()
{
	return m_strLog;
}

} /* Query */				} /* Dispatcher */