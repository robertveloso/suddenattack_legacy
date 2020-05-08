#include "StdAfx.h"

#include "./Factory.h"

namespace GameServer {		namespace Table {

Factory::Factory()
{
	m_pTablePool			= new POOL_T;
	m_pStringPool			= new POOL_S;

	this->install();
}

Factory::~Factory()
{
	this->uninstall();

	SAFE_DELETE( m_pTablePool );
	SAFE_DELETE( m_pStringPool );
}

ITable *
Factory::Create( const CHAR * i_strSource, CULONG i_ulSourceLength, CHAR i_cRowSplitter, CHAR i_cColumnSplitter, CBOOL i_bTrim )
{
	Provider * pTable		= m_pTablePool->Capture();
	pTable->Capture( i_strSource, i_ulSourceLength, i_cRowSplitter, i_cColumnSplitter, i_bTrim );

	return pTable;
}

void
Factory::Destroy( ITable * i_pTable )
{
	Provider * pTable		= static_cast<Provider *>( i_pTable );

	pTable->Release();
	m_pTablePool->Release( pTable );
}

STRING *
Factory::CreateItem()
{
	return m_pStringPool->Capture();
}

void
Factory::DestroyItem( STRING * i_pItem )
{
	m_pStringPool->Release( i_pItem );
}

} /* Table */				} /* GameServer */
