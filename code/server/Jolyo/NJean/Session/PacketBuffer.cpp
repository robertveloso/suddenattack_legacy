#include "StdAfx.h"
#include ".\PacketBuffer.h"

using namespace Session::Manage;

VOID
CPacketBuffer::CopyData( const CHAR* pData, INT nDataSize )
{
	ASSERT( nDataSize <= c_nBufferSize );
	ASSERT( nDataSize > 0 );

	::memcpy( m_Buffer, pData, nDataSize );
	m_pData			= m_Buffer;
	m_nDataSize		= nDataSize;
	m_dwHandle		= 0;
	m_dwSendIndex	= 0;
}

VOID
CPacketBuffer::CopyData( const CHAR* pData, INT nDataSize, DWORD dwHandle, DWORD dwSendIndex )
{
	if( nDataSize == 0 )
	{
		m_nDataSize		= nDataSize;
		m_dwSendIndex	= dwSendIndex;
		return;
	}

	ASSERT( nDataSize <= c_nBufferSize );
	ASSERT( nDataSize > 0 );

	::memcpy( m_Buffer, pData, nDataSize );
	m_pData			= m_Buffer;
	m_nDataSize		= nDataSize;
	m_dwHandle		= dwHandle;
	m_dwSendIndex	= dwSendIndex;
}

VOID
CPacketBuffer::ReplaceDataPos( INT nProcessedSize )
{
	ASSERT( nProcessedSize < m_nDataSize );
	if( nProcessedSize < 0 )
	{
		int a = 3;
	}
//	ASSERT( nProcessedSize > 0 );

	m_pData		= &m_pData[nProcessedSize];
	m_nDataSize	-= nProcessedSize;
}