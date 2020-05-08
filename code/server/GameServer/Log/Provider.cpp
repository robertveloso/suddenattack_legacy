#include "StdAfx.h"

#include "./Provider.h"
#include "../../Dispatcher/IConsole.h"

namespace GameServer {		namespace Log {

Provider::Provider()
{
}

Provider::~Provider()
{
}

void
Provider::Capture( const STRING & strFileName )
{
	Assert( strFileName.GetCount() > 1 );
	m_strFileName				= strFileName;
	m_pFile						= NULL;

	this->changeFile();
}

void
Provider::Release()
{
	if( m_pFile != NULL )
	{
		::fclose( m_pFile );
		m_pFile					= NULL;
	}
}

void
Provider::ChangeFile()
{
	this->changeFile();
}

void
Provider::Write( CSTRING & strMessage )
{
	Assert( m_pFile != NULL );

	CULONG ulCount				= strMessage.GetCount();
	::fwrite( strMessage.GetCSTR(), sizeof( CHAR ), ulCount, m_pFile );
	::fflush( m_pFile );
}

inline void
Provider::changeFile()
{
	if( m_pFile != NULL )
	{
		::fclose( m_pFile );
	}

	STRING					strFileName;
	STRING					strTime;
	
	SYSTEMTIME				SystemTime;
	GetLocalTime( &SystemTime );

	strTime.Printf( "%02d%02d%02d_%02d%02d%02d",
						SystemTime.wYear,
						SystemTime.wMonth,
						SystemTime.wDay,
						SystemTime.wHour,
						SystemTime.wMinute,
						SystemTime.wSecond );

	strFileName.Printf( "%s_%s.dat",
						m_strFileName.GetCSTR(),
						strTime.GetCSTR()
						);

	m_pFile					= ::fopen( strFileName.GetCSTR(), "a+" );
	if( m_pFile == NULL )
	{
		::Dispatcher::ICONSOLE()->Output( STRING( "File Open Fail\r\n" ) );
	}
}

} /* Log */					} /* GameServer */
