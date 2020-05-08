/*****************************************************************************************************************
*****************************************************************************************************************/

#pragma once

#include "../Application/Packet.h"
using namespace Application::Packets;

namespace GamehiSolution{	namespace Session{		namespace Manage{
	class CPacketBuffer
	{
		// 생성 / 소멸자
	public:
		CPacketBuffer()
			:	m_pData			( m_Buffer)
			,	c_nBufferSize	( CPacket::PredefineLength::BasePacketSize	)
			,	m_nDataSize		( 0		)
			,	m_dwHandle		( NULL	)
		{}
		virtual ~CPacketBuffer(VOID){}

		// Attributes
	private:
		CHAR		m_Buffer[CPacket::PredefineLength::BasePacketSize];		// 버퍼의 실제 메모리 영역
		const INT	c_nBufferSize;											// 실제 버퍼의 크기

		CHAR*		m_pData;		// 유효한 데이터의 포인터 : Send의 경우 짤려서 보내게 되면 이 값이 달라진다.
		INT			m_nDataSize;	// 데이터 크기

		DWORD		m_dwHandle;		// 이 패킷의 소유주의 핸들
		DWORD		m_dwSendIndex;	// Send 순서 번호

	public:
		VOID		CopyData		( const CHAR* pData, INT nDataSize );
		VOID		CopyData		( const CHAR* pData, INT nDataSize, DWORD dwHandle/*, DWORD dwSendIndex*/ );

		VOID		ReplaceDataPos	( INT nProcessedSize );

		// Get / Set
		BOOL		IsCreated	()
		{
			return m_Buffer != NULL;
		}
		CHAR*		GetData		()
		{
			return	m_pData;
		}
		VOID		SetDataSize ( INT nDataSize )
		{
			m_nDataSize = nDataSize;
		}
		INT			GetDataSize	()
		{
			return	m_nDataSize;
		}
		INT			GetBufferSize()
		{
			return	c_nBufferSize;
		}
		DWORD		GetHandle	()
		{
			return	m_dwHandle;
		}
		DWORD		GetSendIndex()
		{
			return m_dwSendIndex;
		}
		VOID		SetSendIndex( DWORD dwSendIndex )
		{
			m_dwSendIndex = dwSendIndex;
		}
	};// CPacketBuffer

}/* namespace Manage */		}/* namespace Session */	}/* namespace GamehiSolution */
