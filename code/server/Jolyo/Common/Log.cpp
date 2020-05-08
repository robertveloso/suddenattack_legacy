#include "StdAfx.h"
#include ".\log.h"

#include <sys/timeb.h>
#include <time.h>

using namespace Common::Debug;

CLog::TagOutputLevel	CLog::g_OutputLevel;
FILE*					CLog::g_pFileLog = NULL;
BOOL					CLog::g_bFileWrite	= FALSE;

CRITICAL_SECTION		CLog::g_cs;
CLog::TagModuleDebug	CLog::g_ModuleDebug[100];

int				CLog::g_nIndex;
LARGE_INTEGER	CLog::g_iPerfStart;


CLog::CLog( int nIndex, const char* strModule, LogLevel logLevel )
{
	m_nIndex = nIndex;
	if( !g_ModuleDebug[nIndex].bUse )
	{
		::strcpy( g_ModuleDebug[nIndex].strModule, strModule );
		g_ModuleDebug[nIndex].bUse = TRUE;
	}

	m_ModuleLogLevel = logLevel;
	::QueryPerformanceCounter( &m_iPerfStart );
}

CLog::~CLog(void)
{
	LARGE_INTEGER freq;
	QueryPerformanceFrequency( &freq );
	LONGLONG frequency = freq.QuadPart;

	LARGE_INTEGER iPerf;
	::QueryPerformanceCounter(&iPerf);

	g_ModuleDebug[m_nIndex].lPerf += static_cast<LONG>( ( iPerf.QuadPart - m_iPerfStart.QuadPart ) / ( frequency / 1000000 ) );
}


void CLog::SpecialStart( int nIndex, const char* strModule, LogLevel logLevel )
{
	g_nIndex = nIndex;
	if( !g_ModuleDebug[nIndex].bUse )
	{
		::strcpy( g_ModuleDebug[nIndex].strModule, strModule );
		g_ModuleDebug[nIndex].bUse = TRUE;
	}

	::QueryPerformanceCounter( &g_iPerfStart );
}

void CLog::SpecialEnd(void)
{
	LARGE_INTEGER freq;
	QueryPerformanceFrequency( &freq );
	LONGLONG frequency = freq.QuadPart;

	LARGE_INTEGER iPerf;
	::QueryPerformanceCounter(&iPerf);

	g_ModuleDebug[g_nIndex].lPerf += static_cast<LONG>( ( iPerf.QuadPart - g_iPerfStart.QuadPart ) / ( frequency / 1000000 ) );
//	char perf[100];
//	sprintf( perf, "%d", ( iPerf.QuadPart - m_iPerfStart.QuadPart ) / ( frequency / 1000000 )  );
//	CLog::WriteLog( m_strModule, perf, "", m_ModuleLogLevel );
//	CLog::Flush();
}



/**************************************************************************************
	Public Operations
***************************************************************************************/

// 로그를 기록할 파일을 Open한다.
BOOL CLog::OpenFile( const char* strFileName )
{
	g_pFileLog = ::fopen( strFileName, "w+" );

	::InitializeCriticalSection( &g_cs );

	for( int i = 0; i < 100; i++ )
	{
		g_ModuleDebug[i].bUse = FALSE;
		g_ModuleDebug[i].lPerf = 0;
	}

	if( g_pFileLog == NULL )
		return FALSE;
	else
		return TRUE;

	g_bFileWrite = FALSE;
}

// 로그를 기록한 파일을 Close한다.
void CLog::CloseFile()
{
	::fclose( g_pFileLog );
}

// 로그를 기록한다.
void CLog::WriteLog( const char* strWhere, const char* strWho, const char* strWhat, CLog::LogLevel logLevel )
{
	char strWhen[128];

	// ex : 04/03/02 14:15:49 -> 04년 3월 2일 14시 15분 49초
	_strdate( strWhen );
	strWhen[8] = ' ';
	_strtime( &strWhen[9] );

	WriteToFile		( strWhen, strWhere, strWho, strWhat, logLevel );
	WriteToConsole	( strWhen, strWhere, strWho, strWhat, logLevel );
	WriteToDebug	( strWhen, strWhere, strWho, strWhat, logLevel );
}

// 로그를 기록할 파일에 실제적으로 Write하도록 Flush 한다.
void CLog::Flush()
{
	if( g_bFileWrite )
		::fflush( g_pFileLog );

	g_bFileWrite = FALSE;
}

void CLog::WriteProfile()
{
	char strPerf[100];
	for( int i = 0; i < 100; i++ )
	{
		if( !g_ModuleDebug[i].bUse )
			continue;

		sprintf( strPerf, "%d", g_ModuleDebug[i].lPerf );
		WriteLog( g_ModuleDebug[i].strModule, strPerf, "", CLog::LogLevel::Resource );
		if( i < 20 )
			g_ModuleDebug[i].lPerf = 0;
	}
	Flush();
}


/**************************************************************************************
	Private Operations
***************************************************************************************/

// 파일에 쓴다.
inline void CLog::WriteToFile( const char* strWhen, const char* strWhere, const char* strWho, const char* strWhat, CLog::LogLevel logLevel )
{
	const char* strToken = " | ";

	if( g_pFileLog == NULL )
		return;

	if( logLevel > g_OutputLevel.File )
		return;

//	::EnterCriticalSection( &g_cs );

	::fwrite( strWhen,  sizeof( char ), ::strlen( strWhen ),  g_pFileLog );
	::fwrite( strToken, sizeof( char ), 3, g_pFileLog );

	::fwrite( strWhere, sizeof( char ), ::strlen( strWhere ), g_pFileLog );
	::fwrite( strToken, sizeof( char ), 3, g_pFileLog );

	::fwrite( strWho,   sizeof( char ), ::strlen( strWho ),   g_pFileLog );
	::fwrite( strToken, sizeof( char ), 3, g_pFileLog );

	::fwrite( strWhat,  sizeof( char ), ::strlen( strWhat ),  g_pFileLog );
	::fwrite( "\r\n",  sizeof( char ), 2,  g_pFileLog );

//	::LeaveCriticalSection( &g_cs );

	g_bFileWrite = TRUE;
}

// 콘솔에 쓴다.
inline void CLog::WriteToConsole( const char* strWhen, const char* strWhere, const char* strWho, const char* strWhat, CLog::LogLevel logLevel )
{
	const char* strToken = " | ";

	if( logLevel > g_OutputLevel.Console )
		return;

	::printf( "%s%s%s%s%s%s%s\n",
		strWhen, strToken,
		strWhere, strToken,
		strWho, strToken,
		strWhat );
}

// 디버그창에 쓴다.
inline void CLog::WriteToDebug( const char* strWhen, const char* strWhere, const char* strWho, const char* strWhat, CLog::LogLevel logLevel )
{
	const char* strToken = " | ";

	if( logLevel > g_OutputLevel.Debug )
		return;

	::OutputDebugString( strWhen );
	::OutputDebugString( strToken );

	::OutputDebugString( strWhere );
	::OutputDebugString( strToken );

	::OutputDebugString( strWho );
	::OutputDebugString( strToken );

	::OutputDebugString( strWhat );
	::OutputDebugString( "\r\n" );
}
