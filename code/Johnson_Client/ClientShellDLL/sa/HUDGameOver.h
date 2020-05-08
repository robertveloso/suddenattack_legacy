#ifndef __MURSUM_HUD_GAMEOVER_H__
#define __MURSUM_HUD_GAMEOVER_H__

#include "HUDItem.h"

class CHUDGameOver : public CHUDItem
{
public:
	CHUDGameOver();
	~CHUDGameOver();

    LTBOOL      Init();
	void		Term();

	bool		IsVisible();
    void        Render();
    void        Update();

	void		ShowGameOver( int nResult, bool bRestartRound, float fDelayTime=0.0f );
	void		HideGameOver();

	void		UpdateAni();
	void		ScreenDimChanged();

private:
	bool		m_bVisible;	
	bool		m_bRestartRound;
	
	HTEXTURE	m_hTexWin;
	HTEXTURE	m_hTexDefeat;
	HTEXTURE	m_hTexDraw;

	HTEXTURE	m_hTexture;
	LTPoly_GT4	m_Poly;
	
	float		m_fStartTime;
	float		m_fDeltaTime;
	
	bool		m_bFirstUpdate;
	int			m_nResult;
};

#endif //#ifndef __MURSUM_HUD_GAMEOVER_H__
