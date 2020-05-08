#include "stdafx.h"
#include "PopupSearchUser.h"
#include "ScreenCommands.h"

#include "GameText.h"
#include "GameMessage.h"

CPopupSearchUser::CPopupSearchUser()
{
	m_pEditNickName	= LTNULL;	
	m_pBtnOK		= LTNULL;
	m_pBtnCancel	= LTNULL;
	m_hBack			= LTNULL;
}

CPopupSearchUser::~CPopupSearchUser()
{
}

void CPopupSearchUser::Init( CLTGUICommandHandler* pCommandHandler )
{
	LTIntPt BasePt;
	BasePt.x	= int(USABLE_WIDTH_F*0.5f-163.0f);
	BasePt.y	= int(USABLE_HEIGHT_F*0.5f-98.0f);
	m_hBack = g_pInterfaceResMgr->GetTexture( "SA_INTERFACE\\Menu\\Textures\\Common\\TxSearchUser.dtx" );
		
	Create( m_hBack, 512, 256, LTTRUE);
	SetBasePos(BasePt);
	Show(LTTRUE);

	m_pCommandHandler	= pCommandHandler;

	m_pEditNickName		= new CLTGUIEditBox();
	m_pEditNickName->Create( 0, 0, 240, 20, RGB(255,255,255), RGB(0,0,0), 0xFFFFFFFF,
							D3D_TEXT_NORMAL,
							LTFALSE,
							CMD_EDIT_NICKNAME, this );

	LTIntPt	pt;
	pt.x	= 40;
	pt.y	= 88;
	AddControl(m_pEditNickName,pt);
	
	m_pBtnOK	= new CLTGUIButton();
	AddButton( "ScreenMain", m_pBtnOK, CMD_OK, "BtnOKTxN", "BtnOKTxH", "BtnOKTxD", "BtnOKTxC", "BtnRectSUOk" );

	m_pBtnCancel	= new CLTGUIButton();
	AddButton( "ScreenMain", m_pBtnCancel, CMD_CANCEL, "BtnCancelTxN", "BtnCancelTxH", "BtnCancelTxD", "BtnCancelTxC", "BtnRectSUCancel" );

	SetParentScreen( (CScreenBase*)pCommandHandler );
}

void CPopupSearchUser::OnFocus(LTBOOL bFocus)
{
	if( bFocus )
	{
		m_pEditNickName->ClearText();
		m_pEditNickName->ForceAddEditBox();
	}
	else
	{
		m_pEditNickName->OnLostFocus();
	}
}

uint32 CPopupSearchUser::OnCommand(uint32 dwCommand, uint32 dwParam1, uint32 dwParam2)
{
	switch(dwCommand)
	{
	case CMD_OK:
		if( SendSearchUser() ) CtrlHide();
		break;
	case CMD_CANCEL:
		CtrlHide();
		break;
	case CMD_EDIT_NICKNAME:
		if( UIM_EDIT_ENTER == dwParam1 )
		{
			if( SendSearchUser() ) CtrlHide();
		}
		break;
	}
	return 1;
}

LTBOOL CPopupSearchUser::OnEnter()
{
	OnCommand( CMD_OK, LTNULL, LTNULL );
	return LTTRUE;
}

LTBOOL CPopupSearchUser::OnEscape()
{
	OnCommand( CMD_CANCEL, LTNULL, LTNULL );
	return LTTRUE;
}

void CPopupSearchUser::Render()
{
	if( IsVisible() == LTFALSE ) return;
	CLTGUIWindow::Render();
}

LTBOOL CPopupSearchUser::SendSearchUser()
{
	if( strlen(m_pEditNickName->GetText())<=0 )
	{
		return LTFALSE;
	}

	g_Network.CS_SearchUser( m_pEditNickName->GetText() );
	return LTTRUE;
}