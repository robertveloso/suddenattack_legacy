#include "StdAfx.h"

#include "./Container.h"
#include "./Memento.h"

namespace GameServer {		namespace Player {

Container::Container()
{
	m_pPool					= new POOL;
	m_pTree					= new TREE;

	m_pPoolUserNo			= new POOL_N;
	m_pTreeUserNo			= new TREE_N;

	this->install();

}

Container::~Container()
{
	this->uninstall();

	SAFE_DELETE( m_pTreeUserNo );
	SAFE_DELETE( m_pPoolUserNo );

	SAFE_DELETE( m_pTree );
	SAFE_DELETE( m_pPool );
}

CBOOL
Container::Insert( IPlayer * i_pPlayer )
{
	Assert( IPlayer::IBUILDER()->IsLock() == FALSE );

	COUPLER * pCoupler		= m_pPool->Capture();

	pCoupler->clsItemA		= i_pPlayer->GetMemento()->GetStock().GetNick();
	pCoupler->clsItemB		= i_pPlayer;

	BOOL bSuccess			= m_pTree->Insert( pCoupler );
	if( bSuccess == FALSE )
	{
		return FALSE;
	}


	COUPLER_N * pCouplerUserNo	= m_pPoolUserNo->Capture();

	pCouplerUserNo->clsItemA	= i_pPlayer->GetMemento()->GetUserNo();
	pCouplerUserNo->clsItemB	= i_pPlayer;

	bSuccess					= m_pTreeUserNo->Insert( pCouplerUserNo );
	if( bSuccess == FALSE )
	{
		COUPLER * pCoupler		= m_pTree->Remove( NICK( i_pPlayer->GetMemento()->GetStock().GetNick() ) );

		if( pCoupler == NULL )
		{
			DEBUG_BREAK;
			return FALSE;
		}

		IPlayer * pPlayer		= pCoupler->clsItemB;
		m_pPool->Release( pCoupler );

		return FALSE;
	}


	i_pPlayer->SetLogined();

	return TRUE;
}

void
Container::Remove( IPlayer * i_pPlayer )
{
	COUPLER * pCoupler		= m_pTree->Remove( NICK( i_pPlayer->GetMemento()->GetStock().GetNick() ) );

	if( pCoupler == NULL )
	{
		DEBUG_BREAK;
		return;
	}

	m_pPool->Release( pCoupler );


	COUPLER_N * pCouplerUserNo	= m_pTreeUserNo->Remove( i_pPlayer->GetMemento()->GetUserNo() );

	if( pCouplerUserNo == NULL )
	{
		DEBUG_BREAK;
		return;
	}

	m_pPoolUserNo->Release( pCouplerUserNo );
}

IPlayer *
Container::Find( CNICK & i_strNick )
{
	const COUPLER * pCoupler = m_pTree->Search( i_strNick );
	if( pCoupler == NULL )
	{
		return NULL;
	}

	return pCoupler->clsItemB;
}

IPlayer *
Container::FindUserNo( CUSERNO & i_struserNo )
{
	const COUPLER_N * pCoupler = m_pTreeUserNo->Search( i_struserNo );
	if( pCoupler == NULL )
	{
		return NULL;
	}

	return pCoupler->clsItemB;
}

IPlayer *
Container::First()
{
	m_stIterator			= m_pTree->GetRoot();
	COUPLER * pCoupler		= m_stIterator.GetItem();
	if( pCoupler == NULL )
	{
		return NULL;
	}

	return pCoupler->clsItemB;
}

IPlayer *
Container::Next()
{
	m_stIterator.MoveNext();
	COUPLER * pCoupler		= m_stIterator.GetItem();
	if( pCoupler == NULL )
	{
		return NULL;
	}

	return pCoupler->clsItemB;
}

CINT
Container::GetCount()
{
	return m_pTree->GetCount();
}

} /* Player */				} /* GamerServer */