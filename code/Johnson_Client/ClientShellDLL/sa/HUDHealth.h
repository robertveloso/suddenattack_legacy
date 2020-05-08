// ----------------------------------------------------------------------- //
//
// MODULE  : HUDHealth.h
//
// PURPOSE : HUDItem to display player health
//
// (c) 2001 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#ifndef __HUD_HEALTH_H
#define __HUD_HEALTH_H

#include "HUDItem.h"
#include "HUDBar.h"


//******************************************************************************************
//** HUD Health display
//******************************************************************************************
class CHUDHealth : public CHUDItem
{
public:
	CHUDHealth();

    LTBOOL      Init();
	void		Term();

    void        Render();
    void        Update();

    void        UpdateLayout();
	void		ScreenDimChanged();

private:

    LTBOOL		m_bUseHealthText;
    LTIntPt		m_HealthTextOffset;
	LTIntPt		m_ArmorTextOffset;


	LTPoly_GT4	m_Poly[2];

//--------------------------------------------------------------------------
	//  [04/02/2004] GameHi

	LTPoly_GT4	m_NumPoly[9];
	HTEXTURE	m_hNumber[10];
	int			m_nNum[9];
	
	LTIntPt     m_GameHiArmorBasePos;
	LTIntPt		m_GameHiArmorSize;
	LTPoly_GT4	m_GameHiArmorPoly;
	HTEXTURE	m_GameHiArmorIcon;

	LTIntPt     m_GameHiHealthBasePos;
	LTIntPt		m_GameHiHealthSize;
	LTPoly_GT4	m_GameHiHealthPoly;
	HTEXTURE	m_GameHiHealthIcon;	

	
	//[SPIKE] Air
	LTIntPt     m_AirBasePos[2];
	LTIntPt		m_AirSize[2];
	LTIntPt     m_AirTextOffset;
	LTPoly_GT4	m_AirPoly[2];
	HTEXTURE	m_AirIcon[2];	

	//  [04/02/2004] GameHi
//--------------------------------------------------------------------------
	enum EN_DAMAGED_HUD
	{
		EDH_HP = 0,
		EDH_AP,
		EDH_AIR,
	};

	void		RenderDamaged( HTEXTURE hTexture, LTPoly_GT4* pPoly, EN_DAMAGED_HUD edhType );
	uint32		m_nLastHP;
	uint32		m_nLastAP;
	uint32		m_nLastAir;
	LTBOOL		m_bFirstUpdate;
	LTBOOL		m_bHPDamaged;
	LTBOOL		m_bAPDamaged;
	LTBOOL		m_bAirDamaged;
	LTFLOAT		m_fLastHPNotifiedTime;
	LTFLOAT		m_fLastAPNotifiedTime;
	LTFLOAT		m_fLastAirNotifiedTime;
};


#endif