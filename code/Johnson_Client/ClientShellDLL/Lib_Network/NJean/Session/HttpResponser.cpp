#include "StdAfx.h"
#include "./HttpResponser.h"

using namespace GamehiSolution::Session::Gate;
//using namespace GamehiSolution::Common::Error;

/**************************************************************************************
	持失切 / 社瑚切
***************************************************************************************/

CHttpResponser::CHttpResponser()
:	m_aBuffer	( NULL	)
,	m_nMaxSize	( 0		)
,	m_nResult	( 0		)
{
}

CHttpResponser::~CHttpResponser()
{
}

/**************************************************************************************
	Public Operations
***************************************************************************************/

BOOL CHttpResponser::Create( int nMaxSize )
{
	m_aBuffer = new char[nMaxSize];
	if( m_aBuffer == NULL )
		return FALSE;

	m_nMaxSize = nMaxSize;
	return TRUE;
}

void CHttpResponser::Destroy()
{
	if( m_aBuffer )
		delete[] m_aBuffer;
	m_aBuffer = NULL;

	m_nMaxSize = 0;
}

void CHttpResponser::AddData( const char* pData, int nSize )
{
	if( m_nMaxSize <= nSize + m_nCurrSize )
	{
//		m_dwError = CHttpError::NOT_ENOUGH_MEMORY;
		return;
	}

	::memcpy( &m_aBuffer[m_nCurrSize], pData, nSize );
	m_nCurrSize += nSize;
	m_aBuffer[m_nCurrSize] = NULL;
}
