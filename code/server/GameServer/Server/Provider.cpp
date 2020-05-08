#include "StdAfx.h"

#include "./Provider.h"
#include "./Rank/Provider.h"
#include "../IChannel.h"

namespace GameServer {		namespace Server {

Provider::Provider()
{
	m_bOpen					= FALSE;

	m_pList					= new LIST;
	m_pRank					= new Rank::Provider;

	this->install();
}

Provider::~Provider()
{
	this->uninstall();

	SAFE_DELETE( m_pList );
}

void
Provider::Open( CSTRING & i_strIP, CINT i_nPort )
{
	m_strIP					= i_strIP;
	m_nPort					= i_nPort;
	m_bOpen					= TRUE;
}
	
CSTRING &
Provider::GetID()
{
	Assert( m_bOpen == TRUE );

	return m_strID;
}

void
Provider::SetID( CSTRING & i_strID )
{
	Assert( m_bOpen == TRUE );

	m_strID					= i_strID;
}

CSTRING &
Provider::GetName()
{
	Assert( m_bOpen == TRUE );

	return m_strName;
}

void
Provider::SetName( CSTRING & i_strName )
{
	Assert( m_bOpen == TRUE );

	m_strName				= i_strName;
}

CSTRING &
Provider::GetIP()
{
	return m_strIP;
}

CINT
Provider::GetPort()
{
	return m_nPort;
}

CINT
Provider::GetChannelCount()
{
	Assert( m_bOpen == TRUE );

	return m_pList->GetCount();
}

IChannel *
Provider::Search( CBYTE i_byIndex )
{
	ITERATOR stItr			= m_pList->GetHead();
	while( stItr.GetItem() != NULL )
	{
		if( stItr.GetItem()->GetIndex() == i_byIndex )
		{
			return stItr.GetItem();
		}
		stItr.MoveNext();
	}

	return NULL;
}

void
Provider::AddChannel( IChannel * i_pChannel )
{
	Assert( m_bOpen == TRUE );

	m_pList->PushTail( i_pChannel );
}

void
Provider::RemoveChannel( IChannel * i_pChannel )
{
	Assert( m_bOpen == TRUE );

	ITERATOR stItr			= m_pList->GetHead();
	while( stItr.GetItem() != NULL )
	{
		if( stItr.GetItem() == i_pChannel )
		{
			stItr.Unlink();
			return;
		}
		stItr.MoveNext();
	}
}
	
IChannel *
Provider::GetFirstChannel()
{
	Assert( m_bOpen == TRUE );

	m_stItr					= m_pList->GetHead();
	return m_stItr.GetItem();
}

IChannel *
Provider::GetNextChannel()
{
	Assert( m_bOpen == TRUE );

	m_stItr.MoveNext();
	return m_stItr.GetItem();
}

void
Provider::SetLoginGamehi( CBOOL i_bCan )
{
	m_bLoginGamehi			= i_bCan;
}

void
Provider::SetLoginNetmarble( CBOOL i_bCan )
{
	m_bLoginNetmarble		= i_bCan;
}

CBOOL
Provider::CanLoginGamehi()
{
	return m_bLoginGamehi;
}

CBOOL
Provider::CanLoginNetmarble()
{
	return m_bLoginNetmarble;
}

Provider::IRANK *
Provider::GetRank()
{
	return m_pRank;
}

} /* Server */				} /* GameServer */