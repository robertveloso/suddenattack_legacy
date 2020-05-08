/*
	InternetWrite및 Read에서 큰 용량 처리 부분이 빠져있다.
	현재는 작은 Command 단위의 송수신이라 문제가 없으나,
	대용량 처리를 위해서 구현을 해두어야 한다.
*/

#pragma once

#include "IQueryHandler.h"
#include <wininet.h>

namespace Query
{
	namespace HTTP
	{
		class CSyncInet
		{
		public:
			CSyncInet(void);
			~CSyncInet(void);

		public:

		private:
			HINTERNET		m_hOpen;
			HINTERNET		m_hRequest;

			int				m_nCount;

		public:
			bool		Create	( void );
			void		Destroy	( void );

			BOOL		Request	( LPCTSTR lpszQuery, DWORD &dwError );	// Get방식
			BOOL		Response( ULONG_PTR lpInetKey, CHAR* lpBuffersOut, DWORD dwBufferSize, DWORD &dwNumberOfBytesRead );
			INT			HttpResult( ULONG_PTR lpInetKey );

			bool		Connect	(
				LPCTSTR lpszServerName,			// 접속할 서버 주소 ex> 61.106.140.203
				LPCTSTR lpszObjectName,			// 문서 경로 ex> /scripts/test.asp
				LPCTSTR lpszBody,
				unsigned short port,		// 접속 포트
				LPCTSTR lpszUserName, 
				LPCTSTR lpszPassword, 
				DWORD dwFlags,
				char* lpBuffersOut, 
				DWORD dwBufferSize,
				DWORD * dwReturnSize
				);

		};// class CSyncInet

	}// namespace HTTP

}// namespace Query
