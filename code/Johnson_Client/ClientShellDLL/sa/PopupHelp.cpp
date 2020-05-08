//-------------------------------------------------------------------
// FILE : PopupHelp.cpp
// 대기실(ScreenMain)의 도움말 버튼을 눌렀을 때 나오는 도움말 팝업.

#include "stdafx.h"
#include "PopupHelp.h"
#include "ScreenCommands.h"

CPopupHelp::CPopupHelp()
{
	for(int i = 0; i < _HELPPOPUP_SUBBTN_; ++i)
		m_hHelpFrame[i] = LTNULL;

	m_pBtnOK					= LTNULL;

	m_pTabBtnHelpInterface		= LTNULL;
	m_pTabBtnHelpGamePlay		= LTNULL;

	for(i = 0; i < _HELPPOPUP_SUBBTN_; ++i)
		m_pSubBtn[i] = new CLTGUIButton;

	m_eSubBtnState				= HLPCMD_MOVEMENT; //서브 버튼 기본 설정(탭은 인터페이스)
}

CPopupHelp::~CPopupHelp()
{
}

void CPopupHelp::Init( CLTGUICommandHandler * pCommandHandler )
{
	//---------------------------------------------------------------
	//기본 도움말 생성
	LTIntPt BasePt;
	BasePt.x	= int(USABLE_WIDTH_F*0.5f-200.0f); //화면 중앙
	BasePt.y	= 50;

	//pScreenTag = "ScreenMain"
	char * pScreenTag = (char*)g_pInterfaceMgr->GetScreenMgr()->GetScreenName((eScreenID)SCREEN_ID_MAIN);

	//배경으로 쓰일 텍스처 읽기
	m_hHelpFrame[0] = g_pInterfaceResMgr->GetTexture( "SA_INTERFACE\\menu\\Textures\\help\\TxHelpBase.dtx" );
	m_hHelpFrame[1] = g_pInterfaceResMgr->GetTexture( "SA_INTERFACE\\menu\\Textures\\help\\FmAction.dtx" );
	m_hHelpFrame[2] = g_pInterfaceResMgr->GetTexture( "SA_INTERFACE\\menu\\Textures\\help\\FmChat.dtx" );
	m_hHelpFrame[3] = g_pInterfaceResMgr->GetTexture( "SA_INTERFACE\\menu\\Textures\\help\\FmHotkey.dtx" );
	m_hHelpFrame[4] = g_pInterfaceResMgr->GetTexture( "SA_INTERFACE\\menu\\Textures\\help\\FmBasic.dtx" );
	m_hHelpFrame[5] = g_pInterfaceResMgr->GetTexture( "SA_INTERFACE\\menu\\Textures\\help\\FmMission.dtx" );

	//배경 만들기 - 초기값은...
	Create( m_hHelpFrame[0], 512, 512, LTTRUE);
	SetBasePos(BasePt);
	Show(LTTRUE);

	//---------------------------------------------------------------
	//버튼 설정
	//basic button
	m_pBtnOK	= new CLTGUIButton();
	AddButton( pScreenTag, m_pBtnOK, CMD_OK, "BtnOKTxN", "BtnOKTxH", "BtnOKTxD", "BtnOKTxC", "BtnRectHLPOk" );

	//tab button
	m_pTabBtnHelpInterface = new CLTGUIButton;
	AddCheckButton( pScreenTag , m_pTabBtnHelpInterface, HLPCMD_TAB_INTERFACE,"TabBtnITFRect", "TabBtnITFD", "TabBtnITFH");
	m_pTabBtnHelpGamePlay = new CLTGUIButton;
	AddCheckButton( pScreenTag , m_pTabBtnHelpGamePlay, HLPCMD_TAB_GAMEPLAY,"TabBtnGPRect", "TabBtnGPD", "TabBtnGPH");

	//sub button
	AddButton( pScreenTag, m_pSubBtn[0], HLPCMD_MOVEMENT, "BtnMoveTxN", "BtnMoveTxN", "BtnMoveTxN", "BtnMoveTxH", "BtnRectMove" );
	AddButton( pScreenTag, m_pSubBtn[1], HLPCMD_ACTION, "BtnActTxN", "BtnActTxN", "BtnActTxN", "BtnActTxH", "BtnRectAct" );
	AddButton( pScreenTag, m_pSubBtn[2], HLPCMD_CHAT, "BtnChatTxN", "BtnChatTxN", "BtnChatTxN", "BtnChatTxH", "BtnRectChat" );
	AddButton( pScreenTag, m_pSubBtn[3], HLPCMD_HOTKEY, "BtnHotkeyTxN", "BtnHotkeyTxN", "BtnHotkeyTxN", "BtnHotkeyTxH", "BtnRectHotkey" );
	AddButton( pScreenTag, m_pSubBtn[4], HLPCMD_BASIC, "BtnBasicTxN", "BtnBasicTxN", "BtnBasicTxN", "BtnBasicTxH", "BtnRectBasic" );
	AddButton( pScreenTag, m_pSubBtn[5], HLPCMD_MISSION, "BtnMissionTxN", "BtnMissionTxN", "BtnMissionTxN", "BtnMissionTxH", "BtnRectMission" );

	//버튼을 체크된 상태로 둘 수 있게 한다.
	//가능 조건 : (LTTRUE == m_bChecked && LTNULL != m_hChecked)
	//기능 : m_hChecked = m_hClick; 하여 Checked에 내용을 넣어줌.
	for(int i = 0; i < _HELPPOPUP_SUBBTN_; ++i)
	{
		m_pSubBtn[i]->SetCheckedTexture();
		m_pSubBtn[i]->SetClickSound( (int) IS_CLICK_FLAT );
	}

	// button sound
	m_pTabBtnHelpInterface->SetClickSound( (int)IS_CLICK_FLAT );
	m_pTabBtnHelpGamePlay->SetClickSound( (int)IS_CLICK_FLAT );

	//---------------------------------------------------------------
	//부모 설정
	//부모를 설정해야 팝업이 뜰 때 부모가 반투명 됨.
	m_pCommandHandler	= pCommandHandler;

	SetParentScreen( (CScreenBase*)pCommandHandler );
}

void CPopupHelp::Render()
{
	if( IsVisible() == LTFALSE ) return;

	//현재 배경 정하기
	RenderBackground(m_eSubBtnState);

	CLTGUIWindow::Render();
}

void CPopupHelp::RenderBackground(HELPPOPUPCOMMAND eState)
{
	m_Frame.SetFrame(m_hHelpFrame[eState - HLPCMD_MOVEMENT], LTFALSE);
}

void CPopupHelp::OnFocus(LTBOOL bFocus)
{
	if( bFocus )
	{
		HelpFrameSetting(HLPCMD_MOVEMENT);
		m_eSubBtnState = HLPCMD_MOVEMENT;
	}
	else
	{
		//--
	}
}

LTBOOL CPopupHelp::OnEnter()
{
	OnCommand( CMD_OK, LTNULL, LTNULL );
	return LTTRUE;
}

LTBOOL CPopupHelp::OnEscape()
{
	OnCommand( CMD_CANCEL, LTNULL, LTNULL );
	return LTTRUE;
}

uint32 CPopupHelp::OnCommand(uint32 dwCommand, uint32 dwParam1, uint32 dwParam2)
{
	switch(dwCommand)
	{
	case HLPCMD_TAB_INTERFACE:
		if(HLPCMD_MOVEMENT == m_eSubBtnState || HLPCMD_ACTION == m_eSubBtnState ||
			HLPCMD_CHAT == m_eSubBtnState || HLPCMD_HOTKEY == m_eSubBtnState) break;
		HelpFrameSetting(HLPCMD_MOVEMENT);
		m_eSubBtnState = HLPCMD_MOVEMENT;
		break;
	case HLPCMD_TAB_GAMEPLAY:
		if(HLPCMD_BASIC == m_eSubBtnState || HLPCMD_MISSION == m_eSubBtnState) break;
		HelpFrameSetting(HLPCMD_BASIC);
		m_eSubBtnState = HLPCMD_BASIC;
		break;

	case HLPCMD_MOVEMENT:
		HelpFrameSetting(HLPCMD_MOVEMENT);
		m_eSubBtnState = HLPCMD_MOVEMENT;
		break;
	case HLPCMD_ACTION:
		HelpFrameSetting(HLPCMD_ACTION);
		m_eSubBtnState = HLPCMD_ACTION;
		break;
	case HLPCMD_CHAT:
		HelpFrameSetting(HLPCMD_CHAT);
		m_eSubBtnState = HLPCMD_CHAT;
		break;
	case HLPCMD_HOTKEY:
		HelpFrameSetting(HLPCMD_HOTKEY);
		m_eSubBtnState = HLPCMD_HOTKEY;
		break;
	case HLPCMD_BASIC:
		HelpFrameSetting(HLPCMD_BASIC);
		m_eSubBtnState = HLPCMD_BASIC;
		break;
	case HLPCMD_MISSION:
		HelpFrameSetting(HLPCMD_MISSION);
		m_eSubBtnState = HLPCMD_MISSION;
		break;

	case CMD_OK: //엔터를 누르거나, 버튼이 눌리면 되게함
	case CMD_CANCEL:
		CtrlHide();
		break;
	}
	return 1;
}

void CPopupHelp::HelpFrameSetting(uint8 iSelect)
{
	//일단 초기화
	m_pTabBtnHelpInterface->SetCheck(LTFALSE);
	m_pTabBtnHelpGamePlay->SetCheck(LTFALSE);

	for(int i = 0; i < _HELPPOPUP_SUBBTN_; ++i)
	{
		m_pSubBtn[i]->Show(LTFALSE);		//같은 자리 버튼이 겹쳐서 안 보이게.
		m_pSubBtn[i]->SetCheck(LTFALSE);	//체크된 상태 아니게.
	}

	switch(iSelect) //살릴 것만 확인
	{
	case HLPCMD_MOVEMENT:
		{
			m_pTabBtnHelpInterface->SetCheck(LTTRUE);

			m_pSubBtn[0]->Show(LTTRUE);	//안 보이면 클릭도 안된다.
			m_pSubBtn[1]->Show(LTTRUE);
			m_pSubBtn[2]->Show(LTTRUE);
			m_pSubBtn[3]->Show(LTTRUE);

			m_pSubBtn[0]->SetCheck();	//단지 체크된 그림을 위함.
		}
		break;
	case HLPCMD_ACTION:
		{
			m_pTabBtnHelpInterface->SetCheck(LTTRUE);

			m_pSubBtn[0]->Show(LTTRUE);
			m_pSubBtn[1]->Show(LTTRUE);
			m_pSubBtn[2]->Show(LTTRUE);
			m_pSubBtn[3]->Show(LTTRUE);

			m_pSubBtn[1]->SetCheck();
		}
		break;
	case HLPCMD_CHAT:
		{
			m_pTabBtnHelpInterface->SetCheck(LTTRUE);

			m_pSubBtn[0]->Show(LTTRUE);
			m_pSubBtn[1]->Show(LTTRUE);
			m_pSubBtn[2]->Show(LTTRUE);
			m_pSubBtn[3]->Show(LTTRUE);

			m_pSubBtn[2]->SetCheck();
		}
		break;
	case HLPCMD_HOTKEY:
		{
			m_pTabBtnHelpInterface->SetCheck(LTTRUE);

			m_pSubBtn[0]->Show(LTTRUE);
			m_pSubBtn[1]->Show(LTTRUE);
			m_pSubBtn[2]->Show(LTTRUE);
			m_pSubBtn[3]->Show(LTTRUE);

			m_pSubBtn[3]->SetCheck();
		}
		break;

	case HLPCMD_BASIC:
		{
			m_pTabBtnHelpGamePlay->SetCheck(LTTRUE);

			m_pSubBtn[4]->Show(LTTRUE);
			m_pSubBtn[5]->Show(LTTRUE);

			m_pSubBtn[4]->SetCheck();
		}
		break;
	case HLPCMD_MISSION:
		{
			m_pTabBtnHelpGamePlay->SetCheck(LTTRUE);

			m_pSubBtn[4]->Show(LTTRUE);
			m_pSubBtn[5]->Show(LTTRUE);

			m_pSubBtn[5]->SetCheck();
		}
		break;
	}
}