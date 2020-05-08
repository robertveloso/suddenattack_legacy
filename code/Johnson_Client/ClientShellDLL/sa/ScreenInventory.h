#ifndef _SCREEN_INVENTORY_H_
#define _SCREEN_INVENTORY_H_

#include "ScreenBase.h"
#include "LayoutMgr.h"

#define ITEM_CHAR_SLOT	1
#define CHAR_SELECT_NUM 1
#define ACCE_SELECT_NUM 2

class CPopUpWeaponDetail;
class CPopUpItemBuy;
class CTextScroll;

struct sInventory
{
	CLTGUIFrame*	pFrameInven[4];
	int				nWeaponId[4];
	
	sInventory()
	{
		for( int i = 0; i < 4; i++ )
		{
			pFrameInven[i] = LTNULL;
			nWeaponId[i] = -1;
		}
	}

	~sInventory()
	{
	}
};

// Open 용으로 새로 만든 부분
struct sInven_Set
{
	char				szClass1[2];
	char				szClass2[3];
	char				szExpireTime[64];
	
	uint8				nWeaponId;
	uint8				nClassNum;
	char*				szName;
	char*				szDescription;
	CRect				rDetail;
	bool				bUsed;
	bool				bSelected;
	HTEXTURE			hWeaponTx;
	HTEXTURE			hSelectTx;
	uint16				nClipInAmmo;
	uint16				nSelectAmmo;
	bool				bDefaultItem;
	bool				bBuyItem;

	sInven_Set()
	{
		nWeaponId		= 255;
		nClassNum		= LTNULL;
		szName			= LTNULL;
		szDescription	= LTNULL;
		bUsed			= false;
		bSelected		= false;
		hWeaponTx		= LTNULL;
		hSelectTx		= LTNULL;
		nClipInAmmo		= 0;
		nSelectAmmo		= 0;
		ZeroMemory(szExpireTime, 64);
	}
	virtual ~sInven_Set()
	{
	}
};

struct sPriceSet
{
	int nDuration;
	int nPrice;
};

struct sShopWeapon_Set : public sInven_Set
{
	int			nCount;
	sPriceSet	PriceSet[MAX_PRICE_SET];
	
	sShopWeapon_Set()
		: sInven_Set()
	{
		nCount	= 0;
	}
};

struct sChar_Set
{
	char		szClass1[2];
	char		szClass2[3];
	char		szExpireTime[64];

	int			nClassNum;
	char*		szName;
	char*		szDescription;
	int			nItemId;
	HTEXTURE	hNormalTx;
	HTEXTURE	hSelectTx;
	bool		bUsed;
	bool		bSelected;
	uint8		nTeamId;
	bool		bDefaultItem;
	bool		bBuyItem;

	sChar_Set()
	{
		nClassNum		= -1;
		nItemId			= -1;
		szName			= LTNULL;
		szDescription	= LTNULL;
		hNormalTx		= LTNULL;
		hSelectTx		= LTNULL;
		bUsed			= false;
		bSelected		= false;
		nTeamId			= 255;
		ZeroMemory(szExpireTime, 64);
	}

	~sChar_Set()
	{
	}
};

struct sShopChar_Set : public sChar_Set
{
	int			nCount;
	sPriceSet	PriceSet[MAX_PRICE_SET];

	sShopChar_Set()
		: sChar_Set()
	{
		nCount	= 0;
	}
};

class CScreenInventory : public CScreenBase
{
	public :
		CScreenInventory();
		virtual ~CScreenInventory();

		LTBOOL  Build();
		void	Term();

		void	OnFocus(LTBOOL bFocus);
		virtual	void	CreateBackWindow(eScreenID id);

		void			UpdateModel(bool bUsedButton = false);

		LTBOOL	OnMouseMove(int x, int y);
		LTBOOL	OnLButtonDown(int x, int y);
		LTBOOL	OnLButtonUp(int x, int y);

		void	ShowArsenalTab(LTBOOL bShow);

		void	BuildUserInfo( uint16 nPosX, uint16 nPosY, uint16 nWidth, uint16 nHeight );
		void	SetUserInfo( uint32 nGrade, const char* lpszNick,
							 uint32 nKill, uint32 nDeath, uint32 nWin, 
							 uint32 nLose, uint32 nDraw, __int64 nExp, __int64 nMoney, __int64 nCash );
		void	RenderUserInfo();
		
		LTBOOL	OnWheelUp(int x, int y);
		LTBOOL	OnWheelDown(int x, int y);

		// Open 용으로 제작
		//--------------------------------------------------------
		void	ShopAllHide();
		void	ShowShopWeapon(LTBOOL bShow);
		void	ShowShopCharacter(LTBOOL bShow);
		void	ShowShopEtc(LTBOOL bShow);

		void	ShopSubMenuWeapon(LTBOOL bShow);
		void	ShopSubMenuCharacter(LTBOOL bShow);
		void	ShopSubMenuEtc(LTBOOL bShow);

		void	ShopSubMenuWeaponOrder(uint8 nOrder);
		uint8	GetShopSubMenuWeaponOrder();

		LTBOOL			RemoveAllShop();

		LTBOOL	AddShopWeapon( uint8 nWeaponId, int nPrice1, int nPrice2, int nPrice3, int nPrice4 );
		LTBOOL			 AddShopWeapon( sShopWeapon_Set* pItem, int nDuration, int nPrice );
		sShopWeapon_Set* GetShopWeapon( uint8 nWeaponId );
		LTBOOL			 RemoveShopWeapon( uint8 nWeaponId );
		LTBOOL			 RemoveAllShopWeapon();		

		void	ShowShopSubWeapon(uint8 nClass);
		void	RenderShopWpPrice();
		void	SetShopSelectedWpTexPos( uint8 nClass, int nIndex, uint32 dwParam1 );
		void	ShopSubMenuCharOrder(uint8 nOrder);
		uint8	GetShopSubMenuCharOrder();
		void	RenderShopChPrice();
		void	ShowShopSubChar(uint8 nClass);
		
		LTBOOL	AddShopCharTab( int nId, uint8 nTeamId, int nClass, int nPrice1, int nPrice2, int nPrice3, int nPrice4 );
		LTBOOL			AddShopCharTab( sShopChar_Set* pItem, int nDuration, int nPrice );
		sShopChar_Set*	GetShopCharTab( uint8 nId, uint8 nClass );
		LTBOOL			RemoveShopCharTab( int nId, int nClass );
		LTBOOL			RemoveAllShopChar();

		void	SetShopSelectedChTexPos(uint8 nClass, int nIndex, uint32 dwParam1 );
		void	BuyWeaponInfo();
		void	BuyCharInfo();
		LTBOOL	ItemBuy( );
		// 여기까지 상점

		void	ShowWeapon(LTBOOL bShow);
		void	ShowCharacter(LTBOOL bShow);
		void	ShowEtc(LTBOOL bShow);

		void	SubMenuWeapon(LTBOOL bShow);
		void	SubMenuCharacter(LTBOOL bShow);
		void	SubMenuEtc(LTBOOL bShow);
		void	SubMenuWeaponOrder(uint8 nOrder);
		uint8	GetSubMenuWeaponOrder();

		LTBOOL	WeaponThrow( int nIndex );
		LTBOOL  CharacterThrow( int nIndex );
		void	ShowMainCharacter(LTBOOL bShow);

		LTBOOL	AddWeapon(uint8 nWeaponId, int nDuration);
		LTBOOL		AddWeapon( sInven_Set* pItem );
		sInven_Set*	GetWeapon(uint8 nWeaponId);
		LTBOOL		RemoveWeapon(uint8 nWeaponId);
		LTBOOL		RemoveAllWeapon();

		void		SetDefaultWeapon( int nClass );
		void		SetDefaultCharacter( int nTeamID );
		
		void	SetGauge(int nDamage, int nShoot, int nHit, int nReact, int nClip, int nSltAmmo);
		void	UpdateGaugeAni();
		void	ShowSubWeapon(uint8 nClass);
		void	RenderSubWeaponTitle();
		void	SetUsedTexturePos( uint8 nClass, int nId, uint32 dwParam1 );
		void	SetSelectedTexturePos( uint8 nClass, int nId, uint32 dwParam1 );
		void	RenderGaugeText();
		// char sub set
		void	ShowSubChar(uint8 nClass);
		
		LTBOOL	AddCharTab( int nId, uint8 nTeamId, int nDuration, int nClass );
		LTBOOL		AddCharTab( sChar_Set* pItem );
		sChar_Set*	GetCharTab( uint8 nId, uint8 nClass );
		LTBOOL	RemoveCharTab( int nId, int nClass );
		LTBOOL	RemoveAllChar();

		void	RenderSubCharTitle();
		void	SetCharSelectedTexPos(uint8 nClass, int nIndex, uint32 dwParam1 );
		void	SetCharUsedTexPos( uint8 nClass, int nIndex, uint32 dwParam1 );
		// Shop Main Set
		void	SetShop(bool bShop)	{ m_IsShop = bShop; }
		void	ShowMainShop();
		void	ShopOrInvenInitialize();

		uint8	GetCurrentUsedWeapon(uint8 nClass);	
		uint8	GetCurrentUsedModel(uint8 nTeamId);

		void	SelectShopWeaponClear();
		void	SelectShopCharClear();
		void	SelectInvenWeaponClear();
		void	SelectInvenCharClear();
		void	SelectAllClear();

		void	DetailWeaponClear();
		void	DetailCharClear();
		//--------------------------------------------------------
		
		//Net-----------------------------------------------------
		void	ExitShop();
		void	SetGameMoney( __int64 nMoney );
		void	EquipWeapon( int nIndex );
		void	EquipCharacter( int nIndex, int nTeam );
		void	AddEquipWeapon( int nClass, int nWeaponId );
		void	AddEquipCharacter( int nIndex, int nTeam );
		void	SetRanking();
		//--------------------------------------------------------

	public :
		sInventory			m_InventoryInfo;

	protected :
		uint32  OnCommand(uint32 dwCommand, uint32 dwParam1, uint32 dwParam2);
		LTBOOL	Render(HSURFACE hDestSurf);
		LTBOOL	RenderCloud();
		LTBOOL	OnEnter();
		LTBOOL	OnEscape();
		
		LTBOOL	OnScrollUp( int x = 0, int y = 0 );
		LTBOOL	OnScrollDown( int x = 0, int y = 0 );
		LTBOOL	OnShopScrollUp( int x = 0, int y = 0 );
		LTBOOL	OnShopScrollDown( int x = 0, int y = 0 );
		LTBOOL	UpdateInvView( LTFLOAT fStartYPos, LTBOOL bShow );

		static void DiscardWeaponCallback( LTBOOL bReturn, void* pData );
		static void DiscardCharacterCallback( LTBOOL bReturn, void* pData );

		HTEXTURE			m_hTxCloud;
		LTPoly_GT4			m_hPolyCloud;

		int					m_nCurModelId[2];
		int					m_nCurHeadSkinId[2];
		int					m_nCurBodySkinId[2];
		
		//LTRect				m_rcRedTeam;
		//LTRect				m_rcBlueTeam;

		CPopUpWeaponDetail*	m_pPopUpWeaponDetail;		

		// Inventory		
		CLTGUIButton*		m_pBtnCheckRed;
		CLTGUIButton*		m_pBtnCheckBlue;
		//CLTGUIFrame*		m_pFrameCharBack;
		LTBOOL				m_bClicked;		
		
		HTEXTURE			m_hGradeIcon;
		LTPoly_GT4			m_PolyGrade;

		
		// My Character
		CLTGUIButton*		m_pBtnCheckArsenal;
		CLTGUIButton*		m_pBtnCheckPresent;

		// Arsenal
		CLTGUIFrame*		m_pFrameItem[6];
		CLTGUIFrame*		m_pFrameItemTitle[6];
		CLTGUIFrame*		m_pFrameUsed[4];
		CLTGUIFrame*		m_pFrameItemBack[6];
		CLTGUIButton*		m_pBtnUseItem[6];
		CLTGUIButton*		m_pBtnDetailItem[6];
		CLTGUIDragButton*	m_pBtnItemScroll;
		CLTGUIButton*		m_pBtnItemScrollUp;
		CLTGUIButton*		m_pBtnItemScrollDown;
		

		//--------------------------------------------------
		// Open 용으로 새로 만든 부분
		//--------------------------------------------------
		// Shop
		CLTGUIFrame*		m_pFrmShopBack;		
		CLTGUIDragButton*	m_pBtnShopScroll;
		CLTGUIButton*		m_pBtnShopScrollUp;
		CLTGUIButton*		m_pBtnShopScrollDown;
		CLTGUIButton*		m_pBtnShopBuy;
		CLTGUIButton*		m_pBtnShopPresent;
		CLTGUIButton*		m_pCheckShopWeapon;
		CLTGUIButton*		m_pCheckShopChar;
		CLTGUIButton*		m_pCheckShopEtc;
		CLTGUIButton*		m_pCheckShopWOrder[5];
		CLTGUIButton*		m_pBtnShopWpSlot[5];
		CLTGUIFrame*		m_pFrmShopWpSlot[5];
		CSAD3DText*			m_pFontWpPrice[5][MAX_PRICE_SET];
		CLTGUIFrame*		m_pFrmShopSltWeapon;
		CSAD3DText*			m_pFontShopWpName[5];

		CLTGUIButton*		m_pCheckShopChOrder[3];
		CLTGUIButton*		m_pBtnShopChSlot[4];
		CLTGUIFrame*		m_pFrmShopChSlot[4];
		CSAD3DText*			m_pFontChPrice[4][MAX_PRICE_SET];
		CLTGUIFrame*		m_pFrmShopSltChar;
		CSAD3DText*			m_pFontShopChName[4];

		// Inven
		CLTGUIFrame*		m_pFrmInvenBack;
		CLTGUIButton*		m_pBtnInvenSlot[9];
		CLTGUIFrame*		m_pFrameInvenSlot[9];
		CLTGUIFrame*		m_pFrameSelectedWeapon[4];
		CLTGUIFrame*		m_pFrameSelect;
		CLTGUIFrame*		m_pFrameInvenUsed[4];
		CLTGUIFrame*		m_pFrameWeaponDetail;
		CSAD3DText*			m_pFontWeaponTitle[9];
		CSAD3DText*			m_pFontWeaponName;
		CSAD3DText*			m_pFontWeaponDur;
		
		// Weapon Gauge
		CLTGUIFrame*		m_pFrameWeaponInfo[4];
		CLTGUIFrame*		m_pFrameGaugeBar[4];
		CSAD3DText*			m_pFontWeaponInfo[4];
		CSAD3DText*			m_pFontWeaponClip;
		CSAD3DText*			m_pFontWeaponDesc;
		float				m_fDamage;
		float				m_fShoot;
		float				m_fHit;
		float				m_fReact;		
		float				m_fDeltaDamage;
		float				m_fDeltaShoot;
		float				m_fDeltaHit;
		float				m_fDeltaReact;
		HLTSOUND			m_hGaugeSound;		

		// character tab
		CLTGUIButton*		m_pBtnCharSlot[6];
		CLTGUIFrame*		m_pFrmCharSlot[6];
		CLTGUIFrame*		m_pFrmSelectCharSlot;
		CLTGUIFrame*		m_pFrmUsedCharIcon[3];
		CLTGUIButton*		m_pCheckCharAll;
		CLTGUIButton*		m_pCheckCharChar;
		CLTGUIButton*		m_pCheckCharAccessory;
		CSAD3DText*			m_pFontCharName[6];

		// character Desc
		CLTGUIFrame*		m_pFrmDetailBack;
		CLTGUIFrame*		m_pFrmSelectCharDetail;
		CSAD3DText*			m_pFontCharDesc;
		CSAD3DText*			m_pFontCharDuration;
		CSAD3DText*			m_pFontCharTitle;



		CLTGUIFrame*		m_pFrameDetail;

		CSAD3DText*			m_pInfoNickCap;
		CSAD3DText*			m_pInfoNick;
		CSAD3DText*			m_pInfoGrade;
		CSAD3DText*			m_pInfoWinPercent;
		CSAD3DText*			m_pInfoKillDeath;
		CSAD3DText*			m_pInfoExp;
		CSAD3DText*			m_pInfoGameMoney;
		CSAD3DText*			m_pInfoGradeCap;
		CSAD3DText*			m_pInfoWinPercentCap;
		CSAD3DText*			m_pInfoKillDeathCap;
		CSAD3DText*			m_pInfoExpCap;
		CSAD3DText*			m_pChannelInfo;

		CTextScroll*		m_pTextScroll;
		HTEXTURE			m_hUIBottomL;
		HTEXTURE			m_hUIBottomR;
		LTPoly_GT4			m_PolyUIBottomL;
		LTPoly_GT4			m_PolyUIBottomR;

		CLTGUIButton*		m_pBtnExit;

		CLTGUIButton*		m_pCheckMWeapon;
		CLTGUIButton*		m_pCheckMChar;
		CLTGUIButton*		m_pCheckMEtc;

		CLTGUIButton*		m_pCheckSubWeapon[5];

		CLTGUIButton*		m_pBtnUse;
		CLTGUIButton*		m_pBtnThrow;

		CLTGUIButton*		m_pCheckInven;
		CLTGUIButton*		m_pCheckPresent;

		CLTGUIDragButton*	m_pBtnScroll;
		CLTGUIButton*		m_pBtnScrollUp;
		CLTGUIButton*		m_pBtnScrollDown;

		int					m_SelectedWeaponId[4];
		int					m_SelectedCharItemId[3];

		bool				m_IsShop;

		CPopUpItemBuy*		m_pPopupItemBuy;

		typedef std::vector<sInven_Set*> WeaponInvenList;
		WeaponInvenList		m_Invens;
		typedef std::vector<sChar_Set*> CharInvenList;
		CharInvenList		m_Chars;
		// Shop
		typedef std::vector<sShopWeapon_Set*> ShopWeaponList;
		ShopWeaponList		m_ShopWeapons;
		typedef std::vector<sShopChar_Set*> ShopCharList;
		ShopCharList		m_ShopChars;
};

extern CScreenInventory* g_pScreenInventory;
#endif // _SCREEN_INVENTORY_H_