// stdafx.cpp : 표준 포함 파일만 들어 있는 소스 파일입니다.
// TiniLibGame.pch는 미리 컴파일된 헤더가 됩니다.
// stdafx.obj에는 미리 컴파일된 형식 정보가 포함됩니다.

#include "stdafx.h"

// TODO: 필요한 추가 헤더는
// 이 파일이 아닌 STDAFX.H에서 참조합니다.

void *
operator new( unsigned int uiItemSize )
{
//	s_ulAllocCount++;

//	SAFE_DEFUNLOCK;

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
//	::FREE( pPointer );

//	SAFE_DEFLOCK;

//	s_ulAllocCount--;

//	SAFE_DEFUNLOCK;
}

void *
operator new[]( unsigned int uiItemSize )
{
//	SAFE_DEFLOCK;

//	s_ulAllocCount++;

//	SAFE_DEFUNLOCK;

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

//	SAFE_DEFLOCK;

//	s_ulAllocCount--;

//	SAFE_DEFUNLOCK;
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