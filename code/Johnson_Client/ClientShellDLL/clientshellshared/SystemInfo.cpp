#include "StdAfx.h"
#include "SystemInfo.h"
#include <Mmsystem.h>

#define SAFE_CLOSE_KEY(p) if(p) { RegCloseKey(p); (p)=LTNULL; }


//------------------------------------------------------------------------------
// NAME: CSystemInfo()
// DESC: 
//------------------------------------------------------------------------------
CSystemInfo::CSystemInfo()
{
}



//------------------------------------------------------------------------------
// NAME: ~CSystemInfo()
// DESC: 
//------------------------------------------------------------------------------
CSystemInfo::~CSystemInfo()
{
}



//------------------------------------------------------------------------------
// NAME: GetMemoryInfo()
// DESC: 
//------------------------------------------------------------------------------
BOOL CSystemInfo::GetMemoryInfo(MEMORYSTATUS* pMemstatus)
{
	pMemstatus->dwLength = sizeof(MEMORYSTATUS);
	
	GlobalMemoryStatus(pMemstatus);
	return TRUE;
}



//------------------------------------------------------------------------------
// NAME: GetOS()
// DESC: 
//------------------------------------------------------------------------------
BOOL CSystemInfo::GetOS(LPSTR lpOS)
{
    if( !lpOS ) return FALSE;
    OSVERSIONINFO   osvi;
	
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&osvi);

    switch( osvi.dwPlatformId )
    {
    case VER_PLATFORM_WIN32s:                   //  window 3.1
		{
            wsprintf( lpOS, "Windows 3.1" );       
		}
        break;

    case VER_PLATFORM_WIN32_WINDOWS:            //  win95
		{
			if( osvi.dwMinorVersion  == 0 )
			{
                wsprintf( lpOS, "Windows 95" );
			}
			else if( osvi.dwMinorVersion > 0 && osvi.dwMinorVersion < 90)      //  win98
			{
				if( strcmp(osvi.szCSDVersion , " A ") == 0 )            
                    wsprintf( lpOS, "Windows 98 SE" );
				else
                    wsprintf( lpOS, "Windows 98" ); 
			}
			else
                wsprintf( lpOS, "Windows ME" );
		}        
        break;

    case VER_PLATFORM_WIN32_NT:         // Window NT and 2000
		{
            if( osvi.dwMajorVersion < 5 )
                wsprintf( lpOS, "Windows NT %d.%d %s", osvi.dwMajorVersion ,
						osvi.dwMinorVersion, osvi.szCSDVersion);

			else if ( osvi.dwMinorVersion == 0 )
                wsprintf( lpOS, "Windows 2000 %s", osvi.szCSDVersion);
			else
				wsprintf( lpOS, "Windows XP %s", osvi.szCSDVersion);
       
		}
        
        break;
    }   

    if( strlen( lpOS ) > 32 ) return FALSE;
    return TRUE;

}



//------------------------------------------------------------------------------
// NAME: DetectNumProcessors()
// DESC: 
//------------------------------------------------------------------------------
int CSystemInfo::DetectNumProcessors()
{
	SYSTEM_INFO sinfo;
	GetSystemInfo(&sinfo);
	return sinfo.dwNumberOfProcessors;
}



//------------------------------------------------------------------------------
// NAME: GetVgaInfo()
// DESC: 
//------------------------------------------------------------------------------
void CSystemInfo::GetVgaInfo(char *szInfo,LPSTR lpNTService)
{
	NAMEGROUP namegroup;
	namegroup.nItemCnt = 0;

	__try
	{
		if ( GetVersion() < 0x80000000)
		{		
			// WinNT Series //
			GetVgaInfo_WinNT( &namegroup,"SYSTEM\\CURRENTCONTROLSET\\ENUM\\PCI",lpNTService );
			GetVgaInfo_WinNT( &namegroup,"SYSTEM\\CURRENTCONTROLSET\\ENUM\\ISAPNP",lpNTService );
			// WinNT Series //
		}
		else
		{
			// Win9X Series //
			GetVgaInfo_Win9x( &namegroup, "ENUM\\PCI",lpNTService);
			GetVgaInfo_Win9x( &namegroup, "ENUM\\ISAPNP",lpNTService);
			// Win9X Series //
		}

		/* it use for several device */		
		int nCnt = 0;
		char cToken;
		cToken = ',';

		if( !(namegroup.nItemCnt) )
		{
			strcpy( szInfo, "none" );
		}
		else
		{
			strcpy( szInfo, namegroup.szName[0] );
		}
		
		nCnt = 1;
		for( int i = 2 ; i < namegroup.nItemCnt; i++ ){		
			wsprintf( szInfo, "%s%c%s", szInfo, cToken, namegroup.szName[i] );
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		if( szInfo )
		{
			strcpy( szInfo, "ERR" );
		}
	}
}



//------------------------------------------------------------------------------
// NAME: GetVgaInfo_Win9x()
// DESC: 
//------------------------------------------------------------------------------
void CSystemInfo::GetVgaInfo_Win9x( NAMEGROUP *namegroup,LPSTR lpWin9xSubKey,LPSTR	lpWin9xService )
{
	HKEY		hMainKey = LTNULL;
	HKEY		hSubKey_1 = LTNULL;
	HKEY		hSubKey_2 = LTNULL;
	long		lResult, lResult1;
	DWORD		dwKeyIndex, dwSubKeyIndex ;
	DWORD		dwSize, dwType;
	LPSTR		lpWin9xKeyword	= "CLASS";
	//LPSTR		lpWin9xService	= "DISPLAY";	
	//LPSTR		lpWin9xSubKey	= "ENUM\\PCI";
	
	// SPIKE
	LPSTR		lpWin9xDesc		= "DEVICEDESC";
	LPSTR		lpWin9xMfg		= "MFG";
	char		szRegPath[1024], szSubRegPath[1024], szEnumName[1024], szClass[1024], szDesc[1024], szMfg[1024];

	memset( szRegPath, 0, sizeof(szRegPath) );
	memset( szSubRegPath, 0, sizeof(szRegPath) );
	memset( szEnumName, 0, sizeof(szRegPath) );
	memset( szClass, 0, sizeof(szRegPath) );
	memset( szDesc, 0, sizeof(szRegPath) );
	memset( szMfg, 0, sizeof(szRegPath) );

	// GROBAL INIT
	//namegroup->nItemCnt = 0;
	// clear namegroup
	
	// ONLY ONE LOOP FOR ANYTIME BREAK :P //
	do{
		// INIT VARIABLE
		dwKeyIndex = 0;
		SAFE_CLOSE_KEY( hMainKey );
		lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpWin9xSubKey, 0 , KEY_ENUMERATE_SUB_KEYS|KEY_EXECUTE|KEY_QUERY_VALUE, &hMainKey);
		if (lResult != ERROR_SUCCESS)break;

		// REAL LOOP FOR FINDING DISPLAY NAME
		while( 1 )
		{
			// INIT PATH BUFF			
			if( lpWin9xSubKey == "ENUM\\PCI")
				strcpy( szRegPath, "PCI" );
			else
				strcpy( szRegPath, "ISAPNP" );

			lResult = RegEnumKey( hMainKey , dwKeyIndex, szEnumName, MAX_PATH + 1);						
			SAFE_CLOSE_KEY( hSubKey_1 );
			lResult1 = RegOpenKeyEx(hMainKey, szEnumName, 0, KEY_READ, &hSubKey_1);
			if (lResult == ERROR_NO_MORE_ITEMS || lResult1 != ERROR_SUCCESS )break;						

			// ADD PATH
			wsprintf( szRegPath, "%s\\%s", szRegPath, szEnumName );

			memset( szSubRegPath, 0, sizeof(szSubRegPath) );
			dwSubKeyIndex = 0;
			while(1)
			{
				lResult = RegEnumKey( hSubKey_1, dwSubKeyIndex, szEnumName, MAX_PATH + 1);						
				SAFE_CLOSE_KEY( hSubKey_2 );
				lResult1 = RegOpenKeyEx(hSubKey_1, szEnumName, 0, KEY_READ, &hSubKey_2);
				if (lResult == ERROR_NO_MORE_ITEMS || lResult1 != ERROR_SUCCESS )break;						

				// ADD PATH
				wsprintf( szSubRegPath, "%s\\%s", szRegPath, szEnumName );

				// CHECK WHETHER CLASS IS !!DISPLAY!!
				dwType = REG_SZ;dwSize = sizeof(szClass);memset( szClass, 0, sizeof(szClass) );
				lResult = RegQueryValueEx(hSubKey_2, lpWin9xKeyword, NULL, &dwType, (LPBYTE)szClass, &dwSize);	
				if( lResult != ERROR_SUCCESS ){
					dwSubKeyIndex++;
					continue;
				}

				if( _strnicmp( szClass, lpWin9xService, strlen(szClass) ) )break;				

				if( !IsDynamicDeviceOnWin9x( szSubRegPath ) ){
					dwSubKeyIndex++;
					continue;
				}

				//GET DISPLAY NAME			
				dwType = REG_SZ;dwSize = sizeof(szDesc);memset( szDesc, 0, sizeof(szDesc) );
				lResult = RegQueryValueEx(hSubKey_2, lpWin9xMfg, NULL, &dwType, (LPBYTE)szMfg, &dwSize);	
				if( lResult != ERROR_SUCCESS )break;
				strcat(szMfg, "   ");  // 공백 추가 
				strcpy( namegroup->szName[namegroup->nItemCnt], szMfg );				
				
				lResult = RegQueryValueEx(hSubKey_2, lpWin9xDesc, NULL, &dwType, (LPBYTE)szDesc, &dwSize);	
				if( lResult != ERROR_SUCCESS )break;
				strcat(namegroup->szName[namegroup->nItemCnt], szDesc);
					////////////////////////////////////////////////////////////////////////
					//Ver...
					char Ver[64] = {0,};
					dwSize = 64;
					lResult = RegQueryValueEx(hSubKey_2, "Driver", NULL, &dwType, (LPBYTE)Ver, &dwSize);	
					if( lResult == ERROR_SUCCESS )
					{
						TCHAR strTemp[512];
						wsprintf( strTemp, "System\\CurrentControlSet\\Services\\Class\\%s", Ver );
						
						char Ver1[32] = {0,};
						GetRegValue(HKEY_LOCAL_MACHINE, strTemp, "Ver", dwType, Ver1, 32);

						strcat(namegroup->szName[namegroup->nItemCnt], "  DriverVer:");
						strcat(namegroup->szName[namegroup->nItemCnt], Ver1);
						//AfxMessageBox(Ver1);
					}
					//Ver...
					////////////////////////////////////////////////////////////////////////
//HKEY_LOCAL_MACHINE\System\CurrentControlSet\Services\Class\Display\0000    Ver
//Driver : DISPLAY\0000

				namegroup->nItemCnt++;				
				dwSubKeyIndex++;
			}

			dwKeyIndex++;
		}

	}while(0);
	
	// TERMINATE REGISTRY KEY
	SAFE_CLOSE_KEY( hMainKey );
	SAFE_CLOSE_KEY( hSubKey_1 );
	SAFE_CLOSE_KEY( hSubKey_2 );
}



//------------------------------------------------------------------------------
// NAME: IsDynamicDeviceOnWin9x()
// DESC: 
//------------------------------------------------------------------------------
BOOL CSystemInfo::IsDynamicDeviceOnWin9x(char *szRegPath)
{	
	DWORD		dwKeyIndex;
	DWORD		dwSize, dwType;
	char		szHardwareKey[1024];
	char		szEnumName[1024];
	long		lResult, lResult1;
	HKEY		hMainKey = LTNULL;
	HKEY		hSubKey = LTNULL;
	LPSTR		lpWin9xSubKey = "CONFIG MANAGER\\ENUM";
	LPSTR		lpWin9xKeyword = "HARDWAREKEY";
	
	// ONLY ONE LOOP FOR ANYTIME BREAK :P //
	do{
		// INIT VARIABLE
		dwKeyIndex = 0;
		SAFE_CLOSE_KEY( hMainKey );
		lResult = RegOpenKeyEx(HKEY_DYN_DATA, lpWin9xSubKey, 0 , KEY_ENUMERATE_SUB_KEYS|KEY_EXECUTE|KEY_QUERY_VALUE, &hMainKey);		
		if (lResult != ERROR_SUCCESS)break;

		// REAL LOOP FOR FINDING DISPLAY NAME
		while( 1 )
		{
			lResult = RegEnumKey( hMainKey , dwKeyIndex, szEnumName, MAX_PATH + 1);						
			SAFE_CLOSE_KEY( hSubKey );
			lResult1 = RegOpenKeyEx(hMainKey, szEnumName, 0, KEY_READ, &hSubKey );
			if (lResult == ERROR_NO_MORE_ITEMS || lResult1 != ERROR_SUCCESS )break;						
			
			// CHECK WHETHER HARDWARE KEY IS !!szRegPath!!
			dwType = REG_SZ;dwSize = sizeof(szHardwareKey);memset( szHardwareKey, 0, sizeof(szHardwareKey) );
			lResult = RegQueryValueEx(hSubKey, lpWin9xKeyword, NULL, &dwType, (LPBYTE)szHardwareKey, &dwSize);	
			if( lResult != ERROR_SUCCESS ){
				dwKeyIndex++;
				continue;
			}

			if( _strnicmp( szRegPath, szHardwareKey, strlen(szRegPath) ) ){
				dwKeyIndex++;
				continue;
			}

			// SAME NAME RETURN TRUE
			// TERMINATE REGISTRY KEY
			SAFE_CLOSE_KEY( hMainKey );
			SAFE_CLOSE_KEY( hSubKey );				
			return TRUE;
		}
	}while( 0 );

	// TERMINATE REGISTRY KEY
	SAFE_CLOSE_KEY( hMainKey );
	SAFE_CLOSE_KEY( hSubKey );		
	return FALSE;
}



//------------------------------------------------------------------------------
// NAME: GetVgaInfo_WinNT()
// DESC: 
//------------------------------------------------------------------------------
void CSystemInfo::GetVgaInfo_WinNT( NAMEGROUP *namegroup,LPSTR	lpNTSubKey, LPSTR lpNTService)
{
	BOOL		bVerify;
	DWORD		dwKeyIndex, dwSubKeyIndex, dwVerifyLoop;
	DWORD		dwSize, dwType;
	char		szClass[1024], szDesc[1024], szMfg[1024];
	char		szEnumName[1024];
	long		lResult, lResult1;
	HKEY		hMainKey = LTNULL;
	HKEY		hSubKey_1 = LTNULL;
	HKEY		hSubKey_2 = LTNULL;
//	LPSTR		lpNTSubKey = "SYSTEM\\CURRENTCONTROLSET\\ENUM\\PCI";
	LPSTR		lpNTKeyword = "CLASS";
//	LPSTR		lpNTService = "DISPLAY";

	LPSTR		lpNTVerifyName = "CONTROL";

	// SPIKE
	LPSTR		lpNTDesc = "DEVICEDESC";
	LPSTR		lpNTMfg = "MFG";

	// GROBAL INIT
	//namegroup->nItemCnt = 0;

	// ONLY ONE LOOP FOR ANYTIME BREAK :P //
	do{
		// INIT VARIABLE
		dwKeyIndex = 0;
		SAFE_CLOSE_KEY( hMainKey );
		lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpNTSubKey, 0 , KEY_ENUMERATE_SUB_KEYS|KEY_EXECUTE|KEY_QUERY_VALUE, &hMainKey);		
		if (lResult != ERROR_SUCCESS)break;

		// REAL LOOP FOR FINDING DISPLAY NAME
		while( 1 )
		{
			lResult = RegEnumKey( hMainKey , dwKeyIndex, szEnumName, MAX_PATH + 1);						
			SAFE_CLOSE_KEY( hSubKey_1 );
			lResult1 = RegOpenKeyEx(hMainKey, szEnumName, 0, KEY_READ, &hSubKey_1);
			if (lResult == ERROR_NO_MORE_ITEMS || lResult1 != ERROR_SUCCESS )break;	
				
			dwSubKeyIndex = 0;
			while(1)
			{
				lResult = RegEnumKey( hSubKey_1, dwSubKeyIndex, szEnumName, MAX_PATH + 1);	
				SAFE_CLOSE_KEY( hSubKey_2 );
				lResult1 = RegOpenKeyEx(hSubKey_1, szEnumName, 0, KEY_READ, &hSubKey_2);
				if (lResult == ERROR_NO_MORE_ITEMS || lResult1 != ERROR_SUCCESS )break;						
				
				// CHECK WHETHER CLASS IS !!DISPLAY!!
				dwType = REG_SZ;dwSize = sizeof(szClass);memset( szClass, 0, sizeof(szClass) );
				lResult = RegQueryValueEx(hSubKey_2, lpNTKeyword, NULL, &dwType, (LPBYTE)szClass, &dwSize);				

				if( lResult != ERROR_SUCCESS )
				{
					dwSubKeyIndex++;
					continue;
				}

				if( _strnicmp( szClass, lpNTService, strlen(szClass) ) )
				{
					dwSubKeyIndex++;
					continue;
				}

				// INIT SECOND LOOP
				dwVerifyLoop = 0;
				bVerify = TRUE;
				// SECOND LOOP FOR VERIFYING IT IS USING DEVICE
				while( 1 )
				{
					lResult = RegEnumKey( hSubKey_2, dwVerifyLoop, szEnumName, MAX_PATH + 1);										
					if (lResult == ERROR_NO_MORE_ITEMS  ){
						bVerify = FALSE;
						break;						
					}
					
					if( !_strnicmp( szEnumName, lpNTVerifyName, strlen(szEnumName) ) ){
						bVerify = TRUE;
						break;
					}

					dwVerifyLoop++;
				}

				if( bVerify ){
					// GET DISPLAY`NANE
					dwType = REG_SZ;
					dwSize = sizeof(szDesc);
					memset( szDesc, 0, sizeof(szDesc) );
					memset( szMfg, 0, sizeof(szMfg) );
					
					lResult = RegQueryValueEx(hSubKey_2, lpNTMfg, NULL, &dwType, (LPBYTE)szMfg, &dwSize);	
					if( lResult != ERROR_SUCCESS )
						break;
					
					namegroup->nItemCnt++;
					strcat(szMfg, "   ");
					strcpy( namegroup->szName[namegroup->nItemCnt-1], szMfg );

					lResult = RegQueryValueEx(hSubKey_2, lpNTDesc, NULL, &dwType, (LPBYTE)szDesc, &dwSize);	
					if( lResult != ERROR_SUCCESS )
						break;
					
					strcat( namegroup->szName[namegroup->nItemCnt-1], szDesc );
					////////////////////////////////////////////////////////////////////////
					//Ver...
					char Ver[64] = {0,};
					dwSize = 64;
					lResult = RegQueryValueEx(hSubKey_2, "Driver", NULL, &dwType, (LPBYTE)Ver, &dwSize);	
					if( lResult == ERROR_SUCCESS )
					{
						TCHAR strTemp[512];
						wsprintf( strTemp, "SYSTEM\\CURRENTCONTROLSET\\control\\Class\\%s", Ver );

						char Ver1[32] = {0,};
						GetRegValue(HKEY_LOCAL_MACHINE, strTemp, "DriverVersion", dwType, Ver1, 32);

						strcat(namegroup->szName[namegroup->nItemCnt-1], "  DriverVer:");
						strcat(namegroup->szName[namegroup->nItemCnt-1], Ver1);

						//AfxMessageBox(Ver1);
					}
					//Ver...
					////////////////////////////////////////////////////////////////////////
				}

				dwSubKeyIndex++;				
			}

			dwKeyIndex++;			
		}		
	}while( 0 );

	// TERMINATE REGISTRY KEY

	SAFE_CLOSE_KEY( hMainKey );
	SAFE_CLOSE_KEY( hSubKey_1 );
	SAFE_CLOSE_KEY( hSubKey_2 );
}



//------------------------------------------------------------------------------
// NAME: GetRegValue()
// DESC: 
//------------------------------------------------------------------------------
BOOL CSystemInfo::GetRegValue(HKEY hRootKey, LPCTSTR lpKeyPath, LPCTSTR lpValueName, DWORD nType, PVOID pValue, DWORD nSize)
{
	DWORD ret;
	HKEY hKey = LTNULL;
	memset(pValue,0,nSize);

	if(RegOpenKeyEx(hRootKey, lpKeyPath, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		ret = RegQueryValueEx(hKey,lpValueName,NULL,&nType,(LPBYTE)pValue,&nSize);
		SAFE_CLOSE_KEY(hKey);
		if(ret == ERROR_SUCCESS)
			return TRUE;
	}

	return FALSE;
}



//------------------------------------------------------------------------------
// NAME: GetCPUInfo()
// DESC: 
//------------------------------------------------------------------------------
BOOL CSystemInfo::GetCPUInfo( LPSTR lpBuf )
{

	DWORD	dwSpd = calculateCpuSpeed();

//	TCHAR	szSpd[32];
//	wsprintf( szSpd, "%d", dwSpd );
//	AfgMessageBox( szSpd );

#define CPU_KEY "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0"
#define CPU_VAL "ProcessorNameString"
#define CPU_VAL2 "Identifier"

	HKEY	hKey = LTNULL;
	if ( ERROR_SUCCESS != RegOpenKeyEx( HKEY_LOCAL_MACHINE, CPU_KEY, 0, KEY_READ, &hKey ) )
		return FALSE;

	TCHAR	szBuf[512];
	ZeroMemory( szBuf, sizeof( szBuf ) );

	DWORD	dwType	= REG_SZ,
			dwSize	= sizeof( szBuf );

	if ( ERROR_SUCCESS != RegQueryValueEx( hKey, CPU_VAL, NULL, &dwType, (LPBYTE)szBuf, &dwSize ) )
	{
		if ( ERROR_SUCCESS != RegQueryValueEx( hKey, CPU_VAL2, NULL, &dwType, (LPBYTE)szBuf, &dwSize ) )
		{
            SAFE_CLOSE_KEY( hKey );
			return FALSE;
		}
	} 
	SAFE_CLOSE_KEY( hKey );

	strcpy( lpBuf, szBuf );

	return TRUE;
}



//------------------------------------------------------------------------------
// NAME: GetCPUEx()
// DESC: 
//------------------------------------------------------------------------------
LTFLOAT CSystemInfo::GetCPUEx()
{
	DWORD	dwSpd = calculateCpuSpeed();
	TCHAR	szSpd[128];
	if ( dwSpd >= 1024 )
	{
		TCHAR	szRem[8];
		div_t	dvSpd = div( (int)dwSpd, 1024 );
		wsprintf( szRem, "%d", dvSpd.rem );

		if ( ( (int)strlen( szRem ) ) >= 2 )
			wsprintf( szSpd, "   (%d.%c%cGhz)", dvSpd.quot, szRem[0], szRem[1] );
		else
			wsprintf( szSpd, "   (%d.%cGhz)", dvSpd.quot, szRem[0] );
	}
	else
		wsprintf( szSpd, "   (%dMhz)", dwSpd );

    _p_info info;
	ZeroMemory( &info, sizeof( _p_info ) );

	_cpuid ( &info );

	// SPIKE
	LTFLOAT fResultSpd = 0.0f;
	fResultSpd += ((LTFLOAT)dwSpd)/1000;

	return fResultSpd;
}



//------------------------------------------------------------------------------
// NAME: GetCPUEx()
// DESC: 
//------------------------------------------------------------------------------
LTBOOL CSystemInfo::GetCPUDesc( LPSTR lpCPU )
{
	DWORD	dwSpd = calculateCpuSpeed();
	TCHAR	szSpd[128];

	if( !lpCPU ) return LTFALSE;

	if ( dwSpd >= 1024 )
	{
		TCHAR	szRem[8];
		div_t	dvSpd = div( (int)dwSpd, 1024 );
		wsprintf( szRem, "%d", dvSpd.rem );

		if ( ( (int)strlen( szRem ) ) >= 2 )
			wsprintf( szSpd, "   (%d.%c%cGhz)", dvSpd.quot, szRem[0], szRem[1] );
		else
			wsprintf( szSpd, "   (%d.%cGhz)", dvSpd.quot, szRem[0] );
	}
	else
		wsprintf( szSpd, "   (%dMhz)", dwSpd );

    _p_info info;
	ZeroMemory( &info, sizeof( _p_info ) );

	_cpuid ( &info );

	if ( info.v_name[0] )
		wsprintf( lpCPU, "(%s) %s %s", info.v_name, info.model_name, szSpd );
	else
		wsprintf( lpCPU, "%s %s", info.model, szSpd );

	return LTTRUE;
}



//------------------------------------------------------------------------------
// NAME: calculateCpuSpeed()
// DESC: 
//------------------------------------------------------------------------------
DWORD CSystemInfo::calculateCpuSpeed()
{
   int   nTimeStart = 0;
   int   nTimeStop = 0;
   DWORD dwStartTicks = 0;
   DWORD dwEndTicks = 0;
   DWORD dwTotalTicks = 0;
   DWORD dwCpuSpeed = 0;

   nTimeStart = timeGetTime();

   for(;;)
   {
      nTimeStop = timeGetTime();

      if ((nTimeStop - nTimeStart) > 1)
      {
         dwStartTicks = determineTimeStamp();
         break;
      }
   }

   nTimeStart = nTimeStop;

   for(;;)
   {
      nTimeStop = timeGetTime();
      if ((nTimeStop - nTimeStart) > 500)    // one-half second
      {
         dwEndTicks = determineTimeStamp();
         break;
      }
   }

   dwTotalTicks = dwEndTicks - dwStartTicks;
   dwCpuSpeed = dwTotalTicks / 500000;

   return (dwCpuSpeed);
}



//------------------------------------------------------------------------------
// NAME: IsCPUID()
// DESC: 
//------------------------------------------------------------------------------
int CSystemInfo::IsCPUID()
{
    __try {
        _asm {
            xor eax, eax
            cpuid
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        return 0;
    }
    return 1;
}



//------------------------------------------------------------------------------
// NAME: _cpuid()
// DESC: 
//------------------------------------------------------------------------------
int CSystemInfo::_cpuid (_p_info *pinfo)
{
    DWORD dwStandard = 0;
    DWORD dwFeature = 0;
    DWORD dwMax = 0;
    DWORD dwExt = 0;
    int feature = 0;
    int os_support = 0;
    union {
        char cBuf[12+1];
        struct {
            DWORD dw0;
            DWORD dw1;
            DWORD dw2;
        } s;
    } Ident;

    if (!IsCPUID()) {
        return 0;
    }

    _asm {
        push ebx
        push ecx
        push edx

        // get the vendor string
        xor eax, eax
        cpuid
        mov dwMax, eax
        mov Ident.s.dw0, ebx
        mov Ident.s.dw1, edx
        mov Ident.s.dw2, ecx

        // get the Standard bits
        mov eax, 1
        cpuid
        mov dwStandard, eax
        mov dwFeature, edx

        // get AMD-specials
        mov eax, 80000000h
        cpuid
        cmp eax, 80000000h
        jc notamd
        mov eax, 80000001h
        cpuid
        mov dwExt, edx

notamd:
        pop ecx
        pop ebx
        pop edx
    }

    if (dwFeature & _MMX_FEATURE_BIT) {
        feature |= _CPU_FEATURE_MMX;
        if (_os_support(_CPU_FEATURE_MMX))
            os_support |= _CPU_FEATURE_MMX;
    }
    if (dwExt & _3DNOW_FEATURE_BIT) {
        feature |= _CPU_FEATURE_3DNOW;
        if (_os_support(_CPU_FEATURE_3DNOW))
            os_support |= _CPU_FEATURE_3DNOW;
    }
    if (dwFeature & _SSE_FEATURE_BIT) {
        feature |= _CPU_FEATURE_SSE;
        if (_os_support(_CPU_FEATURE_SSE))
            os_support |= _CPU_FEATURE_SSE;
    }
    if (dwFeature & _SSE2_FEATURE_BIT) {
        feature |= _CPU_FEATURE_SSE2;
        if (_os_support(_CPU_FEATURE_SSE2))
            os_support |= _CPU_FEATURE_SSE2;
    }

    if (pinfo) {
        memset(pinfo, 0, sizeof(_p_info));

        pinfo->os_support = os_support;
        pinfo->feature = feature;
        pinfo->family = (dwStandard >> 8) & 0xF; // retrieve family
        if (pinfo->family == 15) {               // retrieve extended family
            pinfo->family |= (dwStandard >> 16) & 0xFF0;
        }
        pinfo->model = (dwStandard >> 4) & 0xF;  // retrieve model
        if (pinfo->model == 15) {                // retrieve extended model
            pinfo->model |= (dwStandard >> 12) & 0xF;
        }
        pinfo->stepping = (dwStandard) & 0xF;    // retrieve stepping

        Ident.cBuf[12] = 0;
        strcpy(pinfo->v_name, Ident.cBuf);

        map_mname(pinfo->family, 
                  pinfo->model,
                  pinfo->v_name,
                  pinfo->model_name);

        pinfo->checks = _CPU_FEATURE_MMX |
                        _CPU_FEATURE_SSE |
                        _CPU_FEATURE_SSE2 |
                        _CPU_FEATURE_3DNOW;
    }

    return feature;
}



//------------------------------------------------------------------------------
// NAME: determineTimeStamp()
// DESC: 
//------------------------------------------------------------------------------
DWORD CSystemInfo::determineTimeStamp()
{
   DWORD dwTickVal;

   __asm
   {
      _emit 0Fh
      _emit 31h
      mov   dwTickVal, eax
   }

   return (dwTickVal);
}



//------------------------------------------------------------------------------
// NAME: map_mname()
// DESC: 
//------------------------------------------------------------------------------
void CSystemInfo::map_mname(int family, int model, const char *v_name, char *m_name)
{
    // Default to name not known
    m_name[0] = '\0';

    if (!strncmp("AuthenticAMD", v_name, 12)) {
        switch (family) { // extract family code
        case 4: // Am486/AM5x86
            strcpy (m_name, "AMD Am486");
            break;

        case 5: // K6
            switch (model) { // extract model code
            case 0:
            case 1:
            case 2:
            case 3:
                strcpy (m_name, "AMD K5");
                break;
            case 6:
            case 7:
                strcpy (m_name, "AMD K6");
                break;
            case 8:
                strcpy (m_name, "AMD K6-2");
                break;
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
                strcpy (m_name, "AMD K6-3");
                break;
            }
            break;

        case 6: // Athlon
            // No model numbers are currently defined
            strcpy (m_name, "AMD ATHLON");
            break;
        }
    }
    else if (!strncmp("GenuineIntel", v_name, 12)) {
        switch (family) { // extract family code
        case 4:
            switch (model) { // extract model code
            case 0:
            case 1:
                strcpy (m_name, "Intel 486DX");
                break;
            case 2:
                strcpy (m_name, "Intel 486SX");
                break;
            case 3:
                strcpy (m_name, "Intel 486DX2");
                break;
            case 4:
                strcpy (m_name, "Intel 486SL");
                break;
            case 5:
                strcpy (m_name, "Intel 486SX2");
                break;
            case 7:
                strcpy (m_name, "Intel 486DX2E");
                break;
            case 8:
                strcpy (m_name, "Intel 486DX4");
                break;
            }
            break;

        case 5:
            switch (model) { // extract model code
            case 1:
            case 2:
            case 3:
                strcpy (m_name, "Intel Pentium");
                break;
            case 4:
                strcpy (m_name, "Intel Pentium MMX");
                break;
            }
            break;

        case 6:
            switch (model) { // extract model code
            case 1:
                strcpy (m_name, "Intel Pentium PRO");
                break;
            case 3:
            case 5:
                strcpy (m_name, "Intel Pentium II");
                break;  // actual differentiation depends on cache settings
            case 6:
                strcpy (m_name, "Intel Celeron");
                break;
            case 7:
            case 8:
            case 10:
                strcpy (m_name, "Intel Pentium III");
                break;  // actual differentiation depends on cache settings
            }
            break;

        case 15 | (0x00 << 4): // family 15, extended family 0x00
			strcpy (m_name, "Intel Pentium IV");
            break;
        }
    }
    else if (!strncmp("CyrixInstead", v_name, 12)) {
        strcpy (m_name, "Cyrix");
    }
    else if (!strncmp("CentaurHauls", v_name, 12)) {
        strcpy (m_name, "Centaur");
    }

    if (!m_name[0]) 
	{
        strcpy (m_name, "Unknown");
    }
}



//------------------------------------------------------------------------------
// NAME: _os_support()
// DESC: 
//------------------------------------------------------------------------------
int CSystemInfo::_os_support(int feature)
{
 /*   __try {
        switch (feature) {
        case _CPU_FEATURE_SSE:
            __asm {
                xorps xmm0, xmm0        // executing SSE instruction
            }
            break;
        case _CPU_FEATURE_SSE2:
            __asm {
                xorpd xmm0, xmm0        // executing SSE2 instruction
            }
            break;
        case _CPU_FEATURE_3DNOW:
            __asm {
                pfrcp mm0, mm0          // executing 3DNow! instruction
                emms
            }
            break;
        case _CPU_FEATURE_MMX:
            __asm {
                pxor mm0, mm0           // executing MMX instruction
                emms
            }
            break;
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        if (_exception_code() == STATUS_ILLEGAL_INSTRUCTION) {
            return 0;
        }
        return 0;
    }
*/
    return 1;
}



//------------------------------------------------------------------------------
// NAME: GetVgaInfo()
// DESC: 
//------------------------------------------------------------------------------
void CSystemInfo::GetVgaFullInfo(char *szInfo,LPSTR lpNTService)
{
	NAMEGROUP namegroup;
	namegroup.nItemCnt = 0;
	
	__try
	{
		if ( GetVersion() < 0x80000000)
		{		
			// WinNT Series //
			GetVgaFullInfo_WinNT( &namegroup,"SYSTEM\\CURRENTCONTROLSET\\ENUM\\PCI",lpNTService );
			GetVgaFullInfo_WinNT( &namegroup,"SYSTEM\\CURRENTCONTROLSET\\ENUM\\ISAPNP",lpNTService );
			// WinNT Series //
		}
		else
		{
			// Win9X Series //
			GetVgaFullInfo_Win9x( &namegroup, "ENUM\\PCI",lpNTService);
			GetVgaFullInfo_Win9x( &namegroup, "ENUM\\ISAPNP",lpNTService);
			// Win9X Series //
		}

		/* it use for several device */		
		int nCnt = 0;
		char cToken;
		cToken = ',';

		if( !(namegroup.nItemCnt) )
		{
			strcpy( szInfo, "none" );
		}
		else
		{
			strcpy( szInfo, namegroup.szName[0] );
		}

		nCnt = 1;
		for( int i = 2 ; i < namegroup.nItemCnt; i++ ){		
			wsprintf( szInfo, "%s%c%s", szInfo, cToken, namegroup.szName[i] );
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		if( szInfo )
		{
			strcpy( szInfo, "ERR" );
		}
	}
}




//------------------------------------------------------------------------------
// NAME: GetVgaInfo_Win9x()
// DESC: 
//------------------------------------------------------------------------------
void CSystemInfo::GetVgaFullInfo_Win9x( NAMEGROUP *namegroup,LPSTR lpWin9xSubKey,LPSTR	lpWin9xService )
{
	HKEY		hMainKey = LTNULL;
	HKEY		hSubKey_1 = LTNULL;
	HKEY		hSubKey_2 = LTNULL;
	long		lResult, lResult1;
	DWORD		dwKeyIndex, dwSubKeyIndex ;
	DWORD		dwSize, dwType;
	LPSTR		lpWin9xKeyword	= "CLASS";
	//LPSTR		lpWin9xService	= "DISPLAY";	
	//LPSTR		lpWin9xSubKey	= "ENUM\\PCI";
	LPSTR		lpWin9xDesc		= "DEVICEDESC";
	char		szRegPath[1024], szSubRegPath[1024], szEnumName[1024], szClass[1024], szDesc[1024];

	memset( szRegPath, 0, sizeof(szRegPath) );
	memset( szSubRegPath, 0, sizeof(szRegPath) );
	memset( szEnumName, 0, sizeof(szRegPath) );
	memset( szClass, 0, sizeof(szRegPath) );
	memset( szDesc, 0, sizeof(szRegPath) );

	// GROBAL INIT
	//namegroup->nItemCnt = 0;
	// clear namegroup
	
	// ONLY ONE LOOP FOR ANYTIME BREAK :P //
	do{
		// INIT VARIABLE
		dwKeyIndex = 0;
		SAFE_CLOSE_KEY( hMainKey );
		lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpWin9xSubKey, 0 , KEY_ENUMERATE_SUB_KEYS|KEY_EXECUTE|KEY_QUERY_VALUE, &hMainKey);		
		if (lResult != ERROR_SUCCESS)break;

		// REAL LOOP FOR FINDING DISPLAY NAME
		while( 1 )
		{
			// INIT PATH BUFF			
			if( lpWin9xSubKey == "ENUM\\PCI")
				strcpy( szRegPath, "PCI" );
			else
				strcpy( szRegPath, "ISAPNP" );

			lResult = RegEnumKey( hMainKey , dwKeyIndex, szEnumName, MAX_PATH + 1);				
			SAFE_CLOSE_KEY( hSubKey_1 );
			lResult1 = RegOpenKeyEx(hMainKey, szEnumName, 0, KEY_READ, &hSubKey_1);
			if (lResult == ERROR_NO_MORE_ITEMS || lResult1 != ERROR_SUCCESS )break;						

			// ADD PATH
			wsprintf( szRegPath, "%s\\%s", szRegPath, szEnumName );

			memset( szSubRegPath, 0, sizeof(szSubRegPath) );
			dwSubKeyIndex = 0;
			while(1)
			{
				lResult = RegEnumKey( hSubKey_1, dwSubKeyIndex, szEnumName, MAX_PATH + 1);						
				SAFE_CLOSE_KEY( hSubKey_2 );
				lResult1 = RegOpenKeyEx(hSubKey_1, szEnumName, 0, KEY_READ, &hSubKey_2);
				if (lResult == ERROR_NO_MORE_ITEMS || lResult1 != ERROR_SUCCESS )break;						

				// ADD PATH
				wsprintf( szSubRegPath, "%s\\%s", szRegPath, szEnumName );

				// CHECK WHETHER CLASS IS !!DISPLAY!!
				dwType = REG_SZ;dwSize = sizeof(szClass);memset( szClass, 0, sizeof(szClass) );
				lResult = RegQueryValueEx(hSubKey_2, lpWin9xKeyword, NULL, &dwType, (LPBYTE)szClass, &dwSize);	
				if( lResult != ERROR_SUCCESS ){
					dwSubKeyIndex++;
					continue;
				}

				if( _strnicmp( szClass, lpWin9xService, strlen(szClass) ) )break;				

				if( !IsDynamicDeviceOnWin9x( szSubRegPath ) ){
					dwSubKeyIndex++;
					continue;
				}

				//GET DISPLAY NAME			
				dwType = REG_SZ;dwSize = sizeof(szDesc);memset( szDesc, 0, sizeof(szDesc) );
				lResult = RegQueryValueEx(hSubKey_2, lpWin9xDesc, NULL, &dwType, (LPBYTE)szDesc, &dwSize);	
				if( lResult != ERROR_SUCCESS )break;
								
				strcpy( namegroup->szName[namegroup->nItemCnt], szDesc );				

					////////////////////////////////////////////////////////////////////////
					//Ver...
					char Ver[64] = {0,};
					dwSize = 64;
					lResult = RegQueryValueEx(hSubKey_2, "Driver", NULL, &dwType, (LPBYTE)Ver, &dwSize);	
					if( lResult == ERROR_SUCCESS )
					{
						TCHAR strTemp[512];
						wsprintf( strTemp, "System\\CurrentControlSet\\Services\\Class\\%s", Ver );
						
						char Ver1[32] = {0,};
						GetRegValue(HKEY_LOCAL_MACHINE, strTemp, "Ver", dwType, Ver1, 32);

						strcat(namegroup->szName[namegroup->nItemCnt], "  DriverVer:");
						strcat(namegroup->szName[namegroup->nItemCnt], Ver1);
						//AfxMessageBox(Ver1);
					}
					//Ver...
					////////////////////////////////////////////////////////////////////////
//HKEY_LOCAL_MACHINE\System\CurrentControlSet\Services\Class\Display\0000    Ver
//Driver : DISPLAY\0000

				namegroup->nItemCnt++;				
				dwSubKeyIndex++;
			}

			dwKeyIndex++;
		}

	}while(0);
	
	// TERMINATE REGISTRY KEY
	SAFE_CLOSE_KEY( hMainKey );
	SAFE_CLOSE_KEY( hSubKey_1 );
	SAFE_CLOSE_KEY( hSubKey_2 );
}




//------------------------------------------------------------------------------
// NAME: GetVgaInfo_WinNT()
// DESC: 
//------------------------------------------------------------------------------
void CSystemInfo::GetVgaFullInfo_WinNT( NAMEGROUP *namegroup,LPSTR	lpNTSubKey, LPSTR lpNTService)
{
	BOOL		bVerify;
	DWORD		dwKeyIndex, dwSubKeyIndex, dwVerifyLoop;
	DWORD		dwSize, dwType;
	char		szClass[1024], szDesc[1024];
	char		szEnumName[1024];
	long		lResult, lResult1;
	HKEY		hMainKey = LTNULL;
	HKEY		hSubKey_1 = LTNULL;
	HKEY		hSubKey_2 = LTNULL;
//	LPSTR		lpNTSubKey = "SYSTEM\\CURRENTCONTROLSET\\ENUM\\PCI";
	LPSTR		lpNTKeyword = "CLASS";
//	LPSTR		lpNTService = "DISPLAY";

	LPSTR		lpNTVerifyName = "CONTROL";
	LPSTR		lpNTDesc = "DEVICEDESC";

	// GROBAL INIT
	//namegroup->nItemCnt = 0;

	// ONLY ONE LOOP FOR ANYTIME BREAK :P //
	do{
		// INIT VARIABLE
		dwKeyIndex = 0;
		SAFE_CLOSE_KEY( hMainKey );
		lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpNTSubKey, 0 , KEY_ENUMERATE_SUB_KEYS|KEY_EXECUTE|KEY_QUERY_VALUE, &hMainKey);		
		if (lResult != ERROR_SUCCESS)break;

		// REAL LOOP FOR FINDING DISPLAY NAME
		while( 1 )
		{
			lResult = RegEnumKey( hMainKey , dwKeyIndex, szEnumName, MAX_PATH + 1);						
			SAFE_CLOSE_KEY( hSubKey_1 );
			lResult1 = RegOpenKeyEx(hMainKey, szEnumName, 0, KEY_READ, &hSubKey_1);
			if (lResult == ERROR_NO_MORE_ITEMS || lResult1 != ERROR_SUCCESS )break;	
				
			dwSubKeyIndex = 0;
			while(1)
			{
				lResult = RegEnumKey( hSubKey_1, dwSubKeyIndex, szEnumName, MAX_PATH + 1);						
				SAFE_CLOSE_KEY( hSubKey_2 );
				lResult1 = RegOpenKeyEx(hSubKey_1, szEnumName, 0, KEY_READ, &hSubKey_2);
				if (lResult == ERROR_NO_MORE_ITEMS || lResult1 != ERROR_SUCCESS )break;						
				
				// CHECK WHETHER CLASS IS !!DISPLAY!!
				dwType = REG_SZ;dwSize = sizeof(szClass);memset( szClass, 0, sizeof(szClass) );
				lResult = RegQueryValueEx(hSubKey_2, lpNTKeyword, NULL, &dwType, (LPBYTE)szClass, &dwSize);				

				if( lResult != ERROR_SUCCESS )
				{
					dwSubKeyIndex++;
					continue;
				}

				if( _strnicmp( szClass, lpNTService, strlen(szClass) ) )
				{
					dwSubKeyIndex++;
					continue;
				}

				// INIT SECOND LOOP
				dwVerifyLoop = 0;
				bVerify = TRUE;
				// SECOND LOOP FOR VERIFYING IT IS USING DEVICE
				while( 1 )
				{
					lResult = RegEnumKey( hSubKey_2, dwVerifyLoop, szEnumName, MAX_PATH + 1);										
					if (lResult == ERROR_NO_MORE_ITEMS  ){
						bVerify = FALSE;
						break;						
					}
					
					if( !_strnicmp( szEnumName, lpNTVerifyName, strlen(szEnumName) ) ){
						bVerify = TRUE;
						break;
					}

					dwVerifyLoop++;
				}

				if( bVerify ){
					// GET DISPLAY`NANE
					dwType = REG_SZ;
					dwSize = sizeof(szDesc);
					memset( szDesc, 0, sizeof(szDesc) );

					lResult = RegQueryValueEx(hSubKey_2, lpNTDesc, NULL, &dwType, (LPBYTE)szDesc, &dwSize);	
					if( lResult != ERROR_SUCCESS )
						break;

					namegroup->nItemCnt++;
					strcpy( namegroup->szName[namegroup->nItemCnt-1], szDesc );
					////////////////////////////////////////////////////////////////////////
					//Ver...
					char Ver[64] = {0,};
					dwSize = 64;
					lResult = RegQueryValueEx(hSubKey_2, "Driver", NULL, &dwType, (LPBYTE)Ver, &dwSize);	
					if( lResult == ERROR_SUCCESS )
					{
						TCHAR strTemp[512];
						wsprintf( strTemp, "SYSTEM\\CURRENTCONTROLSET\\control\\Class\\%s", Ver );

						char Ver1[32] = {0,};
						GetRegValue(HKEY_LOCAL_MACHINE, strTemp, "DriverVersion", dwType, Ver1, 32);

						strcat(namegroup->szName[namegroup->nItemCnt-1], "  DriverVer:");
						strcat(namegroup->szName[namegroup->nItemCnt-1], Ver1);

						//AfxMessageBox(Ver1);
					}
					//Ver...
					////////////////////////////////////////////////////////////////////////
				}				
				dwSubKeyIndex++;				
			}			
			dwKeyIndex++;			
		}		
	}while( 0 );

	// TERMINATE REGISTRY KEY

	SAFE_CLOSE_KEY( hMainKey );
	SAFE_CLOSE_KEY( hSubKey_1 );
	SAFE_CLOSE_KEY( hSubKey_2 );
}
