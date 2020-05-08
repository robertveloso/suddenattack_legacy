#ifndef __MURSUM_POPUP_SEARCHRESULT_H__
#define __MURSUM_POPUP_SEARCHRESULT_H__

#include "PopupScreenBase.h"
#include "..\\..\\Libs\\LTGUIMgr\\ltguiwindow.h"

class CPopupSearchResult : public CPopScreenBase
{
public : 
	CPopupSearchResult();
	virtual ~CPopupSearchResult();

	void		Init( CLTGUICommandHandler*	pCommandHandler );
	void		CtrlShow( LTBOOL bSuccess, const char* lpszMsg, int nChannelIndex, int nRoomIndex, LTBOOL bScreenMain );
	void		Render();
	void		OnFocus(LTBOOL bFocus);

protected:
	LTBOOL		OnEnter();
	LTBOOL		OnEscape();
    uint32		OnCommand(uint32 dwCommand, uint32 dwParam1, uint32 dwParam2);
			
	CLTGUIButton*		m_pBtnOK;
	CLTGUIButton*		m_pBtnJoin;
	CLTGUIButton*		m_pBtnAdd;
	HTEXTURE			m_hBack;	
	CSAD3DText*			m_pText;
	int					m_nChannelIndex;
	int					m_nRoomIndex;
	CLTGUICommandHandler*	m_pCommandHandler;

	int					m_nBtnPosY;
	int					m_nBtnPos_1_1;
	int					m_nBtnPos_2_1;
	int					m_nBtnPos_2_2;
	int					m_nBtnPos_3_1;
	int					m_nBtnPos_3_2;
	int					m_nBtnPos_3_3;
};

#endif // _POPUP_SCREEN_CREATE_ROOM_H_


