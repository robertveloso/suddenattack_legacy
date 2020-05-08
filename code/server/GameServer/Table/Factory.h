#pragma once

#include "./Provider.h"

namespace GameServer {		namespace Table {

class Factory
:	public					ITable::IFactory
{
public:
							Factory();
	virtual					~Factory();

	virtual ITable *		Create( const CHAR * i_strSource, CULONG i_ulSourceLength, CHAR i_cRowSplitter, CHAR i_cColumnSplitter, CBOOL i_bTrim );
	virtual void			Destroy( ITable * i_pTable );

	virtual STRING *		CreateItem();
	virtual void			DestroyItem( STRING * i_pItem );

private:
	typedef					Memory::Segment<16, Provider>
							POOL_T;
	typedef					Memory::Segment<256, STRING>
							POOL_S;

private:
	POOL_T *				m_pTablePool;
	POOL_S *				m_pStringPool;
};

} /* Table */				} /* GameServer */