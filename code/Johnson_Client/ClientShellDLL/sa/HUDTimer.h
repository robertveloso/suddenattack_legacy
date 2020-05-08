#ifndef __MURSUM_HUD_TIMER_H__
#define __MURSUM_HUD_TIMER_H__

#include "HUDItem.h"

#define TIMER_DIGIT			4
#define BOMB_TIMER_DIGIT	6

class CHUDTimer : public CHUDItem
{
public:
	CHUDTimer();

    virtual LTBOOL      Init();
	virtual void		Term();

    virtual void        Render();
    virtual void        Update();

	void				SetTime(LTFLOAT fTime, bool bPause);
	float				GetTime();
	
	void				Toggle()				{m_bDraw = !m_bDraw;}
	void				SetDraw( bool bDraw )	{ m_bDraw = bDraw; }
	bool				GetDraw()				{ return m_bDraw; }
	virtual void		ScreenDimChanged();
	void				SetBasePos(float x, float y) {m_fBasePosX = x; m_fBasePosY = y;}
			
protected:
    bool		m_bDraw;
	float		m_fTime;
	bool		m_bPause;
	
	float		m_fBasePosX;
	float		m_fBasePosY;
	HTEXTURE	m_hTimerBack;
	LTPoly_GT4	m_hPolyTimerBack;

	HTEXTURE	m_hTimerNumber[10];
	LTPoly_GT4	m_hPolyTimerNumber[TIMER_DIGIT];

	HTEXTURE	m_hTimerSeprator;
	LTPoly_GT4	m_hPolyTimerSeperator;
	LTBOOL		m_bSeperatorBlink;
	
	int			m_nMinute_10;
	int			m_nMinute_1;
	int			m_nSecond_10;
	int			m_nSecond_1;
};


class CHUDTimeBombTimer : public CHUDTimer
{
public:
	CHUDTimeBombTimer();
	virtual LTBOOL      Init();
	virtual void		InitTimer();
	virtual void		Term();
    virtual void        Render();
    virtual void        Update();
	virtual void		ScreenDimChanged();

	LTBOOL				IsTimeBombDefused(){return m_bDefused;}
	VOID				SetTimeBombDefused(LTBOOL bDefused){m_bDefused = bDefused;}
	
	VOID				SetDetonateTimer(LTFLOAT fDetonateTimer);
	
protected:
	LTBOOL		m_bDefused;
	LTPoly_GT4	m_hPolyTimerNumber[BOMB_TIMER_DIGIT];		// Overrided
	LTPoly_GT4	m_hPolyTimerSeperator[2];					// Overrided

	int 		m_nMillSec_10;
	int			m_nMillSec_1;
};

#endif
