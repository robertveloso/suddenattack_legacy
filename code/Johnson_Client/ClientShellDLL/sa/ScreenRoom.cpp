#include "stdafx.h"
#include "ScreenRoom.h"
#include "ScreenMgr.h"
#include "ScreenCommands.h"
#include "MissionMgr.h"
#include "clientmultiplayermgr.h"
#include "GameClientShell.h"
#include "SAPlayerStats.h"
#include "PopupScreenCreateRoom.h"
#include "SAHUDMgr.h"
#include "CurseFilter.h"
#include "PopupSearchResult.h"
#include "PopupUserInfo.h"
#include "PopupInvite.h"
#include "PopupMenu.h"
#include "PopupHelp.h" //[yaiin]
#include "ScreenInventory.h"
#include "TextScroll.h"

//------------------------------------------- (B) Novice (minjin) [03/12/2005]
#include "GameText.h"
#include "GameMessage.h"
//------------------------------------------- (E) Novice (minjin) [03/12/2005]

#include "SAMapList.h"
CScreenRoom *g_pScreenRoom = NULL;

namespace
{	
	// Character Model Test
	//------------------------------------------------
	#define INVALID_ANI			((HMODELANIM)-1)
	//------------------------------------------------
	
#define ROOM_TEXTURE_SIZE			32.0f

	float	g_fClickTime	= 0.0f;
	#define GAMESTART_TIME	40

	int g_nKickPlayerIndex = -1;
	int g_nNewCaptainIndex = -1;

	void KickCallBack(LTBOOL bReturn, void *pData)
	{
		if( bReturn && g_nKickPlayerIndex >= 0 )
		{
			g_Network.CS_KickBan( g_nKickPlayerIndex );
		}
	};

	void ChangeCaptainCallBack(LTBOOL bReturn, void *pData)
	{
		if( bReturn && g_nNewCaptainIndex >= 0 )
		{
			g_Network.CS_ChangeCaptain( g_nNewCaptainIndex );
		}
	};	

	/*
#ifdef BIG_FAT_MODE_TEST
	enum eLocalCommands
	{
		CMD_CHECK_NORMAL	= CMD_CUSTOM+300,
		CMD_CHECK_BIG,
		CMD_CHECK_FAT,
	};

	CLTGUIFrame*		g_pFrameModeTest[3];
	CLTGUIButton*		g_pCheckModeTest[3];
#endif
	*/
}

CScreenRoom::CScreenRoom()
{
	g_pScreenRoom	= this;

	//----------------------------------------------------
	// UI 다시 작업
	//----------------------------------------------------
	m_pListBlueTeam				= LTNULL;
	m_pListRedTeam				= LTNULL;

	m_pBtnReady					= LTNULL;
	m_pBtnBack					= LTNULL;
	m_pBtnArrowTeamLeft			= LTNULL;
	m_pBtnArrowTeamRight		= LTNULL;
	m_pBtnRoomHelp				= LTNULL; //[yaiin]
	
	m_pInfoNick					= LTNULL;
	m_pInfoNickCap				= LTNULL;
	m_pInfoGrade				= LTNULL;
	m_pInfoGradeCap				= LTNULL;
	m_pInfoWinPercentCap		= LTNULL;
	m_pInfoWinPercent			= LTNULL;
	m_pInfoKillDeathCap			= LTNULL;
	m_pInfoKillDeath			= LTNULL;
	m_pInfoExpCap				= LTNULL;
	m_pInfoExp					= LTNULL;
	m_pInfoCashCap				= LTNULL;
	m_pInfoCash					= LTNULL;
	m_pInfoGameMoneyCap			= LTNULL;
	m_pInfoGameMoney			= LTNULL;

	m_pPlayers					= LTNULL;	// 인원수
	m_pGameType					= LTNULL;	// 게임타입
	m_pGTGoal					= LTNULL;	// 라운드 제한
	m_pGTTimeLimit				= LTNULL;	// 게임시간 제한
	m_pIntrusion				= LTNULL;	// 전투중 참여
	m_pTeamBalance				= LTNULL;	// 팀 밸런스
	
	m_pPlayersCap				= LTNULL;
	m_pGameTypeCap				= LTNULL;
	m_pGTGoalCap				= LTNULL;
	m_pGTTimeLimitCap			= LTNULL;
	m_pIntrusionCap				= LTNULL;
	m_pTeamBalanceCap			= LTNULL;
	
	m_pMapTitle					= LTNULL;
	m_pMapComboBox				= LTNULL;
	m_pBtnMapDown				= LTNULL;
	m_pRoomName					= LTNULL;
	// chatting
	m_pBtnTextScroll			= LTNULL;
	m_pBtnTextScrollUp			= LTNULL;
	m_pBtnTextScrollDown		= LTNULL;
	m_pTextList					= LTNULL;
	m_pChatEdit					= LTNULL;
	m_pPopupSearchResult		= LTNULL;
	m_pPopupRoomOption			= LTNULL;
	m_bReady					= LTFALSE;
	m_pPopupUserInfo			= LTNULL;
	m_pPopupInvite				= LTNULL;
	m_bForceExitTimeStart		= LTFALSE;
	m_pMapFrame					= LTNULL;
	m_pTextScroll				= LTNULL;
	m_pChannelInfo				= LTNULL;
	m_pPopupMenu				= LTNULL;
	m_pPopupMenuCaptain			= LTNULL;
	m_pPopupRoomHelp			= LTNULL; //[yaiin]
}

CScreenRoom::~CScreenRoom()
{
	SAFE_DELETE( m_pInfoNickCap			);
	SAFE_DELETE( m_pInfoNick			);
	SAFE_DELETE( m_pInfoGradeCap		);
	SAFE_DELETE( m_pInfoGrade			);
	SAFE_DELETE( m_pInfoWinPercentCap	);
	SAFE_DELETE( m_pInfoWinPercent		);
	SAFE_DELETE( m_pInfoKillDeathCap	);
	SAFE_DELETE( m_pInfoKillDeath		);
	SAFE_DELETE( m_pInfoExpCap			);
	SAFE_DELETE( m_pInfoExp				);
	SAFE_DELETE( m_pInfoCashCap			);
	SAFE_DELETE( m_pInfoCash			);
	SAFE_DELETE( m_pInfoGameMoneyCap	);
	SAFE_DELETE( m_pInfoGameMoney		);
	SAFE_DELETE( m_pRoomName			);

	SAFE_DELETE( m_pPlayersCap			);
	SAFE_DELETE( m_pPlayers				);	// 인원수
	SAFE_DELETE( m_pGameTypeCap			);
	SAFE_DELETE( m_pGameType			);	// 게임타입
	SAFE_DELETE( m_pGTGoalCap			);
	SAFE_DELETE( m_pGTGoal				);	// 라운드 제한
	SAFE_DELETE( m_pGTTimeLimitCap		);
	SAFE_DELETE( m_pGTTimeLimit			);	// 게임시간 제한
	SAFE_DELETE( m_pIntrusion			);	// 전투중 참여
	SAFE_DELETE( m_pTeamBalance			);	// 팀 밸런스
	SAFE_DELETE( m_pIntrusionCap		);
	SAFE_DELETE( m_pTeamBalanceCap		);
	
	SAFE_DELETE( m_pMapTitle			);

	SAFE_DELETE( m_pChannelInfo );
	SAFE_DELETE( m_pTextScroll );
}

void CScreenRoom::Term()
{
	CScreenBase::Term();
}

void CScreenRoom::CreateBackWindow(eScreenID id)
{
	// 구름 Texture
	m_hTxCloud = g_pInterfaceResMgr->GetTexture( "SA_INTERFACE/MENU/TEXTURES/COMMON/BackFrameCloud.dtx" );
	SetupQuadUVs( m_hPolyCloud, m_hTxCloud, 0.0f, 0.0f, 2.0f, 1.0f );
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

LTBOOL CScreenRoom::Build()
{
	CreateBackWindow(SCREEN_ID_SCREEN_ROOM);
	
	////////////////////////////////////////////////////////////////////////////////////////
	// ComboBox ///////////////////////////////////////////////////
	
	// Drag & Scroll
	m_pScrollUp		= AddButton(CMD_SCROLL_UP, "ScrollMapUpRt",
								"ScrollUpN", "ScrollUpH",
								"ScrollUpD", "ScrollUpC", LTTRUE);
	m_pScrollDown	= AddButton(CMD_SCROLL_DOWN, "ScrollMapDnRt",
								"ScrollDnN", "ScrollDnH",
								"ScrollDnD", "ScrollDnC", LTTRUE);
	
	m_pDragBtn		= AddDragButton( CMD_DRAG_UPDOWN, "ScrollMapRt",
									"ScrollHandleN", "ScrollHandleH",
									NULL, "ScrollHandleC" );
	
	m_pDragBtn->SetLockX(LTTRUE);
	m_pDragBtn->SetLimitY( 95+200, 140+200 );
	
	m_pScrollUp->Show(LTFALSE);
	m_pScrollDown->Show(LTFALSE);
	m_pDragBtn->Show(LTFALSE);
	
	char* pScreenTag = (char*)g_pInterfaceMgr->GetScreenMgr()->GetScreenName((eScreenID)SCREEN_ID_SCREEN_ROOM);
	
	char szTempCombo[64] = {LTNULL};
	char szTempCombo2[64] = {LTNULL};
	LTRect	rt;
	LTIntPt BasePos;
	uint8 TotalCount = 0;
	uint8 nInitSelect = 0;
	const uint8 nListFixCount = 6;
	MISSION* pMission;

	pMission = g_pMissionButeMgr->GetMission(0);		
	if( pMission )
	{
		m_pMapFrame = AddFrame( "MapSelectPos", pMission->hScreenShot );
	}
	
	////////////////////////////////////////////////////////////////////////////////////////
	int nMissionCount = g_pMissionButeMgr->GetNumMissions();
	
	m_pMapComboBox		= new CSAMapList();
	nInitSelect			= 0;
	rt					= g_pLayoutMgr->GetRect(pScreenTag, "MapItemPos" );
	
	m_pMapComboBox->Create( 16, 258, 164, LTMIN(nMissionCount,8)*20, 20, this, CMD_SELECT_MAP, LTTRUE,
							0xFF0E1319, 0xFF676D6D );

	AddControl( m_pMapComboBox );//, BasePos );
	m_pMapComboBox->Show(LTFALSE);
	
	// Map Texture
	//------------------------------------------------------------------------------------------		
	
		
	for ( int mission = 0; mission <  nMissionCount ; mission++ )
	{
		MISSION* pMission = g_pMissionButeMgr->GetMission(mission);
		
		if( pMission )
		{
			LPMapData lpMapData		= new MapData;
			lpMapData->dwMapIndex	= mission;
			lpMapData->pMission		= pMission;

			lstrcpy( lpMapData->szMapName, NF_GetMsg(pMission->nNameId) );

			switch ( lpMapData->pMission->eGameType )
			{
			case eGameTypeTeamDeathmatch:	lstrcpy( lpMapData->szMapType, NF_GetMsg( SA_TEXT_TEAM_DM ) );		break;
			case eGameTypeSurvival:			lstrcpy( lpMapData->szMapType, NF_GetMsg( SA_TEXT_BOMB_MISSION ) ); break;
			default:
				lstrcpy( lpMapData->szMapType, "" );
				break;
			}
			
			m_pMapComboBox->Insert( lpMapData );
		}
	}

	m_pMapComboBox->RenderItemText();
	
	m_pBtnMapDown = AddButton( CMD_MAP_DOWN, "BtnMapDownRt",
								"BtnMapDownN", "BtnMapDownH",
								"BtnMapDownD", "BtnMapDownC", LTTRUE );

	// 전투장소 표시
	m_pMapTitle	= new CSAD3DText();
	m_pMapTitle->CreateD3DText( 90, 16, true );
	m_pMapTitle->SetPosition( 91, 242 );
	m_pMapTitle->SetTransparent( true );
	m_pMapTitle->SetTextColor( RGB(255,255,255) );
	m_pMapTitle->DrawD3DText( "", 0 );


	//------------------------------------------------------------------------------------------
	
	// 좌측 버튼들
	m_pBtnReady				= AddButton( CMD_ROOM_BTN_READY, "BtnReadyRt",
										"BtnReadyN", "BtnReadyH",
										"BtnReadyD", "BtnReadyC", LTTRUE);	
	m_pBtnInventory			= AddButton( CMD_ROOM_BTN_CHARACTER, "BtnCharacterRt",
										"BtnCharacterN", "BtnCharacterH",
										"BtnCharacterD", "BtnCharacterC", LTTRUE);	
	m_pBtnRoomOption		= AddButton( CMD_ROOM_BTN_OPTION, "BtnRoomOptionRt",
										"BtnRoomOptionN", "BtnRoomOptionH",
										"BtnRoomOptionD", "BtnRoomOptionC", LTTRUE);

	m_pBtnInvite			= AddButton( CMD_ROOM_BTN_INVITE, "BtnInviteRt",
										"BtnInviteN", "BtnInviteH",
										"BtnInviteD", "BtnInviteC", LTTRUE );

	g_pDrawPrim->SetXYWH( &m_PolyLightButton, 245.0f, 26.0f, 128.0f, 32.0f );
	g_pDrawPrim->SetUVWH( &m_PolyLightButton, 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGB( &m_PolyLightButton, 0xFFFFFF );
	
	m_pBtnReady->SetClickSound( (int)IS_CLICK_RADIO );
	m_pBtnInventory->SetClickSound( (int)IS_CLICK_RADIO );
	m_pBtnRoomOption->SetClickSound( (int)IS_CLICK_RADIO );

	// 상단 버튼
	m_pBtnBack				= AddButton( CMD_ROOM_BTN_BACK, "TBtnExitRt",
										"TBtnExitN", "TBtnExitH",
										"TBtnExitD", "TBtnExitC", LTTRUE);
	m_pBtnRoomHelp			= AddButton( CMD_ROOM_BTN_HELP, "BtnRoomHelpRt",
										"BtnRoomHelpN", "BtnRoomHelpH",
										"BtnRoomHelpD", "BtnRoomHelpC", LTTRUE); //[yaiin]

	m_pBtnRoomHelp->SetClickSound( (int)IS_CLICK_RADIO ); //[yaiin]

	// 팀 변경
	m_pBtnArrowTeamLeft		= AddButton( CMD_ROOM_BTN_TEAMLEFT, "BtnTeamLeftRt",
										"BtnTeamChangeR_N", "BtnTeamChangeR_H",
										"BtnTeamChangeR_D", "BtnTeamChangeR_C", LTTRUE);
	m_pBtnArrowTeamRight	= AddButton(CMD_ROOM_BTN_TEAMRIGHT, "BtnTeamRightRt",
										"BtnTeamChangeB_N", "BtnTeamChangeB_H",
										"BtnTeamChangeB_D", "BtnTeamChangeB_C", LTTRUE);


	m_pBtnReady->SetTooltip( "Ctrl+R" );		m_pBtnReady->SetHotKey( 0x12 );	
	m_pBtnInventory->SetTooltip( "Ctrl+I" );	m_pBtnInventory->SetHotKey( 0x09 );	
	m_pBtnRoomOption->SetTooltip( "Ctrl+O" );	m_pBtnRoomOption->SetHotKey( 0x0f );	
	m_pBtnBack->SetTooltip( "Ctrl+X" );			m_pBtnBack->SetHotKey( 0x18 );
	m_pBtnInvite->SetTooltip( "Ctrl+V" );		m_pBtnInvite->SetHotKey( 0x16 );
	m_pBtnRoomHelp->SetTooltip( "F1" ); //[yaiin] 핫키는 KeyDown으로 직접
	
	BuildRoomInfo( 32, 434, 214, 111 );
	BuildUserInfo( 582, 434, 200, 111 );

	// 방 번호, 이름, 비번
	m_pRoomName	= new CSAD3DText();
	m_pRoomName->CreateD3DText( 400, 16, true );
	m_pRoomName->SetPosition( 244.0f, 66.0f);
	m_pRoomName->SetTransparent( true );
	m_pRoomName->SetTextColor( RGB(180,174,145) );
	m_pRoomName->DrawD3DText( "", 0 );
	
	// 채팅 기록
	m_pTextList	= new CSaListText();
	m_pTextList->Create( 226, 427, 315, 105, 15, this );
	m_pTextList->SetMaxItemCount( 20 );	//History Count
	AddControl( m_pTextList );
	
	// 채팅 기록 스크롤바
	m_pBtnTextScrollUp		= AddButton(CMD_ROOM_CHAT_SCROLL_UP, "ScrollTextUpRt",
										"ScrollUpN", "ScrollUpH",
										"ScrollUpD", "ScrollUpC", LTTRUE );
	m_pBtnTextScrollDown	= AddButton(CMD_ROOM_CHAT_SCROLL_DOWN, "ScrollTextDnRt",
										"ScrollDnN", "ScrollDnH",
										"ScrollDnD", "ScrollDnC", LTTRUE);	
	
	// 채팅 입력창
	m_pChatEdit	= new CLTGUIEditBox();
	m_pChatEdit->Create( 226, 532, 328, 24,
						RGB(183,153,104), RGB(0,0,0), 0xFFB79968,
						D3D_TEXT_SHADOW, 
						LTFALSE,
						CMD_ROOM_CHAT_EDIT, this );
	AddControl( m_pChatEdit );

	m_pBtnTextScroll		= AddDragButton(CMD_ROOM_CHAT_SCROLL, "ScrollTextRt",
											"ScrollHandleN", "ScrollHandleH",
											NULL, "ScrollHandleC" );
	m_pBtnTextScroll->SetLockX(LTTRUE);
	m_pBtnTextScroll->SetLimitY( 441, 487 );
	
	// 팀 리스트 (좌)
	m_pListRedTeam				= new CSaListTeam();	
	m_pListRedTeam->Create( 227, 147, 271, 248, 31, this );
	AddControl(m_pListRedTeam);

	// 팀 리스트 (우)
	m_pListBlueTeam				= new CSaListTeam();
	m_pListBlueTeam->Create( 511, 147, 271, 248, 31, this );
	AddControl(m_pListBlueTeam);

	// 여기 [start button ] 0 : ready , 1 : start
	GET_TEXTURE_FROM_LAYOUT_MNGR( m_hReadyBtnTexture[0], "ScreenRoom", "BtnReadyN" );
	GET_TEXTURE_FROM_LAYOUT_MNGR( m_hReadyBtnTexture[1], "ScreenRoom", "BtnReadyH" );
	GET_TEXTURE_FROM_LAYOUT_MNGR( m_hReadyBtnTexture[2], "ScreenRoom", "BtnReadyD" );
	GET_TEXTURE_FROM_LAYOUT_MNGR( m_hReadyBtnTexture[3], "ScreenRoom", "BtnReadyC" );
	GET_TEXTURE_FROM_LAYOUT_MNGR( m_hReadyBtnTexture[4], "ScreenRoom", "BtnStartGameN" );
	GET_TEXTURE_FROM_LAYOUT_MNGR( m_hReadyBtnTexture[5], "ScreenRoom", "BtnStartGameH" );
	GET_TEXTURE_FROM_LAYOUT_MNGR( m_hReadyBtnTexture[6], "ScreenRoom", "BtnStartGameD" );
	GET_TEXTURE_FROM_LAYOUT_MNGR( m_hReadyBtnTexture[7], "ScreenRoom", "BtnStartGameC" );


	m_pPopupMenu = new CPopupMenu();
	m_pPopupMenu->Create( 0, 0, 80, 20*2, 20, this, LTNULL, LTTRUE, 0xFF17202A, 0xFF676D6D );
	AddControl( m_pPopupMenu );
	m_pPopupMenu->AddButton( NF_GetMsg(SA_TEXT_MENU_USER_INFO), CMD_USER_INFO );
	m_pPopupMenu->AddButton( NF_GetMsg(SA_TEXT_MENU_WHISPER), CMD_WHISPER );
	//m_pPopupMenu->AddButton( NF_GetMsg(SA_TEXT_MENU_ADD_FRIEND), CMD_ADD_FRIENT_LIST );

	m_pPopupMenuCaptain = new CPopupMenu();
	m_pPopupMenuCaptain->Create( 0, 0, 80, 20*4, 20, this, LTNULL, LTTRUE, 0xFF0E1319, 0xFF676D6D );
	AddControl( m_pPopupMenuCaptain );
	m_pPopupMenuCaptain->AddButton( NF_GetMsg(SA_TEXT_MENU_USER_INFO), CMD_USER_INFO );
	m_pPopupMenuCaptain->AddButton( NF_GetMsg(SA_TEXT_MENU_WHISPER), CMD_WHISPER );
	m_pPopupMenuCaptain->AddButton( NF_GetMsg(SA_TEXT_MENU_CHANGE_CAPTAIN), CMD_CHANGE_CAPTAIN );
	m_pPopupMenuCaptain->AddButton( NF_GetMsg(SA_TEXT_MENU_KICK), CMD_KICK_PLAYER );
	//m_pPopupMenu->AddButton( NF_GetMsg(SA_TEXT_MENU_ADD_FRIEND), CMD_ADD_FRIENT_LIST );

	//============================================================
	AddDummyCtrl();
	
	m_pPopupRoomOption = new CPopRoomOption();
	m_pPopupRoomOption->Init( this );
	AddControl((CLTGUICtrl*)m_pPopupRoomOption);

	m_pPopupSearchResult = new CPopupSearchResult();
	m_pPopupSearchResult->Init(this);
	AddControl((CLTGUICtrl*)m_pPopupSearchResult);

	m_pPopupUserInfo = new CPopupUserInfo();
	m_pPopupUserInfo->Init(this);
	AddControl((CLTGUICtrl*)m_pPopupUserInfo);

	m_pPopupInvite = new CPopupInvite();
	m_pPopupInvite->Init(this);
	AddControl((CLTGUICtrl*)m_pPopupInvite);

	//[yaiin]
	m_pPopupRoomHelp = new CPopupHelp();
	m_pPopupRoomHelp->Init(this);
	AddControl((CLTGUICtrl*)m_pPopupRoomHelp);

	m_hStartIcon = g_pInterfaceResMgr->GetTexture( "SA_INTERFACE\\Menu\\Textures\\Room\\BtnStartGame2H.dtx" );
	g_pDrawPrim->SetXYWH( &m_PolyStartIcon, 23.0f, 104.0f, 256.0f, 32.0f );
	g_pDrawPrim->SetUVWH( &m_PolyStartIcon, 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_PolyStartIcon, 0xFFFFFFFF );

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
	///////////////////////////////////////////////////////////////////////////////

/*
#ifdef BIG_FAT_MODE_TEST
	HTEXTURE hModeTexture = LTNULL;
	char szMode[128] = {0, };
	char szPos[128]	 = {0, };
	for(int i = 0; i < 3; i++ )
	{
		sprintf( szMode, "sa_interface\\menu\\textures\\room\\model_%d.dtx", i);
		sprintf( szPos, "FrameModePos_%d", i);
		hModeTexture = g_pInterfaceResMgr->GetTexture(szMode);
		g_pFrameModeTest[i] = AddFrame( szPos, hModeTexture );
		sprintf( szPos, "CheckModePos_%d", i);
		g_pCheckModeTest[i]	= AddButton(CMD_CHECK_NORMAL+i, szPos, "ModeCheckOff", LTNULL, 0, "ModeCheckOn");
		g_pCheckModeTest[i]->SetParam1(i);
	}
	g_pCheckModeTest[0]->SetCheck();
#endif	
	*/
	//------------------------------------------------------------------	
	// Make sure to call the base class
	return CScreenBase::Build();
}

// UI 다시 작업
//---------------------------------------------
void CScreenRoom::AddRoomUser(int nIndex, const char* szNick, int nGrade, int nTeam, int nState, LTBOOL bCaptain )
{
	T_TEAM_ITEMptr pUserItem = new T_TEAM_ITEM;
	pUserItem->nIndex		= nIndex;
	pUserItem->nGrade		= nGrade;
	pUserItem->nTeamType	= nTeam;
	pUserItem->nState		= nState;
	pUserItem->bCaptain		= bCaptain;
	pUserItem->hMark		= LTNULL;
	pUserItem->szClan[0]	= LTNULL;
	strcpy( pUserItem->szNick, szNick );

	if( nTeam == TEAM_A )
	{
		m_pListRedTeam->AddUser(pUserItem);
	}
	else
	{
		m_pListBlueTeam->AddUser(pUserItem);
	}

	if( g_GameDoc.GetUniqueIndex() == nIndex )
	{
		//============================================
		switch( nTeam )
		{
			case 0 :
				{
					m_nCurrentModel = 0;
					m_nCurrentSkinID = 1;
				}break;
			case 1 :
				{
					m_nCurrentModel = 0;
					m_nCurrentSkinID = 0;
				}break;
			default :
				{
					m_nCurrentModel = 0;
					m_nCurrentSkinID = 0;
				}break;
		}
		//============================================
		g_GameDoc.SetTeam( nTeam );
	}

	--m_nRecvPlayerCount;

	if( m_nRecvPlayerCount <= 0 && LTFALSE == m_bRecvComplete )
	{
		RecvComplete();
	}
}

void CScreenRoom::RecvComplete()
{
	if( m_bRecvComplete ) return;
	
	m_bRecvComplete = LTTRUE;

	m_pChatEdit->ForceAddEditBox();

	if( g_pInterfaceMgr->IsExitMessage() )
	{
		g_pInterfaceMgr->ShowMessageBox( g_pInterfaceMgr->GetExitMessage(), LTMB_OK, LTNULL, LTNULL );
		g_pInterfaceMgr->SetExitMessage( LTNULL );
	}
	else
	{
		g_pInterfaceMgr->CloseMessageBox(true);
	}

	if ( g_pInterfaceMgr->IsServerPatchExist() )
	{
		g_pInterfaceMgr->ShowMessageBox( g_pInterfaceMgr->GetServerPatchNotice(), LTMB_OK, CNetwork::DisconnectCallBack, LTNULL );
	}

	SetInstrution( m_bIsInstrusion );
	SetStartReadyButton();
}

void CScreenRoom::CaptainChange( int nIndex )
{
	m_pListRedTeam->ChangeCaptain( nIndex );
	m_pListBlueTeam->ChangeCaptain( nIndex );	
	SetStartReadyButton();
}

void CScreenRoom::ChangeTeam( int nIndex, int nTeam )
{
	if( nTeam )
	{
		T_TEAM_ITEMptr pUser =	m_pListRedTeam->GetUserData(nIndex);
		if( pUser )
		{
			T_TEAM_ITEMptr pUserItem = new T_TEAM_ITEM;
			memcpy( pUserItem, pUser, sizeof(T_TEAM_ITEM));
			m_pListBlueTeam->AddUser( pUserItem );
			m_pListRedTeam->RemoveUser( nIndex );
			if( g_GameDoc.GetUniqueIndex() == nIndex )
			{
				g_GameDoc.SetTeam( nTeam );
				//------------------------------------------------
				m_nCurrentModel		= 0;
				m_nCurrentSkinID	= 0;
				//------------------------------------------------
			}			
		}
	}
	else
	{
		T_TEAM_ITEMptr pUser =	m_pListBlueTeam->GetUserData(nIndex);
		if( pUser )
		{
			T_TEAM_ITEMptr pUserItem = new T_TEAM_ITEM;
			memcpy( pUserItem, pUser, sizeof(T_TEAM_ITEM));
			m_pListRedTeam->AddUser( pUserItem );
			m_pListBlueTeam->RemoveUser( nIndex );
			if( g_GameDoc.GetUniqueIndex() == nIndex )
			{
				g_GameDoc.SetTeam( nTeam );
				//------------------------------------------------
				m_nCurrentModel = 0;
				m_nCurrentSkinID = 1;
				//------------------------------------------------
			}			
		}
	}
}

void CScreenRoom::RemoveRoomUser( int nIndex )
{
	if( LTFALSE == m_pListRedTeam->RemoveUser( nIndex ) )
	{
		m_pListBlueTeam->RemoveUser( nIndex );
	}
}

void CScreenRoom::EnterBufferRoom( LTBOOL bValue )
{
	char szTemp[256] = {0,};
	m_bIsInstrusion = bValue;

	if( strlen( g_GameDoc.GetPassword() ) > 0 )
	{
		sprintf( szTemp, "[%d] %s (%s)", g_GameDoc.GetRoomIndex(), 
										 g_GameDoc.GetTitle(),
										 g_GameDoc.GetPassword() );
	}
	else
	{
		sprintf( szTemp, "[%d] %s", g_GameDoc.GetRoomIndex(), 
									g_GameDoc.GetTitle() );
	}
	m_pRoomName->DrawD3DText( szTemp, strlen(szTemp), DT_LEFT,D3D_TEXT_NORMAL, true  );
	
	SetUserInfo( g_GameDoc.GetGrade(),
				 g_GameDoc.GetNick(),
				 g_GameDoc.GetKill(),
				 g_GameDoc.GetDeath(),
				 g_GameDoc.GetWinCount(),
				 g_GameDoc.GetLoseCount(),
				 g_GameDoc.GetDrawCount(),
				 g_GameDoc.GetExp(),
				 g_GameDoc.GetMoney() );
	
	SetStartReadyButton();
}

void CScreenRoom::RemoveAllTeam()
{
	m_pListRedTeam->DeleteAllItem();
	m_pListBlueTeam->DeleteAllItem();
}

void CScreenRoom::BuildUserInfo( uint16 nPosX, uint16 nPosY, uint16 nWidth, uint16 nHeight )
{
	const int nLineCnt = 6;
	
	int nRow		= 0;
	int nTopAdjust	= 0;
	int nLeftAdjust	= 0;
	int nTextHeight	= 20;

	int nCaptionWidth = 75;
	int nContextWidth = nWidth - nCaptionWidth;

	nLeftAdjust = nPosX + nCaptionWidth;
	
	char szTemp[256] = {LTNULL};

	//"닉네임"
	lstrcpy( szTemp, NF_GetMsg(SA_TEXT_USER_INFO_NICK) );
	m_pInfoNickCap			= new CSAD3DText();
	m_pInfoNickCap->CreateD3DText( nCaptionWidth, nTextHeight, true );
	m_pInfoNickCap->SetPosition( nPosX, nPosY + nTextHeight * nRow );
	m_pInfoNickCap->SetTransparent( true );
	m_pInfoNickCap->SetTextColor( RGB(198,156,109) );
	m_pInfoNickCap->DrawD3DText( szTemp, lstrlen(szTemp), DT_LEFT, D3D_TEXT_OUTLINE );
	
	m_pInfoNick				= new CSAD3DText();
	m_pInfoNick->CreateD3DText( nContextWidth, nTextHeight, true );
	m_pInfoNick->SetPosition( nLeftAdjust, nPosY + nTextHeight * nRow );
	m_pInfoNick->SetTransparent( true );
	m_pInfoNick->SetTextColor( RGB(255,255,255) );
	m_pInfoNick->DrawD3DText( "", 0 );
	nRow++;

	//"계급"
	lstrcpy( szTemp, NF_GetMsg(SA_TEXT_USER_INFO_GRADE) );
	m_pInfoGradeCap			= new CSAD3DText();
	m_pInfoGradeCap->CreateD3DText( nCaptionWidth, nTextHeight, true );
	m_pInfoGradeCap->SetPosition( nPosX, nPosY + nTextHeight * nRow );
	m_pInfoGradeCap->SetTransparent( true );
	m_pInfoGradeCap->SetTextColor( RGB(198,156,109) );
	m_pInfoGradeCap->DrawD3DText( szTemp, lstrlen(szTemp), DT_LEFT, D3D_TEXT_OUTLINE );

	m_pInfoGrade			= new CSAD3DText();
	m_pInfoGrade->CreateD3DText( nContextWidth, nTextHeight, false );
	m_pInfoGrade->SetPosition( nLeftAdjust + 30, nPosY + nTextHeight * nRow );
	m_pInfoGrade->SetTransparent( true );
	m_pInfoGrade->SetTextColor( RGB(198,156,109) );
	m_pInfoGrade->DrawD3DText( "", 0 );
	nRow++;

	//"전적"
	lstrcpy( szTemp, NF_GetMsg(SA_TEXT_USER_INFO_SCORE) );
	m_pInfoWinPercentCap	= new CSAD3DText();
	m_pInfoWinPercentCap->CreateD3DText( nCaptionWidth, nTextHeight, true );
	m_pInfoWinPercentCap->SetPosition( nPosX, nPosY + nTextHeight * nRow );
	m_pInfoWinPercentCap->SetTransparent( true );
	m_pInfoWinPercentCap->SetTextColor( RGB(198,156,109) );
	m_pInfoWinPercentCap->DrawD3DText( szTemp, lstrlen(szTemp), DT_LEFT, D3D_TEXT_OUTLINE );

	m_pInfoWinPercent		= new CSAD3DText();
	m_pInfoWinPercent->CreateD3DText( nContextWidth, nTextHeight, false );
	m_pInfoWinPercent->SetPosition( nLeftAdjust, nPosY + nTextHeight * nRow );
	m_pInfoWinPercent->SetTransparent( true );
	m_pInfoWinPercent->SetTextColor( RGB(198,156,109) );
	m_pInfoWinPercent->DrawD3DText( "", 0 );
	nRow++;
	
	//"Kill/Death"
	lstrcpy( szTemp, NF_GetMsg(SA_TEXT_USER_INFO_KILL_DEATH) );
	m_pInfoKillDeathCap		= new CSAD3DText();
	m_pInfoKillDeathCap->CreateD3DText( nCaptionWidth, nTextHeight, true );
	m_pInfoKillDeathCap->SetPosition( nPosX, nPosY + nTextHeight * nRow );
	m_pInfoKillDeathCap->SetTransparent( true );
	m_pInfoKillDeathCap->SetTextColor( RGB(198,156,109) );
	m_pInfoKillDeathCap->DrawD3DText( szTemp, lstrlen(szTemp), DT_LEFT, D3D_TEXT_OUTLINE );

	m_pInfoKillDeath		= new CSAD3DText();
	m_pInfoKillDeath->CreateD3DText( nContextWidth, nTextHeight, false );
	m_pInfoKillDeath->SetPosition( nLeftAdjust, nPosY + nTextHeight * nRow );
	m_pInfoKillDeath->SetTransparent( true );
	m_pInfoKillDeath->SetTextColor( RGB(198,156,109) );
	m_pInfoKillDeath->DrawD3DText( "", 0 );
	nRow++;
	
	//"경험치"
	lstrcpy( szTemp, NF_GetMsg(SA_TEXT_USER_INFO_EXP) );
	m_pInfoExpCap			= new CSAD3DText();
	m_pInfoExpCap->CreateD3DText( nCaptionWidth, nTextHeight, true );
	m_pInfoExpCap->SetPosition( nPosX, nPosY + nTextHeight * nRow );
	m_pInfoExpCap->SetTransparent( true );
	m_pInfoExpCap->SetTextColor( RGB(198,156,109) );
	m_pInfoExpCap->DrawD3DText( szTemp, lstrlen(szTemp), DT_LEFT, D3D_TEXT_OUTLINE );
	
	m_pInfoExp				= new CSAD3DText();
	m_pInfoExp->CreateD3DText( nContextWidth, nTextHeight, false );
	m_pInfoExp->SetPosition( nLeftAdjust, nPosY + nTextHeight * nRow );
	m_pInfoExp->SetTransparent( true );
	m_pInfoExp->SetTextColor( RGB(198,156,109) );
	m_pInfoExp->DrawD3DText( "", 0 );
	nRow++;

	//"게임머니"
	lstrcpy( szTemp, NF_GetMsg(SA_TEXT_USER_INFO_GAME_MONEY) );
	m_pInfoGameMoneyCap		= new CSAD3DText();
	m_pInfoGameMoneyCap->CreateD3DText( nCaptionWidth, nTextHeight, true );
	m_pInfoGameMoneyCap->SetPosition( nPosX, nPosY + nTextHeight * nRow );
	m_pInfoGameMoneyCap->SetTransparent( true );
	m_pInfoGameMoneyCap->SetTextColor( RGB(198,156,109) );
	m_pInfoGameMoneyCap->DrawD3DText( szTemp, lstrlen(szTemp), DT_LEFT, D3D_TEXT_OUTLINE );
	
	m_pInfoGameMoney		= new CSAD3DText();
	m_pInfoGameMoney->CreateD3DText( nContextWidth, nTextHeight, false );
	m_pInfoGameMoney->SetPosition( nLeftAdjust, nPosY + nTextHeight * nRow );
	m_pInfoGameMoney->SetTransparent( true );
	m_pInfoGameMoney->SetTextColor( RGB(198,156,109) );
	m_pInfoGameMoney->DrawD3DText( "", 0 );
	nRow++;
	
	g_pDrawPrim->SetXYWH( &m_PolyGrade, (float)nLeftAdjust, float(nPosY+nTextHeight-5.0f), ROOM_TEXTURE_SIZE, ROOM_TEXTURE_SIZE );
	g_pDrawPrim->SetRGBA( &m_PolyGrade, 0xFFFFFFFF );
}

void CScreenRoom::BuildRoomInfo( uint16 nPosX, uint16 nPosY, uint16 nWidth, uint16 nHeight )
{
	const int	nLineCnt = 8;
	const int	nGameTypeIndent = 10;
	const int	nGameTypeCapIndent = nGameTypeIndent + 20;
	const float nCapWidthProportion = 0.4f;

	int nRow		= 0;
	int nTopAdjust	= 0;
	int nLeftAdjust	= 0;
	int nTextHeight	= 20;

	int nCaptionWidth = (int)( nWidth * nCapWidthProportion );
	int nContextWidth = nWidth - nCaptionWidth;

	nLeftAdjust = nPosX + nCaptionWidth;

	char szTemp[256] = {LTNULL};


	m_pPlayers = new CSAD3DText();
	m_pPlayers->CreateD3DText( nContextWidth, nTextHeight, true );
	m_pPlayers->SetPosition( nLeftAdjust, nPosY + nTextHeight * nRow );
	m_pPlayers->SetTransparent( true );
	m_pPlayers->SetTextColor( RGB(152,134,117) );
	m_pPlayers->DrawD3DText( "", 0 );

	//"인원수"
	lstrcpy( szTemp, NF_GetMsg(SA_TEXT_ROOM_INFO_PLAYERS) );
	m_pPlayersCap = new CSAD3DText();
	m_pPlayersCap->CreateD3DText( nCaptionWidth, nTextHeight, true );
	m_pPlayersCap->SetPosition( nPosX, nPosY + nTextHeight * nRow );
	m_pPlayersCap->SetTransparent( true );
	m_pPlayersCap->SetTextColor( RGB(152,134,117) );
	m_pPlayersCap->DrawD3DText( szTemp, lstrlen(szTemp), DT_LEFT, D3D_TEXT_OUTLINE );
	nRow++;
	
	m_pGameType = new CSAD3DText();
	m_pGameType->CreateD3DText( nContextWidth, nTextHeight, true );
	m_pGameType->SetPosition( nLeftAdjust, nPosY + nTextHeight * nRow );
	m_pGameType->SetTransparent( true );
	m_pGameType->SetTextColor( RGB(152,134,117) );
	m_pGameType->DrawD3DText( "", 0 );

	//"게임타입"
	lstrcpy( szTemp, NF_GetMsg(SA_TEXT_ROOM_INFO_GAME_TYPE) );
	m_pGameTypeCap = new CSAD3DText();
	m_pGameTypeCap->CreateD3DText( nCaptionWidth, nTextHeight, true );
	m_pGameTypeCap->SetPosition( nPosX, nPosY + nTextHeight * nRow );
	m_pGameTypeCap->SetTransparent( true );
	m_pGameTypeCap->SetTextColor( RGB(152,134,117) );
	m_pGameTypeCap->DrawD3DText( szTemp, lstrlen(szTemp), DT_LEFT, D3D_TEXT_OUTLINE );
	nRow++;

	m_pGTTimeLimit = new CSAD3DText();
	m_pGTTimeLimit->CreateD3DText( nContextWidth, nTextHeight, true );
	m_pGTTimeLimit->SetPosition( nLeftAdjust, nPosY + nTextHeight * nRow );
	m_pGTTimeLimit->SetTransparent( true );
	m_pGTTimeLimit->SetTextColor( RGB(152,134,117) );
	m_pGTTimeLimit->DrawD3DText( "", 0 );
	
	//"게임시간"
	lstrcpy( szTemp, NF_GetMsg(SA_TEXT_ROOM_INFO_GAME_TIME) );
	m_pGTTimeLimitCap = new CSAD3DText();
	m_pGTTimeLimitCap->CreateD3DText( nCaptionWidth, nTextHeight, true );
	m_pGTTimeLimitCap->SetPosition( nPosX, nPosY + nTextHeight * nRow );
	m_pGTTimeLimitCap->SetTransparent( true );
	m_pGTTimeLimitCap->SetTextColor( RGB(152,134,117) );
	m_pGTTimeLimitCap->DrawD3DText( szTemp, lstrlen(szTemp), DT_LEFT, D3D_TEXT_OUTLINE );
	nRow++;

	m_pGTGoal = new CSAD3DText();
	m_pGTGoal->CreateD3DText( nContextWidth, nTextHeight, true );
	m_pGTGoal->SetPosition( nLeftAdjust, nPosY + nTextHeight * nRow );
	m_pGTGoal->SetTransparent( true );
	m_pGTGoal->SetTextColor( RGB(152,134,117) );
	m_pGTGoal->DrawD3DText( "", 0 );
	
	//"승리조건"
	lstrcpy( szTemp, NF_GetMsg(SA_TEXT_ROOM_INFO_VICTORY_CONDITION) );
	m_pGTGoalCap = new CSAD3DText();
	m_pGTGoalCap->CreateD3DText( nCaptionWidth, nTextHeight, true );
	m_pGTGoalCap->SetPosition( nPosX, nPosY + nTextHeight * nRow );
	m_pGTGoalCap->SetTransparent( true );
	m_pGTGoalCap->SetTextColor( RGB(152,134,117) );
	m_pGTGoalCap->DrawD3DText( szTemp, lstrlen(szTemp), DT_LEFT, D3D_TEXT_OUTLINE );
	nRow++;
	
	m_pIntrusion = new CSAD3DText();
	m_pIntrusion->CreateD3DText( nContextWidth, nTextHeight, true );
	m_pIntrusion->SetPosition( nLeftAdjust, nPosY + nTextHeight * nRow );
	m_pIntrusion->SetTransparent( true );
	m_pIntrusion->SetTextColor( RGB(152,134,117) );
	m_pIntrusion->DrawD3DText( "", 0 );

	//"전투 중 참여"
	lstrcpy( szTemp, NF_GetMsg(SA_TEXT_ROOM_INFO_INTRUSION) );
	m_pIntrusionCap = new CSAD3DText();
	m_pIntrusionCap->CreateD3DText( nContextWidth, nTextHeight, true );
	m_pIntrusionCap->SetPosition( nPosX, nPosY + nTextHeight * nRow );
	m_pIntrusionCap->SetTransparent( true );
	m_pIntrusionCap->SetTextColor( RGB(152,134,117) );
	m_pIntrusionCap->DrawD3DText( szTemp, lstrlen(szTemp), DT_LEFT, D3D_TEXT_OUTLINE );
	nRow++;
	
	m_pTeamBalance = new CSAD3DText();
	m_pTeamBalance->CreateD3DText( nContextWidth, nTextHeight, true );
	m_pTeamBalance->SetPosition( nLeftAdjust, nPosY + nTextHeight * nRow );
	m_pTeamBalance->SetTransparent( true );
	m_pTeamBalance->SetTextColor( RGB(152,134,117) );
	m_pTeamBalance->DrawD3DText( "", 0 );

	//"팀 벨런스"
	lstrcpy( szTemp, NF_GetMsg(SA_TEXT_ROOM_INFO_TEAM_BALANCE) );
	m_pTeamBalanceCap = new CSAD3DText();
	m_pTeamBalanceCap->CreateD3DText( nContextWidth, nTextHeight, true );
	m_pTeamBalanceCap->SetPosition( nPosX, nPosY + nTextHeight * nRow );
	m_pTeamBalanceCap->SetTransparent( true );
	m_pTeamBalanceCap->SetTextColor( RGB(152,134,117) );
	m_pTeamBalanceCap->DrawD3DText( szTemp, lstrlen(szTemp), DT_LEFT, D3D_TEXT_OUTLINE );
	nRow++;	

//	g_pDrawPrim->SetXYWH( &m_PolyGrade, nPosX, nPosY, 16.0f, 16.0f );
//	g_pDrawPrim->SetRGBA( &m_PolyGrade, 0xFFFFFFFF );
}

void CScreenRoom::SetUserInfo( uint32 nGrade, 
							  const char* lpszNick,
							  uint32 nKill,
							  uint32 nDeath, 
							  uint32 nWin, uint32 nLose, uint32 nDraw,
							  __int64 nExp,
							  __int64 nGameMoney )
{
	char szTemp[128];
	
	int	 nGameCount		= nWin + nLose + nDraw;
	int	 nWinPercent	= (nGameCount<=0) ? 0 : (int)( (float)nWin * 100.0f / (float)nGameCount );
	
	int  nKillDeathCount	= nKill + nDeath;
	
	m_pInfoNick->DrawD3DText( lpszNick, strlen(lpszNick), DT_LEFT, D3D_TEXT_OUTLINE, true );
	
	m_pInfoGrade->DrawD3DText( g_Global_Data.GetGradeString( nGrade ), strlen(g_Global_Data.GetGradeString( nGrade )) );
	
	sprintf( szTemp, NF_GetMsg(SA_TEXT_USER_INFO_WINPERCENT_FORM), nGameCount, nWin, nLose );	
	m_pInfoWinPercent->DrawD3DText( szTemp, strlen(szTemp), DT_LEFT, D3D_TEXT_NORMAL, false );
	
	sprintf( szTemp, "%d / %d", nKill, nDeath );	
	m_pInfoKillDeath->DrawD3DText( szTemp, strlen(szTemp), DT_LEFT, D3D_TEXT_NORMAL, false );
	
	_i64toa( nExp, szTemp, 10 );
	m_pInfoExp->DrawD3DText( szTemp, strlen(szTemp), DT_LEFT, D3D_TEXT_NORMAL, false );
	
	_i64toa( nGameMoney, szTemp, 10 );
	m_pInfoGameMoney->DrawD3DText( szTemp, strlen(szTemp), DT_LEFT, D3D_TEXT_NORMAL, false );
	
	m_hGradeIcon	= g_Global_Data.GetGradeTexture(nGrade);
	SetupQuadUVs( m_PolyGrade, m_hGradeIcon, 0.0f, 0.0f, 1.0f, 1.0f );
	
}

void CScreenRoom::OnFocus(LTBOOL bFocus)
{
	CUserProfile *pProfile = g_pProfileMgr->GetCurrentProfile();

	if( bFocus )
	{
		g_fClickTime = g_pLTClient->GetTime();
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
					
		m_bReady		= LTFALSE;
		m_bIsInstrusion	= LTFALSE;
		m_bRecvComplete	= LTFALSE;
		
		RemoveAllTeam();
		g_pScores->RemoveAll();
		g_pInterfaceMgr->GetPlayerInfoMgr()->RemoveAll();
		m_hGradeIcon	= LTNULL;

		m_pBtnTextScroll->Show(LTFALSE);
		m_pChatEdit->ClearText();
		
		g_Network.EnableNetwork(LTTRUE);
				
		memset( &m_RoomOption, 0, sizeof(T_ROOM_OPTION) );

		m_pPopupUserInfo->CtrlHide();
		m_pPopupInvite->CtrlHide();
		m_pPopupRoomOption->CtrlHide();
		m_pPopupSearchResult->CtrlHide();
		m_pPopupRoomHelp->CtrlHide(); //[yaiin]

		m_nUpdateCount	= 0;
		m_nRecvPlayerCount	= 1000;
		m_bShowStartIcon = LTFALSE;
		m_fStartIconLastUpdate = 0;
		m_bDrawStartIcon = LTFALSE;

		g_GameDoc.SetWorldName(LTNULL);		
		g_pInterfaceMgr->ShowMessageBox( SA_TEXT_NOW_ON_ROOM_ENTER,
					LTMB_MESSAGE, LTNULL, LTNULL );

		m_pTextScroll->Clear();
		SetRanking();

		char szChannelInfo[128];
		sprintf( szChannelInfo, "%s-%d", g_GameDoc.GetChannelName(), g_GameDoc.GetChannelIndex()+1 );
		m_pChannelInfo->DrawD3DText( szChannelInfo, strlen(szChannelInfo), DT_CENTER );

		m_nChatCount = 0;
		m_bDisableChat	= LTFALSE;
		m_fDoBae	= m_fChatTime = g_pLTClient->GetTime();

		m_bIncrease = LTTRUE;
		m_fAlpha	= 0.0f;
		m_hLightButton = LTNULL;

		g_pScores->ClearTotalScore();
		g_pMissionMgr->SetCurrentRound(0);

	}
	else
	{
		g_GameDoc.SetCurrentMission( m_pMapComboBox->GetSelectIndex() );

		g_fClickTime = 0.0f;
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
		
		//--------------------------------------------------------------------------
		// UI 다시 작업
		//--------------------------------------------------------------------------
		m_bReady = LTFALSE;
		g_Network.EnableNetwork(LTFALSE);
		m_pTextList->DeleteAllItem();	
		RemoveAllTeam();
		m_pChatEdit->OnLostFocus();
		
		pProfile->m_nDMPlayerModel	= m_nCurrentModel;
		pProfile->m_nDMPlayerSkin	= m_nCurrentSkinID;
		
		//[MURSUM]=====================================================
		m_pInfoNick->DrawD3DText( "", 0 );
		m_pInfoGrade->DrawD3DText( "", 0 );
		m_pInfoWinPercent->DrawD3DText( "", 0 );
		m_pInfoKillDeath->DrawD3DText( "", 0 );
		m_pInfoExp->DrawD3DText( "", 0 );
		m_pInfoGameMoney->DrawD3DText( "", 0 );
		g_pInterfaceMgr->CloseMessageBox( LTFALSE );
		
		//g_pInterfaceMgr->SetExitMessage( LTNULL );
		//=============================================================
		
		m_pPopupUserInfo->CtrlHide();
		m_pPopupInvite->CtrlHide();
		m_pPopupRoomOption->CtrlHide();
		m_pPopupSearchResult->CtrlHide();
		m_pPopupRoomHelp->CtrlHide(); //[yaiin]
	}

	CScreenBase::OnFocus(bFocus);

	if( bFocus )
	{
		UpdateModel(0,0,0);
	}
}

LTBOOL CScreenRoom::OnLButtonUp(int x, int y)
{	
	if( m_pMapComboBox->IsVisible() )
	{
		m_pMapComboBox->Show(LTFALSE);
				
		m_pScrollUp->Show(LTFALSE);
		m_pScrollDown->Show(LTFALSE);
		m_pDragBtn->Show(LTFALSE);
	}

	return CScreenBase::OnLButtonUp(x, y);	
}

void CScreenRoom::StartHost()
{
#ifndef _STANDALONE
	g_pLTClient->CPrint( "[START HOST] ----------------------------------------------------------" );
	
	g_pLTClient->CPrint( "\tGAME TYPE: %d", m_RoomOption.nGameType );
	g_pLTClient->CPrint( "\tMAP INDEX: %d", m_RoomOption.nMapIndex );

	g_pLTClient->CPrint( "\tKILL LIMIT: %d", m_RoomOption.nKillLimit );
	g_pLTClient->CPrint( "\tROUND LIMIT: %d", m_RoomOption.nRoundLimit );
	g_pLTClient->CPrint( "\tTIME LIMIT: %d", m_RoomOption.nTimeLimit );	
	g_pLTClient->CPrint( "\tTEAM BALANCE: %d", m_RoomOption.bTeamBalance );
	g_pLTClient->CPrint( "\tINTRUSION: %d", m_RoomOption.bInstrusion );
	g_pLTClient->CPrint( "-----------------------------------------------------------------------" );
	g_GameDoc.SetRoomOption( &m_RoomOption );

	if( LTTRUE == HandleLaunch() )
	{
		g_Network.CS_StartHostReply( LTTRUE );
	}
	else
	{
		//서버로 실패를 알려서 다른 사람이 호스트가 되도록 한다.
		g_Network.CS_StartHostReply( LTFALSE );
	}
#else
	HandleLaunch();
#endif
}

uint32 CScreenRoom::OnCommand(uint32 dwCommand, uint32 dwParam1, uint32 dwParam2)
{
	switch(dwCommand)
	{
		// UI 다시 작업
		//---------------------------

	case CMD_ROOM_BTN_HELP:
		{
			//[yaiin]
			if(LTFALSE == m_pPopupRoomHelp->IsVisible())
			{
				if( m_pDummyCtrl && m_pDummyCtrl->IsVisible() ) break;
				m_pPopupRoomHelp->CtrlShow();
			}
			else
			{
				m_pPopupRoomHelp->CtrlHide();
			}
		}
		break;
	case CMD_ROOM_BTN_CHARACTER:
		g_Network.CS_EnterInven();
		g_pInterfaceMgr->ShowMessageBox( SA_TEXT_ENTER_INVEN, LTMB_MESSAGE, LTNULL, LTNULL );
		break;
	case CMD_ROOM_BTN_OPTION:
		if( g_GameDoc.IsCaptain() )
		{
			m_pPopupRoomOption->CtrlShow( &m_RoomOption );
		}
		else
		{
			//방 옵션 조절 권한은\r\n\r\n방장에게만 있습니다.
			g_pInterfaceMgr->ShowMessageBox( SA_TEXT_ONLY_CAPTINE_AVAILABLE, LTMB_OK, LTNULL, LTNULL );
		}
		break;

	case CMD_ROOM_BTN_INVITE:
		if( LTFALSE == m_pPopupInvite->IsVisible() )
		{
			if( m_pListBlueTeam->GetPlayerCount()+m_pListRedTeam->GetPlayerCount() >= m_RoomOption.nMaxUsers )
			{
				g_pInterfaceMgr->ShowMessageBox( SA_TEXT_INVITE_ERROR, LTMB_OK, LTNULL, LTNULL );
				return 1;
			}

			m_nInviteUserCount = -1;
			m_pPopupInvite->CtrlShow();
			g_pInterfaceMgr->ShowMessageBox( SA_TEXT_GET_LOBBY_USER, LTMB_MESSAGE, LTNULL, LTNULL );
			g_Network.CS_GetLobbyUserList();
		}
		break;

	case CMD_ROOM_BTN_READY :					
		// 방장인지의 여부에 따라 변함
#ifndef _STANDALONE
		if( g_GameDoc.IsCaptain() )
		{
			g_Network.CS_StartGame();
		}
		else
		{
			if( m_bIsInstrusion )
			{
				g_Network.CS_Ready( LTTRUE );
			}
			else
			{
				g_Network.CS_Ready( !m_bReady );
			}
		}
#else
		this->StartHost();
#endif
		
		break;

	case CMD_ROOM_BTN_BACK :		
#ifndef _STANDALONE
		g_Network.CS_ExitGameRoom();
#else
		g_pLTClient->Shutdown();
#endif
		break;

#ifndef _STANDALONE
	case CMD_ROOM_BTN_TEAMLEFT :
		if( TEAM_B == g_GameDoc.GetTeam() ) g_Network.CS_ChangeTeam( TEAM_A );					
		break;

	case CMD_ROOM_BTN_TEAMRIGHT :
		if( TEAM_A == g_GameDoc.GetTeam() ) g_Network.CS_ChangeTeam( TEAM_B );
		break;
#endif

	case CMD_MAP_DOWN:
		if( LTFALSE == m_pMapComboBox->IsVisible() )
		{
			m_pMapComboBox->Show(LTTRUE);				
							
			if(m_pMapComboBox->IsScrollEnable())
			{
				LTIntPt TempPt;
				TempPt.x = 0;
				TempPt.y = 0;

				TempPt.y += 131;
				m_pScrollUp->SetBasePos(TempPt);
				
				TempPt.y += 16;
				m_pDragBtn->SetBasePos(TempPt);
				m_pDragBtn->SetLimitY( TempPt.y, 36+TempPt.y );
				
				TempPt.y += 68;
				m_pScrollDown->SetBasePos(TempPt);

				m_pScrollUp->Show(LTTRUE);
				m_pScrollDown->Show(LTTRUE);
				m_pDragBtn->Show(LTTRUE);
			}	
		}
		else
		{
			m_pMapComboBox->Show(LTFALSE);
			m_pScrollUp->Show(LTFALSE);
			m_pScrollDown->Show(LTFALSE);
			m_pDragBtn->Show(LTFALSE);
		}
		break;
		
	case CMD_SELECT_MAP:
		{
			LPMapData	pMapData = m_pMapComboBox->Get();
			if( pMapData )
			{
				char szTemp[256] = {0,};
				lstrcpy( szTemp, pMapData->pMission->sName.c_str() );
				m_pMapTitle->DrawD3DText( szTemp, lstrlen( szTemp ) );

				g_Network.CS_ChangeOption( g_GameDoc.GetRoomIndex(),
										   pMapData->pMission->eGameType,
										   m_pMapComboBox->Get()->dwMapIndex,
										   int(m_RoomOption.nMaxUsers*0.5f),
										   pMapData->pMission->nTimeLimit,
										   pMapData->pMission->nKillLimit,
										   pMapData->pMission->nRoundLimit,
										   !!m_RoomOption.bInstrusion,
										   !!m_RoomOption.bTeamBalance,
										   !!m_RoomOption.bFreeSpectator );
			}
		}
		break;
	
#ifndef _STANDALONE
	case CMD_ROOM_CHAT_SCROLL_UP :
		if( m_pTextList->IsEnableScroll() )
		{
			m_pTextList->ScrollUp();
			m_pBtnTextScroll->SetPosYRatio( m_pTextList->GetRatio() );
		}
		break;

	case CMD_ROOM_CHAT_SCROLL_DOWN :
		if( m_pTextList->IsEnableScroll() )
		{
			m_pTextList->ScrollDown();
			m_pBtnTextScroll->SetPosYRatio( m_pTextList->GetRatio() );
		}
		break;
		
	case CMD_TEXT_WHELL:
		if( m_pTextList->IsEnableScroll() )
		{
			m_pBtnTextScroll->SetPosYRatio( m_pTextList->GetRatio() );
		}
		break;

	case CMD_ROOM_CHAT_SCROLL :
		if( dwParam1 == CMD_PARAM_MOUSEMOVE ) m_pTextList->ScrollFromRatio( m_pBtnTextScroll->GetPosYRatio() );
		break;

	case CMD_INVITE_RESULT:
		{
			char szInviteResult[64];
			sprintf( szInviteResult, NF_GetMsg(SA_TEXT_SEND_INVITE_PACKET), dwParam1 );
			this->AddChat( szInviteResult, CHAT_TYPE_ADMIN );
		}
		break;

	case CMD_SHOW_POPUP:
		{
			if( g_GameDoc.IsCaptain() )
			{
				m_pPopupMenuCaptain->ShowPopupMenu( g_pInterfaceMgr->GetCursorPos(), dwParam1, dwParam2 );
			}
			else
			{
				m_pPopupMenu->ShowPopupMenu( g_pInterfaceMgr->GetCursorPos(), dwParam1, dwParam2 );
			}
		}
		break;

	case CMD_USER_INFO:
		{
			T_TEAM_ITEMptr	pUserItem = m_pListBlueTeam->GetUserData(dwParam1);
			if( !pUserItem ) pUserItem = m_pListRedTeam->GetUserData(dwParam1);
			if( pUserItem )
			{
				g_Network.CS_UserInfo( pUserItem->nIndex, pUserItem->szNick, pUserItem->nGrade );
			}
		}
		break;

	case CMD_WHISPER:
		{
			T_TEAM_ITEMptr	pUserItem = m_pListBlueTeam->GetUserData(dwParam1);
			if( !pUserItem ) pUserItem = m_pListRedTeam->GetUserData(dwParam1);
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

	case CMD_KICK_PLAYER:
		{
			T_TEAM_ITEMptr	pUserItem = m_pListBlueTeam->GetUserData(dwParam1);
			if( !pUserItem ) pUserItem = m_pListRedTeam->GetUserData(dwParam1);
			if( pUserItem && g_GameDoc.GetUniqueIndex() != pUserItem->nIndex )
			{
				char szTemp[256];
				sprintf( szTemp, NF_GetMsg(SA_TEXT_ASK_KICK), pUserItem->szNick );

				g_nKickPlayerIndex = pUserItem->nIndex;
				g_pInterfaceMgr->ShowMessageBox( szTemp, LTMB_YESNO, KickCallBack, LTNULL );
			}
		}
		break;

	case CMD_CHANGE_CAPTAIN:
		{
			T_TEAM_ITEMptr	pUserItem = m_pListBlueTeam->GetUserData(dwParam1);
			if( !pUserItem ) pUserItem = m_pListRedTeam->GetUserData(dwParam1);
			if( pUserItem && g_GameDoc.GetUniqueIndex() != pUserItem->nIndex )
			{
				char szTemp[256];
				sprintf( szTemp, NF_GetMsg(SA_TEXT_CHANGE_CAPTAIN), pUserItem->szNick );

				g_nNewCaptainIndex = pUserItem->nIndex;
				g_pInterfaceMgr->ShowMessageBox( szTemp, LTMB_YESNO, ChangeCaptainCallBack, LTNULL );
			}
		}
		break;
		
	case CMD_ROOM_CHAT_EDIT :
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
				sprintf( m_szChatBuffer, "[%s] %s", g_GameDoc.GetNick(), m_pChatEdit->GetText());			
				AddChat( m_szChatBuffer );

				if( LTFALSE == IsCurse( m_pChatEdit->GetText() ) )	 //욕 필터링
				{
					g_Network.CS_Chat( m_szChatBuffer );
				}
				CheckDoBae();
				m_pChatEdit->ClearText();
			}
			break;

		case UIM_EDIT_CHAR:
			g_fClickTime = g_pLTClient->GetTime(); // 광
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
				strcmp( m_pChatEdit->GetText(), NF_GetMsg(SA_TEXT_CMD_REPLY) ) == 0 )
			{
				if( strlen( g_GameDoc.GetLastRecvNick() ) > 0 )
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
#endif
	//---------------------------
/*
#ifdef BIG_FAT_MODE_TEST
		case CMD_CHECK_NORMAL :
		case CMD_CHECK_BIG :
		case CMD_CHECK_FAT :
			{	
				for( int i = 0; i < 3; i++ )
				{
					g_pCheckModeTest[i]->SetCheck(LTFALSE);
				}
				g_pCheckModeTest[dwParam1]->SetCheck();					
				g_pClientMultiplayerMgr->SetPlayerModelType((ModelLoadType)dwParam1);
			}break;
#endif		
			*/
	default:
		return CScreenBase::OnCommand(dwCommand,dwParam1,dwParam2);
	}

	return 1;
}

LTBOOL CScreenRoom::RenderRoomInfo()
{
	if( m_pGameType ) m_pGameType->Render();
	if( m_pGameTypeCap ) m_pGameTypeCap->Render();
	if( m_pGTGoal ) m_pGTGoal->Render();
	if( m_pGTGoalCap ) m_pGTGoalCap->Render();
	if( m_pGTTimeLimit ) m_pGTTimeLimit->Render();
	if( m_pGTTimeLimitCap ) m_pGTTimeLimitCap->Render();
	if( m_pPlayers ) m_pPlayers->Render();
	if( m_pPlayersCap ) m_pPlayersCap->Render();
	if( m_pIntrusion ) m_pIntrusion->Render();
	if( m_pTeamBalance ) m_pTeamBalance->Render();
	if( m_pIntrusionCap ) m_pIntrusionCap->Render();
	if( m_pTeamBalanceCap ) m_pTeamBalanceCap->Render();
	
	return TRUE;
}

LTBOOL CScreenRoom::RenderUserInfo()
{
	if( m_pInfoNickCap ) m_pInfoNickCap->Render();
	if( m_pInfoNick ) m_pInfoNick->Render();
	if( m_pInfoGradeCap ) m_pInfoGradeCap->Render();
	if( m_pInfoGrade ) m_pInfoGrade->Render();
	if( m_pInfoWinPercentCap ) m_pInfoWinPercentCap->Render();
	if( m_pInfoWinPercent ) m_pInfoWinPercent->Render();
	if( m_pInfoKillDeathCap ) m_pInfoKillDeathCap->Render();
	if( m_pInfoKillDeath ) m_pInfoKillDeath->Render();
	if( m_pInfoExpCap ) m_pInfoExpCap->Render();
	if( m_pInfoExp ) m_pInfoExp->Render();
	if( m_pInfoGameMoneyCap ) m_pInfoGameMoneyCap->Render();
	if( m_pInfoGameMoney ) m_pInfoGameMoney->Render();
	
	if( m_hGradeIcon )
	{
		g_pDrawPrim->SetTexture( m_hGradeIcon );
		g_pDrawPrim->DrawPrim( &m_PolyGrade );
	}
	
	return LTTRUE;
}

LTBOOL CScreenRoom::Render(HSURFACE hDestSurf)
{
//[SPIKE]강제이동 =====================================================================
#ifdef _FINAL
	
	float fCurTime = g_pLTClient->GetTime();
	m_bForceExitTimeStart = LTFALSE;

	if(g_GameDoc.IsCaptain()) //방장일경우 
	{
		if(g_GameDoc.GetTeam() == TEAM_A) //Red
		{
			if(m_pListBlueTeam->IsExistReadyUser())
			{
				m_bForceExitTimeStart = LTTRUE;
			}
		}
		else
		{
			if(m_pListRedTeam->IsExistReadyUser())
			{
				m_bForceExitTimeStart = LTTRUE;
			}
		}
	}
	else // 방장이 아닐경우 
	{	
		if(!AreYouReady())
		{
			m_bForceExitTimeStart = LTTRUE;
		}
	}

	if(m_bForceExitTimeStart)
	{
		if( (fCurTime - g_fClickTime) > GAMESTART_TIME )//&& g_GameDoc.IsCaptain() )
		{
			if(g_pPopRoomOption->IsVisible()) g_pPopRoomOption->CtrlHide();
			g_pInterfaceMgr->SetExitMessage( NF_GetMsg(SA_TEXT_LONG_TIME_NO_INPUT) );
			OnEscape();
		}
	}
	else
	{
		g_fClickTime = g_pLTClient->GetTime();
	}
	
#endif // _FINAL
//[SPIKE]강제이동 =====================================================================
	
	
	SetRenderState();

	++m_nUpdateCount;
	if( m_nUpdateCount == 5 && LTFALSE == g_pInterfaceMgr->IsPlayBGM() )
	{
		g_pInterfaceMgr->PlayBGM( "SA_INTERFACE\\SND\\BG\\Screen.wav" );
	}

	RenderCloud();
	
	//====================================================
	// Base Rendering
	//====================================================
	LTBOOL result = CScreenBase::Render(hDestSurf);
	//====================================================

	if( m_bIncrease )
	{
		m_fAlpha += g_pLTClient->GetFrameTime() * 512.0f;
		if( m_fAlpha > 255.0f )
		{
			m_fAlpha = 255.0f;
			m_bIncrease = LTFALSE;
		}
	}
	else
	{
		m_fAlpha -= g_pLTClient->GetFrameTime() * 512.0f;
		if( m_fAlpha < 0.0f )
		{
			m_fAlpha = 0.0f;
			m_bIncrease = LTTRUE;
		}
	}
	g_pDrawPrim->SetALPHA( &m_PolyLightButton, (uint8)m_fAlpha );

	if( LTFALSE == m_pBtnReady->IsSelected() && m_hLightButton )
	{
		g_pDrawPrim->SetTexture(m_hLightButton);
		g_pDrawPrim->DrawPrim( &m_PolyLightButton );
	}
	
	m_pTextScroll->Update();
	m_pTextScroll->Render();

	g_pDrawPrim->SetTexture( m_hUIBottomL );
	g_pDrawPrim->DrawPrim( &m_PolyUIBottomL );
	g_pDrawPrim->SetTexture( m_hUIBottomR );
	g_pDrawPrim->DrawPrim( &m_PolyUIBottomR );

	m_pChannelInfo->Render();
	
	// character model test
	//------------------------------------------------------------------------------------
	//g_pLTClient->RenderObjects(g_pInterfaceMgr->GetInterfaceCamera(), (HLOCALOBJ*&)*g_hTestObject, 1, g_pLTClient->GetFrameTime());
	//------------------------------------------------------------------------------------

	m_pRoomName->Render();
	
	m_pMapTitle->Render();
	RenderUserInfo();
	RenderRoomInfo();

	if( m_bDisableChat )
	{
		if( g_pLTClient->GetTime() - m_fDoBae > 20.0f )
		{
			m_bDisableChat = LTFALSE;
		}
	}

	//[yaiin]
	//텍스트들은 ctrlArray에 의해 그려지지 않는다.
	//팝업을 그린 후 텍스트가 위에 남으면 안 되기에 한번 더 그려줌.
	//참고로 방설정 팝업은 다른 텍스트와 겹치지 않기에 걍 그렸다.
	if(LTTRUE == m_pPopupRoomHelp->IsVisible())
	{
		m_pPopupRoomHelp->Render();
	}

	return result;
}

LTBOOL CScreenRoom::RenderCloud()
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

void CScreenRoom::AddChat( DWORD dwIdx, int32 nType )
{
	this->AddChat( NF_GetMsg(dwIdx), nType );
}

void CScreenRoom::AddNotify( const char* lpszNoti )
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

void CScreenRoom::AddChat( const char* lpszChat, int32 nType )
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

void CScreenRoom::SetStartReadyButton()
{	
	if( g_GameDoc.IsCaptain() )
	{
		m_pBtnReady->SetTooltip( "Ctrl+S" );	m_pBtnReady->SetHotKey( 0x13 );	
		m_pBtnReady->SetTexture(m_hReadyBtnTexture[4], m_hReadyBtnTexture[5], m_hReadyBtnTexture[6], m_hReadyBtnTexture[7]);
		m_pBtnMapDown->Show(LTTRUE);
		
		m_bShowStartIcon = LTTRUE;

		m_hLightButton = m_hReadyBtnTexture[7];
	}
	else if( m_bIsInstrusion )
	{
		m_pBtnReady->SetTooltip( "Ctrl+S" );	m_pBtnReady->SetHotKey( 0x13 );	
		m_pBtnReady->SetTexture(m_hReadyBtnTexture[4], m_hReadyBtnTexture[5], m_hReadyBtnTexture[6], m_hReadyBtnTexture[7]);
		m_pBtnMapDown->Show(LTFALSE);

		m_bShowStartIcon = LTTRUE;

		m_hLightButton = m_hReadyBtnTexture[7];
	}
	else
	{
		m_pBtnReady->SetTooltip( "Ctrl+R" );	m_pBtnReady->SetHotKey( 0x12 );
		m_pBtnReady->SetTexture(m_hReadyBtnTexture[0], m_hReadyBtnTexture[1], m_hReadyBtnTexture[2], m_hReadyBtnTexture[3]);
		m_pBtnMapDown->Show(LTFALSE);

		m_bShowStartIcon = LTFALSE;

		m_hLightButton = m_hReadyBtnTexture[3];
	}
}

void CScreenRoom::ChangeOption( unsigned char nGameType,
								unsigned char nMapIndex,
								unsigned char nMaxUsers,
								unsigned char nTimeLimit, 
								unsigned char nKillLimit, 
								unsigned char nRoundLimit, 
								bool bIntrusion, 
								bool bTeamBalance,
								bool bFreeSpec )
{
	ChangeOptMaxUsers( nMaxUsers );
	ChangeOptMapIndex( nMapIndex, nGameType );	
	ChangeOptInstrusion( bIntrusion );
	ChangeOptTeamBalance( bTeamBalance );		
	m_pMapComboBox->SetSelectIndex( m_RoomOption.nMapIndex );
	m_pMapComboBox->SetPreIndex( m_RoomOption.nMapIndex );

	LPMapData	pMapData = m_pMapComboBox->Get();	
	if( IsMissionMode( (GameType)nGameType ) )
	{
		ChangeGameLimit( pMapData ? pMapData->pMission->nTimeLimit : 3, 0, nRoundLimit );
	}
	else
	{
		ChangeGameLimit( nTimeLimit, nKillLimit, 0 );
	}

	m_RoomOption.bFreeSpectator = bFreeSpec;

	//////////////////////////////////////////////////////////////////////////
	// NOVICE : GameDocument 에 세팅한다.
	g_GameDoc.SetRoomOption( &m_RoomOption );
	//////////////////////////////////////////////////////////////////////////
}

void CScreenRoom::ChangeRoomTitle( const char* lpszTitle )
{
	if( lpszTitle )
	{
		char szTemp[256] = {0,};

		g_GameDoc.SetTitle( lpszTitle );

		if( strlen( g_GameDoc.GetPassword() ) > 0 )
		{
			sprintf( szTemp, "[%d] %s (%s)", g_GameDoc.GetRoomIndex(), 
											 g_GameDoc.GetTitle(),
											 g_GameDoc.GetPassword() );
		}
		else
		{
			sprintf( szTemp, "[%d] %s", g_GameDoc.GetRoomIndex(), 
										g_GameDoc.GetTitle() );
		}
		m_pRoomName->DrawD3DText( szTemp, strlen(szTemp), DT_LEFT,D3D_TEXT_NORMAL, true  );
	}
}

void CScreenRoom::ChangeOptMapIndex( uint8 nMapIndex, uint8 nGameType )
{
	MISSION* pMission = g_pMissionButeMgr->GetMission(nMapIndex);

	if( pMission )
	{
		char szTemp[256];

		m_RoomOption.nMapIndex		= nMapIndex;
		m_RoomOption.nGameType		= nGameType;
		m_pMapFrame->SetFrame( pMission->hScreenShot );

		m_pMapComboBox->SetSelectIndex( nMapIndex );

		strcpy( szTemp, NF_GetMsg(pMission->nNameId) );
		m_pMapTitle->DrawD3DText( szTemp, strlen(szTemp) );
		
		strcpy( szTemp, NF_GetMsg(pMission->nGameTypeID) );
		m_pGameType->DrawD3DText( szTemp, strlen(szTemp), DT_LEFT, D3D_TEXT_SHADOW );
	}
}

void CScreenRoom::ChangeOptMaxUsers( short nMaxUsers )
{
	m_RoomOption.nMaxUsers	= nMaxUsers;
	
	char szTemp[256];
	sprintf( szTemp, "%d vs %d", m_RoomOption.nMaxUsers/2, m_RoomOption.nMaxUsers/2 );
	m_pPlayers->DrawD3DText( szTemp, strlen(szTemp), DT_LEFT, D3D_TEXT_SHADOW );
}

void CScreenRoom::ChangeGameLimit( unsigned char nTimeLimit, unsigned char nKillLimit, unsigned char nRoundLimit )
{
	m_RoomOption.nTimeLimit		= nTimeLimit;
	m_RoomOption.nKillLimit		= nKillLimit;
	m_RoomOption.nRoundLimit	= nRoundLimit;

	char szTemp[256];

	//					"분"
	sprintf( szTemp, "%d%s", m_RoomOption.nTimeLimit, NF_GetMsg(SA_TEXT_ROOM_INFO_TIME_LIMIT_UNIT) );
	m_pGTTimeLimit->DrawD3DText( szTemp, strlen(szTemp), DT_LEFT, D3D_TEXT_SHADOW );

	if( m_RoomOption.nKillLimit > 0 )
	{
		//"킬수제한"
		lstrcpy( szTemp, NF_GetMsg(SA_TEXT_ROOM_INFO_KILL_LIMIT) );
		m_pGTGoalCap->DrawD3DText( szTemp, strlen(szTemp), DT_LEFT, D3D_TEXT_OUTLINE );

		//					 "Kill"
		sprintf( szTemp, "%d %s", m_RoomOption.nKillLimit, NF_GetMsg(SA_TEXT_ROOM_KILL_ENG) );
		m_pGTGoal->DrawD3DText( szTemp, strlen(szTemp), DT_LEFT, D3D_TEXT_SHADOW );
	}
	else
	{
		//"라운드제한"
		lstrcpy( szTemp, NF_GetMsg(SA_TEXT_ROOM_INFO_ROUND_LIMIT) );
		m_pGTGoalCap->DrawD3DText( szTemp, strlen(szTemp), DT_LEFT, D3D_TEXT_OUTLINE );

		//					"라운드"
		sprintf( szTemp, "%d %s", m_RoomOption.nRoundLimit, NF_GetMsg(SA_TEXT_ROOM_ROUND) );
		m_pGTGoal->DrawD3DText( szTemp, strlen(szTemp), DT_LEFT, D3D_TEXT_SHADOW );
	}	
}

void CScreenRoom::ChangeOptInstrusion( LTBOOL bEnable )
{
	m_RoomOption.bInstrusion	= bEnable;

	if( m_RoomOption.bInstrusion )
	{
		m_pIntrusion->DrawD3DText( "On", strlen("On"), DT_LEFT, D3D_TEXT_SHADOW );
	}
	else
	{
		m_pIntrusion->DrawD3DText( "Off", strlen("Off"), DT_LEFT, D3D_TEXT_SHADOW );
	}
}

void CScreenRoom::ChangeOptTeamBalance( LTBOOL bEnable )
{
	m_RoomOption.bTeamBalance	= bEnable;

	if( m_RoomOption.bTeamBalance )
	{
		m_pTeamBalance->DrawD3DText( "On", strlen("On"), DT_LEFT, D3D_TEXT_SHADOW );		
	}
	else
	{
		m_pTeamBalance->DrawD3DText( "Off", strlen("Off"), DT_LEFT, D3D_TEXT_SHADOW );
	}
}

void CScreenRoom::StartGameAsClient( const char* szIP, int nPort, bool bSpectator )
{
	if( LTFALSE == g_GameDoc.IsCaptain() && LTFALSE == m_bReady && LTFALSE == m_bIsInstrusion )
	{
		SetInstrution( LTTRUE );
	}
	else
	{
		PrePlaying();
		
		GameType eGameType = eGameTypeTeamDeathmatch;
		MISSION* pMission = g_pMissionButeMgr->GetMission( m_pMapComboBox->GetSelectIndex() );
		if( pMission )
		{
			eGameType = pMission->eGameType;
			g_GameDoc.SetWorldName( pMission->szMap );
		}

		g_pClientMultiplayerMgr->SetMultiplayerData( eGameType, szIP, nPort );
		g_pClientMultiplayerMgr->SetupDefaultSetting( (GameType)m_RoomOption.nGameType,
													  m_RoomOption.nMaxUsers,
													  m_RoomOption.bFriendlyFire,
													  m_RoomOption.nTimeLimit );		
		g_pClientMultiplayerMgr->SetupClient( szIP, g_GameDoc.GetTitle(), "", bSpectator );

		g_pMissionMgr->StartGameAsClient();
	}

	g_pLTClient->CPrint( "[START GAME AS CLIENT] ------------------------------------------------" );
	
	g_pLTClient->CPrint( "\tGAME TYPE: %d", m_RoomOption.nGameType );
	g_pLTClient->CPrint( "\tMAP INDEX: %d", m_RoomOption.nMapIndex );

	g_pLTClient->CPrint( "\tKILL LIMIT: %d", m_RoomOption.nKillLimit );
	g_pLTClient->CPrint( "\tROUND LIMIT: %d", m_RoomOption.nRoundLimit );
	g_pLTClient->CPrint( "\tTIME LIMIT: %d", m_RoomOption.nTimeLimit );	
	g_pLTClient->CPrint( "\tTEAM BALANCE: %d", m_RoomOption.bTeamBalance );
	g_pLTClient->CPrint( "\tINTRUSION: %d", m_RoomOption.bInstrusion );
	g_pLTClient->CPrint( "-----------------------------------------------------------------------" );


	g_GameDoc.SetRoomOption( &m_RoomOption );
}
		
LTBOOL CScreenRoom::HandleLaunch()
{
	PrePlaying();

	GameType eGameType = eGameTypeTeamDeathmatch;
	MISSION* pMission = g_pMissionButeMgr->GetMission( m_pMapComboBox->GetSelectIndex() );
	if( pMission )
	{
		eGameType = pMission->eGameType;
		g_GameDoc.SetWorldName( pMission->szMap );
	}
	
	g_pClientMultiplayerMgr->SetMultiplayerData( eGameType, "127.0.0.1", g_pLTClient->GetHostPort() );	
	g_pClientMultiplayerMgr->SetupDefaultSetting( (GameType)m_RoomOption.nGameType,
												   m_RoomOption.nMaxUsers,					
												   m_RoomOption.bFriendlyFire,
												   m_RoomOption.nTimeLimit );
	if( !g_pClientMultiplayerMgr->SetupServerHost() )
	{
		// This should probably prevent them from starting the server if this function fails,
		// But it never does...
		return LTFALSE;
	}

	if (!LaunchGame())
	{
		//g_pInterfaceMgr->LoadFailed(SCREEN_ID_SCREEN_ROOM);
		// LoadFail Message~~~~~
		return LTFALSE;
	}
	
	return LTTRUE;
}

LTBOOL CScreenRoom::LaunchGame()
{
	if( !g_pMissionMgr->StartGameNew( m_RoomOption.nMapIndex ))
		return LTFALSE;

	return LTTRUE;
}

void CScreenRoom::Escape()
{
	CScreenBase::Escape();
}

bool CScreenRoom::UpdateInterfaceSFX()
{
	CScreenBase::UpdateInterfaceSFX();
	return true;
}

void CScreenRoom::SetInstrution( LTBOOL bInstrution )
{
	m_bIsInstrusion	= bInstrution;
}

void CScreenRoom::EndGame()
{
	SetInstrution( LTFALSE );
}

LTBOOL CScreenRoom::OnEnter()
{
	if( m_pDummyCtrl && m_pDummyCtrl->IsVisible() ) return LTFALSE;		
	if( m_pChatEdit ) m_pChatEdit->ForceAddEditBox();
	return LTTRUE;
}

LTBOOL CScreenRoom::OnEscape()
{
	if( m_pDummyCtrl && m_pDummyCtrl->IsVisible() ) return LTFALSE;
	OnCommand( CMD_ROOM_BTN_BACK, LTNULL, LTNULL );
	return LTTRUE;
}

void CScreenRoom::PrePlaying()
{
	((CSAPlayerStats*)(g_pInterfaceMgr->GetPlayerStats()))->SetResetWeapon();
	((CSAPlayerStats*)(g_pInterfaceMgr->GetPlayerStats()))->SetCanUseWeapon(g_pScreenInventory->GetCurrentUsedWeapon(0), 0);
	((CSAPlayerStats*)(g_pInterfaceMgr->GetPlayerStats()))->SetCanUseWeapon(g_pScreenInventory->GetCurrentUsedWeapon(1), 1);
	((CSAPlayerStats*)(g_pInterfaceMgr->GetPlayerStats()))->SetCanUseWeapon(g_pScreenInventory->GetCurrentUsedWeapon(2), 2);
	((CSAPlayerStats*)(g_pInterfaceMgr->GetPlayerStats()))->SetCanUseWeapon(g_pScreenInventory->GetCurrentUsedWeapon(3), 3);

	m_pListRedTeam->PrePlayingState();
	m_pListBlueTeam->PrePlayingState();
}

LTBOOL CScreenRoom::EditCommand( const char* lpszChatMessage )
{
#ifndef _STANDALONE
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
		//								"/ㅈ"
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
	else if( stricmp( lpszToken, "/C" ) == 0 ||
		//								"/ㅊ"
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
		//"[채팅창 글씨지우기] /C"
		AddChat( SA_TEXT_HELP_CLEAR_CHAT , CHAT_TYPE_NOTICE );		
		m_pChatEdit->ClearText();
	}

#endif

	return LTFALSE;
}

void CScreenRoom::ShowExitError()
{
	MBCreate mb;
	mb.eType		= LTMB_OK;
	mb.pFn			= LTNULL;
	mb.pData		= LTNULL;
	//"레디 상태에서는 퇴장이 불가능 합니다."
	g_pInterfaceMgr->ShowMessageBox( SA_TEXT_CANNOT_EXIT_IN_READY, &mb );
}

void CScreenRoom::ShowEnterInvenError()
{
	MBCreate mb;
	mb.eType		= LTMB_OK;
	mb.pFn			= LTNULL;
	mb.pData		= LTNULL;
	//"[READY] 중에는 캐릭터 설정을 변경할 수 없습니다."
	g_pInterfaceMgr->ShowMessageBox( SA_TEXT_CANNOT_CHANGE_CHARACTER_IN_READY, &mb );
}

void CScreenRoom::EnterInven( int nIndex, bool bEnter )
{
	if( g_GameDoc.GetUniqueIndex() == nIndex )
	{
		// 광식 
		g_pScreenInventory->SetShop(false);
		g_pInterfaceMgr->SwitchToScreen(SCREEN_ID_INVENTORY);
	}
	else
	{
		this->SetPlayerState( nIndex, PLAYER_STATE_INVEN, -1 );
	}
}

void CScreenRoom::SetPlayerState( int nIndex, int nState, int nGrade )
{
	if( LTFALSE == m_pListRedTeam->SetUserState( nIndex, nState, nGrade ) )
	{
		m_pListBlueTeam->SetUserState( nIndex, nState, nGrade );
	}
}

void CScreenRoom::ChangeRoomState( LTBOOL bPlaying )
{
	if( bPlaying )
	{
		if( !g_GameDoc.IsCaptain() && !m_bReady )
		{
			SetInstrution(LTTRUE);
			SetStartReadyButton();
		}
	}
	else
	{
		SetInstrution(LTFALSE);
		SetStartReadyButton();
	}
}

void CScreenRoom::SetPlayerReady( int nIndex, LTBOOL bReady )
{
	if( LTFALSE == m_pListRedTeam->SetUserState( nIndex, bReady ? PLAYER_STATE_READY : PLAYER_STATE_BUFF, -1 ) )
	{
		m_pListBlueTeam->SetUserState( nIndex, bReady ? PLAYER_STATE_READY : PLAYER_STATE_BUFF, -1 );
	}

	if( g_GameDoc.GetUniqueIndex() == nIndex )
	{
		m_bReady = bReady;
	}
}

void CScreenRoom::SearchResult( LTBOOL bSuccess, const char* lpszMsg )
{
	if( m_pPopupSearchResult->IsVisible() )
	{
		m_pPopupSearchResult->CtrlHide();
	}

	m_pPopupSearchResult->CtrlShow( bSuccess, lpszMsg, -1, -1, LTFALSE );
}

void CScreenRoom::PrePopupState()
{
	SetSelection(kNoSelection);
	m_pChatEdit->OnLostFocus();
}

void CScreenRoom::PostPopupState()
{
}

void CScreenRoom::UserInfoReply( const char* lpszNick, uint32 nGrade,
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

void CScreenRoom::UpdateModel(int nModelID, int HeadSkinID, int nBodySkinID )
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

		g_pCommonLT->SetObjectFilenames(hChar, &createStruct);

		uint32 dwAni = g_pLTClient->GetAnimIndex(hChar, "IF_SELECT");
		if( dwAni != INVALID_ANI )
		{
			g_pLTClient->SetModelAnimation(hChar, dwAni);
		}

		CBaseScreen::Update_Model();
	}
}

void CScreenRoom::SetRecvUserCount( int nCount, int nMyTeam, int nState )
{
	m_nRecvPlayerCount	= nCount+1;	//Users + Local
	this->AddRoomUser( g_GameDoc.GetUniqueIndex(),
					   g_GameDoc.GetNick(),
					   g_GameDoc.GetGrade(),
					   nMyTeam,
					   nState,
					   g_GameDoc.IsCaptain() );

	g_pInterfaceMgr->GetPlayerInfoMgr()->AddPlayer( g_GameDoc.GetUniqueIndex(),
													g_GameDoc.GetNick(),
													g_GameDoc.GetGrade(),
													nMyTeam,
													g_GameDoc.IsCaptain() );
}

LTBOOL CScreenRoom::IsMissionMode( GameType eGameType )
{
	switch( eGameType )
	{
	case eGameTypeTeamDeathmatch:
		return LTFALSE;
	default:
		return LTTRUE;
	}
}

LTBOOL CScreenRoom::OnLButtonDown(int x, int y)
{
	g_fClickTime = g_pLTClient->GetTime();
	LTBOOL bResult = CBaseScreen::OnLButtonDown(x, y);
	if( m_pPopupMenu->IsVisible() ) m_pPopupMenu->HidePopupMenu();
	if( m_pPopupMenuCaptain->IsVisible() ) m_pPopupMenuCaptain->HidePopupMenu();
	return bResult;
}

LTBOOL CScreenRoom::OnMouseMove(int x, int y)
{
	g_fClickTime = g_pLTClient->GetTime();
	return CBaseScreen::OnMouseMove( x, y );
}

void CScreenRoom::SetRanking()
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


void CScreenRoom::CheckDoBae()
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
	
	if( m_nChatCount > 3 )
	{
		AddChat( "도배하셨으므로 20초간 채팅을 금지합니다.", CHAT_TYPE_ADMIN );
		m_fDoBae = m_fChatTime;
		m_nChatCount = 0;
		m_bDisableChat = LTTRUE;
	}
}

void CScreenRoom::SetLobbyUserCount( int nCount )
{
	if( m_pPopupInvite->IsVisible() && m_nInviteUserCount < 0 )
	{
		m_nInviteUserCount = nCount;
		if( m_nInviteUserCount <= 0 )
		{
			m_pPopupInvite->CtrlHide();
			g_pInterfaceMgr->ShowMessageBox( SA_TEXT_NO_LOBBY_USER, LTMB_OK, LTNULL, LTNULL );
		}
	}
}

void CScreenRoom::AddLobbyUser( const char* lpszNick, uint8 nGrade )
{
	if( m_pPopupInvite->IsVisible() && m_nInviteUserCount > 0 )
	{
		m_pPopupInvite->AddLobbyUser( lpszNick, nGrade );
		
		--m_nInviteUserCount;
		if( m_nInviteUserCount <= 0 )
		{
			g_pInterfaceMgr->CloseMessageBox(LTTRUE);
		}
	}
}