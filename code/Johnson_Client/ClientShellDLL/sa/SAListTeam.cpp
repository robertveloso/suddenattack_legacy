#include "StdAfx.h"
#include "SaListTeam.h"
#include "InterfaceResMgr.h"
#include "../ClientShellShared/ScreenCommands.h"

#define TEAM_BAR_READY_W	97
#define TEAM_BAR_INVEN_W	80	
#define TEAM_BAR_PLAY_W		110

#define LIST_TEAM_TEXTURE_SIZE		32.0f

CSaListTeam::CSaListTeam() 
	: m_pIDList(LTNULL)
	  ,m_pClanList(LTNULL)
	  ,m_hReady(LTNULL)
	  ,m_hPlay(LTNULL)
	  ,m_hMark(LTNULL)
	  ,m_hCaptain(LTNULL)
	  ,CLTGUIListView()
{
}

CSaListTeam::~CSaListTeam()
{
	this->Term();

	SAFE_DELETE_ARRAY( m_pIDList );
	SAFE_DELETE_ARRAY( m_pClanList );
}

LTBOOL CSaListTeam::Create( uint16 nPosX, uint16 nPosY, 
						    uint16 nWidth, uint16 nHeight, uint16 nItemHeight,
							CLTGUICommandHandler* pCommandHandler,
							LTBOOL bDrawBack, uint32 nBackColor, uint32 nBorderColor )
{
	CLTGUIListView::Create( nPosX, nPosY, nWidth, nHeight, nItemHeight,
							pCommandHandler, bDrawBack, nBackColor, nBorderColor );

	int i;
	m_pIDList = new CSAD3DText[m_nPageItemCount];
	m_pClanList = new CSAD3DText[m_nPageItemCount];

	for( i=0 ; i<m_nPageItemCount ; ++i )
	{
		m_pIDList[i].CreateD3DText( 128, 16, false );
		m_pIDList[i].SetTextColor( RGB(218,218,218), RGB(0,0,0) );
		m_pIDList[i].SetTransparent(true);
		m_pIDList[i].SetPosition( nPosX+73, nPosY+(i*m_nItemHeight)+9 );
		m_pIDList[i].DrawD3DText( "", 0 );

		m_pClanList[i].CreateD3DText( 60, 16, false );
		m_pClanList[i].SetTextColor( RGB(218,218,218), RGB(0,0,0) );
		m_pClanList[i].SetTransparent(true);
		m_pClanList[i].SetPosition( nPosX+177, nPosY+(i*m_nItemHeight)+9 );
		m_pClanList[i].DrawD3DText( "", 0 );
	}

	for( i=0 ; i<MAX_GRADE_TEXTURE ; ++i )
	{
		m_hGradeList[i]	= g_Global_Data.GetGradeTexture(i);
	}

	m_hCaptain	= g_pInterfaceResMgr->GetTexture("SA_INTERFACE\\Menu\\Textures\\Room\\CaptainTx.dtx");
	m_hReady	= g_pInterfaceResMgr->GetTexture("SA_INTERFACE\\Menu\\Textures\\Room\\ReadyTx.dtx");
	m_hPlay		= g_pInterfaceResMgr->GetTexture("SA_INTERFACE\\Menu\\Textures\\Room\\PlayTx.dtx");
	m_hMark		= g_pInterfaceResMgr->GetTexture("SA_INTERFACE\\Menu\\Textures\\Room\\SpaceTx.dtx");
	m_hInven	= g_pInterfaceResMgr->GetTexture("SA_INTERFACE\\Menu\\Textures\\Room\\InvenTx.dtx");
	
	g_pDrawPrim->SetUVWH( &m_PolyMark, 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_PolyMark, 0xFFFFFFFF );
	
	g_pDrawPrim->SetUVWH( &m_PolyBar, 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_PolyBar, 0x70FFFFFF );
	
	g_pDrawPrim->SetUVWH( &m_PolyGrade, 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_PolyGrade, 0xFFFFFFFF );

	g_pDrawPrim->SetUVWH( &m_PolyCaptain, 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_PolyCaptain, 0xFFFFFFFF );


	return  LTTRUE;
}

LTBOOL CSaListTeam::AddUser( T_TEAM_ITEMptr pUserItem )
{
	g_pLTClient->CPrint( "CSaListTeam::AddUser %d\t%d", pUserItem->nIndex, pUserItem->bCaptain);

	//[MURSUM]=====================================================
	//이미 있는 사용자는 추가하지 않음
	if( LTNULL != GetUserData( pUserItem->nIndex ) ) return LTTRUE;
	//=============================================================
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

LTBOOL CSaListTeam::RemoveUser( int nIndex )
{
	int					nVecIndex;
	LTLV_ITEM_LIST_ITR	itr;
	LTBOOL				bResult;
	T_TEAM_ITEMptr		pUserItem;
	LTBOOL				bRemoved = LTFALSE;

	for( itr=m_ItemList.begin(), nVecIndex=0 ; itr!=m_ItemList.end() ; ++itr, ++nVecIndex )
	{
		pUserItem = (T_TEAM_ITEMptr)(*itr);
		if( pUserItem->nIndex == nIndex )
		{
			if( bResult = DeleteItemPtr( pUserItem, nVecIndex ) )
			{
				m_ItemList.erase(itr);
				bRemoved = LTTRUE;
			}
			break;
		}
	}
	
	if( bResult == LTLV_CHANGEED ) RenderItemText();

	return bRemoved;
}

T_TEAM_ITEMptr CSaListTeam::GetUserData( int nIndex, int* pnIndex )
{
	int i;
	LTLV_ITEM_LIST_ITR	itr;
	T_TEAM_ITEMptr		pUserItem;

	for( itr=m_ItemList.begin(), i=0 ; itr!=m_ItemList.end() ; ++itr, ++i )
	{
		pUserItem = (T_TEAM_ITEMptr)(*itr);
		if( pUserItem->nIndex == nIndex )
		{
			if( pnIndex ) (*pnIndex) = i;
			return pUserItem;
		}
	}

	if( pnIndex ) (*pnIndex) = -1;
	return LTNULL;
}

//[SPIKE] 레디유져 존재 유무판단
LTBOOL CSaListTeam::IsExistReadyUser()
{
	LTLV_ITEM_LIST_ITR	itr;
	T_TEAM_ITEMptr		pUserItem;

	for( itr=m_ItemList.begin() ; itr!=m_ItemList.end() ; ++itr )
	{
		pUserItem = (T_TEAM_ITEMptr)(*itr);
		if( pUserItem->nState == PLAYER_STATE_READY )
		{
			return LTTRUE;
		}
	}
	return LTFALSE;
}

int CSaListTeam::GetPlayerCount()
{
	return m_nTotalItemCount;
}

T_TEAM_ITEMptr CSaListTeam::GetUserDataFromVecIdx( int nIndex )
{
	if( nIndex < 0 || nIndex >= m_nTotalItemCount ) return LTNULL;
	return (T_TEAM_ITEMptr)m_ItemList[nIndex];
}

T_TEAM_ITEMptr CSaListTeam::GetSelectedUserData()
{
	if( m_nSelIndex < 0 ) return LTFALSE;
	return (T_TEAM_ITEMptr)m_ItemList[m_nSelIndex];
}

LTBOOL CSaListTeam::SetUserState( int nIndex, int nState, int nGrade )
{
	int nVecIndex;
	T_TEAM_ITEMptr pUserItem = GetUserData( nIndex, &nVecIndex );
	if( pUserItem && nVecIndex >= 0 )
	{
		pUserItem->nState = nState;
		if( nGrade >= 0 ) pUserItem->nGrade = nGrade;
		if( nVecIndex < m_nPageItemCount )
		{
			RenderItemText( nVecIndex );
		}
		else
		{
			g_pLTClient->CPrint( "[ERROR] Team Change" );
		}

		return LTTRUE;
	}

	return LTFALSE;
}

void CSaListTeam::ChangeCaptain( int nIndex )
{
	int i = 0;
	LTLV_ITEM_LIST_ITR	itr;
	T_TEAM_ITEMptr		pUserItem;

	for( itr=m_ItemList.begin() ; itr!=m_ItemList.end() ; ++itr )
	{
		pUserItem = (T_TEAM_ITEMptr)(*itr);
		if( pUserItem->nIndex == nIndex )
		{
			pUserItem->bCaptain = LTTRUE;
		}
		else
		{
			pUserItem->bCaptain = LTFALSE;
		}
	}
}

LTBOOL CSaListTeam::RenderItem( void* pListItem, const LTIntPt* ptItemPos,
								int32 nIndex, bool bSelect, bool bOver )
{
	T_TEAM_ITEMptr pUserItem = (T_TEAM_ITEMptr)pListItem;

	if( (pUserItem->nState == PLAYER_STATE_PLAY) ||
		(pUserItem->nState == PLAYER_STATE_P_READY) )
	{
		//게임중
		g_pDrawPrim->SetXYWH( &m_PolyCaptain, (float)ptItemPos->x+1.0f, (float)ptItemPos->y+1.0f, (float)m_nWidth-2.0f, (float)m_nItemHeight-3.0f );
		g_pDrawPrim->SetTexture( LTNULL );
		g_pDrawPrim->SetRGBA( &m_PolyCaptain, 0x60000000 );
		g_pDrawPrim->DrawPrim( &m_PolyCaptain );

		g_pDrawPrim->SetXYWH( &m_PolyCaptain, (float)ptItemPos->x+211.0f, (float)ptItemPos->y+6.0f, 64.0f, 32.0f );
		g_pDrawPrim->SetTexture( m_hPlay );
		g_pDrawPrim->SetRGBA( &m_PolyCaptain, 0xFFFFFFFF );
		g_pDrawPrim->DrawPrim( &m_PolyCaptain );
	}
	else if(pUserItem->nState == PLAYER_STATE_INVEN)
	{
		//인벤
		g_pDrawPrim->SetXYWH( &m_PolyCaptain, (float)ptItemPos->x+211.0f, (float)ptItemPos->y+6.0f, 64.0f, 32.0f );
		g_pDrawPrim->SetTexture( m_hInven );
		g_pDrawPrim->SetRGBA( &m_PolyCaptain, 0xFFFFFFFF );
		g_pDrawPrim->DrawPrim( &m_PolyCaptain );
	}
	else if( (pUserItem->nState == PLAYER_STATE_READY) 
		  && !pUserItem->bCaptain )
	{
		//레디
		g_pDrawPrim->SetXYWH( &m_PolyCaptain, (float)ptItemPos->x+1.0f, (float)ptItemPos->y+1.0f, (float)m_nWidth-2.0f, (float)m_nItemHeight-3.0f );
		g_pDrawPrim->SetTexture( LTNULL );
		g_pDrawPrim->SetRGBA( &m_PolyCaptain, 0x60000000 );
		g_pDrawPrim->DrawPrim( &m_PolyCaptain );
		
		g_pDrawPrim->SetXYWH( &m_PolyCaptain, (float)ptItemPos->x+211.0f, (float)ptItemPos->y+6.0f, 64.0f, 32.0f );
		g_pDrawPrim->SetTexture( m_hReady );
		g_pDrawPrim->SetRGBA( &m_PolyCaptain, 0xFFFFFFFF );
		g_pDrawPrim->DrawPrim( &m_PolyCaptain );
	}
	else if( bOver )
	{
		g_pDrawPrim->SetXYWH( &m_PolyCaptain, (float)ptItemPos->x+1.0f, (float)ptItemPos->y+1.0f, (float)m_nWidth-2.0f, (float)m_nItemHeight-3.0f );
		g_pDrawPrim->SetTexture( LTNULL );
		g_pDrawPrim->SetRGBA( &m_PolyCaptain, 0x70154556 );
		g_pDrawPrim->DrawPrim( &m_PolyCaptain );
	}

	// 마크
	if( pUserItem->hMark )
	{
		g_pDrawPrim->SetXYWH( &m_PolyMark, (float)ptItemPos->x+8.0f, (float)ptItemPos->y+3.0f, 32.0f, 32.0f );
		g_pDrawPrim->SetTexture( pUserItem->hMark  );
		g_pDrawPrim->SetRGBA( &m_PolyCaptain, 0xFFFFFFFF );
		g_pDrawPrim->DrawPrim(&m_PolyMark);
	}

	// 계급
	g_pDrawPrim->SetXYWH( &m_PolyGrade, (float)ptItemPos->x+38.0f, (float)ptItemPos->y+3.0f, LIST_TEAM_TEXTURE_SIZE, LIST_TEAM_TEXTURE_SIZE );
	g_pDrawPrim->SetTexture( m_hGradeList[pUserItem->nGrade] );
	g_pDrawPrim->SetRGBA( &m_PolyCaptain, 0xFFFFFFFF );
	g_pDrawPrim->DrawPrim(&m_PolyGrade);
	
	if( pUserItem->bCaptain )
	{
		//방장
		g_pDrawPrim->SetXYWH( &m_PolyCaptain, (float)ptItemPos->x+5.0f, (float)ptItemPos->y+2.0f, 16.0f, 16.0f );
		g_pDrawPrim->SetTexture( m_hCaptain );
		g_pDrawPrim->SetRGBA( &m_PolyCaptain, 0xFFFFFFFF );
		g_pDrawPrim->DrawPrim( &m_PolyCaptain );
	}
		
	m_pIDList[nIndex].Render();
	m_pClanList[nIndex].Render();

	return LTTRUE;
}

void CSaListTeam::RenderItemText()
{
	int				i, nIndex;
	T_TEAM_ITEMptr	pUserItem;

	int nDotDotW = -1;

	if( m_nFirstItemIndex < 0 || m_nLastItemIndex < 0 ) return;
	for( i=m_nFirstItemIndex, nIndex=0 ; i<=m_nLastItemIndex ; ++i, ++nIndex )
	{
		pUserItem = (T_TEAM_ITEMptr)m_ItemList[i];
		if( pUserItem )
		{
			if( pUserItem->nState == PLAYER_STATE_INVEN )
			{
				nDotDotW = TEAM_BAR_INVEN_W;
			}
			else if( (pUserItem->nState == PLAYER_STATE_PLAY) || (pUserItem->nState == PLAYER_STATE_P_READY) )
			{
				nDotDotW = TEAM_BAR_PLAY_W;
			}
			else if( (pUserItem->nState == PLAYER_STATE_READY) && !pUserItem->bCaptain )
			{
				nDotDotW = TEAM_BAR_READY_W;
			}
			else
			{
				nDotDotW = -1;
			}

			if( pUserItem->bCaptain )
			{
				m_pIDList[nIndex].SetTextColor( RGB(255,255,255), RGB(10,10,10) );
				m_pIDList[nIndex].DrawD3DText( pUserItem->szNick, strlen(pUserItem->szNick), DT_LEFT, D3D_TEXT_OUTLINE, true );

				m_pClanList[nIndex].SetTextColor( RGB(255,255,255), RGB(10,10,10) );
				m_pClanList[nIndex].DrawD3DText( pUserItem->szClan, strlen(pUserItem->szClan), DT_LEFT, D3D_TEXT_OUTLINE, true );
			}
			else
			{
				m_pIDList[nIndex].SetTextColor( RGB(220,220,220) );
				m_pIDList[nIndex].DrawD3DText( pUserItem->szNick, strlen(pUserItem->szNick), DT_LEFT, D3D_TEXT_NORMAL, true );

				m_pClanList[nIndex].SetTextColor( RGB(220,220,220) );
				m_pClanList[nIndex].DrawD3DText( pUserItem->szClan, strlen(pUserItem->szClan), DT_LEFT, D3D_TEXT_NORMAL, true );
			}
		}
	}
}

void CSaListTeam::RenderItemText( int nIndex )
{
	T_TEAM_ITEMptr	pUserItem;
	
	int nDotDotW = -1;

	if( m_nFirstItemIndex < 0 || m_nLastItemIndex < 0 ||
		m_nLastItemIndex > nIndex || nIndex < 0 ) return;

	pUserItem = (T_TEAM_ITEMptr)m_ItemList[nIndex];
	if( pUserItem )
	{
		if( pUserItem->nState == PLAYER_STATE_INVEN )
		{
			nDotDotW = TEAM_BAR_INVEN_W;
		}
		else if( (pUserItem->nState == PLAYER_STATE_PLAY) || (pUserItem->nState == PLAYER_STATE_P_READY) )
		{
			nDotDotW = TEAM_BAR_PLAY_W;
		}
		else if( (pUserItem->nState == PLAYER_STATE_READY) && !pUserItem->bCaptain )
		{
			nDotDotW = TEAM_BAR_READY_W;
		}
		else
		{
			nDotDotW = -1;
		}

		m_pIDList[nIndex].DrawD3DText( pUserItem->szNick, strlen(pUserItem->szNick), DT_LEFT, D3D_TEXT_NORMAL, true );
		m_pClanList[nIndex].DrawD3DText( pUserItem->szClan, strlen(pUserItem->szClan), DT_LEFT, D3D_TEXT_NORMAL, true );
	}
}

LTBOOL CSaListTeam::OnRButtonDown( int x, int y )
{
	if( CLTGUIListView::OnRButtonDown( x, y ) )
	{
		T_TEAM_ITEMptr	pUserItem = this->GetSelectedUserData();
		if( pUserItem )
		{
			m_pCommandHandler->SendCommand( CMD_SHOW_POPUP, pUserItem->nIndex, LTNULL );
		}
		return LTTRUE;
	}

	return LTFALSE;
}

LTBOOL CSaListTeam::OnLButtonDblClick( int x, int y )
{
	if( m_nSelIndex < 0 ) return LTFALSE;
	
	T_TEAM_ITEMptr	pUserItem = this->GetSelectedUserData();
	if( pUserItem )
	{
		m_pCommandHandler->SendCommand( CMD_USER_INFO, pUserItem->nIndex, LTNULL );
	}
	
	return LTTRUE;
}

void CSaListTeam::PrePlayingState()
{
	LTLV_ITEM_LIST_ITR	itr;
	T_TEAM_ITEMptr		pUserItem;

	CPlayerInfoMgr* pMgr = g_pInterfaceMgr->GetPlayerInfoMgr();

	for( itr=m_ItemList.begin() ; itr!=m_ItemList.end() ; ++itr )
	{
		pUserItem = (T_TEAM_ITEMptr)(*itr);
		if( pUserItem->nState == PLAYER_STATE_READY ||
			pUserItem->nState == PLAYER_STATE_P_READY ||
			pUserItem->nState == PLAYER_STATE_PLAY ||
			pUserItem->bCaptain == LTTRUE )
		{
			pMgr->SetNewPlayer( pUserItem->nIndex, LTFALSE );
		}
	}
}