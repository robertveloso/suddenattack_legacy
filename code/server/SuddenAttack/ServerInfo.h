#ifndef __SERVER_INFORMATION_H__
#define __SERVER_INFORMATION_H__

#pragma once

#define SERVER_INFO_FILE	"SA_Server.ini"


#define DEFAULT_MAX_PACKET_SIZE			8192
#define DEFAULT_MAX_CLIENT_CNT			100
#define DEFAULT_MAX_RESPONSE_BUF_SIZE	65536
#define DEFAULT_MAX_ROOM_USER			32
#define DEFAULT_MAX_CHANNEL				1
#define DEFAULT_SERVER_PORT				12000
#define DEFAULT_SERVER_IP				"211.39.158.149"

typedef struct _tag_SERVER_INFO
{
	INT nMaxPacketSize;
	INT nMaxClients;
	INT nMaxResponseBufferSize;
	INT nMaxRoomUser;
	INT nMaxChannels;
	INT nPort;
	char szIP[16];
} SERVER_INFO, *LP_SERVER_INFO;




#define DEFAULT_DB_CONN_NUM				4
#define DEFAULT_DATA_SOURCE				"211.39.158.152,9637"
#define DEFAULT_CATALOG					"sagmdb"
#define DEFAULT_ACC_ID					"sagmuser"
#define DEFAULT_ACC_PW					"aktlakfh"

typedef struct _tag_DB_INFO
{
	INT nAdoConn;
	char szDataSource[20];
	char szCatalog[20];
	char szAccID[20];
	char szAccPW[20];
} DB_INFO, *LP_DB_INFO;





int GetServerInfo( LP_SERVER_INFO pServerInfo );
int GetDBInfo( LP_DB_INFO pDBInfo );

#endif