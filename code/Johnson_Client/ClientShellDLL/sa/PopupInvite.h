#ifndef __MURSUM_POPUP_INVITE_H__
#define __MURSUM_POPUP_INVITE_H__

#include "PopupScreenBase.h"
#include "..\\..\\Libs\\LTGUIMgr\\ltguiwindow.h"

typedef struct _tagINVITE_USER
{
	int		nGrade;
	char	szNickName[MAX_NICK_LEN];
	LTBOOL	bChecked;
} T_INVITE_USER, *T_INVITE_USERptr;


class CSaListInviteUser : public CLTGUIListView
{
public:
	CSaListInviteUser();
	virtual ~CSaListInviteUser();

	LTBOOL			Create( uint16 nPosX, uint16 nPosY,
							uint16 nWidth, uint16 nHeight,
							uint16 nItemHeight,
							CLTGUICommandHandler* pCommandHandler = LTNULL,
							LTBOOL	bDrawBack = LTFALSE,
							uint32	nBackColor = 0xFF000000,
							uint32	nBorderColor = 0xFFFFFFFF );

	void			AddUser( T_INVITE_USERptr pUser );
	LTBOOL			OnLButtonDown(int x, int y);
	int				SendInvitePacket();
	
protected:
	LTBOOL			RenderItem( void* pListItem, const LTIntPt* ptItemPos,
								int32 nIndex, bool bSelect, bool bOver );
	void			RenderItemText();
	
	CSAD3DText*		m_pNickList;

	HTEXTURE		m_hCheck;
	LTPoly_GT4		m_PolyCheck;
	LTPoly_GT4		m_PolyGrade;
};


class CPopupInvite : public CPopScreenBase
{
public : 
	CPopupInvite();
	virtual ~CPopupInvite();

	void		Init( CLTGUICommandHandler*	pCommandHandler );
	void		Render();
	void		OnFocus(LTBOOL bFocus);
	
	void		AddLobbyUser( const char* lpszNick, int nGrade );
	void		ClearList();
	
	LTBOOL		OnWheelUp(int x, int y);
	LTBOOL		OnWheelDown(int x, int y);

protected:
	LTBOOL		OnEnter();
	LTBOOL		OnEscape();
	uint32		OnCommand(uint32 dwCommand, uint32 dwParam1, uint32 dwParam2);
			
	CLTGUIButton*		m_pBtnInvite;
	CLTGUIButton*		m_pBtnCancel;
	HTEXTURE			m_hBack;
	CSaListInviteUser*	m_pInviteUserList;

	CLTGUIDragButton*		m_pBtnScroll;
	CLTGUIButton*			m_pBtnScrollUp;
	CLTGUIButton*			m_pBtnScrollDown;
	
	CLTGUICommandHandler*	m_pCommandHandler;
};


class CPopupInviteMsg : public CPopScreenBase
{
public : 
	CPopupInviteMsg();
	virtual ~CPopupInviteMsg();

	void		Init( CLTGUICommandHandler*	pCommandHandler );
	void		Render();
	void		OnFocus(LTBOOL bFocus);

	void		CtrlShow( const char* lpszNick, int nRoomIndex, const char* lpszPass );

protected:
	LTBOOL		OnEnter();
	LTBOOL		OnEscape();
	uint32		OnCommand(uint32 dwCommand, uint32 dwParam1, uint32 dwParam2);
			
	CLTGUIButton*		m_pBtnOK;
	CLTGUIButton*		m_pBtnCancel;
	HTEXTURE			m_hBack;
	CSAD3DText*			m_pText;

	int					m_nRoomIndex;
	char				m_szPassword[MAX_PASS_LEN];

	CLTGUICommandHandler*	m_pCommandHandler;
};


#endif // _POPUP_SCREEN_CREATE_ROOM_H_


