/*****************************************************************************************************************
CLog

작성자 : 강진협
작성일 : 2004년 3월 2일

설  명 :
로그를 남긴다.
모든 메쏘드는 Static으로 이루어져서 어느 클래스에서도 같은 로그를 기록하도록 한다.
로그 방법은 파일에 기록, console창에 기록, 디버그 창에 기록 이렇게 세가지가 있다.
로그는 언제, 어디서, 누가, 무엇을, 등급 이렇게 5가지를 기록한다.
- 언  제 : 로그 시간
- 어디서 : 클래스&메쏘드 이름
- 누  가 : 로그를 남기는 Object
- 무엇을 : 로그 내용
- 등  급 : 기록하고자 하는 로그의 등급
로그는 5단계의 등급을 두어 각 등급별로 로그의 출력 방향을 달리 한다.
- Always	: 반드시 필요한 등급
- Important	: 로그를 필요할지도 모르는 중요한 등급
- Command	: 명령 수행이 올바른 순서로 흘러가는지 알기위한 위한 등급
- Debug		: 디버깅 용도로써 Command보다 세부적인 정보를 기록하기 위한 등급
- Resource	: 메모리 누수등을 체크하기 위한 등급
사  용 :
1. SetOutputLevel, OpenFile등을 사용하여, 로그를 위한 기본 값을 설정한다.
2. WriteLog를 통해서 로그를 기록한다.
3. Flush를 통해서 파일에 기록한다.
4. 로그 객체를 다 사용한 다음에는 CloseFile을 통해서 파일 핸들을 닫는다.
*****************************************************************************************************************/


#pragma once
#include <Windows.h>

namespace Common
{
	namespace Debug
	{
		class CLog
		{
			// Attributes
		public:
			enum	LogLevel
			{
				Always		= 0,
				Important	= 1,
				Command		= 2,
				Debug		= 3,
				Resource	= 4,
				None		= 5
			};

			// 생성 / 소멸자
		public:
			CLog( int nIndex, const char* strModule, LogLevel logLevel );
			~CLog(VOID);

		private:
			struct	TagOutputLevel
			{
				LogLevel	File;
				LogLevel	Console;
				LogLevel	Debug;
			};

			struct	TagModuleDebug
			{
				char			strModule[100];
				long			lPerf;
				BOOL			bUse;
			};

			static TagModuleDebug		g_ModuleDebug[100];
			LogLevel					m_ModuleLogLevel;
			LARGE_INTEGER				m_iPerfStart;
			int							m_nIndex;

			static TagOutputLevel	g_OutputLevel;
			static FILE*			g_pFileLog;
			static LogLevel			g_LogLevel;

			static BOOL				g_bFileWrite;

			static CRITICAL_SECTION	g_cs;

			static int				g_nIndex;
			static LARGE_INTEGER	g_iPerfStart;

			// Operations
		public:
			static void SpecialStart( int nIndex, const char* strModule, LogLevel logLevel );
			static void SpecialEnd();

			// 출력 방향별 로그를 기록할 레벨을 설정한다.
			static VOID	SetOutputLevel( LogLevel FileLevel, LogLevel ConsoleLevel, LogLevel DebugLevel )
			{
				g_OutputLevel.File		= FileLevel;
				g_OutputLevel.Console	= ConsoleLevel;
				g_OutputLevel.Debug		= DebugLevel;
			}

			// 로그를 기록할 파일을 Open한다.
			static BOOL OpenFile( const char* strFileName );

			// 로그를 기록한 파일을 Close한다.
			static VOID CloseFile();

			// 로그를 기록한다.
			static VOID WriteLog( const char* strWhere, const char* strWho, const char* strWhat, CLog::LogLevel logLevel );

			// 로그를 기록할 파일에 실제적으로 Write하도록 Flush 한다.
			static VOID Flush();

			static VOID	WriteProfile();

		private:
			// 파일에 쓴다.
			static VOID WriteToFile		( const char* strWhen, const char* strWhere, const char* strWho, const char* strWhat, CLog::LogLevel logLevel );
			// 콘솔에 쓴다.
			static VOID WriteToConsole	( const char* strWhen, const char* strWhere, const char* strWho, const char* strWhat, CLog::LogLevel logLevel );
			// 디버그창에 쓴다.
			static VOID WriteToDebug		( const char* strWhen, const char* strWhere, const char* strWho, const char* strWhat, CLog::LogLevel logLevel );

		};// class CLog

	}// namespace Debug
}// namespace Common
