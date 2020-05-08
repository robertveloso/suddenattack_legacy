//-----------------------------------------------------------------------------
// NAME: CrashPrt.h
//-----------------------------------------------------------------------------
#ifndef __MURSUM_CRASHPRT_H__
#define __MURSUM_CRASHPRT_H__

#if _MSC_VER > 1000
#pragma once
#endif

typedef enum _MURSUM_DUMP_TYPE
{
    MursumDumpNormal                         = 0x00000000,
    MursumDumpWithDataSegs                   = 0x00000001,
    MursumDumpWithFullMemory                 = 0x00000002,
    MursumDumpWithHandleData                 = 0x00000004,
    MursumDumpFilterMemory                   = 0x00000008,
    MursumDumpScanMemory                     = 0x00000010,
    MursumDumpWithUnloadedModules            = 0x00000020,
    MursumDumpWithIndirectlyReferencedMemory = 0x00000040,
    MursumDumpFilterModulePaths              = 0x00000080,
    MursumDumpWithProcessThreadData          = 0x00000100,
    MursumDumpWithPrivateReadWriteMemory     = 0x00000200,
    MursumDumpWithoutOptionalData            = 0x00000400,
    MursumDumpWithFullMemoryInfo             = 0x00000800,
    MursumDumpWithThreadInfo                 = 0x00001000,
} MURSUM_DUMP_TYPE;


#ifdef CRPT_BUILDDLL
	#undef MURSUMAPI
	#define MURSUMAPI __declspec(dllexport)
#else
	#undef MURSUMAPI
	#define MURSUMAPI __declspec(dllimport)
#endif


extern "C" MURSUMAPI BOOL	MursumCrashReport_Install( LPCTSTR	lpszUserID,
													   LPCTSTR	lpszMailAddr,
													   MURSUM_DUMP_TYPE DumpType = MursumDumpWithHandleData );
extern "C" MURSUMAPI BOOL	MursumCrashReport_SetUserID( LPCTSTR lpszUserID );
extern "C" MURSUMAPI BOOL	MursumCrashReport_AddFile( LPCTSTR   lpszFileName );
extern "C" MURSUMAPI void	MursumCrashReport_UnInstall();



#endif