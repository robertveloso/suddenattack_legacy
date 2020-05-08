#ifndef __HUD_GAMESTATES_H__
#define __HUD_GAMESTATES_H__

#include "HUDItem.h"

class CHUDGameState : public CHUDItem
{
public:
	CHUDGameState();
	

    virtual LTBOOL      Init();
	virtual void		Term();

    virtual void        Render();
	virtual void		Update();
    
	void				ScreenDimChanged();

	void				Show( bool bVisible );
	bool				IsVisible() {return m_bVisible;}

	void				DrawNumber( int nPosX, int nPosY, int nNumber );
	void				DrawNumber( int nPosX, int nPosY, float fNumber );

	void				Set( int nKey );

	//[MURSUM] ShowFireInfo
	void				InitFireSendCount();
	void				AddFireSendCount();
	
protected:
	CSAD3DText*	m_pNumber;
	CSAD3DText*	m_pDot;
	CSAD3DText*	m_pMinus;
	CSAD3DText*	m_pText;

	bool		m_bVisible;

	char		m_szNumber[256];

	float		m_fLastFrameUpdate;
	int			m_nAccumFrame;
	int			m_nFrameRate;
	
	int			m_nBasePosX;
	int			m_nBasePosY;
	
	//[MURSUM] ShowFireInfo
	int			m_nSendCount;
	int			m_nSendCountPosX;
	int			m_nSendCountPosY;
};



#endif