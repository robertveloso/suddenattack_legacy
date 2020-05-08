#ifndef _SCREEN_SHOP_H_
#define _SCREEN_SHOP_H_

#include "ScreenBase.h"
#include "LayoutMgr.h"

class CScreenShop : public CScreenBase
{
	public :
		CScreenShop();
		virtual ~CScreenShop();

		LTBOOL  Build();
		void	Term();
		
		void	OnFocus(LTBOOL bFocus);
		virtual	void	CreateBackWindow(eScreenID id);
	protected :
		uint32  OnCommand(uint32 dwCommand, uint32 dwParam1, uint32 dwParam2);
};

#endif //	_SCREEN_SHOP_H_