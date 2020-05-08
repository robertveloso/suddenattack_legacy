#ifndef __MURSUM_HUD_CAMERABLOODFX_H__
#define __MURSUM_HUD_CAMERABLOODFX_H__

#define MAX_NORMAL_BLOOD_TEX	5
#define MAX_NORMAL_BLOOD_POLY	6
#define CAMERA_BLOOD_SHOW_TIME	3.0f

#define DIR_LEFT	0
#define DIR_RIGHT	1
#define DIR_FRONT	2
#define DIR_BACK	3

#include "HUDItem.h"

typedef struct _tagCAMERABLOODFX
{
	HTEXTURE	hTex;
	LTPoly_GT4	hPoly;
} T_CAMERA_BLOOD_FX, *T_CAMERA_BLOOD_FXptr;

class CHUDCameraBloodFX : public CHUDItem
{
public:
	CHUDCameraBloodFX();
	~CHUDCameraBloodFX();

    LTBOOL      Init();
	void		Term();

	void		SetVisible( bool bVisible );
	bool		IsVisible();
    void        Render();
    void        Update();

	void		Show( bool bHeadShot, int nDir = DIR_LEFT );
	void		Hide();
	void		ScreenDimChanged();

private:
	bool		m_bVisible;	

	int32		m_nAlpha;
	float		m_fStartTime;
	float		m_fLastUpdate;
	int			m_nDrawCount;
	
	float		m_fBloodDeltaX;
	float		m_fBloodPosX;
	float		m_fBloodPosY;
	float		m_fHorzDir;

	float		m_fBloodTexW;
	float		m_fBloodTexH;
	
	HTEXTURE	m_hNormalBlood[MAX_NORMAL_BLOOD_TEX];

	T_CAMERA_BLOOD_FX	m_BloodData[MAX_NORMAL_BLOOD_POLY];
};

#endif //#ifndef __MURSUM_HUD_GAMEOVER_H__
