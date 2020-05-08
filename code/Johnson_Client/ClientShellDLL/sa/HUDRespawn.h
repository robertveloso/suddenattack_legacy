// ----------------------------------------------------------------------- //
//
// MODULE  : HUDRespawn.h
//
// PURPOSE : HUDItem to display hiding icon
//
// (c) 2001 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#ifndef __HUD_RESPAWN_H
#define __HUD_RESPAWN_H

#include "HUDMeter.h"

class CHUDRespawnBar : public CHUDMeter
{
	public: // Methods...

		CHUDRespawnBar();
	
		void	Update();
		void	UpdateLayout();
		void	ScreenDimChanged();
};


//******************************************************************************************
//** HUD Respawn display
//******************************************************************************************
class CHUDRespawn : public CHUDItem
{
public:
	CHUDRespawn();

    LTBOOL      Init();
	void		Term();

    void        Render();
    void        Update();

    void        UpdateLayout();
	void		ScreenDimChanged();

private:

	LTFLOAT		m_bDraw;

	float		m_fDuration;
	bool		m_bReady;
	
	HTEXTURE		m_hTex;
	LTPoly_GT4		m_Poly;
	
	CHUDRespawnBar		m_RespawnBar;	
	CSAD3DText*		m_pGodMode;
};

#endif