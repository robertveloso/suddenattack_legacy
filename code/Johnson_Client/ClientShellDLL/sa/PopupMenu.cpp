#include "stdafx.h"
#include "Popupmenu.h"

CPopupMenu::CPopupMenu()
{
	m_pText	= LTNULL;
}

CPopupMenu::~CPopupMenu()
{
	SAFE_DELETE_ARRAY( m_pText );
}

LTBOOL CPopupMenu::Create( uint16 nPosX, uint16 nPosY,
						   uint16 nWidth, uint16 nHeight,
						   uint16 nItemHeight,
						   CLTGUICommandHandler* pCommandHandler, uint32 nCommandID,
						   LTBOOL bDrawBack, uint32 nBackColor, uint32 nBorderColor )
{
	CLTGUIListView::Create( nPosX, nPosY, nWidth, nHeight, nItemHeight,
							pCommandHandler, bDrawBack, nBackColor, nBorderColor );

	m_pText = new CSAD3DText[m_nPageItemCount];

	for ( int i = 0; i < m_nPageItemCount; ++i )
	{
		m_pText[i].CreateD3DText( nWidth, m_nItemHeight, false );
		m_pText[i].SetTransparent( true );
		m_pText[i].DrawD3DText( "", 0 );
	}

	g_pDrawPrim->SetRGBA( &m_PolyBar, 0xFF000000 );
		
	HidePopupMenu();

	return LTTRUE;
}

void CPopupMenu::AddButton( const char* lpszText, uint32 nCommand )
{
	m_pText[m_nTotalItemCount].DrawD3DText( lpszText, strlen(lpszText) );

	uint32* pCommand = new uint32;
	(*pCommand) = nCommand;
	InsertItem( (void *)pCommand );
}

void CPopupMenu::ShowPopupMenu( LTIntPt pt, int nSaveData1, int nSaveData2 )
{
	if( pt.x+m_nWidth > (int)g_pInterfaceResMgr->GetScreenWidth() )
	{
		pt.x = g_pInterfaceResMgr->GetScreenWidth()-m_nWidth;
	}
	else if( pt.x < 0 )
	{
		pt.x = 0;
	}

	if( pt.y+m_nHeight > (int)g_pInterfaceResMgr->GetScreenHeight() )
	{
		pt.y = g_pInterfaceResMgr->GetScreenHeight()-m_nHeight;
	}
	else if( pt.y < 0 )
	{
		pt.y = 0;
	}

	m_nSaveData1 = nSaveData1;
	m_nSaveData2 = nSaveData2;

	SetBasePos( pt );
	for( int i=0 ; i<m_nPageItemCount ; ++i )
	{
		m_pText[i].SetPosition( pt.x+8, pt.y+(i*m_nItemHeight)+4 );
	}
	Show(LTTRUE);
}

void CPopupMenu::HidePopupMenu()
{
	Show(LTFALSE);
}

LTBOOL CPopupMenu::OnLButtonDown( int x, int y )
{
	if( LTFALSE == CLTGUIListView::OnLButtonDown( x, y ) ) return LTFALSE;

	if( m_nSelIndex>=0 && m_nSelIndex<m_nTotalItemCount )
	{
		uint32* pCommand = (uint32*)m_ItemList[m_nSelIndex];
		if( pCommand )
		{
			m_pCommandHandler->SendCommand( (*pCommand), m_nSaveData1, m_nSaveData2 );
		}
	}

	HidePopupMenu();
	return LTTRUE;
}

LTBOOL CPopupMenu::RenderItem( void* pListItem, const LTIntPt* ptfPosition,
							  int32 nIndex, bool bSelect, bool bOver )
{
	if( bOver )
	{
		g_pDrawPrim->SetXYWH( &m_PolyBar, (float)ptfPosition->x, (float)ptfPosition->y,
							  (float)m_nWidth, (float)m_nItemHeight );
		g_pDrawPrim->SetTexture(LTNULL);
		g_pDrawPrim->DrawPrim(&m_PolyBar);
		m_pText[nIndex].SetTextColor( RGB(250,204,133) );
	}
	else
	{
		m_pText[nIndex].SetTextColor( RGB(183,153,104) );
	}
	
	m_pText[nIndex].Render();
	return LTTRUE;
}
