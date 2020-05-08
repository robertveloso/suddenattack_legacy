#include "StdAfx.h"

#include "./Container.h"

#include "./Login.h"
#include "./Noti.h"
#include "./Kick.h"
#include "./FinishNoti.h"
#include "./Finish.h"
#include "./Monitor.h"

namespace GameServer {		namespace Manager {			namespace Process {

Container::Container()
{
	m_pPool					= new POOL;
	m_pTree					= new TREE;

	this->regist( new Login );
	this->regist( new Noti );
	this->regist( new Kick );
	this->regist( new FinishNoti );
	this->regist( new Finish );
	this->regist( new Monitor );

	this->install();
}

Container::~Container()
{
	this->uninstall();

	SAFE_DELETE( m_pPool );
	SAFE_DELETE( m_pTree );
}

IProcess *
Container::Search( CINT i_nCode )
{
	const COUPLER * pCoupler= m_pTree->Search( i_nCode );
	if( pCoupler == NULL )
	{
		return NULL;
	}
	IProcess * pProcess		= pCoupler->clsItemB;
	return pProcess;
}

inline void
Container::regist( IProcess * i_pProcess )
{
	COUPLER * pCoupler		= m_pPool->Capture();
	pCoupler->clsItemA		= i_pProcess->GetCode();
	pCoupler->clsItemB		= i_pProcess;

	CBOOL bSuccess			= m_pTree->Insert( pCoupler );
	ASSERT( bSuccess == TRUE );
}

} /* Process */				} /* Manager */				} /* GamerServer */