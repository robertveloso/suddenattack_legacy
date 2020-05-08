#ifndef __MURSUM_HUD_PLAYERSTATE_H__
#define __MURSUM_HUD_PLAYERSTATE_H__

#include "HUDItem.h"

class CHUDPlayerState : public CHUDItem
{
public:
	CHUDPlayerState();

    LTBOOL      Init();
	void		Term();

    void        Render();
    void        Update();
	void		ScreenDimChanged();

	void		ShowAttachBar();
	void		AttachPlayer( const char* lpszNick, int nAttachState );
	void		HideAttachBar();
	LTBOOL		IsAttachBarVisible();
	int			GetAttachState() { return m_nAttachState; }
		
private:
	void				RenderTimeBombNotifyIcon();
	void				ResetTimeBombNotifyIcon();

	HTEXTURE			m_hTimeBombNotify;
	LTPoly_GT4			m_PolyTimeBombNotify;
	LTFLOAT				m_fLastTimeBombNotifyBlinkTime;


	LTPoly_GT4			m_PolyAttachBar;
	LTBOOL				m_bShowAttachBar;
	CSAD3DText*			m_pAttachedPlayer;
	CSAD3DText*			m_pHelpText;
	LTBOOL				m_bAttached;
	int					m_nAttachState;
};

#endif
