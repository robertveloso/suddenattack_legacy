#ifndef __MURSUM_POPUP_SEARCHUSER_H__
#define __MURSUM_POPUP_SEARCHUSER_H__

#include "PopupScreenBase.h"
#include "..\\..\\Libs\\LTGUIMgr\\ltguiwindow.h"

class CPopupSearchUser : public CPopScreenBase
{
public : 
	CPopupSearchUser();
	virtual ~CPopupSearchUser();

	void		Init( CLTGUICommandHandler*	pCommandHandler );
	void		Render();
	void		OnFocus(LTBOOL bFocus);

	LTBOOL		SendSearchUser();

protected:
	LTBOOL		OnEnter();
	LTBOOL		OnEscape();
    uint32		OnCommand(uint32 dwCommand, uint32 dwParam1, uint32 dwParam2);
			
	CLTGUIEditBox*		m_pEditNickName;
	CLTGUIButton*		m_pBtnOK;
	CLTGUIButton*		m_pBtnCancel;
	HTEXTURE			m_hBack;	
	CLTGUICommandHandler*	m_pCommandHandler;
};

#endif // _POPUP_SCREEN_CREATE_ROOM_H_


