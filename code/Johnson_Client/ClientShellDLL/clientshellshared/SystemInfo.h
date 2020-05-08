#ifndef __MURSUM_SYSTEMINFO_H__
#define __MURSUM_SYSTEMINFO_H__

#if _MSC_VER > 1000
#pragma once
#endif

#define _MMX_FEATURE_BIT        0x00800000
#define _SSE_FEATURE_BIT        0x02000000
#define _SSE2_FEATURE_BIT       0x04000000
#define _3DNOW_FEATURE_BIT      0x80000000

struct FREQ_INFO
{
    unsigned long in_cycles;                                
    unsigned long ex_ticks;     
    unsigned long raw_freq;
    unsigned long norm_freq;
};

typedef struct __CPUINFO
{
	UINT nSpeed;
	char szType[32];
	char szLevel[32];
	char szArchitecture[32];
	char szVenderInfo[32];
	DWORD dwMaxApplicationAddress;
	DWORD dwMinApplicationAddress;
	UINT nActiveProcessMask;
	UINT nProcessorNumber;
	UINT nPageSize;
} ZCPUINFO;

typedef struct tagNAMEGROUP
{
	int nItemCnt;
	char szName[5][1024];	
}NAMEGROUP, *LPNAMEGROUP;

typedef struct __MEMORYINFO
{
	UINT  nUsingPercent;
	DWORD dwPhysicalTotal;
	DWORD dwPhysicalFree;
	DWORD dwVirtualTotal;
	DWORD dwVirtualFree;
}MEMORYINFO;

typedef struct __CPUIDENTIFY
{
   DWORD dwSignature;
   DWORD dwFeatureEbx;
   DWORD dwFeatureEcx;
   DWORD dwFeatures;
}CPUIDENTIFY, FAR* LPCPUINDENTIFY;


#define _CPU_FEATURE_MMX    0x0001
#define _CPU_FEATURE_SSE    0x0002
#define _CPU_FEATURE_SSE2   0x0004
#define _CPU_FEATURE_3DNOW  0x0008

#define _MAX_VNAME_LEN  13
#define _MAX_MNAME_LEN  30

typedef struct _processor_info 
{
    char v_name[_MAX_VNAME_LEN]; 
    char model_name[_MAX_MNAME_LEN];                                        
    int family;                
    int model;                      
    int stepping;                       
    int feature;               
    int os_support;                    
    int checks;                                      
} _p_info;


class CSystemInfo
{
public:
	CSystemInfo();
	virtual ~CSystemInfo();

	BOOL GetMemoryInfo(MEMORYSTATUS* pMemstatus);
	BOOL GetOS(LPSTR lpOS);
	int DetectNumProcessors();
	BOOL GetRegValue(HKEY hRootKey, LPCTSTR lpKeyPath, LPCTSTR lpValueName, DWORD nType, PVOID pValue, DWORD nSize);
	void GetVgaInfo(char *szInfo,LPSTR lpNTService);
	BOOL IsDynamicDeviceOnWin9x(char *szRegPath);
	void GetVgaInfo_WinNT( NAMEGROUP *namegroup,LPSTR	lpNTSubKey,LPSTR	lpNTService );
	void GetVgaInfo_Win9x( NAMEGROUP *namegroup,LPSTR lpWin9xSubKey,LPSTR	lpWin9xService );
	

	void GetVgaFullInfo(char *szInfo,LPSTR lpNTService);
	void GetVgaFullInfo_WinNT( NAMEGROUP *namegroup,LPSTR	lpNTSubKey,LPSTR	lpNTService );
	void GetVgaFullInfo_Win9x( NAMEGROUP *namegroup,LPSTR lpWin9xSubKey,LPSTR	lpWin9xService );


	BOOL GetCPUInfo( LPSTR lpBuf );
	void map_mname(int family, int model, const char *v_name, char *m_name);
	int _os_support(int feature);
	int IsCPUID();
	DWORD determineTimeStamp();
	int _cpuid (_p_info *pinfo);
	DWORD calculateCpuSpeed();
	LTFLOAT GetCPUEx();

	LTBOOL GetCPUDesc( LPSTR lpCPU );
};

#endif
