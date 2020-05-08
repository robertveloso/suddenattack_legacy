#pragma once


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#pragma warning(disable:4127)
#pragma warning(disable:4189)
#pragma warning(disable:4201)
#pragma warning(disable:4100)
#pragma warning(disable:4099) 

// TODO: reference additional headers your program requires here
#include <stdio.h>

#include <iostream>
using namespace std;

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


//#include <Winsock2.h>
//
//#include <iostream>
//#include <tchar.h>
//
#include <Windows.h>
//#include <assert.h>
//#include <mmsystem.h>
//
//#define ASSERT assert
//
//#define interface struct


//const class
//{
//public:
//	template< class Type >
//	operator Type *() const
//	{
//		return 0;
//	}
//
//	template< class Base, class Type >
//	operator Type Base::*() const
//	{
//		return 0;
//	}
//
//} NULL;