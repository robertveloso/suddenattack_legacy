#pragma once

#include "./Type.h"
#include "../Macro.h"
#include "./Template/String.h"
using namespace ::Framework::Template;

namespace Framework {

void
ABORT();

void
OUTPUT( CSTRING & rMessage );

void
MEMSET( void * pPointer, CCHAR cValue, CULONG ulSize );

void
MEMCPY( void * pTarget, void * pSource, CULONG ulSize );

CINT
MEMCMP( void * pPointerA, void * pPointerB, CULONG ulSize );

CINT
RAND( CINT nModValue );

CINT
ATOI( CCHAR * szString );

CLONG
ATOL( CCHAR * szString );

CFLOAT
ATOF( CCHAR * szString );

//CDOUBLE
//ATOD( CCHAR * szString );

//CADDRESS
//ATOA( CCHAR * szString );

CINT
SNPRINTF( CHAR * szBuffer, CULONG ulLength, CCHAR * szFormat, ... );

CINT
VSNPRINTF( CHAR * szBuffer, CULONG ulLength, CCHAR * szFormat, va_list args );

template< class Type >
static inline const Type
ABS( const Type & rValue )
{
	return ( rValue > 0 ) ? rValue : (-rValue);
}

template< class Type >
static inline const Type
MIN( const Type & rValueA, const Type & rValueB )
{
	return ( rValueA > rValueB ) ? rValueB : rValueA;
}

template< class Type >
static inline const Type
MAX( const Type & rValueA, const Type & rValueB )
{
	return ( rValueA > rValueB ) ? rValueA : rValueB;
}

template< class Type >
static inline CBOOL
TEST( const Type & rTarget, const Type & rSource )
{
	if ( ( rTarget & rSource ) == rSource )
	{
		return TRUE;
	}

	return FALSE;
}

} /* Framework */