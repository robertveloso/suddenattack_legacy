#ifndef _POPUP_ITEM_BUY_H_
#define _POPUP_ITEM_BUY_H_

#include "ScreenBase.h"
#include "../ClientShellShared/HanText.h"
#include "PopupScreenBase.h"

struct sShopWeapon_Set;
struct sShopChar_Set;

class CPopUpItemBuy : public CPopScreenBase
{
	public :
		CPopUpItemBuy();
		virtual ~CPopUpItemBuy();

		void	Init( CLTGUICommandHandler* pCommandHandler );
		void	Term();
		void	OnFocus(LTBOOL bFocus);
		void	CreateBackeFrame( eScreenID nScreen );
		void	Render();

		void	ShowItemWeapon(LTBOOL bShow);
		void	ShowItemChar(LTBOOL bShow);
		void	SetWeaponItemInfo( sShopWeapon_Set* pWeapon );
		void	SetCharItemInfo( sShopChar_Set* pCharacter );
		void	CheckButtonOrder(uint8 nOrder);
		uint8	GetButtonOrder();
		
	protected :
		uint32	OnCommand(uint32 dwCommand, uint32 dwParam1, uint32 dwParam2);

		CSAD3DText*			m_pFontPrice[MAX_PRICE_SET];
		CSAD3DText*			m_pFontTitle;
		CLTGUIButton*		m_pBtnOk;
		CLTGUIButton*		m_pBtnCancel;
		CLTGUIButton*		m_pCheckPrice[MAX_PRICE_SET];
		CLTGUIFrame*		m_pFrmItemWp;
		CLTGUIFrame*		m_pFrmItemCh;
		CLTGUIFrame*		m_pFrmItemChBack;
		CLTGUIFrame*		m_pFrmItemWpBack;

		CLTGUICommandHandler*	m_pCommandHandler;


		sShopWeapon_Set*		m_pWeaponSet;
		sShopChar_Set*			m_pCharacterSet;
};

extern CPopUpItemBuy*	g_pPopupItemBuy;

#endif // _POPUP_ITEM_BUY_H_	