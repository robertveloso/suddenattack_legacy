#include "StdAfx.h"
#include ".\packetassembler.h"

using namespace GamehiSolution::Session::Message;
using namespace GamehiSolution::Session::Manage;


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
	Destroy();
}

// 내부 버퍼를 생성한다.
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

	m_dwHandle					= g_dwHandle++;

	m_lstPacketBuffer.Create( nSendPacketListSize );
	m_lstFirstPacketBuffer.Create( 50 );

	::InitializeCriticalSection( &m_cs );

	return true;
}

// 내부 버퍼를 제거한다.
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

	m_lstFirstPacketBuffer.Destroy();
	m_lstPacketBuffer.Destroy();

	::DeleteCriticalSection( &m_cs );
}

// 조립할 패킷의 타입을 설정한다.
void CPacketAssembler::Init( CPacketCheckSum::CheckSumType checkSumType, PacketUnitType packetUnitType )
{
	m_PacketCheckSumType	= checkSumType;
	m_PacketUnitType		= packetUnitType;

	/*
	if( m_lstPacketBuffer.GetElementsCount() > 0 )
	{
		int a = 3;
	}
	if( m_lstPacketBuffer.GetElementsCount() < 0 )
	{
		int a = 3;
	}
	*/
	//ASSERT( m_lstPacketBuffer.GetElementsCount() == 0 );
	//ASSERT( m_lstFirstPacketBuffer.GetElementsCount() == 0 );

	m_nQueueingCount		= 0;

	m_dwSendIndex			= 0;
	m_dwAllocSendIndex		= 0;
	m_SendPacketList		= SendPacketList::None;
}

// 길이 기반 패킷에서 Length 필드를 설정한다.
void CPacketAssembler::InitLengthField( int nLengthPos, LengthSizeType lengthSizeType, LengthType lengthType/* = LengthType::Total*/, BOOL bLittleEndian/* = true*/ )
{
	m_LengthFieldInfo.nPos			= nLengthPos;
	m_LengthFieldInfo.nSize			= (int)lengthSizeType;
	m_LengthFieldInfo.bLittleEndian	= bLittleEndian;
}

// 문자 기반 패킷에서 종료 문자를 설정한다.
void CPacketAssembler::InitTerminatedCharector( char cTerminatedChar )
{
	m_cTerminatedChar	= cTerminatedChar;
}

// 조립을 한다.
BOOL CPacketAssembler::Assemble( char* buffer, int nBufferSize )
{
	if( nBufferSize > m_nBufferSize / 2 )
	{
		return false;
	}

	//
	// 첫번째 데이타이면 위탁한 버퍼를 그대로 사용한다.
	//
	if( m_AssembleData.bFirst )
	{
		m_AssembleData.pDataBuffer	= buffer;
		m_AssembleData.nLength		= nBufferSize;
		m_AssembleData.bFirst		= false;
	}

	//
	// 첫번째 데이타가 아니면 현재 버퍼 뒤에 copy를 한다.
	//
	else
	{
		if( nBufferSize + m_AssembleData.nLength > m_nBufferSize )
		{
			return false;
		}

		// 내부 버퍼를 사용중이면 뒤에 copy를 한다.
		/*
		if( m_AssembleData.pDataBuffer == m_aBuffer )
		{
			::memcpy( &m_AssembleData.pDataBuffer[ m_AssembleData.nLength ], buffer, nBufferSize );
			m_AssembleData.nLength += nBufferSize;
		}
		else
		{
			// 새로 받은 데이타를 뒤에 Copy
			::memcpy( &m_AssembleData.pDataBuffer[ m_AssembleData.nLength ], buffer, nBufferSize );
			m_AssembleData.nLength += nBufferSize;
		}
		*/
		::memcpy( &m_AssembleData.pDataBuffer[ m_AssembleData.nLength ], buffer, nBufferSize );
		m_AssembleData.nLength += nBufferSize;
	}

	return true;
}

// 조립된 하나의 완전한 패킷을 반환한다.
void* CPacketAssembler::GetCompletePacket( int& nOutPacketSize )
{
	switch( m_PacketUnitType )
	{
	case PacketUnitType::Length :
		return GetCompletePacketLengthType( nOutPacketSize );

	case PacketUnitType::Charector :
		return GetCompletePacketCharType( nOutPacketSize );
	}

	return NULL;
}

// Assemble 작업을 Reset한다.
void CPacketAssembler::Reset()
{
	m_AssembleData.bFirst		= true;
	m_AssembleData.nLength		= 0;
	m_AssembleData.pDataBuffer	= NULL;
	m_AssembleData.cFirstChar	= NULL;
	m_dwHandle = g_dwHandle++;

	m_bCloseWait				= TRUE;


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

// 길이 기반일 때의 패킷을 반환한다.
inline void* CPacketAssembler::GetCompletePacketLengthType( int& nOutPacketSize )
{

	// Length : 2Byte
	// Packet : Length + 4Byte
	// Tale : 2Byte

	nOutPacketSize = 0;

//	int		nBaseLengthFieldSize	= 2;
	int		nCheckSumSize			= CPacketCheckSum::GetCheckSumSize( m_PacketCheckSumType );

	//
	// Length Field를 받지 못하면 NULL을 반환한다.
	//
	if( m_AssembleData.nLength < m_LengthFieldInfo.nSize )
	{
		// 위탁한 버퍼를 내부 버퍼로 Copy
		::memcpy( m_aBuffer, m_AssembleData.pDataBuffer, m_AssembleData.nLength );
		m_AssembleData.pDataBuffer = m_aBuffer;

		return NULL;
	}

	//
	// Length Field를 통해서 packet의 length를 구한다.
	//
	char* pLengthField = &m_AssembleData.pDataBuffer[m_LengthFieldInfo.nPos];
	if( m_LengthFieldInfo.bLittleEndian )
	{
		short	nLength;
		::memcpy( &nLength, pLengthField, m_LengthFieldInfo.nSize );
		m_AssembleData.nPacketLength = nLength;// + 책썸싸이즈
	}
	else
	{
	}

	//
	// 잘못된 Length 필드일 경우, Exception 발생
	//
	if( m_AssembleData.nPacketLength < m_LengthFieldInfo.nPos ||
		m_AssembleData.nPacketLength > m_nBufferSize / 2 )
	{

		throw 0;
	}

	//
	// 패킷은 다음 3가지 경우로 구분된다.
	//		Case 1. 현재까지의 메시지가 하나의 패킷인 경우
	//		Case 2. 현재까지의 메시지가 패킷의 중간까지만 온 경우
	//		Case 3. 현재까지의 메시지가 하나 이상의 패킷인 경우
	//

	// Case 1
	if( m_AssembleData.nLength ==
		m_AssembleData.nPacketLength + CPacketCheckSum::GetCheckSumSize( m_PacketCheckSumType ) )
	{
		// 채크섬이 맞는지를 비교한다.
		if( !CPacketCheckSum::IsCorrectCheckSum(
				m_PacketCheckSumType,
				m_AssembleData.pDataBuffer,
				m_AssembleData.nPacketLength ) )
		{
			throw 1;
		}

		char* pDataBuffer;
		
		// Length Type이 없다.
		// Length Type은 현재 Total만 지원된다.

		pDataBuffer = m_AssembleData.pDataBuffer;

//		pDataBuffer[m_AssembleData.nPacketLength-nCheckSumSize] = NULL;

		m_AssembleData.bFirst		= true;
		m_AssembleData.nLength		= 0;
		m_AssembleData.pDataBuffer	= NULL;
		m_AssembleData.cFirstChar	= NULL;

		nOutPacketSize = m_AssembleData.nPacketLength;// + nCheckSumSize;// + 1;	// +1 : NULL값 포함

		return pDataBuffer;
	}

	// Case 2
	else if( m_AssembleData.nLength <
		m_AssembleData.nPacketLength + CPacketCheckSum::GetCheckSumSize( m_PacketCheckSumType ) )
	{
		// 위탁한 버퍼를 내부 버퍼로 Copy
		::memcpy( m_aBuffer, m_AssembleData.pDataBuffer, m_AssembleData.nLength );
		m_AssembleData.pDataBuffer = m_aBuffer;

//		TRACE( "OK %d\r\n", GetTickCount() );
		return NULL;
	}

	// Case 3
	else
	{
		// 채크섬이 맞는지를 비교한다.
		if( !CPacketCheckSum::IsCorrectCheckSum(
				m_PacketCheckSumType,
				m_AssembleData.pDataBuffer,
				m_AssembleData.nPacketLength ) )
		{
			throw 2;
		}

		char* pDataBuffer;
		
		// Length Type이 없다.
		// Length Type은 현재 Total만 지원된다.

		pDataBuffer = m_AssembleData.pDataBuffer;

//		pDataBuffer[m_AssembleData.nPacketLength-nCheckSumSize] = NULL;

		m_AssembleData.nLength		= m_AssembleData.nLength - m_AssembleData.nPacketLength - nCheckSumSize;
		m_AssembleData.pDataBuffer	= &m_AssembleData.pDataBuffer[m_AssembleData.nPacketLength+nCheckSumSize];

		nOutPacketSize = m_AssembleData.nPacketLength;// + nCheckSumSize;


		return pDataBuffer;
	}


	return NULL;
}

// 문자 기반일 때의 조립을 한다.
inline void* CPacketAssembler::GetCompletePacketCharType( int& nPacketSize )
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

	//ASSERT( m_pSendPacketRear );

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

	//ASSERT( pSendPacket );

	// bFirst : Q에 우선적으로 Enqueue

	if( !bFirst )
	{
		if( m_pSendPacketHead == NULL )
		{
			m_pSendPacketHead = m_pSendPacketRear = pSendPacket;
		}
		else
		{
			//ASSERT( m_pSendPacketRear );
			m_pSendPacketRear->SetNext( pSendPacket );
			m_pSendPacketRear = pSendPacket;
		}

		// For Debug
		//ASSERT( pSendPacket != pSendPacket->GetNext() );
		//ASSERT( m_pSendPacketHead != m_pSendPacketHead->GetNext() );
		
	}
	else
	{
		if( m_pSendPacketHead )
		{
			pSendPacket->SetNext( m_pSendPacketHead->GetNext() );
			m_pSendPacketHead->SetNext( pSendPacket );
			if( m_pSendPacketHead == m_pSendPacketRear )
				m_pSendPacketRear = pSendPacket;

			//ASSERT( m_pSendPacketRear != NULL );
		}
		else
		{
			m_pSendPacketHead = m_pSendPacketRear = pSendPacket;
		}

		// For Debug
		//ASSERT( pSendPacket != pSendPacket->GetNext() );
		//ASSERT( m_pSendPacketHead != m_pSendPacketHead->GetNext() );
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
