#include "StdAfx.h"

#include "./Facade.h"

namespace GameServer {		namespace Log {

Facade::Facade()
{
	m_pRecordFail			= NULL;
	m_pChatting				= NULL;

	this->install();
}

Facade::~Facade()
{
	this->uninstall();

	ILog::IFACTORY()->Destroy( m_pChatting );
	ILog::IFACTORY()->Destroy( m_pRecordFail );
}

void
Facade::Open( CPATH & stPath )
{
	Assert( m_pRecordFail == NULL );
	Assert( m_pChatting == NULL );

	STRING strFileName;
	strFileName.Printf( "%s%s", stPath.strRecordFail.GetCSTR(), "RecordFail" );
	m_pRecordFail			= ILog::IFACTORY()->Create( strFileName );

	strFileName.Printf( "%s%s", stPath.strChatting.GetCSTR(), "Chatting" );
	m_pChatting				= ILog::IFACTORY()->Create( strFileName );
}

void
Facade::ChangeFile()
{
	m_pRecordFail->ChangeFile();
	m_pChatting->ChangeFile();
}

void
Facade::RecordFail( CSTRING & strMessage )
{
	Assert( m_pRecordFail != NULL );
	m_pRecordFail->Write( strMessage );
}

void
Facade::Chatting( CSTRING & strMessage )
{
	Assert( m_pChatting != NULL );
	m_pChatting->Write( strMessage );
}

} /* Log */					} /* GameServer */
