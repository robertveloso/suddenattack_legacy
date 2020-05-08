#include "stdafx.h"
#include "ScreenCommands.h"
#include "MissionButeMgr.h"
#include "PopupRoomOption.h"

#include "GameText.h"
#include "GameMessage.h"

struct LessMapType
{
	bool operator()( const PVOID p, const PVOID q ) const
	{
		return ((T_ROOM_OPTION_MAPptr)p)->nGameType < ((T_ROOM_OPTION_MAPptr)q)->nGameType;
	}
};

struct GreatMapType
{
	bool operator()( const PVOID p, const PVOID q ) const
	{
		return ((T_ROOM_OPTION_MAPptr)p)->nGameType > ((T_ROOM_OPTION_MAPptr)q)->nGameType;
	}
};


CPopRoomOption* g_pPopRoomOption = LTNULL;

CPopRoomOption::CPopRoomOption()
{
	m_pBtnOK			= LTNULL;
	m_pBtnCancel		= LTNULL;
	m_pBtnComboPlayers	= LTNULL;
	m_pBtnComboTime		= LTNULL;
	m_pBtnComboKill		= LTNULL;
	m_pBtnComboRound	= LTNULL;
	m_pBtnScroll		= LTNULL;
	m_pBtnScrollUp		= LTNULL;
	m_pBtnScrollDown	= LTNULL;
	m_pBtnMapTypeSort	= LTNULL;
	m_pCkIntrusionOn	= LTNULL;
	m_pCkIntrusionOff	= LTNULL;
	m_pCkTeamBanlanceOn	= LTNULL;
	m_pCkTeamBanlanceOff= LTNULL;
	m_pCkFreeSpectatorOn= LTNULL;
	m_pCkFreeSpectatorOff=LTNULL;
	m_pOptionList		= LTNULL;	
	m_pMapList			= LTNULL;
	
	m_pCommandHandler	= LTNULL;
	m_pEditRoomTitle	= LTNULL;
}

CPopRoomOption::~CPopRoomOption()
{
	SAFE_DELETE( m_OptionInfo[ROOM_OPT_PLAYERS].pD3DText );
	SAFE_DELETE( m_OptionInfo[ROOM_OPT_TIME].pD3DText );
	SAFE_DELETE( m_OptionInfo[ROOM_OPT_KILL].pD3DText );
	SAFE_DELETE( m_OptionInfo[ROOM_OPT_ROUND].pD3DText );

	g_pPopRoomOption = LTNULL;
}	

void CPopRoomOption::Init( CLTGUICommandHandler* pCommandHandler )
{
	LTIntPt BasePt;
	uint32	nTexW, nTexH;

	BasePt.x	= int(USABLE_WIDTH_F*0.5f-103.0f);
	BasePt.y	= 85;

	m_hBack = g_pInterfaceResMgr->GetTexture( "SA_INTERFACE\\Menu\\Textures\\Room\\TxRoomOption.dtx" );
	g_pTexInterface->GetTextureDims( m_hBack, nTexW, nTexH );
	g_pDrawPrim->SetUVWH( &m_Poly, 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_Poly, 0xFFFFFFFF );
	g_pDrawPrim->SetXYWH( &m_Poly, (float)BasePt.x, (float)BasePt.y, (float)nTexW, (float)nTexH );

	Create( LTNULL, USABLE_WIDTH_I, USABLE_HEIGHT_I, LTTRUE);	
	SetBasePos(BasePt);
	Show(LTTRUE);
	m_pCommandHandler	= pCommandHandler;

	m_pEditRoomTitle	= new CLTGUIEditBox();
	m_pEditRoomTitle->Create( 0, 0, 137, 20, RGB(255,255,255), RGB(0,0,0), 0xFFFFFFFF,
							D3D_TEXT_NORMAL,
							LTFALSE,
							CMD_EDIT_ROOM_TITLE, this );
	LTIntPt	pt;
	pt.x	= 45;
	pt.y	= 11;
	AddControl(m_pEditRoomTitle,pt);

	m_pBtnComboPlayers	= new CLTGUIButton();
	AddButton( "ScreenRoom", m_pBtnComboPlayers, CMD_PLAYERS_DOWN, "BtnScrollDownN", "BtnScrollDownH", "BtnScrollDownD", "BtnScrollDownC", "BtnPlayersDownRt" );
	m_pBtnComboTime	= new CLTGUIButton();
	AddButton( "ScreenRoom", m_pBtnComboTime, CMD_TIME_DOWN, "BtnScrollDownN", "BtnScrollDownH", "BtnScrollDownD", "BtnScrollDownC", "BtnTimeDownRt" );
	m_pBtnComboKill	= new CLTGUIButton();
	AddButton( "ScreenRoom", m_pBtnComboKill, CMD_KILL_DOWN, "BtnScrollDownN", "BtnScrollDownH", "BtnScrollDownD", "BtnScrollDownC", "BtnKillDownRt" );
	m_pBtnComboRound	= new CLTGUIButton();
	AddButton( "ScreenRoom", m_pBtnComboRound, CMD_ROUND_DOWN, "BtnScrollDownN", "BtnScrollDownH", "BtnScrollDownD", "BtnScrollDownC", "BtnRoundDownRt" );
	m_pBtnScrollUp	= new CLTGUIButton();
	AddButton( "ScreenRoom", m_pBtnScrollUp, CMD_MAP_SCROLL_UP, "ScrollUpN", "ScrollUpH", "ScrollUpD", "ScrollUpC", "BtnRectMapScrollUp" );
	m_pBtnScrollDown	= new CLTGUIButton();
	AddButton( "ScreenRoom", m_pBtnScrollDown, CMD_MAP_SCROLL_DOWN, "ScrollDnN", "ScrollDnH", "ScrollDnD", "ScrollDnC", "BtnRectMapScrollDown" );
	m_pBtnMapTypeSort	= new CLTGUIButton();
	AddButton( "ScreenRoom", m_pBtnMapTypeSort, CMD_MAP_TYPE_SORT, "MapTypeSortN", "MapTypeSortH", "MapTypeSortD", "MapTypeSortC", "BtnRectMapTypeSort" );
	
	m_pBtnComboPlayers->SetClickSound( (int)IS_CLICK_NORMAL );
	m_pBtnComboTime->SetClickSound( (int)IS_CLICK_NORMAL );
	m_pBtnComboKill->SetClickSound( (int)IS_CLICK_NORMAL );
	m_pBtnComboRound->SetClickSound( (int)IS_CLICK_NORMAL );
	m_pBtnScrollUp->SetClickSound( (int)IS_CLICK_NORMAL );
	m_pBtnScrollDown->SetClickSound( (int)IS_CLICK_NORMAL );
	m_pBtnMapTypeSort->SetClickSound( (int)IS_CLICK_NORMAL );

	
	m_pCkIntrusionOn = new CLTGUIButton;
	AddCheckButton( "ScreenRoom", m_pCkIntrusionOn, CMD_INTRUSION_ON, "CkRectIntrusionOn", "CkBtnOnD", "CkBtnOnH");
	m_pCkIntrusionOn->SetCheck(LTTRUE);
	m_pCkIntrusionOff = new CLTGUIButton;
	AddCheckButton( "ScreenRoom", m_pCkIntrusionOff, CMD_INTRUSION_OFF, "CkRectIntrusionOff", "CkBtnOffD", "CkBtnOffH");
	m_pCkIntrusionOff->SetCheck(LTFALSE);

	m_pCkTeamBanlanceOn = new CLTGUIButton;
	AddCheckButton( "ScreenRoom", m_pCkTeamBanlanceOn, CMD_TEAMBAL_ON, "CkRectTeamBalOn", "CkBtnOnD", "CkBtnOnH");
	m_pCkTeamBanlanceOn->SetCheck(LTTRUE);
	m_pCkTeamBanlanceOff = new CLTGUIButton;
	AddCheckButton( "ScreenRoom", m_pCkTeamBanlanceOff, CMD_TEAMBAL_OFF, "CkRectTeamBalOff", "CkBtnOffD", "CkBtnOffH");
	m_pCkTeamBanlanceOff->SetCheck(LTFALSE);

	m_pCkFreeSpectatorOn = new CLTGUIButton;
	AddCheckButton( "ScreenRoom", m_pCkFreeSpectatorOn, CMD_FREESPEC_ON, "CkRectFreeSpecOn", "CkBtnOnD", "CkBtnOnH");
	m_pCkFreeSpectatorOn->SetCheck(LTTRUE);
	m_pCkFreeSpectatorOff = new CLTGUIButton;
	AddCheckButton( "ScreenRoom", m_pCkFreeSpectatorOff, CMD_FREESPEC_OFF, "CkRectFreeSpecOff", "CkBtnOffD", "CkBtnOffH");
	m_pCkFreeSpectatorOff->SetCheck(LTFALSE);
		
	m_pBtnOK		= new CLTGUIButton();
	AddButton( "ScreenRoom", m_pBtnOK, CMD_OK, "BtnOKTxN", "BtnOKTxH", "BtnOKTxD", "BtnOKTxC", "BtnRectCROk" );
	m_pBtnCancel	= new CLTGUIButton();
	AddButton( "ScreenRoom", m_pBtnCancel, CMD_CANCEL, "BtnCancelTxN", "BtnCancelTxH", "BtnOKTxD", "BtnCancelTxC", "BtnRectCRCancel" );
	m_pBtnCancel->SetClickSound( (int)IS_CLICK_CANCEL );

	m_pOptionList	= new CSaListOptions();
	m_pOptionList->Create( 0, 0, 79, 160, 20, this, LTTRUE, 0xFF0E1319, 0xFF676D6D );
	AddControl( m_pOptionList );

	m_pMapList	= new CSaListMap();
	m_pMapList->Create( BasePt.x+16, BasePt.y+77, 160, 110, 22, this );
	AddControl( m_pMapList );

	m_pBtnScroll = new CLTGUIDragButton();
	AddDragButton( "ScreenRoom", m_pBtnScroll, CMD_MAP_SCROLL, "MapScrollN", "MapScrollH", "MapScrollRect" );
	m_pBtnScroll->SetLockX(LTTRUE);
	m_pBtnScroll->SetLimitY( BasePt.y+96, BasePt.y+146 );

	m_OptionInfo[ROOM_OPT_PLAYERS].pD3DText	= new CSAD3DText();
	m_OptionInfo[ROOM_OPT_PLAYERS].pD3DText->CreateD3DText( 79, 16, false );
	m_OptionInfo[ROOM_OPT_PLAYERS].pD3DText->SetPosition( BasePt.x + 93, BasePt.y + 206 );
	m_OptionInfo[ROOM_OPT_PLAYERS].pD3DText->SetTextColor( RGB(255,255,255), RGB(0,0,0) );
	m_OptionInfo[ROOM_OPT_PLAYERS].pD3DText->SetTransparent(true);
	
	m_OptionInfo[ROOM_OPT_TIME].pD3DText	= new CSAD3DText();
	m_OptionInfo[ROOM_OPT_TIME].pD3DText->CreateD3DText( 79, 16, false );
	m_OptionInfo[ROOM_OPT_TIME].pD3DText->SetPosition( BasePt.x + 93, BasePt.y + 236 );
	m_OptionInfo[ROOM_OPT_TIME].pD3DText->SetTextColor( RGB(255,255,255), RGB(0,0,0) );
	m_OptionInfo[ROOM_OPT_TIME].pD3DText->SetTransparent(true);
	
	m_OptionInfo[ROOM_OPT_KILL].pD3DText	= new CSAD3DText();
	m_OptionInfo[ROOM_OPT_KILL].pD3DText->CreateD3DText( 79, 16, false );
	m_OptionInfo[ROOM_OPT_KILL].pD3DText->SetPosition( BasePt.x + 93, BasePt.y + 266 );
	m_OptionInfo[ROOM_OPT_KILL].pD3DText->SetTextColor( RGB(255,255,255), RGB(0,0,0) );
	m_OptionInfo[ROOM_OPT_KILL].pD3DText->SetTransparent(true);
	
	m_OptionInfo[ROOM_OPT_ROUND].pD3DText	= new CSAD3DText();
	m_OptionInfo[ROOM_OPT_ROUND].pD3DText->CreateD3DText( 79, 16, false );
	m_OptionInfo[ROOM_OPT_ROUND].pD3DText->SetPosition( BasePt.x + 93, BasePt.y + 296 );
	m_OptionInfo[ROOM_OPT_ROUND].pD3DText->SetTextColor( RGB(255,255,255), RGB(0,0,0) );
	m_OptionInfo[ROOM_OPT_ROUND].pD3DText->SetTransparent(true);
	

	SetParentScreen( (CScreenBase*)pCommandHandler );

	g_pPopRoomOption = this;
}

void CPopRoomOption::CtrlShow( T_ROOM_OPTIONptr pRoomOption )
{
	m_pCurOption	= pRoomOption;
	SetDefaultVal( m_pCurOption );
	m_pMapList->SetCurrentIndex( pRoomOption->nMapIndex );
	
	if( m_pMapList->IsEnableScroll() ) m_pBtnScroll->SetPosYRatio( m_pMapList->GetRatio() );
	CPopScreenBase::CtrlShow();
}

void CPopRoomOption::SetDefaultVal( T_ROOM_OPTIONptr pRoomOption )
{
	m_OptionInfo[ROOM_OPT_PLAYERS].nValue	= pRoomOption->nMaxUsers;
	m_OptionInfo[ROOM_OPT_TIME].nValue		= pRoomOption->nTimeLimit;
	m_OptionInfo[ROOM_OPT_KILL].nValue		= pRoomOption->nKillLimit;
	m_OptionInfo[ROOM_OPT_ROUND].nValue		= pRoomOption->nRoundLimit;
	m_OptionInfo[ROOM_OPT_MAP_IDX].nValue	= pRoomOption->nMapIndex;

	if( pRoomOption->nKillLimit > 0 )
	{
		m_pBtnComboKill->Show(LTTRUE);
		sprintf( m_OptionInfo[ROOM_OPT_KILL].szString, "%d", pRoomOption->nKillLimit );		
	}
	else
	{
		m_pBtnComboKill->Show(LTFALSE);
		sprintf( m_OptionInfo[ROOM_OPT_KILL].szString, "" );
	}
	m_OptionInfo[ROOM_OPT_KILL].pD3DText->DrawD3DText( m_OptionInfo[ROOM_OPT_KILL].szString, strlen(m_OptionInfo[ROOM_OPT_KILL].szString), DT_CENTER );

	if( pRoomOption->nRoundLimit > 0 )
	{
		m_pBtnComboRound->Show(LTTRUE);
		m_pBtnComboTime->Show(LTFALSE);
		sprintf( m_OptionInfo[ROOM_OPT_ROUND].szString, "%d", pRoomOption->nRoundLimit );

		m_pCkFreeSpectatorOn->Show(LTTRUE);
		m_pCkFreeSpectatorOff->Show(LTTRUE);
	}
	else
	{
		m_pBtnComboRound->Show(LTFALSE);
		m_pBtnComboTime->Show(LTTRUE);
		sprintf( m_OptionInfo[ROOM_OPT_ROUND].szString, "" );

		m_pCkFreeSpectatorOn->Show(LTFALSE);
		m_pCkFreeSpectatorOff->Show(LTFALSE);
	}
	m_OptionInfo[ROOM_OPT_ROUND].pD3DText->DrawD3DText( m_OptionInfo[ROOM_OPT_ROUND].szString, strlen(m_OptionInfo[ROOM_OPT_ROUND].szString), DT_CENTER );

	sprintf( m_OptionInfo[ROOM_OPT_PLAYERS].szString, "%d vs %d", pRoomOption->nMaxUsers/2, pRoomOption->nMaxUsers/2 );
	m_OptionInfo[ROOM_OPT_PLAYERS].pD3DText->DrawD3DText( m_OptionInfo[ROOM_OPT_PLAYERS].szString, strlen(m_OptionInfo[ROOM_OPT_PLAYERS].szString), DT_CENTER );

	sprintf( m_OptionInfo[ROOM_OPT_TIME].szString, "%d", pRoomOption->nTimeLimit );
	m_OptionInfo[ROOM_OPT_TIME].pD3DText->DrawD3DText( m_OptionInfo[ROOM_OPT_TIME].szString, strlen(m_OptionInfo[ROOM_OPT_TIME].szString), DT_CENTER );

	if( pRoomOption->bInstrusion )
	{
		m_pCkIntrusionOn->SetCheck(LTTRUE);
		m_pCkIntrusionOff->SetCheck(LTFALSE);
	}
	else
	{
		m_pCkIntrusionOn->SetCheck(LTFALSE);
		m_pCkIntrusionOff->SetCheck(LTTRUE);
	}
	if( pRoomOption->bTeamBalance )
	{
		m_pCkTeamBanlanceOn->SetCheck(LTTRUE);
		m_pCkTeamBanlanceOff->SetCheck(LTFALSE);
	}
	else
	{
		m_pCkTeamBanlanceOn->SetCheck(LTFALSE);
		m_pCkTeamBanlanceOff->SetCheck(LTTRUE);
	}
	if( pRoomOption->bFreeSpectator )
	{
		m_pCkFreeSpectatorOn->SetCheck(LTTRUE);
		m_pCkFreeSpectatorOff->SetCheck(LTFALSE);
	}
	else
	{
		m_pCkFreeSpectatorOn->SetCheck(LTFALSE);
		m_pCkFreeSpectatorOff->SetCheck(LTTRUE);
	}
}

void CPopRoomOption::OnFocus(LTBOOL bFocus)
{
	CUserProfile *pProfile = g_pProfileMgr->GetCurrentProfile();

	if( bFocus )
	{	
		m_pOptionList->Show(LTFALSE);
		m_pMapList->Show(LTTRUE);
		m_pEditRoomTitle->SetText( g_GameDoc.GetTitle() );
		m_pEditRoomTitle->ForceAddEditBox();
	}
	else
	{	
		m_pEditRoomTitle->OnLostFocus();
	}	
}

LTBOOL CPopRoomOption::OnLButtonUp(int x, int y)
{
	if( m_pOptionList->IsVisible() )
	{
		m_pOptionList->Show(LTFALSE);
	}
	return CPopScreenBase::OnLButtonUp(x, y);
}

LTBOOL CPopRoomOption::OnWheelUp( int x, int y )
{
	OnCommand( CMD_MAP_SCROLL_UP, LTNULL, LTNULL );
	return LTTRUE;
}

LTBOOL CPopRoomOption::OnWheelDown( int x, int y )
{
	OnCommand( CMD_MAP_SCROLL_DOWN, LTNULL, LTNULL );
	return LTTRUE;
}

uint32 CPopRoomOption::OnCommand(uint32 dwCommand, uint32 dwParam1, uint32 dwParam2)
{
	switch( dwCommand )
	{
	case CMD_OK:
		ApplyOption();
		CtrlHide();
		break;
	case CMD_CANCEL:
		CtrlHide();
		break;

	case CMD_MAP_TYPE_SORT:
		m_pMapList->SortMapType();
		break;

	case CMD_MAP_SCROLL:
		{
			if( dwParam1 == CMD_PARAM_MOUSEMOVE )
			{
				m_pMapList->ScrollFromRatio( m_pBtnScroll->GetPosYRatio() );
			}
		}
		break;

	case CMD_EDIT_ROOM_TITLE:
		if( UIM_EDIT_ENTER == dwParam1 )
		{
			ApplyOption();
			CtrlHide();
		}
		break;

	case CMD_MAP_SCROLL_UP:
		if( m_pMapList->IsEnableScroll() )
		{
			m_pMapList->ScrollUp();
			m_pBtnScroll->SetPosYRatio( m_pMapList->GetRatio() );
		}
		break;

	case CMD_MAP_SCROLL_DOWN:
		if( m_pMapList->IsEnableScroll() )
		{
			m_pMapList->ScrollDown();
			m_pBtnScroll->SetPosYRatio( m_pMapList->GetRatio() );
		}
		break;

	case CMD_PLAYERS_DOWN:
	case CMD_TIME_DOWN:
	case CMD_KILL_DOWN:
	case CMD_ROUND_DOWN:
		{
			int nType = dwCommand-CMD_PLAYERS_DOWN;
			if( m_pOptionList->IsVisible() )
			{
				if( m_pOptionList->GetType() == nType )
				{
					m_pOptionList->Show(LTFALSE);
				}
				else
				{
					m_pOptionList->ShowOption( nType, m_OptionInfo[nType].nValue );
				}
			}
			else
			{
				m_pOptionList->ShowOption( nType, m_OptionInfo[nType].nValue );
			}
		}
		break;
	
	case CMD_INTRUSION_ON:
		if( false == m_pCkIntrusionOn->GetCheck() )
		{
			m_pCkIntrusionOn->SetCheck(LTTRUE);
			m_pCkIntrusionOff->SetCheck(LTFALSE);
		}
		break;
	case CMD_INTRUSION_OFF:
		if( false == m_pCkIntrusionOff->GetCheck() )
		{
			m_pCkIntrusionOff->SetCheck(LTTRUE);
			m_pCkIntrusionOn->SetCheck(LTFALSE);
		}
		break;
	case CMD_TEAMBAL_ON:
		if( false == m_pCkTeamBanlanceOn->GetCheck() )
		{
			m_pCkTeamBanlanceOn->SetCheck(LTTRUE);
			m_pCkTeamBanlanceOff->SetCheck(LTFALSE);
		}
		break;
	case CMD_TEAMBAL_OFF:
		if( false == m_pCkTeamBanlanceOff->GetCheck() )
		{
			m_pCkTeamBanlanceOff->SetCheck(LTTRUE);
			m_pCkTeamBanlanceOn->SetCheck(LTFALSE);
		}
		break;
	case CMD_FREESPEC_ON:
		if( false == m_pCkFreeSpectatorOn->GetCheck() )
		{
			m_pCkFreeSpectatorOn->SetCheck(LTTRUE);
			m_pCkFreeSpectatorOff->SetCheck(LTFALSE);
		}
		break;
	case CMD_FREESPEC_OFF:
		if( false == m_pCkFreeSpectatorOff->GetCheck() )
		{
			m_pCkFreeSpectatorOff->SetCheck(LTTRUE);
			m_pCkFreeSpectatorOn->SetCheck(LTFALSE);
		}
		break;

	case CMD_ROOM_OPT_CHANGED:
		{
			m_OptionInfo[dwParam1].nValue = m_pOptionList->GetSelectedValue();
			strcpy( m_OptionInfo[dwParam1].szString, m_pOptionList->GetSelectedString() );
			
			if( ROOM_OPT_MAP_IDX != dwParam1 )
			{
				m_OptionInfo[dwParam1].pD3DText->DrawD3DText( m_OptionInfo[dwParam1].szString,
															  strlen(m_OptionInfo[dwParam1].szString),
															  DT_CENTER );
			}
		}
		break;

	case CMD_ROOM_MAP_CHANGED:
		{
			m_OptionInfo[ROOM_OPT_MAP_IDX].nValue	= dwParam1;

			MISSION* pMission = g_pMissionButeMgr->GetMission(dwParam1);
			if( !pMission ) break;
			
			T_ROOM_OPTION RoomOpt;			
			RoomOpt.bInstrusion		= m_pCkIntrusionOn->GetCheck();
			RoomOpt.bTeamBalance	= m_pCkTeamBanlanceOn->GetCheck();
			RoomOpt.bFreeSpectator	= m_pCkFreeSpectatorOn->GetCheck();
			RoomOpt.nMaxUsers		= m_OptionInfo[ROOM_OPT_PLAYERS].nValue;
			RoomOpt.nGameType		= (int)pMission->eGameType;			
			RoomOpt.nMapIndex		= pMission->nId;			
			RoomOpt.nTimeLimit		= pMission->nTimeLimit;
			RoomOpt.nKillLimit		= pMission->nKillLimit;
			RoomOpt.nRoundLimit		= pMission->nRoundLimit;
			
			this->SetDefaultVal( &RoomOpt );
		}
		break;
	}
	return 1;
}

LTBOOL CPopRoomOption::OnEnter()
{
	OnCommand( CMD_OK, LTNULL, LTNULL );
	return LTTRUE;
}

LTBOOL CPopRoomOption::OnEscape()
{
	OnCommand( CMD_CANCEL, LTNULL, LTNULL );
	return LTTRUE;
}

void CPopRoomOption::Render()
{
	if( IsVisible() == LTFALSE ) return;

	g_pDrawPrim->SetTexture( m_hBack );
	g_pDrawPrim->DrawPrim( &m_Poly );

	CLTGUIWindow::Render();

	m_OptionInfo[ROOM_OPT_PLAYERS].pD3DText->Render();
	m_OptionInfo[ROOM_OPT_TIME].pD3DText->Render();
	m_OptionInfo[ROOM_OPT_KILL].pD3DText->Render();
	m_OptionInfo[ROOM_OPT_ROUND].pD3DText->Render();

	m_pOptionList->Render();
	m_pMapList->Render();
}

void CPopRoomOption::ApplyOption()
{
	MISSION* pMission = g_pMissionButeMgr->GetMission(m_OptionInfo[ROOM_OPT_MAP_IDX].nValue);
	if( !pMission ) return;
		
	g_Network.CS_ChangeOption( g_GameDoc.GetRoomIndex(),
							   (int)pMission->eGameType,
							   pMission->nId,
							   int(m_OptionInfo[ROOM_OPT_PLAYERS].nValue * 0.5f),
							   m_OptionInfo[ROOM_OPT_TIME].nValue,
							   m_OptionInfo[ROOM_OPT_KILL].nValue,
							   m_OptionInfo[ROOM_OPT_ROUND].nValue,
							   m_pCkIntrusionOn->GetCheck(),
							   m_pCkTeamBanlanceOn->GetCheck(),
							   m_pCkFreeSpectatorOn->GetCheck() );

	if( strcmp( m_pEditRoomTitle->GetText(), g_GameDoc.GetTitle() ) != 0 )
	{
		g_Network.CS_ChangeRoomTitle( m_pEditRoomTitle->GetText() );
	}
}




CSaListOptions::CSaListOptions() 
:m_pTextList(LTNULL)
,CLTGUIListView() {}

CSaListOptions::~CSaListOptions()
{
	this->Term();
	SAFE_DELETE_ARRAY( m_pTextList );
}

LTBOOL CSaListOptions::Create( uint16 nPosX, uint16 nPosY,
							   uint16 nWidth, uint16 nHeight, uint16 nItemHeight, 
							   CLTGUICommandHandler* pCommandHandler,
							   LTBOOL bDrawBack, uint32 nBackColor, uint32 nBorderColor )
{
	CLTGUIListView::Create( nPosX, nPosY, nWidth, nHeight, nItemHeight,
							pCommandHandler, bDrawBack, nBackColor, nBorderColor );

	int i;
	m_pTextList = new CSAD3DText[m_nPageItemCount];

	for( i=0 ; i<m_nPageItemCount ; ++i )
	{
		m_pTextList[i].CreateD3DText( nWidth, 16, false );
		m_pTextList[i].SetTextColor( RGB(255,255,255), RGB(0,0,0) );
		m_pTextList[i].SetTransparent(true);
		m_pTextList[i].SetPosition( nPosX, nPosY+(i*m_nItemHeight)+4 );
		m_pTextList[i].DrawD3DText( "", 0 );
	}

	return  LTTRUE;
}

void CSaListOptions::RenderItemText()
{
	int				i, nIndex;
		
	if( m_nFirstItemIndex < 0 || m_nLastItemIndex < 0 ) return;
	for( i=m_nFirstItemIndex, nIndex=0 ; i<=m_nLastItemIndex ; ++i, ++nIndex )
	{
		T_ROOM_OPTIONSptr pItem = (T_ROOM_OPTIONSptr)m_ItemList[i];
		
		if( pItem )
		{
			m_pTextList[nIndex].DrawD3DText( pItem->szString, 
											 strlen(pItem->szString), 
											 DT_CENTER, D3D_TEXT_NORMAL, false );
		}
	}
}

LTBOOL CSaListOptions::RenderItem( void* pListItem, const LTIntPt* ptItemPos,
								int32 nIndex, bool bSelect, bool bOver )
{
	if( bSelect )
	{
		g_pDrawPrim->SetRGBA( &m_PolyBar, LTLV_DEFAULT_SEL_COLOR );
		g_pDrawPrim->SetXYWH( &m_PolyBar, (float)ptItemPos->x, (float)ptItemPos->y,
								(float)m_nWidth, (float)m_nItemHeight );
		g_pDrawPrim->SetTexture( LTNULL );
		g_pDrawPrim->DrawPrim(&m_PolyBar);
	}
	else if( bOver )
	{
		g_pDrawPrim->SetRGBA( &m_PolyBar, LTLV_DEFAULT_OVER_COLOR );
		g_pDrawPrim->SetXYWH( &m_PolyBar, (float)ptItemPos->x, (float)ptItemPos->y,
								(float)m_nWidth, (float)m_nItemHeight );
		g_pDrawPrim->SetTexture( LTNULL );
		g_pDrawPrim->DrawPrim(&m_PolyBar);
	}

	m_pTextList[nIndex].Render();
	return LTTRUE;
}

LTBOOL CSaListOptions::OnLButtonDown( int x, int y )
{
	if( CLTGUIListView::OnLButtonDown( x, y ) )
	{
		this->Show(LTFALSE);
		if( m_nPreIndex != m_nSelIndex ) m_pCommandHandler->SendCommand( CMD_ROOM_OPT_CHANGED, m_nCurType, LTNULL );
		return LTTRUE;
	}
	
	return LTFALSE;
}

const char* CSaListOptions::GetSelectedString()
{
	if( m_nSelIndex < 0 ) m_nSelIndex = GetLastIndex();

	T_ROOM_OPTIONSptr pItem = (T_ROOM_OPTIONSptr)m_ItemList[m_nSelIndex];	
	return pItem->szString;
}

int CSaListOptions::GetSelectedValue()
{
	if( m_nSelIndex < 0 ) m_nSelIndex = GetLastIndex();

	T_ROOM_OPTIONSptr pItem = (T_ROOM_OPTIONSptr)m_ItemList[m_nSelIndex];
	return pItem->nValue;
}

int CSaListOptions::GetType()
{
	return m_nCurType;
}

void CSaListOptions::ShowOption( int nType, int nValue )
{
	LTIntPt pt;
	pt.x = int(USABLE_WIDTH_F*0.5f-128.0f);
	pt.y = 85;

	m_nCurType	= nType;
	
	switch( m_nCurType )
	{
	case ROOM_OPT_PLAYERS:
		pt.x+=118; pt.y+=222;
		break;
	case ROOM_OPT_TIME:
		pt.x+=118; pt.y+=252;
		break;
	case ROOM_OPT_KILL:
		pt.x+=118; pt.y+=282;
		break;
	case ROOM_OPT_ROUND:
		pt.x+=118; pt.y+=312;
		break;
	}

	SetBasePos( pt );
	for( int i=0 ; i<m_nPageItemCount ; ++i )
	{
		m_pTextList[i].SetPosition( pt.x, pt.y+(i*m_nItemHeight)+4 );
	}
	
	SetDefaultItem( m_nCurType );
	SetBackPolySize();

	m_nSelIndex	= m_nPreIndex = GetSelFromValue(nValue);
	CLTGUIListView::Show(LTTRUE);
}

int CSaListOptions::GetSelFromValue( int nValue )
{
	for( int i=m_nFirstItemIndex, nIndex=0 ; i<=m_nLastItemIndex ; ++i, ++nIndex )
	{
		T_ROOM_OPTIONSptr pItem = (T_ROOM_OPTIONSptr)m_ItemList[i];
		
		if( pItem->nValue == nValue )
		{
			return i;
		}
	}

	return 0;
}

void CSaListOptions::SetDefaultItem( int nType )
{
	this->DeleteAllItem();

	switch( nType )
	{
	case ROOM_OPT_PLAYERS:
		AddItem( "1 vs 1", 2 );
		AddItem( "2 vs 2", 4 );
		AddItem( "3 vs 3", 6 );
		AddItem( "4 vs 4", 8 );
		AddItem( "5 vs 5", 10 );
		AddItem( "6 vs 6", 12 );
		AddItem( "7 vs 7", 14 );
		AddItem( "8 vs 8", 16 );
		break;
	case ROOM_OPT_TIME:
		AddItem( "5", 5 );
		AddItem( "10", 10 );
		AddItem( "15", 15 );
		AddItem( "20", 20 );
		AddItem( "25", 25 );
		AddItem( "30", 30 );
		AddItem( "35", 35 );
		AddItem( "40", 40 );		
		break;
	case ROOM_OPT_KILL:
		AddItem( "20", 20 );
		AddItem( "40", 40 );
		AddItem( "60", 60 );
		AddItem( "80", 80 );
		AddItem( "100", 100 );
		AddItem( "120", 120 );
		AddItem( "140", 140 );
		AddItem( "160", 160 );
		break;
	case ROOM_OPT_ROUND:
		AddItem( "3", 3 );
		AddItem( "5", 5 );
		AddItem( "7", 7 );
		AddItem( "9", 9 );
		//AddItem( "11", 11 );
		//AddItem( "13", 13 );
		//AddItem( "15", 15 );
		break;
	}

	RenderItemText();
}

void CSaListOptions::AddItem( const char* lpszString, int nValue )
{
	T_ROOM_OPTIONSptr pItem = new T_ROOM_OPTIONS;
	strcpy( pItem->szString, lpszString );
	pItem->nValue = nValue;

	InsertItem( (void *)pItem );
}







CSaListMap::CSaListMap() 
:m_hCheck(LTNULL)
,m_bLessType(LTFALSE)
,CLTGUIListView() {}

CSaListMap::~CSaListMap()
{
	// NOVICE : CLEAR !!!
	for( int i=0 ; i<m_nTotalItemCount ; ++i )
	{
		T_ROOM_OPTION_MAPptr pItem = (T_ROOM_OPTION_MAPptr)m_ItemList[i];

		SAFE_DELETE( pItem->pD3DName );
		SAFE_DELETE( pItem->pD3DType );
	}

	this->Term();
}

LTBOOL CSaListMap::Create( uint16 nPosX, uint16 nPosY,
						   uint16 nWidth, uint16 nHeight, uint16 nItemHeight,
						   CLTGUICommandHandler* pCommandHandler,
						   LTBOOL bDrawBack, uint32 nBackColor, uint32 nBorderColor )
{
	CLTGUIListView::Create( nPosX, nPosY, nWidth, nHeight, nItemHeight,
							pCommandHandler, bDrawBack, nBackColor, nBorderColor );

	int nMissionCount = g_pMissionButeMgr->GetNumMissions();
	MISSION* pMission;
	int i;

	for( i=0 ; i<nMissionCount ; ++i )
	{
		pMission	= g_pMissionButeMgr->GetMission(i);
		if( !pMission ) continue;	
		AddItem( pMission );
	}

	m_hCheck	= g_pInterfaceResMgr->GetTexture( "SA_INTERFACE\\Menu\\Textures\\Common\\TxChecked.dtx" );
	g_pDrawPrim->SetRGBA( &m_PolyCheck, 0xFFFFFFFF );
	g_pDrawPrim->SetUVWH( &m_PolyCheck, 0.0f, 0.0f, 1.0f, 1.0f );

	return  LTTRUE;
}

void CSaListMap::SetCurrentIndex( int nIndex )
{
	m_nSelIndex = m_nPreIndex	= nIndex;

	if( m_nSelIndex < m_nFirstItemIndex )
	{
		do
		{
			this->ScrollUp();
		}while( m_nSelIndex < m_nFirstItemIndex );
	}
	else if( m_nSelIndex > m_nLastItemIndex )
	{
		do
		{
			this->ScrollDown();
		}while( m_nSelIndex > m_nLastItemIndex );
	}
}

LTBOOL CSaListMap::RenderItem( void* pListItem, const LTIntPt* ptItemPos,
								int32 nIndex, bool bSelect, bool bOver )
{
	T_ROOM_OPTION_MAPptr pItem = (T_ROOM_OPTION_MAP*)pListItem;

	if( bSelect )
	{
		g_pDrawPrim->SetXYWH( &m_PolyCheck, ptItemPos->x-1.0f, ptItemPos->y+6.0f, 16.0f, 16.0f );
		g_pDrawPrim->SetTexture( m_hCheck );
		g_pDrawPrim->DrawPrim(&m_PolyCheck);
	}
	
	if( bOver )
	{
		pItem->pD3DName->SetTextColor( RGB(150,150,150) );
		pItem->pD3DType->SetTextColor( RGB(150,150,150) );
	}
	else
	{
		pItem->pD3DName->SetTextColor( RGB(255,255,255) );
		pItem->pD3DType->SetTextColor( RGB(255,255,255) );
	}

	pItem->pD3DName->SetPosition( ptItemPos->x+20, ptItemPos->y+6 );
	pItem->pD3DType->SetPosition( ptItemPos->x+95, ptItemPos->y+6 );

	pItem->pD3DName->Render();
	pItem->pD3DType->Render();

	return LTTRUE;
}

LTBOOL CSaListMap::OnLButtonDown( int x, int y )
{
	if( CLTGUIListView::OnLButtonDown( x, y ) )
	{
		T_ROOM_OPTION_MAPptr pItem = (T_ROOM_OPTION_MAP*)m_ItemList[m_nSelIndex];

		if( !pItem ) return LTFALSE;
		
		if( m_nPreIndex != m_nSelIndex )
		{
			m_pCommandHandler->SendCommand( CMD_ROOM_MAP_CHANGED, pItem->nMapIndex, LTNULL );
			m_nPreIndex = m_nSelIndex;
		}
		return LTTRUE;
	}
	
	return LTFALSE;
}

int CSaListMap::GetSelectedMapIndex()
{
	if( m_nSelIndex < 0 ) m_nSelIndex = GetLastIndex();

	T_ROOM_OPTION_MAPptr pItem = (T_ROOM_OPTION_MAP*)m_ItemList[m_nSelIndex];
	return pItem->nMapIndex;
}

void CSaListMap::AddItem( MISSION* pMission )
{
	T_ROOM_OPTION_MAPptr pItem	= new T_ROOM_OPTION_MAP;

	pItem->nGameType	= (int)pMission->eGameType;
	pItem->nMapIndex	= pMission->nId;

	pItem->pD3DName = new CSAD3DText();
	pItem->pD3DName->CreateD3DText( 128, 16, false );
	pItem->pD3DName->SetTextColor( RGB(255,255,255), RGB(0,0,0) );
	pItem->pD3DName->SetTransparent(true);
	pItem->pD3DName->DrawD3DText( NF_GetMsg(pMission->nNameId), 
								  strlen(NF_GetMsg(pMission->nNameId)),
								  DT_LEFT );

	pItem->pD3DType = new CSAD3DText();
	pItem->pD3DType->CreateD3DText( 128, 16, false );
	pItem->pD3DType->SetTextColor( RGB(255,255,255), RGB(0,0,0) );
	pItem->pD3DType->SetTransparent(true);
	pItem->pD3DType->DrawD3DText( NF_GetMsg(pMission->nGameTypeID), 
								  strlen(NF_GetMsg(pMission->nGameTypeID)),
								  DT_LEFT );

	InsertItem( (void *)pItem );
}

void CSaListMap::SortMapType()
{
	T_ROOM_OPTION_MAPptr pItem = (T_ROOM_OPTION_MAP*)m_ItemList[m_nSelIndex];
	int	nSelMapIdx = pItem->nMapIndex;

	if( m_bLessType )
	{
		std::sort( m_ItemList.begin(), m_ItemList.end(), GreatMapType() );		
	}
	else
	{
		std::sort( m_ItemList.begin(), m_ItemList.end(), LessMapType() );
	}
	
	m_bLessType	= !m_bLessType;

	for( int i=0 ; i<m_nTotalItemCount ; ++i )
	{
		pItem = (T_ROOM_OPTION_MAP*)m_ItemList[i];
		if( pItem->nMapIndex == nSelMapIdx )
		{
			m_nSelIndex	= i;
			m_nPreIndex	= -1;
			break;
		}
	}
}