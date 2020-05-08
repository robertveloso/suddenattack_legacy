#include "stdafx.h"
#include "ScreenChannel.h"
#include "ScreenMgr.h"
#include "ScreenCommands.h"
#include "SaListChannel.h"
#include "GameMessage.h"
#include "GameText.h"
#include "GameClientShell.h"
#include "TextScroll.h"
#include "PopupNickName.h"
#include "ClientMultiPlayerMgr.h"

#include "ScreenMain.h"		// NOVICE : 메인화면 로딩 빨리..

namespace
{
	void QuitCallBack(LTBOOL bReturn, void *pData)
	{
		CScreenChannel *pThisScreen = (CScreenChannel *)pData;
		if (bReturn && pThisScreen)
			pThisScreen->SendCommand(CMD_CLIENT_SHUTDOWN,0,0);
	};

	void ShowPopupCallBack(LTBOOL bReturn, void *pData)
	{
		CScreenChannel *pThisScreen = (CScreenChannel*)pData;
		if( bReturn && pThisScreen ) pThisScreen->ShowPopupNick();
	};

	void ChangeNickCallBack(LTBOOL bReturn, void *pData)
	{
		CScreenChannel *pThisScreen = (CScreenChannel*)pData;
		if( pThisScreen )
		{
			if( bReturn )
			{
				//사용
				pThisScreen->SendNickName();
			}
			else
			{
				//다시 생성
				pThisScreen->ShowPopupNick();
			}
		}
	};
}

CScreenChannel *g_pScreenChannel = LTNULL;

CScreenChannel::CScreenChannel()
{
	g_pScreenChannel = this;
	m_pChannelList	= LTNULL;
	m_pBtnJoin		= LTNULL;
	m_pBtnRefresh	= LTNULL;
	m_pBtnExit		= LTNULL;
	m_pTextScroll	= LTNULL;
	m_pPopupNickName = LTNULL;
}

CScreenChannel::~CScreenChannel()
{
	g_pScreenChannel = LTNULL;
	SAFE_DELETE( m_pTextScroll );
	Term();
}

LTBOOL CScreenChannel::Build()
{
	CreateBackWindow(SCREEN_ID_MAIN);

	m_pBtnJoin		= AddButton(CMD_JOIN_CHANNEL, "BtnJoinRt",
								"BtnJoinN", "BtnJoinH",
								"BtnJoinD", "BtnJoinC", LTTRUE );
	m_pBtnRefresh	= AddButton(CMD_REFRESH_CHANNEL, "BtnRefreshRt",
								"BtnRefreshN", "BtnRefreshH",
								"BtnRefreshD", "BtnRefreshC", LTTRUE );
	m_pBtnExit		= AddButton(CMD_EXIT_CHANNEL, "BtnExitRt",
								"BtnExitN", "BtnExitH",
								"BtnExitD", "BtnExitC", LTTRUE );

	m_pChannelList	= new CSaListChannel();
	m_pChannelList->Create( 247, 108, 515, 492, 26, this );
	AddControl( m_pChannelList );

	AddDummyCtrl();
	
	m_pPopupNickName = new CPopupNickName();
	m_pPopupNickName->Init(this);
	AddControl((CLTGUICtrl*)m_pPopupNickName);

	m_hUIBottomL = g_pInterfaceResMgr->GetTexture( "sa_interface\\menu\\textures\\common\\UIBottomL_S.dtx" );
	g_pDrawPrim->SetXYWH( &m_PolyUIBottomL, 0.0f, 580.0f, 16.0f, 32.0f );
	g_pDrawPrim->SetRGBA( &m_PolyUIBottomL, 0xFFFFFFFF );
	g_pDrawPrim->SetUVWH( &m_PolyUIBottomL, 0.0f, 0.0f, 1.0f, 1.0f );

	m_hUIBottomR = g_pInterfaceResMgr->GetTexture( "sa_interface\\menu\\textures\\common\\UIBottomR.dtx" );
	g_pDrawPrim->SetXYWH( &m_PolyUIBottomR, 785.0f, 580.0f, 16.0f, 32.0f );
	g_pDrawPrim->SetRGBA( &m_PolyUIBottomR, 0xFFFFFFFF );
	g_pDrawPrim->SetUVWH( &m_PolyUIBottomR, 0.0f, 0.0f, 1.0f, 1.0f );

	m_pTextScroll = new CTextScroll();
	m_pTextScroll->Create( 15.0f, 583.0f, 760.0f, 20.0f );

	if (!CScreenBase::Build()) return LTFALSE;
	return LTTRUE;
}

void CScreenChannel::Term()
{
}

void CScreenChannel::OnFocus(LTBOOL bFocus)
{
	m_pChannelList->DeleteAllItem();
	
	if( bFocus )
	{
		m_nChannelCount	= 10000;

		g_Network.EnableNetwork(LTTRUE);	
		
		g_pInterfaceMgr->PlayBGM( "SA_INTERFACE\\SND\\BG\\ScreenChannel.wav" );

		m_pTextScroll->Clear();
		SetRanking();
		
		if( m_bShowNickName )
		{
			m_pPopupNickName->CtrlShow();
		}
		else
		{
			m_pPopupNickName->CtrlHide();	
			//"채널 리스트를 받고 있습니다.\r\n\r\n잠시만 기다려 주십시요."
			g_pInterfaceMgr->ShowMessageBox( SA_TEXT_HELP_NOW_RECEIVE_CHANNEL_LIST,
										 LTMB_MESSAGE, LTNULL, LTNULL );
		}
	}
	else
	{
		m_pPopupNickName->CtrlHide();
		g_pInterfaceMgr->KillBGM();
		g_Network.EnableNetwork(LTFALSE);
	}

	CScreenBase::OnFocus(bFocus);	
}

uint32 CScreenChannel::OnCommand(uint32 dwCommand, uint32 dwParam1, uint32 dwParam2)
{
	switch( dwCommand )
	{
	case CMD_JOIN_CHANNEL:
		{
			T_CHANNEL_ITEMptr pChannel = m_pChannelList->GetSelectedChannelData();
			if( pChannel )
			{
				//"채널에 접속 중 입니다.\r\n\r\n잠시만 기다려 주십시요."
				g_pInterfaceMgr->ShowMessageBox( SA_TEXT_HELP_NOW_TRY_CONNECT_CHANNEL, LTMB_MESSAGE, LTNULL, LTNULL );
				g_Network.CS_JoinChannel( pChannel->nIndex );
			}
		}
		break;

	case CMD_REFRESH_CHANNEL:
		g_GameDoc.SetChannelIndex( g_GameDoc.GetChannelIndex() );
		g_Network.CS_RefreshChannel();
		break;

	case CMD_EXIT_CHANNEL:
		//"종료하시겠습니까?"
		g_pInterfaceMgr->ShowMessageBox( SA_TEXT_ASK_SURE_QUIT, LTMB_YESNO, QuitCallBack, this );
		break;

	case CMD_CLIENT_SHUTDOWN:
		g_pLTClient->Shutdown();
		break;

	default:
		break;
	}
	return 1;
}

void CScreenChannel::CreateInterfaceSFX()
{
	CScreenBase::CreateInterfaceSFX();
}

void CScreenChannel::CreateBackWindow(eScreenID id)
{
	// 구름 Texture
	m_hTxCloud = g_pInterfaceResMgr->GetTexture( "SA_INTERFACE/MENU/TEXTURES/COMMON/BackFrameCloud.dtx" );
	SetupQuadUVs( m_hPolyCloud, m_hTxCloud, 0.0f, 0.0f, 2.0f, 1.0f );
	g_pDrawPrim->SetXYWH( &m_hPolyCloud, 0, 0, 1024, 512 );
	g_pDrawPrim->SetRGBA( &m_hPolyCloud, 0xFFFFFFFF );
	
	AddFrame("BackFrameTxPos_0", "BGTexture_0");
	AddFrame("BackFrameTxPos_1", "BGTexture_1");
	AddFrame("BackFrameTxPos_2", "BGTexture_2");
	AddFrame("BackFrameTxPos_3", "BGTexture_3");
	AddFrame("BackFrameTxPos_4", "BGTexture_4");
	AddFrame("BackFrameTxPos_5", "BGTexture_5");
	AddFrame("BackFrameTxPos_6", "BGTexture_6");
	AddFrame("BackFrameTxPos_7", "BGTexture_7");
	AddFrame("BackFrameTxPos_8", "BGTexture_8");
}

void CScreenChannel::SetChannelCount( int nCount )
{
	m_pChannelList->DeleteAllItem();
	m_nChannelCount = nCount;

	//"채널 리스트를 받고 있습니다.\r\n\r\n잠시만 기다려 주십시요."
	g_pInterfaceMgr->ShowMessageBox( SA_TEXT_HELP_NOW_RECEIVE_CHANNEL_LIST,
									 LTMB_MESSAGE, LTNULL, LTNULL );
}

void CScreenChannel::AddChannel( int nIndex, const char* lpszName, int nCurr, int nMax )
{
	if( nIndex < 0 || !lpszName ) return;

	--m_nChannelCount;

	T_CHANNEL_ITEMptr pChannel = new T_CHANNEL_ITEM;
	pChannel->nIndex		= nIndex;
	pChannel->nCurrentUsers	= nCurr;
	pChannel->nMaxUsers		= nMax;
	pChannel->pTextIndex	= LTNULL;
	pChannel->pTextName		= LTNULL;
	pChannel->pTextUsers	= LTNULL;

	strcpy( pChannel->szName, lpszName );
	m_pChannelList->AddChannel( pChannel );

	if( m_nChannelCount <= 0 )
	{
		m_pChannelList->SortNDrawText();
		m_pChannelList->SetSelectIndex( g_GameDoc.GetChannelIndex() );
		g_pInterfaceMgr->CloseMessageBox(LTTRUE);
	}
}

LTBOOL CScreenChannel::OnUp()
{
	int nSelIndex = m_pChannelList->GetSelectIndex();
	if( nSelIndex > 0 )
	{
		g_pInterfaceMgr->RequestInterfaceSound( (InterfaceSound)IS_CLICK_RADIO );
		m_pChannelList->SetSelectIndex( nSelIndex-1 );
		g_GameDoc.SetChannelIndex( nSelIndex-1 );
	}
	return CScreenBase::OnUp();
}

LTBOOL CScreenChannel::OnDown()
{
	int nSelIndex = m_pChannelList->GetSelectIndex();
	if( nSelIndex < m_pChannelList->GetLastIndex() )
	{
		g_pInterfaceMgr->RequestInterfaceSound( (InterfaceSound)IS_CLICK_RADIO );
		m_pChannelList->SetSelectIndex( nSelIndex+1 );
		g_GameDoc.SetChannelIndex( nSelIndex+1 );
	}
	return CScreenBase::OnDown();
}

LTBOOL CScreenChannel::OnEnter()
{
	OnCommand( CMD_JOIN_CHANNEL, LTNULL, LTNULL );
	return LTTRUE;
//	return CScreenBase::OnEnter();
}

LTBOOL CScreenChannel::OnEscape()
{
	OnCommand( CMD_EXIT_CHANNEL, LTNULL, LTNULL );
	return CScreenBase::OnEscape();
}

LTBOOL CScreenChannel::Render( HSURFACE hDestSurf)
{
	SetRenderState();
	RenderCloud();

	if( LTFALSE == CScreenBase::Render(hDestSurf) ) return LTFALSE;	
	
	m_pTextScroll->Update();
	m_pTextScroll->Render();

	g_pDrawPrim->SetTexture( m_hUIBottomL );
	g_pDrawPrim->DrawPrim( &m_PolyUIBottomL );
	g_pDrawPrim->SetTexture( m_hUIBottomR );
	g_pDrawPrim->DrawPrim( &m_PolyUIBottomR );

	return LTTRUE;
}

LTBOOL CScreenChannel::RenderCloud()
{
	if ( m_hTxCloud )
	{
		g_pDrawPrim->SetTexture( m_hTxCloud );
		g_pDrawPrim->DrawPrim( &m_hPolyCloud );
		
		static LTFLOAT fCloudSceneAdjust = 0;
		
		fCloudSceneAdjust += g_pLTClient->GetFrameTime() * 0.025f;
		
		SetupQuadUVs( m_hPolyCloud, m_hTxCloud, 0.0f + fCloudSceneAdjust, 0.0f, 2.0f, 1.0f );
		
		
		if ( fCloudSceneAdjust > 1.0f )
		{
			fCloudSceneAdjust -= 1.0f;
		}
	}

	return LTTRUE;
}

void CScreenChannel::SetNickName( LTBOOL bShow )
{
	m_bShowNickName = bShow;
}

void CScreenChannel::ShowPopupNick()
{
	m_pPopupNickName->CtrlShow();
}

void CScreenChannel::ResponseNickName( int nResult, const char* lpszNick, const char* lpszMsg )
{
	char		szMsg[256];
	MBCreate mb;
	mb.eType	= LTMB_OK;
	mb.pFn		= LTNULL;
	mb.pData	= LTNULL;

	switch( nResult )
	{
	case 0: //Success
		g_pInterfaceMgr->CloseMessageBox(LTTRUE);
		break;
	case 1:
		mb.pFn		= ShowPopupCallBack;
		mb.pData	= this;
		//"이미 사용중인 닉네임 입니다."
		g_pInterfaceMgr->ShowMessageBox( SA_TEXT_FAIL_NICK_ALREADY_USED, &mb );
		break;
		
	case 2:
		mb.pFn		= ShowPopupCallBack;
		mb.pData	= this;
		sprintf( szMsg, "현재 사용중인 닉네임은\r\n\r\n%s입니다.\r\n\r\n"
						"규정에 어긋난 닉네임 이므로\r\n\r\n닉네임을 새로 입력해 주시기 바랍니다.", lpszNick );
		g_pInterfaceMgr->ShowMessageBox( szMsg, &mb );
		break;

	case 3:
		mb.pFn		= ChangeNickCallBack;
		mb.pData	= this;	
		mb.eType	= LTMB_YESNO;
		sprintf( szMsg, "현재 사용중인 닉네임은\r\n\r\n%s입니다.\r\n\r\n기존 닉네임을 유지 하시겠습니까?",
						lpszNick );
		strcpy( m_szBackupNick, lpszNick );

		g_pInterfaceMgr->ShowMessageBox( szMsg, &mb );
		break;
	
	default:
		g_pInterfaceMgr->ShowMessageBox( SA_TEXT_FAIL_ON_NICK_MAKE, &mb );
		break;
	}	
}

void CScreenChannel::SendNickName()
{
	g_Network.CS_FirstNickName( m_szBackupNick );
}

void CScreenChannel::SetRanking()
{
	if( LTFALSE == g_GameDoc.IsRankingInfo() ) return;

	char*				lpszToken;
	int					nCount = 1;
	char				szInfo[257];
	T_TEXT_SCROLLptr	pText = new T_TEXT_SCROLL;

	pText->szText1[0] = LTNULL;
	pText->szText2[0] = LTNULL;
	pText->szText3[0] = LTNULL;
	pText->szText4[0] = LTNULL;
	
	strcpy( szInfo, g_GameDoc.GetRanking() );	
	
	lpszToken = strtok( szInfo, "\r" );
	while( lpszToken )
	{
		if( nCount == 1 )
		{
			strcpy( pText->szText1, lpszToken );
			strcat( pText->szText2, "    " );
			pText->rgb_1	= RGB(57,180,74);
		}
		else if( nCount == 2 )
		{
			strcpy( pText->szText2, lpszToken );
			strcat( pText->szText2, "         " );
			pText->rgb_2	= RGB(250,204,133);
		}
		else if( nCount == 3 )
		{
			strcpy( pText->szText3, lpszToken );
			strcat( pText->szText3, "         " );
			pText->rgb_3	= RGB(250,204,133);
		}
		else if( nCount == 4 )
		{
			strcpy( pText->szText4, lpszToken );
			strcat( pText->szText4, "         " );
			pText->rgb_4	= RGB(250,204,133);
		}
		else
		{
			break;
		}
				
		++nCount;
		lpszToken = strtok( LTNULL, "\r" );
	}

	m_pTextScroll->AddText( pText );
}