// TiniLibGame.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//
#include "stdafx.h"
#include "MyGate.h"
#include "CrashRpt.h"
#include "ServerInfo.h"

int _tmain(int argc, _TCHAR* argv[])
{
	MursumCrashReport_Install( "SuddenAttack_Server", "1" );

	SERVER_INFO siInit;
	GetServerInfo( &siInit );

	CMyGate myGate;
	if ( FALSE == myGate.Create( &siInit ) )	return FALSE;

	myGate.Start();

	MursumCrashReport_UnInstall();

	return 0;
}