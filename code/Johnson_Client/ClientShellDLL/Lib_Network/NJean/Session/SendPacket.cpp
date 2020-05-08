#include "StdAfx.h"
#include ".\SendPacket.h"

using namespace GamehiSolution::Session::Manage;

BOOL CSendPacket::Create( int nBufferSize )
{
	m_aBuffer		= new char[nBufferSize];

	m_nBufferSize	= nBufferSize;
	if( !m_aBuffer )
		return FALSE;
	return TRUE;
}

void CSendPacket::Destroy()
{
	if( m_aBuffer )
		delete[] m_aBuffer;
	m_aBuffer = NULL;
}

BOOL CSendPacket::Init( const char* pData, int nDataSize, DWORD dwHandle )
{
	if( nDataSize > m_nBufferSize || nDataSize <= 0 )
		return FALSE;

	::memcpy( m_aBuffer, pData, nDataSize );
	m_nDataSize	= nDataSize;
	m_nRestSend	= nDataSize;
	m_pNext		= NULL;

	m_dwHandle	= dwHandle;

	return TRUE;
}

char* CSendPacket::GetData()
{
	return &m_aBuffer[ m_nDataSize - m_nRestSend ];
}

int CSendPacket::GetDataSize()
{
	return m_nDataSize;
}

void CSendPacket::SetDataSize( int nDataSize )
{
	m_nDataSize = nDataSize;
	m_nRestSend = nDataSize;
}

int CSendPacket::GetRestSend()
{
	return m_nRestSend;
}

void CSendPacket::SetRestSend( int nRestSend )
{
	m_nRestSend = nRestSend;
}

CSendPacket* CSendPacket::GetNext()
{
	return m_pNext;
}

void CSendPacket::SetNext(CSendPacket* pSendPacket)
{
	m_pNext = pSendPacket;
}
