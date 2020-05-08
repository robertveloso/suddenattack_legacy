#include "stdafx.h"
#include "SAMapList.h"

#define SA_MAPLIST_NAME_RGB		RGB( 255, 255, 255 )
#define SA_MAPLIST_TYPE_RGB		RGB( 180, 180, 180 )
#define SA_MAPLIST_TYPE_WIDTH	70

CSAMapList::CSAMapList()
{
	m_pMapNameText	= LTNULL;
	m_pMapTypeText	= LTNULL;
	m_bSelected		= LTFALSE;
	m_bScrollEnable	= LTFALSE;
	m_nCommand1		= 0;
	m_nPreIndex		= -1;
}

CSAMapList::~CSAMapList()
{
	SAFE_DELETE_ARRAY( m_pMapNameText );
	SAFE_DELETE_ARRAY( m_pMapTypeText );
}


LTBOOL CSAMapList::Create( LTRect rt, uint16 nItemHeight,
						   uint16 nShowCount, int nCommand1,
						   CLTGUICommandHandler* pCommand,
						   LTBOOL	bDrawBack,
						   uint32	nBackColor,
						   uint32	nBorderColor )
{
	m_nCommand1		= nCommand1;	
	
	return Create(	rt.left, rt.top, rt.right - rt.left, rt.bottom - rt.top,
					nItemHeight, pCommand, bDrawBack, nBackColor, nBorderColor );
}

LTBOOL CSAMapList::Create( uint16 nPosX, uint16 nPosY,
						   uint16 nWidth, uint16 nHeight,
						   uint16 nItemHeight,
						   CLTGUICommandHandler* pCommandHandler, uint32 nCommandID,
						   LTBOOL bDrawBack, uint32 nBackColor, uint32 nBorderColor )
{
	CLTGUIListView::Create( nPosX, nPosY, nWidth, nHeight, nItemHeight,
							pCommandHandler, bDrawBack, nBackColor, nBorderColor );

	m_nCommand1	= nCommandID;
	m_pMapNameText = new CSAD3DText[m_nPageItemCount];
	m_pMapTypeText = new CSAD3DText[m_nPageItemCount];

	for ( int nCnt = 0; nCnt < m_nPageItemCount; nCnt++ )
	{
		m_pMapNameText[nCnt].CreateD3DText( nWidth - SA_MAPLIST_TYPE_WIDTH, m_nItemHeight, false );
		m_pMapNameText[nCnt].SetTextColor( SA_MAPLIST_NAME_RGB );
		m_pMapNameText[nCnt].SetTransparent( true );
		m_pMapNameText[nCnt].SetPosition( nPosX+1, nPosY + (m_nItemHeight * nCnt) + 3 );
		m_pMapNameText[nCnt].DrawD3DText( "", 0, DT_CENTER );

		m_pMapTypeText[nCnt].CreateD3DText( SA_MAPLIST_TYPE_WIDTH, m_nItemHeight, false );
		m_pMapTypeText[nCnt].SetTextColor( SA_MAPLIST_TYPE_RGB );
		m_pMapTypeText[nCnt].SetTransparent( true );
		m_pMapTypeText[nCnt].SetPosition( nPosX+1 + (nWidth - SA_MAPLIST_TYPE_WIDTH), nPosY + (m_nItemHeight * nCnt) + 3 );
		m_pMapTypeText[nCnt].DrawD3DText( "", 0, DT_CENTER );
	}

	return LTTRUE;
}

LTBOOL CSAMapList::Insert( LPMapData lpMapData )
{
	LTBOOL bResult = LTFALSE;

	bResult = InsertItem( (void*)lpMapData );

	if ( LTFALSE == bResult )				return LTFALSE;

	//[MURSUM] if ( LTLV_CHANGEED == bResult )			RenderItemText();

	return LTTRUE;
}

LTBOOL CSAMapList::Remove( uint32 dwMapIndex )
{
	LTBOOL		bResult		= LTFALSE;
	uint32		dwIdx		= 0;
	LPMapData	lpMapData	= LTNULL;	
	LTLV_ITEM_LIST_ITR	iter;
	
	for ( iter = m_ItemList.begin(), dwIdx = 0; iter != m_ItemList.end(); iter++, dwIdx++ )
	{
		lpMapData = (LPMapData)(*iter);

		if ( dwMapIndex == lpMapData->dwMapIndex )
		{
			bResult = DeleteItemPtr( lpMapData, dwMapIndex );
			if ( bResult )		m_ItemList.erase( iter );
			break;
		}
	}

	//[MURSUM] if ( LTLV_CHANGEED == bResult )			RenderItemText();

	return LTTRUE;
}

LPMapData CSAMapList::Get( uint32 dwMapIndex )
{
	LPMapData lpMapData = LTNULL;	
	LTLV_ITEM_LIST_ITR	iter;
	
	for ( iter = m_ItemList.begin(); iter != m_ItemList.end(); iter++ )
	{
		lpMapData = (LPMapData)(*iter);
		if ( dwMapIndex == lpMapData->dwMapIndex)
		{
			return lpMapData;
		}
	}

	return LTNULL;
}

LPMapData CSAMapList::Get( void )
{
	if ( m_nSelIndex < 0 )		return FALSE;
	
	return (LPMapData)m_ItemList[m_nSelIndex];
}

LTBOOL CSAMapList::SetItemText( uint32 dwPos, char* szMapName, char* szMapType )
{
	if( dwPos >= m_nPageItemCount || dwPos < 0 || !szMapName || !szMapType ) return LTFALSE;
	
	m_pMapNameText[dwPos].DrawD3DText( szMapName, strlen(szMapName), DT_RIGHT, D3D_TEXT_NORMAL, true);
	m_pMapTypeText[dwPos].DrawD3DText( szMapType, strlen(szMapType), DT_CENTER, D3D_TEXT_NORMAL, true);
//	m_pMapNameText[dwPos].SetPosition( m_pos.x , m_pos.y + dwPos * m_nItemHeight );
	
	return LTTRUE;
}

LTBOOL CSAMapList::OnLButtonDown( int x, int y )
{
	if( LTFALSE == CLTGUIListView::OnLButtonDown( x, y ) ) return LTFALSE;

	if( m_nPreIndex != m_nSelIndex )
	{
		m_nPreIndex = m_nSelIndex;
		m_pCommandHandler->SendCommand( m_nCommand1, 0, 0 );
	}
	
	Show(LTFALSE);
	
	return LTTRUE;
}

void CSAMapList::RenderItemText()
{
	if ( m_nFirstItemIndex < 0 ||
		 m_nLastItemIndex < 0 )				return;

	for ( int nCnt = m_nFirstItemIndex; nCnt <= m_nLastItemIndex; nCnt++ )
	{
		LPMapData lpMapData = (LPMapData)m_ItemList[nCnt];

		LTBOOL bResult = SetItemText( nCnt, lpMapData->szMapName, lpMapData->szMapType );
	}
}

LTBOOL CSAMapList::RenderItem( void* pListItem, const LTIntPt* ptfPosition,
							  int32 nIndex, bool bSelect, bool bOver )
{
	const char* lpszItem = (const char*)pListItem;
	
	if( bSelect )
	{
		g_pDrawPrim->SetRGBA( &m_PolyBar, LTLV_DEFAULT_SEL_COLOR );
		g_pDrawPrim->SetXYWH( &m_PolyBar, (float)ptfPosition->x, (float)ptfPosition->y, (float)m_nWidth, (float)m_nItemHeight );
		g_pDrawPrim->SetTexture( LTNULL );
		g_pDrawPrim->DrawPrim(&m_PolyBar);
	}
	else if( bOver )
	{
		g_pDrawPrim->SetRGBA( &m_PolyBar, LTLV_DEFAULT_OVER_COLOR );
		g_pDrawPrim->SetXYWH( &m_PolyBar, (float)ptfPosition->x, (float)ptfPosition->y, (float)m_nWidth, (float)m_nItemHeight );
		g_pDrawPrim->SetTexture( LTNULL );
		g_pDrawPrim->DrawPrim(&m_PolyBar);
	}
		
	m_pMapNameText[nIndex].Render();
	m_pMapTypeText[nIndex].Render();
	
	return LTTRUE;
}

void CSAMapList::SetPreIndex( int nIndex )
{
	m_nPreIndex	= nIndex;
}