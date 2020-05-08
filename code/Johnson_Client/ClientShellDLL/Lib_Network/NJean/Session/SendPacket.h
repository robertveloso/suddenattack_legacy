/*****************************************************************************************************************
*****************************************************************************************************************/

#pragma once

#include <Windows.h>

namespace GamehiSolution
{
	namespace Session
	{
		namespace Manage
		{
			class CSendPacket
			{
				// 持失 / 社瑚切
			public:
				CSendPacket()
				{
					m_aBuffer = NULL;
				}
				~CSendPacket(void)
				{
					Destroy();
				}

				// Attributes
			private:
				char*			m_aBuffer;
				int				m_nBufferSize;
				int				m_nDataSize;
				int				m_nRestSend;
				DWORD			m_dwHandle;
				CSendPacket*	m_pNext;

				// Operations
			public:
				BOOL			Create		( int nBufferSize );
				void			Destroy		( );

				BOOL			Init		( const char* pData, int nDataSize, DWORD dwHandle );
				
				char*			GetData		( );
				
				int				GetDataSize	( );
				void			SetDataSize	( int nDataSize );
				
				int				GetRestSend	( );
				void			SetRestSend	( int nRestSend );
				
				CSendPacket*	GetNext		( );
				void			SetNext		( CSendPacket* pSendPacket );

				BOOL			GetHandle	()
				{
					return	m_dwHandle;
				}
			
				BOOL			IsCreated	()
				{
					return m_aBuffer != NULL;
				}
			};// CSendPacket

		}// namespace Manage

	}// namespace Session

}// namespace GamehiSolution
