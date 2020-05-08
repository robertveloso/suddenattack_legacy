#ifndef _POPUP_SCREEN_CREATE_ROOM_H_
#define _POPUP_SCREEN_CREATE_ROOM_H_

#include "ScreenBase.h"
#include "..\\..\\Libs\\LTGUIMgr\\ltguiwindow.h"
#include "PopupScreenBase.h"


class CSaListPlayers;

class CPopScreenCreateRoom : public CPopScreenBase
{
public : 
	CPopScreenCreateRoom();
	virtual ~CPopScreenCreateRoom();

	void		Init( CLTGUICommandHandler*	pCommandHandler );
	void		Render();

	LTBOOL		OnLButtonUp(int x, int y);
	void		OnFocus(LTBOOL bFocus);
	void		CreateRoom();
	
protected:
	LTBOOL		OnEnter();
	LTBOOL		OnEscape();
    uint32		OnCommand(uint32 dwCommand, uint32 dwParam1, uint32 dwParam2);
	void		SetDefaultRoomName();

	//Back
	HTEXTURE				m_hBack;
	LTPoly_GT4				m_Poly;

	HTEXTURE				m_hPasswdBack;
	LTPoly_GT4				m_PolyPasswd;

	//EditBox
	CLTGUIEditBox*			m_pEditTitle;
	CLTGUIEditBox*			m_pEditPassword;

	// Button
	CLTGUIButton*			m_pBtnOK;
	CLTGUIButton*			m_pBtnCancel;
	CLTGUIButton*			m_pBtnCombo;
	CLTGUIButton*			m_pCkPassword;

	//List
	CSaListPlayers*			m_pPlayersList;
	CSAD3DText*				m_pPlayers;

	CLTGUICommandHandler*	m_pCommandHandler;
};


class CSaListPlayers : public CLTGUIListView
{
public:
	CSaListPlayers();
	virtual ~CSaListPlayers();

	LTBOOL			Create( uint16 nPosX, uint16 nPosY,
							uint16 nWidth, uint16 nHeight,
							uint16 nItemHeight,
							CLTGUICommandHandler* pCommandHandler = LTNULL,
							LTBOOL	bDrawBack = LTFALSE,
							uint32	nBackColor = 0xFF000000,
							uint32	nBorderColor = 0xFFFFFFFF );
	
	LTBOOL			AddItem( const char* lpszItem );	
	LTBOOL			OnLButtonDown(int x, int y);
	void			RenderItemText();
	const char*		GetSelectedPlayersData();
	int				GetPlayers();

protected:
	LTBOOL			RenderItem( void* pListItem, const LTIntPt* ptItemPos,
								int32 nIndex, bool bSelect, bool bOver );
					
	CSAD3DText*		m_pPlayersList;
	LTPoly_GT4		m_PolyBar;
};

#endif // _POPUP_SCREEN_CREATE_ROOM_H_


