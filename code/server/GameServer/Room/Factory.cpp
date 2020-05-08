#include "StdAfx.h"

#include "./Factory.h"

namespace GameServer {		namespace Room {

Factory::Factory()
{
	m_pPool					= new POOL;

	m_pProcess				= new Process::Provider();

	for( INT i = 0; i < OPTION::E_MODE::__MAX__; i++ )
	{
		m_stSpectators[ i ]	= -1;
	}

	this->install();
}

Factory::~Factory()
{
	this->uninstall();

	SAFE_DELETE( m_pProcess );

	SAFE_DELETE( m_pPool );
}

IRoom *
Factory::Create( IChannel * i_pChannel, IPlayer * i_pPlayer, CNAME & i_strName, CPASS & i_strPass, COPTION & i_stOption )
{
	Provider * pProvider	= m_pPool->Capture();
	pProvider->Capture( i_pChannel, i_pPlayer, i_strName, i_strPass, i_stOption );

	return pProvider;
}

IRoom *
Factory::Create( IChannel * i_pChannel )
{
	Provider * pProvider	= m_pPool->Capture();
	pProvider->Capture( i_pChannel );

	return pProvider;
}

void
Factory::Destroy( IRoom * i_pRoom )
{
	Provider * pProvider	= static_cast<Provider *>( i_pRoom );
	
	pProvider->Release();
	m_pPool->Release( pProvider );
}

void
Factory::SetSpectatorTime( OPTION::E_MODE::ENUM i_eMode, CINT i_nSec )
{
	ASSERT( i_nSec >= 0 );
	ASSERT( m_stSpectators[ i_eMode ] < 0 );
	
	m_stSpectators[ i_eMode ]	= i_nSec;
}

CINT
Factory::GetSpectatorTime( OPTION::E_MODE::ENUM i_eMode )
{
	Assert( m_stSpectators[ i_eMode ] >= 0 );
	return m_stSpectators[ i_eMode ];
}

} /* Room */				} /* GameServer */