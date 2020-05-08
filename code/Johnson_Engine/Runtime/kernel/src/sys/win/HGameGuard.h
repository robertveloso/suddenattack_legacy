#ifndef	_GAMEGUARD_H_
#define _GAMEGUARD_H_

#include <windows.h>

#pragma comment (lib, "version.lib")

////////////////////////////////////////////////////////////////////////////////
//	[Code Generation]
//	Mutlithread 옵션은 _MT 를 정의하고, DLL Dynamic linking 은 _DLL
//	을 정의. _MT 가 안 들어가면 에러를 내야하고, _DLL 여부에 따라
//	링크되는 라이브러리가 달라야 한다. (MSDN 참조)
////////////////////////////////////////////////////////////////////////////////
#if !defined(_MT)
#	error	Error : 'Code generation' must be Multithreaded something!
#endif
#if defined(_DEBUG)
#	if defined(_DLL)
#	pragma message ("HGameGuard : compiling as \"Multithreaded Dll Debug\".")
#	pragma comment (lib, "../../../HGameGuardDllD.lib")
#	else
#	pragma message ("HGameGuard : compiling as \"Multithreaded Debug\".")
#	pragma comment (lib, "../../../HGameGuardD.lib")
#	endif
#else
#	if defined(_DLL)
#	pragma message ("HGameGuard : compiling as \"Multithreaded Dll Release\".")
#	pragma comment (lib, "../../../HGameGuardDll.lib")
#	else
#	pragma message ("HGameGuard : compiling as \"Multithreaded Release\".")
#	pragma comment (lib, "../../../HGameGuard.lib")
#	endif
#endif


////////////////////////////////////////////////////////////////////////////////
//	[환경변수]
////////////////////////////////////////////////////////////////////////////////
#define	X2PGL_VERSION_STRING	_T("2005.04.27.01")		//버전


// callback function definition
typedef	BOOL (*GAMEGUARD_CALLBACKFUNC)(DWORD callback_code, DWORD callback_subcode, LPVOID callback_userdata) ;

// protect flag value
enum EProtectType { 
	eProtect_Nothing		= 0,	
	eProtect_Global			= 0x1,
	eProtect_SpeedHack		= 0x2,
	eProtect_MemoryHack		= 0x4,
	eProtect_Macro			= 0x8,
	eProtect_Network		= 0x10,
	eProtect_Message		= 0x20,
	eProtect_Self			= 0x40,
	eProtect_All			= 0xFFFFFFFF,
};

class CFireGuard  
{
public:
	CFireGuard () ;
	~CFireGuard () ;

	LONG	Initialize (HWND hGameWnd, 
						LPCTSTR lpszGameName, 
						GAMEGUARD_CALLBACKFUNC pfnCallback, 
						DWORD dwProtectFlags) ;
	BOOL	IsAlive () ;
	LONG	Start () ;
	void	Stop () ;

	void	LogClientInfo(LPCTSTR log_text);

};

#endif // _GAMEGUARD_H_