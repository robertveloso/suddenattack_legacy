// ----------------------------------------------------------------------- //
//
// MODULE  : ScreenMain.cpp
//
// PURPOSE : Top level interface screen
//
// (c) 1999-2002 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //


#include "stdafx.h"

#define SCREENMAIN

#include "ScreenMain.h"
#include "ScreenChannel.h"
#include "ScreenMgr.h"
#include "LayoutMgr.h"
#include "ScreenCommands.h"
#include "WinUtil.h"
#include "direct.h"
#include "GameClientShell.h"
#include "ModelButeMgr.h"
#include "MissionMgr.h"
#include "ClientMultiplayerMgr.h"
#include "ResShared.h"
#include "ScreenMulti.h"
#include "ScreenInventory.h"

// Added By Bestline ==>
// Time : 2004-06-21 오후 9:48:28
#include "SAPlayerStats.h"
#include "ScreenWeapon.h"
// Added By Bestline <==
#include "ScreenJoin.h"

#include "GameHIIME.h"

//[MURSUM]==================================================
#include "SaListRoom.h"
#include "SaListUser.h"
#include "SaListText.h"
#include "CurseFilter.h"
#include "TextScroll.h"
//==========================================================

#include "GameMessage.h"
#include "GameText.h"

#include "PopupScreenCreateRoom.h"
#include "PopupScreenOption.h"
#include "PopupPassword.h"
#include "PopupSearchUser.h"
#include "PopupSearchResult.h"
#include "PopupUserInfo.h"
#include "PopupInvite.h"
#include "PopupMenu.h"
#include "PopupHelp.h" //[yaiin]

namespace
{
	// Model
	#define INVALID_ANI			((HMODELANIM)-1)

	int kColumnWidth_Lock			= 20;
	int kColumnWidth_Number			= 37;
	int kColumnWidth_Name			= 248;	
	int kColumnWidth_Status			= 39;
	int	kColumnWidth_Map			= 67;	
	int	kColumnWidth_GameType		= 69;	
	int kColumnWidth_Players		= 37;
	int kColumnWidth_Ping			= 37;
	
	char g_szChatBuffer[512];
	
	void QuitCallBack(LTBOOL bReturn, void *pData)
	{
		CScreenMain *pThisScreen = (CScreenMain *)pData;
		if (bReturn && pThisScreen)
			pThisScreen->SendCommand(CMD_CLIENT_SHUTDOWN,0,0);
	};

#define GAME_TYPE_d		sizeof(g_Game_Type_str) / sizeof(char*)
	char* g_Game_Type_str[] = 
	{
		TEXT("Time"),
		TEXT("Point"),
	};

#define MAIN_TEXTURE_SIZE			32.0f
}

extern bool g_bLAN;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScreenMain::CScreenMain()
{	
	m_hTxCloud				= LTNULL;
	m_pBtnQuickJoin			= LTNULL;
	m_pBtnCreateRoom		= LTNULL;
	m_pBtnInventory			= LTNULL;
	m_pBtnOption			= LTNULL;
	m_pBtnHelp				= LTNULL; //[yaiin]
	m_pBtnShop				= LTNULL;
	m_pBtnClan				= LTNULL;
	m_pBtnExit				= LTNULL;
	m_pBtnUser				= LTNULL;
	m_pBtnFriend			= LTNULL;
	m_pBtnFind				= LTNULL;
	m_pBtnLH_1				= LTNULL;
	m_pBtnLH_2				= LTNULL;
	m_pBtnLH_3				= LTNULL;
	m_pBtnLH_4				= LTNULL;
	m_pBtnLH_5				= LTNULL;
	m_pBtnLH_6				= LTNULL;
	m_pBtnLH_7				= LTNULL;
	m_pBtnRoomScroll		= LTNULL;
	m_pBtnRoomScrollUp		= LTNULL;
	m_pBtnRoomScrollDown	= LTNULL;
	m_pRoomList				= LTNULL;
	m_pBtnUserScroll		= LTNULL;
	m_pBtnUserScrollUp		= LTNULL;
	m_pBtnUserScrollDown	= LTNULL;
	m_pUserList				= LTNULL;
	m_pFriendList			= LTNULL;
	m_pBtnTextScroll		= LTNULL;
	m_pBtnTextScrollUp		= LTNULL;
	m_pBtnTextScrollDown	= LTNULL;
	m_pTextList				= LTNULL;
	m_pChatEdit				= LTNULL;
	m_pPopupOption			= LTNULL;
	m_pPopupHelp			= LTNULL; //[yaiin]
	m_pPopupPassword		= LTNULL;
	m_pPopupSearchUser		= LTNULL;
	m_pPopupSearchResult	= LTNULL;
	m_pPopupUserInfo		= LTNULL;
	m_pTooltipRoom			= LTNULL;
	m_pChannelInfo			= LTNULL;
	m_pTextScroll			= LTNULL;
	m_pPopupMenu			= LTNULL;
	m_pInviteMsg			= LTNULL;

	g_pScreenMain = this;
}

CScreenMain::~CScreenMain()
{
	SAFE_DELETE( m_pChannelInfo );
	SAFE_DELETE( m_pTextScroll );
}

void CScreenMain::CreateBackWindow(eScreenID id)
{
	// 구름 Texture
	m_hTxCloud = g_pInterfaceResMgr->GetTexture( "SA_INTERFACE/MENU/TEXTURES/COMMON/BackFrameCloud.dtx" );
	//SetupQuadUVs( m_hPolyCloud, m_hTxCloud, 0.0f, 0.0f, 2.0f, 1.0f );
	g_pDrawPrim->SetUVWH( &m_hPolyCloud, 0.0f, 0.0f, 2.0f, 1.0f );
	g_pDrawPrim->SetXYWH( &m_hPolyCloud, 0, 0, 1024, 512 );
	g_pDrawPrim->SetRGBA( &m_hPolyCloud, 0xFFFFFFFF );
	
	// 새로 만든 back 은 9장.
	CLTGUIFrame* pBackFrame0 = AddFrame("BackFrameTxPos_0", "BGTexture_0");
	CLTGUIFrame* pBackFrame1 = AddFrame("BackFrameTxPos_1", "BGTexture_1");
	CLTGUIFrame* pBackFrame2 = AddFrame("BackFrameTxPos_2", "BGTexture_2");
	CLTGUIFrame* pBackFrame3 = AddFrame("BackFrameTxPos_3", "BGTexture_3");
	CLTGUIFrame* pBackFrame4 = AddFrame("BackFrameTxPos_4", "BGTexture_4");
	CLTGUIFrame* pBackFrame5 = AddFrame("BackFrameTxPos_5", "BGTexture_5");
	CLTGUIFrame* pBackFrame6 = AddFrame("BackFrameTxPos_6", "BGTexture_6");
	CLTGUIFrame* pBackFrame7 = AddFrame("BackFrameTxPos_7", "BGTexture_7");
	CLTGUIFrame* pBackFrame8 = AddFrame("BackFrameTxPos_8", "BGTexture_8");
}

// Build the screen
LTBOOL CScreenMain::Build()
{	
// SPIKE  >> NOVICE
//	CreateBackFrame();

	CreateBackWindow(SCREEN_ID_MAIN);

	// 왼편 버튼
	m_pBtnQuickJoin		= AddButton(CMD_QUICK_JOIN, "BtnQuickJoinRt", 
									"BtnQuickJoinN", "BtnQuickJoinH",
									"BtnQuickJoinD", "BtnQuickJoinC", LTTRUE);
	m_pBtnCreateRoom	= AddButton(CMD_CREATE_ROOM, "BtnCreateRoomRt",
									"BtnCreateRoomN", "BtnCreateRoomH",
									"BtnCreateRoomD", "BtnCreateRoomC", LTTRUE );	
	m_pBtnInventory		= AddButton(CMD_INVENTORY, "TBtnInventoryRt",
									"TBtnInventoryN", "TBtnInventoryH",
									"TBtnInventoryD", "TBtnInventoryC", LTTRUE);
	m_pBtnOption		= AddButton(CMD_OPTION, "BtnOptionRt",
									"BtnOptionN", "BtnOptionH",
									"BtnOptionD", "BtnOptionC", LTTRUE);
	m_pBtnHelp		= AddButton(CMD_HELP, "BtnHelpRt",
									"BtnHelpN", "BtnHelpH",
									"BtnHelpD", "BtnHelpC", LTTRUE); //[yaiin]

	m_pBtnCreateRoom->SetClickSound( (int)IS_CLICK_RADIO );
	m_pBtnQuickJoin->SetClickSound( (int)IS_CLICK_RADIO );
	m_pBtnInventory->SetClickSound( (int)IS_CLICK_RADIO );
	m_pBtnOption->SetClickSound( (int)IS_CLICK_RADIO );
	m_pBtnHelp->SetClickSound( (int)IS_CLICK_RADIO ); //[yaiin]
	
	// 상단 버튼
	m_pBtnShop			= AddButton(CMD_SHOP, "TBtnShopRt", 
									"TBtnShopN", "TBtnShopH",
									"TBtnShopD", "TBtnShopC", LTTRUE );
	m_pBtnClan			= AddButton(CMD_CLAN, "TBtnClanRt", 
									"TBtnClanN", "TBtnClanH",
									"TBtnClanD", "TBtnClanC", LTTRUE );
	m_pBtnExit			= AddButton(CMD_EXIT, "TBtnExitRt",
									"TBtnExitN", "TBtnExitH",
									"TBtnExitD", "TBtnExitC", LTTRUE);

	//List Header
	m_pBtnLH_1			= AddButton(CMD_LIST_HEADER_1, "BtnLH_1Rt",
									"BtnLH_1n", "BtnLH_1h", "BtnLH_1d", "BtnLH_1c", LTTRUE );
	m_pBtnLH_2			= AddButton(CMD_LIST_HEADER_2, "BtnLH_2Rt",
									"BtnLH_2n", "BtnLH_2h", "BtnLH_2d", "BtnLH_2c", LTTRUE );
	m_pBtnLH_3			= AddButton(CMD_LIST_HEADER_3, "BtnLH_3Rt",
									"BtnLH_3n", "BtnLH_3h", "BtnLH_3d", "BtnLH_3c", LTTRUE );
	m_pBtnLH_4			= AddButton(CMD_LIST_HEADER_4, "BtnLH_4Rt",
									"BtnLH_4n", "BtnLH_4h", "BtnLH_4d", "BtnLH_4c", LTTRUE );
	m_pBtnLH_5			= AddButton(CMD_LIST_HEADER_5, "BtnLH_5Rt",
									"BtnLH_5n", "BtnLH_5h", "BtnLH_5d", "BtnLH_5c", LTTRUE );
	m_pBtnLH_6			= AddButton(CMD_LIST_HEADER_6, "BtnLH_6Rt",
									"BtnLH_6n", "BtnLH_6h", "BtnLH_6d", "BtnLH_6c", LTTRUE );
	m_pBtnLH_7			= AddButton(CMD_LIST_HEADER_7, "BtnLH_7Rt",
									"BtnLH_7n", "BtnLH_7h", "BtnLH_7d", "BtnLH_7c", LTTRUE );
	m_pBtnLH_1->SetClickSound( (int)IS_CLICK_FLAT );
	m_pBtnLH_2->SetClickSound( (int)IS_CLICK_FLAT );
	m_pBtnLH_3->SetClickSound( (int)IS_CLICK_FLAT );
	m_pBtnLH_4->SetClickSound( (int)IS_CLICK_FLAT );
	m_pBtnLH_5->SetClickSound( (int)IS_CLICK_FLAT );
	m_pBtnLH_6->SetClickSound( (int)IS_CLICK_FLAT );
	m_pBtnLH_7->SetClickSound( (int)IS_CLICK_FLAT );

	// 하단 버튼
	m_pBtnUser			= AddButton( CMD_USER, "BBtnUserRt", "BBtnUserN", LTNULL, 0, "BBtnUserH");
	m_pBtnUser->SetCheck();	
	m_pBtnFriend		= AddButton( CMD_FRIEND, "BBtnFriendRt", "BBtnFriendN", LTNULL, 0, "BBtnFriendH");
	m_pBtnFind			= AddButton(CMD_FIND, "BBtnFindRt",
									"BBtnFindN", "BBtnFindH",
									"BBtnFindD", "BBtnFindC", LTTRUE );

	m_pBtnUser->SetClickSound( (int)IS_CLICK_FLAT );
	m_pBtnFriend->SetClickSound( (int)IS_CLICK_FLAT );
	m_pBtnFind->SetClickSound( (int)IS_CLICK_FLAT );
	//////////////////////////////////////////////////////////////////////////


	m_pBtnCreateRoom->SetTooltip( "Ctrl+C" );		m_pBtnCreateRoom->SetHotKey( 0x03 );	
	m_pBtnQuickJoin->SetTooltip( "Ctrl+Q" );		m_pBtnQuickJoin->SetHotKey( 0x11 );	
	m_pBtnInventory->SetTooltip( "Ctrl+I" );		m_pBtnInventory->SetHotKey( 0x09 );	
	m_pBtnOption->SetTooltip( "Ctrl+O" );			m_pBtnOption->SetHotKey( 0x0f );

	//[yaiin] : 참고 : HelpPopup의 VK_F1는 OnChar()가 아닌 OnKeyDown()을 통해 잡힌다.
	//즉 CBaseScreen::HandleHotKey()가 아닌 CBaseScreen::HandleKeyDown()에서 OnCommand(CMD_HELP)한다.
	//처음 "Ctrl+H" = ( 0x08 );
	m_pBtnHelp->SetTooltip( "F1" );					//m_pBtnHelp->SetHotKey( 0x08 ); //[yaiin]
	
	m_pBtnShop->SetTooltip( "Ctrl+S" );				m_pBtnShop->SetHotKey( 0x13 );	
	m_pBtnClan->SetTooltip( "Ctrl+L" );				m_pBtnClan->SetHotKey( 0x0C );	
	m_pBtnExit->SetTooltip( "Ctrl+X" );				m_pBtnExit->SetHotKey( 0x18 );

//	m_pBtnFriend->SetTooltip( "Ctrl+R" );			m_pBtnFriend->SetHotKey( 0x12 );	
	m_pBtnFind->SetTooltip( "Ctrl+F" );				m_pBtnFind->SetHotKey( 0x06 );


	m_pBtnClan->Show(LTFALSE);
	m_pBtnFriend->Show(LTFALSE);
	
	//[MURSUM] >> NOVICE =============================================================

	// 방 목록
	m_pRoomList	= new CSaListRoom();
	m_pRoomList->Create( 226, 110, 536, 280, 28, this );
	AddControl( m_pRoomList );
	
	// 대기실 유저 목록
	m_pUserList	= new CSaListUser();
	m_pUserList->Create( 590, 424, 176, 126, 21, this );
	AddControl( m_pUserList );

	// 친구 리스트
	m_pFriendList	= new CSaListFriend();
	m_pFriendList->Create( 590, 424, 176, 126, 21, this );
	AddControl( m_pFriendList );
	
	// 채팅 기록
	m_pTextList	= new CSaListText();
	m_pTextList->Create( 220, 426, 330, 105, 15, this );
	m_pTextList->SetMaxItemCount( 20 );	//History Count
	AddControl( m_pTextList );

	// 스크롤바..

	// 방 목록 스크롤바
	m_pBtnRoomScrollUp		= AddButton(CMD_ROOM_SCROLL_UP, "ScrollRoomUpRt",
										"ScrollUpN", "ScrollUpH",
										"ScrollUpD", "ScrollUpC", LTTRUE);
	m_pBtnRoomScrollDown	= AddButton(CMD_ROOM_SCROLL_DOWN, "ScrollRoomDnRt",
										"ScrollDnN", "ScrollDnH",
										"ScrollDnD", "ScrollDnC", LTTRUE);

	// 대기실 사용자 목록 스크롤바
	m_pBtnUserScrollUp		= AddButton(CMD_USER_SCROLL_UP, "ScrollUserUpRt",
										"ScrollUpN", "ScrollUpH",
										"ScrollUpD", "ScrollUpC", LTTRUE );
	m_pBtnUserScrollDown	= AddButton(CMD_USER_SCROLL_DOWN, "ScrollUserDnRt",
										"ScrollDnN", "ScrollDnH",
										"ScrollDnD", "ScrollDnC", LTTRUE);	

	// 채팅 기록 스크롤바
	m_pBtnTextScrollUp		= AddButton(CMD_TEXT_SCROLL_UP, "ScrollTextUpRt",
										"ScrollUpN", "ScrollUpH",
										"ScrollUpD", "ScrollUpC", LTTRUE );
	m_pBtnTextScrollDown	= AddButton(CMD_TEXT_SCROLL_DOWN, "ScrollTextDnRt",
										"ScrollDnN", "ScrollDnH",
										"ScrollDnD", "ScrollDnC", LTTRUE);	
	
	// 채팅창
	m_pChatEdit				= new CLTGUIEditBox();
	m_pChatEdit->Create( 215, 532, 355, 24,
						RGB(183,153,104), RGB(0,0,0), 0xFFB79968,
						D3D_TEXT_SHADOW, 
						LTFALSE,
						CMD_CHAT_EDIT, this );
	AddControl( m_pChatEdit );

	//
	m_pBtnRoomScroll		= AddDragButton(CMD_ROOM_SCROLL, "ScrollRoomRt",
											"ScrollHandleN", "ScrollHandleH",
											NULL, "ScrollHandleC" );
	m_pBtnUserScroll		= AddDragButton(CMD_USER_SCROLL, "ScrollUserRt",
											"ScrollHandleN", "ScrollHandleH",
											NULL, "ScrollHandleC" );	
	m_pBtnTextScroll		= AddDragButton(CMD_TEXT_SCROLL, "ScrollTextRt",
											"ScrollHandleN", "ScrollHandleH",
											NULL, "ScrollHandleC" );

	m_pBtnRoomScroll->SetLockX(LTTRUE);
	m_pBtnRoomScroll->SetLimitY( 124, 346 );	
	m_pBtnUserScroll->SetLockX(LTTRUE);	
	m_pBtnUserScroll->SetLimitY( 441, 511 );
	m_pBtnTextScroll->SetLockX(LTTRUE);
	m_pBtnTextScroll->SetLimitY( 441, 487 );



	m_pPopupMenu = new CPopupMenu();
	m_pPopupMenu->Create( 0, 0, 80, 20*2, 20, this, LTNULL, LTTRUE, 0xFF17202A, 0xFF676D6D );
	AddControl( m_pPopupMenu );
	m_pPopupMenu->AddButton( NF_GetMsg(SA_TEXT_MENU_USER_INFO), CMD_USER_INFO );
	m_pPopupMenu->AddButton( NF_GetMsg(SA_TEXT_MENU_WHISPER), CMD_WHISPER );
	//m_pPopupMenu->AddButton( NF_GetMsg(SA_TEXT_MENU_ADD_FRIEND), CMD_ADD_FRIENT_LIST );

	//=====================================================================
	AddDummyCtrl();
	
	/**/
	//-------------------------------------------------
	// 광식 [popup screen test]
	//-------------------------------------------------
	m_pPopupWindow = new CPopScreenCreateRoom();
	m_pPopupWindow->Init(this);
	AddControl((CLTGUICtrl*)m_pPopupWindow);
	//-------------------------------------------------

	m_pPopupOption	= new CPopScreenOption();
	m_pPopupOption->Init(this);
	AddControl((CLTGUICtrl*)m_pPopupOption);

	//[yaiin]
	m_pPopupHelp = new CPopupHelp();
	m_pPopupHelp->Init(this);
	AddControl((CLTGUICtrl*)m_pPopupHelp);

	m_pPopupPassword	= new CPopupPassword();
	m_pPopupPassword->Init(this);
	AddControl((CLTGUICtrl*)m_pPopupPassword);

	m_pPopupSearchUser	= new CPopupSearchUser();
	m_pPopupSearchUser->Init(this);
	AddControl((CLTGUICtrl*)m_pPopupSearchUser);

	m_pPopupSearchResult = new CPopupSearchResult();
	m_pPopupSearchResult->Init(this);
	AddControl((CLTGUICtrl*)m_pPopupSearchResult);

	m_pPopupUserInfo	= new CPopupUserInfo();
	m_pPopupUserInfo->Init(this);
	AddControl((CLTGUICtrl*)m_pPopupUserInfo);

	m_pInviteMsg		= new CPopupInviteMsg();
	m_pInviteMsg->Init(this);
	AddControl((CLTGUICtrl*)m_pInviteMsg);

	m_pTooltipRoom	= new CLTGUITooltipRoom();
	m_pTooltipRoom->CreateTooltip( 355, 163 );
	m_pTooltipRoom->CtrlHide();
	AddControl( (CLTGUICtrl*)m_pTooltipRoom );	

	m_pChannelInfo = new CSAD3DText();
	m_pChannelInfo->CreateD3DText( 190, 16, true );
	m_pChannelInfo->SetTextColor( RGB(255,220,166) );
	m_pChannelInfo->SetTransparent(true);
	m_pChannelInfo->SetPosition( 15, 583 );

	m_hUIBottomL = g_pInterfaceResMgr->GetTexture( "sa_interface\\menu\\textures\\common\\UIBottomL.dtx" );
	g_pDrawPrim->SetXYWH( &m_PolyUIBottomL, 0.0f, 580.0f, 256.0f, 32.0f );
	g_pDrawPrim->SetRGBA( &m_PolyUIBottomL, 0xFFFFFFFF );
	g_pDrawPrim->SetUVWH( &m_PolyUIBottomL, 0.0f, 0.0f, 1.0f, 1.0f );

	m_hUIBottomR = g_pInterfaceResMgr->GetTexture( "sa_interface\\menu\\textures\\common\\UIBottomR.dtx" );
	g_pDrawPrim->SetXYWH( &m_PolyUIBottomR, 785.0f, 580.0f, 16.0f, 32.0f );
	g_pDrawPrim->SetRGBA( &m_PolyUIBottomR, 0xFFFFFFFF );
	g_pDrawPrim->SetUVWH( &m_PolyUIBottomR, 0.0f, 0.0f, 1.0f, 1.0f );

	m_pTextScroll = new CTextScroll();
	m_pTextScroll->Create( 208.0f, 583.0f, 577.0f, 20.0f );
	
	// Make sure to call the base class
	if (!CScreenBase::Build()) return LTFALSE;

	return LTTRUE;
}

void CScreenMain::OnFocus(LTBOOL bFocus)
{
	if (bFocus)
	{
		m_pTextList->DeleteAllItem();			
		SetSelection(-1);		
		g_pInterfaceMgr->SetCommandLineJoin( false );

#ifdef BIG_FAT_MODE_TEST
		if( strncmp( g_GameDoc.GetChannelName(), "서든어택테스트서버", 18 ) == 0 )
		{
			g_GameDoc.SetBigHeadMode(LTTRUE);			
		}
		else
		{
			g_GameDoc.SetBigHeadMode(LTFALSE);			
		}

		if( g_GameDoc.IsBigHeadMode() )
		{
			g_pClientMultiplayerMgr->SetPlayerModelType(eModelFat);
		}
		else
		{
			g_pClientMultiplayerMgr->SetPlayerModelType(eModelNormal);
		}
#endif

		// Model 가져오기
		//-----------------------------------------------------------------------------------
		// Red Team Model 가져오기
		for( int n = 0; n < g_pModelButeMgr->GetNumTeamRedModels(); n++)
		{
			ModelId id = g_pModelButeMgr->GetTeamRedModel(n);
			HMODELDB dummy = LTNULL;
			g_pILTModelClient->CacheModelDB(g_pModelButeMgr->GetModelFilename(id), dummy);
		}
		// Blue Team Model 가져오기
		for( n = 0; n < g_pModelButeMgr->GetNumTeamBlueModels(); n++ )
		{
			ModelId id = g_pModelButeMgr->GetTeamBlueModel(n);
			HMODELDB dummy = LTNULL;
			g_pILTModelClient->CacheModelDB(g_pModelButeMgr->GetModelFilename(id), dummy);
		}
		//-----------------------------------------------------------------------------------
		
	
		//[MURSUM]=================================================================
		m_pBtnTextScroll->Show(LTFALSE);
		m_pChatEdit->ClearText();
						
		CScreenMgr* pScreenMgr = g_pInterfaceMgr->GetScreenMgr();
		if( pScreenMgr && SCREEN_ID_INVENTORY != pScreenMgr->GetPreviousScreenID() )
		{
			m_pRoomList->DeleteAllItem();
			m_pUserList->DeleteAllItem();
			m_pFriendList->DeleteAllItem();	

			m_pBtnRoomScroll->Show(LTFALSE);
			m_pBtnUserScroll->Show(LTFALSE);

			this->AddUser( g_GameDoc.GetUniqueIndex(),
						   g_GameDoc.GetNick(),
						   g_GameDoc.GetGrade() );
			
			if( LTFALSE == g_pInterfaceMgr->IsPlayBGM() )
			{
				g_pInterfaceMgr->PlayBGM( "SA_INTERFACE\\SND\\BG\\Screen.wav" );
				g_pProfileMgr->GetCurrentProfile()->ReSetBGMVolumeRatio(((float)g_pPopupOption->GetSndMusicEditBox()->GetInt())/100);
			}			
		}
		
		//==========================================================================	

		m_pPopupWindow->CtrlHide();
		m_pPopupOption->CtrlHide();
		m_pPopupHelp->CtrlHide(); //[yaiin]
		m_pInviteMsg->CtrlHide();
		m_pPopupPassword->CtrlHide();
		m_pPopupSearchUser->CtrlHide();
		m_pTooltipRoom->CtrlHide();
		m_pPopupSearchResult->CtrlHide();
		m_pPopupUserInfo->CtrlHide();

		if( m_pBtnUser->GetCheck() )
		{
			m_pUserList->Show(LTTRUE);
			m_pFriendList->Show(LTFALSE);
		}
		else
		{
			m_pUserList->Show(LTFALSE);
			m_pFriendList->Show(LTTRUE);
		}		
		
		g_Network.EnableNetwork(LTTRUE);

		m_pTextScroll->Clear();
		SetRanking();

		char szChannelInfo[128];
		sprintf( szChannelInfo, "%s-%d", g_GameDoc.GetChannelName(), g_GameDoc.GetChannelIndex()+1 );
		m_pChannelInfo->DrawD3DText( szChannelInfo, strlen(szChannelInfo), DT_CENTER );

		m_nChatCount = 0;
		m_bDisableChat	= LTFALSE;
		m_fDoBae	= m_fChatTime = g_pLTClient->GetTime();
		g_pInterfaceMgr->CloseMessageBox(LTTRUE);		
	}
	else
	{
		// Model 해제
		//-----------------------------------------------------------------------------------
		// Red Team Model 해제
		for( int n = 0; n < g_pModelButeMgr->GetNumTeamRedModels(); n++)
		{
			ModelId id = g_pModelButeMgr->GetTeamRedModel(n);
			g_pILTModelClient->UncacheModelDB(g_pModelButeMgr->GetModelFilename(id));
		}
		// Blue Team Model 해제
		for( n = 0; n < g_pModelButeMgr->GetNumTeamBlueModels(); n++ )
		{
			ModelId id = g_pModelButeMgr->GetTeamBlueModel(n);
			g_pILTModelClient->UncacheModelDB(g_pModelButeMgr->GetModelFilename(id));
		}
		//-----------------------------------------------------------------------------------

		m_pChatEdit->OnLostFocus();
		m_pPopupWindow->CtrlHide();
		m_pPopupOption->CtrlHide();
		m_pPopupHelp->CtrlHide(); //[yaiin]
		m_pInviteMsg->CtrlHide();
		m_pPopupPassword->CtrlHide();
		m_pPopupSearchUser->CtrlHide();
		m_pTooltipRoom->CtrlHide();	
		m_pPopupSearchResult->CtrlHide();
		m_pPopupUserInfo->CtrlHide();

		g_Network.EnableNetwork(LTFALSE);		
	}

	CScreenBase::OnFocus(bFocus);

	if( bFocus )
	{
		UpdateModel(0,0,0);
	}
	
}

void CScreenMain::ClearScreenMain()
{
	m_pTextList->DeleteAllItem();			
	SetSelection(-1);

	m_pRoomList->DeleteAllItem();
	m_pUserList->DeleteAllItem();
			
	m_pBtnRoomScroll->Show(LTFALSE);
	m_pBtnUserScroll->Show(LTFALSE);
	
	m_pPopupWindow->CtrlHide();
	m_pPopupOption->CtrlHide();
	m_pPopupHelp->CtrlHide(); //[yaiin]
	m_pInviteMsg->CtrlHide();
	m_pPopupPassword->CtrlHide();
	m_pPopupSearchUser->CtrlHide();
	m_pTooltipRoom->CtrlHide();	
	m_pPopupSearchUser->CtrlHide();
	m_pPopupSearchResult->CtrlHide();
	m_pPopupUserInfo->CtrlHide();

	this->AddUser( g_GameDoc.GetUniqueIndex(),
				   g_GameDoc.GetNick(),
				   g_GameDoc.GetGrade() );
}

uint32 CScreenMain::OnCommand(uint32 dwCommand, uint32 dwParam1, uint32 dwParam2)
{
	switch(dwCommand)
	{
	case CMD_FIND:
		if( LTFALSE == m_pPopupSearchUser->IsVisible() )
		{
			m_pPopupSearchUser->CtrlShow();
		}
		break;

	case CMD_JOIN_ROOM:
		JoinRoom( dwParam1, dwParam2 );
		break;		

	case CMD_INVENTORY:
		//g_pScreenInventory->SetShop(false);
		//g_pInterfaceMgr->SwitchToScreen(SCREEN_ID_INVENTORY);
		
		g_Network.CS_EnterInven();
		g_pInterfaceMgr->ShowMessageBox( SA_TEXT_ENTER_INVEN, LTMB_MESSAGE, LTNULL, LTNULL );
		break;
		
	case CMD_USER:
		if(!m_pBtnUser->GetCheck())
		{
			m_pBtnUser->SetCheck();		
			m_pBtnFriend->SetCheck(LTFALSE);

			m_pUserList->Show(LTTRUE);
			m_pFriendList->Show(LTFALSE);
			
			if( m_pUserList->IsEnableScroll() )
			{
				m_pBtnUserScroll->Show(LTTRUE);
				m_pBtnUserScroll->SetPosYRatio( m_pUserList->GetRatio() );
			}
			else
			{
				m_pBtnUserScroll->Show(LTFALSE);
			}
		}
		break;

	case CMD_FRIEND:
		if( !m_pBtnFriend->GetCheck() )
		{
			m_pBtnFriend->SetCheck();
			m_pBtnUser->SetCheck(LTFALSE);

			m_pUserList->Show(LTFALSE);
			m_pFriendList->Show(LTTRUE);

			if( m_pFriendList->IsEnableScroll() )
			{
				m_pBtnUserScroll->Show(LTTRUE);
				m_pBtnUserScroll->SetPosYRatio( m_pFriendList->GetRatio() );
			}
			else
			{
				m_pBtnUserScroll->Show(LTFALSE);
			}
		}
		break;
	
	case CMD_CREATE_ROOM:
		{
			if( m_pPopupWindow->IsVisible() ) break;
			m_pCaptureCtrl = (CLTGUICtrl*)m_pPopupWindow;
			m_pPopupWindow->CtrlShow();
		}
		break;
		
	case CMD_OPTION:
		{
			ShowOptionTab();
		}
		break;

	case CMD_HELP:
		{
			//[yaiin]
			if(LTFALSE == m_pPopupHelp->IsVisible())
			{
				if( m_pDummyCtrl && m_pDummyCtrl->IsVisible() ) break;
				m_pPopupHelp->CtrlShow();
			}
			else
			{
				m_pPopupHelp->CtrlHide();
			}
		}
		break;

	case CMD_QUICK_JOIN:
		{
			g_Network.CS_QuickJoin();
		}
		break;

	case CMD_SHOP:
		{
			g_Network.CS_EnterShop();
			g_pInterfaceMgr->ShowMessageBox( SA_TEXT_ENTER_SHOP, LTMB_MESSAGE, LTNULL, LTNULL );
		}
		break;

	case CMD_CLAN:
		{
			//" 준비 중 입니다."			
			g_pInterfaceMgr->ShowMessageBox( SA_TEXT_WILL_SERVICE, LTMB_OK, LTNULL, LTNULL );
		}
		break;

	case CMD_SHOW_PASSWORD_DLG:
		ShowPasswordDlg();
		break;

	case CMD_JOIN_PRIVATE_ROOM:
		if( dwParam2 > 0 )
		{
			T_ROOM_ITEMptr pRoomItem = m_pRoomList->GetRoomData(dwParam1);
			JoinPrivateRoom( (void*)pRoomItem );
		}
		else
		{
			JoinPrivateRoom( (void*)m_pRoomList->GetSelectedRoomData() );
		}
		break;
		
	case CMD_EXIT:
		g_Network.CS_AwayChannel();	
		g_pScreenChannel->SetNickName(LTFALSE);
		g_pInterfaceMgr->SwitchToScreen(SCREEN_ID_CHANNEL);
		break;

	case CMD_CLIENT_SHUTDOWN:
		{
			g_pLTClient->Shutdown();
		}
		break;

	case CMD_CHAT_EDIT:
		switch( dwParam1 ) 
		{
		case UIM_EDIT_ENTER:
			if( m_pChatEdit->GetChar(0) == '/' )
			{
				if( LTTRUE == EditCommand( m_pChatEdit->GetText() ) ) return 1;
			}

			if( LTTRUE == m_bDisableChat )
			{
				m_pChatEdit->ClearText();
				break;
			}

			if( strlen( m_pChatEdit->GetText() ) > 0 )
			{
				sprintf( g_szChatBuffer, "[%s] %s", g_GameDoc.GetNick(), m_pChatEdit->GetText());			
				AddChat( g_szChatBuffer );

				if( LTFALSE == IsCurse( m_pChatEdit->GetText() ) )	 //욕 필터링
				{
					g_Network.CS_Chat( g_szChatBuffer );
				}
				CheckDoBae();
				m_pChatEdit->ClearText();
			}
			break;

		case UIM_EDIT_CHAR:
			if( stricmp( m_pChatEdit->GetText(), "/w" ) == 0 ||
				//											"/ㅈ"
				strcmp( m_pChatEdit->GetText(), NF_GetMsg(SA_TEXT_CMD_WHISPER) ) == 0 )
			{
				if( false == m_pChatEdit->IsLastBack() && strlen( g_GameDoc.GetLastSendNick() ) > 0 )
				{
					m_pChatEdit->StrCat( " " );
					m_pChatEdit->StrCat( g_GameDoc.GetLastSendNick() );
					m_pChatEdit->StrCat( " " );
				}
			}
			else if( stricmp( m_pChatEdit->GetText(), "/r" ) == 0 ||
				//											"/ㄱ"
				strcmp( m_pChatEdit->GetText(),  NF_GetMsg(SA_TEXT_CMD_REPLY) ) == 0 )
			{
				if( false == m_pChatEdit->IsLastBack() && strlen( g_GameDoc.GetLastRecvNick() ) > 0 )
				{
					m_pChatEdit->ClearText();
					m_pChatEdit->StrCat( "/W " );
					m_pChatEdit->StrCat( g_GameDoc.GetLastRecvNick() );
					m_pChatEdit->StrCat( " " );
				}
			}
			break;
		}

		break;

	//[MURSUM]========================================================
	case CMD_TEXT_WHELL:
		if( m_pTextList->IsEnableScroll() )
		{
			m_pBtnTextScroll->SetPosYRatio( m_pTextList->GetRatio() );
		}
		break;

	case CMD_MAIN_ROOM_WHELL:
		if( m_pRoomList->IsEnableScroll() )
		{
			m_pBtnRoomScroll->SetPosYRatio( m_pRoomList->GetRatio() );
		}
		break;

	case CMD_MAIN_USER_WHELL:
		if( m_pUserList->IsEnableScroll() )
		{
			m_pBtnUserScroll->SetPosYRatio( m_pUserList->GetRatio() );
		}
		break;

	case CMD_MAIN_FRIEND_WHELL:
		if( m_pFriendList->IsEnableScroll() )
		{
			m_pBtnUserScroll->SetPosYRatio( m_pFriendList->GetRatio() );
		}
		break;

	case CMD_ROOM_SCROLL:
		{
			if( dwParam1 == CMD_PARAM_MOUSEMOVE ) m_pRoomList->ScrollFromRatio( m_pBtnRoomScroll->GetPosYRatio() );
		}
		break;

	case CMD_ROOM_SCROLL_UP:
		{
			if( m_pRoomList->IsEnableScroll() )
			{
				m_pRoomList->ScrollUp();
				m_pBtnRoomScroll->SetPosYRatio( m_pRoomList->GetRatio() );
			}
		}
		break;

	case CMD_ROOM_SCROLL_DOWN:
		{
			if( m_pRoomList->IsEnableScroll() )
			{
				m_pRoomList->ScrollDown();
				m_pBtnRoomScroll->SetPosYRatio( m_pRoomList->GetRatio() );
			}			
		}
		break;

	case CMD_USER_SCROLL:
		{
			if( dwParam1 == CMD_PARAM_MOUSEMOVE ) m_pUserList->ScrollFromRatio( m_pBtnUserScroll->GetPosYRatio() );
		}
		break;

	case CMD_USER_SCROLL_UP:
		{
			if( m_pUserList->IsEnableScroll() )
			{
				m_pUserList->ScrollUp();
				m_pBtnUserScroll->SetPosYRatio( m_pUserList->GetRatio() );
			}			
		}
		break;

	case CMD_USER_SCROLL_DOWN:
		{
			if( m_pUserList->IsEnableScroll() )
			{
				m_pUserList->ScrollDown();
				m_pBtnUserScroll->SetPosYRatio( m_pUserList->GetRatio() );
			}			
		}
		break;

	case CMD_TEXT_SCROLL:
		{
			if( dwParam1 == CMD_PARAM_MOUSEMOVE ) m_pTextList->ScrollFromRatio( m_pBtnTextScroll->GetPosYRatio() );
		}
		break;

	case CMD_TEXT_SCROLL_UP:
		{
			if( m_pTextList->IsEnableScroll() )
			{
				m_pTextList->ScrollUp();
				m_pBtnTextScroll->SetPosYRatio( m_pTextList->GetRatio() );
			}			
		}
		break;

	case CMD_TEXT_SCROLL_DOWN:
		{
			if( m_pTextList->IsEnableScroll() )
			{
				m_pTextList->ScrollDown();
				m_pBtnTextScroll->SetPosYRatio( m_pTextList->GetRatio() );
			}			
		}
		break;

	case CMD_SHOW_POPUP:
		{
			m_pPopupMenu->ShowPopupMenu( g_pInterfaceMgr->GetCursorPos(), dwParam1, dwParam2 );
		}
		break;

	case CMD_USER_INFO:
		{
			T_USER_ITEMptr	pUserItem = m_pUserList->GetUserData(dwParam1);
			if( pUserItem )
			{
				g_Network.CS_UserInfo( pUserItem->nIndex, pUserItem->szNick, pUserItem->nGrade );
			}
		}
		break;

	case CMD_WHISPER:
		{
			T_USER_ITEMptr	pUserItem = m_pUserList->GetUserData(dwParam1);
			if( pUserItem )
			{
				char szTemp[256];
				sprintf( szTemp, "/w %s ", pUserItem->szNick );
				m_pChatEdit->ClearText();
				m_pChatEdit->SetText( szTemp );
				m_pChatEdit->ForceAddEditBox();
			}
		}
		break;
	//================================================================

	case CMD_LIST_HEADER_1:
		m_pRoomList->SortRoomNumber();
		break;
	case CMD_LIST_HEADER_2:	//방제목
		m_pRoomList->SortRoomTitle();
		break;
	case CMD_LIST_HEADER_3: //맵
		m_pRoomList->SortMapIndex();
		break;
	case CMD_LIST_HEADER_4: //쪽수
		m_pRoomList->SortUserCount();
		break;
	case CMD_LIST_HEADER_5: //모드
		m_pRoomList->SortGameType();
		break;
	case CMD_LIST_HEADER_6: //상태
		m_pRoomList->SortState();
		break;
	case CMD_LIST_HEADER_7: //핑
		m_pRoomList->SortPing();
		break;
		
	default:
		return CScreenBase::OnCommand(dwCommand,dwParam1,dwParam2);
	}
	return 1;
};

LTBOOL CScreenMain::RenderCloud()
{
	if ( m_hTxCloud )
	{
		g_pDrawPrim->SetTexture( m_hTxCloud );
		g_pDrawPrim->DrawPrim( &m_hPolyCloud );
		
		static LTFLOAT fCloudSceneAdjust = 0;
		
		fCloudSceneAdjust += g_pLTClient->GetFrameTime() * 0.025f;
		
		//SetupQuadUVs( m_hPolyCloud, m_hTxCloud, 0.0f + fCloudSceneAdjust, 0.0f, 2.0f, 1.0f );
		g_pDrawPrim->SetUVWH( &m_hPolyCloud, fCloudSceneAdjust, 0.0f, 2.0f, 1.0f );

		if ( fCloudSceneAdjust > 1.0f )
		{
			fCloudSceneAdjust -= 1.0f;
		}
	}

	return LTTRUE;
}

// Screen specific rendering
LTBOOL CScreenMain::Render(HSURFACE hDestSurf)
{
	SetRenderState();
	
	RenderCloud();		// NOVICE 2005. 03. 22.	밑바탕 구름그리기~~

	if( LTFALSE == CScreenBase::Render(hDestSurf) ) return LTFALSE;	

	m_pTextScroll->Update();
	m_pTextScroll->Render();

	g_pDrawPrim->SetTexture( m_hUIBottomL );
	g_pDrawPrim->DrawPrim( &m_PolyUIBottomL );
	g_pDrawPrim->SetTexture( m_hUIBottomR );
	g_pDrawPrim->DrawPrim( &m_PolyUIBottomR );

	m_pChannelInfo->Render();
	
	if( m_bDisableChat )
	{
		if( g_pLTClient->GetTime() - m_fDoBae > 20.0f )
		{
			m_bDisableChat = LTFALSE;
		}
	}
	
	return LTTRUE;
}


void CScreenMain::Escape()
{
	CScreenBase::Escape();
}

bool CScreenMain::CheckPassword()
{
	return true;
}

void CScreenMain::AddRoom( int nIndex,
						   const char* lpszTitle,
						   const char* lpszCaptain,
						   int nMapIndex,
						   int nCurrentUser,
						   int nMaxUser,
						   int nType,
						   unsigned char nPing,
						   LTBOOL bPlaying,
						   BOOL bPrivate,
						   int nRoundLimit,
						   int nKillLimit,
						   BOOL bInstrusion,
						   BOOL bTeamBalance )
{
	T_ROOM_ITEMptr pRoomItem	= new T_ROOM_ITEM;
	pRoomItem->nIndex			= nIndex;
	pRoomItem->nMapIndex		= nMapIndex;
	pRoomItem->nCurrentUser		= nCurrentUser;
	pRoomItem->nMaxUser			= nMaxUser;	
	pRoomItem->nType			= nType;
	pRoomItem->nPing			= nPing;
	pRoomItem->bPlaying			= bPlaying;
	pRoomItem->bPrivate			= bPrivate;	
	pRoomItem->nRoundLimit		= nRoundLimit;
	pRoomItem->nKillLimit		= nKillLimit;
	pRoomItem->bInstrusion		= bInstrusion;
	pRoomItem->bTeamBalance		= bTeamBalance;
	
	strcpy( pRoomItem->szCaptain, lpszCaptain );
	strcpy( pRoomItem->szTitle, lpszTitle );

	MISSION* pMission = g_pMissionButeMgr->GetMission( nMapIndex );
	if( pMission )
	{
		strcpy( pRoomItem->szType, NF_GetMsg( pMission->nGameTypeID ) );
		sprintf( pRoomItem->szMapName, "%s", NF_GetMsg( pMission->nNameId ) );
		pRoomItem->hScreenShot	= pMission->hScreenShot;
		pRoomItem->nType		= (int)pMission->eGameType;
	}
	else
	{
		strcpy( pRoomItem->szType, "??" );
		sprintf( pRoomItem->szMapName, "??" );
		pRoomItem->hScreenShot	= LTNULL;
	}	
	
	m_pRoomList->AddRoom( pRoomItem );

	if( m_pRoomList->IsEnableScroll() )
	{
		m_pBtnRoomScroll->Show(LTTRUE);
		m_pBtnRoomScroll->SetPosYRatio( m_pRoomList->GetRatio() );
	}
}

void CScreenMain::RemoveRoom( int nRoomIndex )
{
	m_pRoomList->RemoveRoom( nRoomIndex );

	if( m_pRoomList->IsEnableScroll() )
	{
		m_pBtnRoomScroll->SetPosYRatio( m_pRoomList->GetRatio() );
	}
	else
	{
		m_pBtnRoomScroll->Show(LTFALSE);
	}
}

void CScreenMain::AddUser( int nIndex, const char* lpszNick, unsigned int nGrade )
{
	T_USER_ITEMptr pUserItem = new T_USER_ITEM;
	pUserItem->nIndex	= nIndex;
	pUserItem->nGrade	= nGrade;
	strcpy( pUserItem->szNick, lpszNick );	
	m_pUserList->AddUser( pUserItem );

	if( m_pUserList->IsVisible() && m_pUserList->IsEnableScroll() )
	{
		m_pBtnUserScroll->Show(LTTRUE);
		m_pBtnUserScroll->SetPosYRatio( m_pUserList->GetRatio() );
	}
}

void CScreenMain::RemoveUser( int nIndex )
{
	m_pUserList->RemoveUser( nIndex );

	if( m_pUserList->IsVisible() )
	{
		if( m_pUserList->IsEnableScroll() )
		{
			m_pBtnUserScroll->SetPosYRatio( m_pUserList->GetRatio() );
		}
		else
		{
			m_pBtnUserScroll->Show(LTFALSE);
		}
	}
}

void CScreenMain::AddFriend( int nIndex, const char* lpszNick, uint32 nGrade, uint16 nStatus )
{
	T_FRIEND_ITEMptr pFriendItem = new T_FRIEND_ITEM;
	pFriendItem->nIndex		= nIndex;
	pFriendItem->nGrade		= nGrade;
	pFriendItem->nStatus	= nStatus;
	strcpy( pFriendItem->szNick, lpszNick );
	m_pFriendList->AddUser( pFriendItem );

	
	if( m_pFriendList->IsVisible() && m_pFriendList->IsEnableScroll() )
	{
		m_pBtnUserScroll->Show(LTTRUE);
		m_pBtnUserScroll->SetPosYRatio( m_pFriendList->GetRatio() );
	}
}

void CScreenMain::RemoveFriend( int nIndex )
{
	m_pFriendList->RemoveUser( nIndex );

	if( m_pFriendList->IsVisible() )
	{
		if( m_pFriendList->IsEnableScroll() )
		{
			m_pBtnUserScroll->SetPosYRatio( m_pFriendList->GetRatio() );
		}
		else
		{
			m_pBtnUserScroll->Show(LTFALSE);
		}
	}
}

void CScreenMain::AddChat( DWORD dwIdx, int32 nType /*= CHAT_TYPE_NORMAL*/ )
{
	AddChat( NF_GetMsg( dwIdx ), nType );
}

void CScreenMain::AddNotify( const char* lpszNoti )
{
	char* lpszToken;
	char  szTokBuffer[256];
	strcpy( szTokBuffer, lpszNoti );

	lpszToken = strtok( szTokBuffer, "\r\n" );
    while( lpszToken != NULL )
	{
		AddChat( lpszToken, CHAT_TYPE_ADMIN );
		lpszToken = strtok( NULL, "\r\n" );
	}
}

void CScreenMain::AddChat( const char *lpszChat, int32 nType )
{
	COLORREF rgbTextColor, rgbExtColor;
	switch( nType )
	{
	case CHAT_TYPE_NORMAL:
		rgbTextColor	= RGB(183,153,104);
		rgbExtColor		= RGB(0,0,0);
		break;
	case CHAT_TYPE_WHISPER:
		rgbTextColor	= RGB(190,190,190);
		rgbExtColor		= RGB(0,0,0);
		break;
	case CHAT_TYPE_ADMIN:
		rgbTextColor	= RGB(174,208,252);
		rgbExtColor		= RGB(0,0,0);
		break;
	case CHAT_TYPE_NOTICE:
		rgbTextColor	= RGB(255,255,255);
		rgbExtColor		= RGB(0,0,0);
		break;
	default:
		rgbTextColor	= RGB(255,255,255);
		rgbExtColor		= RGB(0,0,0);
		break;
	}
	m_pTextList->Append( lpszChat, D3D_TEXT_SHADOW, rgbTextColor, rgbExtColor, 0 );

	if( m_pTextList->IsEnableScroll() )
	{
		m_pBtnTextScroll->Show(LTTRUE);
		m_pBtnTextScroll->SetPosYRatio( m_pTextList->GetRatio() );
	}
}

void CScreenMain::ChangeOption( int nRoomIndex,
							    unsigned char nGameType,
								unsigned char nMapIndex,
								unsigned char nMaxUsers,
								unsigned char nTimeLimit, 
								unsigned char nKillLimit, 
								unsigned char nRoundLimit, 
								bool bIntrusion, 
								bool bTeamBalance,
								bool bFreeSpec )
{
	ChangeOptMapIndex( nRoomIndex, nMapIndex, nGameType );

	if( IsMissionMode((GameType)nGameType) )
	{
		nKillLimit = 0;
		MISSION* pMission = g_pMissionButeMgr->GetMission(nMapIndex);
		nTimeLimit = pMission ? pMission->nTimeLimit : 3 ;
	}
	else
	{
		nRoundLimit	= 0;
	}
	
	m_pRoomList->SetRoomOption( nRoomIndex,	
								nGameType,
								nMaxUsers,
								nKillLimit,
								nRoundLimit,
								bIntrusion,
								bTeamBalance );
}

void CScreenMain::ChangeRoomTitle( int nRoomIndex, const char* lpszTitle )
{
	if( lpszTitle )
	{
		m_pRoomList->SetRoomTitle( nRoomIndex, lpszTitle );
	}
}

void CScreenMain::ChangeOptMapIndex( int nRoomIndex, uint8 nMapIndex, uint8 nGameType )
{
	MISSION* pMission = g_pMissionButeMgr->GetMission( nMapIndex );
	if( pMission )
	{
		m_pRoomList->SetMapName( nRoomIndex, nMapIndex, pMission->hScreenShot, NF_GetMsg( pMission->nNameId ), nGameType, NF_GetMsg( pMission->nGameTypeID ) );
	}
}

void CScreenMain::ChangeOptMaxUsers( int nRoomIndex, short nMaxUsers )
{
	m_pRoomList->SetMaxUser( nRoomIndex, nMaxUsers );
}

void CScreenMain::ChangeGameLimit( int nRoomIndex, unsigned char nTimeLimit, unsigned char nKillLimit, unsigned char nRoundLimit )
{
	m_pRoomList->SetGameLimit( nRoomIndex, nTimeLimit, nKillLimit, nRoundLimit );
}

void CScreenMain::ChangeOptInstrusion( int nRoomIndex, LTBOOL bEnable )
{
	m_pRoomList->SetInstrusion( nRoomIndex, bEnable );
}

void CScreenMain::ChangeOptTeamBalance( int nRoomIndex, LTBOOL bEnable )
{
	m_pRoomList->SetTeamBalance( nRoomIndex, bEnable );
}

void CScreenMain::ChangeCaptain( int nRoomIndex, const char* lpszCaptain )
{
	m_pRoomList->SetCaptain( nRoomIndex, lpszCaptain );
}

void CScreenMain::IncRoomUser( int nRoomIndex )
{
	m_pRoomList->IncRoomUser( nRoomIndex );
}

void CScreenMain::DecRoomUser( int nRoomIndex )
{
	m_pRoomList->DecRoomUser( nRoomIndex );
}

void CScreenMain::ChangeRoomState( int nRoomIndex, LTBOOL bPlaying )
{
	m_pRoomList->SetRoomState( nRoomIndex, bPlaying );
}

void CScreenMain::SetPing( int nRoomIndex, uint8 nPing )
{
	m_pRoomList->SetPing( nRoomIndex, nPing );
}

void CScreenMain::ShowListTab()
{
//	m_pBtnOption->Show(LTTRUE);
	
	if( LTTRUE == m_pPopupOption->IsVisible() )
	{
		m_pPopupOption->CtrlHide();
	}
}

void CScreenMain::ShowOptionTab()
{
//	m_pBtnOption->Show(LTFALSE);

	if( LTFALSE == m_pPopupOption->IsVisible() )
	{
		m_pPopupOption->CtrlShow();
	}
}

LTBOOL CScreenMain::OnEnter()
{
	if( m_pDummyCtrl && m_pDummyCtrl->IsVisible() ) return LTFALSE;		
	if( m_pChatEdit ) m_pChatEdit->ForceAddEditBox();
	return LTTRUE;
}

LTBOOL CScreenMain::OnEscape()
{
	if( m_pDummyCtrl && m_pDummyCtrl->IsVisible() ) return LTFALSE;
	OnCommand( CMD_EXIT, LTNULL, LTNULL );
	return LTTRUE;
}

void CScreenMain::SortRoomList()
{
	m_pRoomList->SortGameType(LTTRUE);
}

void CScreenMain::OnRecvComplete( int nJoinRoomIndex )
{
	m_pChatEdit->ForceAddEditBox();

	if( nJoinRoomIndex >= 0 )
	{
		JoinRoom( -1, nJoinRoomIndex );
	}
}

LTBOOL CScreenMain::EditCommand( const char* lpszChatMessage )
{
	if( lpszChatMessage == NULL ) return LTFALSE;
	char szChatMessage[MAX_TEXT_BUFFER];
	char szNick[MAX_NICK_LEN];
	LPSTR lpszToken;
	unsigned int iOffset = 0;

	memset( szChatMessage, 0, MAX_TEXT_BUFFER );
	strcpy( szChatMessage, lpszChatMessage );
	lpszToken = strtok( szChatMessage, TEXT(" ") ); 
	if( lpszToken == NULL ) return FALSE;

	iOffset += strlen( lpszToken )+1;
	
	if( stricmp( lpszToken, "/W" ) == 0 ||
		//							  "/ㅈ"
		strcmp( lpszToken, NF_GetMsg(SA_TEXT_CMD_WHISPER) ) == 0 )
	{
		lpszToken = strtok( NULL, TEXT(" ") ); // <- ID
		if( lpszToken == NULL )
		{
			//"유효하지 않은 명령입니다."
			AddChat( SA_TEXT_INVALID_COMMAND, CHAT_TYPE_NOTICE );
			//"[귓말하기 ] /W <아이디> <메세지>"
			AddChat( SA_TEXT_HELP_WHISPER, CHAT_TYPE_NOTICE );
			m_pChatEdit->ClearText();
			return LTFALSE;
		}
		iOffset += strlen( lpszToken )+1;
		strncpy( szNick, lpszToken, MAX_NICK_LEN-1 );
		char szWhisper[MAX_TEXT_BUFFER];
		memset( szWhisper, 0, MAX_TEXT_BUFFER );
		if( iOffset >= strlen( lpszChatMessage ) )
		{
			//"유효하지 않은 명령입니다."
			AddChat( SA_TEXT_INVALID_COMMAND, CHAT_TYPE_NOTICE );
			//"[귓말하기 ] /W <아이디> <메세지>"
			AddChat( SA_TEXT_HELP_WHISPER, CHAT_TYPE_NOTICE );
			m_pChatEdit->ClearText();
			return LTFALSE;
		}
		memcpy( szWhisper, lpszChatMessage+iOffset, strlen( lpszChatMessage )-iOffset );
		if( strlen( szWhisper ) == 0 )
		{
			//"유효하지 않은 명령입니다."
			AddChat( SA_TEXT_INVALID_COMMAND, CHAT_TYPE_NOTICE );
			//"[귓말하기 ] /W <아이디> <메세지>"
			AddChat( SA_TEXT_HELP_WHISPER, CHAT_TYPE_NOTICE );
			m_pChatEdit->ClearText();
			return LTFALSE;
		}
		char szTemp[256];
		if( strcmp( szNick, g_GameDoc.GetNick() ) == 0 )
		{
			//"자신에게는 귓말을 보낼 수 없습니다."
			AddChat( SA_TEXT_SELF_WHISPER_FAIL, CHAT_TYPE_NOTICE );
			m_pChatEdit->ClearText();
			return LTFALSE;
		}
		

		wsprintf( szTemp, " [To:%s] %s", szNick, szWhisper );
		AddChat( szTemp, CHAT_TYPE_WHISPER );
		
		if( LTFALSE == IsCurse( szWhisper ) )
		{	
			g_Network.CS_Whisper( szNick, szWhisper );
		}

		m_pChatEdit->ClearText();
		return LTTRUE;
	}
	else if( stricmp( lpszToken, "/GO" ) == 0 ||
		//							"/해"
		strcmp( lpszToken, NF_GetMsg(SA_TEXT_CMD_GO) ) == 0 )
	{
		lpszToken = strtok( NULL, TEXT(" ") );
		if( lpszToken == NULL )
		{
			//"유효하지 않은 명령입니다."
			AddChat( SA_TEXT_INVALID_COMMAND, CHAT_TYPE_NOTICE );
			//"[방입장하기] /GO <방번호>"
			AddChat( SA_TEXT_HELP_ENTER_ROOM , CHAT_TYPE_NOTICE );
			m_pChatEdit->ClearText();
			return LTFALSE;
		}
		int nRoomID = atoi( lpszToken );
	
		m_pChatEdit->ClearText();
		JoinRoom( (void*)m_pRoomList->GetRoomData( nRoomID ) );
		return TRUE;
	}
	else if( strcmp( lpszToken, "/C" ) == 0 ||
		//							  "/ㅊ"
		strcmp( lpszToken, NF_GetMsg(SA_TEXT_CMD_CHAT_CLEAR) ) == 0 )
	{
		m_pTextList->DeleteAllItem();
		m_pBtnTextScroll->Show(LTFALSE);
		m_pChatEdit->ClearText();
		return TRUE;
	}	
	else if( stricmp( lpszToken, "/s" ) == 0 || stricmp( lpszToken, "/ㄴ" ) == 0 )
	{
		lpszToken = strtok( NULL, TEXT(" ") );
		if( lpszToken == NULL )
		{
			//"유효하지 않은 명령입니다."
			AddChat( SA_TEXT_INVALID_COMMAND, CHAT_TYPE_NOTICE );
			//"[사람찾기] /S <닉네임>"
			AddChat( SA_TEXT_HELP_SEARCH_USER_NICK, CHAT_TYPE_NOTICE );
			m_pChatEdit->ClearText();
			return LTFALSE;
		}		
		g_Network.CS_SearchUser( lpszToken );
		m_pChatEdit->ClearText();
		return LTTRUE;
	}
	else if( strcmp( lpszToken, "/?" ) == 0 )
	{
		//"[귓말하기] /W <아이디> <메세지>"
		AddChat( SA_TEXT_HELP_WHISPER, CHAT_TYPE_NOTICE );
		//"[답변하기] /R"
		AddChat( SA_TEXT_HELP_REPLY, CHAT_TYPE_NOTICE );
		//"[사람찾기] /S <닉네임>"
		AddChat( SA_TEXT_HELP_SEARCH_USER_NICK, CHAT_TYPE_NOTICE );
		//"[방입장하기] /GO <방번호>"
		AddChat( SA_TEXT_HELP_ENTER_ROOM , CHAT_TYPE_NOTICE );
		//"[채팅창 글씨지우기] /C"
		AddChat( SA_TEXT_HELP_CLEAR_CHAT , CHAT_TYPE_NOTICE );
		m_pChatEdit->ClearText();
	}
	
	return LTFALSE;
}

LTBOOL CScreenMain::JoinRoom( void* pRoomItem )
{
	if( LTNULL == pRoomItem ) return LTFALSE;
	
	T_ROOM_ITEMptr pRoomInfo = (T_ROOM_ITEMptr)pRoomItem;
	if( pRoomInfo->bPrivate )
	{
		ShowPasswordDlg();
	}
	else
	{
		g_GameDoc.SetTitle( pRoomInfo->szTitle );
		g_GameDoc.SetPassword( "" );
		g_Network.CS_JoinRoom( pRoomInfo->nIndex );
	}

	return LTTRUE;
}

LTBOOL CScreenMain::JoinRoom( int nChannelIndex, int nRoomIndex )
{
	if( nChannelIndex < 0 )
	{
		JoinRoom( m_pRoomList->GetRoomData( nRoomIndex ) );
	}
	else
	{
		//"채널을 이동 중 입니다.\r\n\r\n잠시만 기다려 주십시요."
		g_pInterfaceMgr->ShowMessageBox( SA_TEXT_HELP_NOW_TRY_MOVE_CHANNEL, LTMB_MESSAGE, LTNULL, LTNULL );
		g_Network.CS_ChannelQuickJoin( nChannelIndex, nRoomIndex );
	}

	return LTTRUE;
}

LTBOOL CScreenMain::JoinPrivateRoom( void* pRoomItem )
{
	if( LTNULL == pRoomItem ) return LTFALSE;
	
	T_ROOM_ITEMptr pRoomInfo = (T_ROOM_ITEMptr)pRoomItem;

	g_GameDoc.SetTitle( pRoomInfo->szTitle );
	g_Network.CS_JoinRoom( pRoomInfo->nIndex, g_GameDoc.GetPassword() );

	return LTTRUE;
}

void CScreenMain::ShowPasswordDlg()
{
	if( LTFALSE == m_pPopupPassword->IsVisible() )
	{
		m_pPopupPassword->CtrlShow();
	}
}

void CScreenMain::SearchResult( LTBOOL bSuccess, const char* lpszMsg, int nChannelIndex, int nRoomIndex )
{
	if( m_pPopupSearchResult->IsVisible() )
	{
		m_pPopupSearchResult->CtrlHide();
	}

	m_pPopupSearchResult->CtrlShow( bSuccess, lpszMsg, nChannelIndex, nRoomIndex, LTTRUE );
}

void CScreenMain::UserInfoReply( const char* lpszNick,
								 uint32 nGrade,
								 uint32 nKill, uint32 nDeath,
								 uint32 nWin, uint32 nLose, uint32 nDraw,
								 __int64 nExp,
								 uint8 nA_ModelID, uint8 nB_ModelID )
{
	if( m_pPopupUserInfo->IsVisible() ) m_pPopupUserInfo->CtrlHide();

	m_pPopupUserInfo->SetUserInfo( lpszNick, nGrade, nKill, nDeath, nWin, nLose, nDraw, nExp, 
								   nA_ModelID, 0, 0,
								   nB_ModelID, 0, 0 );

	m_pPopupUserInfo->m_hCharObject = m_CharacterSFX[0].GetObject();
	m_pPopupUserInfo->CtrlShow();
}

void CScreenMain::UpdateModel( int nModelID, int HeadSkinID, int nBodySkinID )
{
	HOBJECT hChar = m_CharacterSFX[0].GetObject();
	if( hChar )
	{
		ModelId  id;
		id = ModelId(nModelID);

		g_pLTClient->Common()->SetObjectFlags(hChar, OFT_Flags, 0, FLAG_VISIBLE );	

		ObjectCreateStruct createStruct;
		INIT_OBJECTCREATESTRUCT(createStruct);

		if( LTNULL == g_pModelButeMgr->GetModelFilename(id) )
		{
			id = static_cast<ModelId>( 0 );
		}

		SAFE_STRCPY(createStruct.m_Filename, g_pModelButeMgr->GetModelFilename(id));
		if( g_pModelButeMgr->GetSkinReader(id) )
		{
			g_pModelButeMgr->GetSkinReader(id)->CopyList(0, createStruct.m_SkinNames[0], MAX_CS_FILENAME_LEN+1);
		}

		//---------------------------------------------------------------------------
		char* szTemp;
		g_pModelButeMgr->GetModelHeadSkinFilename(id, HeadSkinID, szTemp);
		if( szTemp ) strcpy( createStruct.m_SkinNames[0], szTemp );
		g_pModelButeMgr->GetModelBodySkinFilename(id, nBodySkinID, szTemp);
		if( szTemp ) strcpy( createStruct.m_SkinNames[1], szTemp );
		//---------------------------------------------------------------------------

		g_pModelButeMgr->CopyRenderStyleFilenames( id, &createStruct );

		//g_pLTClient->SetModelAnimation(hChar, 0);
		g_pCommonLT->SetObjectFilenames(hChar, &createStruct);

		// 나중에 screen에서 사용할 Animation 작업
		uint32 dwAni = g_pLTClient->GetAnimIndex(hChar, "IF_SELECT");
		if( dwAni != INVALID_ANI )
		{
			g_pLTClient->SetModelAnimation(hChar, dwAni);
		}

		/*
		ClearAttachFX();
		
		//LTVector vScale = m_CharSFX.GetInitialScale();
		LTVector vScale = m_CharacterSFX[0].GetInitialScale();
		float fScale = vScale.x;
		uint8 nDefaultAttachments = g_pModelButeMgr->GetNumDefaultAttachments( id );

		const char *pszAttachmentPos;
		const char *pszAttachment;

		for( int8 i = 0; i < nDefaultAttachments; ++i )
		{
			INT_ATTACH acs;
			
			g_pModelButeMgr->GetDefaultAttachment( id, i, pszAttachmentPos, pszAttachment );
			acs.nAttachmentID	= g_pAttachButeMgr->GetAttachmentIDByName( pszAttachment );
			acs.fScale			= fScale;
			LTStrCpy( acs.szSocket, pszAttachmentPos, ARRAY_LEN( acs.szSocket ));						
			CreateAttachFX( &acs );
			//CreateAttachFX( &acs, 0 );
		}
		*/
		CBaseScreen::Update_Model();
	}
}

void CScreenMain::PrePopupState()
{
	SetSelection(kNoSelection);
	m_pChatEdit->OnLostFocus();
}

void CScreenMain::PostPopupState()
{
}

LTBOOL CScreenMain::IsMissionMode( GameType eGameType )
{
	switch( eGameType )
	{
	case eGameTypeTeamDeathmatch:
		return LTFALSE;
	default:
		return LTTRUE;
	}
}

void CScreenMain::SetRanking()
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

void CScreenMain::CheckDoBae()
{
	if( g_pLTClient->GetTime() - m_fChatTime < 0.8f )
	{
		++m_nChatCount;
	}
	else
	{
		m_nChatCount = 0;
	}

	m_fChatTime = g_pLTClient->GetTime();
	
#ifdef _FINAL
	if( m_nChatCount > 3 )
	{
		AddChat( "도배하셨으므로 20초간 채팅을 금지합니다.", CHAT_TYPE_ADMIN );
		m_fDoBae = m_fChatTime;
		m_nChatCount = 0;
		m_bDisableChat = LTTRUE;
	}
#endif
}

LTBOOL CScreenMain::OnLButtonDown( int x, int y )
{
	LTBOOL bResult = CScreenBase::OnLButtonDown( x, y );
	if( m_pPopupMenu->IsVisible() ) m_pPopupMenu->HidePopupMenu();
	return bResult;
}

void CScreenMain::ShowInviteMsg( const char* lpszNick, int nRoomIndex, const char* lpszPass )
{
	//if( m_pInviteMsg->IsVisible() ) return;
	//[yaiin] : 다른 팝업 뜬 상태에는 초대 팝업을 받지 않는다.
	if( m_pDummyCtrl && m_pDummyCtrl->IsVisible() ) return;

	m_pInviteMsg->CtrlShow( lpszNick, nRoomIndex, lpszPass );
}

void CScreenMain::EnterShop()
{
	g_pScreenInventory->SetShop(true);
	g_pInterfaceMgr->SwitchToScreen(SCREEN_ID_INVENTORY);
}