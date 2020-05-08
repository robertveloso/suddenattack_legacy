#include "stdafx.h"
#include "PopupPassword.h"
#include "ScreenCommands.h"

#include "GameText.h"
#include "GameMessage.h"

CPopupPassword::CPopupPassword()
{
	m_pEditPassword	= LTNULL;	
	m_pBtnOK		= LTNULL;
	m_pBtnCancel	= LTNULL;
	m_hBack			= LTNULL;
}

CPopupPassword::~CPopupPassword()
{
}

void CPopupPassword::Init( CLTGUICommandHandler* pCommandHandler )
{
	LTIntPt BasePt;
	BasePt.x	= int(USABLE_WIDTH_F*0.5f-163.0f);
	BasePt.y	= 150;
	m_hBack = g_pInterfaceResMgr->GetTexture( "SA_INTERFACE\\Menu\\Textures\\DlgMain\\TxPasswordFrame.dtx" );
		
	Create( m_hBack, 512, 256, LTTRUE);
	SetBasePos(BasePt);
	Show(LTTRUE);

	m_pCommandHandler	= pCommandHandler;

	m_pEditPassword		= new CLTGUIEditBox();
	m_pEditPassword->Create( 0, 0, 235, 20, RGB(255,255,255), RGB(0,0,0), 0xFFFFFFFF,
							D3D_TEXT_NORMAL,
							LTTRUE,
							CMD_EDIT_PASSWORD, this );

	LTIntPt	pt;
	pt.x	= 40;
	pt.y	= 88;
	AddControl(m_pEditPassword,pt);
	
	m_pBtnOK	= new CLTGUIButton();
	AddButton( "ScreenMain", m_pBtnOK, CMD_OK, "BtnOKTxN", "BtnOKTxH", "BtnOKTxD", "BtnOKTxC", "BtnRectPWOk" );
	m_pBtnCancel	= new CLTGUIButton();
	AddButton( "ScreenMain", m_pBtnCancel, CMD_CANCEL, "BtnCancelTxN", "BtnCancelTxH", "BtnOKTxD", "BtnCancelTxC", "BtnRectPWCancel" );

	SetParentScreen( (CScreenBase*)pCommandHandler );
}

void CPopupPassword::OnFocus(LTBOOL bFocus)
{
	if( bFocus )
	{
		m_pEditPassword->ClearText();
		m_pEditPassword->ForceAddEditBox();
	}
	else
	{
		m_pEditPassword->OnLostFocus();
	}
}

uint32 CPopupPassword::OnCommand(uint32 dwCommand, uint32 dwParam1, uint32 dwParam2)
{
	switch(dwCommand)
	{
	case CMD_OK:
		g_GameDoc.SetPassword( m_pEditPassword->GetText() );
		m_pCommandHandler->SendCommand( CMD_JOIN_PRIVATE_ROOM, LTNULL, LTNULL );
		CtrlHide();
		break;
	case CMD_CANCEL:
		CtrlHide();
		break;
	case CMD_EDIT_PASSWORD:
		if( UIM_EDIT_ENTER == dwParam1 )
		{
			g_GameDoc.SetPassword( m_pEditPassword->GetText() );
			m_pCommandHandler->SendCommand( CMD_JOIN_PRIVATE_ROOM, LTNULL, LTNULL );
			CtrlHide();
		}
		break;
	}
	return 1;
}

LTBOOL CPopupPassword::OnEnter()
{
	OnCommand( CMD_EDIT_PASSWORD, UIM_EDIT_ENTER, LTNULL );
	return LTTRUE;
}

LTBOOL CPopupPassword::OnEscape()
{
	CtrlHide();
	return LTTRUE;
}

void CPopupPassword::Render()
{
	if( IsVisible() == LTFALSE ) return;
	CLTGUIWindow::Render();
}
