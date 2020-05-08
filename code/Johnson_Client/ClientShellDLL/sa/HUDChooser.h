#ifndef __HUD_CHOOSER_H
#define __HUD_CHOOSER_H

#include "HUDItem.h"

#define MAX_WEAPON_COUNT 10

typedef struct _tagWEAPONCHOOSER
{
	HTEXTURE	hTexture;
	int			nWeaponID;

	uint32		nTexW;
	uint32		nTexH;	
	float		fTexMinW;
	float		fTexMinH;
	float		fTexMaxW;
	float		fTexMaxH;
	float		fTexRealW;
	float		fTexRealH;
	float		fTexWHRatio;

	LTPoly_GT4	hPoly;
	LTBOOL		bIncrease;
	LTBOOL		bDecrease;
} T_WPN_CHOOSER, *T_WPN_CHOOSERptr;

class CHUDWpnChooser : public CHUDItem
{
public:
	CHUDWpnChooser();

    LTBOOL      Init();
	void		Term();

	void		AddWeapon( int nClass, int nWeaponID, LTBOOL bSelected=LTTRUE, LTBOOL bChangePos=LTTRUE );
	void		RemoveWeapon( int nClass );
	void		RemoveAllWeapon();

	void		SelectFromClass( int nClass );
	void		SelectFromID( int nWeaponID );
	void		LastWeapon();
	void		SetIconPos();

    void        Render();
    void        Update();

	void		ScreenDimChanged();
	
	void		ClearLastWeapon();

private:
    LTIntPt			m_BasePos;

	T_WPN_CHOOSER	m_WpnInfo[MAX_WEAPON_COUNT];
	
	int				m_nIconInterval;
	int				m_nWeaponCount;
	int				m_nCurrentWeapon;
	LTBOOL			m_bChanging;

	int				m_nTotalWidth;
	int				m_nLastWeaponID;
	
	LTFLOAT			m_fShowTime;
};

#endif