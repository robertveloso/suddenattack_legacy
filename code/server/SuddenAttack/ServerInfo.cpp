#include "stdafx.h"
#include "ServerInfo.h"
#include "MyCommon.h"

int GetServerInfo( LP_SERVER_INFO pServerInfo )
{
	char szCurDir[_MAX_PATH]		= {0,};
	char szFinalFilePath[_MAX_PATH] = {0,};

	GetCurrentDirectory( _MAX_PATH, szCurDir );
	wsprintf( szFinalFilePath, "%s\\%s", szCurDir, SERVER_INFO_FILE );

	// 파일이 없을때, 기본값 INI 파일 만들어 주는것은
	// 차후 INI 파일 Encoding / Decoding 모듈이 완성된 후 추가

	pServerInfo->nMaxPacketSize			= ::GetPrivateProfileInt( "BASE_SERVER_INFO", "MAX_PACKET_SIZE", DEFAULT_MAX_PACKET_SIZE, szFinalFilePath );
	pServerInfo->nMaxClients			= ::GetPrivateProfileInt( "BASE_SERVER_INFO", "MAX_CLIENT_CNT", DEFAULT_MAX_CLIENT_CNT, szFinalFilePath );
	pServerInfo->nMaxResponseBufferSize	= ::GetPrivateProfileInt( "BASE_SERVER_INFO", "MAX_RESPONSE_BUF_SIZE", DEFAULT_MAX_RESPONSE_BUF_SIZE, szFinalFilePath );

	pServerInfo->nMaxRoomUser			= ::GetPrivateProfileInt( "EACH_SERVER_INFO", "MAX_ROOM_USER", DEFAULT_MAX_ROOM_USER, szFinalFilePath );
	pServerInfo->nMaxChannels			= ::GetPrivateProfileInt( "EACH_SERVER_INFO", "MAX_CHANNEL", DEFAULT_MAX_CHANNEL, szFinalFilePath );
	pServerInfo->nPort					= ::GetPrivateProfileInt( "EACH_SERVER_INFO", "SERVER_PORT", DEFAULT_SERVER_PORT, szFinalFilePath );

	::GetPrivateProfileString( "EACH_SERVER_INFO", "SERVER_IP", DEFAULT_SERVER_IP, pServerInfo->szIP, 16, szFinalFilePath );

	return TRUE;
}

int GetDBInfo( LP_DB_INFO pDBInfo )
{
	char szCurDir[_MAX_PATH]		= {0,};
	char szFinalFilePath[_MAX_PATH] = {0,};

	GetCurrentDirectory( _MAX_PATH, szCurDir );
	wsprintf( szFinalFilePath, "%s\\%s", szCurDir, SERVER_INFO_FILE );

	// 파일이 없을때, 기본값 INI 파일 만들어 주는것은
	// 차후 INI 파일 Encoding / Decoding 모듈이 완성된 후 추가

	pDBInfo->nAdoConn	= ::GetPrivateProfileInt( "DB_SERVER_INFO", "NUM_OF_ADO_CONN", DEFAULT_DB_CONN_NUM, szFinalFilePath );
	
	::GetPrivateProfileString( "DB_SERVER_INFO", "DATA_SOURCE", DEFAULT_DATA_SOURCE, pDBInfo->szDataSource, 20, szFinalFilePath );
	::GetPrivateProfileString( "DB_SERVER_INFO", "CATALOG", DEFAULT_CATALOG, pDBInfo->szCatalog, 20, szFinalFilePath );
	::GetPrivateProfileString( "DB_SERVER_INFO", "ACC_ID", DEFAULT_ACC_ID, pDBInfo->szAccID, 20, szFinalFilePath );
	::GetPrivateProfileString( "DB_SERVER_INFO", "ACC_PW", DEFAULT_ACC_PW, pDBInfo->szAccPW, 20, szFinalFilePath );

	return TRUE;
}