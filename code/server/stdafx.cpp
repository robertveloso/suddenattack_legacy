// stdafx.cpp : source file that includes just the standard includes
// CasualServerLib.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

#include "malloc.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file
void *
operator new( unsigned int uiItemSize )
{
	return ::Talloc( uiItemSize );
}

void
operator delete( void * pPointer )
{
	if ( pPointer == 0 )
	{
		return;
	}

	::Tfree( pPointer );
}

void *
operator new[]( unsigned int uiItemSize )
{
	return ::Talloc( uiItemSize );
}

void
operator delete[]( void * pPointer )
{
	if ( pPointer == NULL )
	{
		return;
	}

	::Tfree( pPointer );
}


void *
Talloc( unsigned int uiItemSize )
{
	return ::malloc( uiItemSize );
}

void
Tfree( void * pPointer )
{
	::free( pPointer );
}