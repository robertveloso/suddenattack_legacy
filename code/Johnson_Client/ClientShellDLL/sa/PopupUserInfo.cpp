#include "stdafx.h"
#include "PopupUserInfo.h"
#include "ScreenCommands.h"
#include "ScreenMain.h"
#include "ScreenRoom.h"

#include "GameText.h"
#include "GameMessage.h"

CPopupUserInfo::CPopupUserInfo()
{
	m_pBtnOK		= LTNULL;
	m_pCkRedTeam	= LTNULL;
	m_pCkBlueTeam	= LTNULL;

	m_hBack			= LTNULL;

	m_hGradeIcon	= LTNULL;
	m_pCapText		= LTNULL;
	m_pInfoText		= LTNULL;
	m_pRedTeamText	= LTNULL;
	m_pBlueTeamText	= LTNULL;

	m_hCharObject	= LTNULL;
}

CPopupUserInfo::~CPopupUserInfo()
{
	SAFE_DELETE( m_pCapText	);
	SAFE_DELETE( m_pInfoText );
	SAFE_DELETE( m_pRedTeamText );
	SAFE_DELETE( m_pBlueTeamText );
}

void CPopupUserInfo::Init( CLTGUICommandHandler* pCommandHandler )
{
	LTIntPt BasePt;
	BasePt.x	= int(USABLE_WIDTH_F*0.5f-213.0f);
	BasePt.y	= int(USABLE_HEIGHT_F*0.5f-117.0f);
	m_hBack = g_pInterfaceResMgr->GetTexture( "SA_INTERFACE\\Menu\\Textures\\Common\\TxUserInfo.dtx" );
		
	Create( m_hBack, 512, 256, LTTRUE);
	SetBasePos(BasePt);
	Show(LTTRUE);

	m_pCommandHandler	= pCommandHandler;

	m_pBtnOK	= new CLTGUIButton();
	AddButton( "ScreenMain", m_pBtnOK, CMD_OK, "BtnOKTxN", "BtnOKTxH", "BtnOKTxD", "BtnOKTxC", "BtnRectUIOk" );

	m_pCkRedTeam = new CLTGUIButton;
	AddCheckButton( "ScreenMain", m_pCkRedTeam, CMD_UI_RED_TEAM, "CkRectRedTeam", "BtnCheckOff", "BtnCheckOn");
	m_pCkRedTeam->SetCheck(LTTRUE);
	m_pCkBlueTeam = new CLTGUIButton;
	AddCheckButton( "ScreenMain", m_pCkBlueTeam, CMD_UI_BLUE_TEAM, "CkRectBlueTeam", "BtnCheckOff", "BtnCheckOn");
	m_pCkBlueTeam->SetCheck(LTFALSE);

	g_pDrawPrim->SetXYWH( &m_PolyGrade, 0.0f, 0.0f, 16.0f, 16.0f );
	g_pDrawPrim->SetUVWH( &m_PolyGrade, 0.0f, 0.0f, 1.0f, 1.0f );	

	char szTemp[256];
	//"닉네임\r\n\r\n"  "계급\r\n\r\n"  "전적\r\n\r\n"  "Kill/Death\r\n\r\n"  "경험치\r\n\r\n"
	sprintf( szTemp, NF_GetMsg(SA_TEXT_USER_INFO_WHOLE_CAPTION) );

	m_pCapText	= new CSAD3DText();
	m_pCapText->CreateD3DText( 128, 128, true );
	m_pCapText->SetTextColor( RGB(255,255,255) );
	m_pCapText->SetPosition( BasePt.x+118, BasePt.y+43 );
	m_pCapText->SetTransparent(true);
	m_pCapText->DrawD3DText( szTemp, strlen(szTemp), DT_LEFT, D3D_TEXT_OUTLINE );

	m_pInfoText	= new CSAD3DText();
	m_pInfoText->CreateD3DText( 220, 128, true );
	m_pInfoText->SetTextColor( RGB(255,255,255) );
	m_pInfoText->SetPosition( BasePt.x+200, BasePt.y+43 );
	m_pInfoText->SetTransparent(true);

	m_pRedTeamText	= new CSAD3DText();
	m_pRedTeamText->CreateD3DText( 220, 16, false );
	m_pRedTeamText->SetTextColor( RGB(255,16,72) );
	m_pRedTeamText->SetPosition( BasePt.x+158, BasePt.y+162 );
	m_pRedTeamText->SetTransparent(true);
	m_pRedTeamText->DrawD3DText( "RED TEAM", strlen("RED TEAM"), DT_LEFT, D3D_TEXT_OUTLINE );

	m_pBlueTeamText	= new CSAD3DText();
	m_pBlueTeamText->CreateD3DText( 220, 16, false );
	m_pBlueTeamText->SetTextColor( RGB(66,142,255) );
	m_pBlueTeamText->SetPosition( BasePt.x+270, BasePt.y+162 );
	m_pBlueTeamText->SetTransparent(true);
	m_pBlueTeamText->DrawD3DText( "BLUE TEAM", strlen("BLUE TEAM"), DT_LEFT, D3D_TEXT_OUTLINE );

	m_rcRedTeam.left	= 346;
	m_rcRedTeam.top		= 344;
	m_rcRedTeam.right	= 414;
	m_rcRedTeam.bottom	= 361;

	m_rcBlueTeam.left	= 458;
	m_rcBlueTeam.top	= 342;
	m_rcBlueTeam.right	= 533;
	m_rcBlueTeam.bottom	= 361;
	
	SetParentScreen( (CScreenBase*)pCommandHandler );
}

void CPopupUserInfo::SetUserInfo( const char* lpszNick, uint32 nGrade,
								  uint32 nKill, uint32 nDeath,
								  uint32 nWin, uint32 nLose, uint32 nDraw,
								  __int64 nExp,
								  uint8 nA_ModelID, uint8 nA_HeadSkinID, uint8 nA_BodySkinID,
								  uint8 nB_ModelID, uint8 nB_HeadSkinID, uint8 nB_BodySkinID )
{
	char	szExp[64];

	int	 nGameCount			= nWin + nLose + nDraw;
	int	 nWinPercent		= (nGameCount<=0) ? 0 : (int)( (float)nWin * 100.0f / (float)nGameCount );
	int  nKillDeathCount	= nKill + nDeath;
	int	 nKillPercent		= (nKillDeathCount<=0) ? 0 : (int)( (float)nKill * 100 / (float)nKillDeathCount);

	_i64toa( nExp, szExp, 10 );

	//"%s\r\n\r\n"  "%s\r\n\r\n"  "%d전 %d승 %d패 (%d%%)\r\n\r\n"  "%d / %d\r\n\r\n"  "%s"
	sprintf( m_szTemp, NF_GetMsg(SA_TEXT_USER_INFO_WHOLE_FORM),
					 lpszNick,
					 g_Global_Data.GetGradeString( nGrade ),
					 nGameCount, nWin, nLose, nWinPercent,
					 nKill, nDeath,
					 szExp );
	m_pInfoText->DrawD3DText( m_szTemp, strlen(m_szTemp), DT_LEFT, D3D_TEXT_SHADOW );

	m_hGradeIcon = g_Global_Data.GetGradeTexture( nGrade );

	m_nA_ModelID		= nA_ModelID;
	m_nA_HeadSkinID		= nA_HeadSkinID;
	m_nA_BodySkinID		= nA_BodySkinID;
	m_nB_ModelID		= nB_ModelID;
	m_nB_HeadSkinID		= nB_HeadSkinID;
	m_nB_BodySkinID		= nB_BodySkinID;
}

void CPopupUserInfo::OnFocus(LTBOOL bFocus)
{
	if( bFocus )
	{
		m_pCkRedTeam->SetCheck(LTTRUE);
		m_pCkBlueTeam->SetCheck(LTFALSE);
		UpdateModel( m_nA_ModelID, m_nA_HeadSkinID, m_nA_BodySkinID );		
	}
	else
	{
		g_pLTClient->Common()->SetObjectFlags(m_hCharObject, OFT_Flags, 0, FLAG_VISIBLE );
	}
}

uint32 CPopupUserInfo::OnCommand(uint32 dwCommand, uint32 dwParam1, uint32 dwParam2)
{
	switch(dwCommand)
	{
	case CMD_OK:
		CtrlHide();
		break;

	case CMD_UI_RED_TEAM:
		if( LTFALSE == m_pCkRedTeam->IsClicked() )
		{
			m_pCkRedTeam->SetCheck(LTTRUE);
			m_pCkBlueTeam->SetCheck(LTFALSE);
			UpdateModel( m_nA_ModelID, m_nA_HeadSkinID, m_nA_BodySkinID );
		}
		break;

	case CMD_UI_BLUE_TEAM:
		if( LTFALSE == m_pCkBlueTeam->IsClicked() )
		{
			m_pCkRedTeam->SetCheck(LTFALSE);
			m_pCkBlueTeam->SetCheck(LTTRUE);
			UpdateModel( m_nB_ModelID, m_nB_HeadSkinID, m_nB_BodySkinID );
		}
		break;
	}
	return 1;
}

LTBOOL CPopupUserInfo::OnEnter()
{
	OnCommand( CMD_OK, LTNULL, LTNULL );
	return LTTRUE;
}

LTBOOL CPopupUserInfo::OnEscape()
{
	OnCommand( CMD_OK, LTNULL, LTNULL );
	return LTTRUE;
}

void CPopupUserInfo::Render()
{
	if( IsVisible() == LTFALSE ) return;
	CLTGUIWindow::Render();
	
	m_pCapText->Render();
	m_pInfoText->Render();
	m_pRedTeamText->Render();
	m_pBlueTeamText->Render();
	
	if( m_hGradeIcon )
	{
		g_pDrawPrim->SetTexture( m_hGradeIcon );
		g_pDrawPrim->DrawPrim( &m_PolyGrade );
	}
}

LTBOOL CPopupUserInfo::OnLButtonDown( int x, int y )
{
	if( x >= m_rcRedTeam.left && y >= m_rcRedTeam.top &&
		x <= m_rcRedTeam.right && y <= m_rcRedTeam.bottom )
	{
		g_pInterfaceMgr->RequestInterfaceSound( IS_CLICK_RADIO );
		this->OnCommand( CMD_UI_RED_TEAM, LTNULL, LTNULL );
		return LTTRUE;
	}
	else if( x >= m_rcBlueTeam.left && y >= m_rcBlueTeam.top &&
			 x <= m_rcBlueTeam.right && y <= m_rcBlueTeam.bottom )
	{
		g_pInterfaceMgr->RequestInterfaceSound( IS_CLICK_RADIO );
		this->OnCommand( CMD_UI_BLUE_TEAM, LTNULL, LTNULL );
		return LTTRUE;
	}

	return CPopScreenBase::OnLButtonDown( x, y );
}

void CPopupUserInfo::UpdateModel( int nModelID, int HeadSkinID, int nBodySkinID )
{
	switch( g_pInterfaceMgr->GetScreenMgr()->GetCurrentScreenID() )
	{
		case SCREEN_ID_MAIN :
			g_pScreenMain->UpdateModel(nModelID, HeadSkinID,  nBodySkinID);
			break;
		case SCREEN_ID_SCREEN_ROOM :
			g_pScreenRoom->UpdateModel(nModelID, HeadSkinID,  nBodySkinID);
			break;
	}
	
	g_pLTClient->Common()->SetObjectFlags(m_hCharObject, OFT_Flags, FLAG_VISIBLE, FLAG_VISIBLE );	
}