#ifndef __HUD_CHAT_H
#define __HUD_CHAT_H

#include "HUDItem.h"
#include "LTGUIMgr.h"
#include "../../Shared/LTPoly.h"

class CHUDChatInput : public CHUDItem
{
public:
	CHUDChatInput();	

	LTBOOL		Init();
	void		Term();

	void		InitGame();
	void        Render();
    void        Update();

	void		ScreenDimChanged();
	void		Show( eChatMsgType eType = kMsgChat );
	void		Hide();

	void		OnChar();
	void		OnEnter();
	void		OnEscape();

	LTBOOL		IsVisible() { return m_bVisible; }

protected:
	LTBOOL		EditCommand( const char* lpszChatMessage );
	void		CheckDoBae();
	LTBOOL		CheckSameRoom( const char* lpszNick );
	
	eChatMsgType	m_eChatMsgType;
	CLTGUIEditBox*	m_pChatEdit;
	LTBOOL			m_bVisible;
	
	HTEXTURE		m_hChatBack;
	
	HTEXTURE		m_hTypeTex;
	HTEXTURE		m_hChatTypeAll;
	HTEXTURE		m_hChatTypeTeam;
	HTEXTURE		m_hChatTypeGhost;
	HTEXTURE		m_hChatTypeTeamGhost;

	LTPoly_GT4		m_Poly;
	LTPoly_GT4		m_PolyType;
	
	int					m_nChatCount;
	float				m_fChatTime;
	float				m_fDoBae;
	LTBOOL				m_bDisableChat;
};

#endif