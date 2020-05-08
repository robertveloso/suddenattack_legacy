#include "StdAfx.h"
#include "SaListText.h"
#include "../ClientShellShared/ScreenCommands.h"

CSaListText::CSaListText() 
:m_nMaxItemCount(15)
,m_pTextList(LTNULL)
,CLTGUIListView() {}

CSaListText::~CSaListText()
{
	this->Term();

	if( m_pTextList )
	{
		delete [] m_pTextList;
		m_pTextList = NULL;
	}
}

LTBOOL CSaListText::Create( uint16 nPosX, uint16 nPosY, 
						    uint16 nWidth, uint16 nHeight, uint16 nItemHeight,
							CLTGUICommandHandler* pCommandHandler,
							LTBOOL bDrawBack, uint32 nBackColor, uint32 nBorderColor )
{
	CLTGUIListView::Create( nPosX, nPosY, nWidth, nHeight, nItemHeight,
							pCommandHandler, bDrawBack, nBackColor, nBorderColor );

	m_pTextList = new CSAD3DText[m_nPageItemCount];

	for( int i=0 ; i<m_nPageItemCount ; ++i )
	{
		m_pTextList[i].CreateD3DText( m_nWidth, 16, false );
		m_pTextList[i].SetTextColor( RGB(0,0,0), RGB(255,255,0) );
		m_pTextList[i].SetTransparent(true);
		m_pTextList[i].SetPosition( nPosX, nPosY+(i*m_nItemHeight) );
		m_pTextList[i].DrawD3DText( "", 0 );
	}

	return  LTTRUE;
}

LTBOOL CSaListText::SetMaxItemCount( int nCount )
{
	if( nCount < 0 )
	{
		m_nMaxItemCount = m_nPageItemCount;
	}
	else
	{
		m_nMaxItemCount = (m_nPageItemCount>nCount) ? m_nPageItemCount : nCount;
	}

	return TRUE;
}

LTBOOL CSaListText::Append( LPCSTR lpszText,
						    unsigned long dwDrawType,
							COLORREF rgbTextColor,
							COLORREF rgbExtColor,
							unsigned long dwDeleteTick )
{
	SIZE cSize;
	HDC hDC = m_pTextList[0].GetDC();
	strcpy( m_szBuffer, lpszText );
	char* start = m_szBuffer;
	char* end	= m_szBuffer;

	if( !hDC ) return LTFALSE;

	::GetTextExtentPoint32( hDC, start, strlen(start), &cSize );
		
	if( cSize.cx <= m_nWidth-5 )
	{
		this->AddText( start, dwDrawType, rgbTextColor, rgbExtColor, dwDeleteTick );
	} 
	else
	{
		while( *end != '\0' )
		{
			end = ::CharNext(end);
			::GetTextExtentPoint32( hDC, start, end-start, &cSize );
			if( cSize.cx > m_nWidth-5 ) 
			{
				end = ::CharPrev(start,end);
				char tmpChar = *end;
				*end = '\0';
				this->AddText( start, dwDrawType, rgbTextColor, rgbExtColor, dwDeleteTick );
				*end = tmpChar;
				start = end;
			}
		}
		if( *start != '\0' )
		{
			this->AddText( start, dwDrawType, rgbTextColor, rgbExtColor, dwDeleteTick );
		}
	}

	ScrollBottom();

	return LTTRUE;
}

LTBOOL CSaListText::AddText( LPCSTR lpszText,
							 unsigned long dwDrawType,
							 COLORREF rgbTextColor,
							 COLORREF rgbExtColor,
							 unsigned long dwDeleteTick )
{
	LTBOOL bResult;
	T_TEXT_ITEMptr pItem = new T_TEXT_ITEM;
	
	pItem->dwDrawType		= dwDrawType;
	pItem->rgbTextColor	= rgbTextColor;
	pItem->rgbExtColor	= rgbExtColor;
	pItem->pszText		= new char[strlen(lpszText)+1];
	pItem->dwDeleteTick	= dwDeleteTick;
	strcpy( pItem->pszText, lpszText );

	bResult = InsertItem( pItem );
	if( !bResult ) return LTFALSE;
	
	if( m_nTotalItemCount > m_nMaxItemCount )
	{
		DeleteTop();
	}

	if( bResult == LTLV_CHANGEED )
	{
		RenderItemText();
	}
	
	return LTTRUE;
}

void CSaListText::ScrollBottom()
{
	if( !m_bEnableScroll ) return;
	
	m_nLastItemIndex = m_nTotalItemCount-1;
	m_nFirstItemIndex = m_nLastItemIndex-m_nPageItemCount+1;

	RenderItemText();
}

void CSaListText::DeleteTop()
{
	LTLV_ITEM_LIST_ITR	itr;
	LTBOOL				bResult;
	T_TEXT_ITEMptr		pTextItem;	

	if( m_ItemList.empty() ) return;

	itr = m_ItemList.begin();
	pTextItem = (T_TEXT_ITEMptr)(*itr);

	if( pTextItem )
	{
		if( pTextItem->pszText )
		{
			delete [] pTextItem->pszText;
			pTextItem->pszText = LTNULL;
		}
		
		bResult = bResult = DeleteItemPtr( pTextItem, 0 );
		if( bResult ) m_ItemList.erase(itr);			
		if( bResult == LTLV_CHANGEED ) RenderItemText();
	}	
}

void CSaListText::DeleteAllItem()
{
	T_TEXT_ITEMptr		pTextItem;
	LTLV_ITEM_LIST_ITR	itr;

	m_nTotalItemCount	= 0;
	m_nFirstItemIndex	= -1;
	m_nLastItemIndex	= -1;
	m_nSelIndex			= -1;
	m_nOverIndex		= -1;
	m_bEnableScroll		= FALSE;

	if( !m_ItemList.empty() )
	{
		for( itr=m_ItemList.begin() ; itr!=m_ItemList.end() ; ++itr )
		{
			pTextItem = (T_TEXT_ITEMptr)(*itr);
			
			if( pTextItem->pszText )
			{
				delete [] pTextItem->pszText;
				pTextItem->pszText = LTNULL;
			}

			if( pTextItem )
			{
				delete pTextItem;
				pTextItem = LTNULL;
			}		
		}

		m_ItemList.clear();
	}
}

void CSaListText::DeleteText( unsigned long dwCurrentTick )
{
	if( !m_ItemList.empty() )
	{
		LTLV_ITEM_LIST_ITR itr=m_ItemList.begin();
		T_TEXT_ITEMptr pTextItem = (T_TEXT_ITEMptr)(*itr);
		if( pTextItem->dwDeleteTick < dwCurrentTick )
		{
			this->DeleteTop();
		}
	}
}

LTBOOL CSaListText::RenderItem( void* pListItem, const LTIntPt* ptItemPos, int32 nIndex, bool bSelect, bool bOver )
{
	m_pTextList[nIndex].Render();
	return LTTRUE;
}

void CSaListText::RenderItemText()
{
	int				i, nIndex;
	T_TEXT_ITEMptr	pTextItem;
	
	if( m_nFirstItemIndex < 0 || m_nLastItemIndex < 0 ) return;
	for( i=m_nFirstItemIndex, nIndex=0 ; i<=m_nLastItemIndex ; ++i, ++nIndex )
	{
		pTextItem = (T_TEXT_ITEMptr)m_ItemList[i];
		if( pTextItem )
		{
			m_pTextList[nIndex].SetTextColor( pTextItem->rgbTextColor, pTextItem->rgbExtColor );
			m_pTextList[nIndex].DrawD3DText( pTextItem->pszText, strlen(pTextItem->pszText), DT_LEFT, pTextItem->dwDrawType );
		}
	}
}

LTBOOL CSaListText::OnWheelUp( int x, int y )
{
	LTBOOL bResult = CLTGUIListView::OnWheelUp( x, y );
	m_pCommandHandler->SendCommand( CMD_TEXT_WHELL, LTNULL, LTNULL );
	return bResult;
}

LTBOOL CSaListText::OnWheelDown( int x, int y )
{
	LTBOOL bResult = CLTGUIListView::OnWheelDown( x, y );
	m_pCommandHandler->SendCommand( CMD_TEXT_WHELL, LTNULL, LTNULL );
	return bResult;
}