//-----------------------------------------------------------------------------
//
//	Network Firewall Manager DLL
//
//	!!!!! 한 프로그램 내에서 실행과 종료 시점에 방화벽에 조작을 가한다. !!!!!
//
//											Written by minjin. 2005. 02. 07.
//
//	와우.. 설 연휴 바로 전날이다.. -_-;;
//	DirectX 9.0 샘플을 이용해서, DLL 로 뺐다.
//
//	다음 순서로 실행하여야 함!!
//	
//	- 함수 설명
//
//	  _fnSTART		<-	NF_Create 의 함수 포인터.
//						방화벽 모듈을 생성한다.
//						프로세스명과, 목록에 표시될 이름을 같이 적어주며,
//						wchar 를 통해 입력받는다.
//						방화벽의 예외 목록에 실제로 항목을 추가한다.
//
//	  _fnISALLOWED	<-	NF_IsAllowed 의 함수 포인터.
//						방화벽에 정상적으로 등록되어 있는지 확인하여,
//						정상이면 TRUE, 아니면 FALSE -_-;;
//
//	  _fnEND		<-	NF_End 의 함수 포인터.
//						방화벽의 예외 목록에서 항목 제거.
//						방화벽 모듈 Release.
//
//-----------------------------------------------------------------------------

#ifndef __NFMNGR_DLL_H__
#define __NFMNGR_DLL_H__

#pragma once


#define NFMNGR_API __declspec(dllexport)


#define _FIREWALL_MANAGER_DLL_				TEXT("NFMngr.dll")			// DLL 이름
#define _FIREWALL_EXCEPTION_LIST_NAME_		L"Sudden Attack"			// 방화벽 목록에 추가될 이름



// 함수 포인터 타입 정의부분
// * 알아보기 쉽게 동일한 형식도 다른 이름으로 typedef 한다
typedef BOOL ( *_fnSTART		)( IN const wchar_t* szProcPath, IN const wchar_t* szProcName );
typedef BOOL ( *_fnEND			)( IN const wchar_t* szProcPath );
typedef BOOL ( *_fnISALLOWED	)( IN const wchar_t* szProcPath );


_fnSTART		__NF_Start		= NULL;
_fnISALLOWED	__NF_IsAllowed	= NULL;
_fnEND			__NF_End		= NULL;


// DLL 에서 Export 된 함수
extern "C" NFMNGR_API BOOL _NF_Start( IN const wchar_t* szProcPath, IN const wchar_t* szProcName );
extern "C" NFMNGR_API BOOL _NF_IsAllowed( IN const wchar_t* szProcPath );
extern "C" NFMNGR_API BOOL _NF_End( IN const wchar_t* szProcPath );

HMODULE g_hNF_Dll		= NULL;
BOOL	g_bNFActivated	= FALSE;


inline BOOL NF_Initialize()
{
	if ( g_hNF_Dll )			return TRUE;
	
	// DLL 로딩 및 함수 초기화 부분
	g_hNF_Dll	= LoadLibrary( _FIREWALL_MANAGER_DLL_ );
	
	if ( !g_hNF_Dll )			return FALSE;
	
	
	__NF_Start		= (_fnSTART)	GetProcAddress( g_hNF_Dll, "_NF_Start" );
	__NF_End		= (_fnEND)		GetProcAddress( g_hNF_Dll, "_NF_End" );
	__NF_IsAllowed	= (_fnISALLOWED)GetProcAddress( g_hNF_Dll, "_NF_IsAllowed" );
	
	if ( NULL == __NF_Start )		return FALSE;
	if ( NULL == __NF_End )			return FALSE;
	if ( NULL == __NF_IsAllowed )	return FALSE;
	
	g_bNFActivated = TRUE;

	return TRUE;
}

inline BOOL NF_Release()
{
	if ( g_hNF_Dll )
	{
		return FreeLibrary( g_hNF_Dll );
	}
	
	return FALSE;
}

inline void NF_Start( IN const wchar_t* szProcPath, IN const wchar_t* szProcName )
{
	if ( FALSE == g_bNFActivated || FALSE == g_hNF_Dll )
		return;

	__NF_Start( szProcPath, szProcName );
}

inline void NF_End( IN const wchar_t* szProcPath )
{
	if ( FALSE == g_bNFActivated || FALSE == g_hNF_Dll )
		return;

	__NF_End( szProcPath );
}

inline void NF_IsAllowed( IN const wchar_t* szProcPath )
{
	if ( FALSE == g_bNFActivated || FALSE == g_hNF_Dll )
		return;

	__NF_IsAllowed( szProcPath );
}


#endif //__NFMNGR_DLL_H__