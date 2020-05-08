#pragma once

#include "../ITable.h"

#include "../../System/Memory/Segment.h"
#include "../../System/Collections/Tree.h"

namespace GameServer {		namespace Table {

class Provider
:	public					ITable
{
public:
							Provider();
	virtual					~Provider();

	void					Capture( const CHAR * i_strSource, CULONG i_ulSourceLength, CHAR i_cRowSplitter, CHAR i_cColumnSplitter, CBOOL bTrim );
	void					Release();

	virtual CULONG			GetMaxRow();
	virtual CULONG			GetMaxColumn();
	
	virtual STRING const *	GetItem( CULONG i_ulRow, CULONG i_ulColumn );

private:
	CBOOL					isTrimChar( const CHAR i_cChar );
	void					insertItem( CSTRING * i_pItem, CULONG i_ulRow, CULONG i_ulColumn, CBOOL i_bTrim );

private:
	static CINT				s_nMaxColumn = 65536;

	typedef					Collections::Coupler<INT, STRING *>
							COUPLER;
	typedef					Memory::Segment<128, COUPLER>
							POOL;
	typedef					Collections::Tree<COUPLER, Less<COUPLER>, POOL>
							TREE;

private:
	POOL *					m_pPool;
	TREE *					m_pTree;

	ULONG					m_ulMaxRow;
	ULONG					m_ulMaxColumn;

};

} /* Table */				} /* GameServer */