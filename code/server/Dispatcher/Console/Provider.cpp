#include "StdAfx.h"
#include <time.h>

#include "./Provider.h"
#include <direct.h>

namespace Dispatcher {		namespace Console {

Provider::Provider()
{
	CHAR strOut[1024];
	m_pWrite				= fopen( "Event.txt", "a+" );
	ASSERT(m_pWrite);
	_getcwd( strOut, 1024 );
	m_strRead.Printf( "%s%s", strOut, "\\Config.ini" );
	m_strReadTable.Printf( "%s%s", strOut, "\\Table.ini" );

	char dbuffer [9];
	char tbuffer [9];
	_strdate( dbuffer );
	_strtime( tbuffer );
	
	fprintf(m_pWrite,"\r\n======= %s == %s ===============\r\n",dbuffer,tbuffer);
	fflush( m_pWrite );
	this->install();
}

Provider::~Provider()
{
	this->uninstall();
	fclose( m_pWrite );
}

CSTRING &
Provider::Read( CSTRING & i_strKey )
{
	CHAR strOut[1024];
	CDWORD dwReturn = GetPrivateProfileString( "SERVER", i_strKey.GetCSTR(), NULL, strOut, 1024, m_strRead.GetCSTR() );
	m_strOut				= strOut;
	return m_strOut;
}

CSTRING &
Provider::ReadTable( CSTRING & i_strTitle, CSTRING & i_strKey )
{
	CHAR strOut[1024];
	CDWORD dwReturn = GetPrivateProfileString( i_strTitle, i_strKey.GetCSTR(), NULL, strOut, 1024, m_strReadTable.GetCSTR() );
	m_strOut				= strOut;
	return m_strOut;
}

void
Provider::Write( CSTRING & i_strData )
{
	fwrite( i_strData.GetCSTR(), sizeof( CHAR ), i_strData.GetCount(), m_pWrite );
	fflush( m_pWrite );
}

void
Provider::Output( CSTRING & i_strData )
{
	OutputDebugString( i_strData );
}

void
Provider::WriteUnsafe( const char * i_strData )
{
	int nLen = 1024;
	for( int i = 0; i < 1024; i++ )
	{
		if( i_strData[ i ] == '\0' )
		{
			nLen = i;
			break;
		}
	}
	fwrite( i_strData, sizeof( CHAR ), nLen, m_pWrite );
	fflush( m_pWrite );
}

} /* Console */				} /* Dispatcher */