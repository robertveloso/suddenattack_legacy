#ifndef __MURSUM_HUD_FLASHBANG_H__
#define __MURSUM_HUD_FLASHBANG_H__

#include "HUDItem.h"

class CHUDFlashBangFX : public CHUDItem
{
public:
	CHUDFlashBangFX();

    LTBOOL      Init();
	void		Term();

	void		InitVar();
	void		SetVisible( bool bVisible, float fBlindTime = 0.0f );
	bool		IsVisible();
    void        Render();
    void        Update();

	void		ScreenDimChanged();

private:
	void		DeleteScreenBackup();
	void		ApplySoundVolume( float fPersent, bool bInc=false );

	bool		m_bVisible;
	bool		m_bIncrease;
	bool		m_bDecrease;
	float		m_fAddVal;
	float		m_fBlindTime;
	float		m_fSurfaceAlpha;

	LTPoly_GT4	m_PolyAddAlpha;	

	float		m_fSurfaceAlphaDelay;
	HSURFACE	m_hScreenBackup;
	HLTSOUND	m_hLoopSound;

	float		m_fWeaponsSndMul;
	float		m_fSpeechSndMul;
	float		m_fDefaultSndMul;
	float		m_fMursumSndMul;
};

#endif //#ifndef __MURSUM_HUD_FLASHBANG_H__
