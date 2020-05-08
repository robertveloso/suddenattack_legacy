// ----------------------------------------------------------------------- //
//
// MODULE  : HUDCrosshair.h
//
// PURPOSE : HUDItem to display crosshair
//
// (c) 2001 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#ifndef __HUD_CROSSHAIR_H
#define __HUD_CROSSHAIR_H

#include "HUDItem.h"
#define  MAXPOLYCOUNT 6

//******************************************************************************************
//** HUD crosshair
//******************************************************************************************
class CHUDCrosshair : public CHUDItem
{
public:
	CHUDCrosshair();
	

    LTBOOL	Init();
	void	Term();

    void	Render();
    void	Update();

	void	UpdateLayout();

	void	RenderScope();

	void	Toggle() {m_bEnabled = !m_bEnabled;}

	void	SetStyle(uint8 style);
	void	ScreenDimChanged();

protected:

	LTPoly_GT4		m_Poly[MAXPOLYCOUNT];
	
	float			m_fScale;
	
	//activation data
	HOBJECT		m_hObj;
	uint16		m_nString;

	bool		m_bEnabled;
	bool		m_bArmed;

	bool		m_bCamActive;

	HTEXTURE	m_hAccurate;
	HTEXTURE	m_hInaccurate;

	float		m_fInaccurateSz;

	uint8		m_style;
	
	bool		m_bCanActivateTarget;
	uint8		m_nTargetTeam;

	
	HTEXTURE	m_hScpe[4];
	CSAD3DText*	m_pFont;
};

#endif