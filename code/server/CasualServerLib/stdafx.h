// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이 
// 들어 있는 포함 파일입니다.
//

#pragma once

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.

//static ULONG s_ulAllocCount	= 0;

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


#include <WinSock2.h>

#include <iostream>
#include <tchar.h>

#include <windows.h>
//#include <assert.h>

#import "c:\Program Files\Common Files\System\ADO\msado15.dll" \
    no_namespace rename("EOF", "EndOfFile")


//#ifndef NDEBUG
//	#define ASSERT(expression) assert(expression)
//#else
	#define ASSERT(expression) static_cast<void>( 0 )
//#endif