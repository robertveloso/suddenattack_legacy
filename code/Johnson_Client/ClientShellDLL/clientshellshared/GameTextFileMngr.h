// GameTextFileMngr.h: interface for the CGameTextFileMngr class.
/*
 *	게임 텍스트에 쓰는 파일 매니져다..		Written by Novice.
 *
 *	Win32API 의 파일 관련 함수들을 Wrapping 해 놓았다.
 *	아시다시피.. 꽤나 허접하다.. -_-;;
 *
 *	차후에, Load(), Save() 만 놔두고 다 없애버릴 예정이다.. -_-;;
 */
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GAMETEXTFILEMNGR_H__E7C2248D_E60D_4A71_AA97_E7046E667FF8__INCLUDED_)
#define AFX_GAMETEXTFILEMNGR_H__E7C2248D_E60D_4A71_AA97_E7046E667FF8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//////////////////////////////////////////////////////////////////////////

#define NF_FILE_MNGR()				CGameTextFileMngr::GetInstance()

#define NF_FILE_SIGNATURE			TEXT( "GameHi Sudden Attack Message File Version 0.1.0 @@ Written by Novice. \x81\x12\x21\x01\x02\x03%d\x08" )
#define NF_FILE_SIGNATURE_SIZE		128
#define NF_FILE_BUFFER_SIZE			4096

typedef struct _Buffer				// File 용 Buffer 정보 타입
{
	LPVOID	pBuf;
	DWORD	dwBufSize;
} NF_Buff, *LPNF_Buff;

//////////////////////////////////////////////////////////////////////////

class CGameTextFileMngr  
{
public:
	static CGameTextFileMngr* GetInstance();

	// !!! WARNNING !!!
	// bBinary 플래그를 TRUE 로 설정하면, 파일 앞에 시그니쳐 붙는다.
	// 이거 나중에 빼버릴것임.. -_-;;
	//
	// Create 부터 Destroy 까지 순서대로 쓴다. 글케 안쓰면 에러나게 해주지.. ㅋㅋ
	BOOL	Create( LPCTSTR lpFileName, BOOL bSave = FALSE );

	DWORD	Load( BOOL bBinary );				// 파일 읽어서 버퍼로!
	void	GetBuffer( OUT LPNF_Buff buf );		// 읽힌 내용이 들어있는 버퍼 반환

	BOOL	SetBuffer( LPNF_Buff buf,			// 파일에 쓸 버퍼 설정
					   BOOL bSave = FALSE,		// 여기서 bSave 가 TRUE 이면, 밑에있는 Save() 를 실행한다.
					   BOOL bBinary = TRUE );	// 한마디로, SetBuffer( ..., TRUE ); 하면, Save() 안해도 된다.

	BOOL	Save( BOOL bBinary );				// 버퍼를 파일로!

	BOOL	Destroy();							// 만든거 끝날때 반드시 호출해라.. -_-;;


protected:

	CGameTextFileMngr();
	virtual ~CGameTextFileMngr();
	
	static CGameTextFileMngr* m_pInstance;
	
	DWORD	Read( BOOL bBinary );
	BOOL	ReadSignature( LPDWORD dwVer, LPDWORD dwFileSize );

	BOOL	Write( BOOL bBinary );
	BOOL	WriteSignature( DWORD dwVer, DWORD dwFileSize );

	enum EN_FILE_MNGR_PROPER_STEP		// 진행상황 관리용 enum
	{
		FMPS_NULL		= 0x00,
		FMPS_CREATED	= 0x10,
		FMPS_LOADED		= 0x20,
		FMPS_READ		= 0x30,
		FMPS_GOTBUF		= 0x40,
		FMPS_SETBUF		= 0x50,
		FMPS_WRITTEN	= 0x60,
		FMPS_SAVED		= 0x70,
		FMPS_DESTROYED	= 0x80,
		FMPS_INVALID	= 0xff,
	};
	
	EN_FILE_MNGR_PROPER_STEP m_enStep;	// 현재 진행상황 가지고 있는놈

	HANDLE	m_hFile;					// 파일 핸들
	TCHAR	m_strFileName[_MAX_PATH];	// 파일 이름
	BOOL	m_bSave;					// 파일을 쓰기용으로 만들건지, 읽기용으로 만들건지.. ( TRUE -> 쓰기용, FALSE -> 읽기용 )
	DWORD	m_dwFileSize;				// 파일 전체 크기
	
	TCHAR*	m_pBuffer;					// 파일내용 그대로 들고있을 놈
	DWORD	m_dwBufSize;				// 쓰기용 파일 버퍼 크기

	DWORD	m_dwReadSize;				// 지금까지 읽어들인 크기
	DWORD	m_dwWriteSize;				// 지금까지 쓴 크기

	DWORD	m_dwVersionInfo;			// 파일 버전 정보
};

#endif // !defined(AFX_GAMETEXTFILEMNGR_H__E7C2248D_E60D_4A71_AA97_E7046E667FF8__INCLUDED_)
