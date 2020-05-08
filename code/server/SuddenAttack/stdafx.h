#pragma once


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

// TODO: reference additional headers your program requires here
#define USE_TRACE

void *
Talloc( unsigned int uiItemSize );

void
Tfree( void * pPointer );

void *
operator new( unsigned int uiItemSize );

void
operator delete( void * pPointer );

void *
operator new[]( unsigned int uiItemSize );
void
operator delete[]( void * pPointer );


#include <Winsock2.h>

#include <iostream>
#include <tchar.h>

#include <windows.h>
#include <assert.h>
#include <mmsystem.h>

#define ASSERT assert

#define interface struct