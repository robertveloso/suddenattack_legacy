/*****************************************************************************************************************
*****************************************************************************************************************/

#pragma once

#include "..\Application\Peer.h"
#include "../Application/Packet.h"
using namespace Application::Packets;

namespace GamehiSolution{	namespace Session{		namespace Manage{
	class CQueryBuffer
	{
		// 생성 / 소멸자
	public:
		CQueryBuffer()
			:	m_pData			( NULL	)
			,	c_nBufferSize	( CPacket::PredefineLength::GetQuerySize	)
			,	m_nDataSize		( 0		)
			,	m_dwHandle		( NULL	)
		{}
		virtual ~CQueryBuffer(VOID){}

		// Attributes
	private:
		CHAR		m_Buffer[CPacket::PredefineLength::GetQuerySize];		// 버퍼의 실제 메모리 영역
		const INT	c_nBufferSize;											// 실제 버퍼의 크기

		CHAR*		m_pData;		// 유효한 데이터의 포인터 : Send의 경우 짤려서 보내게 되면 이 값이 달라진다.
		INT			m_nDataSize;	// 데이터 크기

		DWORD		m_dwHandle;		// 이 패킷의 소유주의 핸들

	public:
		VOID		CopyData		( const CHAR* pData, INT nDataSize, DWORD dwHandle = 0 );

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
		BOOL		GetHandle	()
		{
			return	m_dwHandle;
		}
	};// CQueryBuffer

	class CRequestObject
	{
	private:
		Application::CPeer*		m_pPeer;
		UINT					m_uiMessage;
		CQueryBuffer*			m_pQueryBuffer;
	public:
		void SetProperty( Application::CPeer* pPeer, UINT uiMessage, CQueryBuffer* pQueryBuffer )
		{
			m_pPeer			= pPeer;
			m_uiMessage		= uiMessage;
			m_pQueryBuffer	= pQueryBuffer;
		}
		Application::CPeer*	GetPeer()		{ return m_pPeer;		}
		UINT				GetMessage()	{ return m_uiMessage;	}
		CQueryBuffer*		GetQueryBuffer(){ return m_pQueryBuffer;}
	};
}/* namespace Manage */		}/* namespace Session */	}/* namespace GamehiSolution */
