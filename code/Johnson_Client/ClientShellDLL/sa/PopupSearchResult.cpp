#include "stdafx.h"
#include "PopupSearchResult.h"
#include "ScreenCommands.h"

#include "GameText.h"
#include "GameMessage.h"

CPopupSearchResult::CPopupSearchResult()
{
	m_pBtnOK		= LTNULL;
	m_pBtnJoin		= LTNULL;
	m_pBtnAdd		= LTNULL;
	m_hBack			= LTNULL;
	m_pText			= LTNULL;
}

CPopupSearchResult::~CPopupSearchResult()
{
	SAFE_DELETE( m_pText );
}

void CPopupSearchResult::Init( CLTGUICommandHandler* pCommandHandler )
{
	LTIntPt BasePt;
	BasePt.x	= int(USABLE_WIDTH_F*0.5f-163.0f);
	BasePt.y	= int(USABLE_HEIGHT_F*0.5f-98.0f);
	m_hBack = g_pInterfaceResMgr->GetTexture( "SA_INTERFACE\\Menu\\Textures\\Common\\TxDefaultMsgBox.dtx" );
		
	Create( m_hBack, 512, 256, LTTRUE);
	SetBasePos(BasePt);
	Show(LTTRUE);

	m_pCommandHandler	= pCommandHandler;

	m_pBtnOK	= new CLTGUIButton();
	AddButton( "ScreenMain", m_pBtnOK, CMD_OK, "BtnOKTxN", "BtnOKTxH", "BtnOKTxD", "BtnOKTxC", "BtnRectSROk" );

	m_pBtnJoin	= new CLTGUIButton();
	AddButton( "ScreenMain", m_pBtnJoin, CMD_JOIN, "BtnJoinTxN", "BtnJoinTxH", "BtnJoinTxD", "BtnJoinTxC", "BtnRectSRJoin" );

	m_pBtnAdd	= new CLTGUIButton();
	AddButton( "ScreenMain", m_pBtnAdd, CMD_ADD, "BtnAddTxN", "BtnAddTxH", "BtnAddTxD", "BtnAddTxC", "BtnRectSRAdd" );
	
	m_pText	= new CSAD3DText();
	m_pText->CreateD3DText( 256, 36, true );
	m_pText->SetTextColor( RGB(255,255,255) );
	m_pText->SetPosition( BasePt.x+30, BasePt.y+41 );
	m_pText->SetTransparent(true);
	
	SetParentScreen( (CScreenBase*)pCommandHandler );

	m_nBtnPosY		= m_pos.y+135;

	m_nBtnPos_1_1	= m_pos.x+120;

	m_nBtnPos_2_1	= m_pos.x+71;
	m_nBtnPos_2_2	= m_pos.x+167;

	m_nBtnPos_3_1	= m_pos.x+30;
	m_nBtnPos_3_2	= m_pos.x+120;
	m_nBtnPos_3_3	= m_pos.x+210;
}

void CPopupSearchResult::CtrlShow( LTBOOL bSuccess, const char* lpszMsg, int nChannelIndex, int nRoomIndex, LTBOOL bScreenMain )
{
	m_pBtnAdd->Show(LTFALSE);
	m_pBtnJoin->Show(LTFALSE);

	if( bSuccess )
	{
		if( !bScreenMain )
		{
			m_pBtnOK->SetBasePos( m_nBtnPos_1_1, m_nBtnPosY );
		}
		else
		{
			if( nChannelIndex >= 0 || nRoomIndex >= 0 )
			{
				m_nChannelIndex	= nChannelIndex;
				m_nRoomIndex	= nRoomIndex;

				m_pBtnJoin->Show(LTTRUE);
				m_pBtnJoin->SetBasePos( m_nBtnPos_2_1, m_nBtnPosY );
				m_pBtnOK->SetBasePos( m_nBtnPos_2_2, m_nBtnPosY );
			}
			else
			{
				m_nChannelIndex = -1;
				m_nRoomIndex	= -1;

				m_pBtnOK->SetBasePos( m_nBtnPos_1_1, m_nBtnPosY );
			}
		}
	}
	else
	{
		m_pBtnOK->SetBasePos( m_nBtnPos_1_1, m_nBtnPosY );
	}

	m_pText->DrawD3DText( lpszMsg, strlen(lpszMsg), DT_LEFT, D3D_TEXT_SHADOW );

	CPopScreenBase::CtrlShow();
}

void CPopupSearchResult::OnFocus(LTBOOL bFocus)
{
}

uint32 CPopupSearchResult::OnCommand(uint32 dwCommand, uint32 dwParam1, uint32 dwParam2)
{
	switch(dwCommand)
	{
	case CMD_OK:
		CtrlHide();
		break;

	case CMD_ADD:
		CtrlHide();
		break;
		
	case CMD_JOIN:
		CtrlHide();
		m_pCommandHandler->SendCommand( CMD_JOIN_ROOM, m_nChannelIndex, m_nRoomIndex );
		break;
	}
	return 1;
}

LTBOOL CPopupSearchResult::OnEnter()
{
	OnCommand( CMD_OK, LTNULL, LTNULL );
	return LTTRUE;
}

LTBOOL CPopupSearchResult::OnEscape()
{
	OnCommand( CMD_OK, LTNULL, LTNULL );
	return LTTRUE;
}

void CPopupSearchResult::Render()
{
	if( IsVisible() == LTFALSE ) return;
	CLTGUIWindow::Render();
	m_pText->Render();
}
