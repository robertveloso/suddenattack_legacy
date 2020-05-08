#include "stdafx.h"
#include "./SyncInet.h"

using namespace Query::HTTP;

CSyncInet::CSyncInet(void) :	
m_hOpen				(NULL)
{
}

CSyncInet::~CSyncInet(void)
{
}

bool CSyncInet::Create(  )
{
	m_hRequest		= NULL;
	m_hOpen			= ::InternetOpen( "HTTP", INTERNET_OPEN_TYPE_DIRECT , NULL, NULL, NULL );
	if( !m_hOpen )
		return false;

m_nCount=0;
	return true;
}

void CSyncInet::Destroy( void )
{
	if( m_hOpen )
		::InternetCloseHandle( m_hOpen );
	m_hOpen = NULL;
}

bool CSyncInet::Connect(	LPCTSTR lpszServerName, 
							LPCTSTR lpszObjectName, 
							LPCTSTR lpszBody,
							unsigned short port,		// 접속 포트
							LPCTSTR lpszUserName, 
							LPCTSTR lpszPassword, 
							DWORD dwFlags,
							char* lpBuffersOut, 
							DWORD dwBufferSize,
							DWORD * dwReturnSize
							)
{
	//cout << "Add " << pContext->m_index << " + " << m_RequestContextPool->m_useCount << endl;
	HINTERNET	hConnect;
	HINTERNET	hRequest;

	hConnect = ::InternetConnect( m_hOpen, lpszServerName, port, lpszUserName, lpszPassword, INTERNET_SERVICE_HTTP, dwFlags, NULL );
	if( !hConnect )
	{
		return false;
	}	

	hRequest = ::HttpOpenRequest(	hConnect, 
									"POST", 
									lpszObjectName,
									NULL,
									NULL,
									NULL,
									INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE,
									NULL );

	if( !hRequest )
	{
		::InternetCloseHandle( hConnect );
		return false;
	}

    INTERNET_BUFFERS InetBuff;

	char postheader[50];
	strcpy(postheader, "Content-Type: application/x-www-form-urlencoded");
	//send the postdata along with the content-type header.
	if(!HttpSendRequest(hRequest, postheader, static_cast<INT>( strlen(postheader) ), (LPVOID)lpszBody, static_cast<INT>( strlen(lpszBody) )))
	{
		::InternetCloseHandle( hConnect );
		::InternetCloseHandle( hRequest );
		return false;
	}

	char	header[1024];
	DWORD	nLen = 1024;

	if( !::HttpQueryInfo( hRequest, HTTP_QUERY_RAW_HEADERS_CRLF, header, &nLen, NULL ) )
	{
		DWORD e = GetLastError();
		::InternetCloseHandle( hConnect );
		::InternetCloseHandle( hRequest );
		return false;
	}

	char	seps[] = " ";
	char*	strResult;

	::strtok( header, seps );				// http://HTTP/1.1
	strResult = ::strtok( NULL, seps );		// 200 (response 결과)

	int result = ::atoi( strResult );		// 결과 코드


	::FillMemory(&InetBuff, sizeof(InetBuff), 0);
	InetBuff.dwStructSize	= sizeof(InetBuff);
	InetBuff.lpcszHeader	= header;
	InetBuff.dwHeadersLength = sizeof(header);
	InetBuff.lpvBuffer		= lpBuffersOut;
	InetBuff.dwBufferLength	= dwBufferSize - 1;

	if( ! ::InternetReadFileEx( hRequest, &InetBuff, IRF_SYNC, NULL ) )
	{
		DWORD e = GetLastError();
		::InternetCloseHandle( hRequest );
		::InternetCloseHandle( hConnect );
		return false;
	}
	lpBuffersOut[InetBuff.dwBufferLength] = '\0';

	*dwReturnSize = InetBuff.dwBufferLength;

	::InternetCloseHandle( hRequest );
	::InternetCloseHandle( hConnect );
	
	return true;
}

BOOL CSyncInet::Request( LPCTSTR lpszQuery, DWORD &dwError )
{
	m_nCount++;
	DWORD nTick = ::GetTickCount();
	m_hRequest = ::InternetOpenUrl(
				m_hOpen,
				lpszQuery,
				NULL,
				0,
				INTERNET_FLAG_NO_CACHE_WRITE,
				NULL
				);
	nTick = ::GetTickCount() - nTick;
	if( nTick > 1000 )
	{
		char a[100];
		sprintf( a, "InternetOpenUrl Tick : %d %d", m_nCount, nTick );
		::OutputDebugString( a );
	}

	
	if( m_hRequest == NULL )
	{
		dwError = ::GetLastError();
		char a[100];
		sprintf( a, "InternetOpenUrl ERROR : %d\n", dwError );
		::OutputDebugString( a );
		return FALSE;
	}

	return TRUE;
}

BOOL CSyncInet::Response( ULONG_PTR lpInetKey, CHAR* lpBuffersOut, DWORD dwBufferSize, DWORD &dwNumberOfBytesRead )
{
	if( m_hRequest == NULL )
		return FALSE;

	dwNumberOfBytesRead = 0;
	if( !::InternetReadFile( m_hRequest, lpBuffersOut, dwBufferSize - 1, &dwNumberOfBytesRead ) )
	{
		::InternetCloseHandle( m_hRequest );
		return FALSE;
	}

	if( dwNumberOfBytesRead == 0 )
	{
		::InternetCloseHandle( m_hRequest );
		return TRUE;
	}

	lpBuffersOut[dwNumberOfBytesRead] = NULL;
	return TRUE;
}

INT CSyncInet::HttpResult( ULONG_PTR lpInetKey )
{
	char	header[1024];
	DWORD	nLen = 1024;
	if( !::HttpQueryInfo( m_hRequest, HTTP_QUERY_RAW_HEADERS_CRLF, header, &nLen, NULL ) )
	{
		return -1;
	}

	char	seps[] = " ";
	char*	strResult;

	::strtok( header, seps );				// http://HTTP/1.1
	strResult = ::strtok( NULL, seps );		// 200 (response 결과)

	return( ::atoi( strResult ) );
}