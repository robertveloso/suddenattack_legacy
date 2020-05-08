// ----------------------------------------------------------------------- //
//
// MODULE  : SAHUDMgr.h
//
// PURPOSE : Definition of CSAHUDMgr class
//
// CREATED : 07/17/01
//
// (c) 2001 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#ifndef __SA_HUDMGR_H
#define __SA_HUDMGR_H

#include "HUDMgr.h"
#include "HUDAmmo.h"
#include "HUDCrosshair.h"
#include "HUDCompass.h"
#include "HUDTimer.h"
#include "HUDHealth.h"
#include "HUDDamageDir.h"
#include "HUDRadio.h"
#include "HUDProgressBar.h"
#include "HUDRespawn.h"
#include "HUDGameOver.h"
#include "HUDCameraBloodFX.h"
#include "HUDChooser.h"
#include "HUDGogglesFX.h"
#include "HUDFlashBangFX.h"
#include "HUDPlayerState.h"
#include "HUDGameState.h"
#include "HUDSensiCtrl.h"
#include "HUDMiniMap.h"


//******************************************************************************************
//** HUD Manager
//******************************************************************************************
class CSAHUDMgr : public CHUDMgr
{
public:

	CSAHUDMgr();
	~CSAHUDMgr();

    virtual	LTBOOL		Init();
	virtual void		WaitRestart();

protected:

	CHUDAmmo			m_Ammo;
	CHUDCrosshair		m_Crosshair;
	CHUDCompass			m_Compass;
	CHUDTimer			m_Timer;
	CHUDTimeBombTimer	m_TimeBombTimer;
	CHUDHealth			m_Health;
	CHUDDamageDir		m_DamageDir;
	CHUDRadio			m_Radio;
	CHUDProgressBar		m_ProgressBar;
	CHUDRespawn			m_Respawn;
	CHUDGameOver		m_GameOver;
	CHUDWpnChooser		m_WpnChooser;
	CHUDFlashBangFX		m_HUDFlashBangFX;
	CHUDSensiCtrl		m_HUDSensiCtrl;
	CHUDPlayerState		m_HUDPlayerState;
	CHUDGameState		m_HUDGameState;
	CHUDMiniMap			m_HUDMiniMap;

//	CHUDCameraBloodFX	m_CameraBloodFX;
//	CHUDGogglesFX		m_HUDGogglesFX;
};


extern CHUDCrosshair*		g_pCrosshair;
extern CHUDCompass*			g_pCompass;
extern CHUDTimer*			g_pHUDTimer;
extern CHUDTimeBombTimer*	g_pHUDTimeBombTimer;
extern CHUDRadio*			g_pRadio;
extern CHUDGameOver*		g_pHUDGameOver;
extern CHUDWpnChooser*		g_pWpnChooser;
extern CHUDFlashBangFX*		g_pHUDFlashBangFX;
extern CHUDSensiCtrl*		g_pHUDSensiCtrl;
extern CHUDPlayerState*		g_pHUDPlayerState;
extern CHUDGameState*		g_pHUDGameState;
extern CHUDAmmo*			g_pHUDAmmo;
extern CHUDMiniMap*			g_pHUDMiniMap;

//extern CHUDCameraBloodFX*	g_pCameraBloodFX;
//extern CHUDGogglesFX*		g_pHUDGogglesFX;

#endif
