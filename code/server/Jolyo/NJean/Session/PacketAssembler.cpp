#include "StdAfx.h"
#include ".\packetassembler.h"

using namespace Session::Message;
using namespace Session::Manage;


DWORD CPacketAssembler::g_dwHandle = 0;
CPacketAssembler::CPacketAssembler(void)
:	m_aBuffer			( NULL	)
,	m_nBufferSize		( 0		)
,	m_aSendBuffer		( NULL	)
,	m_nSendBufferSize	( 0		)
,	m_cTerminatedChar	( NULL	)
{
	m_LengthFieldInfo.nPos			= -1;
	m_LengthFieldInfo.nSize			= 0;
	m_LengthFieldInfo.bLittleEndian	= true;

	m_AssembleData.bFirst			= true;
	m_AssembleData.nLength			= 0;
	m_AssembleData.pDataBuffer		= NULL;
	m_AssembleData.cFirstChar		= NULL;

	m_nQueueingCount				= 0;
}

CPacketAssembler::~CPacketAssembler(void)
{
}

// ³»ºÎ ¹öÆÛ¸¦ »ý¼ºÇÑ´Ù.
BOOL CPacketAssembler::Create( int nBufferSize, INT nSendPacketListSize )
{
	m_aBuffer = new char[nBufferSize];
	if( m_aBuffer == NULL )
		return false;
	m_nBufferSize = nBufferSize;


	m_aSendBuffer = new char[nBufferSize];
	if( m_aSendBuffer == NULL )
		return false;
	m_nSendBufferSize = nBufferSize;

	m_pSendPacketHead			= NULL;
	m_pSendPacketRear			= NULL;
//	m_dwHandle					= 0;

	m_AssembleData.bFirst		= true;
	m_AssembleData.nLength		= 0;
	m_AssembleData.pDataBuffer	= NULL;
	m_AssembleData.cFirstChar	= NULL;

	m_bCloseWait				= FALSE;

	m_bFree						= FALSE;
	m_bOver						= FALSE;

	m_dwHandle					= g_dwHandle++;

	m_lstPacketBuffer.Create( nSendPacketListSize );
//	m_lstFirstPacketBuffer.Create( 50 );

	::InitializeCriticalSection( &m_cs );

//	char a1[100];
//	sprintf( a1, "Alloc %d\r\n", (int)this );
//	::OutputDebugString( a1 );

	return true;
}

// ³»ºÎ ¹öÆÛ¸¦ Á¦°ÅÇÑ´Ù.
void CPacketAssembler::Destroy()
{
	if( m_aBuffer )
	{
		delete[] m_aBuffer;

		m_aBuffer		= NULL;
		m_nBufferSize	= 0;
	}
	if( m_aSendBuffer )
	{
		delete[] m_aSendBuffer;

		m_aSendBuffer		= NULL;
		m_nSendBufferSize	= 0;
	}

	::DeleteCriticalSection( &m_cs );
}

// Á¶¸³ÇÒ ÆÐÅ¶ÀÇ Å¸ÀÔÀ» ¼³Á¤ÇÑ´Ù.
void CPacketAssembler::Init( CPacketCheckSum::CheckSumType checkSumType, PacketUnitType packetUnitType )
{
	m_PacketCheckSumType	= checkSumType;
	m_PacketUnitType		= packetUnitType;

	
	if( m_lstPacketBuffer.GetElementsCount() > 0 )
	{
		int a = 3;
	}
	if( m_lstPacketBuffer.GetElementsCount() < 0 )
	{
		int a = 3;
	}
	
//	ASSERT( m_lstPacketBuffer.GetElementsCount() == 0 );
//	ASSERT( m_lstFirstPacketBuffer.GetElementsCount() == 0 );

	m_nQueueingCount		= 0;

	m_dwSendIndex			= 1;
	m_dwAllocSendIndex		= 1;
}

// ÉèÖÃÏûÏ¢°üµÄÊôÐÔ¡£µ÷ÓÃ´ÎÐò£º
//1£ºconnect»òAccep£»
//2£ºITcpNetworkHandler::OnAllocNetworkObject;ÏÈ½«Ò»¸öCPacketAssemblerÓësocket¹Ø¼ü×Ö½øÐÐ¶ÔÓ¦,È»ºóInitPacketType->InitLengthFieldOfPacket
//3: ITcpNetworkHandlerÊµÏÖÕßCSessionManager::InitLengthFieldOfPacketÖÐµ÷ÓÃ¸Ãº¯Êý
void CPacketAssembler::InitLengthField( int nLengthPos, LengthSizeType lengthSizeType, LengthType lengthType/* = LengthType::Total*/, BOOL bLittleEndian/* = true*/ )
{
	m_LengthFieldInfo.nPos			= nLengthPos;
	m_LengthFieldInfo.nSize			= (int)lengthSizeType;
	m_LengthFieldInfo.bLittleEndian	= bLittleEndian;
}

// ¹®ÀÚ ±â¹Ý ÆÐÅ¶¿¡¼­ Á¾·á ¹®ÀÚ¸¦ ¼³Á¤ÇÑ´Ù.
void CPacketAssembler::InitTerminatedCharector( char cTerminatedChar )
{
	m_cTerminatedChar	= cTerminatedChar;
}

// Á¶¸³À» ÇÑ´Ù.
BOOL CPacketAssembler::Assemble( char* buffer, int nBufferSize )
{
	if( nBufferSize > m_nBufferSize / 2 )
	{
		return false;
	}

	//
	// Ã¹¹øÂ° µ¥ÀÌÅ¸ÀÌ¸é À§Å¹ÇÑ ¹öÆÛ¸¦ ±×´ë·Î »ç¿ëÇÑ´Ù.
	//
	if( m_AssembleData.bFirst )
	{
		m_AssembleData.pDataBuffer	= buffer;
		m_AssembleData.nLength		= nBufferSize;
		m_AssembleData.bFirst		= false;
	}

	//
	// Ã¹¹øÂ° µ¥ÀÌÅ¸°¡ ¾Æ´Ï¸é ÇöÀç ¹öÆÛ µÚ¿¡ copy¸¦ ÇÑ´Ù.
	//
	else
	{
		if( nBufferSize + m_AssembleData.nLength > m_nBufferSize )
		{
			return false;
		}

		// ³»ºÎ ¹öÆÛ¸¦ »ç¿ëÁßÀÌ¸é µÚ¿¡ copy¸¦ ÇÑ´Ù.
		/*
		if( m_AssembleData.pDataBuffer == m_aBuffer )
		{
			::memcpy( &m_AssembleData.pDataBuffer[ m_AssembleData.nLength ], buffer, nBufferSize );
			m_AssembleData.nLength += nBufferSize;
		}
		else
		{
			// »õ·Î ¹ÞÀº µ¥ÀÌÅ¸¸¦ µÚ¿¡ Copy
			::memcpy( &m_AssembleData.pDataBuffer[ m_AssembleData.nLength ], buffer, nBufferSize );
			m_AssembleData.nLength += nBufferSize;
		}
		*/
		::memcpy( &m_AssembleData.pDataBuffer[ m_AssembleData.nLength ], buffer, nBufferSize );
		m_AssembleData.nLength += nBufferSize;
	}

	return true;
}

// Á¶¸³µÈ ÇÏ³ªÀÇ ¿ÏÀüÇÑ ÆÐÅ¶À» ¹ÝÈ¯ÇÑ´Ù.
void* CPacketAssembler::GetCompletePacket( int& nOutPacketSize, int& nOutResult )
{
	switch( m_PacketUnitType )
	{
	case PacketUnitType::Length :
		return GetCompletePacketLengthType( nOutPacketSize, nOutResult );

	case PacketUnitType::Charector :
		return GetCompletePacketCharType( nOutPacketSize, nOutResult );
	}

	return NULL;
}

// Assemble ÀÛ¾÷À» ResetÇÑ´Ù.
void CPacketAssembler::Reset()
{
	m_AssembleData.bFirst		= true;
	m_AssembleData.nLength		= 0;
	m_AssembleData.pDataBuffer	= NULL;
	m_AssembleData.cFirstChar	= NULL;

	m_bCloseWait				= TRUE;


	if( m_bOver == TRUE )
		int a = 3;
	m_bOver						= FALSE;
	m_bFree						= FALSE;
}

void CPacketAssembler::AddCheckSum( const char* pData, int nDataLength )
{
	::memcpy( m_aSendBuffer, pData, nDataLength );

	CPacketCheckSum::SetCheckSum( m_PacketCheckSumType, (char*)m_aSendBuffer, nDataLength );
	m_nSendBufferSize = nDataLength + CPacketCheckSum::GetCheckSumSize( m_PacketCheckSumType );
}

void CPacketAssembler::SetCheckSum( CHAR* pData, INT nDataLength, INT& out_nDataLength )
{
	CPacketCheckSum::SetCheckSum( m_PacketCheckSumType, pData, nDataLength );
	out_nDataLength = nDataLength + CPacketCheckSum::GetCheckSumSize( m_PacketCheckSumType );
}

// ±æÀÌ ±â¹ÝÀÏ ¶§ÀÇ ÆÐÅ¶À» ¹ÝÈ¯ÇÑ´Ù.
inline void* CPacketAssembler::GetCompletePacketLengthType( int& nOutPacketSize, int& nOutResult )
{

	// Length : 2Byte
	// Packet : Length + 4Byte
	// Tale : 2Byte

	nOutPacketSize = 0;
	nOutResult = 0;

//	int		nBaseLengthFieldSize	= 2;
	int		nCheckSumSize			= CPacketCheckSum::GetCheckSumSize( m_PacketCheckSumType );

	//
	// Length Field¸¦ ¹ÞÁö ¸øÇÏ¸é NULLÀ» ¹ÝÈ¯ÇÑ´Ù.
	//
	if( m_AssembleData.nLength < m_LengthFieldInfo.nSize )	//²»×ãÒ»¸ö°üµÄ»°£¬²»ÌáÈ¡
	{
		// À§Å¹ÇÑ ¹öÆÛ¸¦ ³»ºÎ ¹öÆÛ·Î Copy
		::memcpy( m_aBuffer, m_AssembleData.pDataBuffer, m_AssembleData.nLength );
		m_AssembleData.pDataBuffer = m_aBuffer;

		return NULL;
	}

	//
	// Length Field¸¦ ÅëÇØ¼­ packetÀÇ length¸¦ ±¸ÇÑ´Ù.
	//
	char* pLengthField = &m_AssembleData.pDataBuffer[m_LengthFieldInfo.nPos];
	if( m_LengthFieldInfo.bLittleEndian )
	{
		short	nLength;
		::memcpy( &nLength, pLengthField, m_LengthFieldInfo.nSize );
		m_AssembleData.nPacketLength = nLength;// + Ã¥½æ½ÎÀÌÁî
	}
	else
	{
	}

	//
	// Àß¸øµÈ Length ÇÊµåÀÏ °æ¿ì, Exception ¹ß»ý
	//
	if( m_AssembleData.nPacketLength < m_LengthFieldInfo.nPos ||
		m_AssembleData.nPacketLength > m_nBufferSize / 2 )
	{

//		throw 0;
		nOutResult = 1;
		return NULL;
	}

	//
	// ÆÐÅ¶Àº ´ÙÀ½ 3°¡Áö °æ¿ì·Î ±¸ºÐµÈ´Ù.
	//		Case 1. ÇöÀç±îÁöÀÇ ¸Þ½ÃÁö°¡ ÇÏ³ªÀÇ ÆÐÅ¶ÀÎ °æ¿ì
	//		Case 2. ÇöÀç±îÁöÀÇ ¸Þ½ÃÁö°¡ ÆÐÅ¶ÀÇ Áß°£±îÁö¸¸ ¿Â °æ¿ì
	//		Case 3. ÇöÀç±îÁöÀÇ ¸Þ½ÃÁö°¡ ÇÏ³ª ÀÌ»óÀÇ ÆÐÅ¶ÀÎ °æ¿ì
	//

	// Case 1
	if( m_AssembleData.nLength ==
		m_AssembleData.nPacketLength + CPacketCheckSum::GetCheckSumSize( m_PacketCheckSumType ) )
	{
		// Ã¤Å©¼¶ÀÌ ¸Â´ÂÁö¸¦ ºñ±³ÇÑ´Ù.
		if( !CPacketCheckSum::IsCorrectCheckSum(
				m_PacketCheckSumType,
				m_AssembleData.pDataBuffer,
				m_AssembleData.nPacketLength ) )
		{
			nOutResult = 2;
			return NULL;
		}

		char* pDataBuffer;
		
		// Length TypeÀÌ ¾ø´Ù.
		// Length TypeÀº ÇöÀç Total¸¸ Áö¿øµÈ´Ù.

		pDataBuffer = m_AssembleData.pDataBuffer;

//		pDataBuffer[m_AssembleData.nPacketLength-nCheckSumSize] = NULL;

		m_AssembleData.bFirst		= true;
		m_AssembleData.nLength		= 0;
		m_AssembleData.pDataBuffer	= NULL;
		m_AssembleData.cFirstChar	= NULL;

		nOutPacketSize = m_AssembleData.nPacketLength;// + nCheckSumSize;// + 1;	// +1 : NULL°ª Æ÷ÇÔ

		return pDataBuffer;
	}

	// Case 2
	else if( m_AssembleData.nLength <
		m_AssembleData.nPacketLength + CPacketCheckSum::GetCheckSumSize( m_PacketCheckSumType ) )
	{
		// À§Å¹ÇÑ ¹öÆÛ¸¦ ³»ºÎ ¹öÆÛ·Î Copy
		::memcpy( m_aBuffer, m_AssembleData.pDataBuffer, m_AssembleData.nLength );
		m_AssembleData.pDataBuffer = m_aBuffer;

		return NULL;
	}

	// Case 3
	else
	{
		// Ã¤Å©¼¶ÀÌ ¸Â´ÂÁö¸¦ ºñ±³ÇÑ´Ù.
		if( !CPacketCheckSum::IsCorrectCheckSum(
				m_PacketCheckSumType,
				m_AssembleData.pDataBuffer,
				m_AssembleData.nPacketLength ) )
		{
			nOutResult = 3;
			return NULL;
		}

		char* pDataBuffer;
		
		// Length TypeÀÌ ¾ø´Ù.
		// Length TypeÀº ÇöÀç Total¸¸ Áö¿øµÈ´Ù.

		pDataBuffer = m_AssembleData.pDataBuffer;

//		pDataBuffer[m_AssembleData.nPacketLength-nCheckSumSize] = NULL;

		m_AssembleData.nLength		= m_AssembleData.nLength - m_AssembleData.nPacketLength - nCheckSumSize;
		m_AssembleData.pDataBuffer	= &m_AssembleData.pDataBuffer[m_AssembleData.nPacketLength+nCheckSumSize];

		nOutPacketSize = m_AssembleData.nPacketLength;// + nCheckSumSize;


		return pDataBuffer;
	}


	return NULL;
}

// ¹®ÀÚ ±â¹ÝÀÏ ¶§ÀÇ Á¶¸³À» ÇÑ´Ù.
inline void* CPacketAssembler::GetCompletePacketCharType( int& nPacketSize, int& nOutResult )
{
	return NULL;
}


BOOL CPacketAssembler::IsEmptySendList()
{
	if( m_pSendPacketHead == NULL )
		return TRUE;
	else
		return FALSE;
}

CSendPacket* CPacketAssembler::RemoveFromSendList( int nSendingLength )
{
	if( m_pSendPacketHead == NULL )
	{
		return NULL;
	}

	ASSERT( m_pSendPacketRear );

	CSendPacket*	pSendPacket	= m_pSendPacketHead;
	int				nRestSend	= pSendPacket->GetRestSend() - nSendingLength;

	if( nRestSend > 0 )
	{
		pSendPacket->SetRestSend( nRestSend );
		return NULL;
	}
	else
	{
		m_pSendPacketHead = pSendPacket->GetNext();
		if( m_pSendPacketHead == NULL )
			m_pSendPacketRear = NULL;
		m_nQueueingCount--;
		return pSendPacket;
	}
}

void CPacketAssembler::AddToSendList( CSendPacket* pSendPacket, BOOL bFirst )
{
	CPacketCheckSum::SetCheckSum( m_PacketCheckSumType, pSendPacket->GetData(), pSendPacket->GetDataSize() );
	pSendPacket->SetDataSize( pSendPacket->GetDataSize() + CPacketCheckSum::GetCheckSumSize( m_PacketCheckSumType ) );

	ASSERT( pSendPacket );

	// bFirst : Q¿¡ ¿ì¼±ÀûÀ¸·Î Enqueue

	if( !bFirst )
	{
		if( m_pSendPacketHead == NULL )
		{
			m_pSendPacketHead = m_pSendPacketRear = pSendPacket;
		}
		else
		{
			ASSERT( m_pSendPacketRear );
			m_pSendPacketRear->SetNext( pSendPacket );
			m_pSendPacketRear = pSendPacket;
		}

		// For Debug
		ASSERT( pSendPacket != pSendPacket->GetNext() );
		ASSERT( m_pSendPacketHead != m_pSendPacketHead->GetNext() );
		
	}
	else
	{
		if( m_pSendPacketHead )
		{
			pSendPacket->SetNext( m_pSendPacketHead->GetNext() );
			m_pSendPacketHead->SetNext( pSendPacket );
			if( m_pSendPacketHead == m_pSendPacketRear )
				m_pSendPacketRear = pSendPacket;

			ASSERT( m_pSendPacketRear != NULL );
		}
		else
		{
			m_pSendPacketHead = m_pSendPacketRear = pSendPacket;
		}

		// For Debug
		ASSERT( pSendPacket != pSendPacket->GetNext() );
		ASSERT( m_pSendPacketHead != m_pSendPacketHead->GetNext() );
	}
	m_nQueueingCount++;
}

CSendPacket* CPacketAssembler::FreeSendList()
{
	if( m_pSendPacketHead == NULL )
		return NULL;
	CSendPacket* pSendPacket	= m_pSendPacketHead;
	m_pSendPacketHead			= m_pSendPacketHead->GetNext();
	if( m_pSendPacketHead == NULL )
		m_pSendPacketRear = NULL;

	m_nQueueingCount--;
	return pSendPacket;
}

char* CPacketAssembler::GetSendBuffer()
{
	return m_pSendPacketHead->GetData();
}

int CPacketAssembler::GetSendSize()
{
	if( m_pSendPacketHead == NULL )
	{
		return 0;
	}
	return m_pSendPacketHead->GetRestSend();
}
