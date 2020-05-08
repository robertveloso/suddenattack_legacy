#include "stdafx.h"
#include "PopupNickName.h"
#include "ScreenCommands.h"
#include "CurseFilter.h"

#include "GameText.h"
#include "GameMessage.h"

namespace
{
	void NickNameCallBack(LTBOOL bReturn, void *pData)
	{
		CPopupNickName *pThisScreen = (CPopupNickName *)pData;
		if( pThisScreen) pThisScreen->ForceAddEditBox();
	};
}

CPopupNickName::CPopupNickName()
{
	m_pEditNickName	= LTNULL;	
	m_pBtnOK		= LTNULL;
	m_hBack			= LTNULL;
}

CPopupNickName::~CPopupNickName()
{
}

void CPopupNickName::Init( CLTGUICommandHandler* pCommandHandler )
{
	LTIntPt BasePt;
	BasePt.x	= int(USABLE_WIDTH_F*0.5f-163.0f);
	BasePt.y	= int(USABLE_HEIGHT_F*0.5f-98.0f);
	m_hBack = g_pInterfaceResMgr->GetTexture( "SA_INTERFACE\\Menu\\Textures\\Channel\\TxNickBack.dtx" );
		
	Create( m_hBack, 512, 256, LTTRUE);
	SetBasePos(BasePt);
	Show(LTTRUE);

	m_pCommandHandler	= pCommandHandler;

	m_pEditNickName		= new CLTGUIEditBox();
	m_pEditNickName->Create( 0, 0, 170, 20, RGB(255,255,255), RGB(0,0,0), 0xFFFFFFFF,
							D3D_TEXT_NORMAL,
							LTFALSE,
							CMD_EDIT_NICKNAME, this );

	LTIntPt	pt;
	pt.x	= 26;
	pt.y	= 56;
	AddControl(m_pEditNickName,pt);
	
	m_pBtnOK	= new CLTGUIButton();
	AddButton( "ScreenCharacter", m_pBtnOK, CMD_OK, "BtnOKTxN", "BtnOKTxH", "BtnOKTxD", "BtnOKTxC", "BtnRectOk" );

	SetParentScreen( (CScreenBase*)pCommandHandler );
}

void CPopupNickName::OnFocus(LTBOOL bFocus)
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

uint32 CPopupNickName::OnCommand(uint32 dwCommand, uint32 dwParam1, uint32 dwParam2)
{
	switch(dwCommand)
	{
	case CMD_OK:
		if( SendNickName() ) CtrlHide();
		break;
	case CMD_EDIT_NICKNAME:
		if( UIM_EDIT_ENTER == dwParam1 )
		{
			if( SendNickName() ) CtrlHide();
		}
		break;
	}
	return 1;
}

LTBOOL CPopupNickName::OnEnter()
{
	OnCommand( CMD_EDIT_NICKNAME, UIM_EDIT_ENTER, LTNULL );
	return LTTRUE;
}

LTBOOL CPopupNickName::OnEscape()
{
	return LTTRUE;
}

void CPopupNickName::Render()
{
	if( IsVisible() == LTFALSE ) return;
	CLTGUIWindow::Render();
}

LTBOOL CPopupNickName::SendNickName()
{
	if( strlen(m_pEditNickName->GetText()) < MIN_NICK_LEN )
	{
		m_pEditNickName->OnLostFocus();
		// "닉네임은 최소 영문 4글자 혹은\r\n\r\n한글 2글자 이상이어야 합니다."
		g_pInterfaceMgr->ShowMessageBox( SA_TEXT_HELP_NICK_MIN_LENGTH,
										 LTMB_OK, NickNameCallBack, this );
		m_pEditNickName->ClearText();
		return LTFALSE;
	}
	
	if( strlen(m_pEditNickName->GetText()) > 16 )
	{
		m_pEditNickName->OnLostFocus();
		//닉네임의 최대길이를 벗어 났습니다.
		g_pInterfaceMgr->ShowMessageBox( SA_TEXT_NICK_LEN_ERROR,
										 LTMB_OK, NickNameCallBack, this );
		m_pEditNickName->ClearText();
		return LTFALSE;
	}

	if( LTTRUE == IsCurse(m_pEditNickName->GetText()) )
	{
		m_pEditNickName->OnLostFocus();
		// "금지된 문구나 욕설은 사용할 수 없습니다."
		g_pInterfaceMgr->ShowMessageBox( SA_TEXT_HELP_CANNOT_USE_CURSE, 
										  LTMB_OK, NickNameCallBack, this );
		m_pEditNickName->ClearText();
		return LTFALSE;
	}
	
	if( LTTRUE == m_pEditNickName->CheckEmptySpace() )
	{
		m_pEditNickName->OnLostFocus();

		g_pInterfaceMgr->ShowMessageBox( SA_TEXT_NICK_ERROR, 
										 LTMB_OK, NickNameCallBack, this );
		m_pEditNickName->ClearText();
		return LTFALSE;
	}

	// "닉네임을 생성 중 입니다.\r\n\r\n잠시만 기다려 주십시오."
	g_pInterfaceMgr->ShowMessageBox( SA_TEXT_NOW_ON_NICK_MAKE,
									  LTMB_MESSAGE, LTNULL, LTNULL );

	g_Network.CS_FirstNickName( m_pEditNickName->GetText() );
	return LTTRUE;
}

void CPopupNickName::ForceAddEditBox()
{
	m_pEditNickName->ForceAddEditBox();
}
