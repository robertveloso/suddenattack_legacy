#include "stdafx.h"
#include "PopupInvite.h"
#include "ScreenCommands.h"
#include "GameText.h"
#include "GameMessage.h"

CPopupInvite::CPopupInvite()
{
	m_pBtnInvite	= LTNULL;
	m_pBtnCancel	= LTNULL;
	m_hBack			= LTNULL;
	m_pInviteUserList	= LTNULL;
}

CPopupInvite::~CPopupInvite()
{
}

void CPopupInvite::Init( CLTGUICommandHandler* pCommandHandler )
{
	LTIntPt BasePt;
	BasePt.x	= int(USABLE_WIDTH_F*0.5f-157.0f);
	BasePt.y	= 150;
	m_hBack = g_pInterfaceResMgr->GetTexture( "SA_INTERFACE\\Menu\\Textures\\Room\\TxInvite.dtx" );
		
	Create( m_hBack, 512, 256, LTTRUE);
	SetBasePos(BasePt);
	Show(LTTRUE);

	m_pCommandHandler	= pCommandHandler;

	m_pBtnInvite	= new CLTGUIButton();
	AddButton( "ScreenRoom", m_pBtnInvite, CMD_INVITE, "BtnDlgInviteN", "BtnDlgInviteH", "BtnDlgInviteD", "BtnDlgInviteC", "BtnDlgInviteRt" );
	m_pBtnCancel	= new CLTGUIButton();
	AddButton( "ScreenRoom", m_pBtnCancel, CMD_CANCEL, "BtnDlgCancelN", "BtnDlgCancelH", "BtnDlgCancelD", "BtnDlgCancelC", "BtnDlgCancelRt" );
	m_pBtnScrollUp	= new CLTGUIButton();
	AddButton( "ScreenRoom", m_pBtnScrollUp, CMD_INVITE_SCROLL_UP, "ScrollUpN", "ScrollUpH", "ScrollUpD", "ScrollUpC", "BtnDlgInviteScrollUp" );
	m_pBtnScrollDown	= new CLTGUIButton();
	AddButton( "ScreenRoom", m_pBtnScrollDown, CMD_INVITE_SCROLL_DOWN, "ScrollDnN", "ScrollDnH", "ScrollDnD", "ScrollDnC", "BtnDlgInviteScrollDn" );
	

	m_pInviteUserList	= new CSaListInviteUser();
	m_pInviteUserList->Create( BasePt.x+22, BasePt.y+44, 250, 135, 27, this );
	AddControl( m_pInviteUserList );
	

	m_pBtnScroll = new CLTGUIDragButton();
	AddDragButton( "ScreenRoom", m_pBtnScroll, CMD_INVITE_SCROLL, "MapScrollN", "MapScrollH", "BtnDlgInviteScroll" );
	m_pBtnScroll->SetLockX(LTTRUE);
	m_pBtnScroll->SetLimitY( BasePt.y+54, BasePt.y+135 );

	SetParentScreen( (CScreenBase*)pCommandHandler );
}

void CPopupInvite::OnFocus(LTBOOL bFocus)
{
	if( bFocus )
	{		
	}
	else
	{
		ClearList();
	}
}

void CPopupInvite::AddLobbyUser( const char* lpszNick, int nGrade )
{
	if( !lpszNick ) return;

	T_INVITE_USERptr pUser = new T_INVITE_USER;
	pUser->bChecked	= LTFALSE;
	pUser->nGrade	= nGrade;
	strcpy( pUser->szNickName, lpszNick );
	
	m_pInviteUserList->AddUser( pUser );
}

void CPopupInvite::ClearList()
{
	m_pInviteUserList->DeleteAllItem();
}

uint32 CPopupInvite::OnCommand(uint32 dwCommand, uint32 dwParam1, uint32 dwParam2)
{
	switch(dwCommand)
	{
	case CMD_INVITE:
		{
			int nSendCount;
			nSendCount = m_pInviteUserList->SendInvitePacket();
			if( nSendCount > 0 )
			{
				m_pCommandHandler->SendCommand( CMD_INVITE_RESULT, nSendCount, LTNULL );
			}

			CtrlHide();
		}
		break;
	case CMD_CANCEL:
		CtrlHide();
		break;
	case CMD_INVITE_SCROLL:
		{
			if( dwParam1 == CMD_PARAM_MOUSEMOVE )
			{
				m_pInviteUserList->ScrollFromRatio( m_pBtnScroll->GetPosYRatio() );
			}
		}
		break;
	case CMD_INVITE_SCROLL_UP:
		if( m_pInviteUserList->IsEnableScroll() )
		{
			m_pInviteUserList->ScrollUp();
			m_pBtnScroll->SetPosYRatio( m_pInviteUserList->GetRatio() );
		}
		break;
	case CMD_INVITE_SCROLL_DOWN:
		if( m_pInviteUserList->IsEnableScroll() )
		{
			m_pInviteUserList->ScrollDown();
			m_pBtnScroll->SetPosYRatio( m_pInviteUserList->GetRatio() );
		}
		break;
	}
	return 1;
}

void CPopupInvite::Render()
{
	if( IsVisible() == LTFALSE ) return;
	CLTGUIWindow::Render();

	m_pInviteUserList->Render();
}

LTBOOL CPopupInvite::OnWheelUp( int x, int y )
{
	OnCommand( CMD_INVITE_SCROLL_UP, LTNULL, LTNULL );
	return LTTRUE;
}

LTBOOL CPopupInvite::OnWheelDown( int x, int y )
{
	OnCommand( CMD_INVITE_SCROLL_DOWN, LTNULL, LTNULL );
	return LTTRUE;
}

LTBOOL CPopupInvite::OnEnter()
{
	OnCommand( CMD_INVITE, LTNULL, LTNULL );
	return LTTRUE;
}

LTBOOL CPopupInvite::OnEscape()
{
	OnCommand( CMD_CANCEL, LTNULL, LTNULL );
	return LTTRUE;
}


CSaListInviteUser::CSaListInviteUser() 
:m_hCheck(LTNULL)
,m_pNickList(LTNULL)
,CLTGUIListView() {}

CSaListInviteUser::~CSaListInviteUser()
{
	SAFE_DELETE_ARRAY( m_pNickList );
	this->Term();
}

LTBOOL CSaListInviteUser::Create( uint16 nPosX, uint16 nPosY,
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
		m_pNickList[i].CreateD3DText( 188, 16, false );
		m_pNickList[i].SetTextColor( RGB(216,185,122), RGB(0,0,0) );
		m_pNickList[i].SetTransparent(true);
		m_pNickList[i].SetPosition( nPosX+50, nPosY+(i*m_nItemHeight)+5 );
		m_pNickList[i].DrawD3DText( "", 0 );
	}
	
	m_hCheck	= g_pInterfaceResMgr->GetTexture( "SA_INTERFACE\\Menu\\Textures\\Common\\TxChecked.dtx" );
	g_pDrawPrim->SetRGBA( &m_PolyCheck, 0xFFFFFFFF );
	g_pDrawPrim->SetUVWH( &m_PolyCheck, 0.0f, 0.0f, 1.0f, 1.0f );

	g_pDrawPrim->SetUVWH( &m_PolyGrade, 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_PolyGrade, 0xFFFFFFFF );


	return  LTTRUE;
}

void CSaListInviteUser::AddUser( T_INVITE_USERptr pUser )
{
	LTBOOL bResult;
	bResult = InsertItem( (void *)pUser );
	if( bResult == LTLV_CHANGEED )
	{
		RenderItemText();
	}
}

LTBOOL CSaListInviteUser::RenderItem( void* pListItem, const LTIntPt* ptItemPos,
								int32 nIndex, bool bSelect, bool bOver )
{
	
	T_INVITE_USERptr pItem = (T_INVITE_USERptr)pListItem;

	if( pItem->bChecked )
	{
		g_pDrawPrim->SetXYWH( &m_PolyCheck, (float)ptItemPos->x+2.0f, (float)ptItemPos->y+3.0f, 16.0f, 16.0f );
		g_pDrawPrim->SetTexture(m_hCheck);
		g_pDrawPrim->DrawPrim( &m_PolyCheck );
	}

	g_pDrawPrim->SetXYWH( &m_PolyGrade, (float)ptItemPos->x+22.0f, (float)ptItemPos->y-1.0f, 32.0f, 32.0f );
	g_pDrawPrim->SetTexture( g_Global_Data.GetGradeTexture(pItem->nGrade) );
	g_pDrawPrim->DrawPrim(&m_PolyGrade);


	if( bOver )
	{
		m_pNickList[nIndex].SetTextColor( RGB(255,255,255) );
	}
	else
	{
		m_pNickList[nIndex].SetTextColor( RGB(216,185,122) );
	}
		
	m_pNickList[nIndex].Render();

	return LTTRUE;
}

LTBOOL CSaListInviteUser::OnLButtonDown( int x, int y )
{
	if( CLTGUIListView::OnLButtonDown( x, y ) )
	{
		if( m_nSelIndex < 0 || m_nSelIndex >= m_nTotalItemCount ) return LTTRUE;
		
		T_INVITE_USERptr pItem = (T_INVITE_USERptr)m_ItemList[m_nSelIndex];
		if( !pItem ) return LTFALSE;

		if( pItem->bChecked )
		{
			pItem->bChecked = LTFALSE;
		}
		else
		{
			pItem->bChecked = LTTRUE;
		}
		return LTTRUE;
	}
	
	return LTFALSE;
}

void CSaListInviteUser::RenderItemText()
{
	int					i, nIndex;
	T_INVITE_USERptr	pItem;
	
	if( m_nFirstItemIndex < 0 || m_nLastItemIndex < 0 ) return;
	for( i=m_nFirstItemIndex, nIndex=0 ; i<=m_nLastItemIndex ; ++i, ++nIndex )
	{
		pItem = (T_INVITE_USERptr)m_ItemList[i];
		if( pItem )
		{
			m_pNickList[nIndex].DrawD3DText( pItem->szNickName, 
											 strlen(pItem->szNickName), 
											 DT_LEFT, D3D_TEXT_NORMAL, true );
		}
	}
}

int CSaListInviteUser::SendInvitePacket()
{
	int					nCount = 0;
	LTLV_ITEM_LIST_ITR	itr;
	T_INVITE_USERptr	pItem;	

	for( itr=m_ItemList.begin() ; itr!=m_ItemList.end() ; ++itr )
	{
		pItem = (T_INVITE_USERptr)(*itr);
		if( pItem && pItem->bChecked )
		{
			++nCount;
			g_Network.CS_SendInvite( pItem->szNickName );
		}
	}

	return nCount;
}




CPopupInviteMsg::CPopupInviteMsg()
{
	m_pBtnOK		= LTNULL;
	m_pBtnCancel	= LTNULL;
	m_hBack			= LTNULL;
	m_pText			= LTNULL;
}

CPopupInviteMsg::~CPopupInviteMsg()
{
	SAFE_DELETE( m_pText );
}

void CPopupInviteMsg::Init( CLTGUICommandHandler* pCommandHandler )
{
	LTIntPt BasePt;
	BasePt.x	= int(USABLE_WIDTH_F*0.5f-163.0f);
	BasePt.y	= int(USABLE_HEIGHT_F*0.5f-98.0f);
	m_hBack = g_pInterfaceResMgr->GetTexture( "SA_INTERFACE\\Menu\\Textures\\Common\\TxDefaultMsgBox.dtx" );
		
	Create( m_hBack, 512, 256, LTTRUE);
	SetBasePos(BasePt);
	Show(LTTRUE);

	m_pCommandHandler = pCommandHandler;
	
	m_pBtnOK	= new CLTGUIButton();
	AddButton( "ScreenMain", m_pBtnOK, CMD_OK, "BtnOKTxN", "BtnOKTxH", "BtnOKTxD", "BtnOKTxC", "BtnRectInviteMsgOk" );

	m_pBtnCancel= new CLTGUIButton();
	AddButton( "ScreenMain", m_pBtnCancel, CMD_CANCEL, "BtnCancelTxN", "BtnCancelTxH", "BtnCancelTxD", "BtnCancelTxC", "BtnRectInviteMsgCancel" );

	m_pText	= new CSAD3DText();
	m_pText->CreateD3DText( 256, 64, true );
	m_pText->SetTextColor(RGB(175,148,104));
	m_pText->SetPosition( BasePt.x+30, BasePt.y+41 );
	m_pText->SetTransparent(true);
	
	SetParentScreen( (CScreenBase*)pCommandHandler );
}

void CPopupInviteMsg::OnFocus( LTBOOL bFocus )
{
}

void CPopupInviteMsg::CtrlShow( const char* lpszNick, int nRoomIndex, const char* lpszPass )
{
	if( !lpszNick ) return;

	char szTemp[256];
	sprintf( szTemp, NF_GetMsg(SA_TEXT_INVITE_MSG), nRoomIndex, lpszNick );
	m_pText->DrawD3DText( szTemp, strlen(szTemp), DT_LEFT, D3D_TEXT_OUTLINE );
		
	m_nRoomIndex = nRoomIndex;
	if( lpszPass )
	{
		strcpy( m_szPassword, lpszPass );
	}
	else
	{
		m_szPassword[0] = LTNULL;
	}
	CPopScreenBase::CtrlShow();
}

void CPopupInviteMsg::Render()
{
	if( IsVisible() == LTFALSE ) return;
	CLTGUIWindow::Render();
	m_pText->Render();
}

LTBOOL CPopupInviteMsg::OnEnter()
{
	OnCommand( CMD_OK, LTNULL, LTNULL );
	return LTTRUE;
}

LTBOOL CPopupInviteMsg::OnEscape()
{
	OnCommand( CMD_CANCEL, LTNULL, LTNULL );
	return LTTRUE;
}

uint32 CPopupInviteMsg::OnCommand( uint32 dwCommand, uint32 dwParam1, uint32 dwParam2)
{
	switch(dwCommand)
	{
	case CMD_OK:
		if( strlen( m_szPassword ) > 0 )
		{
			g_GameDoc.SetPassword(m_szPassword);
			m_pCommandHandler->SendCommand( CMD_JOIN_PRIVATE_ROOM, m_nRoomIndex, 1 );
		}
		else
		{
			g_GameDoc.SetPassword("");
			m_pCommandHandler->SendCommand( CMD_JOIN_ROOM, -1, m_nRoomIndex );
		}
		CtrlHide();
		break;

	case CMD_CANCEL:
		CtrlHide();
		break;
	}
	return 1;
}