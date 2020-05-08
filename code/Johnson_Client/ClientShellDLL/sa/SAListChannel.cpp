#include "StdAfx.h"
#include "SaListChannel.h"
#include "ScreenCommands.h"
#include "InterfaceResMgr.h"

#include "GameText.h"
#include "GameMessage.h"

struct LessChannelIndex
{
	bool operator()( const PVOID p, const PVOID q ) const
	{
		return ((T_CHANNEL_ITEMptr)p)->nIndex < ((T_CHANNEL_ITEMptr)q)->nIndex;
	}
};

CSaListChannel::CSaListChannel() 
: CLTGUIListView() {}

CSaListChannel::~CSaListChannel()
{
	this->Term();
}

LTBOOL CSaListChannel::Create( uint16 nPosX, uint16 nPosY, 
						    uint16 nWidth, uint16 nHeight, uint16 nItemHeight,
							CLTGUICommandHandler* pCommandHandler,
							LTBOOL bDrawBack, uint32 nBackColor, uint32 nBorderColor )
{
	CLTGUIListView::Create( nPosX, nPosY, nWidth, nHeight, nItemHeight,
							pCommandHandler, bDrawBack, nBackColor, nBorderColor );

	m_hSelected	= g_pInterfaceResMgr->GetTexture( "SA_Interface\\Menu\\Textures\\Common\\BtnChecked.dtx" );
	g_pDrawPrim->SetUVWH( &m_PolyChecked, 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_PolyChecked, 0xFFFFFFFF );
	
	g_pDrawPrim->SetUVWH( &m_PolyBar, 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_PolyBar, 0x802C394D );
	
	return  LTTRUE;
}

LTBOOL CSaListChannel::AddChannel( T_CHANNEL_ITEMptr pChannelItem )
{
	LTBOOL bResult;

	bResult = InsertItem( (void *)pChannelItem );
	if( !bResult )
	{
		return LTFALSE;
	}
	return LTTRUE;
}

T_CHANNEL_ITEMptr CSaListChannel::GetSelectedChannelData()
{
	if( m_nSelIndex < 0 ) return LTFALSE;
	return (T_CHANNEL_ITEMptr)m_ItemList[m_nSelIndex];
}

LTBOOL CSaListChannel::RenderItem( void* pListItem, const LTIntPt* ptItemPos,
								int32 nIndex, bool bSelect, bool bOver )
{
	if( !m_bRecvComplete ) return LTTRUE;

	T_CHANNEL_ITEMptr pChannelItem = (T_CHANNEL_ITEMptr)pListItem;
	
	if( !pChannelItem ) return LTFALSE;

	if( bOver )
	{
		g_pDrawPrim->SetRGBA( &m_PolyBar, 0x702B384C );
		g_pDrawPrim->SetXYWH( &m_PolyBar, (float)ptItemPos->x, (float)ptItemPos->y, m_nWidth, m_nItemHeight );
		g_pDrawPrim->SetTexture( LTNULL );
		g_pDrawPrim->DrawPrim( &m_PolyBar ); 
	}

	if( bSelect )
	{
		//¶Ë±×¶ó¹Ì
		g_pDrawPrim->SetXYWH( &m_PolyChecked, (float)ptItemPos->x+20.0f, (float)ptItemPos->y+5.0f,
							  16.0f, 16.0f );
		g_pDrawPrim->SetTexture( m_hSelected );
		g_pDrawPrim->DrawPrim( &m_PolyChecked );
	}

	if( pChannelItem->pTextIndex ) pChannelItem->pTextIndex->Render();
	if( pChannelItem->pTextName ) pChannelItem->pTextName->Render();
	if( pChannelItem->pTextUsers ) pChannelItem->pTextUsers->Render();
	
	return LTTRUE;
}

void CSaListChannel::RenderItemText()
{
}

LTBOOL CSaListChannel::OnLButtonDblClick( int x, int y )
{
	if( m_nSelIndex < 0 ) return LTFALSE;	//Not Handled.
	
	if( this->PointInMe( x, y ) )
	{
		int nPreSel = m_nSelIndex;
		m_nSelIndex = m_nFirstItemIndex + ((y-m_pos.y)/m_nItemHeight);
		if( m_nSelIndex > m_nLastItemIndex )
		{
			m_nSelIndex = nPreSel;
			return LTTRUE;
		}
		m_pCommandHandler->SendCommand( CMD_JOIN_CHANNEL, LTNULL, LTNULL );

		return LTTRUE;
	}
	
	return LTFALSE;
}

void CSaListChannel::SortNDrawText()
{
	std::sort( m_ItemList.begin(), m_ItemList.end(), LessChannelIndex() );

	T_CHANNEL_ITEMptr pChannelItem;
	char	szTemp[128];
	COLORREF	rgbText;

	int	nPosX;
	int nPosY	= GetPos().y + 7;
	
	if( !m_ItemList.empty() )
	{
		for( LTLV_ITEM_LIST_ITR itr=m_ItemList.begin() ; itr!=m_ItemList.end() ; ++itr )
		{
			pChannelItem = (T_CHANNEL_ITEMptr)(*itr);
			if( pChannelItem )
			{
				nPosX = GetPos().x + 25;
				sprintf( szTemp, "%d", pChannelItem->nIndex+1 );
				pChannelItem->pTextIndex	= new CSAD3DText();
				pChannelItem->pTextIndex->CreateD3DText( 32, 16, true );
				pChannelItem->pTextIndex->SetPosition( nPosX, nPosY );
				pChannelItem->pTextIndex->SetTransparent(true);
				pChannelItem->pTextIndex->SetTextColor( RGB(255,255,255) );
				pChannelItem->pTextIndex->DrawD3DText( szTemp, strlen(szTemp), DT_RIGHT, D3D_TEXT_SHADOW );

				nPosX += 80;
				sprintf( szTemp, "%s", pChannelItem->szName );
				pChannelItem->pTextName	= new CSAD3DText();
				pChannelItem->pTextName->CreateD3DText( 256, 16, true );
				pChannelItem->pTextName->SetPosition( nPosX, nPosY );
				pChannelItem->pTextName->SetTextColor( RGB(255,255,255) );
				pChannelItem->pTextName->SetTransparent(true);				
				pChannelItem->pTextName->DrawD3DText( szTemp, strlen(szTemp), DT_LEFT, D3D_TEXT_SHADOW );

				nPosX += 342;
				
				sprintf( szTemp, "%d / %d", pChannelItem->nCurrentUsers, pChannelItem->nMaxUsers );

				if( pChannelItem->nCurrentUsers == pChannelItem->nMaxUsers )
				{
					rgbText = RGB(212,183,130);
				}
				else
				{
					rgbText = RGB(255,255,255);
				}

				pChannelItem->pTextUsers	= new CSAD3DText();
				pChannelItem->pTextUsers->CreateD3DText( 64, 16, true );
				pChannelItem->pTextUsers->SetPosition( nPosX, nPosY );
				pChannelItem->pTextUsers->SetTransparent(true);
				pChannelItem->pTextUsers->SetTextColor( rgbText );
				pChannelItem->pTextUsers->DrawD3DText( szTemp, strlen(szTemp), DT_CENTER, D3D_TEXT_SHADOW );

				nPosY += m_nItemHeight;
			}			
		}		
	}

	m_nSelIndex = 0;
	m_bRecvComplete	= LTTRUE;
}

LTBOOL CSaListChannel::OnLButtonDown( int x, int y )
{
	int	nPreSel = m_nSelIndex;
	LTBOOL bResult = CLTGUIListView::OnLButtonDown( x, y );

	if( m_nSelIndex < 0 ) m_nSelIndex = nPreSel;

	if( nPreSel != m_nSelIndex )
	{
		g_pInterfaceMgr->RequestInterfaceSound( (InterfaceSound)IS_CLICK_RADIO );
		g_GameDoc.SetChannelIndex( m_nSelIndex );
	}
	return bResult;
}

LTBOOL CSaListChannel::OnRButtonDown( int x, int y )
{
	int	nPreSel = m_nSelIndex;
	LTBOOL bResult = CLTGUIListView::OnLButtonDown( x, y );

	if( m_nSelIndex < 0 ) m_nSelIndex = nPreSel;
	return bResult;
}

void CSaListChannel::OnOverChange( int nPreIndex, int nNewIndex )
{
	g_pInterfaceMgr->RequestInterfaceSound( (InterfaceSound)IS_OVER );
}

void CSaListChannel::DeleteAllItem()
{
	if( !m_ItemList.empty() )
	{
		T_CHANNEL_ITEMptr pChannelItem;
		for( LTLV_ITEM_LIST_ITR itr=m_ItemList.begin() ; itr!=m_ItemList.end() ; ++itr )
		{
			pChannelItem = (T_CHANNEL_ITEMptr)(*itr);
			if( pChannelItem )
			{
				SAFE_DELETE( pChannelItem->pTextIndex );
				SAFE_DELETE( pChannelItem->pTextName );
				SAFE_DELETE( pChannelItem->pTextUsers );
			}			
		}		
	}
	
	m_bRecvComplete = LTFALSE;
	CLTGUIListView::DeleteAllItem();
}