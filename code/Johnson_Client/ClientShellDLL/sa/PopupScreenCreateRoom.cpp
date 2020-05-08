#include "stdafx.h"
#include "ScreenCommands.h"
#include "MissionButeMgr.h"
#include "PopupScreenCreateRoom.h"

#include "GameText.h"
#include "GameMessage.h"

#define CREATEROOM_W	326.0f
#define CREATEROOM_H	195.0f

CPopScreenCreateRoom::CPopScreenCreateRoom()
{
	m_pEditTitle	= LTNULL;
	m_pEditPassword	= LTNULL;
	m_pBtnOK		= LTNULL;
	m_pBtnCancel	= LTNULL;
	m_pBtnCombo		= LTNULL;
	m_pCkPassword	= LTNULL;
	m_pPlayersList	= LTNULL;
	m_pPlayers		= LTNULL;
}

CPopScreenCreateRoom::~CPopScreenCreateRoom()
{
	SAFE_DELETE( m_pPlayers );
}

void CPopScreenCreateRoom::Init( CLTGUICommandHandler* pCommandHandler )
{
	LTIntPt BasePt;
	uint32	nTexW, nTexH;

	BasePt.x	= int(USABLE_WIDTH_F*0.5f-CREATEROOM_W*0.5f);
	BasePt.y	= 150;

	m_hBack = g_pInterfaceResMgr->GetTexture( "SA_INTERFACE\\Menu\\Textures\\DlgMain\\TxBack.dtx" );
	g_pTexInterface->GetTextureDims( m_hBack, nTexW, nTexH );
	g_pDrawPrim->SetUVWH( &m_Poly, 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_Poly, 0xFFFFFFFF );
	g_pDrawPrim->SetXYWH( &m_Poly, (float)BasePt.x, (float)BasePt.y, (float)nTexW, (float)nTexH );

	m_hPasswdBack = g_pInterfaceResMgr->GetTexture( "SA_INTERFACE\\Menu\\Textures\\DlgMain\\TxPassword.dtx" );
	g_pDrawPrim->SetUVWH( &m_PolyPasswd, 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_PolyPasswd, 0xFFFFFFFF );
	g_pDrawPrim->SetXYWH( &m_PolyPasswd, (float)BasePt.x+87.0f, (float)BasePt.y+81.0f, 256.0f, 32.0f );
		
	//Create( LTNULL, nTexW, nTexH+40, LTTRUE);
	Create( LTNULL, USABLE_WIDTH_I, USABLE_HEIGHT_I, LTTRUE);
	
	SetBasePos(BasePt);
	Show(LTTRUE);
	m_pCommandHandler	= pCommandHandler;

	LTIntPt CtrlPt;

	m_pEditTitle	= new CLTGUIEditBox();
	m_pEditTitle->Create( 0, 0, 205, 16,
						RGB(255,255,255), RGB(0,0,0), 0xFFFFFFFF,
						D3D_TEXT_SHADOW, 
						LTFALSE,
						CMD_CREATE_ROOM_TITLE, this );

	CtrlPt.x = 87; CtrlPt.y = 46;
	AddControl( m_pEditTitle, CtrlPt );

	m_pEditPassword	= new CLTGUIEditBox();
	m_pEditPassword->Create( 0, 0, 205, 16,
						RGB(255,255,255), RGB(0,0,0), 0xFFFFFFFF,
						D3D_TEXT_SHADOW, 
						LTTRUE,
						CMD_CREATE_ROOM_PASSWORD, this );
	
	CtrlPt.x = 87; CtrlPt.y = 84;
	AddControl( m_pEditPassword, CtrlPt );

	m_pCkPassword = new CLTGUIButton;
	AddCheckButton( "ScreenMain", m_pCkPassword, CMD_CREATE_ROOM_CHECK, "CkRectPasswd", "CkPasswd", "CkPasswdH");
	m_pCkPassword->SetCheck(LTFALSE);
	
	m_pBtnOK		= new CLTGUIButton();
	AddButton( "ScreenMain", m_pBtnOK, CMD_OK, "BtnOKTxN", "BtnOKTxH", "BtnOKTxD", "BtnOKTxC", "BtnRectCROk" );

	m_pBtnCancel	= new CLTGUIButton();
	AddButton( "ScreenMain", m_pBtnCancel, CMD_CANCEL, "BtnCancelTxN", "BtnCancelTxH", "BtnOKTxD", "BtnCancelTxC", "BtnRectCRCancel" );
	m_pBtnCancel->SetClickSound( (int)IS_CLICK_CANCEL );

	m_pBtnCombo		= new CLTGUIButton();
	AddButton( "ScreenMain", m_pBtnCombo, CMD_CREATE_ROOM_COMBO, "BtnComboTxN", "BtnComboTxH", "BtnComboTxD", "BtnComboTxC", "BtnRectCRCombo" );
	
	m_pPlayersList	= new CSaListPlayers();
	m_pPlayersList->Create( BasePt.x + 88, BasePt.y + 139, 191, 160, 20, this, LTTRUE,
							0xFF0E1319, 0xFF676D6D );

	for( int i=1 ; i<=8 ; ++i )
	{
		char* pItem = new char[16];
		sprintf( pItem, "%d vs %d", i, i );
		m_pPlayersList->AddItem( pItem );
	}
	m_pPlayersList->RenderItemText();
	AddControl( m_pPlayersList );

	m_pPlayers	= new CSAD3DText();
	m_pPlayers->CreateD3DText( 203, 16, false );
	m_pPlayers->SetPosition( BasePt.x + 82, BasePt.y + 122 );
	m_pPlayers->SetTextColor( RGB(255,255,255), RGB(0,0,0) );
	m_pPlayers->SetTransparent(true);

	SetParentScreen( (CScreenBase*)pCommandHandler );
}

void CPopScreenCreateRoom::OnFocus(LTBOOL bFocus)
{
	CUserProfile *pProfile = g_pProfileMgr->GetCurrentProfile();

	if( bFocus )
	{	
		this->SetDefaultRoomName();
		m_pEditTitle->ForceAddEditBox();
		m_pEditPassword->ClearText();
		m_pEditPassword->Show(LTFALSE);
		m_pCkPassword->SetCheck(LTFALSE);

		m_pPlayersList->SetSelectIndex(m_pPlayersList->GetLastIndex());
		m_pPlayersList->Show(LTFALSE);
		m_pPlayers->DrawD3DText( m_pPlayersList->GetSelectedPlayersData(),
								strlen(m_pPlayersList->GetSelectedPlayersData()),
								DT_CENTER, D3D_TEXT_SHADOW, false );
	}
	else
	{	
		m_pEditTitle->OnLostFocus();
		m_pEditPassword->OnLostFocus();
	}	
}

void CPopScreenCreateRoom::CreateRoom()
{
	int nNumMission = g_pMissionButeMgr->GetNumMissions();
	MISSION* pMission;

#ifdef GAME_SHOW
	pMission = g_pMissionButeMgr->GetMission( 1 ); //3rd.Supplybase
#elif _FINAL
	srand( ::GetTickCount() );
	pMission = g_pMissionButeMgr->GetMission( GetRandom(0,nNumMission-1) );
#else
	pMission = g_pMissionButeMgr->GetMission( 0 ); //ware house
#endif

	if( !pMission )
	{
		pMission = g_pMissionButeMgr->GetMission(0);
		if( !pMission ) return;
	}

	if( strlen( m_pEditTitle->GetText() ) <= 0 )
	{
		SetDefaultRoomName();
	}
	
	g_Network.CS_CreateRoom( m_pEditTitle->GetText(),
							 m_pCkPassword->GetCheck() ? m_pEditPassword->GetText() : "", 
							 m_pPlayersList->GetPlayers(),
							 pMission->nId,
							 pMission->eGameType,
							 pMission->nTimeLimit,
							 pMission->nKillLimit,
							 pMission->nRoundLimit );
}

LTBOOL 
CPopScreenCreateRoom::OnLButtonUp(int x, int y)
{
	if( m_pPlayersList->IsVisible() )
	{
		m_pPlayersList->Show(LTFALSE);
	}
	return CPopScreenBase::OnLButtonUp(x, y);
}

LTBOOL CPopScreenCreateRoom::OnEnter()
{
	OnCommand( CMD_CREATE_ROOM_TITLE, UIM_EDIT_ENTER, LTNULL );
	return LTTRUE;
}

LTBOOL CPopScreenCreateRoom::OnEscape()
{
	CtrlHide();
	return LTTRUE;
}

uint32 CPopScreenCreateRoom::OnCommand(uint32 dwCommand, uint32 dwParam1, uint32 dwParam2)
{
	switch( dwCommand )
	{
	case CMD_OK:
		CreateRoom();
		CtrlHide();
		break;
	case CMD_CANCEL:
		CtrlHide();
		break;
	case CMD_CREATE_ROOM_COMBO:
		if( m_pPlayersList->IsVisible() )
		{
			m_pPlayersList->Show(LTFALSE);
		}
		else
		{
			m_pPlayersList->Show(LTTRUE);
		}
		break;
	case CMD_CREATE_ROOM_PLAYERS:
		m_pPlayers->DrawD3DText( m_pPlayersList->GetSelectedPlayersData(),
								strlen(m_pPlayersList->GetSelectedPlayersData()),
								DT_CENTER, D3D_TEXT_SHADOW, false );
		break;
	case CMD_CREATE_ROOM_CHECK:
		if( m_pCkPassword->GetCheck() )
		{
			m_pCkPassword->SetCheck(LTFALSE);
			m_pEditPassword->Show(LTFALSE);
		}
		else
		{
			m_pCkPassword->SetCheck(LTTRUE);
			m_pEditPassword->Show(LTTRUE);
			m_pEditPassword->ForceAddEditBox();
		}
		break;

	case CMD_CREATE_ROOM_TITLE:
	case CMD_CREATE_ROOM_PASSWORD:
		if( UIM_EDIT_ENTER == dwParam1 )
		{
			CreateRoom();
			CtrlHide();
		}
		break;
	}
	return 1;
}

void CPopScreenCreateRoom::Render()
{
	if( IsVisible() == LTFALSE ) return;

	g_pDrawPrim->SetTexture( m_hBack );
	g_pDrawPrim->DrawPrim( &m_Poly );

	if( m_pCkPassword->GetCheck() )
	{
		g_pDrawPrim->SetTexture( m_hPasswdBack );
		g_pDrawPrim->DrawPrim( &m_PolyPasswd );
	}

	CLTGUIWindow::Render();
	m_pPlayers->Render();
}

void CPopScreenCreateRoom::SetDefaultRoomName()
{
	char	szAttr[64];
	int		nTitleMessageNum = 0;
	char	szTitle[MAX_R_TITLE_LEN];


	sprintf( szAttr,
			 "DefaultRoomName_%d",
			 GetRandom( 1, g_pLayoutMgr->GetInt( "ScreenMain", "DefaultRoomNameCount", 1 ) ) );

	nTitleMessageNum = g_pLayoutMgr->GetInt( "ScreenMain", szAttr, 0 );

	if( nTitleMessageNum <= 0 )
	{
		lstrcpy( szTitle, NF_GetMsg(SA_TEXT_DEFAULT_ROOM_NAME_4) );
	}
	else
	{
		lstrcpy( szTitle, NF_GetMsg((DWORD)nTitleMessageNum) );
	}

	m_pEditTitle->SetText( szTitle );
}






//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


CSaListPlayers::CSaListPlayers() 
:m_pPlayersList(LTNULL)
,CLTGUIListView() {}

CSaListPlayers::~CSaListPlayers()
{
	this->Term();
	SAFE_DELETE_ARRAY( m_pPlayersList );
}

LTBOOL CSaListPlayers::Create( uint16 nPosX, uint16 nPosY, uint16 nWidth, uint16 nHeight,
							   uint16 nItemHeight,
							   CLTGUICommandHandler* pCommandHandler,
							   LTBOOL bDrawBack, uint32 nBackColor, uint32 nBorderColor )
{
	CLTGUIListView::Create( nPosX, nPosY, nWidth, nHeight,
							nItemHeight, pCommandHandler, bDrawBack, nBackColor, nBorderColor );

	int i;
	m_pPlayersList = new CSAD3DText[m_nPageItemCount];

	for( i=0 ; i<m_nPageItemCount ; ++i )
	{
		m_pPlayersList[i].CreateD3DText( nWidth, 16, false );
		m_pPlayersList[i].SetTextColor( RGB(255,255,255), RGB(0,0,0) );
		m_pPlayersList[i].SetTransparent(true);
		m_pPlayersList[i].SetPosition( nPosX, nPosY+(i*m_nItemHeight)+4 );
		m_pPlayersList[i].DrawD3DText( "", 0 );
	}

	return  LTTRUE;
}

LTBOOL CSaListPlayers::AddItem( const char* lpszItem )
{
	LTBOOL bResult;
	bResult = InsertItem( (void *)lpszItem );
	if( !bResult )
	{
		return LTFALSE;
	}
	return LTTRUE;
}

void CSaListPlayers::RenderItemText()
{
	int				i, nIndex;
		
	if( m_nFirstItemIndex < 0 || m_nLastItemIndex < 0 ) return;
	for( i=m_nFirstItemIndex, nIndex=0 ; i<=m_nLastItemIndex ; ++i, ++nIndex )
	{
		const char* lpszItem = (const char*)m_ItemList[i];
		if( lpszItem )
		{
			m_pPlayersList[nIndex].DrawD3DText( lpszItem, 
											 strlen(lpszItem), 
											 DT_CENTER, D3D_TEXT_NORMAL, false );
		}
	}
}

LTBOOL CSaListPlayers::RenderItem( void* pListItem, const LTIntPt* ptItemPos,
								int32 nIndex, bool bSelect, bool bOver )
{
	const char* lpszItem = (const char*)pListItem;
	
	if( bSelect )
	{
		g_pDrawPrim->SetRGBA( &m_PolyBar, LTLV_DEFAULT_SEL_COLOR );
		g_pDrawPrim->SetXYWH( &m_PolyBar, (float)ptItemPos->x, (float)ptItemPos->y, (float)m_nWidth, (float)m_nItemHeight );
		g_pDrawPrim->SetTexture( LTNULL );
		g_pDrawPrim->DrawPrim(&m_PolyBar);
	}
	else if( bOver )
	{
		g_pDrawPrim->SetRGBA( &m_PolyBar, LTLV_DEFAULT_OVER_COLOR );
		g_pDrawPrim->SetXYWH( &m_PolyBar, (float)ptItemPos->x, (float)ptItemPos->y, (float)m_nWidth, (float)m_nItemHeight );
		g_pDrawPrim->SetTexture( LTNULL );
		g_pDrawPrim->DrawPrim(&m_PolyBar);
	}

	m_pPlayersList[nIndex].Render();
	return LTTRUE;
}

LTBOOL CSaListPlayers::OnLButtonDown( int x, int y )
{
	if( CLTGUIListView::OnLButtonDown( x, y ) )
	{
		this->Show(LTFALSE);
		m_pCommandHandler->SendCommand( CMD_CREATE_ROOM_PLAYERS, LTNULL, LTNULL );
		return LTTRUE;
	}
	
	return LTFALSE;
}

const char* CSaListPlayers::GetSelectedPlayersData()
{
	if( m_nSelIndex < 0 )
	{
		m_nSelIndex = GetLastIndex();
	}
	return (const char*)m_ItemList[m_nSelIndex];
}

int CSaListPlayers::GetPlayers()
{
	if( m_nSelIndex < 0 )
	{
		return ( GetLastIndex()+1 ) * 2;
	}
	return (m_nSelIndex+1) * 2;
}