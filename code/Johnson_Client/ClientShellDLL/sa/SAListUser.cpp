#include "StdAfx.h"
#include "SaListUser.h"
#include "InterfaceResMgr.h"
#include "../ClientShellShared/ScreenCommands.h"

#define LIST_USER_TEXTURE_SIZE		32.0f

#define USER_GRADE_TEXTURE_SIZE		32.0f

CSaListUser::CSaListUser() 
:m_pNickList(LTNULL)
,CLTGUIListView() {}

CSaListUser::~CSaListUser()
{
	this->Term();
	SAFE_DELETE_ARRAY( m_pNickList );
}

LTBOOL CSaListUser::Create( uint16 nPosX, uint16 nPosY, 
						    uint16 nWidth, uint16 nHeight, uint16 nItemHeight,
							CLTGUICommandHandler* pCommandHandler,
							LTBOOL bDrawBack, uint32 nBackColor, uint32 nBorderColor )
{
	CLTGUIListView::Create( nPosX, nPosY, nWidth, nHeight, nItemHeight,
							pCommandHandler, bDrawBack, nBackColor, nBorderColor );

	int i;
	m_pNickList = new CSAD3DText[m_nPageItemCount];

	for( i=0 ; i<m_nPageItemCount ; ++i )
	{
		m_pNickList[i].CreateD3DText( 128, 16, false );
		m_pNickList[i].SetTextColor( RGB(216,185,122), RGB(0,0,0) );
		m_pNickList[i].SetTransparent(true);
		m_pNickList[i].SetPosition( nPosX+30, nPosY+(i*m_nItemHeight)+5 );
		m_pNickList[i].DrawD3DText( "", 0 );
	}

	g_pDrawPrim->SetUVWH( &m_PolyGrade, 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_PolyGrade, 0xFFFFFFFF );

	return  LTTRUE;
}

LTBOOL CSaListUser::AddUser( T_USER_ITEMptr pUserItem )
{
	LTBOOL bResult;
	bResult = InsertItem( (void *)pUserItem );
	if( !bResult )
	{
		return LTFALSE;
	}
	else if( bResult == LTLV_CHANGEED )
	{
		RenderItemText();
	}

	return LTTRUE;
}

LTBOOL CSaListUser::RemoveUser( int nIndex )
{
	int					nListIndex;
	LTLV_ITEM_LIST_ITR	itr;
	BOOL				bResult;
	T_USER_ITEMptr		pUserItem;	

	for( itr=m_ItemList.begin(), nListIndex=0 ; itr!=m_ItemList.end() ; ++itr, ++nListIndex )
	{
		pUserItem = (T_USER_ITEMptr)(*itr);
		if( pUserItem->nIndex == nIndex )
		{
			bResult = DeleteItemPtr( pUserItem, nListIndex );
			if( bResult ) m_ItemList.erase(itr);
			break;
		}
	}
	
	if( bResult == LTLV_CHANGEED ) RenderItemText();

	return LTTRUE;
}

T_USER_ITEMptr CSaListUser::GetUserData( int nIndex )
{
	LTLV_ITEM_LIST_ITR	itr;
	T_USER_ITEMptr		pUserItem;

	for( itr=m_ItemList.begin() ; itr!=m_ItemList.end() ; ++itr )
	{
		pUserItem = (T_USER_ITEMptr)(*itr);
		if( pUserItem->nIndex == nIndex )
		{
			return pUserItem;
		}
	}

	return LTNULL;
}

T_USER_ITEMptr CSaListUser::GetSelectedUserData()
{
	if( m_nSelIndex < 0 ) return LTFALSE;
	return (T_USER_ITEMptr)m_ItemList[m_nSelIndex];
}

LTBOOL CSaListUser::RenderItem( void* pListItem, const LTIntPt* ptItemPos,
								int32 nIndex, bool bSelect, bool bOver )
{
	T_USER_ITEMptr pUserItem = (T_USER_ITEMptr)pListItem;
	
	if( bSelect )
	{
		g_pDrawPrim->SetRGBA( &m_PolyBar, 0x70FFFFF );
		g_pDrawPrim->SetXYWH( &m_PolyBar, (float)ptItemPos->x, (float)ptItemPos->y, (float)m_nWidth, (float)m_nItemHeight );
		g_pDrawPrim->SetTexture( LTNULL );
		g_pDrawPrim->DrawPrim(&m_PolyBar);
	}
	else if( bOver )
	{
		g_pDrawPrim->SetRGBA( &m_PolyBar, 0xFF000000 );
		g_pDrawPrim->SetXYWH( &m_PolyBar, (float)ptItemPos->x, (float)ptItemPos->y, (float)m_nWidth, (float)m_nItemHeight );
		g_pDrawPrim->SetTexture( LTNULL );
		g_pDrawPrim->DrawPrim(&m_PolyBar);
		m_pNickList[nIndex].SetTextColor( RGB(250,204,133) );
	}
	else
	{
		m_pNickList[nIndex].SetTextColor( RGB(216,185,122) );
	}
	
	// [SPIKE] 계급표시 문제해결  
	g_pDrawPrim->BeginDrawPrim();
	g_pDrawPrim->SetXYWH( &m_PolyGrade, (float)ptItemPos->x, (float)ptItemPos->y, LIST_USER_TEXTURE_SIZE, LIST_USER_TEXTURE_SIZE );
	g_pDrawPrim->SetTexture( g_Global_Data.GetGradeTexture(pUserItem->nGrade) );
	g_pDrawPrim->DrawPrim(&m_PolyGrade);
	g_pDrawPrim->EndDrawPrim();
	m_pNickList[nIndex].Render();
	// <===
	return LTTRUE;
}

void CSaListUser::RenderItemText()
{
	int				i, nIndex;
	T_USER_ITEMptr	pUserItem;
	
	if( m_nFirstItemIndex < 0 || m_nLastItemIndex < 0 ) return;
	for( i=m_nFirstItemIndex, nIndex=0 ; i<=m_nLastItemIndex ; ++i, ++nIndex )
	{
		pUserItem = (T_USER_ITEMptr)m_ItemList[i];
		if( pUserItem )
		{
			m_pNickList[nIndex].DrawD3DText( pUserItem->szNick, 
											 strlen(pUserItem->szNick), 
											 DT_LEFT, D3D_TEXT_NORMAL, true );
		}
	}
}

LTBOOL CSaListUser::OnRButtonDown( int x, int y )
{
	if( CLTGUIListView::OnRButtonDown( x, y ) )
	{
		T_USER_ITEMptr	pUserItem = this->GetSelectedUserData();
		if( pUserItem )
		{
			m_pCommandHandler->SendCommand( CMD_SHOW_POPUP, pUserItem->nIndex, LTNULL );
		}
		return LTTRUE;
	}

	return LTFALSE;
}

LTBOOL CSaListUser::OnLButtonDblClick( int x, int y )
{
	if( m_nSelIndex < 0 ) return LTFALSE;
	
	T_USER_ITEMptr	pUserItem = this->GetSelectedUserData();
	if( pUserItem )
	{
		m_pCommandHandler->SendCommand( CMD_USER_INFO, pUserItem->nIndex, LTNULL );
	}
	return LTTRUE;
}

LTBOOL CSaListUser::OnWheelUp( int x, int y )
{
	LTBOOL bResult = CLTGUIListView::OnWheelUp( x, y );
	m_pCommandHandler->SendCommand( CMD_MAIN_USER_WHELL, LTNULL, LTNULL );
	return bResult;
}

LTBOOL CSaListUser::OnWheelDown( int x, int y )
{
	LTBOOL bResult = CLTGUIListView::OnWheelDown( x, y );
	m_pCommandHandler->SendCommand( CMD_MAIN_USER_WHELL, LTNULL, LTNULL );
	return bResult;
}




CSaListFriend::CSaListFriend() : CSaListUser() {}
CSaListFriend::~CSaListFriend()
{
	this->Term();
	SAFE_DELETE_ARRAY( m_pNickList );
}

LTBOOL CSaListFriend::AddUser( T_FRIEND_ITEMptr pFriendItem )
{
	return CSaListUser::AddUser( (T_USER_ITEMptr)pFriendItem );
}

T_FRIEND_ITEMptr CSaListFriend::GetUserData( int nIndex )
{
	return (T_FRIEND_ITEMptr)CSaListUser::GetUserData( nIndex );
}

T_FRIEND_ITEMptr CSaListFriend::GetSelectedUserData()
{
	return (T_FRIEND_ITEMptr)CSaListUser::GetSelectedUserData();
}

LTBOOL CSaListFriend::SetStatus( uint16 nStatus )
{
	return LTTRUE;
}

LTBOOL CSaListFriend::OnWheelUp( int x, int y )
{
	LTBOOL bResult = CLTGUIListView::OnWheelUp( x, y );
	m_pCommandHandler->SendCommand( CMD_MAIN_FRIEND_WHELL, LTNULL, LTNULL );
	return bResult;
}

LTBOOL CSaListFriend::OnWheelDown( int x, int y )
{
	LTBOOL bResult = CLTGUIListView::OnWheelDown( x, y );
	m_pCommandHandler->SendCommand( CMD_MAIN_FRIEND_WHELL, LTNULL, LTNULL );
	return bResult;
}

LTBOOL CSaListFriend::RenderItem( void* pListItem, const LTIntPt* ptItemPos,
								 int32 nIndex, bool bSelect, bool bOver )
{
	T_FRIEND_ITEMptr pFriendItem = (T_FRIEND_ITEMptr)pListItem;
	
	if( bSelect )
	{
		g_pDrawPrim->SetRGBA( &m_PolyBar, 0x70000000 );
		g_pDrawPrim->SetXYWH( &m_PolyBar, (float)ptItemPos->x, (float)ptItemPos->y, (float)m_nWidth, (float)m_nItemHeight );
		g_pDrawPrim->SetTexture( LTNULL );
		g_pDrawPrim->DrawPrim(&m_PolyBar);
	}
	else if( bOver )
	{
		g_pDrawPrim->SetRGBA( &m_PolyBar, 0x35000000 );
		g_pDrawPrim->SetXYWH( &m_PolyBar, (float)ptItemPos->x, (float)ptItemPos->y, (float)m_nWidth, (float)m_nItemHeight );
		g_pDrawPrim->SetTexture( LTNULL );
		g_pDrawPrim->DrawPrim(&m_PolyBar);
	}
	
	g_pDrawPrim->BeginDrawPrim();
	g_pDrawPrim->SetXYWH( &m_PolyGrade, (float)ptItemPos->x+5.0f, (float)ptItemPos->y+2.0f, 16.0f, 16.0f );
	g_pDrawPrim->SetTexture( g_Global_Data.GetGradeTexture(pFriendItem->nGrade) );
	g_pDrawPrim->DrawPrim(&m_PolyGrade);
	g_pDrawPrim->EndDrawPrim();
	m_pNickList[nIndex].Render();
	
	return LTTRUE;
}

void CSaListFriend::RenderItemText()
{
	int					i, nIndex;
	T_FRIEND_ITEMptr	pFriendItem;
	
	if( m_nFirstItemIndex < 0 || m_nLastItemIndex < 0 ) return;
	for( i=m_nFirstItemIndex, nIndex=0 ; i<=m_nLastItemIndex ; ++i, ++nIndex )
	{
		pFriendItem = (T_FRIEND_ITEMptr)m_ItemList[i];
		if( pFriendItem )
		{
			m_pNickList[nIndex].DrawD3DText( pFriendItem->szNick, 
											 strlen(pFriendItem->szNick), 
											 DT_LEFT, D3D_TEXT_NORMAL, true );
		}
	}
}