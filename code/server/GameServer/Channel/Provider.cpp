#include "StdAfx.h"

#include "./Provider.h"
#include "../IRoom.h"

namespace GameServer {		namespace Channel {

Provider::Provider()
{
	m_pPool					= new POOL;
	m_pTree					= new TREE;
}

Provider::~Provider()
{
	SAFE_DELETE( m_pTree );
	SAFE_DELETE( m_pPool );
}

void
Provider::Capture( CBYTE i_byIndex, CINT i_nMaxUser )
{
	m_nMaxUser				= i_nMaxUser;
	m_byIndex				= i_byIndex;
	m_pLobby				= IRoom::IFACTORY()->Create( this );
}

void
Provider::Release()
{
	IRoom::IFACTORY()->Destroy( m_pLobby );
	m_pLobby				= NULL;
}

CBYTE
Provider::GetIndex()
{
	return m_byIndex;
}

IRoom *
Provider::GetLobby()
{
	return m_pLobby;
}

CINT
Provider::GetMaxUser()
{
	return m_nMaxUser;
}

CINT
Provider::GetCurrentUser()
{
	INT nCount				= 0;
	nCount += m_pLobby->GetPlayerCount();
	
	ITERATOR stItr			= m_pTree->GetRoot();
	while( stItr.GetItem() != NULL )
	{
		COUPLER * pCoupler	= stItr.GetItem();
		nCount += pCoupler->clsItemB->GetPlayerCount();
		
		stItr.MoveNext();
	}
	return nCount;
}

CINT
Provider::GetRoomCount()
{
	return m_pTree->GetCount();
}

IRoom *
Provider::Search( CINT i_nRoomIndex )
{
	const COUPLER * pCoupler = m_pTree->Search( i_nRoomIndex );
	if( pCoupler == NULL )
	{
		return NULL;
	}

	return pCoupler->clsItemB;
}

void
Provider::AddRoom( IRoom * i_pRoom )
{
	COUPLER * pCoupler		= m_pPool->Capture();
	pCoupler->clsItemA		= i_pRoom->GetIndex();
	pCoupler->clsItemB		= i_pRoom;

	m_pTree->Insert( pCoupler );
}

void
Provider::RemoveRoom( IRoom * i_pRoom )
{
	COUPLER * pCoupler		= m_pTree->Remove( i_pRoom->GetIndex() );
	if( pCoupler != NULL )
	{
		m_pPool->Release( pCoupler );
		return;
	}

	DEBUG_BREAK;
}
	
IRoom *
Provider::GetFirstRoom()
{
	m_stItr					= m_pTree->GetRoot();
	COUPLER * pCoupler		= m_stItr.GetItem();
	if( pCoupler == NULL )
	{
		return NULL;
	}

	return pCoupler->clsItemB;
}

IRoom *
Provider::GetNextRoom()
{
	m_stItr.MoveNext();

	COUPLER * pCoupler		= m_stItr.GetItem();
	if( pCoupler == NULL )
	{
		return NULL;
	}

	return pCoupler->clsItemB;
}

} /* Channel */				} /* GameServer */