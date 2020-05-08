#ifndef __MURSUM_POPUP_USERINFO_H__
#define __MURSUM_POPUP_USERINFO_H__

#include "PopupScreenBase.h"
#include "..\\..\\Libs\\LTGUIMgr\\ltguiwindow.h"

class CPopupUserInfo : public CPopScreenBase
{
public : 
	CPopupUserInfo();
	virtual ~CPopupUserInfo();

	void		Init( CLTGUICommandHandler*	pCommandHandler );
	void		Render();
	void		OnFocus(LTBOOL bFocus);

	LTBOOL	    OnLButtonDown(int x, int y);
		
	void		SetUserInfo( const char* lpszNick, uint32 nGrade,
							 uint32 nKill, uint32 nDeath, 
							 uint32	nWin, uint32 nLose, uint32 nDraw,
							 __int64 nExp, 
							 uint8 nA_ModelID, uint8 nA_HeadSkinID, uint8 nA_BodySkinID,
							 uint8 nB_ModelID, uint8 nB_HeadSkinID, uint8 nB_BodySkinID );

	void		UpdateModel( int nModelID, int HeadSkinID, int nBodySkinID );
	
	HOBJECT				m_hCharObject;

protected:
	LTBOOL		OnEnter();
	LTBOOL		OnEscape();
    uint32		OnCommand(uint32 dwCommand, uint32 dwParam1, uint32 dwParam2);
			
	CLTGUIButton*		m_pBtnOK;
	CLTGUIButton*		m_pCkRedTeam;
	CLTGUIButton*		m_pCkBlueTeam;

	HTEXTURE			m_hBack;	
	CSAD3DText*			m_pText;	
	CLTGUICommandHandler*	m_pCommandHandler;


	CSAD3DText*			m_pCapText;
	CSAD3DText*			m_pInfoText;

	CSAD3DText*			m_pRedTeamText;
	CSAD3DText*			m_pBlueTeamText;

	LTRect				m_rcRedTeam;
	LTRect				m_rcBlueTeam;

	HTEXTURE			m_hGradeIcon;
	LTPoly_GT4			m_PolyGrade;

	int					m_nA_ModelID;
	int					m_nA_HeadSkinID;
	int					m_nA_BodySkinID;
	int					m_nB_ModelID;
	int					m_nB_HeadSkinID;
	int					m_nB_BodySkinID;

	char				m_szTemp[1024];
};

#endif // _POPUP_SCREEN_CREATE_ROOM_H_


