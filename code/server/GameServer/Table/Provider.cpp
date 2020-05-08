#include "StdAfx.h"

#include "./Provider.h"

#include "../../Dispatcher/IConsole.h"

namespace GameServer {		namespace Table {

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
Provider::Capture( const CHAR * i_strSource, CULONG i_ulSourceLength, CHAR i_cRowSplitter, CHAR i_cColumnSplitter, CBOOL i_bTrim )
{
	m_ulMaxRow				= 0;
	m_ulMaxColumn			= 0;

	ULONG	ulMaxColumn		= 0;

	STRING * pItem			= NULL;
	ULONG ulIndex			= 0;

	BOOL bAppend			= FALSE;
	BOOL bLTrimed			= FALSE;

	pItem					= IFACTORY()->CreateItem();
	ulIndex					= 0;

	for( ULONG i = 0; i < i_ulSourceLength; i++ )
	{
		const CHAR cChar	= i_strSource[ i ];
		if( cChar == '\0' )
		{
			if( ulIndex > 0 )
			{
				( * pItem )[ ulIndex ] = '\0';
				this->insertItem( pItem, m_ulMaxRow, ulMaxColumn, i_bTrim );
				bLTrimed	= FALSE;
				pItem		= NULL;
				m_ulMaxRow ++;
			}
			else
			{
				IFACTORY()->DestroyItem( pItem );
			}
			ulMaxColumn ++;
			if( ulMaxColumn > m_ulMaxColumn )
			{
				m_ulMaxColumn = ulMaxColumn;
			}
			break;
		}

		if( cChar == i_cRowSplitter )
		{
			if( ulIndex > 0 )
			{
				( * pItem )[ ulIndex ] = '\0';
				this->insertItem( pItem, m_ulMaxRow, ulMaxColumn, i_bTrim );
				bLTrimed	= FALSE;
				pItem		= IFACTORY()->CreateItem();
				ulIndex		= 0;
			}
			m_ulMaxRow ++;
			ulMaxColumn ++;
			if( ulMaxColumn > m_ulMaxColumn )
			{
				m_ulMaxColumn = ulMaxColumn;
			}
			ulMaxColumn		= 0;
			continue;
		}
		else if( cChar == i_cColumnSplitter )
		{
			if( ulIndex > 0 )
			{
				( * pItem )[ ulIndex ] = '\0';
				this->insertItem( pItem, m_ulMaxRow, ulMaxColumn, i_bTrim );
				bLTrimed	= FALSE;
				pItem		= IFACTORY()->CreateItem();
				ulIndex		= 0;
			}
			ulMaxColumn ++;
			continue;
		}
		else if( i_bTrim == TRUE && bLTrimed == FALSE )
		{
			if( this->isTrimChar( cChar ) == TRUE )
			{
				continue;
			}
			else
			{
				bLTrimed	= TRUE;
			}
		}

		if( ulIndex >= 256 )
		{
			::Dispatcher::ICONSOLE()->Output( STRING( "TableError\tTooLongLength\r\n" ) );
			::Dispatcher::ICONSOLE()->Write( STRING( "TableError\tTooLongLength\r\n" ) );
			DEBUG_BREAK;
			continue;
		}
		( * pItem )[ ulIndex ] = cChar;
		ulIndex ++;
	}
}

void
Provider::Release()
{
	TREE::Iterator stItr	= m_pTree->GetRoot();
	while( stItr.GetItem() != NULL )
	{
		COUPLER * pCoupler	= stItr.GetItem();
		IFACTORY()->DestroyItem( pCoupler->clsItemB );
		m_pPool->Release( pCoupler );
		stItr.Unlink();
	}
}

CULONG
Provider::GetMaxRow()
{
	return m_ulMaxRow;
}

CULONG
Provider::GetMaxColumn()
{
	return m_ulMaxColumn;
}
	
STRING const *
Provider::GetItem( CULONG i_ulRow, CULONG i_ulColumn )
{
	CULONG ulKey			= i_ulColumn * s_nMaxColumn + i_ulRow;
	const COUPLER * pCoupler = m_pTree->Search( ulKey );
	if( pCoupler == NULL )
	{
		return NULL;
	}

	return pCoupler->clsItemB;
}

inline CBOOL
Provider::isTrimChar( const CHAR i_cChar )
{
	switch( i_cChar )
	{
	case ' ' :
		return TRUE;

	case '\t' :
		return TRUE;

	case '\r' :
		return TRUE;

	case '\n' :
		return TRUE;

	default :
		return FALSE;
	}
	return FALSE;
}

inline void
Provider::insertItem( CSTRING * i_pItem, CULONG i_ulRow, CULONG i_ulColumn, CBOOL i_bTrim )
{
	if( i_pItem == NULL )
	{
		return;
	}
	if( i_bTrim == TRUE )
	{
		i_pItem->TrimRight();
	}
	CULONG ulKey			= i_ulColumn * s_nMaxColumn + i_ulRow;

	STRING strLog;
	strLog.Printf( "TABLE %d, %d = %s\r\n", i_ulRow, i_ulColumn, i_pItem->GetCSTR() );
	::Dispatcher::ICONSOLE()->Output( strLog );

	COUPLER * pCoupler		= m_pPool->Capture();
	pCoupler->clsItemA		= ulKey;
	pCoupler->clsItemB		= i_pItem;
	CBOOL bInsert			= m_pTree->Insert( pCoupler );
	Assert( bInsert == TRUE );
}

} /* Table */				} /* GameServer */