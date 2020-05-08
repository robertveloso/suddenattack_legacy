// GameText.h: interface for the CGameText class.
/*
	GameText Class						Written by Novice.

	대한민국, 중국, 일본 세개 국가의 메시지 지원을 위한 클래스.

	항상 한개의 뜻을 가지는 메세지는,
	세개 국가의 버젼으로 한번에 Insert / Remove / Update 된다.
	리소스의 분할을 최대한 막아 실수가 없도록 하기 위함이다.
*/
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GAMETEXT_H__10FB6C7E_D944_461F_A5EE_F17BA685F6B4__INCLUDED_)
#define AFX_GAMETEXT_H__10FB6C7E_D944_461F_A5EE_F17BA685F6B4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "tchar.h"


//#define KOR_LOCALIZING			// 한글판
//#define JPN_LOCALIZING			// 일본어판
#define CHN_LOCALIZING			// 중국어판

#if		defined KOR_LOCALIZING
#define NLC_NATION			NLC_KOREA
#define LOCALIZING_CHARSET	HANGUL_CHARSET
#define DEFAULT_FONT_NAME	"\xb1\xbc\xb8\xb2"										//KOREAN font
#elif	defined JPN_LOCALIZING
#define NLC_NATION			NLC_JAPAN
#define LOCALIZING_CHARSET	SHIFTJIS_CHARSET
#define DEFAULT_FONT_NAME	"\x82\x6c\x82\x72\x20\x83\x53\x83\x56\x83\x62\x83\x4e"	//JAPANESE font
#elif	defined CHN_LOCALIZING
#define NLC_NATION			NLC_CHINA
#define LOCALIZING_CHARSET	GB2312_CHARSET
#define DEFAULT_FONT_NAME	"\xB0\x65\x30\x7D\x0E\x66\xD4\x9A\x00\x00"				//TRADITIONAL_CHINESE font
#else
#define LOCALIZING_CHARSET	DEFAULT_CHARSET
#endif


//////////////////////////////////////////////////////////////////////////
char*	NF_GetMsg( DWORD dwIdx );							// 게임에서 메세지 가져갈때 쓴다.

#define NF_GAME_TEXT()				CGameText::GetInstance()

#define MAX_MESSAGE_CNT				1024					// 게임 메세지 갯수 최대값
#define MAX_MESSAGE_LANG			3						// 국가 갯수
#define MAX_MESSAGE_LENGTH			512 * sizeof(TCHAR)		// 게임 메세지 길이 최대값
#define MAX_MESSAGE_DEF_LENGTH		256						// 게임 메세지 표시할 DEFINE 의 길이 최대값

#define MAX_CONTEXT_BODY_SIZE		MAX_MESSAGE_CNT * sizeof(GameText)
#define MAX_CONTEXT_HEADER_SIZE		MAX_MESSAGE_CNT * sizeof(GameTextHeader)
#define MAX_CONTEXT_SIZE			MAX_CONTEXT_BODY_SIZE + MAX_CONTEXT_HEADER_SIZE

enum EN_NATIONAL_LANGUAGE_CODE	// 국가 코드 Enum
{
	NLC_KOREA	= 0,
	NLC_CHINA	= 1,
	NLC_JAPAN	= 2,
};

#define GAME_MESSAGE_FILE_NAME		TEXT("SA_MSGS.gsm")

#define END_OF_GAMETEXT_HEADER		TEXT("\xa8\x97\x30\xd0\xa4\xe0\xe0\x02")
#define END_OF_GAMETEXT_CONTEXT		TEXT("\x97\xa4\xa8\xe0\x02\xe0\x30\xd0")


typedef struct _GameTextHeader
{
	DWORD dwIdx;
	DWORD dwDefineOffset;
	DWORD dwDefineLength;
	DWORD dwOffset[MAX_MESSAGE_LANG];
	DWORD dwLength[MAX_MESSAGE_LANG];
} GameTextHeader, *LPGameTextHeader;

typedef struct _GameText		// Game Text 구조체
{
	DWORD dwIdx;	// 배열 인덱스와 같은 값이지만, 하나 넣어놓는다.
					// 나중에 제대로 정렬된 메세지인지 확인할때 쓴다.

	TCHAR txtDefine[MAX_MESSAGE_DEF_LENGTH];
	TCHAR txtMessage[MAX_MESSAGE_LANG][MAX_MESSAGE_LENGTH];


	// 맨 처음 생성할때는 내용 날려버려야지!
	_GameText() {	ZeroMemory( this, sizeof(_GameText) );	}

} GameText, *LPGameText;
//////////////////////////////////////////////////////////////////////////


class CGameText  
{
public:
	static CGameText* GetInstance();
	BOOL	Destroy();

	BOOL	IsExist( LPCTSTR lpDefinition );
	
	BOOL	Insert( LPGameText pGameText );		// Message 의 Insert, Remove 는
	BOOL	Remove( DWORD dwIdx );				// 무조건 전 국가에 전체 적용하는것으로 한다.
	
	void	Update( LPGameText pGameText );
	void	Update( DWORD dwArrIdx, DWORD dwRealIdx );
	void	Update( DWORD dwIdx, LPCTSTR lpszDefine );
	void	Update( DWORD dwIdx, LPCTSTR lpszMsg, EN_NATIONAL_LANGUAGE_CODE nlCode );

	DWORD	GetTotMsgCnt();
	BOOL	GetRow( DWORD dwIdx, OUT LPGameText pGameText );

	LPCTSTR GetMsg( DWORD dwIdx );
	LPCTSTR	GetMsg( EN_NATIONAL_LANGUAGE_CODE nlCode, DWORD dwIdx );


	/////////////////////////// 파일 관련 함수 ///////////////////////////////
	// 파일로 내보낼 때 쓰는 함수
	LPVOID	GetContext();
	DWORD	GetContextSize() { return m_dwGameTextContextSize; }

	LPVOID	GetContextHeader();
	DWORD	GetContextHeaderSize() { return m_dwGameTextContextHeaderSize; }


	// 파일로부터 내용 받아올 때 쓰는 함수
	void	SetContext( LPVOID lpContext, DWORD dwContextSize );


	// 게임 내에서 쓰는 헤더 파일용..
	LPVOID	GetRefHeader();
	DWORD	GetRefHeaderSize() { return m_dwGameTextRefHeaderSize; }
	//////////////////////////////////////////////////////////////////////////
	

protected:
	CGameText();
	virtual ~CGameText();

	static CGameText* m_pInstance;

	void	ProcessingArrayToContext();				// 텍스트 배열에 있는 값들을 스트림으로 변환
	void	ProcessingContextToArray();				// 스트림 값들을 텍스트 배열화
	void	ProcessingArrayToGameHeader();			// 텍스트 배열 값들을 기초로, 헤더 출력.

	// in ProcessingArraryToContext()
	DWORD	MakeContextHeader();
	DWORD	MakeHeaderFromBody( DWORD dwCnt, DWORD dwContextOffset );
	DWORD	WriteHeaderIntoContext( DWORD dwContextOffset );
	DWORD	WriteBodyIntoContext( DWORD dwContextOffset );

	// in ProcessingContextToArray()
	void	AnalyzeContextHeader();
	DWORD	WriteContextIntoBody( DWORD dwCnt, DWORD dwContextOffset );


	TCHAR*	m_pGameTextRefHeader;				// 참고용 헤더(.h) 파일
	DWORD	m_dwGameTextRefHeaderSize;

	TCHAR*	m_pGameTextContext;
	TCHAR*	m_pGameTextContextHeader;
	DWORD	m_dwGameTextContextHeaderSize;
	DWORD	m_dwGameTextContextSize;

	TCHAR*	m_pTmpGameTextContext;				// 읽을때는 Temp 를 쓴다.
	DWORD	m_dwTmpGameTextContextSize;
	
	DWORD	m_dwTotMsgCnt;						// 전체 메세지 갯수

	GameText		m_arrGameText[MAX_MESSAGE_CNT];
	GameTextHeader	m_arrGameTextHeader[MAX_MESSAGE_CNT];
	
	// BOOL SearchMessage( LPCTSTR lpszMessage );

};

#endif // !defined(AFX_GAMETEXT_H__10FB6C7E_D944_461F_A5EE_F17BA685F6B4__INCLUDED_)
