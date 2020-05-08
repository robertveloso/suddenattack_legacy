#include "StdAfx.h"
#include ".\asyncinet.h"

using namespace Query::HTTP;
using namespace Query;

CAsyncInet::CAsyncInet(void)
:	m_hOpen				( NULL	)
{
}

CAsyncInet::~CAsyncInet(void)
{
}

BOOL CAsyncInet::Create( int nInetCount, IQueryHandler* pHandler )
{
	INTERNET_STATUS_CALLBACK iscCallback;

	m_pHandler = pHandler;

	if( m_RequestContextPool.Create( nInetCount ) )
	{
		RequestContext** paContext = new RequestContext*[ nInetCount ];
		for( int i = 0; i < nInetCount; i++ )
		{
			paContext[i] = m_RequestContextPool.Alloc();

			paContext[i]->hCreatedEvent = ::CreateEvent( NULL, FALSE, FALSE, NULL );
		}
		for( int i = 0; i < nInetCount; i++ )
		{
			m_RequestContextPool.Free( paContext[i] );
		}
		delete[] paContext;
	}
	else
	{
		return FALSE;
	}

	m_hOpen = ::InternetOpen( NULL, INTERNET_OPEN_TYPE_DIRECT , NULL, NULL, INTERNET_FLAG_ASYNC );

	if( !m_hOpen )
		return FALSE;

	iscCallback = ::InternetSetStatusCallback( m_hOpen, (INTERNET_STATUS_CALLBACK)InetCallBack );

	if( iscCallback == INTERNET_INVALID_STATUS_CALLBACK )
	{
		::InternetCloseHandle( m_hOpen );
		return FALSE;
	}

	return TRUE;
}

void CAsyncInet::Destroy( void )
{
	if( m_hOpen )
		::InternetCloseHandle( m_hOpen );
	m_hOpen = NULL;

	m_RequestContextPool.Destroy();
}

BOOL CAsyncInet::Connect( ULONG_PTR& lpOutInetKey, InternetType type, LPCTSTR lpszServerName, ULONG_PTR lpContext, LPCTSTR lpszUserName, LPCTSTR lpszPassword, DWORD dwFlags )
{
	lpOutInetKey = (ULONG_PTR)m_RequestContextPool.Alloc();
	RequestContext*	pContext = (RequestContext*) lpOutInetKey;
	pContext->bUsed	= FALSE;
	pContext->pThis	= this;
	if( lpOutInetKey == NULL )
		return FALSE;

	pContext->lpContext	= lpContext;

	switch( type )
	{
	case InternetType::HTTP :
		pContext->hConnect =
			::InternetConnect( m_hOpen, lpszServerName, INTERNET_DEFAULT_HTTP_PORT, lpszUserName, lpszPassword, INTERNET_SERVICE_HTTP, dwFlags, (DWORD_PTR)pContext );
		if( !pContext->hConnect )
		{
			m_RequestContextPool.Free( pContext );
			return FALSE;
		}
		break;

	default:
		m_RequestContextPool.Free( pContext );
		return FALSE;
		break;
	}

	::WaitForSingleObject( pContext->hCreatedEvent, INFINITE );
	return TRUE;
}

void CAsyncInet::HttpRequestPost(
		ULONG_PTR	lpOutInetKey,
		LPCTSTR lpszObjectName,
		LPCTSTR lpszBody )
{
	RequestContext* pContext	= (RequestContext*)lpOutInetKey;
	pContext->bEndRequest		= FALSE;
	pContext->hRequest	= ::HttpOpenRequest(
		pContext->hConnect, 
		"POST", 
		lpszObjectName,
		NULL,
		NULL,
		NULL,
		INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE,
		(DWORD_PTR)pContext );

	if( !pContext->hRequest )
	{
		m_pHandler->OnErrorInet( pContext->lpContext, ::GetLastError() );
		::InternetCloseHandle( pContext->hConnect );
		m_RequestContextPool.Free( pContext );
		return;
	}

	::WaitForSingleObject( pContext->hCreatedEvent, INFINITE );


    INTERNET_BUFFERS IntBuff;
	::FillMemory( &IntBuff, sizeof( IntBuff ), 0 );
    IntBuff.dwStructSize	= sizeof( IntBuff );
	IntBuff.dwBufferTotal	= (DWORD)::strlen( lpszBody );
    IntBuff.lpcszHeader		= "Content-Type: application/x-www-form-urlencoded";
	IntBuff.dwHeadersLength	= ::lstrlen( IntBuff.lpcszHeader );

	pContext->nextStep = NextStep::InternetWriteFile;
	pContext->lpszBody = lpszBody;
	if( ! ::HttpSendRequestEx( pContext->hRequest, &IntBuff, NULL, 0, (DWORD_PTR)pContext ) )
    {
		DWORD dwError = ::GetLastError();
		if( dwError != ERROR_IO_PENDING )
        {
			m_pHandler->OnErrorInet( pContext->lpContext, dwError );
			::InternetCloseHandle( pContext->hConnect );
			::InternetCloseHandle( pContext->hRequest );
			m_RequestContextPool.Free( pContext );
        }
		return;
    }

	AsyncInternetWriteFile( pContext );
}

inline void CAsyncInet::AsyncInternetWriteFile( RequestContext* pContext )
{
	DWORD dwBytesWritten;
	pContext->nextStep = NextStep::HttpEndRequest;
	if( ! ::InternetWriteFile( pContext->hRequest, pContext->lpszBody, (DWORD)::strlen( pContext->lpszBody ), &dwBytesWritten ) )
	{
		DWORD dwError = ::GetLastError();
		if( dwError != ERROR_IO_PENDING )
        {
			m_pHandler->OnErrorInet( pContext->lpContext, dwError );
			::InternetCloseHandle( pContext->hConnect );
			::InternetCloseHandle( pContext->hRequest );
			m_RequestContextPool.Free( pContext );
        }
		return;
	}
	
	AsyncHttpEndRequest( pContext );
}

inline void CAsyncInet::AsyncHttpEndRequest( RequestContext* pContext )
{
	pContext->bEndRequest	= TRUE;
	if( ! ::HttpEndRequest( pContext->hRequest, NULL, HSR_INITIATE, (DWORD_PTR)pContext ) )
	{
		DWORD dwError = ::GetLastError();
		if( dwError != ERROR_IO_PENDING )
        {
			m_pHandler->OnErrorInet( pContext->lpContext, dwError );
			::InternetCloseHandle( pContext->hConnect );
			::InternetCloseHandle( pContext->hRequest );
			m_RequestContextPool.Free( pContext );
        }
	}
}

void CAsyncInet::HttpRequestGet( ULONG_PTR lpContext, LPCTSTR lpszURL )
{
	RequestContext*	pContext = m_RequestContextPool.Alloc();
	pContext->bUsed	= FALSE;
	pContext->pThis	= this;

	pContext->lpContext	= lpContext;
	pContext->hConnect	= NULL;


	pContext->bEndRequest	= TRUE;
	pContext->hRequest	= ::InternetOpenUrl(
		m_hOpen, 
		lpszURL,
		NULL,
		0,
		0,
		reinterpret_cast<DWORD_PTR>( pContext ) );

	if( !pContext->hRequest )
	{
		if( ::GetLastError() != ERROR_IO_PENDING )
		{
			m_pHandler->OnErrorInet( pContext->lpContext, ::GetLastError() );
			m_RequestContextPool.Free( pContext );
		}
		return;
	}
}

int CAsyncInet::HttpResult( ULONG_PTR lpInetKey )
{
	RequestContext* pContext = (RequestContext*)lpInetKey;

	char	header[1024];
	DWORD	nLen = 1024;
	if( !::HttpQueryInfo( pContext->hRequest, HTTP_QUERY_RAW_HEADERS_CRLF, header, &nLen, NULL ) )
	{
		return -1;
	}

	char	seps[] = " ";
	char*	strResult;

	::strtok( header, seps );				// http://HTTP/1.1
	strResult = ::strtok( NULL, seps );		// 200 (response 결과)

	return( ::atoi( strResult ) );
}

DWORD CAsyncInet::HttpResponse( ULONG_PTR lpInetKey, char* lpBuffersOut, DWORD dwBufferSize )
{
	RequestContext* pContext = (RequestContext*)lpInetKey;

	// Post
	if( pContext->hConnect )
	{
		INTERNET_BUFFERS InetBuff;
		::FillMemory(&InetBuff, sizeof(InetBuff), 0);
		InetBuff.dwStructSize	= sizeof(InetBuff);
		InetBuff.lpvBuffer		= lpBuffersOut;
		InetBuff.dwBufferLength	= dwBufferSize - 1;

		if( ! ::InternetReadFileEx( pContext->hRequest, &InetBuff, 0, NULL ) )
		{
			if( ::GetLastError() != ERROR_IO_PENDING )
				return -1;
		}

		lpBuffersOut[InetBuff.dwBufferLength] = NULL;

		return InetBuff.dwBufferLength;
	}

	// Get
	else
	{
		DWORD dwNumberOfBytesRead = 0;
		if( !::InternetReadFile( pContext->hRequest, lpBuffersOut, dwBufferSize - 1, &dwNumberOfBytesRead ) )
		{
			int nError = ::GetLastError();
			if( nError != ERROR_IO_PENDING )
				return -1;
			return -1;
		}
		else
		{
			int a = 3;
		}
		lpBuffersOut[dwNumberOfBytesRead] = NULL;
		return dwNumberOfBytesRead;
	}
}

void CAsyncInet::Close( ULONG_PTR lpInetKey )
{
	RequestContext* pContext = (RequestContext*)lpInetKey;

	::InternetCloseHandle( pContext->hRequest );
	::InternetCloseHandle( pContext->hConnect );
	pContext->hRequest = NULL;
	pContext->hConnect = NULL;

	m_RequestContextPool.Free( pContext );
}
void __stdcall CAsyncInet::InetCallBack(
	HINTERNET hInternet, DWORD dwContext, DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength)
{
	RequestContext*		pContext	= reinterpret_cast<RequestContext*>( dwContext );

	CAsyncInet*			pThis		= pContext->pThis;
	IQueryHandler*		pHandler	= pContext->pThis->m_pHandler;

	switch (dwInternetStatus)
	{
		case INTERNET_STATUS_CLOSING_CONNECTION:
			break;

		case INTERNET_STATUS_CONNECTED_TO_SERVER:
			break;

		case INTERNET_STATUS_CONNECTING_TO_SERVER:
			break;

		case INTERNET_STATUS_CONNECTION_CLOSED:
			break;

		case INTERNET_STATUS_HANDLE_CLOSING:
			break;

		case INTERNET_STATUS_HANDLE_CREATED:
			::SetEvent( pContext->hCreatedEvent );
			break;

		case INTERNET_STATUS_INTERMEDIATE_RESPONSE:
			break;

		case INTERNET_STATUS_NAME_RESOLVED:
			break;

		case INTERNET_STATUS_RECEIVING_RESPONSE:
			break;

		case INTERNET_STATUS_RESPONSE_RECEIVED:
			break;

		case INTERNET_STATUS_REDIRECT:
			break;

		case INTERNET_STATUS_REQUEST_COMPLETE:
			DWORD dwError;
			dwError = ::LPINTERNET_ASYNC_RESULT(lpvStatusInformation)->dwError;

			// Success
			if( dwError == 0 )
			{
				// Get 방식
				if( pContext->hRequest == NULL )
				{
					pContext->hRequest = HINTERNET(
						LPINTERNET_ASYNC_RESULT(lpvStatusInformation)->dwResult);

					pHandler->OnRecvInet( (ULONG_PTR)pContext, pContext->lpContext );
					break;
				}

				// Post 방식
				if( pContext->bEndRequest )
				{
					pHandler->OnRecvInet( (ULONG_PTR)pContext, pContext->lpContext );
				}
				else
				{
					switch( pContext->nextStep )
					{
					case NextStep::InternetWriteFile :
						pContext->pThis->AsyncInternetWriteFile( pContext );
						break;

					case NextStep::HttpEndRequest :
						pContext->pThis->AsyncHttpEndRequest( pContext );
						break;
					}
				}
			}
			else
			{
				pHandler->OnErrorInet( pContext->lpContext, dwError );
			}

			break;

		case INTERNET_STATUS_REQUEST_SENT:
			break;

		case INTERNET_STATUS_RESOLVING_NAME:
			break;

		case INTERNET_STATUS_SENDING_REQUEST:
			break;

		case INTERNET_STATUS_STATE_CHANGE:
			break;

		default:
			break;
	}
}