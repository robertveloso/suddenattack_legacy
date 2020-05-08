#include "stdafx.h"
#include "FinalGameText.h"
#include "GameText.h"

CFinalGameText* CFinalGameText::m_pInstance = NULL;

CFinalGameText::CFinalGameText()
{
	m_pArrFinalGameTextHeader	= NULL;
	m_pWholeContext				= NULL;
	m_dwHeaderCount				= NULL;
}

CFinalGameText::~CFinalGameText()
{
	if ( NULL != m_pArrFinalGameTextHeader )
	{
		delete [] m_pArrFinalGameTextHeader;
		m_pArrFinalGameTextHeader	= NULL;
	}

	if( NULL != m_pWholeContext )
	{
		delete [] m_pWholeContext;
		m_pWholeContext	= NULL;
	}
}

CFinalGameText* CFinalGameText::GetInstance()
{
	if ( NULL == m_pInstance )
	{
		m_pInstance = new CFinalGameText();
	}

	return m_pInstance;
}

BOOL CFinalGameText::Destroy()
{
	if ( NULL != m_pInstance )
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}

	return TRUE;
}

LPCTSTR CFinalGameText::GetMsg( DWORD dwIdx )
{
	if ( dwIdx > m_dwHeaderCount ) return "";

	return (NULL != (m_pWholeContext + m_pArrFinalGameTextHeader[dwIdx].dwOffset)) ?
		(m_pWholeContext + m_pArrFinalGameTextHeader[dwIdx].dwOffset) : "";
}

BOOL CFinalGameText::SetHeaderWithParsing( LPVOID lpHeaderBuf )
{
	DWORD dwHeaderCount		= 0;
	DWORD dwHeaderBufOffset = 0;
	DWORD dwContextOffset	= 0;

	// 맨 처음 헤더 갯수 가져온다.
	CopyMemory( &dwHeaderCount, lpHeaderBuf, sizeof(DWORD) );
	dwHeaderBufOffset += sizeof(DWORD);

	m_dwHeaderCount = dwHeaderCount - 1;

	m_pArrFinalGameTextHeader = new FinalGameTextHeader[dwHeaderCount + 1];
	ZeroMemory( m_pArrFinalGameTextHeader, sizeof(FinalGameTextHeader) * ( dwHeaderCount + 1 ) );
	
	for ( DWORD dwCnt = 0; dwCnt < dwHeaderCount; dwCnt++ )
	{
		CopyMemory( &m_pArrFinalGameTextHeader[dwCnt].dwIdx, (char*)lpHeaderBuf + dwHeaderBufOffset, sizeof(DWORD) );
		dwHeaderBufOffset += sizeof(DWORD);						// 인덱스정보
		dwHeaderBufOffset += sizeof(DWORD) * 2;					// 디파인정보 뛰어넘고,
		dwHeaderBufOffset += sizeof(DWORD) * MAX_MESSAGE_LANG;	// 나라 메세지의 오프셋 뛰어넘고,
		dwHeaderBufOffset += sizeof(DWORD) * NLC_NATION;		// 해당국가 메세지 길이정보 가져오고,

		m_pArrFinalGameTextHeader[dwCnt+1].dwOffset =
			(DWORD)(*((DWORD*)((char*)lpHeaderBuf + dwHeaderBufOffset))) + dwContextOffset + 1;	// Null Character Included

		dwContextOffset	  = m_pArrFinalGameTextHeader[dwCnt+1].dwOffset;

		dwHeaderBufOffset += (sizeof(DWORD) * (MAX_MESSAGE_LANG - NLC_NATION));	// 나머지 나라는 뛰어넘고,
	}

	m_dwWholeContextSize = dwContextOffset;

	return TRUE;
}

BOOL CFinalGameText::SetContextWithParsing( LPVOID lpHeaderInfoBuf, LPVOID lpContextBuf )
{
	// 게임 텍스트 헤더정보 가져오기..
	DWORD dwHeaderCount		= 0;
	DWORD dwContextOffset	= 0;
	CopyMemory( &dwHeaderCount, lpHeaderInfoBuf, sizeof(DWORD) );

	// 맨 처음 갯수는 건너뛰고, 그 뒤부터 헤더정보다.
	TCHAR*				pGameTextContext	= (TCHAR*)lpContextBuf;
	LPGameTextHeader	pGameTextHeaderInfo = (LPGameTextHeader)((char*)lpHeaderInfoBuf + sizeof(DWORD));

	m_pWholeContext = new char[m_dwWholeContextSize];
	ZeroMemory( m_pWholeContext, m_dwWholeContextSize );
	
	LPCTSTR lpCurMsgPos		= NULL;
	DWORD	dwCurMsgLength	= 0;

	pGameTextContext += ( dwHeaderCount * sizeof(GameTextHeader) ) + 12;

	for ( DWORD dwCnt = 0; dwCnt < dwHeaderCount; dwCnt++ )
	{
		lpCurMsgPos		= pGameTextContext + pGameTextHeaderInfo[dwCnt].dwOffset[NLC_NATION];
		dwCurMsgLength	= lstrlen( lpCurMsgPos );

		CopyMemory( &m_pWholeContext[dwContextOffset], lpCurMsgPos, dwCurMsgLength );
		dwContextOffset += dwCurMsgLength + 1;		// Null Character Included
	}

	return TRUE;
}

