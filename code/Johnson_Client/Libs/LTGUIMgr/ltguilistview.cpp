#include "ltguimgr.h"

CLTGUIListView::CLTGUIListView()
{
	m_nWidth			= 0;
	m_nHeight			= 0;
	m_nItemHeight		= 0;
	m_nPageItemCount	= 0;
	m_nItemLeft			= 0;
	m_nItemRight		= 0;
	m_nTotalItemCount	= 0;
	m_nFirstItemIndex	= -1;
	m_nLastItemIndex	= -1;
	m_nOverIndex		= -1;
	m_nSelIndex			= -1;
	m_bEnableScroll		= LTFALSE;
	m_pBtnScrollUp		= LTNULL;
	m_pBtnScrollDown	= LTNULL;
	m_pBtnScroll		= LTNULL;	
	m_bDrawBack			= LTFALSE;
}

CLTGUIListView::~CLTGUIListView()
{
	this->Term();
}

LTBOOL CLTGUIListView::Create( uint16 nPosX, uint16 nPosY,
							   uint16 nWidth, uint16 nHeight,
							   uint16 nItemHeight,
							   CLTGUICommandHandler* pCommandHandler,
							   LTBOOL bDrawBack,
							   uint32 nBackColor,
							   uint32 nBorderColor )
{
	m_pos.x				= nPosX;
	m_pos.y				= nPosY;
	
	m_nWidth			= nWidth;
	m_nHeight			= nHeight;
	m_nItemHeight		= nItemHeight;

	m_nPageItemCount	= m_nHeight/m_nItemHeight;
	m_nItemLeft			= m_pos.x;
	m_nItemRight		= m_pos.x + m_nWidth;
	m_ptItemPos.x		= m_pos.x;

	m_pCommandHandler	= pCommandHandler;

	if( bDrawBack )
	{
		m_bDrawBack = bDrawBack;
		g_pDrawPrim->SetRGBA( &m_PolyBack, nBackColor );
		g_pDrawPrim->SetRGBA( &m_PolyBorder, nBorderColor );
		g_pDrawPrim->SetXYWH( &m_PolyBack, (float)m_pos.x, (float)m_pos.y, (float)m_nWidth, (float)m_nHeight );
		g_pDrawPrim->SetXYWH( &m_PolyBorder, (float)m_pos.x-1.0f, (float)m_pos.y-1.0f,
											(float)m_nWidth+2.0f, (float)m_nHeight+2.0f );
	}

	CLTGUICtrl::Create(LTNULL, LTNULL, LTNULL, LTNULL);
	DeleteAllItem();

	this->Enable(true);
	
	return LTTRUE;
}

void CLTGUIListView::Term()
{
	this->Enable(false);
	this->DeleteAllItem();
}

LTBOOL CLTGUIListView::InsertItem( void* pItem )
{
	if( !pItem ) return LTFALSE;

	LTBOOL bResult = LTTRUE;
	
	++m_nTotalItemCount;
	if( m_nFirstItemIndex < 0 )
	{
		bResult = LTLV_CHANGEED;
		m_nFirstItemIndex	= 0;
	}
	
	if( m_nLastItemIndex-m_nFirstItemIndex < (m_nPageItemCount-1) )
	{
		bResult = LTLV_CHANGEED;
		++m_nLastItemIndex;
	}

	if( m_nTotalItemCount > m_nPageItemCount )
	{
		m_bEnableScroll = LTTRUE;
	}
	
	m_ItemList.push_back(pItem);

	return bResult;
}

LTBOOL CLTGUIListView::DeleteItemPtr( void* pItem, int nIndex )
{
	if( !pItem || m_nTotalItemCount <= 0 ) return FALSE;

	int		nPreOverIndex = m_nOverIndex;
	BOOL	bResult = LTTRUE;

	--m_nTotalItemCount;
		
	if( LTFALSE == m_bEnableScroll )
	{
		bResult = LTLV_CHANGEED;
		if( m_nTotalItemCount <= 0 )  //하나남은 아이템을 지우는 경우
		{
			m_nFirstItemIndex	= -1;
			m_nLastItemIndex	= -1;
			m_nSelIndex			= -1;
			m_nOverIndex		= -1;
		}
		else
		{
			--m_nLastItemIndex;
		}
	}
	else
	{
		if( m_nFirstItemIndex > nIndex )
		{
			bResult = LTLV_CHANGEED;
			--m_nFirstItemIndex;
			--m_nLastItemIndex;		
		}		
		else if( m_nLastItemIndex >= nIndex )
		{
			bResult = LTLV_CHANGEED;
			if( m_nLastItemIndex >= (m_nTotalItemCount-1) )
			{
				--m_nFirstItemIndex;
				--m_nLastItemIndex;
				if( m_nFirstItemIndex < 0 )
				{
					++m_nFirstItemIndex;
					++m_nLastItemIndex;
				}
			}
		}		
	}

	if( m_nTotalItemCount <= m_nPageItemCount )
	{
		m_bEnableScroll = FALSE;
	}

	if( m_nSelIndex == nIndex ) m_nSelIndex = -1;
	if( m_nOverIndex == nIndex ) m_nOverIndex = -1;

	if( pItem )
	{
		delete pItem;
		pItem = LTNULL;
	}
	
	if( nPreOverIndex != m_nOverIndex )
	{
		OnOverChange( nPreOverIndex, m_nOverIndex );
	}
	return bResult;
}

void CLTGUIListView::DeleteAllItem()
{
	m_nTotalItemCount	= 0;
	m_nFirstItemIndex	= -1;
	m_nLastItemIndex	= -1;
	m_nSelIndex			= -1;
	m_nOverIndex		= -1;
	m_bEnableScroll		= FALSE;

	if( !m_ItemList.empty() )
	{
		for( LTLV_ITEM_LIST_ITR itr=m_ItemList.begin() ; itr!=m_ItemList.end() ; ++itr )
		{
			if( (*itr) )
			{
				delete (*itr);
				(*itr) = LTNULL;
			}			
		}

		m_ItemList.clear();
	}
}

LTBOOL CLTGUIListView::ScrollUp(int8 nAcceleration)
{
	if( !m_bEnableScroll || m_nFirstItemIndex <= 0 ) return FALSE;
	
	m_nFirstItemIndex -= 1 + nAcceleration;
	m_nLastItemIndex  -= 1 + nAcceleration;
//	if(m_nFirstItemIndex < 0) m_nFirstItemIndex = 0;   First와 Last관계를 잘 생각해서 해라. 

	RenderItemText();

	return LTTRUE;
}

LTBOOL CLTGUIListView::ScrollDown(int8 nAcceleration)
{
	if( !m_bEnableScroll || (m_nLastItemIndex+1) >= m_nTotalItemCount ) return FALSE;
	
	m_nFirstItemIndex += 1 + nAcceleration;
	m_nLastItemIndex  += 1 + nAcceleration;
//	if(m_nLastItemIndex > m_nTotalItemCount) m_nLastItemIndex = m_nTotalItemCount;
		
	RenderItemText();

	return LTTRUE;
}

LTBOOL CLTGUIListView::PageUp()
{
	if( !m_bEnableScroll || m_nFirstItemIndex <= 0 ) return FALSE;

	m_nFirstItemIndex	-= m_nPageItemCount;
	if( m_nFirstItemIndex < 0 ) m_nFirstItemIndex = 0;
	m_nLastItemIndex	= m_nFirstItemIndex+m_nPageItemCount-1;

	RenderItemText();

	return LTTRUE;
}

LTBOOL CLTGUIListView::PageDown()
{
	if( !m_bEnableScroll || (m_nLastItemIndex+1) >= m_nTotalItemCount ) return FALSE;

	m_nLastItemIndex	+= m_nPageItemCount;
	if( (m_nLastItemIndex+1) >= m_nTotalItemCount ) m_nLastItemIndex = m_nTotalItemCount-1;
	m_nFirstItemIndex	= m_nLastItemIndex-m_nPageItemCount+1;
	
	RenderItemText();
	return LTTRUE;
}

LTBOOL CLTGUIListView::ScrollFromRatio( float fRatio )
{
	if( !m_bEnableScroll ) return LTFALSE;

	int nPreItemIndex = m_nFirstItemIndex;

	m_nFirstItemIndex = int( (m_nTotalItemCount-m_nPageItemCount)*fRatio);

	if( m_nFirstItemIndex>m_nTotalItemCount-m_nPageItemCount )
	{
		m_nFirstItemIndex = m_nTotalItemCount-m_nPageItemCount;
	}
	m_nLastItemIndex = m_nFirstItemIndex+m_nPageItemCount-1;

	if( m_nFirstItemIndex != nPreItemIndex )
	{		
		RenderItemText();
		return LTTRUE;
	}

	return LTFALSE;
}

float CLTGUIListView::GetRatio()
{
	return ((float)m_nFirstItemIndex / (float)(m_nTotalItemCount-m_nPageItemCount));
}

LTBOOL CLTGUIListView::OnMouseMove(int x, int y)
{
	int nNewOverIndex = -2000;

	if( this->PointInMe( x, y ) )
	{
		nNewOverIndex = m_nFirstItemIndex + ((y-m_pos.y)/m_nItemHeight);
		if( nNewOverIndex > m_nLastItemIndex ) nNewOverIndex = -1;
		
		if( nNewOverIndex != m_nOverIndex )
		{
			OnOverChange( m_nOverIndex, nNewOverIndex );
			m_nOverIndex	= nNewOverIndex;
		}
		
		return LTTRUE;
	}
	
	//Not handled.
	m_nOverIndex = -1;	
	return LTFALSE;
}

LTBOOL CLTGUIListView::OnLButtonDown(int x, int y)
{
	if( this->PointInMe( x, y ) )
	{
		m_nSelIndex = m_nFirstItemIndex + ((y-m_pos.y)/m_nItemHeight);
		if( m_nSelIndex > m_nLastItemIndex ) m_nSelIndex = -1;
		return LTTRUE;
	}
	
	//Not handled.
	return LTFALSE;
}

LTBOOL CLTGUIListView::OnRButtonDown(int x, int y)
{
	if( this->PointInMe( x, y ) )
	{
		m_nSelIndex = m_nFirstItemIndex + ((y-m_pos.y)/m_nItemHeight);
		if( m_nSelIndex > m_nLastItemIndex ) m_nSelIndex = -1;
		return LTTRUE;
	}
	
	//Not handled.
	return LTFALSE;
}

LTBOOL		
CLTGUIListView::OnWheelUp(int x, int y)
{
	return ScrollUp(0);
}
LTBOOL		
CLTGUIListView::OnWheelDown(int x, int y)
{	
	return ScrollDown(0);
}

void CLTGUIListView::OnSelChange()
{
	if( m_bSelected == LTFALSE )
	{
		if( m_nOverIndex >= 0 ) OnOverChange( m_nOverIndex, -1 );
		m_nOverIndex = -1;
	}
}

LTBOOL CLTGUIListView::RenderItem( void* pListItem, 
								   const LTIntPt* ptfPosition,
								   int32 nIndex,
								   bool bSelect,
								   bool bOver )
{
	return LTTRUE;
}

void CLTGUIListView::RenderItemText()
{
}

void CLTGUIListView::Render()
{
	if (!IsVisible() || m_nLastItemIndex<0) return;

	int32 i, idx;
	
	if( m_bDrawBack )
	{
		g_pDrawPrim->SetTexture(LTNULL);
		g_pDrawPrim->DrawPrim( &m_PolyBorder );
		g_pDrawPrim->DrawPrim( &m_PolyBack );
	}

	LTLV_ITEM_LIST_ITR itr = m_ItemList.begin();

	m_ptItemPos.y = m_pos.y;
	
	for( i=m_nFirstItemIndex, idx=0 ; i<=m_nLastItemIndex ; ++i, ++idx )
	{
		if( itr+i != m_ItemList.end() )
		{
			RenderItem( *(itr+i), &m_ptItemPos, idx, !!(m_nSelIndex==i), !!(m_nOverIndex==i) );
		}

		m_ptItemPos.y += m_nItemHeight;
	}
}

LTBOOL CLTGUIListView::HandleKeyDown(int key, int rep)
{
	if (CLTGUICtrl::HandleKeyDown(key, rep))
	{
        return LTTRUE;
	}
	/*
    LTBOOL handled = LTFALSE;
	CLTGUICtrl* pCtrl = GetSelectedControl();
	if (pCtrl)
		handled = pCtrl->HandleKeyDown(key,rep);
	return handled;
	*/
	return LTFALSE;
}

LTBOOL CLTGUIListView::PointInMe( int x, int y )
{
	if( m_pos.x<=x && x<=(m_pos.x+m_nWidth) && m_pos.y<=y && y<=(m_pos.y+m_nHeight) )return LTTRUE;
	return LTFALSE;
}

LTBOOL CLTGUIListView::IsVisibleItem( int32 nIndex )
{
	if( m_nTotalItemCount <=0 ) return LTFALSE;

	if( nIndex <= GetLastIndex() && nIndex >= GetFirstIndex() ) return LTTRUE;

	return LTFALSE;
}

void CLTGUIListView::SetBasePos( LTIntPt pos )
{
	m_ptItemPos.x = m_pos.x = pos.x;
	m_pos.y = pos.y;

	if( m_bDrawBack )
	{
		g_pDrawPrim->SetXYWH( &m_PolyBack, (float)m_pos.x, (float)m_pos.y, (float)m_nWidth, (float)m_nHeight );
		g_pDrawPrim->SetXYWH( &m_PolyBorder, (float)m_pos.x-1.0f, (float)m_pos.y-1.0f,
											(float)m_nWidth+2.0f, (float)m_nHeight+2.0f );
	}

	CLTGUICtrl::SetBasePos( pos );
}

void CLTGUIListView::SetBackPolySize( int nLines )
{
	if( nLines < 0 ) nLines = m_nTotalItemCount;
	
	g_pDrawPrim->SetXYWH( &m_PolyBack, (float)m_pos.x, (float)m_pos.y, (float)m_nWidth, float(nLines*m_nItemHeight) );
	g_pDrawPrim->SetXYWH( &m_PolyBorder, (float)m_pos.x-1.0f, (float)m_pos.y-1.0f,
										(float)m_nWidth+2.0f, float(nLines*m_nItemHeight) + 2.0f );
}