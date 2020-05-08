#include "StdAfx.h"

#include "./Provider.h"
#include "../IHandler.h"

namespace Dispatcher {		namespace Time {

Provider::Provider()
{
	m_bTime					= FALSE;
	m_uiOrder				= 0;

	this->install();
}

Provider::~Provider()
{
	this->uninstall();
}

CSTRING &
Provider::GetTime()
{
	if( m_bTime == FALSE )
	{
		m_bTime				= TRUE;

		SYSTEMTIME SystemTime;
		GetLocalTime( &SystemTime );

		m_strTime.Printf( "%d-%02d-%02d %02d:%02d:%02d",
			SystemTime.wYear,
			SystemTime.wMonth,
			SystemTime.wDay,
			SystemTime.wHour,
			SystemTime.wMinute,
			SystemTime.wSecond );

		m_strReturn.Printf( "%d\t%s",
			m_uiOrder ++,
			m_strTime.GetCSTR() );
	}
	else
	{
		m_strReturn.Printf( "%d\t%s",
			m_uiOrder ++,
			m_strTime.GetCSTR() );
	}

	return m_strReturn;
}

void
Provider::SetTime( CBOOL i_bSet )
{
	m_bTime					= i_bSet;
}

} /* Time */				} /* Dispatcher */