#include "StdAfx.h"
#include ".\QueryBuffer.h"

using namespace GamehiSolution::Session::Manage;

VOID
CQueryBuffer::CopyData( const CHAR* pData, INT nDataSize, DWORD dwHandle )
{
	//ASSERT( nDataSize <= c_nBufferSize );
	//ASSERT( nDataSize > 0 );

	::memcpy( m_Buffer, pData, nDataSize );
	m_pData		= m_Buffer;
	m_nDataSize	= nDataSize;
	m_dwHandle	= dwHandle;
}

VOID
CQueryBuffer::ReplaceDataPos( INT nProcessedSize )
{
	//ASSERT( nProcessedSize < m_nDataSize );
	if( nProcessedSize < 0 )
	{
		int a = 3;
	}
//	ASSERT( nProcessedSize > 0 );

	m_pData		= &m_pData[nProcessedSize];
	m_nDataSize	-= nProcessedSize;
}