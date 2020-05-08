//[SPIKE] 
#ifndef __HUD_MISSIONTEXT_H
#define __HUD_MISSIONTEXT_H

#include "ButeMgr.h"
#include "PopupScreenOption.h"

class CHUDSensiCtrl : public CHUDItem
{
public:
	CHUDSensiCtrl();
	virtual ~CHUDSensiCtrl();

    virtual LTBOOL      Init();
	virtual void		Term();

    virtual void        Render();
	virtual void		Update();

	VOID				ScreenDimChanged();

	VOID				Show(LTBOOL bShow){m_bVisible = bShow;}
	
	LTBOOL				IsVisible() {return m_bVisible;}
				
	VOID				CalculateSensitivity(LTBOOL bLow_Or_High); // True == Low / False == High

protected:
	LTBOOL				m_bVisible;			
	CSAD3DTextEx*		m_pSensitivityText;
	float				m_fSensiDurationTime;
	float				m_fSensiTextAlpha;
	CButeMgr			m_buteMgr;
};

#endif