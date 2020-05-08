#include "StdAfx.h"

namespace Kernel {
#include <process.h>
#include <Stdlib.h>
#include <Stdio.h>
} /* Kernel */

#include "./Function.h"

namespace Framework {

void
ABORT()
{
	::Kernel::abort();
}

void
OUTPUT( CSTRING & rMessage )
{
#ifdef WIN32
	OutputDebugString( rMessage );
#else
	CULONG ulLength		= rMessage.GetCount();
	CCHAR * szBuffer	= rMessage;

	clsAgent.Enter();

	CINT nBytes = ::write( 1, szBuffer, ulLength );
	if ( nBytes != static_cast<INT>( ulLength ) )
	{
		::syslog( LOG_ERR, szBuffer );
	}

	clsAgent.Leave();
#endif
}

void
MEMSET( void * pPointer, CCHAR cValue, CULONG ulSize )
{
	assert( pPointer != NULL );

	if ( ulSize != 0 )
	{
		::memset( pPointer, cValue, ulSize );
	}
}

void
MEMCPY( void * pTarget, void * pSource, CULONG ulSize )
{
	assert( pTarget != NULL && pSource != NULL );

	if ( ulSize != 0 )
	{
		::memcpy( pTarget, pSource, ulSize );
	}
}

CINT
MEMCMP( void * pPointerA, void * pPointerB, CULONG ulSize )
{
	assert( pPointerA != NULL && pPointerB != NULL );

	if ( ulSize == 0 )
	{
		return 0;
	}

	return ::memcmp( pPointerA, pPointerB, ulSize );
}

CINT
RAND( CINT nModValue )
{
	assert( nModValue > 0 );

	return (::Kernel::rand() % nModValue);
}

CINT
ATOI( CCHAR * szString )
{
	assert( szString != NULL );

	return ::Kernel::atoi( szString );
}

CLONG
ATOL( CCHAR * szString )
{
	assert( szString != NULL );

	return ::Kernel::atol( szString );
}

CFLOAT
ATOF( CCHAR * szString )
{
	assert( szString != NULL );

	return static_cast<FLOAT>( ::Kernel::atof( szString ) );
}


CINT
SNPRINTF( CHAR * szBuffer, CULONG ulLength, CCHAR * szFormat, ... )
{
	assert( szBuffer != NULL && szFormat != NULL );

	va_list args;
	va_start( args, szFormat );

	CINT nRetVal = VSNPRINTF( szBuffer, ulLength, szFormat, args );

	va_end( args );

	return nRetVal;
}

CINT
VSNPRINTF( CHAR * szBuffer, CULONG ulLength, CCHAR * szFormat, va_list args )
{
	assert( szBuffer != NULL && szFormat != NULL );

#ifdef WIN32
	return ::Kernel::_vsnprintf( szBuffer, ulLength, szFormat, args );
#else
	return ::vsnprintf( szBuffer, ulLength, szFormat, args );
#endif
}

} /* Framework */