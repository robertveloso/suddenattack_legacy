#include "stdafx.h"
#include "ScreenInventory.h"
#include "ScreenMgr.h"
#include "ScreenCommands.h"
#include "ClientMultiplayerMgr.h"
#include "GameClientShell.h"
#include "PopupWeaponDetail.h"
#include "PopupItemBuy.h"
#include "TextScroll.h"

#include "GameText.h"
#include "GameMessage.h"

CScreenInventory* g_pScreenInventory	 = LTNULL;

namespace 
{
	#define INVALID_ANI			((HMODELANIM)-1)

	enum eLocalCommands
	{
		CMD_INVEN_EXIT	= CMD_CUSTOM+1,
		// Inventory
		CMD_INVEN_CHECK_RED,
		CMD_INVEN_CHECK_BLUE,
		//------------------------------------ Open 여기부터 새로 만든거
		CMD_CHECK_SHOP_WEAPON,
		CMD_CHECK_SHOP_CHAR,
		CMD_CHECK_SHOP_ETC,
		CMD_SHOP_BUY,
		CMD_SHOP_PRESENT,
		CMD_CHECK_SHOP_W_ALL,
		CMD_CHECK_SHOP_W_FIRST,
		CMD_CHECK_SHOP_W_SECOND,
		CMD_CHECK_SHOP_W_THIRD,
		CMD_CHECK_SHOP_W_FOUR,

		CMD_SCROLL_SHOP,
		CMD_SCROLL_UP_SHOP,
		CMD_SCROLL_DOWN_SHOP,

		CMD_BTN_SHOP_W_SLOT_0,
		CMD_BTN_SHOP_W_SLOT_1,
		CMD_BTN_SHOP_W_SLOT_2,
		CMD_BTN_SHOP_W_SLOT_3,
		CMD_BTN_SHOP_W_SLOT_4,

		CMD_BTN_SHOP_W_SLOT_DBL_0,
		CMD_BTN_SHOP_W_SLOT_DBL_1,
		CMD_BTN_SHOP_W_SLOT_DBL_2,
		CMD_BTN_SHOP_W_SLOT_DBL_3,
		CMD_BTN_SHOP_W_SLOT_DBL_4,

		CMD_BTN_SHOP_C_SLOT_0,
		CMD_BTN_SHOP_C_SLOT_1,
		CMD_BTN_SHOP_C_SLOT_2,
		CMD_BTN_SHOP_C_SLOT_3,

		CMD_BTN_SHOP_C_SLOT_DBL_0,
		CMD_BTN_SHOP_C_SLOT_DBL_1,
		CMD_BTN_SHOP_C_SLOT_DBL_2,
		CMD_BTN_SHOP_C_SLOT_DBL_3,

		CMD_SHOP_SUB_CHAR_ALL,
		CMD_SHOP_SUB_CHAR_CHAR,
		CMD_SHOP_SUB_CHAR_ACC,

		CMD_CHECK_INVEN,
		CMD_CHECK_PRESENT,
		CMD_CHECK_M_WEAPON,
		CMD_CHECK_M_CHAR,
		CMD_CHECK_M_ETC,

		CMD_BTN_S_ALL,
		CMD_BTN_S_FIRST,
		CMD_BTN_S_SECOND,
		CMD_BTN_S_THIRD,
		CMD_BTN_S_FOUR,

		CMD_BTN_USE,
		CMD_BTN_THROW,

		CMD_SCROLL_INVEN,
		CMD_SCROLL_UP_INVEN,
		CMD_SCROLL_DOWN_INVEN,

		// Inven Weapon
		CMD_BTN_INVEN_WEAPON_0,
		CMD_BTN_INVEN_WEAPON_1,
		CMD_BTN_INVEN_WEAPON_2,
		CMD_BTN_INVEN_WEAPON_3,
		CMD_BTN_INVEN_WEAPON_4,
		CMD_BTN_INVEN_WEAPON_5,
		CMD_BTN_INVEN_WEAPON_6,
		CMD_BTN_INVEN_WEAPON_7,
		CMD_BTN_INVEN_WEAPON_8,

		CMD_BTN_INVEN_WEAPON_DBL_0,
		CMD_BTN_INVEN_WEAPON_DBL_1,
		CMD_BTN_INVEN_WEAPON_DBL_2,
		CMD_BTN_INVEN_WEAPON_DBL_3,
		CMD_BTN_INVEN_WEAPON_DBL_4,
		CMD_BTN_INVEN_WEAPON_DBL_5,
		CMD_BTN_INVEN_WEAPON_DBL_6,
		CMD_BTN_INVEN_WEAPON_DBL_7,
		CMD_BTN_INVEN_WEAPON_DBL_8,

		// Inven Char
		CMD_BTN_INVEN_CHAR_0,
		CMD_BTN_INVEN_CHAR_1,
		CMD_BTN_INVEN_CHAR_2,
		CMD_BTN_INVEN_CHAR_3,
		CMD_BTN_INVEN_CHAR_4,
		CMD_BTN_INVEN_CHAR_5,

		CMD_BTN_INVEN_CHAR_DBL_0,
		CMD_BTN_INVEN_CHAR_DBL_1,
		CMD_BTN_INVEN_CHAR_DBL_2,
		CMD_BTN_INVEN_CHAR_DBL_3,
		CMD_BTN_INVEN_CHAR_DBL_4,
		CMD_BTN_INVEN_CHAR_DBL_5,

		// Char Sub
		CMD_SUB_CHAR_ALL,
		CMD_SUB_CHAR_CHAR,
		CMD_SUB_CHAR_ACC,
	};

	LTIntPt		g_MousePos;
	
#define INVEN_TEXTURE_SIZE			32.0f
	//[SPIKE]
	float g_fClickTime = 0.0f;
	#define  GAMESTART_TIME 20

	//----------------- Open 용으로 다시 만든 부분
	HTEXTURE			g_hEmptyWeaponDetailTx	= LTNULL;
	HTEXTURE			g_hEmptyCharDetailTx	= LTNULL;
	// weapon sub set
	int					g_nOldSelectWeapon;
	int					g_nSelectWeapon;
	LTRect				g_rtInvenBoxBase;
	int					g_nCurSubTab	= 0;
	int					g_nSubStep[5];
	int					g_nCurSubStep[5];
	float				g_fSubRatio[5];
	HTEXTURE			g_hEmptyInevnTx = LTNULL;
	// char sub set
	int					g_nSelectItem;
	int					g_nSelectItemClass;
	int					g_nCharCurSubTab = 0;
	int					g_nCharSubStep[3];
	int					g_nCharCurSubStep[3];
	float				g_fCharSubRatio[3];
	// Shop Weapon sub set
	int					g_nOldShopSelectWeapon;
	int					g_nShopSelectWeapon;
	int					g_nShopCurWpSubTab = 0;
	int					g_nShopSubStep[5];
	int					g_nShopCurSubStep[5];
	float				g_fShopSubRatio[5];
	// Shop Char sub set
	int					g_nShopSelectItem;
	int					g_nShopSelectItemClass;
	int					g_nShopCharCurSubTab = 0;
	int					g_nShopCharSubStep[3];
	int					g_nShopCharCurSubStep[3];
	float				g_fShopCharSubRatio[3];
}

CScreenInventory::CScreenInventory()
{
	m_hGradeIcon			= LTNULL;

	m_pBtnExit				= LTNULL;
	m_pBtnCheckRed			= LTNULL;
	m_pBtnCheckBlue			= LTNULL;
	
	//m_pFrameCharBack		= LTNULL;
	m_bClicked				= LTFALSE;

	// 상점 수정
	//m_pBtnCheckArsenal		= LTNULL;
	//m_pBtnCheckPresent		= LTNULL;

	//m_pBtnItemScroll		= LTNULL;
	//m_pBtnItemScrollUp		= LTNULL;
	//m_pBtnItemScrollDown	= LTNULL;

	//memset(m_pFrameUsed,	 0, sizeof(CLTGUIFrame*)*4);
	//memset(m_pFrameItemBack, 0, sizeof(CLTGUIFrame*)*6);
	//memset(m_pFrameItem,	 0, sizeof(CLTGUIFrame*)*6);
	//memset(m_pFrameItemTitle,0, sizeof(CLTGUIFrame*)*6);
	//memset(m_pBtnUseItem,	 0, sizeof(CLTGUIButton*)*6);
	//memset(m_pBtnDetailItem, 0, sizeof(CLTGUIButton*)*6);

	//memset(m_nCurModelId,	0, sizeof(int)*2);
	//memset(m_nCurHeadSkinId,0, sizeof(int)*2);
	//memset(m_nCurBodySkinId,0, sizeof(int)*2);

	//m_InventoryInfo.nWeaponId[0] = 4;
	//m_InventoryInfo.nWeaponId[1] = 2;
	//m_InventoryInfo.nWeaponId[2] = 0;
	//m_InventoryInfo.nWeaponId[3] = 8;

	// Open 용으로 새로만듬.
	//--------------------------------------------------------------------
	// Shop
	m_pPopupItemBuy			= LTNULL;
	m_IsShop				= false;
	m_pFrmShopBack			= LTNULL;
	m_pBtnShopScroll		= LTNULL;
	m_pBtnShopScrollUp		= LTNULL;
	m_pBtnShopScrollDown	= LTNULL;
	m_pBtnShopBuy			= LTNULL;
	m_pBtnShopPresent		= LTNULL;
	m_pCheckShopWeapon		= LTNULL;
	m_pCheckShopChar		= LTNULL;
	m_pCheckShopEtc			= LTNULL;
	memset( m_pCheckShopWOrder,		0,	sizeof(CLTGUIButton*)*5 );
	memset( m_pBtnShopWpSlot,		0, sizeof(CLTGUIButton*)*5 );
	memset( m_pFrmShopWpSlot,		0, sizeof(CLTGUIFrame*)*5 );
	memset( m_pFontWpPrice,			0, sizeof(CSAD3DText*)*5*MAX_PRICE_SET );
	m_pFrmShopSltWeapon		= LTNULL;

	memset( m_pCheckShopChOrder,	0, sizeof(CLTGUIButton*)*3 );
	memset( m_pBtnShopChSlot,		0, sizeof(CLTGUIButton*)*4 );
	memset( m_pFrmShopChSlot,		0, sizeof(CLTGUIFrame*)*4 );
	memset( m_pFontChPrice,			0, sizeof(CSAD3DText*)*4*MAX_PRICE_SET );
	memset( m_pFontShopChName,		0, sizeof(CSAD3DText*)*4 );
	
	m_pFrmShopSltChar		= LTNULL;
	// 여기까지 Shop

	memset( m_pBtnInvenSlot,		0, sizeof(CLTGUIButton*)*9 );
	memset( m_pFrameInvenSlot,		0, sizeof(CLTGUIFrame*)*9 );
	memset( m_pFrameSelectedWeapon, 0, sizeof(CLTGUIFrame*)*4 );
	memset( m_pFontWeaponTitle,		0, sizeof(CSAD3DText*)*9 );	
	memset( m_pFrameInvenUsed,		0, sizeof(CLTGUIFrame*)*4 );
	memset( m_SelectedWeaponId,		255, sizeof(int)*4 );
	memset( m_SelectedCharItemId,	255, sizeof(int)*3 );
	m_pFrameSelect			= LTNULL;
	m_pFrameWeaponDetail	= LTNULL;
	m_pFontWeaponName		= LTNULL;
	m_pFontWeaponDur		= LTNULL;
	m_pFrmInvenBack			= LTNULL;
	memset( m_pFontShopWpName,		0, sizeof(CSAD3DText*)*5 );

	// gauge
	memset( m_pFrameWeaponInfo,		0, sizeof(CLTGUIFrame*)*4 );
	memset( m_pFontWeaponInfo,		0, sizeof(CSAD3DText*)*4 );
	memset( m_pFrameGaugeBar,		0, sizeof(CLTGUIFrame*)*4 );
	m_fDamage				= 0.0f;
	m_fShoot				= 0.0f;
	m_fHit					= 0.0f;
	m_fReact				= 0.0f;
	m_fDeltaDamage			= 0.0f;
	m_fDeltaShoot			= 0.0f;
	m_fDeltaHit				= 0.0f;
	m_fDeltaReact			= 0.0f;
	m_pFontWeaponClip		= LTNULL;
	m_hGaugeSound			= LTNULL;
	m_pFontWeaponDesc		= LTNULL;

	// character
	memset( m_pBtnCharSlot,		0, sizeof(CLTGUIButton*)*6 );
	memset( m_pFrmCharSlot,		0, sizeof(CLTGUIFrame*)*6 );
	memset( m_pFrmUsedCharIcon,	0, sizeof(CLTGUIFrame*)*3 );
	m_pFrmSelectCharSlot	= LTNULL;
	m_pFrmSelectCharDetail	= LTNULL;
	m_pFontCharDesc			= LTNULL;
	m_pFontCharDuration		= LTNULL;
	m_pFontCharTitle		= LTNULL;
	m_pCheckCharAll			= LTNULL;
	m_pCheckCharChar		= LTNULL;
	m_pCheckCharAccessory	= LTNULL;
	m_pFrmDetailBack		= LTNULL;
	memset( m_pFontCharName,	0, sizeof(CSAD3DText*)*6 );

	// Font
	m_pInfoNickCap			= LTNULL;
	m_pInfoNick				= LTNULL;
	m_pInfoGrade			= LTNULL;
	m_pInfoWinPercent		= LTNULL;
	m_pInfoKillDeath		= LTNULL;
	m_pInfoExp				= LTNULL;
	m_pInfoGameMoney		= LTNULL;
	m_pInfoGradeCap			= LTNULL;
	m_pInfoWinPercentCap	= LTNULL;
	m_pInfoKillDeathCap		= LTNULL;
	m_pInfoExpCap			= LTNULL;
	m_pFrameDetail			= LTNULL;
	m_pChannelInfo			= LTNULL;
	m_pTextScroll			= LTNULL;

	m_pCheckMWeapon			= LTNULL;
	m_pCheckMChar			= LTNULL;
	m_pCheckMEtc			= LTNULL;
	memset( m_pCheckSubWeapon, 0, sizeof(CLTGUIButton*)*5 );

	m_pBtnUse				= LTNULL;
	m_pBtnThrow				= LTNULL;

	m_pCheckInven			= LTNULL;
	m_pCheckPresent			= LTNULL;

	m_pBtnScroll			= LTNULL;
	m_pBtnScrollUp			= LTNULL;
	m_pBtnScrollDown		= LTNULL;

	g_pScreenInventory = this;
}

CScreenInventory::~CScreenInventory()
{
	RemoveAllShop();
	Term();
}

LTBOOL CScreenInventory::Build()
{
	// Create Backframe
	CreateBackWindow(SCREEN_ID_INVENTORY);

	char szName[256] = {0, };

	// Shop
	m_pFrmShopBack	= AddFrame( "ShopBackRt", "ShopBackTx", LTTRUE );	
	
	m_pBtnShopScroll		= AddDragButton(CMD_SCROLL_SHOP,
											"ShopScrollRt",
											"ScrollHandleN",
											"ScrollHandleH",
											NULL,
											"ScrollHandleC");
	m_pBtnShopScrollUp		= AddButton(CMD_SCROLL_UP_SHOP,
										"ShopScrollUpRt",
										"ScrollUpN",
										"ScrollUpH",
										"ScrollUpD",
										"ScrollUpC",
										LTTRUE);
	m_pBtnShopScrollDown	= AddButton(CMD_SCROLL_DOWN_SHOP,
										"ShopScrollDnRt",
										"ScrollDnN",
										"ScrollDnH",
										"ScrollDnD",
										"ScrollDnC",
										LTTRUE);
	m_pBtnShopBuy			= AddButton(CMD_SHOP_BUY, "BtnBuyRt", "BuyNTx", "BuyHTx", "BuyDTx", "BuyCTx", LTTRUE);
	m_pBtnShopPresent		= AddButton(CMD_SHOP_PRESENT, "BtnPresentRt", "PresentNTx", "PresentHTx", "PresentDTx", "PresentCTx", LTTRUE);
	
	m_pBtnShopScroll->SetLockX(LTTRUE);
	m_pBtnShopScroll->SetLimitY( 171, 462 );	

	m_pCheckShopWeapon		= AddButton(CMD_CHECK_SHOP_WEAPON, "CheckSWeapon", "CheckMWeaponOff", LTNULL, 0, "CheckMWeaponOn");
	m_pCheckShopChar		= AddButton(CMD_CHECK_SHOP_CHAR, "CheckSChar", "CheckMCharOff", LTNULL, 0, "CheckMCharOn");
	m_pCheckShopEtc			= AddButton(CMD_CHECK_SHOP_ETC, "CheckSEtc", "CheckMEtcOff", LTNULL, 0, "CheckMEtcOn");
	m_pCheckShopWeapon->SetCheck(LTTRUE);
	m_pCheckShopChar->SetCheck(LTFALSE);
	m_pCheckShopEtc->SetCheck(LTFALSE);
	
	m_pCheckShopWOrder[0]		= AddButton(CMD_CHECK_SHOP_W_ALL, "BtnSAllRt", "BtnSAllNTx", LTNULL, 0, "BtnSAllSTx");
	m_pCheckShopWOrder[0]->SetBasePos(20,128);
	m_pCheckShopWOrder[0]->SetParam1(0);
	m_pCheckShopWOrder[1]		= AddButton(CMD_CHECK_SHOP_W_FIRST, "BtnSFirstRt", "BtnSFirstNTx", LTNULL, 0, "BtnSFirstSTx");
	m_pCheckShopWOrder[1]->SetBasePos(94,128);
	m_pCheckShopWOrder[1]->SetParam1(1);
	m_pCheckShopWOrder[2]		= AddButton(CMD_CHECK_SHOP_W_SECOND, "BtnSSecondRt", "BtnSSecondNTx", LTNULL, 0, "BtnSSecondSTx");
	m_pCheckShopWOrder[2]->SetBasePos(165,128);
	m_pCheckShopWOrder[2]->SetParam1(2);
	m_pCheckShopWOrder[3]		= AddButton(CMD_CHECK_SHOP_W_THIRD, "BtnSThirdRt", "BtnSThirdNTx", LTNULL, 0, "BtnSThirdSTx");
	m_pCheckShopWOrder[3]->SetBasePos(236,128);
	m_pCheckShopWOrder[3]->SetParam1(3);
	m_pCheckShopWOrder[4]		= AddButton(CMD_CHECK_SHOP_W_FOUR, "BtnSFourRt", "BtnSFourNTx", LTNULL, 0, "BtnSFourSTx");
	m_pCheckShopWOrder[4]->SetBasePos(307,128);
	m_pCheckShopWOrder[4]->SetParam1(4);

	for( int i = 0; i < 5; i++ )
	{
		sprintf( szName , "ShopWpSlot%d", i );
		m_pBtnShopWpSlot[i]		= AddButton(CMD_BTN_SHOP_W_SLOT_0+i,
											szName,
											"BtnShopWpSlotNTx",
											"BtnShopWpSlotSTx",
											"BtnShopWpSlotDTx",
											"BtnShopWpSlotCTx",
											LTTRUE );
		m_pBtnShopWpSlot[i]->SetParam1(i);
		m_pBtnShopWpSlot[i]->SetDblCommand(CMD_BTN_SHOP_W_SLOT_DBL_0 + i);

		sprintf( szName, "FrmShopWSlotRt%d", i);
		m_pFrmShopWpSlot[i]		= AddFrame( szName, "FrmWeaponSlotTx", LTTRUE );
	}
	m_pFrmShopSltWeapon		= AddFrame("FrmSelectRt", "FrmSelectTx", LTTRUE);
	m_pFrmShopSltWeapon->Show(LTFALSE);	
	for( i = 0; i < 5; i++ )
	{
		for( int j = 0; j <MAX_PRICE_SET ; j++ )
		{
			m_pFontWpPrice[i][j] = new CSAD3DText();
			m_pFontWpPrice[i][j]->CreateD3DText( 128, 16, false );
			m_pFontWpPrice[i][j]->SetPosition( 138 + ((j%2)*109), 185 + (j/2)*20 + (i*71) );
			m_pFontWpPrice[i][j]->SetTransparent( true );
			m_pFontWpPrice[i][j]->SetTextColor(RGB(153,193,190));
			m_pFontWpPrice[i][j]->DrawD3DText("", lstrlen(""));
		}
	}

	m_pCheckShopChOrder[0]			= AddButton( CMD_SHOP_SUB_CHAR_ALL, "BtnSAllRt", "CheckSCharAllOff", LTNULL, 0, "CheckSCharAllOn");
	m_pCheckShopChOrder[0]->SetParam1(0);
	m_pCheckShopChOrder[0]->SetBasePos( 22, 128);
	m_pCheckShopChOrder[0]->Show(LTFALSE);
	m_pCheckShopChOrder[1]		= AddButton( CMD_SHOP_SUB_CHAR_CHAR, "BtnSFirstRt", "CheckSCharCharOff", LTNULL, 0, "CheckSCharCharOn");
	m_pCheckShopChOrder[1]->SetParam1(1);
	m_pCheckShopChOrder[1]->SetBasePos( 96, 128 );
	m_pCheckShopChOrder[1]->Show(LTFALSE);
	m_pCheckShopChOrder[2]	= AddButton( CMD_SHOP_SUB_CHAR_ACC, "BtnSSecondRt", "CheckSCharAccOff", LTNULL, 0, "CheckSCharAccOn");
	m_pCheckShopChOrder[2]->SetParam1(2);
	m_pCheckShopChOrder[2]->SetBasePos( 170, 128 );
	m_pCheckShopChOrder[2]->Show(LTFALSE);
	for( i = 0; i < 4; i++ )
	{
		sprintf( szName, "BtnShopChSlotRt%d", i );
		m_pBtnShopChSlot[i]	= AddButton( CMD_BTN_SHOP_C_SLOT_0 + i,
										 szName,
										 "BtnShopChSlotNTx",
										 "BtnShopChSlotSTx",
										 "BtnShopChSlotDTx",
										 "BtnShopChSlotCTx",
										 LTTRUE );
		m_pBtnShopChSlot[i]->SetParam1(i);
		m_pBtnShopChSlot[i]->SetDblCommand(CMD_BTN_SHOP_C_SLOT_DBL_0+i);
		m_pBtnShopChSlot[i]->Show(LTFALSE);
		sprintf( szName , "FrmShopChSlotRt%d", i );
		m_pFrmShopChSlot[i] = AddFrame( szName, "FrmCharInvenTx", LTTRUE );
		m_pFrmShopChSlot[i]->Show(LTFALSE);
	}
	m_pFrmShopSltChar		= AddFrame( "ShopCharSelectRt", "InvenCharSelectTx", LTTRUE );
	m_pFrmShopSltChar->Show(LTFALSE);
	for( i = 0; i < 4; i++ )
	{
		for( int j = 0; j < MAX_PRICE_SET; j++ )
		{
			m_pFontChPrice[i][j] = new CSAD3DText();
			m_pFontChPrice[i][j]->CreateD3DText( 128, 16, false );
			m_pFontChPrice[i][j]->SetPosition( 99 + ((i%2)*174), 179 + (21*j) + ((i/2)*183) );
			m_pFontChPrice[i][j]->SetTransparent( true );
			m_pFontChPrice[i][j]->SetTextColor(RGB(153,193,190));
			m_pFontChPrice[i][j]->DrawD3DText("", lstrlen(""));
		}
	}
	for(i = 0; i < 4; i++ )
	{
		m_pFontShopChName[i]	= new CSAD3DText();
		m_pFontShopChName[i]->CreateD3DText( 128, 16, false );
		m_pFontShopChName[i]->SetPosition( 22+((i%2)*172), 162+((i/2)*183) );
		m_pFontShopChName[i]->SetTransparent( true );
		m_pFontShopChName[i]->SetTextColor(RGB(255,255,255));
		m_pFontShopChName[i]->DrawD3DText("", lstrlen(""));
	}
	for( i = 0; i < 5; i++ )
	{
		m_pFontShopWpName[i] = new CSAD3DText();
		m_pFontShopWpName[i]->CreateD3DText( 128, 16, false );
		m_pFontShopWpName[i]->SetPosition( 28, 166 +(i*70) );
		m_pFontShopWpName[i]->SetTransparent( true );
		m_pFontShopWpName[i]->SetTextColor(RGB(255,255,255));
		m_pFontShopWpName[i]->DrawD3DText("m_pFontShopWpName", lstrlen("m_pFontShopWpName"));
	}	
	// 여기까지 Shop
	m_pFrmInvenBack = AddFrame( "InvenBackRt", "InvenBackTx", LTTRUE );
	m_pFrmInvenBack->Show(LTFALSE);

	g_rtInvenBoxBase = g_pLayoutMgr->GetRect("ScreenInventory", "InvenBoxRect");

	g_hEmptyInevnTx = g_pInterfaceResMgr->GetTexture("sa_interface\\menu\\textures\\inventory\\EmptyWeapon.dtx");

	m_pBtnCheckRed	= AddButton(CMD_INVEN_CHECK_RED, "CheckRedRect", "RedCheckOff", LTNULL, 0, "RedCheckOn");

	if( m_pBtnCheckRed )
		m_pBtnCheckRed->SetCheck();

	m_pBtnCheckBlue	= AddButton(CMD_INVEN_CHECK_BLUE, "CheckBlueRect", "BlueCheckOff", LTNULL, 0, "BlueCheckOn");

	m_pFrameDetail	= AddFrame("FrmDetailRt", "FrmDetailTx", LTTRUE);
	
	for( i = 0; i < 9; i++ )
	{
		sprintf(szName, "BtnWeaponSlotRt%d", i);
		m_pBtnInvenSlot[i] = AddButton( CMD_BTN_INVEN_WEAPON_0+i,
										szName,
										"BtnWeaponSlotNTx",
										"BtnWeaponSlotSTx",
										"BtnWeaponSlotDTx",
										"BtnWeaponSlotCTx",
										LTTRUE );
		m_pBtnInvenSlot[i]->SetParam1(i);
		m_pBtnInvenSlot[i]->SetDblCommand(CMD_BTN_INVEN_WEAPON_DBL_0 + i);;

		sprintf(szName, "FrmWeaponSlotRt%d", i);
		m_pFrameInvenSlot[i] = AddFrame( szName, "FrmWeaponSlotTx", LTTRUE );
	}	

	for( i = 0; i < 4; i++ )
	{
		sprintf(szName, "SelectedWeaponRt%d", i);
		m_pFrameSelectedWeapon[i] = AddFrame( szName, "FrmWeaponSlotTx", LTTRUE );		

		m_pFrameInvenUsed[i]	= AddFrame( "FrmUsedTxRt", "FrmUsedTx", LTTRUE );
		m_pFrameInvenUsed[i]->Show(LTFALSE);
	}	
	m_pFrameSelect	= AddFrame("FrmSelectRt", "FrmSelectTx", LTTRUE);
	m_pFrameSelect->Show(LTFALSE);
	g_pLayoutMgr->GetString("ScreenInventory", "FrmSelectWDTx", szName, 256 );
	g_hEmptyWeaponDetailTx = g_pInterfaceResMgr->GetTexture(szName);	
	m_pFrameWeaponDetail	= AddFrame( "FrmSelectWDRt", "FrmSelectWDTx", LTTRUE );
	m_pFrameWeaponDetail->Show(LTFALSE);


	BuildUserInfo( 24, 434, 364, 116 );
	
	SetUserInfo( g_GameDoc.GetGrade(),
				 g_GameDoc.GetNick(),
				 g_GameDoc.GetKill(),
				 g_GameDoc.GetDeath(),
				 g_GameDoc.GetWinCount(),
				 g_GameDoc.GetLoseCount(),
				 g_GameDoc.GetDrawCount(),
				 g_GameDoc.GetExp(),
				 g_GameDoc.GetMoney(),
				 0 );

	m_pBtnExit	= AddButton(CMD_INVEN_EXIT, "BtnExitRect", "BtnExitNTx", "BtnExitSTx", "BtnExitDTx", "BtnExitCTx", LTTRUE);	
	m_pBtnExit->SetTooltip( "Ctrl+X" );		m_pBtnExit->SetHotKey( 0x18 );

	m_pCheckInven			= AddButton(CMD_CHECK_INVEN, "CheckInven", "CheckInvenOff", LTNULL, 0, "CheckInvenOn");
	m_pCheckInven->SetCheck(LTTRUE);
	m_pCheckPresent			= AddButton(CMD_CHECK_PRESENT, "CheckPresent", "CheckPresentOff", LTNULL, 0, "CheckPresentOn");
	m_pCheckPresent->SetCheck(LTFALSE);

	m_pCheckMWeapon			= AddButton(CMD_CHECK_M_WEAPON, "CheckMWeapon", "CheckMWeaponOff", LTNULL, 0, "CheckMWeaponOn");
	m_pCheckMWeapon->SetCheck(LTTRUE);
	m_pCheckMChar			= AddButton(CMD_CHECK_M_CHAR, "CheckMChar", "CheckMCharOff", LTNULL, 0, "CheckMCharOn");
	m_pCheckMChar->SetCheck(LTFALSE);
	m_pCheckMEtc			= AddButton(CMD_CHECK_M_ETC, "CheckMEtc", "CheckMEtcOff", LTNULL, 0, "CheckMEtcOn");
	m_pCheckMEtc->SetCheck(LTFALSE);

	m_pCheckSubWeapon[0]			= AddButton(CMD_BTN_S_ALL, "BtnSAllRt", "BtnSAllNTx", LTNULL, 0, "BtnSAllSTx" );
	m_pCheckSubWeapon[0]->SetParam1(0);
	m_pCheckSubWeapon[1]			= AddButton(CMD_BTN_S_FIRST, "BtnSFirstRt", "BtnSFirstNTx", LTNULL, 0, "BtnSFirstSTx" );
	m_pCheckSubWeapon[1]->SetParam1(1);
	m_pCheckSubWeapon[2]			= AddButton(CMD_BTN_S_SECOND, "BtnSSecondRt", "BtnSSecondNTx", LTNULL, 0, "BtnSSecondSTx" );
	m_pCheckSubWeapon[2]->SetParam1(2);
	m_pCheckSubWeapon[3]			= AddButton(CMD_BTN_S_THIRD, "BtnSThirdRt", "BtnSThirdNTx", LTNULL, 0, "BtnSThirdSTx" );
	m_pCheckSubWeapon[3]->SetParam1(3);
	m_pCheckSubWeapon[4]			= AddButton(CMD_BTN_S_FOUR, "BtnSFourRt", "BtnSFourNTx", LTNULL, 0, "BtnSFourSTx" );
	m_pCheckSubWeapon[4]->SetParam1(4);

	m_pBtnUse				= AddButton(CMD_BTN_USE, "BtnUseRt", "BtnUseNTx", "BtnUseSTx", "BtnUseDTx", "BtnUseCTx", LTTRUE );
	m_pBtnThrow				= AddButton(CMD_BTN_THROW, "BtnThrowRt", "BtnThrowNTx", "BtnThrowSTx", "BtnThrowDTx", "BtnThrowCTx", LTTRUE );	

	LTIntPt FontPt;
	for(i = 0; i < 9; i++ )
	{
		FontPt = m_pBtnInvenSlot[i]->GetBasePos();
		m_pFontWeaponTitle[i] = new CSAD3DText();
		m_pFontWeaponTitle[i]->CreateD3DText( 128, 16, false );
		m_pFontWeaponTitle[i]->SetPosition( FontPt.x + 3, FontPt.y + 3 );
		m_pFontWeaponTitle[i]->SetTransparent( true );
		m_pFontWeaponTitle[i]->SetTextColor(RGB(255,255,255));
		m_pFontWeaponTitle[i]->DrawD3DText("", 0 );
	}

	m_pFontWeaponName	= new CSAD3DText();
	m_pFontWeaponName->CreateD3DText( 128, 16, true );
	m_pFontWeaponName->SetPosition( 431, 385 );
	m_pFontWeaponName->SetTransparent( true );
	m_pFontWeaponName->SetTextColor(RGB(255,255,255));
	m_pFontWeaponName->DrawD3DText("", 0 );

	m_pFontWeaponDur	= new CSAD3DText();
	m_pFontWeaponDur->CreateD3DText( 256, 16, false );
	//m_pFontWeaponDur->SetPosition( 566, 385 );
	m_pFontWeaponDur->SetPosition( 506, 385 );
	m_pFontWeaponDur->SetTransparent( true );
	m_pFontWeaponDur->SetTextColor(RGB(250,204,133));
	m_pFontWeaponDur->DrawD3DText( "", 0, DT_RIGHT );

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

	// Gauge
	for( i = 0; i < 4; i++ )
	{
		sprintf(szName, "FrmGaugeRt%d", i);		
		m_pFrameGaugeBar[i]		= AddFrame( szName, "FrmGaugeBarTx", LTTRUE );
		m_pFrameWeaponInfo[i]	= AddFrame( szName, "FrmGaugeTx", LTTRUE );
		m_pFrameWeaponInfo[i]->Show(LTFALSE);

		m_pFontWeaponInfo[i]	= new CSAD3DText();
		m_pFontWeaponInfo[i]->CreateD3DText( 128, 16, false );
		m_pFontWeaponInfo[i]->SetPosition( 742, 407 + i*19 );
		m_pFontWeaponInfo[i]->SetTransparent( true );
		m_pFontWeaponInfo[i]->SetTextColor(RGB(190,160,112));
	}
	m_pFontWeaponClip	= new CSAD3DText();
	m_pFontWeaponClip->CreateD3DText( 128, 16, false );
	m_pFontWeaponClip->SetPosition( 664, 483 );
	m_pFontWeaponClip->SetTransparent( true );
	m_pFontWeaponClip->SetTextColor(RGB(190,160,112));

	m_pFontWeaponDesc = new CSAD3DText();
	m_pFontWeaponDesc->CreateD3DText( 512, 64, false );
	m_pFontWeaponDesc->SetTransparent( true );
	m_pFontWeaponDesc->SetTextColor(RGB(190,160,112));
	m_pFontWeaponDesc->SetPosition( 425, 506 );
	m_pFontWeaponDesc->DrawD3DText( "", 0 );

	//character
	m_pCheckCharAll			= AddButton( CMD_SUB_CHAR_ALL, "BtnSAllRt", "CheckSCharAllOff", LTNULL, 0, "CheckSCharAllOn");
	m_pCheckCharAll->SetParam1(0);
	m_pCheckCharAll->Show(LTFALSE);
	m_pCheckCharChar		= AddButton( CMD_SUB_CHAR_CHAR, "BtnSFirstRt", "CheckSCharCharOff", LTNULL, 0, "CheckSCharCharOn");
	m_pCheckCharChar->SetParam1(1);
	m_pCheckCharChar->Show(LTFALSE);
	m_pCheckCharAccessory	= AddButton( CMD_SUB_CHAR_ACC, "BtnSSecondRt", "CheckSCharAccOff", LTNULL, 0, "CheckSCharAccOn");
	m_pCheckCharAccessory->SetParam1(2);
	m_pCheckCharAccessory->Show(LTFALSE);

	m_pFrmDetailBack		= AddFrame( "CharDetailBack", "CharDetailBackTx", LTTRUE );
	m_pFrmDetailBack->Show(LTFALSE);

	for( i = 0; i < 6; i++ )
	{
		sprintf( szName, "BtnCharSlotRt%d", i);
		m_pBtnCharSlot[i]	= AddButton( CMD_BTN_INVEN_CHAR_0 + i,
										 szName,
										 "BtnCharSlotNTx",
										 "BtnCharSlotSTx",
										 "BtnCharSlotDTx",
										 "BtnCharSlotCTx",
										 LTTRUE );
		m_pBtnCharSlot[i]->SetParam1(i);
		m_pBtnCharSlot[i]->SetDblCommand(CMD_BTN_INVEN_CHAR_DBL_0 + i);;
		m_pBtnCharSlot[i]->Show(LTFALSE);

		sprintf(szName, "FrmCharInvenRt%d", i);
		m_pFrmCharSlot[i]	= AddFrame( szName, "FrmCharInvenTx", LTTRUE );
		m_pFrmCharSlot[i]->Show(LTFALSE);
	}

	m_pFrmSelectCharSlot	= AddFrame( "InvenCharSelectRt", "InvenCharSltTx", LTTRUE );
	m_pFrmSelectCharSlot->Show(LTFALSE);
	for( i = 0; i < 3; i++ )
	{
		m_pFrmUsedCharIcon[i]	= AddFrame( "FrmUsedTxRt", "FrmUsedTx", LTTRUE );
		m_pFrmUsedCharIcon[i]->Show(LTFALSE);
	}

	g_pLayoutMgr->GetString("ScreenInventory", "CharEptSltTx", szName, 256 );
	g_hEmptyCharDetailTx = g_pInterfaceResMgr->GetTexture(szName);	
	m_pFrmSelectCharDetail	= AddFrame( "CharEptSltRt", "CharEptSltTx", LTTRUE );
	m_pFrmSelectCharDetail->Show(LTFALSE);
	m_pFontCharDesc			= new CSAD3DText();
	m_pFontCharDesc->CreateD3DText( 290, 128, false );
	m_pFontCharDesc->SetTransparent( true );
	m_pFontCharDesc->SetTextColor(RGB(190,160,112));
	m_pFontCharDesc->SetPosition( 564, 415 );
	m_pFontCharDesc->DrawD3DText( "", 0 );
	
	m_pFontCharDuration		= new CSAD3DText();
	m_pFontCharDuration->CreateD3DText( 256, 16, false );
	//m_pFontCharDuration->SetPosition( 566, 385 );
	m_pFontCharDuration->SetPosition( 506, 385 );
	m_pFontCharDuration->SetTransparent( true );
	m_pFontCharDuration->SetTextColor(RGB(250,204,133));
	m_pFontCharDuration->DrawD3DText("", 0, DT_RIGHT);

	m_pFontCharTitle		= new CSAD3DText();
	m_pFontCharTitle->CreateD3DText( 128, 16, true );
	m_pFontCharTitle->SetPosition( 428, 385 );
	m_pFontCharTitle->SetTransparent( true );
	m_pFontCharTitle->SetTextColor(RGB(255,255,255));
	m_pFontCharTitle->DrawD3DText("", lstrlen(""));

	for( i = 0; i < 6; i++ )
	{
		m_pFontCharName[i] = new CSAD3DText();
		m_pFontCharName[i]->CreateD3DText( 128, 16, false );
		m_pFontCharName[i]->SetPosition( 424 + ((i%3)*113), 132 + ((i/3)*110) );
		m_pFontCharName[i]->SetTransparent( true );
		m_pFontCharName[i]->SetTextColor(RGB(255,255,255));
		m_pFontCharName[i]->DrawD3DText("", lstrlen(""));
	}

	m_pBtnScrollUp			= AddButton(CMD_SCROLL_UP_INVEN,
										"ScrollUpRt",
										"ScrollUpN",
										"ScrollUpH",
										"ScrollUpD",
										"ScrollUpC",
										LTTRUE);
	m_pBtnScrollDown		= AddButton(CMD_SCROLL_DOWN_INVEN,
										"ScrollDnRt",
										"ScrollDnN",
										"ScrollDnH",
										"ScrollDnD",
										"ScrollDnC",
										LTTRUE);
	m_pBtnScroll			= AddDragButton(CMD_SCROLL_INVEN,
											"ScrollRt",
											"ScrollHandleN",
											"ScrollHandleH",
											NULL,
											"ScrollHandleC");

	m_pBtnScroll->SetLockX(LTTRUE);
	m_pBtnScroll->SetLimitY( 147, 285 );

	// Item buy popup
	AddDummyCtrl();
	m_pPopupItemBuy = new CPopUpItemBuy();
	m_pPopupItemBuy->Init(this);
	AddControl((CLTGUICtrl*)m_pPopupItemBuy);
	
	if (!CScreenBase::Build()) return LTFALSE;

	return LTTRUE;
}

void CScreenInventory::BuildUserInfo( uint16 nPosX, uint16 nPosY, uint16 nWidth, uint16 nHeight )
{
	const int nLineCnt = 6;
	const float nCapWidthProportion = 0.35f;

	int nRow		= 0;
	int nTopAdjust	= 0;
	int nLeftAdjust	= 0;
	int nTextHeight	= (nHeight / nLineCnt)+4;

	int nCaptionWidth = (int)( nWidth * nCapWidthProportion );
	int nContextWidth = nWidth - nCaptionWidth;

	nLeftAdjust = nPosX + nCaptionWidth;

	char szTemp[256] = {LTNULL};

	m_pInfoNickCap			= new CSAD3DText();
	m_pInfoNickCap->CreateD3DText( nContextWidth, nTextHeight, true );
	m_pInfoNickCap->SetPosition( nPosX, nPosY + nTextHeight * nRow );
	m_pInfoNickCap->SetTransparent( true );
	m_pInfoNickCap->SetTextColor( RGB(190,160,112) );
	m_pInfoNickCap->DrawD3DText( "닉네임", lstrlen("닉네임"), DT_LEFT, D3D_TEXT_OUTLINE );

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
	m_pInfoGradeCap->SetTextColor( RGB(190,160,112) );
	m_pInfoGradeCap->DrawD3DText( szTemp, lstrlen(szTemp), DT_LEFT, D3D_TEXT_OUTLINE );

	m_pInfoGrade			= new CSAD3DText();
	m_pInfoGrade->CreateD3DText( nContextWidth, nTextHeight, false );
	m_pInfoGrade->SetPosition( nLeftAdjust + 30, nPosY + nTextHeight * nRow );
	m_pInfoGrade->SetTransparent( true );
	m_pInfoGrade->SetTextColor( RGB(190,160,112) );
	m_pInfoGrade->DrawD3DText( "", 0 );
	g_pDrawPrim->SetXYWH( &m_PolyGrade, (float)nLeftAdjust, float(nPosY + nTextHeight * nRow - 5.0f), INVEN_TEXTURE_SIZE, INVEN_TEXTURE_SIZE );
	g_pDrawPrim->SetRGBA( &m_PolyGrade, 0xFFFFFFFF );
	nRow++;

	//"승률"
	lstrcpy( szTemp, NF_GetMsg(SA_TEXT_USER_INFO_WINPERCENT) );
	m_pInfoWinPercentCap	= new CSAD3DText();
	m_pInfoWinPercentCap->CreateD3DText( nCaptionWidth, nTextHeight, true );
	m_pInfoWinPercentCap->SetPosition( nPosX, nPosY + nTextHeight * nRow );
	m_pInfoWinPercentCap->SetTransparent( true );
	m_pInfoWinPercentCap->SetTextColor( RGB(190,160,112) );
	m_pInfoWinPercentCap->DrawD3DText( szTemp, lstrlen(szTemp), DT_LEFT, D3D_TEXT_OUTLINE );

	m_pInfoWinPercent		= new CSAD3DText();
	m_pInfoWinPercent->CreateD3DText( nContextWidth, nTextHeight, false );
	m_pInfoWinPercent->SetPosition( nLeftAdjust, nPosY + nTextHeight * nRow );
	m_pInfoWinPercent->SetTransparent( true );
	m_pInfoWinPercent->SetTextColor( RGB(190,160,112) );
	m_pInfoWinPercent->DrawD3DText( "", 0 );
	nRow++;
	
	//"Kill/Death"
	lstrcpy( szTemp, NF_GetMsg(SA_TEXT_USER_INFO_KILL_DEATH) );
	m_pInfoKillDeathCap		= new CSAD3DText();
	m_pInfoKillDeathCap->CreateD3DText( nCaptionWidth, nTextHeight, true );
	m_pInfoKillDeathCap->SetPosition( nPosX, nPosY + nTextHeight * nRow );
	m_pInfoKillDeathCap->SetTransparent( true );
	m_pInfoKillDeathCap->SetTextColor( RGB(190,160,112) );
	m_pInfoKillDeathCap->DrawD3DText( szTemp, lstrlen(szTemp), DT_LEFT, D3D_TEXT_OUTLINE );

	m_pInfoKillDeath		= new CSAD3DText();
	m_pInfoKillDeath->CreateD3DText( nContextWidth, nTextHeight, false );
	m_pInfoKillDeath->SetPosition( nLeftAdjust, nPosY + nTextHeight * nRow );
	m_pInfoKillDeath->SetTransparent( true );
	m_pInfoKillDeath->SetTextColor( RGB(190,160,112) );
	m_pInfoKillDeath->DrawD3DText( "", 0 );
	nRow++;
	
	//"경험치"
	lstrcpy( szTemp, NF_GetMsg(SA_TEXT_USER_INFO_EXP) );
	m_pInfoExpCap			= new CSAD3DText();
	m_pInfoExpCap->CreateD3DText( nCaptionWidth, nTextHeight, true );
	m_pInfoExpCap->SetPosition( nPosX, nPosY + nTextHeight * nRow );
	m_pInfoExpCap->SetTransparent( true );
	m_pInfoExpCap->SetTextColor( RGB(190,160,112) );
	m_pInfoExpCap->DrawD3DText( szTemp, lstrlen(szTemp), DT_LEFT, D3D_TEXT_OUTLINE );
	
	m_pInfoExp				= new CSAD3DText();
	m_pInfoExp->CreateD3DText( nContextWidth, nTextHeight, false );
	m_pInfoExp->SetPosition( nLeftAdjust, nPosY + nTextHeight * nRow );
	m_pInfoExp->SetTransparent( true );
	m_pInfoExp->SetTextColor( RGB(190,160,112) );
	m_pInfoExp->DrawD3DText( "", 0 );
	nRow++;

	m_pInfoGameMoney		= new CSAD3DText();
	m_pInfoGameMoney->CreateD3DText( 116, nTextHeight, false );
	m_pInfoGameMoney->SetPosition( 474.0f, 26.0f );
	m_pInfoGameMoney->SetTransparent( true );
	m_pInfoGameMoney->SetTextColor( RGB(190, 160, 112) );
	m_pInfoGameMoney->DrawD3DText( "", 0 );
}

void CScreenInventory::Term()
{
	//SAFE_DELETE_ARRAY(g_pWeaponTx);


	//------------------- open 용으로 새로 만듬
	// Font
	SAFE_DELETE(m_pInfoNickCap);
	SAFE_DELETE(m_pInfoNick);
	SAFE_DELETE(m_pInfoGrade);
	SAFE_DELETE(m_pInfoWinPercent);
	SAFE_DELETE(m_pInfoKillDeath);
	SAFE_DELETE(m_pInfoExp);
	SAFE_DELETE(m_pInfoGameMoney);
	SAFE_DELETE(m_pInfoGradeCap);
	SAFE_DELETE(m_pInfoWinPercentCap);
	SAFE_DELETE(m_pInfoKillDeathCap);
	SAFE_DELETE(m_pInfoExpCap);
	SAFE_DELETE(m_pChannelInfo);
	SAFE_DELETE(m_pTextScroll);
	
	for( int i = 0; i < 9; i++ )
		SAFE_DELETE(m_pFontWeaponTitle[i]);

	SAFE_DELETE(m_pFontWeaponName);
	SAFE_DELETE(m_pFontWeaponDur);

	for( i = 0; i < 4; i++)
		SAFE_DELETE(m_pFontWeaponInfo[i]);

	SAFE_DELETE(m_pFontWeaponClip);
	SAFE_DELETE(m_pFontWeaponDesc);

	//character
	SAFE_DELETE(m_pFontCharDesc);
	SAFE_DELETE(m_pFontCharDuration);
	SAFE_DELETE(m_pFontCharTitle);

	for( i = 0; i < 6; i++ )
	{
		SAFE_DELETE(m_pFontCharName[i]);
	}	

	//Shop	
	for( i = 0; i < 5; i++ )
	{
		for( int j = 0; j < MAX_PRICE_SET; j++ )
		{
			SAFE_DELETE(m_pFontWpPrice[i][j]);
		}
	}
	for( i = 0; i < 4; i++ )
	{
		for( int j = 0; j < MAX_PRICE_SET; j++ )
		{
			SAFE_DELETE(m_pFontChPrice[i][j]);
		}
	}
	for(i = 0; i < 5; i++ )
	{
		SAFE_DELETE(m_pFontShopWpName[i]);
	}
	for(i = 0; i < 4; i++ )
	{
		SAFE_DELETE(m_pFontShopChName[i]);
	}
}

LTBOOL CScreenInventory::OnMouseMove(int x, int y)
{
	if( m_bClicked )
	{
		float fResult = (g_MousePos.x - x)*0.01f;

		LTRotation rYaw, rChar;
		LTVector vPos;
		rYaw.Init();
		rYaw.Rotate( LTVector(0,1,0), fResult );

		g_pLTClient->GetObjectRotation( m_CharacterSFX[0].GetObject(), &rChar );
		rChar = rChar * rYaw;
		g_pLTClient->SetObjectRotation(m_CharacterSFX[0].GetObject(), &rChar);

		g_MousePos.x = x;
		g_MousePos.y = y;
	}

	g_fClickTime = g_pLTClient->GetTime();

	return CBaseScreen::OnMouseMove(x, y);
}

LTBOOL CScreenInventory::OnLButtonDown(int x, int y)
{
	if( x >= 145 && x < 259 && y >=101 && y < 393 )
	{
		g_MousePos.x = x; g_MousePos.y = y;
		m_bClicked = LTTRUE;
	}
	
	return CBaseScreen::OnLButtonDown(x, y);
}

LTBOOL CScreenInventory::OnLButtonUp(int x, int y)
{
	g_fClickTime = g_pLTClient->GetTime();
	
	m_bClicked = LTFALSE;
	return CBaseScreen::OnLButtonUp(x, y);
}

LTBOOL CScreenInventory::RenderCloud()
{
	if ( m_hTxCloud )
	{
		g_pDrawPrim->SetTexture( m_hTxCloud );
		g_pDrawPrim->DrawPrim( &m_hPolyCloud );
		
		static LTFLOAT fCloudSceneAdjust = 0;
		
		fCloudSceneAdjust += g_pLTClient->GetFrameTime() * 0.015f;
		
		SetupQuadUVs( m_hPolyCloud, m_hTxCloud, 0.0f + fCloudSceneAdjust, 0.0f, 2.0f, 1.0f );
		
		
		if ( fCloudSceneAdjust > 1.0f )
		{
			fCloudSceneAdjust -= 1.0f;
		}
	}
	
	return LTTRUE;
}

void CScreenInventory::CreateBackWindow(eScreenID id)
{
	// 구름 Texture
	m_hTxCloud = g_pInterfaceResMgr->GetTexture( "SA_INTERFACE/MENU/TEXTURES/COMMON/BackFrameCloud.dtx" );
	SetupQuadUVs( m_hPolyCloud, m_hTxCloud, 0.0f, 0.0f, 1.0f, 1.0f );
	//g_pDrawPrim->SetXYWH( &m_hPolyCloud, 0, 75, 1024, 550 );
	g_pDrawPrim->SetXYWH( &m_hPolyCloud, 0, 0, 1024, 550 );
	g_pDrawPrim->SetRGBA( &m_hPolyCloud, 0xFFFFFFFF );
	
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

void CScreenInventory::ShopOrInvenInitialize()
{
	m_pBtnShopScroll->SetPosYRatio( 0.0f );
	m_pBtnScroll->SetPosYRatio( 0.0f );
	// Inven Weapon
	g_nOldSelectWeapon = -1;
	g_nSelectWeapon = -1;
	g_nCurSubTab	= 0;
	for(int i = 0; i < 5; i++ )
	{
		g_nSubStep[i] = 0;
		g_nCurSubStep[i] = 0;
		g_fSubRatio[i] = 0.0f;
	}
	
	// Inven Char
	g_nSelectItem		= -1;
	g_nSelectItemClass	= 0;
	g_nCharCurSubTab	= 0;
	for( i = 0; i < 3; i++ )
	{
		g_nCharSubStep[i]		= 0;
		g_nCharCurSubStep[i]	= 0;
		g_fCharSubRatio[i]		= 0.0f;
	}

	// Shop Weapon
	g_nOldShopSelectWeapon	= -1;
	g_nShopSelectWeapon		= -1;
	g_nShopCurWpSubTab		= 0;
	for( i = 0; i < 5; i++ )
	{
		g_nShopSubStep[i];
		g_nShopCurSubStep[i];
		g_fShopSubRatio[i];
	}

	// Shop Char
	g_nShopSelectItem		= -1;
	g_nShopSelectItemClass	= 0;
	g_nShopCharCurSubTab	= 0;
	for( i = 0; i < 3; i++ )
	{
		g_nShopCharSubStep[i];
		g_nShopCharCurSubStep[i];
		g_fShopCharSubRatio[i];
	}
}

void CScreenInventory::OnFocus(LTBOOL bFocus)
{
	if( bFocus )
	{
		//[MURSUM]
		CScreenMgr* pScreenMgr = g_pInterfaceMgr->GetScreenMgr();
		if( pScreenMgr && SCREEN_ID_MAIN == pScreenMgr->GetPreviousScreenID() )
		{
			//Shop 추가
		}

		if( m_IsShop )
		{
			m_pFrmInvenBack->Show(LTFALSE);
			m_pCheckShopWeapon->Show(LTTRUE);
			m_pCheckShopChar->Show(LTTRUE);
			m_pCheckShopEtc->Show(LTTRUE);
			m_pBtnShopScrollUp->Show(LTTRUE);
			m_pBtnShopScrollDown->Show(LTTRUE);
			m_pBtnShopBuy->Show(LTTRUE);
			m_pBtnShopPresent->Show(LTTRUE);
			for( int i = 0; i < 4; i++ )
			{
				m_pFrameSelectedWeapon[i]->Show(LTFALSE);
			}			
			this->ShowShopWeapon(LTTRUE);			
		}
		else
		{
			m_pFrmInvenBack->Show(LTTRUE);
			for( int i = 0; i < 4; i++ )
			{
				sInven_Set* pInven = this->GetWeapon(m_SelectedWeaponId[i]);
				if( pInven )
				{
					m_pFrameSelectedWeapon[i]->SetFrame(pInven->hWeaponTx);
				}
				m_pFrameSelectedWeapon[i]->Show(LTTRUE);
			}			
			ShopAllHide();
		}

		ShopOrInvenInitialize();

		this->ShowWeapon(LTTRUE);

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
		
		SetUserInfo( g_GameDoc.GetGrade(),
					 g_GameDoc.GetNick(),
					 g_GameDoc.GetKill(),
					 g_GameDoc.GetDeath(),
					 g_GameDoc.GetWinCount(),
					 g_GameDoc.GetLoseCount(),
					 g_GameDoc.GetDrawCount(),
					 g_GameDoc.GetExp(),
					 g_GameDoc.GetMoney(),
					 0 );

		char szChannelInfo[128];
		sprintf( szChannelInfo, "%s-%d", g_GameDoc.GetChannelName(), g_GameDoc.GetChannelIndex()+1 );
		m_pChannelInfo->DrawD3DText( szChannelInfo, strlen(szChannelInfo), DT_CENTER );

		m_pTextScroll->Clear();
		SetRanking();

		g_fClickTime = g_pLTClient->GetTime();

		
		g_Network.EnableNetwork(LTTRUE);

		g_pLTClient->ClearInput();
		

		//g_nCurStep = 0;
		//m_pBtnItemScroll->SetPosYRatio(0);
		UpdateInvView( 0, LTTRUE );
		

		
		// Inven Weapon
		g_nSelectWeapon = -1;

		// Inven Char
		g_nSelectItem	= -1;
		g_nSelectItemClass = 0;

		// Shop Weapon
		g_nShopSelectWeapon = -1;
		g_nShopCurWpSubTab = 0;
		for( n = 0; n < 5; n++ )
		{
			g_nShopCurSubStep[n] = 0;
		}		

		g_nCurSubTab		= 0;
		g_nCharCurSubTab	= 0;
		m_pCheckMWeapon->SetCheck(LTTRUE);
		m_pCheckMChar->SetCheck(LTFALSE);
		m_pCheckMEtc->SetCheck(LTFALSE);
		for( n = 0; n < 5; n++ )
		{
			g_nCurSubStep[5] = 0;
		}
		
		
		g_Network.EnableNetwork(LTTRUE);

		g_pInterfaceMgr->CloseMessageBox(LTTRUE);
		//----------------------------------------------------------------------------------
		// test test test test test test test
		//----------------------------------------------------------------------------------
		
		int nWeaponSize = g_pWeaponMgr->GetNumWeapons();
		for( n = 0; n < nWeaponSize; n++ )
		{
			WEAPON const* pWeapon = g_pWeaponMgr->GetWeapon(n);
			if( pWeapon )
			{
				int nClass = g_pWeaponMgr->GetWeaponClass(n);
				if( nClass != 5 )
				{
					AddWeapon(n, 0);

					AddShopWeapon( n, 10*n, 20*n, 30*n, 40*n );
				}
			}
		}
		this->ShowSubWeapon(0);
		// char 정보
		for( n = 0; n < 3; n++ )
		{
			ModelId nModelId = g_pModelButeMgr->GetTeamRedModel(n);
			AddCharTab( nModelId, 0, 20, 1 );
			AddShopCharTab( nModelId, 0, 1, n*10, n*20, n*30, n*40 );
		}
		for( n = 0; n < 3; n++ )
		{
			ModelId nModelId = g_pModelButeMgr->GetTeamBlueModel(n);
			AddCharTab( nModelId, 1, 23, 1 );
			AddShopCharTab( nModelId, 1, 1, n*10, n*20, n*30, n*40 );
		}
		for( n = 0; n < 2; n++ )
		{
			AddCharTab( n, 1, 40, 2 );
			AddShopCharTab( n, 1, 2, n*10, n*20, n*30, n*40 );
		}
		if( m_IsShop )
		{
			this->ShowShopWeapon(LTTRUE);
		}
		
		//----------------------------------------------------------------------------------
		
		SetDefaultCharacter( TEAM_B );
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
		if( m_hGaugeSound )
		{
			g_pLTClient->SoundMgr()->KillSound( m_hGaugeSound );
			m_hGaugeSound	= LTNULL;
		}
		
		SelectAllClear();

		g_Network.EnableNetwork(LTFALSE);
	
		g_fClickTime = 0.0f;

		/*
		if( m_pPopUpWeaponDetail->IsVisible() ) m_pPopUpWeaponDetail->CtrlHide();

		g_Network.EnableNetwork(LTFALSE);

		*/
	}
	CScreenBase::OnFocus(bFocus);

	if( bFocus )
	{
		HOBJECT hCamera = g_pInterfaceMgr->GetInterfaceCamera();
		if( hCamera )
		{
			g_pLTClient->SetCameraFOV(hCamera, DEG2RAD(80.0f), DEG2RAD(60.0f) );
		}
		UpdateModel();

		ShowMainShop();
	}
	else
	{
		HOBJECT hCamera = g_pInterfaceMgr->GetInterfaceCamera();
		if( hCamera )
		{
			g_pLTClient->SetCameraFOV(hCamera, DEG2RAD(90.0f), DEG2RAD(75.0f) );
		}
	}
}

uint32 CScreenInventory::OnCommand(uint32 dwCommand, uint32 dwParam1, uint32 dwParam2)
{
	switch(dwCommand)
	{
		case CMD_INVEN_EXIT	:
			{
				if( m_IsShop )
				{
					g_Network.CS_ExitShop();
					g_pInterfaceMgr->ShowMessageBox( SA_TEXT_EXIT_SHOP, LTMB_MESSAGE, LTNULL, LTNULL );
				}
				else
				{
					CScreenMgr* pScreenMgr = g_pInterfaceMgr->GetScreenMgr();
					if( pScreenMgr && SCREEN_ID_MAIN == pScreenMgr->GetPreviousScreenID() )
					{
						g_pInterfaceMgr->SwitchToScreen(SCREEN_ID_MAIN);
					}
					else
					{
						g_Network.CS_LeaveInven();
					}
				}
			}
			break;

			//Shop
		case CMD_OK :
			{				
				//ItemBuy(dwParam1, dwParam2);
			}break;
		case CMD_SHOP_BUY :
			{
				if( m_pCheckShopWeapon->GetCheck() )
				{
					BuyWeaponInfo();					
				}
				else if( m_pCheckShopChar->GetCheck() )
				{
					BuyCharInfo();					
				}
				else
				{
				}
			}break;
		case CMD_SCROLL_SHOP :
			{
				if( m_IsShop )
				{
					if( dwParam1 == CMD_PARAM_MOUSEMOVE )
					{
						if( m_pCheckShopWeapon->GetCheck() )
						{
							float tempPos = m_pBtnShopScroll->GetPosYRatio();
							int tempStep = (int)(tempPos/g_fShopSubRatio[g_nShopCurWpSubTab]);
							if( tempStep != g_nShopCurSubStep[g_nShopCurWpSubTab] )
							{
								g_nShopCurSubStep[g_nShopCurWpSubTab] = tempStep;

								this->ShowShopSubWeapon(g_nShopCurWpSubTab);
							}
						}
						else if ( m_pCheckShopChar->GetCheck() )
						{
							float tempPos = m_pBtnShopScroll->GetPosYRatio();
							int tempStep = (int)(tempPos/g_fShopCharSubRatio[g_nShopCurWpSubTab]);
							if( tempStep != g_nShopCharCurSubStep[g_nShopCurWpSubTab] )
							{
								g_nShopCharCurSubStep[g_nShopCurWpSubTab] = tempStep;

								this->ShowShopSubChar(g_nShopCurWpSubTab);
							}
						}
					}
				}
			}break;
		case CMD_SCROLL_UP_SHOP :
			{
				if( m_IsShop )
				{
					this->OnShopScrollUp();
				}
			}break;
		case CMD_SCROLL_DOWN_SHOP :
			{
				if( m_IsShop )
				{
					this->OnShopScrollDown();
				}
			}break;

		case CMD_BTN_SHOP_W_SLOT_0 :
		case CMD_BTN_SHOP_W_SLOT_1 :
		case CMD_BTN_SHOP_W_SLOT_2 :
		case CMD_BTN_SHOP_W_SLOT_3 :
		case CMD_BTN_SHOP_W_SLOT_4 :
			{
				SelectInvenWeaponClear();
				m_pFrameSelect->Show(LTFALSE);
				SelectInvenCharClear();
				m_pFrmSelectCharSlot->Show(LTFALSE);

				int nIndex = g_nShopCurSubStep[g_nShopCurWpSubTab] + dwParam1;
				if( g_nOldShopSelectWeapon != nIndex )
				{
					g_nOldShopSelectWeapon = nIndex;
					SetShopSelectedWpTexPos( g_nShopCurWpSubTab, nIndex, dwParam1 );
				}
			}break;

		case CMD_BTN_SHOP_W_SLOT_DBL_0 :
		case CMD_BTN_SHOP_W_SLOT_DBL_1 :
		case CMD_BTN_SHOP_W_SLOT_DBL_2 :
		case CMD_BTN_SHOP_W_SLOT_DBL_3 :
		case CMD_BTN_SHOP_W_SLOT_DBL_4 :
			{
				if( m_pCheckShopWeapon->GetCheck() )
				{
					BuyWeaponInfo();					
				}
				else if( m_pCheckShopChar->GetCheck() )
				{
					BuyCharInfo();					
				}
				else
				{
				}
			}break;

			case CMD_BTN_SHOP_C_SLOT_0 :
			case CMD_BTN_SHOP_C_SLOT_1 :
			case CMD_BTN_SHOP_C_SLOT_2 :
			case CMD_BTN_SHOP_C_SLOT_3 :
				{
					SelectInvenWeaponClear();
					m_pFrameSelect->Show(LTFALSE);
					SelectInvenCharClear();
					m_pFrmSelectCharSlot->Show(LTFALSE);

					int nIndex = g_nShopCharCurSubStep[g_nShopCharCurSubTab] + dwParam1;
					SetShopSelectedChTexPos( g_nShopCharCurSubTab, nIndex, dwParam1 );
				}break;
			case CMD_BTN_SHOP_C_SLOT_DBL_0 :
			case CMD_BTN_SHOP_C_SLOT_DBL_1 :
			case CMD_BTN_SHOP_C_SLOT_DBL_2 :
			case CMD_BTN_SHOP_C_SLOT_DBL_3 :
				{
					if( m_pCheckShopWeapon->GetCheck() )
					{
						BuyWeaponInfo();					
					}
					else if( m_pCheckShopChar->GetCheck() )
					{
						BuyCharInfo();					
					}
					else
					{
					}
				}break;

		case CMD_CHECK_SHOP_WEAPON :
			{
				SelectInvenWeaponClear();
				m_pFrameSelect->Show(LTFALSE);
				SelectInvenCharClear();
				m_pFrmSelectCharSlot->Show(LTFALSE);

				if( !m_pCheckShopWeapon->GetCheck() )
				{
					m_pFrameDetail->Show(LTTRUE);
					m_pFrmDetailBack->Show(LTFALSE);
					DetailWeaponClear();
					DetailCharClear();

					SelectShopCharClear();

					m_pBtnShopScroll->SetPosYRatio( 0.0f );
					g_nShopCurSubStep[0] = 0;
					this->ShowShopWeapon(LTTRUE);
				}
			}break;
		case CMD_CHECK_SHOP_CHAR :
			{
				SelectInvenWeaponClear();
				m_pFrameSelect->Show(LTFALSE);
				SelectInvenCharClear();
				m_pFrmSelectCharSlot->Show(LTFALSE);

				if( !m_pCheckShopChar->GetCheck() )
				{
					m_pFrameDetail->Show(LTFALSE);
					m_pFrmDetailBack->Show(LTTRUE);
					DetailWeaponClear();
					DetailCharClear();

					SelectShopWeaponClear();

					m_pBtnShopScroll->SetPosYRatio( 0.0f );
					g_nShopCurSubStep[0] = 0;
					this->ShowShopCharacter(LTTRUE);
				}
			}break;
		case CMD_CHECK_SHOP_ETC :
			{
				SelectInvenWeaponClear();
				m_pFrameSelect->Show(LTFALSE);
				SelectInvenCharClear();
				m_pFrmSelectCharSlot->Show(LTFALSE);
				
				if( !m_pCheckShopEtc->GetCheck() )
				{
					m_pFrameDetail->Show(LTFALSE);
					m_pFrmDetailBack->Show(LTFALSE);					
					DetailWeaponClear();
					DetailCharClear();
					for(int i = 0; i < 4; i++ )
					{
						m_pFrameGaugeBar[i]->Show(LTFALSE);
					}
					
					SelectShopWeaponClear();
					SelectShopCharClear();

					m_pBtnShopScroll->SetPosYRatio( 0.0f );
					g_nShopCurSubStep[0] = 0;
					this->ShowShopEtc(LTTRUE);
				}
			}break;
		case CMD_CHECK_SHOP_W_ALL :
		case CMD_CHECK_SHOP_W_FIRST :
		case CMD_CHECK_SHOP_W_SECOND :
		case CMD_CHECK_SHOP_W_THIRD :
		case CMD_CHECK_SHOP_W_FOUR :
			{
				SelectInvenWeaponClear();
				m_pFrameSelect->Show(LTFALSE);
				SelectInvenCharClear();
				m_pFrmSelectCharSlot->Show(LTFALSE);

				if( GetShopSubMenuWeaponOrder() != dwParam1 )
				{
					DetailWeaponClear();

					SelectShopWeaponClear();

					g_nOldShopSelectWeapon = -1;
					m_pBtnShopScroll->SetPosYRatio( 0.0f );
					g_nShopCurSubStep[dwParam1] = 0;
					this->ShopSubMenuWeaponOrder(dwParam1);
					this->ShowShopSubWeapon(dwParam1);
				}
			}break;

		case CMD_SHOP_SUB_CHAR_ALL :
		case CMD_SHOP_SUB_CHAR_CHAR :
		case CMD_SHOP_SUB_CHAR_ACC :
			{
				SelectInvenWeaponClear();
				m_pFrameSelect->Show(LTFALSE);
				SelectInvenCharClear();
				m_pFrmSelectCharSlot->Show(LTFALSE);

				if( GetShopSubMenuCharOrder() != dwParam1 )
				{
					DetailCharClear();

					SelectShopCharClear();

					m_pBtnShopScroll->SetPosYRatio( 0.0f );
					g_nShopCharCurSubStep[dwParam1] = 0;
					this->ShopSubMenuCharOrder(dwParam1);
					this->ShowShopSubChar(dwParam1);
				}
			}break;
			// 여기까지 Shop

		case CMD_CHECK_INVEN :
		case CMD_CHECK_PRESENT :
			break;
		case CMD_CHECK_M_WEAPON :
			{
				SelectShopWeaponClear();
				m_pFrmShopSltWeapon->Show(LTFALSE);
				SelectShopCharClear();
				m_pFrmShopSltChar->Show(LTFALSE);

				if( !m_pCheckMWeapon->GetCheck() )
				{
					DetailWeaponClear();
					DetailCharClear();
				}
				
				SelectInvenCharClear();
				this->ShowWeapon(LTTRUE);
			}break;
		case CMD_CHECK_M_CHAR :
			{				
				SelectShopWeaponClear();
				m_pFrmShopSltWeapon->Show(LTFALSE);
				SelectShopCharClear();
				m_pFrmShopSltChar->Show(LTFALSE);

				if( !m_pCheckMChar->GetCheck() )
				{
					DetailWeaponClear();
					DetailCharClear();
				}
				
				SelectInvenWeaponClear();
				this->ShowCharacter(LTTRUE);
			}break;
		case CMD_CHECK_M_ETC :
			{
				SelectShopWeaponClear();
				m_pFrmShopSltWeapon->Show(LTFALSE);
				SelectShopCharClear();
				m_pFrmShopSltChar->Show(LTFALSE);

				if( !m_pCheckMEtc->GetCheck() )
				{
					DetailWeaponClear();
					DetailCharClear();
				}
					
				
				SelectInvenCharClear();
				SelectInvenWeaponClear();
				this->ShowEtc(LTTRUE);
			}break;

		case CMD_BTN_S_ALL :
		case CMD_BTN_S_FIRST :
		case CMD_BTN_S_SECOND :
		case CMD_BTN_S_THIRD :
		case CMD_BTN_S_FOUR :
			{
				SelectShopWeaponClear();
				m_pFrmShopSltWeapon->Show(LTFALSE);
				SelectShopCharClear();
				m_pFrmShopSltChar->Show(LTFALSE);
				
				if( GetSubMenuWeaponOrder() != dwParam1 )
				{
					DetailWeaponClear();

					SelectInvenWeaponClear();

					g_nOldSelectWeapon = -1;
					m_pBtnScroll->SetPosYRatio( 0.0f );
					g_nCurSubStep[dwParam1] = 0;
					SubMenuWeaponOrder(dwParam1);
					ShowSubWeapon(dwParam1);
				}
				
			}break;

		case CMD_SCROLL_INVEN :
			{
				if( dwParam1 == CMD_PARAM_MOUSEMOVE )
				{
					// 마우스로 잡고 scroll bar 움직였을 때
					if( m_pCheckMWeapon->GetCheck() )
					{
						float tempPos = m_pBtnScroll->GetPosYRatio();
						int tempStep = (int)(tempPos/g_fSubRatio[g_nCurSubTab]);
						if( tempStep != g_nCurSubStep[g_nCurSubTab] )
						{
							g_nCurSubStep[g_nCurSubTab] = tempStep;
							
							ShowSubWeapon(g_nCurSubTab);
						}
					}
					else if( m_pCheckMChar->GetCheck() )
					{
						float tempPos = m_pBtnScroll->GetPosYRatio();
						int tempStep = (int)(tempPos/g_fCharSubRatio[g_nCharCurSubTab]);
						if( tempStep != g_nCharCurSubStep[g_nCharCurSubTab] )
						{
							g_nCharCurSubStep[g_nCharCurSubTab] = tempStep;
							
							ShowSubChar(g_nCharCurSubTab);
						}
					}
				}
			}break;			
		case CMD_SCROLL_UP_INVEN :
			{
				OnScrollUp();
			}break;
		case CMD_SCROLL_DOWN_INVEN :
			{
				OnScrollDown();
			}break;
			
		case CMD_INVEN_CHECK_RED :
			if( m_pBtnCheckRed && !m_pBtnCheckRed->GetCheck())
			{
				m_pBtnCheckRed->SetCheck();
				m_pBtnCheckBlue->SetCheck(LTFALSE);
				UpdateModel();
			}
			break;
		case CMD_INVEN_CHECK_BLUE :
			if( m_pBtnCheckBlue && !m_pBtnCheckBlue->GetCheck())
			{
				m_pBtnCheckBlue->SetCheck();
				m_pBtnCheckRed->SetCheck(LTFALSE);
				UpdateModel();
			}
			break;
		case CMD_BTN_INVEN_WEAPON_0 :
		case CMD_BTN_INVEN_WEAPON_1 :
		case CMD_BTN_INVEN_WEAPON_2 :
		case CMD_BTN_INVEN_WEAPON_3 :
		case CMD_BTN_INVEN_WEAPON_4 :
		case CMD_BTN_INVEN_WEAPON_5 :
		case CMD_BTN_INVEN_WEAPON_6 :
		case CMD_BTN_INVEN_WEAPON_7 :
		case CMD_BTN_INVEN_WEAPON_8 :
			{
				SelectShopWeaponClear();
				m_pFrmShopSltWeapon->Show(LTFALSE);
				SelectShopCharClear();
				m_pFrmShopSltChar->Show(LTFALSE);
				
				int step = g_nCurSubStep[g_nCurSubTab]*3 + dwParam1;
				if( g_nOldSelectWeapon != step )
				{
					g_nOldSelectWeapon = step;
					SetSelectedTexturePos(g_nCurSubTab, step, dwParam1 );
				}
			}break;
		case CMD_BTN_INVEN_WEAPON_DBL_0 :
		case CMD_BTN_INVEN_WEAPON_DBL_1 :
		case CMD_BTN_INVEN_WEAPON_DBL_2 :
		case CMD_BTN_INVEN_WEAPON_DBL_3 :
		case CMD_BTN_INVEN_WEAPON_DBL_4 :
		case CMD_BTN_INVEN_WEAPON_DBL_5 :
		case CMD_BTN_INVEN_WEAPON_DBL_6 :
		case CMD_BTN_INVEN_WEAPON_DBL_7 :
		case CMD_BTN_INVEN_WEAPON_DBL_8 :
			{
				if( m_pCheckMWeapon->GetCheck() )
				{
					sInven_Set* pWeapon = GetWeapon( g_nSelectWeapon );
					if( pWeapon && pWeapon->bUsed == false )
					{
						g_Network.CS_AddEquipment( pWeapon->nClassNum+ITEM_IDX_CHAR_B,
												   pWeapon->szClass1[0],
												   pWeapon->szClass2[0], pWeapon->szClass2[1],
												   pWeapon->nWeaponId );
					}
				}
				else if( m_pCheckMChar->GetCheck() )
				{
					sChar_Set* pCharacter = GetCharTab( g_nSelectItem, ITEM_CHAR_SLOT );
					if( pCharacter && pCharacter->bUsed == false )
					{
						g_Network.CS_AddEquipment( pCharacter->nTeamId,
												   pCharacter->szClass1[0],
												   pCharacter->szClass2[0], pCharacter->szClass2[1],
												   pCharacter->nItemId );
					}
				}
			}break;

		case CMD_BTN_INVEN_CHAR_0 :
		case CMD_BTN_INVEN_CHAR_1 :
		case CMD_BTN_INVEN_CHAR_2 :
		case CMD_BTN_INVEN_CHAR_3 :
		case CMD_BTN_INVEN_CHAR_4 :
		case CMD_BTN_INVEN_CHAR_5 :
			{
				SelectShopWeaponClear();
				m_pFrmShopSltWeapon->Show(LTFALSE);
				SelectShopCharClear();
				m_pFrmShopSltChar->Show(LTFALSE);
				
				int nIndex = g_nCharCurSubStep[g_nCharCurSubTab]*3 + dwParam1;
				SetCharSelectedTexPos( g_nCharCurSubTab, nIndex, dwParam1 );
			}break;

		case CMD_BTN_INVEN_CHAR_DBL_0 :
		case CMD_BTN_INVEN_CHAR_DBL_1 :
		case CMD_BTN_INVEN_CHAR_DBL_2 :
		case CMD_BTN_INVEN_CHAR_DBL_3 :
		case CMD_BTN_INVEN_CHAR_DBL_4 :
		case CMD_BTN_INVEN_CHAR_DBL_5 :
			{
				if( m_pCheckMWeapon->GetCheck() )
				{
					sInven_Set* pWeapon = GetWeapon( g_nSelectWeapon );
					if( pWeapon && pWeapon->bUsed == false )
					{
						g_Network.CS_AddEquipment( pWeapon->nClassNum+ITEM_IDX_CHAR_B,
												   pWeapon->szClass1[0],
												   pWeapon->szClass2[0], pWeapon->szClass2[1],
												   pWeapon->nWeaponId );
					}
				}
				else if( m_pCheckMChar->GetCheck() )
				{
					sChar_Set* pCharacter = GetCharTab( g_nSelectItem, ITEM_CHAR_SLOT );
					if( pCharacter && pCharacter->bUsed == false )
					{
						g_Network.CS_AddEquipment( pCharacter->nTeamId,
												   pCharacter->szClass1[0],
												   pCharacter->szClass2[0], pCharacter->szClass2[1],
												   pCharacter->nItemId );
					}
				}
			}break;

		case CMD_BTN_USE :
			{	
				if( m_pCheckMWeapon->GetCheck() )
				{
					sInven_Set* pWeapon = GetWeapon( g_nSelectWeapon );
					if( pWeapon && pWeapon->bUsed == false )
					{
						g_Network.CS_AddEquipment( pWeapon->nClassNum+ITEM_IDX_CHAR_B,
												   pWeapon->szClass1[0],
												   pWeapon->szClass2[0], pWeapon->szClass2[1],
												   pWeapon->nWeaponId );
					}
				}
				else if( m_pCheckMChar->GetCheck() )
				{
					sChar_Set* pCharacter = GetCharTab( g_nSelectItem, ITEM_CHAR_SLOT );
					if( pCharacter && pCharacter->bUsed == false )
					{
						g_Network.CS_AddEquipment( pCharacter->nTeamId,
												   pCharacter->szClass1[0],
												   pCharacter->szClass2[0], pCharacter->szClass2[1],
												   pCharacter->nItemId );
					}
				}
			}break;
		case CMD_BTN_THROW :
			{
				if( m_pCheckMWeapon->GetCheck() )
				{
					sInven_Set* pWeapon = GetWeapon(g_nSelectWeapon);
					if( pWeapon )
					{
						if( pWeapon->bDefaultItem )
						{
							g_pInterfaceMgr->ShowMessageBox( SA_TEXT_THROW_WEAPON_ERROR,
															 LTMB_OK, LTNULL, LTNULL );
						}
						else
						{
							char szTemp[256];
							sprintf( szTemp, NF_GetMsg(SA_TEXT_THROW_WEAPON), pWeapon->szName );
							g_pInterfaceMgr->ShowMessageBox( szTemp, LTMB_YESNO, DiscardWeaponCallback, (void*)pWeapon );
						}
					}
				}
				else if( m_pCheckMChar->GetCheck() )
				{
					sChar_Set* pCharacter = GetCharTab(g_nSelectItem, 1);
					if( pCharacter )
					{
						if( pCharacter->bDefaultItem )
						{
							g_pInterfaceMgr->ShowMessageBox( SA_TEXT_THROW_CHAR_ERROR,
															 LTMB_OK, LTNULL, LTNULL );
						}
						else
						{
							char szTemp[256];
							sprintf( szTemp, NF_GetMsg(SA_TEXT_THROW_CHAR), pCharacter->szName );
							g_pInterfaceMgr->ShowMessageBox( szTemp, LTMB_YESNO, DiscardCharacterCallback, (void*)pCharacter );
						}						
					}
				}				
			}break;

		case CMD_SUB_CHAR_ALL :
		case CMD_SUB_CHAR_CHAR :
		case CMD_SUB_CHAR_ACC :
			{
				if( m_pCheckMChar->GetCheck() )
				{
					DetailCharClear();
					
					SelectInvenCharClear();

					m_pBtnScroll->SetPosYRatio( 0.0f );
					g_nShopCharCurSubStep[dwParam1] = 0;
					ShowSubChar(dwParam1);
				}
				
			}break;
	}

	return 1;
}

LTBOOL CScreenInventory::Render(HSURFACE hDestSurf)
{
	if( m_pCheckMWeapon->GetCheck() && g_nSelectWeapon >= 0 )
	{
		UpdateGaugeAni();
	}

	if( m_pCheckShopWeapon->GetCheck() && g_nShopSelectWeapon >= 0 )
	{
		UpdateGaugeAni();
	}

	SetRenderState();

	RenderCloud();
	
	if( LTFALSE == CScreenBase::Render(hDestSurf) ) return LTFALSE;
	
	m_pTextScroll->Update();
	m_pTextScroll->Render();

	g_pDrawPrim->SetTexture( m_hUIBottomL );
	g_pDrawPrim->DrawPrim( &m_PolyUIBottomL );
	g_pDrawPrim->SetTexture( m_hUIBottomR );
	g_pDrawPrim->DrawPrim( &m_PolyUIBottomR );

	m_pChannelInfo->Render();
	
	if( m_pCheckMWeapon->GetCheck() )
	{
		RenderSubWeaponTitle();
	}

	if( m_pFrameDetail->IsVisible() )
	{

		m_pFontWeaponName->Render();
		m_pFontWeaponDur->Render();

		//if( g_nSelectWeapon >= 0 )
		{
			RenderGaugeText();
		}
	}

	//if( m_pCheckMChar->GetCheck() )
	if(m_pFrmDetailBack->IsVisible() )
	{
		RenderSubCharTitle();
	}

	if( !m_IsShop )
	{
		RenderUserInfo();
	}
	else
	{
		if( m_pCheckShopWeapon->GetCheck() )
		{
			RenderShopWpPrice();
		}
		else if( m_pCheckShopChar->GetCheck() )
		{
			RenderShopChPrice();
		}
	}
	m_pInfoGameMoney->Render();
	
	if( m_pCheckMChar->GetCheck() )
	{
		for( int i = 0; i < 6; i++ )
		{
			m_pFontCharName[i]->Render();
		}
	}
	
	if( m_pPopupItemBuy )
	{
		m_pPopupItemBuy->Render();
	}

	
	//[SPIKE] 강제퇴장 
	CScreenMgr* pScreenMgr = g_pInterfaceMgr->GetScreenMgr();			
	float fCurTime = g_pLTClient->GetTime();
	if( (fCurTime - g_fClickTime) > GAMESTART_TIME  && (pScreenMgr /*&& SCREEN_ID_MAIN != pScreenMgr->GetPreviousScreenID()*/) )//&& g_GameDoc.IsCaptain() )
	{	
		g_Network.CS_LeaveInven();
	}
	//================================================
	
	return LTTRUE;
}

uint8 CScreenInventory::GetCurrentUsedModel(uint8 nTeamId)
{
	CharInvenList::iterator iter = m_Chars.begin();
	while(iter != m_Chars.end())
	{
		if( (*iter)->nTeamId == nTeamId && (*iter)->bUsed ) break;

		iter++;
	}

	if( iter != m_Chars.end() )
	{
		return (*iter)->nItemId;
	}
	
	return 255;
}

void CScreenInventory::UpdateModel(bool bUsedButton)
{
	if( m_IsShop ) return;
	// Update Model
	uint8 nType = 1;
	uint8 nModelId = 255;
	if( bUsedButton )
	{
		nModelId = g_nSelectItem;
		uint8 ntempId = GetCurrentUsedModel(TEAM_A);
		if( nModelId == ntempId )
		{
			m_pBtnCheckRed->SetCheck(LTTRUE);
			m_pBtnCheckBlue->SetCheck(LTFALSE);
		}
		else
		{
			m_pBtnCheckRed->SetCheck(LTFALSE);
			m_pBtnCheckBlue->SetCheck(LTTRUE);
		}

	}
	else
	{
		if( m_pBtnCheckRed->GetCheck() )
		{
			nModelId = GetCurrentUsedModel(TEAM_A);		
		}
		else
		{
			nModelId = GetCurrentUsedModel(TEAM_B);
		}
	}
	
	HOBJECT hChar = m_CharacterSFX[0].GetObject();
	if( hChar )
	{
		ModelId id;
		id = ModelId(nModelId);

		const char* szFileName = g_pModelButeMgr->GetModelFilename(id);
		if( !szFileName ) return;
		
		g_pLTClient->Common()->SetObjectFlags(hChar, OFT_Flags, FLAG_VISIBLE, FLAG_VISIBLE );	

		ObjectCreateStruct createStruct;
		INIT_OBJECTCREATESTRUCT(createStruct);

		//SAFE_STRCPY(createStruct.m_Filename, g_pModelButeMgr->GetModelFilename(id));
		SAFE_STRCPY(createStruct.m_Filename, szFileName);
		if( g_pModelButeMgr->GetSkinReader(id) )
		{
			g_pModelButeMgr->GetSkinReader(id)->CopyList(0, createStruct.m_SkinNames[0], MAX_CS_FILENAME_LEN+1);
		}

		//---------------------------------------------------------------------------
		char* szTemp;
		//g_pModelButeMgr->GetModelHeadSkinFilename(id, m_nCurHeadSkinId[nType], szTemp);	
		g_pModelButeMgr->GetModelHeadSkinFilename(id, 0, szTemp);	
		if( szTemp ) strcpy( createStruct.m_SkinNames[0], szTemp );
		//g_pModelButeMgr->GetModelBodySkinFilename(id, m_nCurBodySkinId[nType], szTemp);
		g_pModelButeMgr->GetModelBodySkinFilename(id, 0, szTemp);
		if( szTemp ) strcpy( createStruct.m_SkinNames[1], szTemp );
		//---------------------------------------------------------------------------

		g_pModelButeMgr->CopyRenderStyleFilenames( id, &createStruct );

		//g_pLTClient->SetModelAnimation(hChar, 0);
		g_pCommonLT->SetObjectFilenames(hChar, &createStruct);

		uint32 dwAni = g_pLTClient->GetAnimIndex(hChar, "IF_SELECT");
		if( dwAni != INVALID_ANI )
		{
			g_pLTClient->SetModelAnimation(hChar, dwAni);
		}
	
		ClearAttachFX();

		/*
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

void CScreenInventory::ShowArsenalTab(LTBOOL bShow)
{
	/*

	for( int i = 0; i < 6; i++ )
	{
		m_pFrameItem[i]->Show(bShow);
		m_pBtnUseItem[i]->Show(bShow);
		m_pBtnDetailItem[i]->Show(bShow);

		m_pFrameItemTitle[i]->Show(bShow);
	}

	UpdateInvView( 0.0f, bShow );
	*/
}

void CScreenInventory::RenderUserInfo()
{
	m_pInfoNickCap->Render();
	m_pInfoNick->Render();
	m_pInfoGrade->Render();
	m_pInfoWinPercent->Render();
	m_pInfoKillDeath->Render();
	m_pInfoExp->Render();
	//m_pInfoGameMoney->Render();
	m_pInfoGradeCap->Render();
	m_pInfoWinPercentCap->Render();
	m_pInfoKillDeathCap->Render();
	m_pInfoExpCap->Render();

	if( m_hGradeIcon )
	{
		g_pDrawPrim->SetTexture( m_hGradeIcon );
		g_pDrawPrim->DrawPrim( &m_PolyGrade );
	}
}

void CScreenInventory::SetUserInfo( uint32 nGrade, 
								   const char* lpszNick,
								   uint32 nKill,
								   uint32 nDeath, 
								   uint32 nWin, uint32 nLose, uint32 nDraw,
								   __int64 nExp,
								   __int64 nGameMoney,
								   __int64 nCash )
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
	//m_pInfoGameMoney->DrawD3DText( szTemp, strlen(szTemp), DT_RIGHT, D3D_TEXT_NORMAL, false );
	m_pInfoGameMoney->DrawD3DText( szTemp, strlen(szTemp), DT_LEFT, D3D_TEXT_NORMAL, false );

	m_hGradeIcon	= g_Global_Data.GetGradeTexture(nGrade);
	SetupQuadUVs( m_PolyGrade, m_hGradeIcon, 0.0f, 0.0f, 1.0f, 1.0f );
}

LTBOOL CScreenInventory::OnWheelUp(int x, int y)
{
	if( m_IsShop )
	{
		if( x >= 19 && x < 388 && y >= 155 && y < 524 )
		{
			return OnShopScrollUp( x, y );
		}
	}
	

	if( x >= 418 && x <781 && y >= 100 && y < 344 )
	{
		return OnScrollUp( x, y );
	}

	return LTTRUE;
}

LTBOOL CScreenInventory::OnWheelDown(int x, int y)
{
	if( m_IsShop )
	{
		if( x >= 19 && x < 388 && y >= 155 && y < 524 )
		{
			return OnShopScrollDown( x, y );
		}
	}

	if( x >= 418 && x <781 && y >= 100 && y < 344 )
	{
		return OnScrollDown( x, y );
	}
	return LTTRUE;
}

LTBOOL CScreenInventory::OnShopScrollUp( int x, int y )
{
	if( m_pCheckShopWeapon->GetCheck() )
	{
		g_nShopCurSubStep[g_nShopCurWpSubTab]--;
		if( g_nShopCurSubStep[g_nShopCurWpSubTab] < 0 )
		{
			g_nShopCurSubStep[g_nShopCurWpSubTab] = 0;
		}

		float tempPos = g_nShopCurSubStep[g_nShopCurWpSubTab] * g_fShopSubRatio[g_nShopCurWpSubTab];
		m_pBtnShopScroll->SetPosYRatio( tempPos );

		ShowShopSubWeapon(g_nShopCurWpSubTab);
		
	}
	else if( m_pCheckShopChar->GetCheck() )
	{
		g_nShopCharCurSubStep[g_nShopCharCurSubTab]--;
		if( g_nShopCharCurSubStep[g_nShopCharCurSubTab] < 0 )
		{
			g_nShopCharCurSubStep[g_nShopCharCurSubTab] = 0;
		}

		float tempPos = g_nShopCharCurSubStep[g_nShopCharCurSubTab] * g_fShopCharSubRatio[g_nShopCharCurSubTab];
		
		m_pBtnShopScroll->SetPosYRatio( tempPos );

		ShowShopSubChar(g_nShopCharCurSubTab);
	}
	return LTTRUE;
}

LTBOOL CScreenInventory::OnShopScrollDown( int x, int y )
{
	if( m_pCheckShopWeapon->GetCheck() )
	{
		g_nShopCurSubStep[g_nShopCurWpSubTab]++;
		if( g_nShopCurSubStep[g_nShopCurWpSubTab] > g_nShopSubStep[g_nShopCurWpSubTab] )
		{
			g_nShopCurSubStep[g_nShopCurWpSubTab] = g_nShopSubStep[g_nShopCurWpSubTab];
		}

		float tempPos = g_nShopCurSubStep[g_nShopCurWpSubTab] * g_fShopSubRatio[g_nShopCurWpSubTab];
		m_pBtnShopScroll->SetPosYRatio( tempPos );

		ShowShopSubWeapon(g_nShopCurWpSubTab);
	}
	else if( m_pCheckShopChar->GetCheck() )
	{
		g_nShopCharCurSubStep[g_nShopCharCurSubTab]++;
		if( g_nShopCharCurSubStep[g_nShopCharCurSubTab] > g_nShopCharSubStep[g_nShopCharCurSubTab] )
		{
			g_nShopCharCurSubStep[g_nShopCharCurSubTab] = g_nShopCharSubStep[g_nShopCharCurSubTab];
		}

		float tempPos = g_nShopCharCurSubStep[g_nShopCharCurSubTab] * g_fShopCharSubRatio[g_nShopCharCurSubTab];
		
		m_pBtnShopScroll->SetPosYRatio( tempPos );

		ShowShopSubChar(g_nShopCharCurSubTab);
	}
	return LTTRUE;
}

LTBOOL CScreenInventory::OnScrollUp( int x, int y )
{
	if( m_pCheckMWeapon->GetCheck() )
	{
		g_nCurSubStep[g_nCurSubTab]--;
		if( g_nCurSubStep[g_nCurSubTab] < 0 )
		{
			g_nCurSubStep[g_nCurSubTab] = 0;
		}

		float tempPos = g_nCurSubStep[g_nCurSubTab] * g_fSubRatio[g_nCurSubTab];

		m_pBtnScroll->SetPosYRatio( tempPos );

		ShowSubWeapon(g_nCurSubTab);
	}
	else if( m_pCheckMChar->GetCheck() )
	{
		g_nCharCurSubStep[g_nCharCurSubTab]--;
		if( g_nCharCurSubStep[g_nCharCurSubTab] < 0 )
		{
			g_nCharCurSubStep[g_nCharCurSubTab] = 0;
		}
		
		float tempPos = g_nCharCurSubStep[g_nCharCurSubTab] * g_fCharSubRatio[g_nCharCurSubTab];

		m_pBtnScroll->SetPosYRatio( tempPos );

		ShowSubChar( g_nCharCurSubTab );
	}

	return LTTRUE;
}

LTBOOL CScreenInventory::OnScrollDown( int x, int y )
{
	if( m_pCheckMWeapon->GetCheck() )
	{
		g_nCurSubStep[g_nCurSubTab]++;
		if( g_nCurSubStep[g_nCurSubTab] > g_nSubStep[g_nCurSubTab] ) 
		{
			g_nCurSubStep[g_nCurSubTab] = g_nSubStep[g_nCurSubTab];
		}

		float tempPos = g_nCurSubStep[g_nCurSubTab] * g_fSubRatio[g_nCurSubTab];

		m_pBtnScroll->SetPosYRatio( tempPos );

		ShowSubWeapon(g_nCurSubTab);
	}
	else if( m_pCheckMChar->GetCheck() )
	{
		g_nCharCurSubStep[g_nCharCurSubTab]++;
		if( g_nCharCurSubStep[g_nCharCurSubTab] > g_nCharSubStep[g_nCharCurSubTab] )
		{
			g_nCharCurSubStep[g_nCharCurSubTab] = g_nCharSubStep[g_nCharCurSubTab];
		}
		
		float tempPos = g_nCharCurSubStep[g_nCharCurSubTab] * g_fCharSubRatio[g_nCharCurSubTab];

		m_pBtnScroll->SetPosYRatio( tempPos );

		ShowSubChar( g_nCharCurSubTab );
	}

	return LTTRUE;
}

LTBOOL CScreenInventory::OnEnter()
{
	return LTFALSE;
}

LTBOOL CScreenInventory::OnEscape()
{
	OnCommand( CMD_INVEN_EXIT, LTNULL, LTNULL );
	return LTTRUE;
}

LTBOOL CScreenInventory::UpdateInvView( LTFLOAT fStartYPos, LTBOOL bShow )
{	
	return LTTRUE;
}

void CScreenInventory::ShowWeapon(LTBOOL bShow)
{
	m_pCheckMWeapon->SetCheck(bShow);
	m_pCheckMChar->SetCheck(!bShow);
	m_pCheckMEtc->SetCheck(!bShow);
	
	SubMenuWeapon(bShow);
	SubMenuCharacter(!bShow);
	SubMenuEtc(!bShow);
}

void CScreenInventory::ShowCharacter(LTBOOL bShow)
{
	m_pCheckMWeapon->SetCheck(!bShow);
	m_pCheckMChar->SetCheck(bShow);
	m_pCheckMEtc->SetCheck(!bShow);

	SubMenuWeapon(!bShow);
	SubMenuCharacter(bShow);
	SubMenuEtc(!bShow);
}

void CScreenInventory::ShowEtc(LTBOOL bShow)
{
	m_pCheckMWeapon->SetCheck(!bShow);
	m_pCheckMChar->SetCheck(!bShow);
	m_pCheckMEtc->SetCheck(bShow);
	
	SubMenuWeapon(!bShow);
	SubMenuCharacter(!bShow);
	SubMenuEtc(bShow);
}

void CScreenInventory::SubMenuWeapon(LTBOOL bShow)
{
	for( int n = 0; n < 5; n++ )
	{
		m_pCheckSubWeapon[n]->Show(bShow);
	}

	if( bShow )
	{
		this->SubMenuWeaponOrder(0);
		this->ShowSubWeapon(0);
	}
	else
	{
		for( int i = 0; i < 9; i++ )
		{
			m_pBtnInvenSlot[i]->Show(bShow);
			m_pFrameInvenSlot[i]->Show(bShow);
			
		}
		for( i = 0; i < 4; i++ )
		{
			m_pFrameInvenUsed[i]->Show(bShow);
		}		
		m_pFrameSelect->Show(bShow);

		// gauge 사운드 끄기
		if( m_hGaugeSound )
		{
			g_pLTClient->SoundMgr()->KillSound( m_hGaugeSound );
			m_hGaugeSound	= LTNULL;
		}
	}

	m_pFrameDetail->Show(bShow);

	m_pFrameWeaponDetail->Show(bShow);
	// Gauge	
	for( int i = 0; i < 4; i++ )
	{
		m_pFrameGaugeBar[i]->Show(bShow);

		if( g_nSelectWeapon >= 0)
			m_pFrameWeaponInfo[i]->Show(bShow);
	}
}

void CScreenInventory::SubMenuCharacter(LTBOOL bShow)
{
	// button
	m_pCheckCharAll->Show(bShow);
	m_pCheckCharChar->Show(bShow);
	m_pCheckCharAccessory->Show(bShow);

	if( bShow )
	{
		this->ShowSubChar(0);
	}
	else
	{
		for( int i = 0; i < 6; i++)
		{
			m_pBtnCharSlot[i]->Show(bShow);
			m_pFrmCharSlot[i]->Show(bShow);
		}
		m_pFrmSelectCharSlot->Show(bShow);
		for( i = 0; i < 3; i++ )
		{
			m_pFrmUsedCharIcon[i]->Show(bShow);
		}
	}
	
	m_pFrmSelectCharDetail->Show(bShow);
	m_pFrmDetailBack->Show(bShow);	
}
void CScreenInventory::SubMenuEtc(LTBOOL bShow)
{
}

sInven_Set*	CScreenInventory::GetWeapon(uint8 nWeaponId)
{
	int nCount = 0;
	int nSize = m_Invens.size();
	WeaponInvenList::iterator iter = m_Invens.begin();
	while( iter != m_Invens.end() )
	{
		if( (*iter)->nWeaponId == nWeaponId )
		{
			break;
		}
		iter++;
		nCount++;
	}

	if( nCount >= nSize )
	{
		return LTNULL;
	}

	return m_Invens[nCount];
}
LTBOOL CScreenInventory::AddWeapon( sInven_Set* pItem )
{
	if( pItem->nClassNum == WMGR_INVALID_ID ) 
	{
		ASSERT(!"pItem->nClassNum ==255");
		return LTFALSE;
	}

	WeaponInvenList::iterator iter = m_Invens.begin();
	while( iter != m_Invens.end() && (*iter)->nWeaponId != pItem->nWeaponId )
	{
		iter++;
	}

	WEAPON const *pWeapon = g_pWeaponMgr->GetWeapon(pItem->nWeaponId);
	if( !pWeapon ) return LTFALSE;
	AMMO const*pAmmo = g_pWeaponMgr->GetAmmo(pWeapon->nDefaultAmmoId);
	if( !pAmmo ) return LTFALSE;

	pItem->bSelected		= false;
	pItem->bUsed			= false;
	pItem->nClipInAmmo		= pWeapon->nShotsPerClip;
	pItem->nSelectAmmo		= pAmmo->nSelectionAmount;
	pItem->rDetail			= g_pWeaponMgr->GetWeaponPerformance(pItem->nWeaponId);
	pItem->szName			= NF_GetMsg( (DWORD)g_pWeaponMgr->GetWeaponIconNameIndex(pItem->nWeaponId) );
	pItem->szDescription	= NF_GetMsg( (DWORD)g_pWeaponMgr->GetWeaponDescription(pItem->nWeaponId) );
	pItem->hWeaponTx		= g_pInterfaceResMgr->GetTexture( pWeapon->GetInvenNormalIcon().c_str() );
	pItem->hSelectTx		= g_pInterfaceResMgr->GetTexture( pWeapon->GetInvenSelectIcon().c_str() );


	if( iter == m_Invens.end() )
	{
		m_Invens.push_back(pItem);
	}
	else
	{
		//기존에 있다는것은 기간연장의 경우
		strcpy( (*iter)->szExpireTime, pItem->szExpireTime );
		delete pItem;
	}
	return LTTRUE;
}

LTBOOL CScreenInventory::AddWeapon(uint8 nWeaponId, int nDuration)
{
	int nCount = 0;
	WeaponInvenList::iterator iter = m_Invens.begin();
	while(iter != m_Invens.end() && (*iter)->nWeaponId != nWeaponId)
	{
		iter++;
		nCount++;
	}

	sInven_Set* pInven = LTNULL;
	if( iter == m_Invens.end() )
	{
		WEAPON const *pWeapon = g_pWeaponMgr->GetWeapon(nWeaponId);
		if( !pWeapon ) return LTFALSE;
		AMMO const*pAmmo = g_pWeaponMgr->GetAmmo(pWeapon->nDefaultAmmoId);
		if( !pAmmo ) return LTFALSE;

		std::string strName;
		
		pInven = debug_new(sInven_Set);
		
		pInven->nWeaponId		= nWeaponId;
		pInven->nClipInAmmo		= pWeapon->nShotsPerClip;
		pInven->nSelectAmmo		= pAmmo->nSelectionAmount;
		pInven->bSelected		= false;
		pInven->bUsed			= false;
		pInven->nClassNum		= g_pWeaponMgr->GetWeaponClass(nWeaponId);
		pInven->rDetail			= g_pWeaponMgr->GetWeaponPerformance(nWeaponId);
		pInven->szDescription	= NF_GetMsg( (DWORD)g_pWeaponMgr->GetWeaponDescription(nWeaponId) );
		strName = pWeapon->GetInvenNormalIcon();
		pInven->hWeaponTx		= g_pInterfaceResMgr->GetTexture(strName.c_str());
		strName = pWeapon->GetInvenSelectIcon();		
		pInven->hSelectTx		= g_pInterfaceResMgr->GetTexture(strName.c_str());
		//pInven->pFontDuration	= new CSAD3DText();
		//pInven->pFontDuration->CreateD3DText( 128, 16, true );
		//pInven->pFontDuration->SetPosition(533, 383);
		//pInven->pFontDuration->SetTextColor(RGB(190,160,112));
		//char szDuration[256] = {0, };
		//sprintf(szDuration, "남은기간 : %d일", nDuration);
		//pInven->pFontDuration->DrawD3DText(szDuration, lstrlen(szDuration), DT_LEFT, D3D_TEXT_OUTLINE );

		m_Invens.push_back(pInven);
	}
	else
	{
		return LTFALSE;
	}

	return LTTRUE;
}

LTBOOL CScreenInventory::RemoveWeapon(uint8 nWeaponId)
{
	WeaponInvenList::iterator iter = m_Invens.begin();
	while (iter != m_Invens.end() && (*iter)->nWeaponId != nWeaponId )
	{
		iter++;
	}
	
	if( iter != m_Invens.end() )
	{	
		//SAFE_DELETE((*iter)->pFontDuration);		
		int nClass = (*iter)->nClassNum;
		LTBOOL bUsed = (*iter)->bUsed;
		debug_delete(*iter);
		m_Invens.erase(iter);
		
		if( nClass > 0 && nClass <= 5 )
		{
			m_SelectedWeaponId[ nClass-1 ] = WMGR_INVALID_ID;
			m_pFrameSelectedWeapon[nClass - 1]->SetFrame(g_hEmptyInevnTx);			
		}

		if( bUsed ) SetDefaultWeapon( nClass );
	}
	else
	{
		return LTFALSE;
	}

	return LTTRUE;
}

void CScreenInventory::SetGauge(int nDamage, int nShoot, int nHit, int nReact, int nClip, int nSltAmmo)
{
	m_fDamage				= (float)nDamage;
	m_fShoot				= (float)nShoot;
	m_fHit					= (float)nHit;
	m_fReact				= (float)nReact;	
	m_fDeltaDamage			= 0.0f;
	m_fDeltaShoot			= 0.0f;
	m_fDeltaHit				= 0.0f;
	m_fDeltaReact			= 0.0f;

	for( int i = 0; i < 4; i++ )
	{
		m_pFrameGaugeBar[i]->Show(LTTRUE);
		m_pFrameWeaponInfo[i]->Show(LTTRUE);
	}
	
	char szTemp[64] = {0, };
	sprintf(szTemp, "%d발 / %d발", nClip, nSltAmmo);
	m_pFontWeaponClip->DrawD3DText(szTemp, lstrlen(szTemp) );

	if( LTNULL == m_hGaugeSound )
	{
		m_hGaugeSound = g_pClientSoundMgr->PlayInterfaceSound( "SA_INTERFACE\\SND\\GaugeLoop.wav", PLAYSOUND_LOOP );
	}
}

void CScreenInventory::UpdateGaugeAni()
{	
	float fGuageDelta = 100 / 1.5f * g_pLTClient->GetFrameTime();
	int nFlag = 0;
	
	if( m_fDamage > m_fDeltaDamage )
	{
		m_fDeltaDamage += fGuageDelta;
	}
	else
	{
		m_fDeltaDamage = m_fDamage;
		nFlag |= 0x0000000F;
	}

	if( m_fShoot > m_fDeltaShoot )
	{
		m_fDeltaShoot += fGuageDelta;
	}
	else
	{
		m_fDeltaShoot = m_fShoot;
		nFlag |= 0x000000F0;
	}

	if( m_fHit > m_fDeltaHit )
	{
		m_fDeltaHit += fGuageDelta;
	}
	else
	{
		m_fDeltaHit = m_fHit;
		nFlag |= 0x00000F00;
	}

	if( m_fReact > m_fDeltaReact )
	{
		m_fDeltaReact += fGuageDelta;
	}
	else
	{
		m_fDeltaReact = m_fReact;
		nFlag |= 0x0000F000;
	}

	char szText[64] = {0, };
	uint16 nHeight = 13;
	uint16 nWidth = (uint16)(84*(m_fDeltaDamage*0.01f));
	m_pFrameWeaponInfo[0]->SetSize(nWidth, nHeight);
	nWidth = (uint16)(84*(m_fDeltaShoot*0.01f));
	m_pFrameWeaponInfo[1]->SetSize(nWidth, nHeight);
	nWidth = (uint16)(84*(m_fDeltaHit*0.01f));
	m_pFrameWeaponInfo[2]->SetSize(nWidth, nHeight);
	nWidth = (uint16)(84*(m_fDeltaReact*0.01f));
	m_pFrameWeaponInfo[3]->SetSize(nWidth, nHeight);	

	sprintf(szText, "%3.0f%%", m_fDeltaDamage);
	m_pFontWeaponInfo[0]->DrawD3DText(szText, lstrlen(szText));
	ZeroMemory( szText, sizeof(szText) );
	sprintf(szText, "%3.0f%%", m_fDeltaShoot);
	m_pFontWeaponInfo[1]->DrawD3DText(szText, lstrlen(szText));
	ZeroMemory( szText, sizeof(szText) );
	sprintf(szText, "%3.0f%%", m_fDeltaHit);
	m_pFontWeaponInfo[2]->DrawD3DText(szText, lstrlen(szText));
	ZeroMemory( szText, sizeof(szText) );
	sprintf(szText, "%3.0f%%", m_fDeltaReact);
	m_pFontWeaponInfo[3]->DrawD3DText(szText, lstrlen(szText));

	if( nFlag >= 0x0000FFFF && m_hGaugeSound )
	{
		g_pLTClient->SoundMgr()->KillSound( m_hGaugeSound );
		m_hGaugeSound	= LTNULL;
	}
}

void CScreenInventory::RenderGaugeText()
{
	for(int i = 0; i < 4; i++ )
	{
		m_pFontWeaponInfo[i]->Render();
	}
	m_pFontWeaponClip->Render();
	m_pFontWeaponDesc->Render();
}

void CScreenInventory::ShowSubWeapon(uint8 nClass)
{
	if( nClass < 0 || nClass > 4) return;

	g_nCurSubTab = nClass;

	for( int n = 0; n < 9; n++ )
	{
		m_pBtnInvenSlot[n]->Show(LTTRUE);
		m_pFrameInvenSlot[n]->Show(LTTRUE);
		m_pFontWeaponTitle[n]->DrawD3DText("", 0 );
	}

	m_pFrameSelect->Show(LTFALSE);

	for( n = 0; n < 4; n++ )
	{
		m_pFrameInvenUsed[n]->Show(LTFALSE);
	}	

	WeaponInvenList::iterator iter = m_Invens.begin();

	int nSize = 0;
	int nCount = 0;
	int nSlot = 0;
	if( nClass == 0 )
	{
		nSize = m_Invens.size();
	}
	else
	{
		while ( iter != m_Invens.end() )
		{
			if( (*iter)->nClassNum == g_nCurSubTab )
			{
				nCount++;
			}
			iter++;			
		}
		nSize = nCount;
	}

	if( nSize <= 9 )
	{
		g_nSubStep[g_nCurSubTab] = 0;
		g_fSubRatio[g_nCurSubTab] = 0.0f;
	}
	else
	{
		int tempSize = nSize/3;
		int modSize = nSize%3;
		if( modSize )
		{
			tempSize += 1;
		}
		tempSize -= 3;
		g_nSubStep[g_nCurSubTab] = tempSize;
		g_fSubRatio[g_nCurSubTab]	= 1.0f / (float)g_nSubStep[g_nCurSubTab];
	}
	/*
	g_nSubStep[g_nCurSubTab] = (int)(nSize/9);
	g_fSubRatio[g_nCurSubTab]	= 1.0f / (float)g_nSubStep[g_nCurSubTab];	
	*/
	if( g_nSubStep[g_nCurSubTab] )
	{
		m_pBtnScroll->Show(LTTRUE);
	}
	else
	{
		m_pBtnScroll->Show(LTFALSE);
	}

	std::string strName;
	nCount = 0;
	nSlot = 0;
	iter = m_Invens.begin();
	if( nClass == 0 )
	{
		while( iter != m_Invens.end() )
		{
			if( nCount >= g_nCurSubStep[g_nCurSubTab]*3 && nCount < ((g_nCurSubStep[g_nCurSubTab]*3)+9) )
			{
				m_pFrameInvenSlot[nSlot]->SetFrame((*iter)->hWeaponTx, LTTRUE);				
				//WEAPON const* pWeapon = g_pWeaponMgr->GetWeapon((*iter)->nWeaponId);
				//if( pWeapon )
				//{
				//	strName = pWeapon->szName;
				//}
				//m_pFontWeaponTitle[nSlot]->DrawD3DText(strName.c_str(), lstrlen(strName.c_str()) );
				m_pFontWeaponTitle[nSlot]->DrawD3DText((*iter)->szName, lstrlen((*iter)->szName),DT_LEFT,D3D_TEXT_SHADOW );
				
				

				if( (*iter)->bUsed == true )
				{
					// 사용중 이미지 처리
					LTIntPt basePos;
					basePos = m_pBtnInvenSlot[nSlot]->GetBasePos();
					basePos.x += 67;
					basePos.y += 48;
					if( (*iter)->nClassNum - 1 >= 0 && (*iter)->nClassNum - 1 < 4)
					{
						m_pFrameInvenUsed[(*iter)->nClassNum - 1]->SetBasePos(basePos);
						m_pFrameInvenUsed[(*iter)->nClassNum - 1]->Show(LTTRUE);
					}					
				}

				if( (*iter)->bSelected == true )
				{
					// Select 이미지 처리
					LTIntPt basePos;
					basePos = m_pBtnInvenSlot[nSlot]->GetBasePos();
					basePos.x += 1;
					basePos.y += 16;
					m_pFrameSelect->SetBasePos(basePos);
					m_pFrameSelect->Show(LTTRUE);					
				}
				
				nSlot++;
			}
			iter++;
			nCount++;
		}

		if( nSlot < 9 )
		{
			for( int i = nSlot; i < 9; i++ )
			{
				m_pBtnInvenSlot[i]->Show(LTFALSE);
				m_pFrameInvenSlot[i]->Show(LTFALSE);
				m_pFontWeaponTitle[i]->DrawD3DText("", 0 );
			}
		}
	}
	else
	{
		while( iter != m_Invens.end() )
		{
			if( (*iter)->nClassNum == nClass )
			{
				if( nCount >= g_nCurSubStep[g_nCurSubTab]*3 && nCount < ((g_nCurSubStep[g_nCurSubTab]*3)+9) )
				{
					m_pFrameInvenSlot[nSlot]->SetFrame((*iter)->hWeaponTx, LTTRUE);
					//WEAPON const* pWeapon = g_pWeaponMgr->GetWeapon((*iter)->nWeaponId);
					//if( pWeapon )
					//{
					//	strName = pWeapon->szName;
					//}
					//m_pFontWeaponTitle[nSlot]->DrawD3DText(strName.c_str(), lstrlen(strName.c_str()) );
					m_pFontWeaponTitle[nSlot]->DrawD3DText((*iter)->szName, lstrlen((*iter)->szName),DT_LEFT,D3D_TEXT_SHADOW );

					if( (*iter)->bUsed == true )
					{
						// 사용중 이미지 처리
						LTIntPt basePos;
						basePos = m_pBtnInvenSlot[nSlot]->GetBasePos();
						basePos.x += 67;
						basePos.y += 48;
						if( nClass - 1 >= 0 && nClass - 1 < 4)
						{
							m_pFrameInvenUsed[nClass - 1]->SetBasePos(basePos);
							m_pFrameInvenUsed[nClass - 1]->Show(LTTRUE);
						}
						
					}

					if( (*iter)->bSelected == true )
					{
						// Select 이미지 처리
						LTIntPt basePos;
						basePos = m_pBtnInvenSlot[nSlot]->GetBasePos();
						basePos.x += 1;
						basePos.y += 16;
						m_pFrameSelect->SetBasePos(basePos);
						m_pFrameSelect->Show(LTTRUE);
					}
					nSlot++;
				}
				nCount++;
			}
			iter++;
		}

		if( nSlot < 9 )
		{
			for( int i = nSlot; i < 9; i++ )
			{
				m_pBtnInvenSlot[i]->Show(LTFALSE);
				m_pFrameInvenSlot[i]->Show(LTFALSE);
				m_pFontWeaponTitle[i]->DrawD3DText("", 0 );
			}
		}
	}
}

void CScreenInventory::RenderSubWeaponTitle()
{
	for( int i = 0; i < 9; i++ )
	{
		m_pFontWeaponTitle[i]->Render();
	}
}

void CScreenInventory::SetSelectedTexturePos( uint8 nClass, int nId, uint32 dwParam1 )
{
	int nSize = 0;
	int nCount = 0;
	int nWeaponId = 0;
	WeaponInvenList::iterator iter = m_Invens.begin();
	if( nClass == 0 )
	{
		nSize = m_Invens.size();
		//nWeaponId = nId;
		while(iter != m_Invens.end())
		{
			if( nCount == nId )
			{
				nWeaponId = (*iter)->nWeaponId;
				break;
			}
			iter++;
			nCount++;
		}
	}
	else
	{
		while(iter != m_Invens.end())
		{
			if( (*iter)->nClassNum == nClass )
			{
				if( nCount == nId )
				{
					nWeaponId = (*iter)->nWeaponId; 
				}
				nCount++;
			}		
			iter++;		
		}
		nSize = nCount;
	}

	if( nId < 0 || nId >= nSize ) return;
	
	iter = m_Invens.begin();
	while ( iter != m_Invens.end() )
	{		
		(*iter)->bSelected = false;		
		iter++;
	}

	nCount = 0;
	iter = m_Invens.begin();
	while ( iter != m_Invens.end() )
	{	
		if( (*iter)->nWeaponId == nWeaponId )
		{
			{
				m_pFrmDetailBack->Show(LTFALSE);
				m_pFrmSelectCharDetail->Show(LTFALSE);
			}
			(*iter)->bSelected = true;
			//gauge
			m_pFontWeaponName->DrawD3DText((*iter)->szName, lstrlen((*iter)->szName),DT_LEFT,D3D_TEXT_SHADOW );
			m_pFontWeaponDur->DrawD3DText((*iter)->szExpireTime, lstrlen((*iter)->szExpireTime), DT_RIGHT);
			m_pFrameDetail->Show(LTTRUE);
			m_pFrameWeaponDetail->SetFrame((*iter)->hSelectTx);
			m_pFrameWeaponDetail->Show(LTTRUE);
			//m_pFontWeaponDesc->DrawD3DText((*iter)->szDescription, lstrlen((*iter)->szDescription), DT_LEFT,	D3D_TEXT_OUTLINE );
			m_pFontWeaponDesc->DrawD3DText((*iter)->szDescription, lstrlen((*iter)->szDescription));
			this->SetGauge( (*iter)->rDetail.left, 
							(*iter)->rDetail.top,
							(*iter)->rDetail.right,
							(*iter)->rDetail.bottom,
							(*iter)->nClipInAmmo, (*iter)->nSelectAmmo );

			break;
		}
		iter++;
		nCount++;
	}

	g_nSelectWeapon = nWeaponId;
	LTIntPt	pos;
	pos = m_pBtnInvenSlot[dwParam1]->GetBasePos();
	pos.x += 1;
	pos.y += 16;
	m_pFrameSelect->SetBasePos(pos);
	m_pFrameSelect->Show(LTTRUE);	
}

void CScreenInventory::SetUsedTexturePos( uint8 nClass, int nId, uint32 dwParam1 )
{
	int nSelectClass;
	HTEXTURE hTex	= LTNULL;
	WeaponInvenList::iterator iter = m_Invens.begin();
	while( iter != m_Invens.end() )
	{
		if( (*iter)->nWeaponId == nId )
		{
			(*iter)->bUsed = true;
			nSelectClass = (*iter)->nClassNum;
			hTex = (*iter)->hWeaponTx;
		}
		iter++;
	}
	iter = m_Invens.begin();
	while(iter != m_Invens.end())
	{
		if( (*iter)->nWeaponId != nId && (*iter)->nClassNum == nSelectClass )
		{
			(*iter)->bUsed = false;
		}
		iter++;
	}

	m_SelectedWeaponId[nSelectClass - 1] = nId;
	if( !m_IsShop )
	{
		if( nSelectClass - 1 >= 0 && nSelectClass - 1 < 4 )
		{
			//m_SelectedWeaponId[nSelectClass - 1] = nId;
			m_pFrameSelectedWeapon[nSelectClass - 1]->SetFrame(hTex);
			m_pFrameSelectedWeapon[nSelectClass - 1]->Show(LTTRUE);
		}
	}

	//this->ShowSubWeapon(nClass);
	this->ShowSubWeapon(g_nCurSubTab);
}

void CScreenInventory::ShowSubChar(uint8 nClass)
{
	if( nClass < 0 || nClass  > 2) return;

	// button 위치
	switch ( nClass )
	{
		case 0 :
			{
				m_pCheckCharAll->SetCheck(LTTRUE);
				m_pCheckCharChar->SetCheck(LTFALSE);
				m_pCheckCharAccessory->SetCheck(LTFALSE);
			}break;
		case 1 :
			{
				m_pCheckCharAll->SetCheck(LTFALSE);
				m_pCheckCharChar->SetCheck(LTTRUE);
				m_pCheckCharAccessory->SetCheck(LTFALSE);
			}break;
		case 2 :
			{
				m_pCheckCharAll->SetCheck(LTFALSE);
				m_pCheckCharChar->SetCheck(LTFALSE);
				m_pCheckCharAccessory->SetCheck(LTTRUE);
			}break;
		default :
			break;
	}

	g_nCharCurSubTab = nClass;

	for( int n = 0; n < 6; n++ )
	{
		m_pBtnCharSlot[n]->Show(LTTRUE);
		m_pFrmCharSlot[n]->Show(LTTRUE);
	}
	m_pFontCharTitle->DrawD3DText( "", lstrlen("") );
	m_pFrmSelectCharSlot->Show(LTFALSE);

	for( n = 0; n < 3; n++ )
	{
		m_pFrmUsedCharIcon[n]->Show(LTFALSE);
	}

	// Item Size
	CharInvenList::iterator iter = m_Chars.begin();

	int nSize = 0;
	int nCount = 0;
	int nSlot = 0;
	if( nClass == 0 )
	{
		nSize = m_Chars.size();
	}
	else
	{
		while( iter != m_Chars.end() )
		{
			if( (*iter)->nClassNum == nClass )
			{
				nCount++;
			}
			iter++;
		}
		nSize = nCount;
	}
	
	/*
	g_nCharSubStep[nClass]	= (int)(nSize/6);
	g_fCharSubRatio[nClass]	= 1.0f / (float)g_nCharSubStep[nClass];
	*/
	if( nSize <= 6 )
	{
		g_nCharSubStep[nClass] = 0;
		g_fCharSubRatio[nClass] = 0.0f;
	}
	else
	{
		int tempSize = nSize/3;
		int modSize = nSize%3;
		if( modSize )
		{
			tempSize += 1;
		}
		tempSize -= 2;
		g_nCharSubStep[nClass] = tempSize;
		g_fCharSubRatio[nClass]	= 1.0f / (float)g_nCharSubStep[nClass];
	}

	if( g_nCharSubStep[nClass] )
	{
		m_pBtnScroll->Show(LTTRUE);
	}
	else
	{
		m_pBtnScroll->Show(LTFALSE);
	}
	
	nCount = 0;
	nSlot = 0;
	iter = m_Chars.begin();
	if( nClass == 0 )
	{
		while( iter != m_Chars.end() )
		{			
			if( (nCount >= g_nCharCurSubStep[g_nCharCurSubTab]*3) &&
				nCount < ((g_nCharCurSubStep[g_nCharCurSubTab]*3)+6) )
			{
				m_pFrmCharSlot[nSlot]->SetFrame((*iter)->hNormalTx, LTTRUE);

				m_pFontCharName[nSlot]->DrawD3DText((*iter)->szName, lstrlen((*iter)->szName));

				if( (*iter)->bUsed == true )
				{
					// 사용중 이미지 처리
					LTIntPt basePos;
					basePos = m_pBtnCharSlot[nSlot]->GetBasePos();
					basePos.x += 67;
					basePos.y += 82;
					if( (*iter)->nClassNum == ITEM_CHAR_SLOT )
					{
						if( (*iter)->nTeamId == 0 || (*iter)->nTeamId == 1 )
						{
							m_pFrmUsedCharIcon[(*iter)->nTeamId]->SetBasePos(basePos);
							m_pFrmUsedCharIcon[(*iter)->nTeamId]->Show(LTTRUE);
						}
					}
					else
					{
						m_pFrmUsedCharIcon[2]->SetBasePos(basePos);
						m_pFrmUsedCharIcon[2]->Show(LTTRUE);
					}
				}

				if( (*iter)->bSelected == true )
				{
					// Select 이미지 처리
					LTIntPt basePos;
					basePos = m_pBtnCharSlot[nSlot]->GetBasePos();
					basePos.x += 1;
					basePos.y += 16;
					m_pFrmSelectCharSlot->SetBasePos(basePos);
					m_pFrmSelectCharSlot->Show(LTTRUE);
					m_pFontCharTitle->DrawD3DText( (*iter)->szName, 
												   lstrlen((*iter)->szName),
												   DT_LEFT,
												   D3D_TEXT_SHADOW );
				}
				nSlot++;
			}
			iter++;
			nCount++;
		}
		if( nSlot < 6 )
		{
			for( int i = nSlot; i < 6; i++ )
			{
				m_pBtnCharSlot[i]->Show(LTFALSE);
				m_pFrmCharSlot[i]->Show(LTFALSE);

				m_pFontCharName[i]->DrawD3DText("", 0);
			}
		}
	}
	else
	{
		while( iter != m_Chars.end() )
		{
			if( (*iter)->nClassNum == nClass )
			{
				if( (nCount >= g_nCharCurSubStep[g_nCharCurSubTab]*3) &&
					nCount < ((g_nCharCurSubStep[g_nCharCurSubTab]*3)+6) )
				{
					m_pFrmCharSlot[nSlot]->SetFrame((*iter)->hNormalTx, LTTRUE);

					m_pFontCharName[nSlot]->DrawD3DText((*iter)->szName, lstrlen((*iter)->szName));

					if( (*iter)->bUsed == true )
					{
						// 사용중 이미지 처리
						LTIntPt basePos;
						basePos = m_pBtnCharSlot[nSlot]->GetBasePos();
						basePos.x += 67;
						basePos.y += 82;
						if( (*iter)->nClassNum == ITEM_CHAR_SLOT )
						{
							if( (*iter)->nTeamId == 0 || (*iter)->nTeamId == 1 )
							{
								m_pFrmUsedCharIcon[(*iter)->nTeamId]->SetBasePos(basePos);
								m_pFrmUsedCharIcon[(*iter)->nTeamId]->Show(LTTRUE);
							}
						}
						else
						{
							m_pFrmUsedCharIcon[2]->SetBasePos(basePos);
							m_pFrmUsedCharIcon[2]->Show(LTTRUE);
						}
					}


					if( (*iter)->bSelected == true )
					{
						// Select 이미지 처리
						LTIntPt basePos;
						basePos = m_pBtnCharSlot[nSlot]->GetBasePos();
						basePos.x += 1;
						basePos.y += 16;
						m_pFrmSelectCharSlot->SetBasePos(basePos);
						m_pFrmSelectCharSlot->Show(LTTRUE);
						m_pFontCharTitle->DrawD3DText( (*iter)->szName, 
												   lstrlen((*iter)->szName),
												   DT_LEFT,
												   D3D_TEXT_SHADOW );
					}
					nSlot++;
				}
				nCount++;
			}
			iter++;
		}
		if( nSlot < 6 )
		{
			for( int i = nSlot; i < 6; i++ )
			{
				m_pBtnCharSlot[i]->Show(LTFALSE);
				m_pFrmCharSlot[i]->Show(LTFALSE);
				
				m_pFontCharName[i]->DrawD3DText("", 0);
			}
		}
	}
}

sChar_Set* CScreenInventory::GetCharTab( uint8 nId, uint8 nClass )
{
	int nCount = 0;
	int nSize = m_Chars.size();

	CharInvenList::iterator iter = m_Chars.begin();
	while( iter != m_Chars.end() )
	{
		if( (*iter)->nItemId == nId && (*iter)->nClassNum == nClass )
		{
			break;
		}
		iter++;
		nCount++;
	}

	if( nCount >= nSize )
	{
		return LTNULL;
	}

	return m_Chars[nCount];
}

LTBOOL CScreenInventory::AddCharTab( sChar_Set* pItem )
{
	CharInvenList::iterator iter = m_Chars.begin();
	while( iter != m_Chars.end() )
	{
		if( (*iter)->nItemId == pItem->nItemId && (*iter)->nClassNum == pItem->nClassNum ) break;

		iter++;
	}

	ModelId  id;
	id = ModelId(pItem->nItemId);

	pItem->bSelected		= false;
	pItem->bUsed			= false;
	pItem->hNormalTx		= g_pInterfaceResMgr->GetTexture(g_pModelButeMgr->GetShopNormalIcon(id).c_str());
	pItem->hSelectTx		= g_pInterfaceResMgr->GetTexture(g_pModelButeMgr->GetShopDetailIcon(id).c_str());
	pItem->szName			= NF_GetMsg( g_pModelButeMgr->GetShopIconNameIndex(id) );
	pItem->szDescription	= NF_GetMsg( g_pModelButeMgr->GetModelDescIndex(id) );		
	pItem->nTeamId			= g_pModelButeMgr->GetModelTeamID(id);

	if( iter == m_Chars.end() )
	{
		m_Chars.push_back(pItem);
	}
	else
	{
		//기존에 있다는것은 기간연장의 경우
		strcpy( (*iter)->szExpireTime, pItem->szExpireTime );
		delete pItem;		
	}

	return LTTRUE;
}

LTBOOL CScreenInventory::AddCharTab( int nId, uint8 nTeamId, int nDuration, int nClass )
{
	CharInvenList::iterator iter = m_Chars.begin();
	while( iter != m_Chars.end() )
	{
		if( (*iter)->nItemId == nId && (*iter)->nClassNum == nClass ) break;

		iter++;
	}	

	sChar_Set* pInven = LTNULL;
	if( iter == m_Chars.end() )
	{
		pInven = debug_new(sChar_Set);

		std::string strName;

		pInven->nClassNum	= nClass;
		pInven->nItemId		= nId;
		if( nClass == ITEM_CHAR_SLOT )		// 1 번이 character
		{
			ModelId  id;
			id = ModelId(nId);
			strName = g_pModelButeMgr->GetShopNormalIcon(id);
			pInven->hNormalTx = g_pInterfaceResMgr->GetTexture(strName.c_str());
			strName = g_pModelButeMgr->GetShopDetailIcon(id);
			pInven->hSelectTx = g_pInterfaceResMgr->GetTexture(strName.c_str());
			//----------------------------------- Test Test ------------------------------------
			if(pInven->szName)sprintf(pInven->szName, "땀복");
			pInven->szDescription = NF_GetMsg( (DWORD)g_pWeaponMgr->GetWeaponDescription(0) );
			//----------------------------------------------------------------------------------
			pInven->nTeamId = nTeamId;
			//pInven->pFontDuration = new CSAD3DText();
			//pInven->pFontDuration->CreateD3DText( 128, 16, true );
			//pInven->pFontDuration->SetPosition( 533, 384 );
			//pInven->pFontDuration->SetTransparent( true );
			//pInven->pFontDuration->SetTextColor(RGB(199,178,153));
			//sprintf((char*)strName.c_str(), "남은 시간%d", nDuration);
			//pInven->pFontDuration->DrawD3DText(strName.c_str(), lstrlen(strName.c_str()), DT_LEFT, D3D_TEXT_OUTLINE );
		}
		
		m_Chars.push_back(pInven);
	}
	else
	{
		return LTFALSE;
	}
	
	return LTTRUE;
}

LTBOOL CScreenInventory::RemoveCharTab( int nId, int nClass )
{
	CharInvenList::iterator iter = m_Chars.begin();
	while ( iter != m_Chars.end() )
	{
		if( (*iter)->nItemId == nId && (*iter)->nClassNum == nClass ) break;
		
		iter++;
	}

	if( iter != m_Chars.end() )
	{
		//SAFE_DELETE( (*iter)->pFontDuration );
		int nTeamID = (*iter)->nTeamId;
		LTBOOL bUsed = (*iter)->bUsed;

		debug_delete(*iter);
		m_Chars.erase(iter);

		if( bUsed ) SetDefaultCharacter( nTeamID );
	}
	else
	{
		return LTFALSE;
	}

	return LTTRUE;
}

void CScreenInventory::RenderSubCharTitle()
{
	m_pFontCharTitle->Render();
	m_pFontCharDesc->Render();
	m_pFontCharDuration->Render();
}

void CScreenInventory::SetCharSelectedTexPos(uint8 nClass, int nIndex, uint32 dwParam1 )
{
	int nSize = 0;
	int nCount = 0;
	int nItemId = 0;
	int nClassNum = 0;
	CharInvenList::iterator iter = m_Chars.begin();
	if( nClass == 0 )
	{
		nSize = m_Chars.size();
		while( iter != m_Chars.end() )
		{
			if( nCount == nIndex )
			{
				nItemId = (*iter)->nItemId;
				nClassNum = (*iter)->nClassNum;
				break;
			}
			iter++;
			nCount++;
		}
	}
	else
	{
		while( iter != m_Chars.end() )
		{
			if( (*iter)->nClassNum == nClass )
			{
				if( nCount == nIndex )
				{
					nItemId = (*iter)->nItemId;
					nClassNum = (*iter)->nClassNum;
				}
				nCount++;
			}
			iter++;
		}
		nSize = nCount;
	}

	if( nIndex < 0 || nIndex >= nSize ) return;

	iter = m_Chars.begin();
	while( iter != m_Chars.end() )
	{
		(*iter)->bSelected = false;		
		iter++;
	}

	iter = m_Chars.begin();
	while( iter != m_Chars.end() )
	{
		if( (*iter)->nItemId == nItemId && (*iter)->nClassNum == nClassNum )
		{
			{
				m_pFrameDetail->Show(LTFALSE);
				m_pFrameWeaponDetail->Show(LTFALSE);
				for( int i = 0; i < 4; i++ )
				{
					m_pFrameGaugeBar[i]->Show(LTFALSE);
					m_pFrameWeaponInfo[i]->Show(LTFALSE);
				}
			}
			(*iter)->bSelected = true;
			m_pFontCharDuration->DrawD3DText((*iter)->szExpireTime, lstrlen((*iter)->szExpireTime), DT_RIGHT);
			m_pFrmDetailBack->Show(LTTRUE);
			m_pFrmSelectCharDetail->SetFrame( (*iter)->hSelectTx );
			m_pFrmSelectCharDetail->Show(LTTRUE);
			m_pFontCharDesc->DrawD3DText((*iter)->szDescription, lstrlen((*iter)->szDescription));
			m_pFontCharTitle->DrawD3DText( (*iter)->szName, 
										   lstrlen((*iter)->szName),
										   DT_LEFT,
										   D3D_TEXT_SHADOW );
			break;
		}
		iter++;
	}

	g_nSelectItem = nItemId;
	g_nSelectItemClass = nClassNum;
	LTIntPt pos;
	pos = m_pBtnCharSlot[dwParam1]->GetBasePos();
	pos.x += 1;
	pos.y += 16;
	m_pFrmSelectCharSlot->SetBasePos(pos);
	m_pFrmSelectCharSlot->Show(LTTRUE);
}

void CScreenInventory::SetCharUsedTexPos( uint8 nClass, int nIndex, uint32 dwParam1 )
{
	if( nIndex < 0 ) return;

	HTEXTURE hTex = LTNULL;
	int nTeamId = 255;
	CharInvenList::iterator iter = m_Chars.begin();
	while( iter != m_Chars.end() )
	{
		if( (*iter)->nItemId == nIndex && (*iter)->nClassNum == g_nSelectItemClass )
		{
			(*iter)->bUsed = true;
			nTeamId = (*iter)->nTeamId;
			break;
		}
		iter++;
	}

	iter = m_Chars.begin();
	while( iter != m_Chars.end() )
	{
		if( (*iter)->nClassNum == g_nSelectItemClass && 
			(*iter)->nItemId != nIndex &&
			(*iter)->nTeamId == nTeamId )
		{
			(*iter)->bUsed = false;
		}
		iter++;
	}

	if( g_nSelectItemClass == ITEM_CHAR_SLOT )
	{
		if( nTeamId == 0 || nTeamId == 1 )
		{
			m_SelectedCharItemId[nTeamId] = nIndex;
		}
	}
	else
	{
		m_SelectedCharItemId[ACCE_SELECT_NUM] = nIndex;
	}

	this->ShowSubChar(nClass);
}

LTBOOL CScreenInventory::WeaponThrow( int nIndex )
{	
	bool bUsed;
	bool bSelected;
	int nClass;
	WeaponInvenList::iterator iter = m_Invens.begin();
	while ( iter != m_Invens.end() )
	{
		if( (*iter)->nWeaponId == nIndex )
		{
			bUsed = (*iter)->bUsed;
			bSelected = (*iter)->bSelected;
			nClass	= (*iter)->nClassNum;
			break;
		}
		iter++;
	}

	if( RemoveWeapon(nIndex) )
	{
		g_nCurSubStep[g_nCurSubTab] = 0;
		m_pBtnScroll->SetPosYRatio(0);
		if( m_hGaugeSound )
		{
			g_pLTClient->SoundMgr()->KillSound( m_hGaugeSound );
			m_hGaugeSound	= LTNULL;
		}
		
		if( bUsed == true )
		{
			if( (nClass - 1) < 0 || (nClass - 1) >= 4 ) return LTFALSE;
			m_SelectedWeaponId[nClass - 1] = 255;
			m_pFrameSelectedWeapon[nClass - 1]->SetFrame(g_hEmptyInevnTx);
		}

		if( bSelected == true )
		{
			g_nSelectWeapon = -1;
		}
		
		ShowSubWeapon(g_nCurSubTab);

		for( int i = 0; i < 4; i++ )
		{
			m_pFrameWeaponInfo[i]->Show(LTFALSE);
		}
		m_pFrameWeaponDetail->Show(LTFALSE);
	}
	
	return LTTRUE;
}

LTBOOL CScreenInventory::CharacterThrow( int nIndex )
{
	bool bUsed;
	bool bSelected;
	int nTeamId;
	CharInvenList::iterator iter = m_Chars.begin();
	while( iter != m_Chars.end() )
	{
		if( (*iter)->nItemId == nIndex && (*iter)->nClassNum == ITEM_CHAR_SLOT )
		{
			bUsed		= (*iter)->bUsed;
			bSelected	= (*iter)->bSelected;
			nTeamId		= (*iter)->nTeamId;
			break;
		}
		iter++;
	}

	if( RemoveCharTab(nIndex, ITEM_CHAR_SLOT) )
	{
		g_nCharCurSubStep[g_nCharCurSubTab] = 0;
		m_pBtnScroll->SetPosYRatio(0.0f);
		if( bUsed == true )
		{			
			m_SelectedCharItemId[nTeamId] = 255;
			m_pFrmUsedCharIcon[nTeamId]->Show(LTFALSE);
		}

		if( bSelected == true )
		{
			g_nSelectItem = -1;
		}

		iter = m_Chars.begin();
		while( iter != m_Chars.end() )
		{
			if( (*iter)->bDefaultItem && (*iter)->nTeamId == nTeamId )
			{
				g_nSelectItem = (*iter)->nItemId;
				m_SelectedCharItemId[nTeamId] = (*iter)->nItemId;
				(*iter)->bUsed = true;
			}
			iter++;
		}

		UpdateModel(true);
		ShowSubChar(g_nCharCurSubTab);	
	}
 
	return LTTRUE;
}

void CScreenInventory::ShowMainCharacter(LTBOOL bShow)
{
	HOBJECT hChar = m_CharacterSFX[0].GetObject();
	if( hChar )
	{
		if( bShow )
		{
			g_pLTClient->Common()->SetObjectFlags(hChar, OFT_Flags, FLAG_VISIBLE, FLAG_VISIBLE );	
		}
		else
		{
			g_pLTClient->Common()->SetObjectFlags(hChar, OFT_Flags, 0, FLAG_VISIBLE );	
		}
	}
}

void CScreenInventory::ShowMainShop()
{
	// Inven 일 때
	ShowMainCharacter(!m_IsShop);
	m_pBtnCheckRed->Show(!m_IsShop);
	m_pBtnCheckBlue->Show(!m_IsShop);

	// Shop 일 때
	m_pFrmShopBack->Show(m_IsShop);
}

void CScreenInventory::ShowShopWeapon(LTBOOL bShow)
{
	m_pCheckShopWeapon->SetCheck(bShow);
	m_pCheckShopChar->SetCheck(!bShow);
	m_pCheckShopEtc->SetCheck(!bShow);

	ShopSubMenuWeapon(bShow);
	ShopSubMenuCharacter(!bShow);
	ShopSubMenuEtc(!bShow);
}

void CScreenInventory::ShowShopCharacter(LTBOOL bShow)
{
	m_pCheckShopWeapon->SetCheck(!bShow);
	m_pCheckShopChar->SetCheck(bShow);
	m_pCheckShopEtc->SetCheck(!bShow);

	ShopSubMenuWeapon(!bShow);
	ShopSubMenuCharacter(bShow);
	ShopSubMenuEtc(!bShow);
}

void CScreenInventory::ShowShopEtc(LTBOOL bShow)
{
	m_pCheckShopWeapon->SetCheck(!bShow);
	m_pCheckShopChar->SetCheck(!bShow);
	m_pCheckShopEtc->SetCheck(bShow);

	ShopSubMenuWeapon(!bShow);
	ShopSubMenuCharacter(!bShow);
	ShopSubMenuEtc(bShow);
}

void CScreenInventory::ShopSubMenuWeapon(LTBOOL bShow)
{
	for( int i = 0; i < 5; i++ )
	{
		m_pCheckShopWOrder[i]->Show(bShow);
	}

	m_pFrmShopSltWeapon->Show(bShow);
	
	

	if( bShow )
	{
		ShopSubMenuWeaponOrder(0);
		this->ShowShopSubWeapon(0);
	}
	else
	{
		for( i = 0; i < 5; i++ )
		{
			m_pBtnShopWpSlot[i]->Show(bShow);
			m_pFrmShopWpSlot[i]->Show(bShow);
		}		
	}
}

void CScreenInventory::ShopSubMenuCharacter(LTBOOL bShow)
{
	for( int i = 0; i < 3; i++ )
	{
		m_pCheckShopChOrder[i]->Show(bShow);
	}

	m_pFrmShopSltChar->Show(bShow);
	
	if( bShow )
	{
		this->ShopSubMenuCharOrder(0);
		this->ShowShopSubChar(0);
	}
	else
	{
		for( i = 0; i < 4; i++ )
		{
			m_pBtnShopChSlot[i]->Show(bShow);
			m_pFrmShopChSlot[i]->Show(bShow);
		}
	}
}

void CScreenInventory::ShopSubMenuEtc(LTBOOL bShow)
{
}

void CScreenInventory::ShopAllHide()
{
	m_pCheckShopWeapon->Show(LTFALSE);
	m_pCheckShopChar->Show(LTFALSE);
	m_pCheckShopEtc->Show(LTFALSE);
	m_pBtnShopScroll->Show(LTFALSE);
	m_pBtnShopScrollUp->Show(LTFALSE);
	m_pBtnShopScrollDown->Show(LTFALSE);
	m_pBtnShopBuy->Show(LTFALSE);
	m_pBtnShopPresent->Show(LTFALSE);

	ShopSubMenuWeapon(LTFALSE);
	ShopSubMenuCharacter(LTFALSE);
	ShopSubMenuEtc(LTFALSE);
}

void CScreenInventory::ShopSubMenuWeaponOrder(uint8 nOrder)
{
	
	for( int i = 0; i < 5; i++ )
	{
		if( i == nOrder )
		{
			m_pCheckShopWOrder[i]->SetCheck(LTTRUE);
		}
		else
		{
			m_pCheckShopWOrder[i]->SetCheck(LTFALSE);
		}
	}
}

uint8 CScreenInventory::GetShopSubMenuWeaponOrder()
{
	for( int i = 0; i < 5; i++ )
	{
		if( m_pCheckShopWOrder[i]->GetCheck() )
		{
			return i;
		}
	}

	return 0;
}

uint8 CScreenInventory::GetShopSubMenuCharOrder()
{
	for( int i = 0; i < 3; i++ )
	{
		if( m_pCheckShopChOrder[i]->GetCheck() )
		{
			return i;
		}
	}

	return 0;
}

sShopWeapon_Set* CScreenInventory::GetShopWeapon( uint8 nWeaponId )
{
	int nCount = 0;
	int nSize = m_ShopWeapons.size();
	ShopWeaponList::iterator iter = m_ShopWeapons.begin();
	while( iter != m_ShopWeapons.end() )
	{
		if((*iter)->nWeaponId == nWeaponId )
		{
			break;
		}
		iter++;
		nCount++;
	}
	if( nCount >= nSize )
	{
		return LTNULL;
	}

	return m_ShopWeapons[nCount];
}

LTBOOL CScreenInventory::AddShopWeapon( sShopWeapon_Set* pItem, int nDuration, int nPrice )
{
	if( pItem->nClassNum == WMGR_INVALID_ID ) 
	{
		ASSERT(!"pItem->nClassNum ==255");
		return LTFALSE;
	}

	ShopWeaponList::iterator iter = m_ShopWeapons.begin();
	while( iter != m_ShopWeapons.end() && (*iter)->nWeaponId != pItem->nWeaponId )
	{
		iter++;
	}

	WEAPON const *pWeapon = g_pWeaponMgr->GetWeapon(pItem->nWeaponId);
	if( !pWeapon ) return LTFALSE;
	AMMO const*pAmmo = g_pWeaponMgr->GetAmmo(pWeapon->nDefaultAmmoId);
	if( !pAmmo ) return LTFALSE;

	pItem->bSelected		= false;
	pItem->bUsed			= false;
	pItem->nClipInAmmo		= pWeapon->nShotsPerClip;
	pItem->nSelectAmmo		= pAmmo->nSelectionAmount;
	pItem->rDetail			= g_pWeaponMgr->GetWeaponPerformance(pItem->nWeaponId);
	pItem->szName			= NF_GetMsg( (DWORD)g_pWeaponMgr->GetWeaponIconNameIndex(pItem->nWeaponId) );
	pItem->szDescription	= NF_GetMsg( (DWORD)g_pWeaponMgr->GetWeaponDescription(pItem->nWeaponId) );	
	pItem->hWeaponTx		= g_pInterfaceResMgr->GetTexture( pWeapon->GetInvenNormalIcon().c_str() );
	pItem->hSelectTx		= g_pInterfaceResMgr->GetTexture( pWeapon->GetInvenSelectIcon().c_str() );

	if( iter == m_ShopWeapons.end() )
	{
		pItem->nCount = 0;
		pItem->PriceSet[0].nDuration	= nDuration;
		pItem->PriceSet[0].nPrice		= nPrice;
		m_ShopWeapons.push_back(pItem);
	}
	else
	{
		// 같은 무기이면 기간만 더한다...		
		sShopWeapon_Set* pWeapon = (sShopWeapon_Set*)(*iter);

		++pWeapon->nCount;
		pWeapon->PriceSet[pWeapon->nCount].nDuration	= nDuration;
		pWeapon->PriceSet[pWeapon->nCount].nPrice		= nPrice;

		SAFE_DELETE( pItem );

		return LTFALSE;
	}

	return LTTRUE;
}


LTBOOL CScreenInventory::AddShopWeapon( uint8 nWeaponId, int nPrice1, int nPrice2, int nPrice3, int nPrice4 )
{
	ShopWeaponList::iterator iter = m_ShopWeapons.begin();
	while( iter != m_ShopWeapons.end() && (*iter)->nWeaponId != nWeaponId )
	{
		iter++;
	}

	sShopWeapon_Set* pItem = LTNULL;
	if( iter == m_ShopWeapons.end() )
	{
		WEAPON const *pWeapon = g_pWeaponMgr->GetWeapon(nWeaponId);
		if( !pWeapon ) return LTFALSE;
		AMMO const*pAmmo = g_pWeaponMgr->GetAmmo(pWeapon->nDefaultAmmoId);
		if( !pAmmo ) return LTFALSE;

		std::string strName;

		pItem = debug_new(sShopWeapon_Set);

		pItem->nWeaponId		= nWeaponId;
		pItem->nClipInAmmo		= pWeapon->nShotsPerClip;
		pItem->nSelectAmmo		= pAmmo->nSelectionAmount;
		pItem->bSelected		= false;
		pItem->bUsed			= false;
		pItem->nClassNum		= g_pWeaponMgr->GetWeaponClass(nWeaponId);
		pItem->rDetail			= g_pWeaponMgr->GetWeaponPerformance(nWeaponId);
		pItem->szDescription	= NF_GetMsg( (DWORD)g_pWeaponMgr->GetWeaponDescription(nWeaponId) );
		strName = pWeapon->GetInvenNormalIcon();
		pItem->hWeaponTx		= g_pInterfaceResMgr->GetTexture(strName.c_str());
		strName = pWeapon->GetInvenSelectIcon();		
		pItem->hSelectTx		= g_pInterfaceResMgr->GetTexture(strName.c_str());
	/*	pItem->nPrice[0]		= nPrice1;
		pItem->nPrice[1]		= nPrice2;
		pItem->nPrice[2]		= nPrice3;
		pItem->nPrice[3]		= nPrice4;
	*/	
		m_ShopWeapons.push_back(pItem);
	}
	else
	{
		return LTFALSE;
	}
	
	return LTTRUE;
}

LTBOOL CScreenInventory::RemoveShopWeapon( uint8 nWeaponId )
{
	ShopWeaponList::iterator iter = m_ShopWeapons.begin();
	while( iter != m_ShopWeapons.end() && (*iter)->nWeaponId != nWeaponId )
	{
		iter++;
	}

	if( iter != m_ShopWeapons.end() )
	{
		//SAFE_DELETE((*iter)->pFontDuration);		
		debug_delete(*iter);
		m_ShopWeapons.erase(iter);
	}
	else
	{
		return LTFALSE;
	}

	return LTTRUE;
}

void CScreenInventory::ShowShopSubWeapon(uint8 nClass)
{
	int i;
	sShopWeapon_Set* pWeapon;

	if( nClass < 0 || nClass > 4) return;

	g_nShopCurWpSubTab = nClass;

	for( i = 0; i < 5; i++ )
	{
		m_pBtnShopWpSlot[i]->Show(LTTRUE);
		m_pFrmShopWpSlot[i]->Show(LTTRUE);
	}
	
	for( i = 0; i < 5; i++ )
	{
		for( int j = 0; j < MAX_PRICE_SET; j++ )
		{
			m_pFontWpPrice[i][j]->DrawD3DText("", 0);
		}

		m_pFontShopWpName[i]->DrawD3DText("m_pFontShopWpName", lstrlen("m_pFontShopWpName"));
	}

	ShopWeaponList::iterator iter = m_ShopWeapons.begin();
	int nSize = 0;
	int nCount = 0;
	int nSlot = 0;
	if( nClass == 0 )
	{
		while ( iter != m_ShopWeapons.end() )
		{
			if( (*iter)->bBuyItem )
			{
				nCount++;
			}
			iter++;
		}
		nSize = nCount;
	}
	else
	{
		while ( iter != m_ShopWeapons.end() )
		{
			if( (*iter)->bBuyItem && (*iter)->nClassNum == nClass )
			{
				nCount++;
			}
			iter++;
		}
		nSize = nCount;
	}
	m_pFrmShopSltWeapon->Show(LTFALSE);
	
	g_nShopSubStep[g_nShopCurWpSubTab] = nSize - 5;
	if( g_nShopSubStep[g_nShopCurWpSubTab] < 0 )
	{
		g_nShopSubStep[g_nShopCurWpSubTab] = 0;
		g_fShopSubRatio[g_nShopCurWpSubTab]	 = 0.0f;
	}
	else
	{
		g_fShopSubRatio[g_nShopCurWpSubTab]	 = 1.0f / (float)g_nShopSubStep[g_nShopCurWpSubTab];
	}
	
	if( g_nShopSubStep[g_nShopCurWpSubTab] )
	{
		m_pBtnShopScroll->Show(LTTRUE);
	}
	else
	{
		m_pBtnShopScroll->Show(LTFALSE);
	}

	char szPrice[64] = {0, };
	std::string strName;
	nCount = 0;
	nSlot = 0;
	iter = m_ShopWeapons.begin();
	if( nClass == 0 )
	{
		while( iter != m_ShopWeapons.end() )
		{
			pWeapon = (sShopWeapon_Set*)(*iter);

			if( pWeapon->bBuyItem )
			{
				if( nCount >= g_nShopCurSubStep[g_nShopCurWpSubTab] && 
					nCount < g_nShopCurSubStep[g_nShopCurWpSubTab] + 5 )
				{
					m_pFrmShopWpSlot[nSlot]->SetFrame( pWeapon->hWeaponTx, LTTRUE );

					for( i=0 ; i<MAX_PRICE_SET ; ++i )
					{
						if( i<=pWeapon->nCount )
						{
							if( pWeapon->PriceSet[i].nDuration <= 0 )
							{
								sprintf( szPrice, "제한없음" );
							}
							else
							{
								sprintf( szPrice, "%d일: %dP", 
									pWeapon->PriceSet[i].nDuration, pWeapon->PriceSet[i].nPrice );
							}
						}
						else
						{
							szPrice[0] = LTNULL;
						}
						m_pFontWpPrice[nSlot][i]->DrawD3DText( szPrice, lstrlen(szPrice) );
					}
					m_pFontShopWpName[nSlot]->DrawD3DText(pWeapon->szName, lstrlen(pWeapon->szName) );

					if( pWeapon->bSelected == true )
					{
						LTIntPt basePos;
						basePos = m_pBtnShopWpSlot[nSlot]->GetBasePos();
						basePos.x += 1;
						basePos.y += 16;
						m_pFrmShopSltWeapon->SetBasePos(basePos);
						m_pFrmShopSltWeapon->Show(LTTRUE);
					}
					nSlot++;
				}

				nCount++;
			}

			iter++;			
		}

		if( nSlot < 5 )
		{
			for( int i = nSlot; i < 5; i++ )
			{
				m_pBtnShopWpSlot[i]->Show(LTFALSE);
				m_pFrmShopWpSlot[i]->Show(LTFALSE);
				for( int j = 0 ; j < 4; j++ )
				{
					m_pFontWpPrice[i][j]->DrawD3DText( "", 0 );
				}
				m_pFontShopWpName[i]->DrawD3DText("m_pFontShopWpName", lstrlen("m_pFontShopWpName"));
			}
		}
	}
	else
	{
		while( iter != m_ShopWeapons.end() )
		{
			pWeapon = (sShopWeapon_Set*)(*iter);
			
			if( pWeapon->bBuyItem && pWeapon->nClassNum == nClass )
			{
				if( nCount >= g_nShopCurSubStep[g_nShopCurWpSubTab] && 
					nCount < g_nShopCurSubStep[g_nShopCurWpSubTab] + 5 )
				{
					m_pFrmShopWpSlot[nSlot]->SetFrame( pWeapon->hWeaponTx, LTTRUE );
					
					for( i=0 ; i<MAX_PRICE_SET ; ++i )
					{
						if( i<=pWeapon->nCount )
						{
							if( pWeapon->PriceSet[i].nDuration <= 0 )
							{
								sprintf( szPrice, "제한없음" );
							}
							else
							{
								sprintf( szPrice, "%d일: %dP", 
									pWeapon->PriceSet[i].nDuration, pWeapon->PriceSet[i].nPrice );
							}							
						}
						else
						{
							szPrice[0] = LTNULL;
						}
						m_pFontWpPrice[nSlot][i]->DrawD3DText( szPrice, lstrlen(szPrice) );
					}
					m_pFontShopWpName[nSlot]->DrawD3DText(pWeapon->szName, lstrlen(pWeapon->szName) );

					if(pWeapon->bSelected == true )
					{
						LTIntPt basePos;
						basePos = m_pBtnShopWpSlot[nSlot]->GetBasePos();
						basePos.x += 1;
						basePos.y += 16;
						m_pFrmShopSltWeapon->SetBasePos(basePos);
						m_pFrmShopSltWeapon->Show(LTTRUE);
					}
					nSlot++;
				}
				nCount++;
			}
			iter++;
		}
		if( nSlot < 5 )
		{
			for( int i = nSlot; i < 5; i++ )
			{
				m_pBtnShopWpSlot[i]->Show(LTFALSE);
				m_pFrmShopWpSlot[i]->Show(LTFALSE);
				for( int j = 0 ; j <MAX_PRICE_SET ; j++ )
				{
					m_pFontWpPrice[i][j]->DrawD3DText( "", 0 );
				}
				m_pFontShopWpName[i]->DrawD3DText("m_pFontShopWpName", lstrlen("m_pFontShopWpName"));
			}
		}
	}
}

void CScreenInventory::RenderShopWpPrice()
{
	for( int i = 0; i < 5; i++ )
	{
		for( int j = 0; j < MAX_PRICE_SET; j++ )
		{
			m_pFontWpPrice[i][j]->Render();
		}

		m_pFontShopWpName[i]->Render();
	}
}

void CScreenInventory::SetShopSelectedWpTexPos( uint8 nClass, int nIndex, uint32 dwParam1 )
{
	int nSize = 0;
	int nCount = 0;
	int nWeaponId = 0;
	ShopWeaponList::iterator iter = m_ShopWeapons.begin();
	if( nClass == 0 )
	{
		nSize = m_ShopWeapons.size();
		while( iter != m_ShopWeapons.end() )
		{
			if( (*iter)->bBuyItem )
			{
				if( nCount == nIndex )
				{
					nWeaponId = (*iter)->nWeaponId;
					break;
				}
				nCount++;
			}			
			iter++;
			
		}
	}
	else
	{
		while( iter != m_ShopWeapons.end() )
		{
			if( (*iter)->nClassNum == nClass && (*iter)->bBuyItem )
			{
				if( nCount == nIndex )
				{
					nWeaponId = (*iter)->nWeaponId;
				}
				nCount++;
			}
			iter++;
		}
		nSize = nCount;
	}
	
	if( nIndex < 0 || nIndex >= nSize ) return;
	
	iter = m_ShopWeapons.begin();
	while( iter != m_ShopWeapons.end() )
	{
		(*iter)->bSelected = false;
		iter++;
	}

	nCount = 0;
	iter = m_ShopWeapons.begin();
	while( iter != m_ShopWeapons.end() )
	{
		if((*iter)->nWeaponId == nWeaponId )
		{
			{
				m_pFrmDetailBack->Show(LTFALSE);
				m_pFrmSelectCharDetail->Show(LTFALSE);
			}
			(*iter)->bSelected = true;
			// gauge
			m_pFontWeaponName->DrawD3DText((*iter)->szName, lstrlen((*iter)->szName),DT_LEFT,D3D_TEXT_SHADOW );
			m_pFontWeaponDur->DrawD3DText((*iter)->szExpireTime, lstrlen((*iter)->szExpireTime), DT_RIGHT);
			m_pFrameDetail->Show(LTTRUE);
			m_pFrameWeaponDetail->SetFrame((*iter)->hSelectTx);
			m_pFrameWeaponDetail->Show(LTTRUE);
			//m_pFontWeaponDesc->DrawD3DText((*iter)->szDescription, lstrlen((*iter)->szDescription), DT_LEFT,	D3D_TEXT_OUTLINE );
			m_pFontWeaponDesc->DrawD3DText((*iter)->szDescription, lstrlen((*iter)->szDescription) );
			this->SetGauge( (*iter)->rDetail.left, 
							(*iter)->rDetail.top,
							(*iter)->rDetail.right,
							(*iter)->rDetail.bottom,
							(*iter)->nClipInAmmo, (*iter)->nSelectAmmo );

			break;
		}
		iter++;
		nCount++;
	}

	g_nShopSelectWeapon = nWeaponId;
	LTIntPt pos;
	pos = m_pBtnShopWpSlot[dwParam1]->GetPos();
	pos.x += 2;
	pos.y += 16;
	m_pFrmShopSltWeapon->SetBasePos(pos);
	m_pFrmShopSltWeapon->Show(LTTRUE);
}

void CScreenInventory::ShopSubMenuCharOrder(uint8 nOrder)
{
	for( int i = 0; i < 3; i++ )
	{
		if( i == nOrder )
		{
			m_pCheckShopChOrder[i]->SetCheck(LTTRUE);
		}
		else
		{
			m_pCheckShopChOrder[i]->SetCheck(LTFALSE);
		}
	}
}

void CScreenInventory::RenderShopChPrice()
{
	for( int i = 0; i < 4; i++ )
	{
		for( int j = 0; j < MAX_PRICE_SET; j++ )
		{
			m_pFontChPrice[i][j]->Render();
		}
		m_pFontShopChName[i]->Render();
	}	
}

void CScreenInventory::ShowShopSubChar(uint8 nClass)
{
	sShopChar_Set* pCharacter;

	if( nClass < 0 || nClass > 2 ) return;

	for( int i = 0; i < 3; i++ )
	{
		if( i == nClass )
		{
			m_pCheckShopChOrder[i]->SetCheck(LTTRUE);
		}
		else
		{
			m_pCheckShopChOrder[i]->SetCheck(LTFALSE);
		}
	}

	g_nShopCharCurSubTab = nClass;

	for( int n = 0; n < 4; n++ )
	{
		m_pBtnShopChSlot[n]->Show(LTTRUE);
		m_pFrmShopChSlot[n]->Show(LTTRUE);
	}
	m_pFontCharTitle->DrawD3DText( "", lstrlen("") );
	m_pFrmShopSltChar->Show(LTFALSE);

	// Item Size
	ShopCharList::iterator iter = m_ShopChars.begin();
	int nSize = 0;
	int nCount = 0;
	int nSlot = 0;
	if( nClass == 0 )
	{
		while ( iter != m_ShopChars.end() )
		{
			if( (*iter)->bBuyItem )
			{
				nCount++;
			}
			iter++;
		}
		nSize = nCount;		
	}
	else
	{
		while( iter != m_ShopChars.end() )
		{
			if( (*iter)->bBuyItem && (*iter)->nClassNum == nClass )
			{
				nCount++;
			}
			iter++;
		}
		nSize = nCount;
	}
	
	/*
	g_nShopCharSubStep[nClass] = (int)(nSize/4);
	g_fShopCharSubRatio[nClass]	= 1.0f / (float)g_nShopCharSubStep[nClass];	
	*/
	if( nSize <= 4 )
	{
		g_nShopCharSubStep[nClass] = 0;
		g_fShopCharSubRatio[nClass] = 0.0f;
	}
	else
	{
		g_nShopCharSubStep[nClass] = (nSize/2 + nSize%2) - 2;
		g_fShopCharSubRatio[nClass]	= 1.0f / (float)g_nShopCharSubStep[nClass];	
	}

	if( g_nShopCharSubStep[nClass] )
	{
		m_pBtnShopScroll->Show(LTTRUE);
	}
	else
	{
		m_pBtnShopScroll->Show(LTFALSE);
	}

	nCount = 0;
	nSlot = 0;
	char szPrice[64] = {0, };
	iter = m_ShopChars.begin();
	if( nClass == 0 )
	{
		while( iter != m_ShopChars.end() )
		{
			pCharacter = (sShopChar_Set*)(*iter);

			if( pCharacter->bBuyItem )
			{
				if( (nCount >= g_nShopCharCurSubStep[g_nShopCharCurSubTab]*2) &&
					(nCount < (g_nShopCharCurSubStep[g_nShopCharCurSubTab]*2) + 4) )
				{
					m_pFrmShopChSlot[nSlot]->SetFrame( pCharacter->hNormalTx, LTTRUE );

					for( i=0 ; i<MAX_PRICE_SET ; ++i )
					{
						if( i<=pCharacter->nCount )
						{
							if( pCharacter->PriceSet[i].nDuration <= 0 )
							{
								sprintf( szPrice, "제한없음" );
							}
							else
							{
								sprintf( szPrice, "%d일: %dP", 
									pCharacter->PriceSet[i].nDuration, pCharacter->PriceSet[i].nPrice );
							}
						}
						else
						{
							szPrice[0] = LTNULL;
						}
						m_pFontChPrice[nSlot][i]->DrawD3DText( szPrice, lstrlen(szPrice) );
					}
					m_pFontShopChName[nSlot]->DrawD3DText(pCharacter->szName, lstrlen(pCharacter->szName));

					if( (*iter)->bSelected == true )
					{
						LTIntPt basePos;
						basePos = m_pBtnShopChSlot[nSlot]->GetBasePos();
						basePos.x += 1;
						basePos.y += 16;
						m_pFrmShopSltChar->SetBasePos(basePos);
						m_pFrmShopSltChar->Show(LTTRUE);
						m_pFontCharTitle->DrawD3DText( (*iter)->szName, 
													   lstrlen((*iter)->szName),
													   DT_LEFT,
													   D3D_TEXT_SHADOW );
					}
					
					nSlot++;
				}
				nCount++;
			}
			iter++;			
		}
	}
	else
	{
		while( iter != m_ShopChars.end() )
		{
			pCharacter = (sShopChar_Set*)(*iter);

			if( pCharacter->bBuyItem && pCharacter->nClassNum == nClass )
			{
				if( (nCount >= g_nShopCharCurSubStep[g_nShopCharCurSubTab]*2) &&
					(nCount < (g_nShopCharCurSubStep[g_nShopCharCurSubTab]*2)+4) )
				{
					m_pFrmShopChSlot[nSlot]->SetFrame( pCharacter->hNormalTx, LTTRUE );

					for( i=0 ; i<MAX_PRICE_SET ; ++i )
					{
						if( i<=pCharacter->nCount )
						{
							if( pCharacter->PriceSet[i].nDuration <= 0 )
							{
								sprintf( szPrice, "제한없음" );
							}
							else
							{
								sprintf( szPrice, "%d일: %dP", 
									pCharacter->PriceSet[i].nDuration, pCharacter->PriceSet[i].nPrice );
							}
						}
						else
						{
							szPrice[0] = LTNULL;
						}
						m_pFontChPrice[nSlot][i]->DrawD3DText( szPrice, lstrlen(szPrice) );
					}
					m_pFontShopChName[nSlot]->DrawD3DText(pCharacter->szName, lstrlen(pCharacter->szName));

					if( pCharacter->bSelected == true )
					{
						LTIntPt basePos;
						basePos = m_pBtnShopChSlot[nSlot]->GetBasePos();
						basePos.x += 1;
						basePos.y += 16;
						m_pFrmShopSltChar->SetBasePos(basePos);
						m_pFrmShopSltChar->Show(LTTRUE);
						m_pFontCharTitle->DrawD3DText( (*iter)->szName, 
													   lstrlen((*iter)->szName),
													   DT_LEFT,
													   D3D_TEXT_SHADOW );
					}
					nSlot++;
				}
				nCount++;
			}
			iter++;
		}
	}
	if( nSlot < 4 )
	{
		for( int i = nSlot; i < 4; i++ )
		{
			m_pBtnShopChSlot[i]->Show(LTFALSE);
			m_pFrmShopChSlot[i]->Show(LTFALSE);

			// 가격
			for( int j=0 ; j<MAX_PRICE_SET ; ++j )
			{
				m_pFontChPrice[i][j]->DrawD3DText( "", 0 );
			}
			m_pFontShopChName[i]->DrawD3DText("",0);
		}
	}
}

sShopChar_Set* CScreenInventory::GetShopCharTab( uint8 nId, uint8 nClass )
{
	int nCount = 0;
	int nSize = m_ShopChars.size();
	ShopCharList::iterator iter = m_ShopChars.begin();
	while( iter != m_ShopChars.end() )
	{
		if( (*iter)->nItemId == nId && (*iter)->nClassNum == nClass )
		{
			break;
		}
		iter++;
		nCount++;
	}

	if( nCount >= nSize )
	{
		return LTNULL;
	}

	return m_ShopChars[nCount];
}

LTBOOL CScreenInventory::AddShopCharTab( sShopChar_Set* pItem, int nDuration, int nPrice )
{
	ShopCharList::iterator iter = m_ShopChars.begin();
	while( iter != m_ShopChars.end() )
	{
		if((*iter)->nItemId == pItem->nItemId && (*iter)->nClassNum == pItem->nClassNum ) break;

		iter++;
	}

	ModelId  id;
	id = ModelId(pItem->nItemId);

	pItem->bSelected		= false;
	pItem->bUsed			= false;
	pItem->hNormalTx		= g_pInterfaceResMgr->GetTexture(g_pModelButeMgr->GetShopNormalIcon(id).c_str());
	pItem->hSelectTx		= g_pInterfaceResMgr->GetTexture(g_pModelButeMgr->GetShopDetailIcon(id).c_str());
	pItem->szName			= NF_GetMsg( g_pModelButeMgr->GetShopIconNameIndex(id) );
	pItem->szDescription	= NF_GetMsg( g_pModelButeMgr->GetModelDescIndex(id) );		
	pItem->nTeamId			= g_pModelButeMgr->GetModelTeamID(id);

	if( iter == m_ShopChars.end() )
	{
		pItem->nCount = 0;
		pItem->PriceSet[0].nDuration	= nDuration;
		pItem->PriceSet[0].nPrice		= nPrice;
		m_ShopChars.push_back( pItem );
	}
	else
	{
		sShopChar_Set* pCharacter = (sShopChar_Set*)(*iter);

		++pCharacter->nCount;
		pCharacter->PriceSet[pCharacter->nCount].nDuration	= nDuration;
		pCharacter->PriceSet[pCharacter->nCount].nPrice		= nPrice;

		SAFE_DELETE( pItem );

		return LTFALSE;
	}

	return LTTRUE;
}

LTBOOL CScreenInventory::AddShopCharTab( int nId, uint8 nTeamId, int nClass, int nPrice1, int nPrice2, int nPrice3, int nPrice4 )
{
	ShopCharList::iterator iter = m_ShopChars.begin();
	while( iter != m_ShopChars.end() )
	{
		if((*iter)->nItemId == nId && (*iter)->nClassNum == nClass  ) break;

		iter++;
	}
	sShopChar_Set* pItem = LTNULL;
	if( iter == m_ShopChars.end() )
	{
		pItem = debug_new(sShopChar_Set);

		std::string strName;
		pItem->nClassNum	= nClass;
		pItem->nItemId		= nId;
		if( nClass == ITEM_CHAR_SLOT )		// 1 번이 character
		{
			ModelId  id;
			id = ModelId(nId);
			strName = g_pModelButeMgr->GetShopNormalIcon(id);
			pItem->hNormalTx = g_pInterfaceResMgr->GetTexture(strName.c_str());
			strName = g_pModelButeMgr->GetShopDetailIcon(id);
			pItem->hSelectTx = g_pInterfaceResMgr->GetTexture(strName.c_str());
			//----------------------------------- Test Test ------------------------------------
			if(pItem->szName)sprintf(pItem->szName, "땀복");
			pItem->szDescription = NF_GetMsg( (DWORD)g_pWeaponMgr->GetWeaponDescription(0) );
			//----------------------------------------------------------------------------------
			pItem->nTeamId = nTeamId;			
		}
/*		pItem->nPrice[0] = nPrice1;
		pItem->nPrice[1] = nPrice2;
		pItem->nPrice[2] = nPrice3;
		pItem->nPrice[3] = nPrice4;
*/
		m_ShopChars.push_back(pItem);

	}
	else
	{
		return LTFALSE;
	}

	return LTTRUE;
}


LTBOOL CScreenInventory::RemoveShopCharTab( int nId, int nClass )
{
	ShopCharList::iterator iter = m_ShopChars.begin();
	while( iter != m_ShopChars.end() )
	{
		if( (*iter)->nItemId == nId && (*iter)->nClassNum == nClass ) break;

		iter++;
	}

	if( iter != m_ShopChars.end() )
	{
		//SAFE_DELETE( (*iter)->pFontDuration );
		debug_delete(*iter);
		m_ShopChars.erase(iter);
	}
	else
	{
		return LTFALSE;
	}
	
	return LTTRUE;
}

void CScreenInventory::SetShopSelectedChTexPos(uint8 nClass, int nIndex, uint32 dwParam1 )
{
	int nSize = 0;
	int nCount = 0;
	int nItemId = 0;
	int nClassNum = 0;
	ShopCharList::iterator iter = m_ShopChars.begin();
	if( nClass == 0 )
	{
		nSize = m_ShopChars.size();
		while( iter != m_ShopChars.end() )
		{
			if( (*iter)->bBuyItem )
			{
				if( nCount == nIndex )
				{
					nItemId = (*iter)->nItemId;
					nClassNum = (*iter)->nClassNum;
					break;
				}
				nCount++;
			}
			
			iter++;
		}
	}
	else
	{
		while( iter != m_ShopChars.end() )
		{
			if((*iter)->nClassNum == nClass && (*iter)->bBuyItem )
			{
				if( nCount == nIndex )
				{
					nItemId = (*iter)->nItemId;
					nClassNum = (*iter)->nClassNum;
				}
				nCount++;
			}
			iter++;
		}
		nSize = nCount;
	}

	if( nIndex < 0 || nIndex >= nSize ) return;

	iter = m_ShopChars.begin();
	while( iter != m_ShopChars.end() )
	{
		(*iter)->bSelected = false;
		iter++;
	}

	iter = m_ShopChars.begin();
	while( iter != m_ShopChars.end() )
	{
		if( (*iter)->nItemId == nItemId && (*iter)->nClassNum == nClassNum )
		{
			{
				m_pFrameDetail->Show(LTFALSE);
				m_pFrameWeaponDetail->Show(LTFALSE);
				for( int i = 0; i < 4; i++ )
				{
					m_pFrameGaugeBar[i]->Show(LTFALSE);
					m_pFrameWeaponInfo[i]->Show(LTFALSE);
				}
			}
			(*iter)->bSelected = true;
			m_pFontCharDuration->DrawD3DText((*iter)->szExpireTime, lstrlen((*iter)->szExpireTime), DT_RIGHT);
			m_pFrmDetailBack->Show(LTTRUE);
			m_pFrmSelectCharDetail->SetFrame( (*iter)->hSelectTx );
			m_pFrmSelectCharDetail->Show(LTTRUE);
			m_pFontCharDesc->DrawD3DText((*iter)->szDescription, lstrlen((*iter)->szDescription));
			m_pFontCharTitle->DrawD3DText( (*iter)->szName, 
										   lstrlen((*iter)->szName),
										   DT_LEFT,
										   D3D_TEXT_SHADOW );
			break;
		}
		iter++;
	}

	g_nShopSelectItem = nItemId;
	g_nShopSelectItemClass = nClassNum;
	LTIntPt pos;
	pos = m_pBtnShopChSlot[dwParam1]->GetBasePos();
	pos.x += 2;
	pos.y += 16;
	m_pFrmShopSltChar->SetBasePos(pos);
	m_pFrmShopSltChar->Show(LTTRUE);
}

void CScreenInventory::BuyWeaponInfo()
{
	if( g_nShopSelectWeapon < 0 ) return;

	ShopWeaponList::iterator iter = m_ShopWeapons.begin();
	while( iter != m_ShopWeapons.end() )
	{
		if((*iter)->nWeaponId == g_nShopSelectWeapon )
		{
			WEAPON const* pWeapon = g_pWeaponMgr->GetWeapon(g_nShopSelectWeapon);
			if( pWeapon )
			{
				g_pPopupItemBuy->SetWeaponItemInfo( (sShopWeapon_Set*)(*iter) );
				m_pPopupItemBuy->CtrlShow();
				break;
			}
			
		}
		iter++;
	}
}

void CScreenInventory::BuyCharInfo()
{
	if( g_nShopSelectItem < 0 || g_nShopSelectItemClass < 0 ) return;

	ShopCharList::iterator iter = m_ShopChars.begin();
	while( iter != m_ShopChars.end() )
	{
		if((*iter)->nItemId == g_nShopSelectItem && (*iter)->nClassNum == g_nShopSelectItemClass )
		{
			g_pPopupItemBuy->SetCharItemInfo( (sShopChar_Set*)(*iter) );
			m_pPopupItemBuy->CtrlShow();
			break;
		}
		iter++;
	}
}

LTBOOL CScreenInventory::ItemBuy( )
{
	eScreenID scrId = g_pInterfaceMgr->GetCurrentScreen();
	if( scrId != SCREEN_ID_INVENTORY ) return LTFALSE;
	if( m_pCheckShopWeapon->GetCheck() )
	{
		int tempTab = g_nCurSubTab;
		this->ShowWeapon(LTTRUE);
		if( tempTab < 0 ) tempTab = 0;
		this->ShowSubWeapon(tempTab);
		g_nCurSubTab = tempTab;
	}
	else if( m_pCheckShopChar->GetCheck() )
	{
		int tempTab = g_nCharCurSubTab;
		this->ShowCharacter(LTTRUE);
		if( tempTab < 0 ) tempTab = 0;
		this->ShowSubChar(tempTab);
		g_nCharCurSubTab = tempTab;
	}
	
	return LTTRUE;
}

LTBOOL CScreenInventory::RemoveAllShopWeapon()
{
	sShopWeapon_Set* ptr;

	ShopWeaponList::iterator iter = m_ShopWeapons.begin();
	while( iter != m_ShopWeapons.end() )
	{
		ptr = (sShopWeapon_Set*)(*iter);
		if( ptr )
		{
			delete ptr;
			ptr = LTNULL;
		}
		iter++;
	}
	m_ShopWeapons.clear();
	return LTTRUE;
}

LTBOOL CScreenInventory::RemoveAllShop()
{
	RemoveAllShopWeapon();
	RemoveAllShopChar();
	RemoveAllWeapon();
	RemoveAllChar();

	return LTTRUE;
}

LTBOOL CScreenInventory::RemoveAllShopChar()
{
	sShopChar_Set* ptr;

	ShopCharList::iterator iter = m_ShopChars.begin();
	while( iter != m_ShopChars.end() )
	{
		ptr = (sShopChar_Set*)(*iter);
		if( ptr )
		{
			delete ptr;
			ptr = LTNULL;
		}
		iter++;
	}
	m_ShopChars.clear();

	return LTTRUE;
}

LTBOOL CScreenInventory::RemoveAllWeapon()
{
	sInven_Set* ptr;

	WeaponInvenList::iterator iter = m_Invens.begin();
	while( iter != m_Invens.end() )
	{
		ptr = (sInven_Set*)(*iter);
		if( ptr )
		{
			delete ptr;
			ptr = LTNULL;
		}
		iter++;
	}
	m_Invens.clear();

	return LTTRUE;
}

LTBOOL CScreenInventory::RemoveAllChar()
{
	sChar_Set* ptr;

	CharInvenList::iterator iter = m_Chars.begin();
	while( iter != m_Chars.end() )
	{
		ptr = (sChar_Set*)(*iter);
		if( ptr )
		{
			delete ptr;
			ptr = LTNULL;
		}
		iter++;
	}
	m_Chars.clear();

	return LTTRUE;
}

void CScreenInventory::ExitShop()
{
	g_pInterfaceMgr->SwitchToScreen(SCREEN_ID_MAIN);
}

void CScreenInventory::SetGameMoney( __int64 nMoney )
{
	char szTemp[64];
	_i64toa( nMoney, szTemp, 10 );
	m_pInfoGameMoney->DrawD3DText( szTemp, strlen(szTemp), DT_LEFT, D3D_TEXT_NORMAL, false );
}

void CScreenInventory::EquipWeapon( int nIndex )
{
	WeaponInvenList::iterator iter = m_Invens.begin();
	while( iter != m_Invens.end() && (*iter)->nWeaponId != nIndex ) iter++;
	
	if( iter != m_Invens.end() )
	{
		(*iter)->bUsed = true;
		m_SelectedWeaponId[(*iter)->nClassNum-1] = (*iter)->nWeaponId;
	}
}

void CScreenInventory::EquipCharacter( int nIndex, int nTeam )
{
	CharInvenList::iterator iter = m_Chars.begin();
	while( iter != m_Chars.end() && (*iter)->nItemId != nIndex ) iter++;
	
	if( iter != m_Chars.end() )
	{
		g_GameDoc.SetUserModelId( (*iter)->nItemId, nTeam );
		(*iter)->bUsed = true;
	}
}

void CScreenInventory::SetDefaultWeapon( int nClass )
{
	sInven_Set* pSet = LTNULL;

	WeaponInvenList::iterator iter = m_Invens.begin();
	while( iter != m_Invens.end() )
	{
		if( (*iter)->nClassNum == nClass && (*iter)->bDefaultItem )
		{
			if( (*iter)->nClassNum == 1 )	//소총이면 M16을 기본으로
			{
				if( stricmp( (*iter)->szName, "M16" ) == 0 )
				{
					pSet = (sInven_Set*)(*iter);
					break;
				}
			}
			else							//소총이 아니면 디폴트가 1개뿐
			{
				pSet = (sInven_Set*)(*iter);
				break;
			}
		}
		++iter;
	}	

	if( pSet )
	{
		g_Network.CS_AddEquipment( pSet->nClassNum+ITEM_IDX_CHAR_B,
								   pSet->szClass1[0], 
								   pSet->szClass2[0],
								   pSet->szClass2[1],
								   pSet->nWeaponId );
	}
}

void CScreenInventory::SetDefaultCharacter( int nTeamID )
{
	sChar_Set* pSet = LTNULL;
	
	CharInvenList::iterator iter = m_Chars.begin();
	while( iter != m_Chars.end() )
	{
		if( (*iter)->nTeamId == nTeamID && (*iter)->bDefaultItem )
		{
			pSet = (sChar_Set*)(*iter);
			break;
		}
		++iter;
	}
	
	if( pSet )
	{
		g_Network.CS_AddEquipment( nTeamID,
								   pSet->szClass1[0], 
								   pSet->szClass2[0],
								   pSet->szClass2[1],
								   pSet->nItemId );
	}
}

uint8 CScreenInventory::GetCurrentUsedWeapon(uint8 nClass)
{
	if( nClass >= 0 && nClass < 4 )
	{
		//return nClass;
		return m_SelectedWeaponId[nClass];
	}

	return 255;
}

uint8 CScreenInventory::GetSubMenuWeaponOrder()
{
	for( int i = 0; i < 5; i++ )
	{
		if( m_pCheckSubWeapon[i]->GetCheck() )
		{
			return i;
		}
	}

	return 0;
}

void CScreenInventory::SubMenuWeaponOrder(uint8 nOrder)
{
	for(int i = 0; i < 5; i++ )
	{
		if( nOrder == i )
		{
			m_pCheckSubWeapon[i]->SetCheck(LTTRUE);
		}
		else
		{
			m_pCheckSubWeapon[i]->SetCheck(LTFALSE);
		}
	}
}

void CScreenInventory::SetRanking()
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

void CScreenInventory::AddEquipWeapon( int nClass, int nWeaponId )
{
	m_SelectedWeaponId[nClass-1] = nWeaponId;

	WeaponInvenList::iterator iter = m_Invens.begin();
	while( iter != m_Invens.end() )
	{
		if((*iter)->nWeaponId == m_SelectedWeaponId[nClass-1] )
		{
			(*iter)->bUsed = true;
			break;
		}
		iter++;
	}
	iter = m_Invens.begin();
	while( iter != m_Invens.end() )
	{
		if( (*iter)->nWeaponId != m_SelectedWeaponId[nClass-1] && (*iter)->nClassNum == nClass )
		{
			(*iter)->bUsed = false;
		}
		iter++;
	}
	
	if( g_pInterfaceMgr->GetCurrentScreen() != SCREEN_ID_INVENTORY ) return;

	g_pScreenInventory->SetUsedTexturePos( nClass, nWeaponId, LTNULL );
	
	if( nClass == 1 )
	{
		g_pClientSoundMgr->PlayInterfaceSound( "sa_interface\\snd\\button\\wpnsel.wav" );
	}
	else
	{
		g_pClientSoundMgr->PlayInterfaceSound( "sa_interface\\snd\\button\\wpnselpst.wav" );
	}
}

void CScreenInventory::AddEquipCharacter( int nIndex, int nTeam )
{
	g_GameDoc.SetUserModelId( nIndex, nTeam );
	
	if( g_pInterfaceMgr->GetCurrentScreen() != SCREEN_ID_INVENTORY ) return;

	g_pScreenInventory->SetCharUsedTexPos( ITEM_CHAR_SLOT, nIndex, LTNULL );			
	g_pScreenInventory->UpdateModel(true);
}

void CScreenInventory::DiscardWeaponCallback( LTBOOL bReturn, void* pData )
{
	if( bReturn && pData )
	{
		sInven_Set* pWeapon = (sInven_Set*)pData;
		g_Network.CS_DiscardItem( pWeapon->szClass1[0],
								  pWeapon->szClass2[0], pWeapon->szClass2[1],
								  pWeapon->nWeaponId );
	}
}

void CScreenInventory::DiscardCharacterCallback( LTBOOL bReturn, void* pData )
{
	if( bReturn && pData )
	{
		sChar_Set* pCharacter = (sChar_Set*)pData;
		g_Network.CS_DiscardItem( pCharacter->szClass1[0],
								  pCharacter->szClass2[0], pCharacter->szClass2[1],
								  pCharacter->nItemId );
	}
}

void CScreenInventory::SelectAllClear()
{
	WeaponInvenList::iterator iter = m_Invens.begin();
	while(iter != m_Invens.end())
	{
		(*iter)->bSelected = false;
		iter++;
	}
	CharInvenList::iterator iterChar = m_Chars.begin();
	while( iterChar != m_Chars.end() )
	{
		(*iterChar)->bSelected = false;
		iterChar++;
	}
	ShopWeaponList::iterator iterShopWp = m_ShopWeapons.begin();
	while( iterShopWp != m_ShopWeapons.end() )
	{
		(*iterShopWp)->bSelected = false;
		iterShopWp++;
	}
	ShopCharList::iterator iterShopCh = m_ShopChars.begin();
	while( iterShopCh != m_ShopChars.begin() )
	{
		(*iterShopCh)->bSelected = false;
		iterShopCh++;
	}
}

void CScreenInventory::SelectShopWeaponClear()
{
	g_nShopSelectWeapon = -1;
	g_nOldShopSelectWeapon	= -1;
	
	ShopWeaponList::iterator iter = m_ShopWeapons.begin();
	while( iter != m_ShopWeapons.end() )
	{
		(*iter)->bSelected = false;
		iter++;
	}
}

void CScreenInventory::SelectShopCharClear()
{
	g_nShopSelectItem = -1;

	ShopCharList::iterator iter = m_ShopChars.begin();
	while( iter != m_ShopChars.end() )
	{
		(*iter)->bSelected = false;
		iter++;
	}
}

void CScreenInventory::SelectInvenWeaponClear()
{
	g_nSelectWeapon = -1;
	g_nOldSelectWeapon = -1;

	WeaponInvenList::iterator iter = m_Invens.begin();
	while( iter != m_Invens.end() )
	{
		(*iter)->bSelected = false;
		iter++;
	}
}

void CScreenInventory::SelectInvenCharClear()
{
	g_nSelectItem = -1;

	CharInvenList::iterator iter = m_Chars.begin();
	while( iter != m_Chars.end() )
	{
		(*iter)->bSelected = false;
		iter++;
	}
}

void CScreenInventory::DetailWeaponClear()
{
	m_pFrameWeaponDetail->SetFrame(g_hEmptyWeaponDetailTx);
	m_pFrameWeaponDetail->Show(LTFALSE);
	for( int i = 0; i < 4; i++ )
	{
		m_pFrameGaugeBar[i]->Show(LTTRUE);
		m_pFrameWeaponInfo[i]->Show(LTFALSE);
		m_pFontWeaponInfo[i]->DrawD3DText("", 0);
	}
	m_pFontWeaponClip->DrawD3DText("", 0);
	m_pFontWeaponDesc->DrawD3DText("", 0);
	m_pFontWeaponName->DrawD3DText("", 0);
	m_pFontWeaponDur->DrawD3DText("", 0);
	m_fDamage = 0.0f;
	m_fShoot  = 0.0f;
	m_fHit	  = 0.0f;
	m_fReact  = 0.0f;
	// gauge 사운드 끄기
	if( m_hGaugeSound )
	{
		g_pLTClient->SoundMgr()->KillSound( m_hGaugeSound );
		m_hGaugeSound	= LTNULL;
	}
}

void CScreenInventory::DetailCharClear()
{
	m_pFrmSelectCharDetail->SetFrame(g_hEmptyCharDetailTx);
	m_pFrmSelectCharDetail->Show(LTFALSE);
	m_pFontCharDesc->DrawD3DText("", 0);
	m_pFontCharDuration->DrawD3DText("", 0);
	m_pFontCharTitle->DrawD3DText("", 0);
}