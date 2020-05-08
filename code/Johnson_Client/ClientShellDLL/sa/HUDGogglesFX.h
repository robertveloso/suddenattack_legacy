#ifndef __MURSUM_HUD_GOGGLES_H__
#define __MURSUM_HUD_GOGGLES_H__

#include "HUDItem.h"

#define MAX_GOGGLES_NOISE_NUM 3

class CHUDGogglesFX : public CHUDItem
{
public:
	CHUDGogglesFX();

    LTBOOL      Init();
	void		Term();

	void		SetVisible( bool bVisible );
	bool		IsVisible();
    void        Render();
    void        Update();

	void		ScreenDimChanged();

private:
	void		SetGammaRamp( float fR, float fG, float fB );
	
	bool		m_bVisible;
	bool		m_bAdding;
	float		m_fAddVal;

	HTEXTURE	m_hNoise[MAX_GOGGLES_NOISE_NUM];
	LTPoly_GT4	m_PolyNoise[MAX_GOGGLES_NOISE_NUM];
	LTPoly_GT4	m_PolyAddAlpha;	

	LTFLOAT		m_fGammaR;
	LTFLOAT		m_fGammaG;
	LTFLOAT		m_fGammaB;

	int			m_nNoiseIndex;
	float		m_fNoiseUpdateTime;
};

#endif //#ifndef __MURSUM_HUD_GOGGLES_H__
